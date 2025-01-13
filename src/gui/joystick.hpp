//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
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
// joystick.hpp
//---------------------------------------------------------------------------
#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP


class Widgets;

/**
 * Joystick1 class
 */
class Joystick1 : public UBox {
 public:
  Joystick1(Widgets* gw, GLint radius);

 private:
  Widgets& gw;
  UBox  canvas;
  USize canvas_size;

  bool  is_drawing;
  GLfloat radius;
  UPoint center_point, current_point;

  Navig * navig();

  // CallBacks
  void paintCanvasCB(UPaintEvent&);
  void pressCanvasCB(UMouseEvent&);
  void dragCanvasCB(UMouseEvent&);
  void releaseCanvasCB(UMouseEvent&);
};

/**
 * Joystick2 class
 */
class Joystick2 : public UBox {
 public:
  Joystick2(Widgets* gw, GLint radius);

 private:
  UBox  canvas;
  USize canvas_size;

  GLfloat delta;
  GLfloat angle;
  GLfloat circle_radius, current_radius;
  UPoint arrow_point;

  void doAction();

  // CallBacks
  void paintCB(UPaintEvent&);
  void dragCB(UMouseEvent&);
  void doubleClickCB(UMouseEvent&);
};

#endif
