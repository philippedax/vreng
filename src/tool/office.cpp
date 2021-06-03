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
#include "office.hpp"
#include "tool.hpp"
#include "cache.hpp"	// download


int Office::toolid = MSOFFICE_TOOL;

// local
static pid_t pid = -1;


void Office::init(int _toolid)
{
  toolid = _toolid;
}

void Office::launch(const char *tool, const char *file)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
#if MACOSX
    execlp("open", "open", file, (char*)NULL);
#else
    execlp(tool, tool, file, (char*)NULL);
#endif
    error("%s %s", tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Office::start(const char *url)
{
  char *file = new char[PATH_LEN];

  if (Cache::download(url, file) != 0) {
    if (toolid == MSOFFICE_TOOL)
      launch("office", file);
    else if (toolid == OPENOFFICE_TOOL)
      launch("ooffice", file);
    else if (toolid == LIBROFFICE_TOOL)
      launch("ooffice", file);
    else if (toolid == STAROFFICE_TOOL)
      launch("soffice", file);
    else
      error("%s office", e_tool);
  }
}

void Office::quit()
{
  if (pid > 0) {
    kill(pid, SIGKILL);
    pid = -1;
  }
}
