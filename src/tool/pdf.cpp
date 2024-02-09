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
/*
 * PDF tool
 */

#include "vreng.hpp"
#include "pdf.hpp"
#include "tool.hpp"
#include "cache.hpp"	// download


int Pdf::toolid = ACROBAT_TOOL;

// local
static pid_t pid = -1;


void Pdf::init(int _toolid)
{
  toolid = _toolid;
}

void Pdf::launch(const char *tool, const char *file)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
#if MACOSX
    execlp("open", "open", file, NULL);
    execlp("open", "open", "-a", PREVIEW_PATH, file, NULL);
#else
    execlp(tool, tool, file, NULL);
#endif
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Pdf::start(const char *url)
{
  char *file = new char[PATH_LEN];

  if (Cache::download(url, file)) {
    if (toolid == ACROBAT_TOOL)
      launch("acroread", file);
    else if (toolid == XPDF_TOOL)
      launch("xpdf", file);
    else if (toolid == EVINCE_TOOL)
      launch("evince", file);
    else {
      error("%s pdf, toolid=%x", e_tool, toolid);
      launch("preview", file);
    }
  }
}

void Pdf::quit()
{
  if (pid > 0) {
    kill(pid, SIGKILL);
    pid = -1;
  }
}
