//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// thunder.cpp
//
// Thunder effect
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "thunder.hpp"


const OClass Thunder::oclass(THUNDER_TYPE, "Thunder", Thunder::creator);

const uint16_t Thunder::NUMBER = 1;
const float Thunder::PERIOD = 2;

void Thunder::funcs() {}


WO * Thunder::creator(char *l)
{
  return new Thunder(l);
}

void Thunder::defaults()
{
  number = NUMBER;
  period = PERIOD;
  state = false;
  dlist = -1;
}

void Thunder::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &number, "number");
    else if (! stringcmp(l, "period")) l = parseFloat(l, &period, "period");
  }
  end_while_parse(l);
}

void Thunder::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);
}

void Thunder::inits()
{
  initEphemeralObject(0);

  draw();
}

void Thunder::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"none\" />");
  parseSolid(s);
}

Thunder::Thunder(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
}

void Thunder::draw()
{
  for (int i=0; i<4; i++) color[i] = 1;	// white

  dlist = glGenLists(number);
  for (int n=0; n < number; n++) {
    s_point pt;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    srand((time_t) tv.tv_usec);

    int sign = rand()%2 - 1;
    sign = (sign < 0) ? -1 : 1;	// [-1,1]

    int div = rand()%12 + 12;	// [12..24]
    // starting point
    pt.x = pos.x + ((rand()%10) - 5);	// xorig +- 5
    pt.y = pos.y + ((rand()%40) - 20);	// yorig +- 20
    pt.z = pos.z + ((rand()%10) - 5);	// zorig +- 5
    // increments
    inc.x = pt.x / div;
    inc.y = pt.y / div;
    inc.z = (pt.z + 10) / div;

    glNewList(dlist + n, GL_COMPILE);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i < div; i++) {
      glVertex3f(pt.x, pt.y, pt.z);
      // next point
      pt.x += (sign * (inc.x + ((rand()%6) - 3)));
      pt.y += (sign * (inc.y + ((rand()%6) - 3)));
      //pt.z -= inc.z + ((rand()%2) - 1);
      pt.z -= inc.z; // fall down
    }
    glEnd();
  }
  glEndList();
}

void Thunder::changePermanent(float lasting)
{
  if ((rand()%20) == 0) {	// [0..19]
    if (dlist > 0) {
      glDeleteLists(dlist, number);
      dlist = -1;
    }

    draw();

    // color
    int rgb = rand()%3;	// [Ø..2]
    if (rand()%2) {	// one over two is white
      color[rgb] = 0.6;
      rgb = rand()%3;
      color[rgb] = 0.6;
    }
    state = true;	// to display
  }
  else {
    state = false;
  }
}

void Thunder::render()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  if ( static_cast<time_t>(tv.tv_sec) % int(period) != 0 ) return;
  if (!state || dlist <= 0) return;

  for (int n=0; n < number; n++) {
    int wid = rand()%6 + 1;	// thickness [1..6]
    glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glLineWidth(wid);

    glCallList(dlist + n);	// display

    glLineWidth(1);
    glPopMatrix();
    glPopAttrib();
  }
}

void Thunder::quit()
{}
