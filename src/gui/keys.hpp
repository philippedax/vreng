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
#ifndef KEYS_HPP
#define KEYS_HPP

/* Keys needed by VREng move module */
enum {
  KEY_AV,	///< move forward left
  KEY_AR,	///< move backward right
  KEY_DR,	///< turn right
  KEY_GA,	///< turn left
  KEY_SD,	///< move forward right
  KEY_SG,	///< move backward left
  KEY_MT,	///< down backward
  KEY_DE,	///< down forward
  KEY_HZ,	///< stand up
  KEY_JD,	///< move down
  KEY_JU,	///< move up
  KEY_TL,	///< tilt left
  KEY_TR,	///< tilt right
  KEY_VI,	///< speeder
  MAXKEYS	///< max of keys
};

#endif
