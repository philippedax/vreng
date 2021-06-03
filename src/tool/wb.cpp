//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#include "wb.hpp"
#include "tool.hpp"
#include "channel.hpp"	// getGroup getPort


int Wb::toolid = WB_TOOL;

static pid_t pid = -1;


void Wb::init(int _toolid)
{
  toolid = _toolid;
}

void Wb::getnewchan(const char *chan, char *newchan)
{
  char group[GROUP_LEN];

  uint16_t port = Channel::getPort(chan);
  port += WHITEBOARD_INCR_PORT;
  Channel::getGroup(chan, group);
  sprintf(newchan, "%s/%u/%d", group, port, Channel::currentTtl());
}

void Wb::start(const char *chan)
{
  char *p, *ttl;
  char newchan[CHAN_LEN];

  getnewchan(chan, newchan);
  p = strrchr(newchan, '/');
  *p = '\0';	// addr/port
  ttl = ++p;

#if defined(WIN32) && !defined(CYGWIN32) // _spawn
  if (toolid == WB_TOOL) {
    pid = _spawnlp(_P_NOWAIT, "wb", "wb", "-C", "VREng-wb", "-t", ttl, "-l", "+r", newchan, NULL);
  }
  if (toolid == WBD_TOOL) {
    pid = _spawnlp(_P_NOWAIT, "wbd", "wbd", "-C", "VREng-wbd", "-t", ttl, "-l", "+r", newchan, NULL);
  }
  if (toolid == NTE_TOOL) {
    pid = _spawnlp(_P_NOWAIT, "nte", "nte", "-C", "VREng-nte", "-t", ttl, newchan, NULL);
  }
#else
  switch (pid = fork()) {
  case -1:
    error("%s whiteboard", e_fork);
    break;
  case 0:
    if (toolid == WB_TOOL) {
      execlp("wb", "wb", "-C", "VREng-wb", "-t", ttl, "-l", "+r", newchan, (char*)NULL);
      error("%s wb", e_exec);
    }
    if (toolid == WBD_TOOL) {
      execlp("wbd", "wbd", "-C", "VREng-wbd", "-t", ttl, "-l", "+r", newchan, (char*)NULL);
      error("%s wbd", e_exec);
    }
    if (toolid == NTE_TOOL) {
      execlp("nte", "nte", "-C", "VREng-nte", "-t", ttl, newchan, (char*)NULL);
      error("%s nte", e_exec);
    }
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
#endif
}

void Wb::quit()
{
#ifndef WIN32 // can't kill under windows
  if (pid > 0) kill(pid, SIGKILL);
#endif // !WIN32
}
