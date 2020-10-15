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
/*
 *  joystick.cpp : joystick for the VREng GUI
 *  NOTE: this file should be common to all GUI variants
 *
 *  VREng Project / Ubit GUI
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  (C) 1999-2009 Eric Lecolinet / ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */
#include <cmath>

#include "vreng.hpp"
#include "widgets.hpp"
#include "joystick.hpp"
#include "navig.hpp"
#include "mvt.hpp"
#include "theme.hpp"
#include "user.hpp"	// UserAction


Joystick::Joystick(Widgets* _gw, int _radius)
: gw(*_gw), is_drawing(false), radius(_radius)
{
  canvas_size.set(radius*2, radius*2);
  center_point.set(radius, radius);
  Navig* navig = gw.getNavig();

  canvas.addAttr(canvas_size
                 + UOn::paint    / ucall(this, &Joystick::paintCanvasCB)
                 + UOn::mdrag    / ucall(this, &Joystick::dragCanvasCB)
                 + UOn::mpress   / ucall(this, &Joystick::pressCanvasCB)
                 + UOn::mrelease / ucall(this, &Joystick::releaseCanvasCB)
                 );

  add(UOrient::horizontal + upadding(1,0) + UFont::small
      + uvbox(uhcenter()
              + uitem(UOn::deselect / g.theme.joystickColor
                      + UOn::select / g.theme.joystickArmColor
                      + "Lock" + utip("Set gravity on/off")
                      + ucall(int(UserAction::UA_PAUSE), Widgets::callAction)
                      ).setSelectable()
              + uitem(g.theme.JoystickUpDown
                      + utip("Drag on the Y axis to move verticaly")
                      + UOn::arm / ucall(navig, (Mvt*)0, &Mvt::ztrans, &Navig::startMotion)
                      + UOn::mdrag / ucall(navig, &Navig::doMotion)
                      + UOn::mrelease / ucall(navig, &Navig::stopMotion)
                      )
              )
      + uvbox(
              canvas
              + uright()
              + uitem(g.theme.JoystickLeftRight
                      + utip("Drag on the X axis to move lateraly")
                      + UOn::arm / ucall(navig, &Mvt::xtrans, (Mvt*)0, &Navig::startMotion)
                      + UOn::mdrag / ucall(navig, &Navig::doMotion)
                      + UOn::mrelease / ucall(navig, &Navig::stopMotion)
                      )
              )
      );
}

void Joystick::pressCanvasCB(UMouseEvent& e)
{
  gw.getNavig()->startMotion(e, &Mvt::zrot, &Mvt::ytrans);
  current_point.set(e.getX(), e.getY());
  is_drawing = true;
  repaint();
}

void Joystick::dragCanvasCB(UMouseEvent& e)
{
  gw.getNavig()->doMotion(e);
  current_point.set(e.getX(), e.getY());
  repaint();
}

void Joystick::releaseCanvasCB(UMouseEvent& e)
{
  gw.getNavig()->stopMotion();
  current_point.set(e.getX(), e.getY());
  is_drawing = false;
  repaint();
}

void Joystick::paintCanvasCB(UPaintEvent& e)
{
  UGraph g(e);
  g.setColor(::g.theme.joystickColor);

  // draw cercle
  g.drawEllipse(0, 0, 2*radius, 2*radius);

  if (!is_drawing) {
    // draw center point
    g.fillEllipse(center_point.x-5, center_point.y-5, 10, 10);
  }
  else {
    g.setColor(::g.theme.joystickArmColor);

    // draw center point
    g.fillEllipse(center_point.x-5, center_point.y-5, 10, 10);

    float dx = current_point.x - center_point.x;
    float dy = current_point.y - center_point.y;
    float r = sqrt(dx * dx + dy * dy);
    float a = dx == 0 ? 0 : atan(dy / dx);
    float sign = dx > 0 ? +1 : -1;
    if (r > radius) r = radius;
    g.drawLine(center_point.x, center_point.y,
               center_point.x + sign * r * cos(a),
               center_point.y + sign * r * sin(a));
  }
}
