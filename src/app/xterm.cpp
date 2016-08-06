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
#include "xterm.hpp"
#include "app.hpp"


int Xterm::toolid = SSH_TOOL;

static pid_t pid = -1;


void Xterm::init(int _toolid)
{
  toolid = _toolid;
}

void Xterm::start(const char *host)
{
  switch (pid = fork()) {
  case -1:
    error("%s xterm", e_fork);
    break;
  case 0:
    if (toolid == SSH_TOOL)
      execlp("xterm", "xterm", "-title", host, "-e", "ssh", host, (char*)NULL);
    if (toolid == TELNET_TOOL)
      execlp("xterm", "xterm", "-title", host, "-e", "telnet", host, (char*)NULL);
    error("%s xterm", e_exec);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Xterm::quit()
{
  if (pid > 0) kill(pid, SIGKILL);
}
