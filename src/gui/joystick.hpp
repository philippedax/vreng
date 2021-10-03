//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP


class Widgets;

/**
 * Joystick classes
 */
class Joystick1 : public UBox {
public:
  Joystick1(Widgets* gw, GLint radius);

private:
  Widgets& gw;
  bool  is_drawing;
  GLfloat radius;
  UBox  canvas;
  USize canvas_size;
  UPoint center_point, current_point;

  void paintCanvasCB(UPaintEvent&);
  void pressCanvasCB(UMouseEvent&);
  void dragCanvasCB(UMouseEvent&);
  void releaseCanvasCB(UMouseEvent&);
};

class Joystick2 : public UBox {
public:
  Joystick2(Widgets* gw, GLint radius);

private:
  bool  is_drawing;
  UBox  canvas;
  USize canvas_size;

  GLfloat delta;
  GLfloat angle;
  GLfloat circle_radius, current_radius;
  UPoint arrow_point;

  void doAction();
  void paintCB(UPaintEvent&);
  void dragCB(UMouseEvent&);
  void doubleClickCB(UMouseEvent&);
};

#endif
