//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "area.hpp"
#include "user.hpp"	// USER_TYPE


const OClass Area::oclass(AREA_TYPE, "Area", Area::creator);


/* creation from a file */
WObject * Area::creator(char *l)
{
  return new Area(l);
}

void Area::defaults()
{
  inside = false;
}

void Area::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Area::Area(char *l)
{
  parser(l);

  enableBehavior(COLLIDE_ONCE);
  enableBehavior(UNVISIBLE);
  initObject(INVISIBLE);
}

/** Intersection with an object */
bool Area::whenIntersect(WObject *pcur, WObject *pold)
{
  if (pcur->type == USER_TYPE) inside = true;
  //TODO: alarm sound
  return true;
}

bool Area::whenIntersectOut(WObject *pcur, WObject *pold)
{
  if (pcur->type == USER_TYPE) {
    inside = false;
    return true;
  }
  return false;
}

void Area::funcs()
{}
