//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// carrier.cpp
//
// Carrier handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "carrier.hpp"
#include "user.hpp"	// localuser
#include "gui.hpp"	// setToCarrier
#include "solid.hpp"	// setBB
#include "thing.hpp"	// Thing
#include "wall.hpp"	// Wall
#include "ball.hpp"	// Ball
#include "mirage.hpp"	// Mirage
#include "model.hpp"	// Model

const OClass Carrier::oclass(CARRIER_TYPE, "Carrier", NULL);

const float Carrier::LSPEED = 0.5;
const float Carrier::ASPEED = 0.5;


void Carrier::defaults()
{
  control = false;
  object = NULL;
  lspeed = LSPEED;
  aspeed = ASPEED;
}

/** Constructor */
Carrier::Carrier()
{
  defaults();
  ::g.gui.setToCarrier(this);
}

/** Checks control */
bool Carrier::underControl() const
{
  //echo("ctrl: %d", control);
  return control;
}

/** Gets object under control */
Object * Carrier::getObject()
{
  return object;
}

/** Takes control of the mouse to enter in manipulation mode */
void Carrier::take(Object *o)
{
  if (! o) return;
  //if (control) return;		// already under control - segfault
  object = o;			// memorize object
  if (o->mode != MOBILE) {
    echo("%s is not mobile", o->objectName());
    return;
  }
  //::g.gui.showManipulator();	// open Manipulator palette
  //::g.gui.expandNavig();	// shows Manipulator palette

  o->move.manip = true;
  o->move.lspeed.v[0] = lspeed;
  o->move.aspeed.v[1] = aspeed;
  o->imposedMovement(1);
  o->enableBehavior(NO_ELEMENTARY_MOVE);
  if (! o->carrier) {
    o->carrier = new Carrier();
  }
  control = true;
  o->carrier->control = true;
  localuser->carrier->control = true;
  echo("take control of %s, manipulation mode", o->objectName());
}

/** Leaves control of the mouse to enter in navigation mode */
void Carrier::leave(Object *o)
{
  if (! o) return;
  //if (! control) return;		// already has leaved
  //::g.gui.collapseNavig();	// close Manipulator palette
  //::g.gui.showNavigator();	// open Navigator palette

  o->move.manip = false;
  o->pos.alter = true;		// mark it has changed
  if (o->carrier) {
    o->carrier->control = false;
    localuser->carrier->control = false;
  }
  control = false;
  echo("leave control of %s, navigation mode", o->objectName());
  defaults();			// reset the carrier
}

/** Mouse event
 * called by mpress (navig.cpp)
 */
void Carrier::mouseEvent(uint16_t x, uint16_t y, uint8_t button)
{
#if 0
  if (button) {			// any button pressed
    if (object) {
      //echo("leave %s", object->objectName());
      leave(object);		// left clic => leave mouse control
    }
  }
#endif
}

/** Mouse event
 * called by changePosition (move.cpp)
 */
void Carrier::mouseEvent(Object *o, int8_t vkey, float last)
{
  if (! o) return;

  //echo("vkey: %d", vkey);
  switch (vkey) {
    case KEY_FW: o->pos.x += last*lspeed; break; // ^
    case KEY_BW: o->pos.x -= last*lspeed; break; // v
    case KEY_ML: o->pos.y += last*lspeed; break; // <
    case KEY_MR: o->pos.y -= last*lspeed; break; // >
    case KEY_UP: o->pos.z += last*lspeed; break; // ^
    case KEY_DO: o->pos.z -= last*lspeed; break; // V
    case KEY_LE: o->pos.az += last*aspeed; break; // <-
    case KEY_RI: o->pos.az -= last*aspeed; break; // ->
    case KEY_TL: o->pos.ax += last*aspeed; break; // <,
    case KEY_TR: o->pos.ax -= last*aspeed; break; // ,>
    case KEY_MU: o->pos.ay += last*aspeed; break; // ^,
    case KEY_MD: o->pos.ay -= last*aspeed; break; // ,^
    case KEY_M: resize(o, '-'); break; // -
    case KEY_P: resize(o, '+'); break; // +
  }
  //o->updateGrid(o->pos);
  o->updatePos();
}

/** Resizes object by a new object */
void Carrier::resize(Object *o, char sign)
{
  V3 dim;
  char dimstr[32];

  o->getDim(dim);
  //echo("dim: %.3f %.3f %.3f", dim.v[0]*2, dim.v[1]*2, dim.v[2]*2);

  switch (sign) {
    case '-': for (int i=0; i<3; i++) {
                dim.v[i] -= dim.v[i]/10;
                dim.v[i] = MAX(dim.v[i], 0.001);
                o->pos.bbs.v[i] -= o->pos.bbs.v[i]/10;
                o->pos.bbs.v[i] = MAX(o->pos.bbs.v[i], 0.001);
              }
              break;
    case '+': for (int i=0; i<3; i++) {
                dim.v[i] += dim.v[i]/10;
                dim.v[i] = MIN(dim.v[i], 10);
                o->pos.bbs.v[i] += o->pos.bbs.v[i]/10;
              }
              break;
  }

  switch (o->solid->shape) {
    case STOK_BOX:    sprintf(dimstr, "dim=\"%.3f %.3f %.3f\"", dim.v[0]*2, dim.v[1]*2, dim.v[2]*2);
                      break;
    case STOK_SPHERE: sprintf(dimstr, "r=\"%.3f\"", dim.v[0]);
                      break;
    default: return;
  }
  //echo("dimstr: %s", dimstr);

  o->geom = new char[128];
  sprintf(o->geom, "shape=\"%s\" %s a=\"%.1f\" dif=\"%.1f %.1f %.1f\" />",
          o->solid->getShape(o->solid->shape),
          dimstr,
          MAX(object->solid->alpha, 1),
          MAX(object->solid->mat_diffuse[0], 1),
          MAX(object->solid->mat_diffuse[1], 1),
          MAX(object->solid->mat_diffuse[2], 1));
  echo("geom: %s", o->geom);
  switch (o->type) {
    case THING_TYPE:{ Thing *o2 = new Thing(localuser, o->geom); take(o2); o2->pos = o->pos; } break;
    case WALL_TYPE: { Wall *o2 = new Wall(localuser, o->geom); take(o2); o2->pos = o->pos; }   break;
    case BALL_TYPE: { Ball *o2 = new Ball(localuser, o->geom); take(o2); o2->pos = o->pos; }   break;
    case MIRAGE_TYPE:{ Mirage *o2 = new Mirage(localuser, o->geom); take(o2); o2->pos = o->pos; } break;
  }
    
  o->toDelete();
}

void Carrier::setLspeed(Carrier *pc, void *d, time_t s, time_t u)
{
  pc->lspeed = *(static_cast<float *> (d));
}

void Carrier::setAspeed(Carrier *pc, void *d, time_t s, time_t u)
{
  pc->aspeed = *(static_cast<float *> (d));
}

void Carrier::funcs()
{
  setActionFunc(CARRIER_TYPE, O_SETLSPEED, _Action setLspeed, "");
  setActionFunc(CARRIER_TYPE, O_SETASPEED, _Action setAspeed, "");
}
