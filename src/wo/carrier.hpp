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
#ifndef CARRIER_HPP
#define CARRIER_HPP

#include "wobject.hpp"

#define CARRIER_TYPE	41
#define CARRIER_NAME	"Carrier"


/**
 * Carrier class
 *
 * Used to move an object.
 * The object, deleted from mobile-list, is managed by the Carrier.
 * The motion is done :
 *  - either by the mouse in the manipulator menu
 *  - either by arrow keys -> Obsoleted
 *    . button 1 leaves the object
 *    . button 3 changes the mode of motion :
 *      mode 1: translation along X/Y axis (coordoninates relative to avatar)
 *      mode 2: translation along Y/Z axis
 *      mode 3: rotation around X and Z axis
 *      mode 4: rotation around Y axis
 */
class Carrier: public WObject {

private:
  class WObject *carriedObject;	///< object ptr
  float lspeed;			///< linear speed
  float aspeed;			///< angular speed
  bool taking;			///< flag true/false


public:
  static const float LSPEED;	///< linear speed
  static const float ASPEED;	///< angular speed

  /* actions */
  enum {
    SETLSPEED,
    SETASPEED
  };

  static const OClass oclass;   ///< class variable

  virtual const OClass* getOClass() {return &oclass;} ///< virtual inst. method

  Carrier();
  /**< Constructor */

  static void funcs();		///< init funclist

  virtual bool isTaking() const;
  /**< Accessor: is something taking ? */

  virtual void take(WObject *po);
  /**< handle position changes
   * Takes control of the object to manipulate it.
   * The position/orientation of the object will be changed by changePosition
   * and the object will be updated in the 3D */

  virtual void set(WObject *po);
  /**< Sets taking */

  virtual void leave(WObject *po);
  virtual void leave();
  /**< called when we release an object
   * Releases the carried object, put it into the mobile-list
   * and desactivates the carrier */

  virtual void mouseEvent(int8_t vkey, float last);

  virtual void mouseEvent(uint16_t x, uint16_t y, uint8_t button);
  /**< called by mpress in navig.cpp to redirect mouse clics
   * when we move the object
   * left button leaves the object
   * rigth button changes the motion mode */

  virtual void keyEvent(uint8_t vkey, float last);
  /**< called by userChangePositionOneType from move.cc to redirect
   * arrow keys to move the object
   * and handle intersects of this object */

private:
  virtual void defaults();
  /**< Sets default values */

  // GUI and callbacks
  static void takeControl(WObject *po, void *d, time_t s, time_t u);
  /**< called when we take an object
   * puts the object into the carrier and realizes all
   * needed operations to pass in transport mode */

  static void leaveControl(WObject *po, void *d, time_t s, time_t u);
  /**< called when we leave an object */

  static void setLspeed(Carrier *pc, void *d, time_t s, time_t u);
  static void setAspeed(Carrier *pc, void *d, time_t s, time_t u);
};

#endif
