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
/*
 *  motion.cpp : Ubit Movements for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Author: Eric Lecolinet
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002-2008 Eric Lecolinet @ ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */
#include "vreng.hpp"
#include "motion.hpp"
#include "user.hpp"	// UserAction
#include "carrier.hpp"	// Carrier
#include "move.hpp"	// changeKey


const float Motion::LINEAR_ACCEL  = 0.055;
const float Motion::ANGULAR_ACCEL = 0.0045;

// local
static Motion *motion;	///< current pointer


Motion::Motion(int _minuskey, int _pluskey, int _fun, float _accel)
{
  minuskey = _minuskey;
  pluskey  = _pluskey;
  fun      = _fun;
  accel    = _accel;
  motion   = this;
  carrier  = NULL;
}

Motion * Motion::current()
{
  return motion;
}

void Motion::stop()
{
  move(0);
}

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

// User

// Move forward, backward
Motion Motion::ytrans(0 /*KEY_AV*/, KEY_AR, UserAction::UA_SETLSPEED, LINEAR_ACCEL);
// Turn left, right
Motion Motion::zrot(0   /*KEY_GA*/, KEY_DR, UserAction::UA_SETASPEED, ANGULAR_ACCEL);
// Move left, right
Motion Motion::xtrans(0 /*KEY_SG*/, KEY_SD, UserAction::UA_SETLSPEED, LINEAR_ACCEL);
// Move down, up
Motion Motion::ztrans(0 /*KEY_JU*/, KEY_JD, UserAction::UA_SETLSPEED, LINEAR_ACCEL);

// Object

// Move forward, backward
Motion Motion::trans_forw(0 /*KEY_AV*/, KEY_AR, Carrier::SETLSPEED, LINEAR_ACCEL);
// Move left, right
Motion Motion::trans_left(0 /*KEY_SG*/, KEY_SD, Carrier::SETLSPEED, LINEAR_ACCEL);
// Move down, up
Motion Motion::trans_up(0 /*KEY_JU*/, KEY_JD, Carrier::SETLSPEED, LINEAR_ACCEL);

// Turn left, right (Yaw)
Motion Motion::zrot_left(0,   KEY_GA, Carrier::SETASPEED, 0);
Motion Motion::zrot_right(0,  KEY_DR, Carrier::SETASPEED, 0);
// Roll left, right
Motion Motion::xrot_left(0,   KEY_TL, Carrier::SETASPEED, 0);
Motion Motion::xrot_right(0,  KEY_TR, Carrier::SETASPEED, 0);
// Tilt down, up
Motion Motion::yrot_down(0,   KEY_DE, Carrier::SETASPEED, 0);
Motion Motion::yrot_up(0,     KEY_MT, Carrier::SETASPEED, 0);
