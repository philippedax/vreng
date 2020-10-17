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
/*
 *  pitch.cpp : pitch gadget functions for the VREng GUI
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
#include "user.hpp"	// UserAction
#include "widgets.hpp"
#include "pitch.hpp"
#include "theme.hpp"
#include "wobject.hpp"
#include "user.hpp"	// localuser


Pitch::Pitch(Widgets* _gw, GLint _radius)  // 25
: gw(*_gw), is_drawing(false)
{
  circle_radius = _radius;
  GLint w = (GLint) circle_radius + 6;
  GLint h = (GLint) circle_radius*2 + 4;
  canvas_size.set(w, h);
  arrow_point.set(w, h/2.);
  current_radius = 2;
  delta = 0;
  angle = 0;

  add
  (canvas_size
   + UOn::paint       / ucall(this, &Pitch::paintCB)
   + UOn::mdrag       / ucall(this, &Pitch::dragCB)
   + UOn::doubleClick / ucall(this, &Pitch::doubleClickCB)
   );
}

void Pitch::doAction()
{
  if (! localuser) return;
  struct timeval t;
  gettimeofday(&t, NULL);
  GLfloat val[1];
  val[0] = angle;
  localuser->specialAction(UserAction::UA_SETPITCH, (void *)val, t.tv_sec, t.tv_usec);
}


void Pitch::dragCB(UMouseEvent &e)
{
  GLfloat dx = e.getX() - arrow_point.x;
  GLfloat dy = arrow_point.y - e.getY();

  current_radius = sqrt(dx * dx + dy * dy);
  delta = asin(dy / current_radius);

  if (dx <= 0) {
    if (dy < 5 && dy > -5) delta = 0;
  }
  else {
    if (dy >= 0) delta = M_PI/2; else delta = -M_PI/2;
  }
  angle = 180 * delta / M_PI;

  doAction();
  repaint();
}


void Pitch::doubleClickCB(UMouseEvent &e)
{
  delta = 0;
  angle = 0;
  doAction();
  repaint();
}


void Pitch::paintCB(UPaintEvent &e)
{
  UGraph g(e);
  g.setColor(::g.theme.joystickColor);
  g.setWidth(2);

  g.drawLine(1, 1, 1, arrow_point.y-2);
  g.drawLine(1, arrow_point.y+2, 1, 2*arrow_point.y-1);
  g.drawLine(0, arrow_point.y, 2, arrow_point.y);

  g.setColor(::g.theme.joystickArmColor);
  g.drawLine(arrow_point.x, arrow_point.y,
             arrow_point.x - circle_radius * cos(delta),
             arrow_point.y - circle_radius * sin(delta));

  g.setColor(::g.theme.joystickColor);
  g.drawLine(arrow_point.x, arrow_point.y,
             arrow_point.x - 12*cos(delta+(M_PI*20/180)),
             arrow_point.y - 12*sin(delta+(M_PI*20/180)));
  g.drawLine(arrow_point.x, arrow_point.y,
             arrow_point.x - 12*cos(delta-(M_PI*20/180)),
             arrow_point.y - 12*sin(delta-(M_PI*20/180)));
}
