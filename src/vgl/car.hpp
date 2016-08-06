//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef CAR_HPP
#define CAR_HPP


/**
 * Car model
 */
class Car {

public:
  static void draw(GLfloat width, GLfloat depth, GLfloat height, const int textures[], GLfloat rtex[6][2], uint8_t slices, uint8_t style);
  /**< Draws a car */

private:
  static void car(GLfloat adir, const int textures[], GLfloat rtex[6][2], uint8_t slices, uint8_t style);
  static void rouederdte(GLfloat r, int p);
  static void rouedergche(GLfloat r, int p);
  static void roueavtdte(GLfloat r, GLfloat adir, int p);
  static void roueavtgche(GLfloat r, GLfloat adir, int p);
  static void roue(GLfloat r, int normale, int p);
  //static void roue(GLfloat r, int normale, int p);
  static void roue(GLfloat r, int p);
  static void cylindre(GLfloat height, GLfloat radius, int precision);
};


#endif
