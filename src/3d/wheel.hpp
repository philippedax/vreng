//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
#ifndef WHEEL_HPP
#define WHEEL_HPP

/**
 * Wheel class
 */
class Wheel {

private:
  GLint wheel_dlist;
  GLint needle_dlist;
  float needle_angle;

public:
  Wheel(uint8_t teeth, GLfloat r, const GLfloat *color); ///< constructor.
  Wheel(); ///< constructor.

  virtual ~Wheel();	///< destructor.

  static Wheel * current();	///< accessor.

  virtual void render();
  /**< Renders wheel */

private:
  virtual void defaults();
  /**< Sets default values */

  virtual void draw(uint8_t teeth, GLfloat r, const GLfloat *color);
  /**< Draws wheel */

  virtual void spoke(GLfloat r);
  virtual void dial(GLfloat a, GLfloat r, const GLfloat *color);
  virtual void needle(uint8_t teeth, GLfloat r);
  virtual void needle_displaylist(uint8_t teeth, GLfloat r);
  virtual void displaylist(uint8_t teeth, GLfloat r, const GLfloat *color);

};


#endif
