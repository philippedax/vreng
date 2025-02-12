//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// car.hpp
//---------------------------------------------------------------------------
#ifndef CAR_HPP
#define CAR_HPP


/**
 * Car class
 */
class Car {
 private:
  void wheelbackright(GLfloat r, int p);
  void wheelbackleft(GLfloat r, int p);
  void wheelfronttright(GLfloat r, int p);
  void wheelfronttleft(GLfloat r, int p);
  void wheel(GLfloat radius, int p);
  void cylindre(GLfloat height, GLfloat radius, int precision);

 public:
  Car() {};		/**< constructor */
  virtual ~Car() {};	/**< destructor */

  void draw(GLfloat width, GLfloat depth, GLfloat height, const int tex[], GLfloat rtex[6][2], uint8_t slices, uint8_t style);
  /**< Draws a car */
};


#endif
