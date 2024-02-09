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
#include "au.hpp"
#include "tool.hpp"
#include "url.hpp"	// abs
#include "cache.hpp"	// download


#if MACOSX
int Au::toolid = QUICKTIME_TOOL;  // default tool
#else
int Au::toolid = PLAY_TOOL;  // default tool
#endif

// local
static pid_t pid = -1;


void Au::init(int _toolid)
{
  toolid = _toolid;
}

void Au::launch(const char *tool, const char *url, char *file)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    switch (toolid) {
    case PLAY_TOOL:      execlp(tool, tool, file, NULL); break;
#if MACOSX
    case QUICKTIME_TOOL: execlp(tool, tool, QUICKTIME_PATH, url, NULL); break;
#endif
    }
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Au::start(const char *_url)
{
  char url[URL_LEN];
  char *file = new char[PATH_LEN];

  Url::abs(_url, url);

  switch (toolid) {
  case PLAY_TOOL:      if (Cache::download(url, file)) launch("play", url, file); break;
  case QUICKTIME_TOOL: launch("open", url, file); break;
  default:             error("%s au", e_tool);
  }
  delete[] file;
}

void Au::stop()
{
  if (pid > 0) kill(pid, SIGKILL);
}

void Au::pause()
{
  if (pid > 0) kill(pid, SIGSTOP);
}

void Au::cont()
{
  if (pid > 0) kill(pid, SIGCONT);
}
