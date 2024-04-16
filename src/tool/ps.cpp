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
// ps.cpp
//
// Plays Postscript files (ghostview, gv,...)
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "ps.hpp"
#include "tool.hpp"
#include "cache.hpp"	// download


int Ps::toolid = GV_TOOL;

// local
static pid_t pid = -1;


void Ps::init(int _toolid)
{
  toolid = _toolid;
}

void Ps::launch(const char *tool, const char *file)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    execlp(tool, tool, file, NULL);
#if MACOSX
    execlp("open", "open", "-a", PREVIEW_PATH, file, NULL);
#endif
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Ps::view(const char *url)
{
  char *file = new char[PATH_LEN];

  if (Cache::download(url, file) != 0) {
    if (toolid == GV_TOOL)
      launch("gv", file);
    else if (toolid == GHOSTVIEW_TOOL)
      launch("ghostscript", file);
    else {
      error("%s ps, toolid=%x", e_tool, toolid);
      launch("preview", file);
    }
  }
  delete[] file;
}

void Ps::launcha2ps(const char *tool, const char *file)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    execlp("a2ps", tool, "-d", file, NULL);
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Ps::print(const char *url)
{
  char *file = new char[PATH_LEN];

  if (Cache::download(url, file)) launcha2ps("a2ps", file);
  delete[] file;
}

void Ps::quit()
{
  if (pid > 0) kill(pid, SIGKILL);
}
