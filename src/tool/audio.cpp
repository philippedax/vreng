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
#include "audio.hpp"
#include "tool.hpp"
#include "channel.hpp"	// getGroup getPort
#include "pref.hpp"	// ::g.pref.silent


#define DEVAUDIO	"/dev/audio"
#define DEVAUDIOCTL	"/dev/audioctl"


int Audio::toolid = RAT_TOOL;

// global
int audioactive = false;

// local
static pid_t pid = -1;


void Audio::init(int _toolid)
{
  toolid = _toolid;
}

void Audio::getnewchan(const char *chan, char *newchan)
{
  char group[GROUP_LEN];

  uint16_t port = Channel::getPort(chan);
  port += AUDIO_INCR_PORT;
  if (port % 1) port++;	// even port for RTP
  Channel::getGroup(chan, group);
  sprintf(newchan, "%s/%u/%d", group, port, Channel::currentTtl());
}

void Audio::start(const char *chan)
{
  if (::g.pref.silent)  return;

  char *p, newchan[CHAN_LEN];
  char ttl[4];

  getnewchan(chan, newchan);
  sprintf(ttl, "%d", Channel::currentTtl());
  p = strrchr(newchan, '/');
  *p = '\0';	// addr//port

#if defined(WIN32) && !defined(CYGWIN32) // _spawn
  if (toolid == RAT_TOOL) {
    pid = _spawnlp(_P_NOWAIT, "rat", "rat", "-t", ttl,
                       "-f", "pcm", "-C", "VREng", newchan, NULL);
  }
  if (toolid == VAT_TOOL) {
    pid = _spawnlp(_P_NOWAIT, "vat", "vat", "-aAsM", "-t", ttl,
                       "-C", "VREng", newchan, NULL);
  }
  if (toolid == FPHONE_TOOL) {
    pid = _spawnlp(_P_NOWAIT, "fphone", "fphone", "-t", ttl,
                       newchan, NULL);
  }
#else
  switch (pid = fork()) {
  case -1:
    error("%s audio", e_fork);
    break;
  case 0:
    if (toolid == RAT_TOOL) {
      execlp("rat", "rat", "-f", "pcm", "-C", "VREng", "-t", ttl, newchan, (void*)NULL);
      error("%s rat", e_exec);
    }
    if (toolid == VAT_TOOL) {
      execlp("vat", "vat", "-aAsM", "-C", "VREng", "-t", ttl, newchan, (void*)NULL);
      error("%s vat", e_exec);
    }
    if (toolid == FPHONE_TOOL) {
      execlp("fphone", "fphone", "/T", ttl, newchan, (void*)NULL);
      error("%s fphone", e_exec);
    }
    signal(SIGCHLD, SIG_IGN);
    exit(1);  // child death
  default:
    break;
  }
#endif
}

void Audio::quit()
{
#if WIN32 /* how to kill a process under Windows ? */
  if (pid > 0) kill(pid, SIGKILL);
#endif // !WIN32
}

#if 0 // TODO
int last_volume = -1;
int getAudioVolumeMin() { return AUDIO_MIN_GAIN; }
int getAudioVolumeMax() { return AUDIO_MAX_GAIN; }

int Audio::getAudioVolume(int fdaudio)
{
  if (last_volume < 0) {
    audio_info_t ainfo;
    ioctl(fdaudio, AUDIO_GETINFO, &ainfo);
    last_volume = ainfo.play.gain;
  }
  return last_volume;
}

void Audio::setAudioVolume(int fdaudio, int v)
{
  last_volume = v;

  if (fdaudio >= 0) {
    audio_info_t ainfo;

    AUDIO_INITINFO(&ainfo);
    ainfo.play.gain = v;
    ioctl(fdaudio, AUDIO_SETINFO, &ainfo);
  }
}
#endif
