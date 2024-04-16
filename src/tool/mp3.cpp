//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// mp3.cpp
//
// Plays audio MP3 files
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "mp3.hpp"
#include "tool.hpp"
#include "url.hpp"	// abs
#include "cache.hpp"	// download
#include "pref.hpp"	// silent


#if MACOSX
int Mp3::toolid = QUICKTIME_TOOL;
#else
int Mp3::toolid = MPG123_TOOL;
#endif

// local
static pid_t pid = -1;


void Mp3::init(int _toolid)
{
  toolid = _toolid;
}

void Mp3::launch(const char *tool, const char *url)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    switch (toolid) {
    case MPG123_TOOL:    execlp(tool, tool, "-qy", url, NULL); break;
    case XAUDIO_TOOL:    execlp(tool, tool, url, NULL); break;
    case FREEAMP_TOOL:   execlp(tool, tool, url, NULL); break;
#if MACOSX
    case QUICKTIME_TOOL: execlp(tool, tool, "-a", QUICKTIME_PATH, url, NULL); break;
    case VLC_TOOL:       execlp(tool, tool, "--hide", "-a", VLC_PATH, url, "--args", "--play-and-exit", NULL); break;
#else
    case VLC_TOOL:       execlp(tool, tool, "--quiet", url, NULL); break;
#endif
    }
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Mp3::start(const char *_url)
{
  if (::g.pref.silent)  return;

  char url[URL_LEN];
  Url::abs(_url, url);

  switch (toolid) {
  case MPG123_TOOL:    launch("mpg123", url); break;
  case XAUDIO_TOOL:    launch("xaudio", url); break;
  case FREEAMP_TOOL:   launch("freeamp", url); break;
#if MACOSX
  case QUICKTIME_TOOL: launch("open", url); break;
  case VLC_TOOL:       launch("open", url); break;
#else
  case VLC_TOOL:       launch("vlc", url); break;
#endif
  default:             error("%s mp3", e_tool);
  }
}

void Mp3::stop()
{
  if (pid > 0) kill(pid, SIGKILL);
}

void Mp3::pause()
{
  if (pid > 0) kill(pid, SIGSTOP);
}

void Mp3::cont()
{
  if (pid > 0) kill(pid, SIGCONT);
}
