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
 *  mvt.cpp : Ubit Movements for the VREng GUI
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
#include "mvt.hpp"
#include "wobject.hpp"
#include "useraction.hpp"
#include "user.hpp"	// User::LSPEED;  // !! incorrect !!
#include "carrier.hpp"
#include "move.hpp"	// changeKey
#include "world.hpp"	// isCurrent

const float Mvt::MOUSE_LINEAR_ACCEL  = 0.055;
const float Mvt::MOUSE_ANGULAR_ACCEL = 0.0045;

// local
static Mvt *mvt;	///< pointer


Mvt::Mvt(int _minuskey, int _pluskey, int _fun, float _accel)
{
  minuskey = _minuskey;
  pluskey  = _pluskey;
  fun      = _fun;
  accel    = _accel;
  mvt      = this;
  carrier  = NULL;
}

Mvt * Mvt::pointer()
{
  return mvt;
}

void Mvt::stop()
{
  move(0);
}

void Mvt::move(int mspeed)
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
      if (carrier && carrier->isTaking())
        new_speed = Carrier::LSPEED;
      else
        new_speed = User::LSPEED;  // !! incorect !!
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

void Mvt::setToCarrier(class Carrier *_carrier)
{
  carrier = _carrier;
}

// User

// Move forward, backward
Mvt Mvt::ytrans(0 /*KEY_AV*/, KEY_AR, UserAction::SETLSPEED, MOUSE_LINEAR_ACCEL);
// Turn left, right
Mvt Mvt::zrot(0   /*KEY_GA*/, KEY_DR, UserAction::SETASPEED, MOUSE_ANGULAR_ACCEL);
// Move left, right
Mvt Mvt::xtrans(0 /*KEY_SG*/, KEY_SD, UserAction::SETLSPEED, MOUSE_LINEAR_ACCEL);
// Move down, up
Mvt Mvt::ztrans(0 /*KEY_JU*/, KEY_JD, UserAction::SETLSPEED, MOUSE_LINEAR_ACCEL);

// Object

// Move forward, backward
Mvt Mvt::trans_forw(0 /*KEY_AV*/, KEY_AR, Carrier::SETLSPEED, MOUSE_LINEAR_ACCEL);
// Move left, right
Mvt Mvt::trans_left(0 /*KEY_SG*/, KEY_SD, Carrier::SETLSPEED, MOUSE_LINEAR_ACCEL);
// Move down, up
Mvt Mvt::trans_up(0 /*KEY_JU*/, KEY_JD, Carrier::SETLSPEED, MOUSE_LINEAR_ACCEL);

// Turn left, right (Yaw)
Mvt Mvt::zrot_left(0,   KEY_GA, Carrier::SETASPEED, 0);
Mvt Mvt::zrot_right(0,  KEY_DR, Carrier::SETASPEED, 0);
// Roll left, right
Mvt Mvt::xrot_left(0,   KEY_TL, Carrier::SETASPEED, 0);
Mvt Mvt::xrot_right(0,  KEY_TR, Carrier::SETASPEED, 0);
// Tilt down, up (Pitch)
Mvt Mvt::yrot_down(0,   KEY_DE, Carrier::SETASPEED, 0);
Mvt Mvt::yrot_up(0,     KEY_MT, Carrier::SETASPEED, 0);
