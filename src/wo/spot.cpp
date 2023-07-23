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
  state = false;
  alpha = .3;
  dist = 10;
  color[0] = color[1] = color[2] = 1; // white
  clearV3(dim);
}

void Spot::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "alpha")) l = parseFloat(l, &alpha, "alpha");
    else if (! stringcmp(l, "color")) l = parseVector3f(l, color, "color");
  }
  end_while_parse(l);
}

void Spot::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  if (state == true) {
    enableBehavior(SPECIFIC_RENDER);
    enableBehavior(MIX_RENDER);
  }
}

void Spot::geometry()
{
  char s[128];

  if (dim.v[0] == 0 && dim.v[1] == 0) { 
    getDim(dim);
  }
  echo("dim: %.1f %.1f %.1f", dim.v[0], dim.v[1], dim.v[2]);
  float base = dim.v[0]/2;
  if (base == 0)
    base = 1;
  sprintf(s, "solid shape=\"pyramid\" s=\"%f\" h=\"%f\" a=\"%f\" />", base, dist, alpha);
  parseSolid(s);
}

Spot::Spot(char *l)
{
  parser(l);
  behaviors();
  geometry();
  if (pos.az == 0)
    pos.az = M_PI_2;
  if (pos.ax == 0)
    pos.ax = -M_PI_2;
  //echo("spot: %.1f %.1f", pos.az, pos.ax);

  initMobileObject(0);
}

/** Created by movie */
Spot::Spot(WObject *movie, void *d, time_t s, time_t u)
{
  echo("create from movie");
  defaults();
  behaviors();

  movie->getDim(dim);
  geometry();

  /* orientation */
  pos.az = movie->pos.az + M_PI_2;
  pos.ax = movie->pos.ax - M_PI_2;
  
  initMobileObject(0);
}

void Spot::render()
{
  if (state == false) return;

  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  glPopMatrix();
}

void Spot::On(Spot *po, void *data, time_t s, time_t u)
{
  if (po->state) return;
  po->state = true;
  po->enableBehavior(SPECIFIC_RENDER);
  po->enableBehavior(MIX_RENDER);
}

void Spot::Off(Spot *po, void *data, time_t s, time_t u)
{
  if (! po->state) return;
  po->state = false;
  po->disableBehavior(SPECIFIC_RENDER);
  po->disableBehavior(MIX_RENDER);
}

void Spot::create_cb(Spot *po, void *d, time_t s, time_t u)
{
  new Spot(po, d, s, u);
}

void Spot::funcs()
{
  setActionFunc(SPOT_TYPE, 0, _Action On, "On");
  setActionFunc(SPOT_TYPE, 1, _Action Off, "Off");
  setActionFunc(SPOT_TYPE, 2, _Action create_cb, "");
}

void Spot::quit()
{
}