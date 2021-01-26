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
/* Copyright (c) Mark J. Kilgard, 1994. */
/**
 * (c) Copyright 1993, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

#include "vreng.hpp"
#include "stars.hpp"


const OClass Stars::oclass(STARS_TYPE, "Stars", Stars::creator);
const uint16_t Stars::NUMBER = 200;
const uint16_t Stars::MAXPOS = 4000;
const float Stars::SPEED = 10.;		///< 10 m/s
const float Stars::WARP = 20.;

void Stars::funcs() {}


void Stars::create(int n, int d)
{
  stars[n].x[0] = (float) (rand() % maxpos - maxpos/2);
  stars[n].y[0] = (float) (rand() % maxpos - maxpos/2);
  stars[n].v[0] = (float) (rand() % maxpos /*+ d*/);
  stars[n].x[1] = stars[n].x[0];
  stars[n].y[1] = stars[n].y[0];
  stars[n].v[1] = stars[n].v[0];
  if (rand() % 10 == 0) {
    stars[n].dx = (float) (rand() % 100 - 100/2);
    stars[n].dy = (float) (rand() % 100 - 100/2);
    stars[n].dr = (float) (rand() % 25 - 25/2);
  } else {
    stars[n].dx = 0;
    stars[n].dy = 0;
    stars[n].dr = 0;
  }
}

void Stars::rotate(float *x, float *y, float rot)
{
  float tx = *x * cos(rot) - *y * sin(rot);
  float ty = *y * cos(rot) + *x * sin(rot);
  *x = tx;
  *y = ty;
}

void Stars::move()
{
  for (int n=0; n < number; n++) {
    stars[n].x[1] = stars[n].x[0];
    stars[n].y[1] = stars[n].y[0];
    stars[n].v[1] = stars[n].v[0];
    stars[n].x[0] += stars[n].dx;
    stars[n].y[0] += stars[n].dy;
    stars[n].v[0] -= speed;
    stars[n].rot += stars[n].dr;
    if (stars[n].rot >= 360) stars[n].rot = 0.;
  }
}

bool Stars::pointed(int n)
{
  float x0 = stars[n].x[0] * 2*pos.x / stars[n].v[0];
  float y0 = stars[n].y[0] * 2*pos.y / stars[n].v[0];

  rotate(&x0, &y0, stars[n].rot);
  x0 += 2*pos.x;
  y0 += 2*pos.y;
  if (x0 < 2*pos.x && y0 < 2*pos.y) return true;
  else return false;
}

void Stars::show(int n)
{
  GLfloat color[4] = {1,1,1,1};

  float x0 = stars[n].x[0] * 2*pos.x / stars[n].v[0];
  float y0 = stars[n].y[0] * 2*pos.y / stars[n].v[0];
  rotate(&x0, &y0, stars[n].rot);
  x0 += 2*pos.x;
  y0 += 2*pos.y;

  if (x0 < 2*pos.x && y0 < 2*pos.y) {
    float x1 = stars[n].x[1] * 2*pos.x / stars[n].v[1];
    float y1 = stars[n].y[1] * 2*pos.y / stars[n].v[1];
    rotate(&x1, &y1, stars[n].rot);
    x1 += 2*pos.x;
    y1 += 2*pos.y;

    //glLineWidth(maxpos / 100. / stars[n].v[0] + 1);

    //color[1] = (float) ((warp - speed) / warp);
    //color[2] = (float) ((warp - speed) / warp);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    if (fabs(x0 - x1) < 1. && fabs(y0 - y1) < 1.) {
      glBegin(GL_POINTS);
      glVertex2f(x0, y0);
      glEnd();
    } else {
      glBegin(GL_LINES);
      glVertex2f(x0, y0);
      glVertex2f(x1, y1);
      glEnd();
    }
  }
}

void Stars::update()
{
  for (int n=0; n < number; n++) {
    if (stars[n].v[0] > speed || (stars[n].v[0] > 0 && speed < warp)) {
      if (! pointed(n)) create(n, maxpos);
    }
    else create(n, maxpos);
  }
}

WObject * Stars::creator(char *l)
{
  return new Stars(l);
}

void Stars::defaults()
{
  number = NUMBER;
  maxpos = MAXPOS;
  speed = SPEED;
  warp = WARP;
}

void Stars::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse (l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "number")) l = parse()->parseUInt16(l, &number, "number");
    else if (!stringcmp(l, "speed")) l = parse()->parseFloat(l, &speed, "speed");
    else if (!stringcmp(l, "warp")) l = parse()->parseFloat(l, &warp, "warp");
    else if (!stringcmp(l, "maxpos")) l = parse()->parseUInt16(l, &maxpos, "maxpos");
  }
  end_while_parse(l);
}

Stars::Stars(char *l)
{
  parser(l);

  if (maxpos <= 0) maxpos = MAXPOS;
  if (warp <= speed) warp = WARP;

  stars = new tStar[number];
  srand((uint32_t) time(NULL));

  for (int n=0; n < number; n++) create(n, 100);

  enableBehavior(PARTICLE);
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(PRIOR_LOW);

  initMobileObject(0);
  enablePermanentMovement();
}

void Stars::changePermanent(float lasting)
{
  move();
  update();
}

void Stars::render()
{
  //DAX glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushMatrix();
  glTranslatef(0, 0, pos.z);
  glRotatef(RAD2DEG(pos.az), 0, 0, 1);
  glRotatef(RAD2DEG(pos.ax), 1, 0, 0);

  for (int n=0; n < number; n++) {
    if (stars[n].v[0] > speed || (stars[n].v[0] > 0 && speed < warp))
      show(n);
  }
  glPopMatrix();
  //DAX glPopAttrib();
}

void Stars::quit()
{
  if (stars) delete[] stars;
}
