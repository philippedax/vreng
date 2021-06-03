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
#include "mpeg.hpp"
#include "tool.hpp"
#include "url.hpp"	// abs
#include "cache.hpp"	// download


#if MACOSX
int Mpeg::toolid = QUICKTIME_TOOL;
#else
int Mpeg::toolid = MTVP_TOOL;
#endif

// local
static pid_t pid = -1;


void Mpeg::init(int _toolid)
{
  toolid = _toolid;
}

void Mpeg::launch(const char *tool, const char *url, char *file)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    switch (toolid) {
    case MPEGPLAY_TOOL:  execlp(tool, tool, file, (char*)NULL); break;
    case MTVP_TOOL:      execlp(tool, tool, url, (char*)NULL); break;
#if MACOSX
    case QUICKTIME_TOOL: execlp(tool, tool, "-a", QUICKTIME_PATH, url, (char*)NULL); break;
    case VLC_TOOL:       execlp(tool, tool, "--hide", "-a", VLC_PATH, url, "--args", "--play-and-exit", (char*)NULL); break;
#else
    case VLC_TOOL:       execlp(tool, tool, url, (char*)NULL); break;
#endif
    }
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Mpeg::start(const char *_url)
{
  char url[URL_LEN];
  char *file = new char[PATH_LEN];

  Url::abs(_url, url);

  switch (toolid) {
  case MPEGPLAY_TOOL:  if (Cache::download(url, file)) launch("mpeg_play", url, file); break;
  case MTVP_TOOL:      launch("mtvp", url, file); break;
#if MACOSX
  case QUICKTIME_TOOL: launch("open", url, NULL); break;
  case VLC_TOOL:       launch("open", url, NULL); break;
#else
  case VLC_TOOL:       launch("vlc", url, NULL); break;
#endif
  default:             error("%s mpeg", e_tool);
  }
  delete[] file;
}

void Mpeg::stop()
{
  if (pid > 0) kill(pid, SIGKILL);
}

void Mpeg::pause()
{
  if (pid > 0) kill(pid, SIGSTOP);
}

void Mpeg::cont()
{
  if (pid > 0) kill(pid, SIGCONT);
}
