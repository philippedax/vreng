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
#include "carrier.hpp"
#include "user.hpp"	// localuser
#include "gui.hpp"	// setToCarrier

const OClass Carrier::oclass(CARRIER_TYPE, "Carrier", NULL);

const float Carrier::LSPEED = 0.5;
const float Carrier::ASPEED = 0.5;


void Carrier::defaults()
{
  taking = false;
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
  return taking;
}

void Carrier::takeControl(WObject *po, void *d, time_t s, time_t us)
{
  localuser->carrier->take(po);
}

void Carrier::leaveControl(WObject *po, void *d, time_t s, time_t us)
{
  localuser->carrier->leave(po);
}

/** Takes control of the mouse to enter in manipulation mode */
void Carrier::take(WObject *po)
{
  if (po->mode != MOBILE) {
    warning("%s is not mobile", po->getInstance());
    return;
  }

  ::g.gui.showManipulator();
  ::g.gui.expandNavig();
  echo("take control of %s", po->getInstance());

  object = po;
  object->move.lspeed.v[0] = 0;
  object->move.aspeed.v[1] = 0;
  object->initImposedMovement(0);
  taking = true;
}

/** Leaves control of the mouse to enter in navigation mode */
void Carrier::leave(WObject *po)
{
  leave();
}

void Carrier::leave()
{
  ::g.gui.collapseNavig();
  ::g.gui.showNavigator();
  echo("leave control, enter in navigation mode");

  if (! object)  return;

  object->pos.alter = true;  // mark it has changed
  defaults();  // reset the carrier
}

void Carrier::set(WObject *po)
{
  taking = true;
  object = po;
}

/** Mouse event
 * called by mpress (navig.cpp)
 */
void Carrier::mouseEvent(uint16_t x, uint16_t y, uint8_t button)
{
  if (button == 1)  leave();  // left clic => leave mouse control
}

/** Mouse event
 * called by Motion::move (navig.cpp)
 */
void Carrier::mouseEvent(int8_t vkey, float last)
{
  WObject *poldobj = new WObject();
  object->copyPositionAndBB(poldobj);	// copy oldpos, oldangle

  switch (vkey) {
    case KEY_AV: object->pos.x += last*lspeed; break; // ^
    case KEY_AR: object->pos.x -= last*lspeed; break; // v
    case KEY_SG: object->pos.y += last*lspeed; break; // <
    case KEY_SD: object->pos.y -= last*lspeed; break; // >
    case KEY_JU: object->pos.z += last*lspeed; break; // ^
    case KEY_JD: object->pos.z -= last*lspeed; break; // V
    case KEY_GA: object->pos.az += last*lspeed; break; // <-
    case KEY_DR: object->pos.az -= last*lspeed; break; // ->
    case KEY_TL: object->pos.ax += last*lspeed; break; // <,
    case KEY_TR: object->pos.ax -= last*lspeed; break; // ,>
    case KEY_MT: object->pos.ay += last*lspeed; break; // ^,
    case KEY_DE: object->pos.ay -= last*lspeed; break; // ,^
  }
  //echo("pos: %.1f %.1f %.1f", object->pos.x,object->pos.y,object->pos.z);

  object->updatePositionAndGrid(object->pos);
  object->updatePosition();

  object->updateGrid(poldobj);
  if (object->isBehavior(COLLIDE_NEVER)) {
    delete poldobj;
    return;
  }

#if 1 //dax
  OList *vicinitylist = object->getVicinity(poldobj);
  object->generalIntersect(poldobj, vicinitylist);
  if (*names.type) {	//FIXME: segfault
    vicinitylist->remove();
  }
  delete poldobj;
#endif
}

/** Key event: called by changePositionOneDir (move.cc) */
void Carrier::keyEvent(uint8_t vkey, float last)
{}

void Carrier::setLspeed(Carrier *pc, void *d, time_t s, time_t u)
{
  pc->lspeed = *((float *) d);
}

void Carrier::setAspeed(Carrier *pc, void *d, time_t s, time_t u)
{
  pc->aspeed = *((float *) d);
}

void Carrier::funcs()
{
  setActionFunc(CARRIER_TYPE, Carrier::SETLSPEED, _Action setLspeed, "");
  setActionFunc(CARRIER_TYPE, Carrier::SETASPEED, _Action setAspeed, "");
}
