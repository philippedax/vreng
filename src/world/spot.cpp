//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2023 Philippe Dax
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
  shape = SPOT_PENTAGON;
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
    else if (! stringcmp(l, "shape")) {
      char str[16];
      l = parseString(l, str, "shape");
      if (! stringcmp(str, "cone")) {
        shape = SPOT_CONE;
      }
    }
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
  float base = 1;

  base = dim.v[0];
  //echo("base: %.2f shape: %d", base, shape);
  switch (shape) {
  case SPOT_PENTAGON:
    sprintf(s, "solid shape=\"pentagon\" s=\"%f\" h=\"%f\" a=\"%f\" />", base, dist, alpha);
    break;
  case SPOT_CONE:
    sprintf(s, "solid shape=\"cone\" rb=\"%f\" rt=\"0.05\" h=\"%f\" a=\"%f\" />", base, dist, alpha);
    break;
  }
  parseSolid(s);
}

Spot::Spot(char *l)
{
  parser(l);
  state = true;			// switch on
  behaviors();
  getDim(dim);			// dim of spot itself
  dim.v[0] /= 2;		// bbox effect FIXME!
  geometry();

  /* orientation */
  if (pos.az == 0)
    pos.az += M_PI_2;
  if (pos.ax == 0)
    pos.ax = -M_PI_2;		// horizontal
  //echo("spot: %.1f %.1f", pos.az, pos.ax);

  initStillObject();
}

/** Created by movie */
Spot::Spot(WObject *movie, void *d, time_t s, time_t u)
{
  defaults();
  state = true;			// switch on
  shape = (d == (void *)SPOT_PENTAGON) ? SPOT_PENTAGON : SPOT_CONE;
  behaviors();

  movie->getDim(dim);		// dim of movie
  geometry();

  /* orientation */
  pos.x = movie->pos.x;
  pos.y = movie->pos.y;
  pos.z = movie->pos.z;
  pos.az = movie->pos.az;
  pos.ax = movie->pos.ax - M_PI_2;	// horizontal
  //echo("pos: %.2f %.2f %.2f %.2f %.2f", pos.x,pos.y,pos.z,pos.az,pos.ax);
  
  initStillObject();
}

void Spot::render()
{
  if (state == false) return;

  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  glPopMatrix();
}

void Spot::On(Spot *po, void *d, time_t s, time_t u)
{
  if (po->state) return;
  po->state = true;
  po->enableBehavior(SPECIFIC_RENDER);
  po->enableBehavior(MIX_RENDER);
  po->disableBehavior(INVISIBLE);
}

void Spot::Off(Spot *po, void *d, time_t s, time_t u)
{
  if (! po->state) return;
  po->state = false;
  po->disableBehavior(SPECIFIC_RENDER);
  po->disableBehavior(MIX_RENDER);
  po->enableBehavior(INVISIBLE);
}

void Spot::create_cb(WObject *po, void *d, time_t s, time_t u)
{
  new Spot(po, d, s, u);
}

void Spot::funcs()
{
  //setActionFunc(SPOT_TYPE, Spot::SPOT_ON, _Action On, "On");
  //setActionFunc(SPOT_TYPE, Spot::SPOT_OFF, _Action Off, "Off");
  //setActionFunc(SPOT_TYPE, Spot::SPOT_CREATE, _Action create_cb, ""); // not necessary
}

void Spot::quit()
{
}
