//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// report.cpp
//
// Reporting operation
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "report.hpp"
#include "env.hpp"


/*
 * Sends a report to the maintainer
 */
int report(const char *mess)
{
  FILE *pp;
  char cmd[BUFSIZ];
  char hostname[MAXHOSTNAMELEN];
  gethostname(hostname, sizeof(hostname)-1);

  sprintf(cmd, "echo \"%s\" | /usr/bin/mailx -s \"[vreng] (%s@%s - %s-%s)\" %s",
    mess, ::g.user, hostname, ::g.env.sysname(), ::g.env.relname(), PACKAGE_BUGREPORT);

  if (! (pp = popen(cmd, "w"))) {
    perror("pipe mail");
    return 0;
  }
  fflush(pp);
  pclose(pp);
  //echo("%s", cmd);
  return 1;
}
