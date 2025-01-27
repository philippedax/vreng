//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// carrier.hpp
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
class Carrier: public Object {
 private:
  class Object *object;	///< object ptr
  float lspeed;		///< linear speed
  float aspeed;		///< angular speed
  bool control;		///< flag true/false

 public:
  static const float LSPEED;	///< linear speed
  static const float ASPEED;	///< angular speed

  /* actions */
  enum {
    O_SETLSPEED,
    O_SETASPEED
  };

  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;} ///< inst. method

  Carrier();
  /**< Constructor */

  static void funcs();		///< init funclist

  bool underControl() const;
  /**< Accessor: is something taking ? */

  void take(Object *po);
  /**< handle position changes
   * Takes control of the object to manipulate it.
   * The position/orientation of the object will be changed by changePosition
   * and the object will be updated in the 3D */

  void leave();
  /**< called when we release an object
   * Releases the carried object, put it into the mobile-list
   * and desactivates the carrier */

  void mouseEvent(uint16_t x, uint16_t y, uint8_t button);
  /**< called by mpress in navig.cpp to redirect mouse clics
   * when we move the object
   * left button leaves the object
   * rigth button changes the motion mode */

  void mouseEvent(int8_t vkey, float last);
  /**< called by userChangePositionOneType from move.cc to redirect
   * arrow keys to move the object
   * and handle intersects of this object */

 private:
  void defaults();
  /**< Sets default values */

  static void setLspeed(Carrier *pc, void *d, time_t s, time_t u);
  static void setAspeed(Carrier *pc, void *d, time_t s, time_t u);
};

#endif
