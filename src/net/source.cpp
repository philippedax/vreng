//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "source.hpp"
#include "session.hpp"	// Session
#include "channel.hpp"	// Channel
#include "prof.hpp"	// new_source


// local
static uint16_t sourcesCounter;		// sources counter


/*
 * Handling Network Sources
 */

/**
 * creates a new Source
 */
Source::Source(uint32_t _ssrc)
{
  new_source++;
  ssrc = _ssrc;
  extended_max = 0;
  expected = 0;
  lost = 0;
  sdes.si_type = 0;
  sdes.si_len = 0;
  sdes.si_str = NULL;
  sdes.si_next = NULL;
  next = NULL;
}

/**
 * finds source by its ssrc else creates a new source
 */
Source * Source::getSource(uint32_t _ssrc)
{
  Channel *pchan = Channel::current(); // maybe indeterminated...
  if (! pchan->session->source) return NULL;

  Source *pso, *psolast;
  int i = 0;

  for (pso = psolast = pchan->session->source;
       pso && (i < sourcesCounter);
       pso = pso->next, i++) {
    if (pso->ssrc == _ssrc) return pso;	// registered source
    psolast = pso;
  }
  /* not yet registered source */
  if (! pso) {
    /* no source found, we must create it now */
    pchan->session->source = new Source(_ssrc);

    pchan->session->incrSources();
    trace(DBG_RTP, "getSource: create source=%p", pchan->session->source);
    return pchan->session->source;
  }
  else {
    error("bad new source");
    /* new incoming source: create this source */
    psolast->next = new Source(_ssrc);

    pchan->session->incrSources();
    return psolast->next;
  }
}

/**
 * Source destructor
 */
Source::~Source()
{
  del_source++;
}

uint16_t Source::getSourcesNumber()
{
  return sourcesCounter;
}

uint16_t Source::incrSourcesNumber()
{
  return ++sourcesCounter;
}

uint16_t Source::decrSourcesNumber()
{
  return --sourcesCounter;
}

uint16_t Source::getMembersNumber()
{
  uint16_t members = 0;

  for (Session *pse = Session::getList(); pse && pse->mode; pse = pse->next) {
    for (Source *pso = pse->source; pso ; pso = pso->next) {
      members++;
    }
  }
  return members;
}

void Source::dump()
{
  // if (&sdes == NULL) return;  // BUG! segfault

  //echo("this=%p", this);
  echo("ssrc=%x", ssrc);
  echo("lost=%d", lost);
  //echo("next=%p", next);

  SdesItem *sitem;
  int i;
  for (i=0, sitem = &sdes; sitem ; sitem = sitem->si_next, i++) {
    if (sitem->si_type > RTCP_SDES_END && sitem->si_type <= RTCP_SDES_SOURCE && sitem->si_len >0 && sitem->si_len < 128 && sitem->si_str) {
      echo("  sdes[%d]=%s", i, sitem->si_str);
    }
  }
  echo("");
}

void Source::dumpAll()
{
  uint32_t * ssrctab = new uint32_t[100];

  for (Session *pse = Session::getList(); pse && pse->mode && pse->nbsources > 1; pse = pse->next) {
    int i = 0;
    for (Source *pso = pse->source; pso && i < 100; pso = pso->next, i++) {
      if (pso->ssrc == 0) {
        delete[] ssrctab;	// no more sources
        return;
      }
      int yet = 0;
      for (int j=0; ssrctab[j]; j++) {
        if (ssrctab[j] == pso->ssrc) {
          yet = 1;
          break;
        }
      }
      if (yet == 0) {
        ssrctab[i] = pso->ssrc;
        pso->dump();
      }
    }
  }
  delete[] ssrctab;
}
