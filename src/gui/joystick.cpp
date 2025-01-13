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
//
//  joystick.cpp : joystick for the VREng GUI
//
//  VREng Project / Ubit GUI
//  Authors: Eric Lecolinet and Philippe Dax
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "widgets.hpp"	// gw
#include "joystick.hpp"
#include "navig.hpp"	// getNavig
#include "motion.hpp"	// Motion
#include "theme.hpp"	// Theme
#include "user.hpp"	// User


/** Constructor */
Joystick1::Joystick1(Widgets* _gw, GLint _radius) :
 gw(*_gw),
 is_drawing(false),
 radius(_radius)
{
  canvas_size.set(radius*2, radius*2);
  center_point.set(radius, radius);

  canvas.addAttr(canvas_size
                 + UOn::paint    / ucall(this, &Joystick1::paintCanvasCB)
                 + UOn::mdrag    / ucall(this, &Joystick1::dragCanvasCB)
                 + UOn::mpress   / ucall(this, &Joystick1::pressCanvasCB)
                 + UOn::mrelease / ucall(this, &Joystick1::releaseCanvasCB)
                );
  add(UOrient::horizontal + upadding(1,0) + UFont::small
      + uvbox(uhcenter()
              + uitem(UOn::deselect / g.theme.joystickColor
                      + UOn::select / g.theme.joystickArmColor
                      + "Lock"
                      + utip("Set gravity on/off")
                      + ucall(int(User::UA_PAUSE), Widgets::action)
                     ).setSelectable()
              + uitem(g.theme.JoystickUpDown
                      + utip("Drag on the Y axis to move verticaly")
                      + UOn::arm      / ucall(navig(), (Motion*)0, &Motion::u_trans_z, &Navig::userMotion)
                      + UOn::mdrag    / ucall(navig(), &Navig::doMotion)
                      + UOn::mrelease / ucall(navig(), &Navig::stopMotion)
                     )
             )
      + uvbox(
              canvas
              + uright()
              + uitem(g.theme.JoystickLeftRight
                      + utip("Drag on the X axis to move lateraly")
                      + UOn::arm      / ucall(navig(), &Motion::u_trans_x, (Motion*)0, &Navig::userMotion)
                      + UOn::mdrag    / ucall(navig(), &Navig::doMotion)
                      + UOn::mrelease / ucall(navig(), &Navig::stopMotion)
                     )
             )
     );
}

Navig * Joystick1::navig()
{
  return gw.getNavig();
}

/** Press into the joystick */
void Joystick1::pressCanvasCB(UMouseEvent& e)
{
  navig()->userMotion(e, &Motion::u_rot_z, &Motion::u_trans_y);
  current_point.set(e.getX(), e.getY());
  is_drawing = true;
  repaint();		// uwin
}

/** Drags into the joystick */
void Joystick1::dragCanvasCB(UMouseEvent& e)
{
  navig()->doMotion(e);
  current_point.set(e.getX(), e.getY());
  repaint();		// uwin
}

/** Releases into the joystick */
void Joystick1::releaseCanvasCB(UMouseEvent& e)
{
  navig()->stopMotion();
  current_point.set(e.getX(), e.getY());
  is_drawing = false;
  repaint();		// uwin
}

/** Points into the joystick */
void Joystick1::paintCanvasCB(UPaintEvent& e)
{
  UGraph g(e);
  g.setColor(::g.theme.joystickColor);

  // draw center point
  g.fillEllipse(center_point.x-5, center_point.y-5, 10, 10);

  // draw circle
  g.drawEllipse(0, 0, 2*radius, 2*radius);

  if (is_drawing) {
    g.setColor(::g.theme.joystickArmColor);

    float dx = current_point.x - center_point.x;
    float dy = current_point.y - center_point.y;
    float r = sqrt(dx*dx + dy*dy);
    float a = dx == 0 ? 0 : atan(dy / dx);
    float sign = dx > 0 ? +1 : -1;
    if (r > radius) r = radius;
    g.drawLine(center_point.x,
               center_point.y,
               center_point.x + sign * r * cos(a),
               center_point.y + sign * r * sin(a)
              );
  }
}

//---------------------------------------------------------------------------

/** Constructor for joystick2 */
Joystick2::Joystick2(Widgets* _gw, GLint _radius)
{
  circle_radius = _radius;
  GLint w = (GLint) circle_radius + 6;
  GLint h = (GLint) circle_radius*2 + 4;
  canvas_size.set(w, h);
  arrow_point.set(w, h/2.);
  current_radius = 2;
  delta = 0;
  angle = 0;

  add(canvas_size
      + UOn::paint       / ucall(this, &Joystick2::paintCB)
      + UOn::mdrag       / ucall(this, &Joystick2::dragCB)
      + UOn::doubleClick / ucall(this, &Joystick2::doubleClickCB)
     );
}

/** Do an action */
void Joystick2::doAction()
{
  if (! localuser)  return;

  struct timeval t;
  gettimeofday(&t, NULL);
  GLfloat val[1];
  val[0] = angle;
  localuser->specialAction(User::UA_SETPITCH, (void *)val, t.tv_sec, t.tv_usec);
}

/** Drags joystick2 */
void Joystick2::dragCB(UMouseEvent &e)
{
  GLfloat dx = e.getX() - arrow_point.x;
  GLfloat dy = arrow_point.y - e.getY();

  current_radius = sqrt(dx*dx + dy*dy);
  delta = asin(dy / current_radius);

  if (dx <= 0) {
    if (dy < 5 && dy > -5)
      delta = 0;
  }
  else {
    if (dy >= 0)
      delta = M_PI/2;
    else
      delta = -M_PI/2;
  }
  angle = delta * (180/M_PI);

  doAction();
  repaint();	// uwin
}

/** Doubleclick */
void Joystick2::doubleClickCB(UMouseEvent &e)
{
  delta = 0;
  angle = 0;
  doAction();
  repaint();	// uwin
}

/** Paints joystick2 */
void Joystick2::paintCB(UPaintEvent &e)
{
  UGraph g(e);
  g.setColor(::g.theme.joystickColor);
  g.setWidth(2);

  g.drawLine(1, 1, 1, arrow_point.y-2);
  g.drawLine(1, arrow_point.y+2, 1, 2*arrow_point.y-1);
  g.drawLine(0, arrow_point.y, 2, arrow_point.y);

  g.setColor(::g.theme.joystickArmColor);
  g.drawLine(arrow_point.x,
             arrow_point.y,
             arrow_point.x - circle_radius * cos(delta),
             arrow_point.y - circle_radius * sin(delta));

  g.setColor(::g.theme.joystickColor);
  g.drawLine(arrow_point.x,
             arrow_point.y,
             arrow_point.x - 12*cos(delta+(M_PI*20/180)),
             arrow_point.y - 12*sin(delta+(M_PI*20/180)));
  g.drawLine(arrow_point.x,
             arrow_point.y,
             arrow_point.x - 12*cos(delta-(M_PI*20/180)),
             arrow_point.y - 12*sin(delta-(M_PI*20/180)));
}
