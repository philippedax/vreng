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
#include "modeler.hpp"
#include "tool.hpp"


int Modeler::toolid = VRED_TOOL;

// local
static pid_t pid = -1;


void Modeler::init(int _toolid)
{
  toolid = _toolid;
}

void Modeler::start()
{
#if defined(WIN32) && !defined(CYGWIN32) // _spawn
  if (toolid == VRED_TOOL)
    pid = _spawnlp(_P_NOWAIT, "vred", "vred", NULL);
  if (toolid == VREM_TOOL)
    pid = _spawnlp(_P_NOWAIT, "vrem", "vrem", NULL);
#else
  switch (pid = fork()) {
  case -1:
    error("%s modeler", e_fork);
    break;
  case 0:
    if (toolid == VRED_TOOL) {
      execlp("./bin/vred", "vred", NULL);
      error("%s vred", e_exec);
    }
    if (toolid == VREM_TOOL) {
      execlp("vrem", "vrem", NULL);
      error("%s vrem", e_exec);
    }
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
#endif
}

void Modeler::quit()
{
#ifndef WIN32 // can't kill under windows
  if (pid > 0) kill(pid, SIGKILL);
#endif // !WIN32
}
