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
  carriedObject = NULL;
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
bool Carrier::isTaking() const
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
    notice("%s is not mobile", po->getInstance());
    return;
  }

  ::g.gui.showManipulator();
  notice("take control of %s", po->getInstance());

  carriedObject = po;
  carriedObject->move.lspeed.v[0] = 0;
  carriedObject->move.aspeed.v[1] = 0;
  carriedObject->initImposedMovement(0);
  taking = true;
}

/** Leaves control of the mouse to enter in navigation mode */
void Carrier::leave(WObject *po)
{
  leave();
}

void Carrier::leave()
{
  ::g.gui.showNavigator();
  notice("leave control, enter in navigation mode");

  if (! carriedObject)  return;

  carriedObject->pos.moved = true;  // mark it has moved
  defaults();  // reset the carrier
}

void Carrier::set(WObject *po)
{
  taking = true;
  carriedObject = po;
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
#if 0 //dax
  WObject *poldobj = new WObject();
  carriedObject->copyPositionAndBB(poldobj);	// copy oldpos, oldangle
#endif

  switch (vkey) {
    case KEY_AV: carriedObject->pos.x += last*lspeed; break; // ^
    case KEY_AR: carriedObject->pos.x -= last*lspeed; break; // v
    case KEY_SG: carriedObject->pos.y += last*lspeed; break; // <
    case KEY_SD: carriedObject->pos.y -= last*lspeed; break; // >
    case KEY_JU: carriedObject->pos.z += last*lspeed; break; // ^
    case KEY_JD: carriedObject->pos.z -= last*lspeed; break; // V
    case KEY_GA: carriedObject->pos.az += last*lspeed; break; // <-
    case KEY_DR: carriedObject->pos.az -= last*lspeed; break; // ->
    case KEY_TL: carriedObject->pos.ax += last*lspeed; break; // <,
    case KEY_TR: carriedObject->pos.ax -= last*lspeed; break; // ,>
    case KEY_MT: carriedObject->pos.ay += last*lspeed; break; // ^,
    case KEY_DE: carriedObject->pos.ay -= last*lspeed; break; // ,^
  }

  carriedObject->updatePositionAndGrid(carriedObject->pos);
  //dax carriedObject->updatePosition();

#if 0 //dax
  carriedObject->updateGrid(poldobj);
  if (carriedObject->isBehavior(COLLIDE_NEVER)) {
    delete poldobj;
    return;
  }

  ObjectList *vicinitylist = carriedObject->getVicinityList(poldobj);
  carriedObject->generalIntersect(poldobj, vicinitylist);
  if (*names.type)	//FIXME: segfault
    vicinitylist->remove();
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
  setActionFunc(CARRIER_TYPE, Carrier::SETLSPEED, WO_ACTION setLspeed, "");
  setActionFunc(CARRIER_TYPE, Carrier::SETASPEED, WO_ACTION setAspeed, "");
}
