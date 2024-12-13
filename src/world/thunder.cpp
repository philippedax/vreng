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
const uint16_t Thunder::PERIOD = 10;


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

/** Parses a line */
void Thunder::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &number, "number");
    else if (! stringcmp(l, "period")) l = parseUInt16(l, &period, "period");
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

/** Draws segments */
void Thunder::draw()
{
  for (int i=0; i<4; i++) color[i] = 1;	// white

  dlist = glGenLists(number);
  for (int n=0; n < number; n++) {
    s_point pt;

    srand(time(NULL));

    int sign = rand()%2 - 1;
    sign = (sign < 0) ? -1 : 1;	// [-1,1]

    int segs = rand()%12 + 12;	// [12..24]
    // starting point
    pt.x = pos.x + ((rand()%10) - 5);	// xorig +- 5
    pt.y = pos.y + ((rand()%40) - 20);	// yorig +- 20
    pt.z = pos.z + ((rand()%10) - 5);	// zorig +- 5
    // increments
    inc.x = pt.x / segs;
    inc.y = pt.y / segs;
    inc.z = (pt.z + 10) / segs;

    glNewList(dlist + n, GL_COMPILE);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i < segs; i++) {
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

/** Makes changes */
void Thunder::changePermanent(float lasting)
{
  if ((rand()%period) == 0) {	// [0..PERIOD]
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
    state = false;	// canceled
  }
}

/** Renders thunder */
void Thunder::render()
{
  if (!state || dlist <= 0) return;

  int w = rand()%6 + 1;		// thickness [1..6]
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

  for (int n=0; n < number; n++) {
    //glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    glPushMatrix();
    glLineWidth(w);

    glCallList(dlist + n);	// display segments

    glPopMatrix();
    //glPopAttrib();
  }
  glLineWidth(1);
}

void Thunder::funcs() {}
