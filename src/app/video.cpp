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
#include "video.hpp"
#include "app.hpp"
#include "channel.hpp"	// getGroup getPort


#ifndef VIDEODEVICE
#if SOLARIS
#define VIDEODEVICE	"rtvc"
#else
#define VIDEODEVICE	"video"
#endif
#endif


#if MACOSX || LINUX
int Video::toolid = VLCMC_TOOL;
#else
int Video::toolid = VIC_TOOL;
#endif

// local
static pid_t pid = -1;


void Video::init(int _toolid)
{
  toolid = _toolid;
}

void Video::getnewchan(const char *chan, char *newchan)
{
  char group[GROUP_LEN];

  uint16_t port = Channel::getPort(chan);
  port += VIDEO_INCR_PORT;
  if (port % 1) port++;	// even port for RTP
  Channel::getGroup(chan, group);
  sprintf(newchan, "%s/%u/%d", group, port, Channel::currentTtl());
}

void Video::start(const char *chan)
{
  char newchan[CHAN_LEN];
  getnewchan(chan, newchan);

#if defined(WIN32) && !defined(CYGWIN32) // _spawn
  pid = _spawnlp(_P_NOWAIT, "vic", "vic", "-A", "rtp", "-B", "20", "-D",
                 VIDEODEVICE, "-f", "h261", "-C", "VREng-vic", newchan, NULL);
#else
  switch (pid = fork()) {
  case -1:
    error("%s video", e_fork);
    break;
  case 0:
    switch (toolid) {
    case VIC_TOOL:
      execlp("vic", "vic", "-A", "rtp", "-B", "20", "-D", VIDEODEVICE,
           "-f", "h261", "-C", "VREng-vic", newchan, (char*)NULL);
      error("%s vic", e_exec);
      break;
    case VLCMC_TOOL:
      char group[GROUP_LEN];
      uint16_t port = Channel::getPort(chan);
      port += VIDEO_INCR_PORT;
      if (port % 1) port++;  // even port for RTP
      Channel::getGroup(chan, group);
      sprintf(newchan, "udp:@%s/%u", group, port);
      execlp("vlc", "vlc", "--sout-rtp-name", "VREng-vlc", "--sout-rtp-ttl", Channel::currentTtl(), newchan, (char*)NULL);
      error("%s vlc", e_exec);
      break;
    }
    signal(SIGCHLD, SIG_IGN);
    exit(1);  // child death
  default:
    break;  // parent
  }
#endif
}

void Video::quit()
{
#ifndef WIN32
  if (pid > 0) kill(pid, SIGKILL);
#endif
}
