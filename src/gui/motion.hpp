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
#ifndef MOTION_HPP
#define MOTION_HPP


/**
 * Motion class
 * Process movements
 */
class Motion {
 private:
  static const bool PRESSED = true;	///< key pressed
  static const bool RELEASED = false;	///< key released
  static const float LINEAR_ACCEL;	///< linear acceleraton
  static const float ANGULAR_ACCEL;	///< angular acceleraton

  int minuskey, pluskey, fun;
  float accel;			///< accelerator

 public:
  static Motion xtrans, ytrans, ztrans, zrot;		///< user movement
  static Motion trans_forw, trans_back, trans_left, trans_right, trans_down, trans_up, zrot_left, zrot_right, xrot_left, xrot_right, yrot_down, yrot_up;	///< object movement

  class Carrier *carrier;	///< Object carrier

  Motion(int minuskey, int pluskey, int fun, float accel);
  ///< constructor

  virtual ~Motion() {}
  ///< destructor

  virtual void move(int mspeed);
  /**< Mouse motion according to mouse speed */

  virtual void stop();
  /**< Stops motion == move(0) */

  static Motion * current();
  /**< Returns current instance */

  void setToCarrier(class Carrier *_carrier);
};

#endif
