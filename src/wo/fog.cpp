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
#include "fog.hpp"


const OClass Fog::oclass(FOG_TYPE, "Fog", Fog::creator);


/* creation from a file */
WObject * Fog::creator(char *l)
{
  return new Fog(l);
}

void Fog::defaults()
{
  density = 0;
  color[0] = color[1] = color[2] = 1; // white
}

void Fog::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (!stringcmp(l, "density"))   l = parseFloat(l, &density, "density");
    else if (!stringcmp(l, "color"))  l = parseVector3f(l, color, "color");
  }
  end_while_parse(l);
}

void Fog::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
}

void Fog::geometry()
{
  char s[128];
  sprintf(s, "solid shape=\"none\" />");
  parseSolid(s);
}

Fog::Fog(char *l)
{
  parser(l);
  behaviors();
  geometry();
}

void Fog::render()
{
  glPushMatrix();
  glEnable(GL_FOG);
  glFogf(GL_FOG_START, 2.5);
  glFogf(GL_FOG_END, 4);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogi(GL_FOG_MODE, GL_EXP);
  glFogf(GL_FOG_DENSITY, density);
  glFogfv(GL_FOG_COLOR, color);
  glDisable(GL_FOG);
  glPopMatrix();
}

void Fog::quit()
{
}

void Fog::funcs() {}
