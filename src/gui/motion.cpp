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
//  motion.cpp : Ubit Movements for the VREng GUI
//
//  VREng / Ubit Project
//  Author: Eric Lecolinet
//
//  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "motion.hpp"
#include "user.hpp"	// User
#include "carrier.hpp"	// Carrier
#include "move.hpp"	// changeKey


const float Motion::LINEAR_ACCEL  = 0.055;
const float Motion::ANGULAR_ACCEL = 0.0045;

// local
static Motion *motion;	///< current pointer


/** Constructor */
Motion::Motion(int _minuskey, int _pluskey, int _fun, float _accel)
{
  minuskey = _minuskey;
  pluskey  = _pluskey;
  fun      = _fun;
  accel    = _accel;
  motion   = this;
  carrier  = NULL;
}

/** Gets current instance */
Motion * Motion::current()
{
  return motion;
}

/** Stops motion */
void Motion::stop()
{
  move(0);
}

/** Do motion */
void Motion::move(int mspeed)
{
  struct timeval t;
  gettimeofday(&t, NULL);

  if (mspeed == 0) {	// stop
    changeKey(minuskey, RELEASED, t.tv_sec, t.tv_usec);
    changeKey(pluskey,  RELEASED, t.tv_sec, t.tv_usec);
  }
  else {
    if (! localuser) return;
    float new_speed;

    if (accel > 0) {
      new_speed = accel * mspeed;
    }
    else {
      if (carrier && carrier->underControl()) {
        new_speed = Carrier::LSPEED;
      }
      else {
        new_speed = User::LSPEED;  // !! incorrect !!
      }
    }
    if (minuskey == 0) {
      localuser->specialAction(fun, &new_speed, t.tv_sec, t.tv_usec);
      changeKey(pluskey, PRESSED, t.tv_sec, t.tv_usec);
    }
    else {	// minuskey > 0
      if (new_speed > 0) {
        localuser->specialAction(fun, &new_speed, t.tv_sec, t.tv_usec);
        changeKey(minuskey, RELEASED, t.tv_sec, t.tv_usec);
        changeKey(pluskey,  PRESSED,  t.tv_sec, t.tv_usec);
      }
      else {
        new_speed = -new_speed;
        localuser->specialAction(fun, &new_speed, t.tv_sec, t.tv_usec);
        changeKey(minuskey, PRESSED,  t.tv_sec, t.tv_usec);
        changeKey(pluskey,  RELEASED, t.tv_sec, t.tv_usec);
      }
    }
  }
}

void Motion::setToCarrier(class Carrier *_carrier)
{
  carrier = _carrier;
}

/////////////////
// User movements
/////////////////

// Translation movements

/** Move forward, backward */
Motion Motion::u_trans_y(0, KEY_AR /*KEY_AV*/, User::UA_SETLSPEED, LINEAR_ACCEL);
/** Move left, right */
Motion Motion::u_trans_x(0, KEY_SD /*KEY_SG*/, User::UA_SETLSPEED, LINEAR_ACCEL);
/** Move up, down */
Motion Motion::u_trans_z(0, KEY_JD /*KEY_JU*/, User::UA_SETLSPEED, LINEAR_ACCEL);

// Rotation movements

/** Turn left, right */
Motion Motion::u_rot_z(0,   KEY_DR /*KEY_GA*/, User::UA_SETASPEED, ANGULAR_ACCEL);

///////////////////
// Object movements
///////////////////

// Translation movements

/** Move forward, backward */
Motion Motion::o_trans_f(0, KEY_AR /*KEY_AV*/, Carrier::SETLSPEED, LINEAR_ACCEL);
/** Move left, right */
Motion Motion::o_trans_l(0, KEY_SD /*KEY_SG*/, Carrier::SETLSPEED, LINEAR_ACCEL);
/** Move up, down */
Motion Motion::o_trans_u(0, KEY_JD /*KEY_JU*/, Carrier::SETLSPEED, LINEAR_ACCEL);

// Rotation movements

/** Turn left, right (Yaw) */
Motion Motion::o_rot_l_z(0,  KEY_GA, Carrier::SETASPEED, 0);
Motion Motion::o_rot_r_z(0,  KEY_DR, Carrier::SETASPEED, 0);
/** Roll left, right */
Motion Motion::o_rot_l_x(0,  KEY_TL, Carrier::SETASPEED, 0);
Motion Motion::o_rot_r_x(0,  KEY_TR, Carrier::SETASPEED, 0);
/** Tilt down, up */
Motion Motion::o_rot_d_y(0,  KEY_DE, Carrier::SETASPEED, 0);
Motion Motion::o_rot_u_y(0,  KEY_MT, Carrier::SETASPEED, 0);
