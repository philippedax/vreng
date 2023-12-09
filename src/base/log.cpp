//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2022 Philippe Dax
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
#include "vreng.hpp"
#include "gui.hpp" 	// writeMessage
#include "env.hpp" 	// dir
#include "pref.hpp" 	// ::g.pref
#include "file.hpp" 	// open, close
#include <iostream>

const char *e_fork = "can't fork for";
const char *e_exec = "can't exec";
const char *e_tool = "unknown tool";
const char *e_not_implemented = "not implemented";
const char *e_bad_type = "bad type";

static bool infatal = false;


void fatal(const char *s, ...)
{
  va_list ap;

  va_start(ap, s);
  vfprintf(stderr, s, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  if (! infatal) {
    infatal = true;
    Vreng::quit(-1);
  }
}

void warning(const char *s, ...)
{
  va_list ap;
  char buf[256];

  va_start(ap, s);
  vfprintf(stderr, s, ap);
  va_end(ap);
  fprintf(stderr, "\n");

  va_start(ap, s);
  vsprintf(buf, s, ap);
  ::g.gui.writeMessage("warning", NULL, buf);
  va_end(ap);
}

void notice(const char *s, ...)
{
  va_list ap;
  char buf[256];

  va_start(ap, s);
  vsprintf(buf, s, ap);
  va_end(ap);
  
  ::g.gui.writeMessage("notice", NULL, buf);
}

void trace(int dbgmask, const char *s, ...)
{
  va_list ap;

  if (::g.pref.debug & (1 << dbgmask) || dbgmask == DBG_FORCE) {
    va_start(ap, s);
    vfprintf(stderr, s, ap);
    va_end(ap);
    fprintf(stderr, "\n");
  }
}

void trace2(int dbgmask, const char *s, ...)	// without cr
{
  va_list ap;

  if (::g.pref.debug & (1 << dbgmask) || dbgmask == DBG_FORCE) {
    va_start(ap, s);
    vfprintf(stderr, s, ap);
    va_end(ap);
  }
}

void error(const char *s, ...)
{
  va_list ap;

  va_start(ap, s);
  vfprintf(stderr, s, ap);
  va_end(ap);
  fprintf(stderr, "\n");
}

void echo(const char *s, ...)
{
  va_list ap;

  va_start(ap, s);
  vfprintf(stdout, s, ap);
  va_end(ap);
  fprintf(stdout, "\n");
}

FILE * writelog(const char *s, ...)
{
  static FILE *fl = NULL;
  va_list ap;
  char logfile[PATH_LEN];
  File *file = new File();

  if (fl == NULL) {
    sprintf(logfile, "%s/log", ::g.env.dir());
    unlink(logfile);
    if ((fl = file->open(logfile, "w")) == NULL) {
      perror("open log");
      return (FILE *) NULL;
    }
  }
  va_start(ap, s);
  vfprintf(fl, s, ap);
  fprintf(fl, "\n");
  va_end(ap);
  return fl;
}

void printlog()
{
  FILE *fl;
  char logfile[PATH_LEN];
  char buf[256];

  File *file = new File();
  sprintf(logfile, "%s/log", ::g.env.dir());
  if ((fl = file->open(logfile, "r")) == NULL) {
    perror("open log");
    return;
  }
  while (fgets(buf, sizeof(buf), fl) != NULL) {
    fputs(buf, stderr);
  }
  file->close();
  delete file;
}

void progression(char c)
{
  if (::g.pref.progress) {
    fprintf(stderr, "%c", c);
    //::g.gui.writeMessage("progress", NULL, &c);
  }
}

void endprogression()
{
  if (::g.pref.progress) {
    fprintf(stderr, "\n");
    //::g.gui.writeMessage("progress", NULL, "\n");
  }
}

void fopenlog(FILE *f, const char *name)
{
  fprintf(stderr, "open: %s\n", name);
}

void fcloselog(FILE *f)
{
  //file->close();
}

#if 0 //notused
void closelog(FILE *fl)
{
  //file->close();
}
#endif //notused
