//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "entry.hpp"
#include "user.hpp"	// User


const OClass Entry::oclass(ENTRY_TYPE, "Entry", Entry::creator);

// class member initialization
Entry * Entry::entry = (Entry *) NULL;

void Entry::funcs() {}


/* creation from a file */
WObject * Entry::creator(char *l)
{
  return new Entry(l);
}

void Entry::parser(char *l)
{
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Entry::Entry()
{
  //dax entry = NULL;
}

Entry::Entry(char *l)
{
  parser(l);
  initObject(INVISIBLE);
  entry = this;
}

/* set by gate */
Entry::Entry(float *newpos)
{
  pos.x = newpos[0];
  pos.y = newpos[1];
  pos.z = newpos[2];
  entry = this;
}

Entry * Entry::current()
{
  return entry;
}

/* queried by user */
void Entry::query(User *user)
{
  if (user) {
    user->pos.x = pos.x;
    user->pos.y = pos.y;
    user->pos.z = pos.z + user->height /2 + 0.15;
    user->pos.az = pos.az;
    user->pos.ax = pos.ax;
  }
}

void Entry::quit()
{
  entry = (Entry *) NULL;
}
