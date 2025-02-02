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
  type = CARRIER_TYPE;
  ::g.gui.setToCarrier(this);
}

/** Accessor */
bool Carrier::underControl() const
{
  return control;
}

/** Takes control of the mouse to enter in manipulation mode */
void Carrier::take(Object *po)
{
  if (po->mode != MOBILE) {
    echo("%s is not mobile", po->objectName());
    return;
  }

  ::g.gui.showManipulator();
  ::g.gui.expandNavig();	// shows Manipulator palette
  echo("take control of %s", po->objectName());

  object = po;
  object->move.manip = true;
  object->move.lspeed.v[0] = lspeed;
  object->move.aspeed.v[1] = aspeed;
  object->imposedMovement(1);
  control = true;
}

/** Leaves control of the mouse to enter in navigation mode */
void Carrier::leave()
{
  ::g.gui.collapseNavig();	// hints Manipulator palette
  ::g.gui.showNavigator();
  echo("leave control, enter in navigation mode");

  if (! object)  return;

  object->pos.alter = true;	// mark it has changed
  object->move.manip = false;
  defaults();			// reset the carrier
}

/** Mouse event
 * called by mpress (navig.cpp)
 */
void Carrier::mouseEvent(uint16_t x, uint16_t y, uint8_t button)
{
  if (button == 1) {
    leave();			// left clic => leave mouse control
  }
}

/** Mouse event
 * called by Motion::move (navig.cpp)
 */
void Carrier::mouseEvent(int8_t vkey, float last)
{
  Object *poldobj = new Object();
  object->copyPositionAndBB(poldobj);	// copy oldpos, oldangle

  echo("carrier: k=%d", vkey);
  switch (vkey) {
    case KEY_FW: object->pos.x += last*lspeed; break; // ^
    case KEY_BW: object->pos.x -= last*lspeed; break; // v
    case KEY_ML: object->pos.y += last*lspeed; break; // <
    case KEY_MR: object->pos.y -= last*lspeed; break; // >
    case KEY_UP: object->pos.z += last*lspeed; break; // ^
    case KEY_DO: object->pos.z -= last*lspeed; break; // V
    case KEY_LE: object->pos.az += last*aspeed; break; // <-
    case KEY_RI: object->pos.az -= last*aspeed; break; // ->
    case KEY_TL: object->pos.ax += last*aspeed; break; // <,
    case KEY_TR: object->pos.ax -= last*aspeed; break; // ,>
    case KEY_MU: object->pos.ay += last*aspeed; break; // ^,
    case KEY_MD: object->pos.ay -= last*aspeed; break; // ,^
  }
  echo("move pos: %.1f %.1f %.1f", object->pos.x, object->pos.y, object->pos.z);

  object->updatePositionAndGrid(object->pos);
  object->updatePosition();

  OList *vicilist = object->getVicinity(poldobj);
  object->generalIntersect(poldobj, vicilist);
  if (*name.type) {	//FIXME: segfault
    vicilist->remove();
  }
  delete poldobj;
  object->updGrid(poldobj);
  if (object->isBehavior(COLLIDE_NEVER)) {
    delete poldobj;
    return;
  }
}

void Carrier::setLspeed(Carrier *pc, void *d, time_t s, time_t u)
{
  pc->lspeed = *(static_cast<float *>(d));
}

void Carrier::setAspeed(Carrier *pc, void *d, time_t s, time_t u)
{
  pc->aspeed = *(static_cast<float *>(d));
}

void Carrier::funcs()
{
  setActionFunc(CARRIER_TYPE, O_SETLSPEED, _Action setLspeed, "");
  setActionFunc(CARRIER_TYPE, O_SETASPEED, _Action setAspeed, "");
}
