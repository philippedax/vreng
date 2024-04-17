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
// axis.hpp
//---------------------------------------------------------------------------
#ifndef AXIS_HPP
#define AXIS_HPP


/**
 * Axis class
 */
class Axis {
 private:
  static const GLfloat DEF_GLYPH;
  static const GLfloat DEF_WIDTH;

  bool visible;		///< flag visible or not
  GLint dlist;		///< gl display list
  GLfloat glyph;	///< axis glyph

 public:
  Axis();
  Axis(GLfloat glyph);
  /**< Constructors */

  virtual ~Axis() {};
  /**< Destructor */

  virtual void render();
  /**< Renders axis */

  static Axis * axis();
  /**< Returns axis instance */

  void toggle();

  void reset();

  void init();

 private:
  void draw();		///< draws axis
  void defaults();	///< sets default values
};

#endif
