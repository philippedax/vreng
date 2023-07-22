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
#include "spot.hpp"


const OClass Spot::oclass(SPOT_TYPE, "Spot", Spot::creator);


/* creation from a file */
WObject * Spot::creator(char *l)
{
  return new Spot(l);
}

void Spot::defaults()
{
  on = true;
  alpha = 0;
  dist = 10;
  base = 1;
  color[0] = color[1] = color[2] = 1; // white
}

void Spot::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (!stringcmp(l, "alpha"))   l = parseFloat(l, &alpha, "alpha");
    else if (!stringcmp(l, "color"))  l = parseVector3f(l, color, "color");
  }
  end_while_parse(l);
}

void Spot::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(SPECIFIC_RENDER);
}

void Spot::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"pyramid\" s=\"%f\" h=\"%f\" a=\"%f\" />", base, dist, alpha);
  parseSolid(s);
}

Spot::Spot(char *l)
{
  pos.ax = -M_PI_2;
  parser(l);
  behaviors();
  geometry();

  initMobileObject(0);
}

void Spot::render()
{
  if (! on) return;

  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  glPopMatrix();
}

void Spot::quit()
{
}

void Spot::funcs() {}
