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
// motion.hpp
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
  float accel;				///< accelerator

  class Carrier *carrier;		///< object carrier

 public:
  static Motion u_trans_x, u_trans_y, u_trans_z, u_rot_z;	///< user movement
  static Motion o_trans_f, o_trans_b, o_trans_l, o_trans_r, o_trans_d, o_trans_u, o_rot_l_z, o_rot_r_z, o_rot_l_x, o_rot_r_x, o_rot_d_y, o_rot_u_y;	///< object movement

  Motion(int minuskey, int pluskey, int fun, float accel);	///< constructor

  virtual ~Motion() {}		///< destructor

  void move(int mspeed);
  /**< Mouse motion according to mouse speed */

  void stop();
  /**< Stops motion == move(0) */

  static Motion * current();
  /**< Returns current instance */

  void setToCarrier(class Carrier *_carrier);
};

#endif
