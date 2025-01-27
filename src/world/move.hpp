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
// move.hpp
//---------------------------------------------------------------------------
#ifndef MOVE_HPP
#define MOVE_HPP

#define GRAVITY		1.5		// 1.5 m/s
#define MIN_LASTING     0.00005		// 50 usec
#define MIN_KEYLASTING	0.000001	// 1 usec

/* Keys needed by VREng move module */
enum {
  KEY_FW,	///< move forward
  KEY_BW,	///< move backward
  KEY_LE,	///< turn left
  KEY_RI,	///< turn right
  KEY_ML,	///< move left
  KEY_MR,	///< move right
  KEY_MD,	///< down backward
  KEY_MU,	///< down forward
  KEY_HO,	///< stand up
  KEY_DO,	///< move down
  KEY_UP,	///< move up
  KEY_TL,	///< tilt left
  KEY_TR,	///< tilt right
  KEY_SP,	///< speeder
  MAXKEYS	///< max of keys
};


void changeKey(int key_id, bool pressed, time_t s, time_t u);
/** pressed is TRUE for a Key Press and FALSE for a Key Release */

void gotoFront(class Object *po, void *d, time_t s, time_t u);
/** Moves the user in front of the object */

void gotoBehind(class Object *po, void *d, time_t s, time_t u);
/** Moves the user behind the object */

void gotoXYZ(float gox,float goy,float goz,float az);
/** Move the user to the point */


#endif
