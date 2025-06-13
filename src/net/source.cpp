//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
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
// source.cpp
//
// Source handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "source.hpp"
#include "session.hpp"	// Session
#include "channel.hpp"	// Channel
#include "stat.hpp"	// new_source


// local
static uint16_t sourceCounter;		// source counter


/**
 * Creates a new Source
 */
Source::Source(uint32_t _ssrc)
{
  new_source++;
  ssrc = _ssrc;
  extended_max = 0;	// payload
  expected = 0;		// payload
  lost = 0;
  sdes.si_type = 0;
  sdes.si_len = 0;
  sdes.si_str = NULL;
  sdes.si_next = NULL;
  next = NULL;
}

/**
 * Finds source by its ssrc else creates a new source
 */
Source * Source::getSource(uint32_t _ssrc)
{
  Channel *pchan = Channel::current(); // maybe indeterminated...
  if (! pchan->session->source) return NULL;

  Source *pso, *psolast;
  int i = 0;

  for (pso = psolast = pchan->session->source;
       pso && (i < sourceCounter);
       pso = pso->next, i++) {
    if (pso->ssrc == _ssrc) return pso;	// registered source
    psolast = pso;
  }
  // not yet registered source
  if (! pso) {
    // no source found, we must create it now
    pchan->session->source = new Source(_ssrc);
    pchan->session->incrSources();
    //trace1(DBG_RTP, "getSource: create source=%p", pchan->session->source);
    return pchan->session->source;
  }
  else {
    //error("bad new source");
    // new incoming source: create this source
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

uint16_t Source::getNumber()
{
  return sourceCounter;
}

uint16_t Source::incrNumber()
{
  return ++sourceCounter;
}

uint16_t Source::decrNumber()
{
  return --sourceCounter;
}
