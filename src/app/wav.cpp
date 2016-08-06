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
#include "wav.hpp"
#include "app.hpp"
#include "url.hpp"	// abs


int Wav::toolid = VLC_TOOL;

// local
static pid_t pid = -1;


void Wav::init(int _toolid)
{
  toolid = _toolid;
}

void Wav::launch(const char *tool, const char *url, bool loop)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    switch (toolid) {
    case VLC_TOOL:
#if MACOSX
      execlp(tool, tool, "--hide", "-a", VLC_PATH, url, "--args", "--play-and-exit", (char*)NULL);
#else
      if (loop) execlp(tool, tool, "--quiet", "--repeat", url, (char*)NULL);
      else      execlp(tool, tool, "--quiet", url, "vlc:quit", (char*)NULL);
#endif
      break;
    }
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
#if MACOSX
    case QUICKTIME_TOOL:
      execlp(tool, tool, "-a", QUICKTIME_PATH, url, (char*)NULL);
      break;
#endif
  default:
    break;
  }
}

void Wav::start(const char *_url, bool loop)
{
  char url[URL_LEN];
  Url::abs(_url, url);

  switch (toolid) {
#if MACOSX
  case QUICKTIME_TOOL: launch("open", url, loop); break;
  case VLC_TOOL:       launch("open", url, loop); break;
#else
  case VLC_TOOL:       launch("vlc", url, loop); break;
#endif
  default:             error("%s wav", e_tool);
  }
}

void Wav::stop()
{
  if (pid > 0) kill(pid, SIGKILL);
}

void Wav::pause()
{
  if (pid > 0) kill(pid, SIGSTOP);
}

void Wav::cont()
{
  if (pid > 0) kill(pid, SIGCONT);
}
