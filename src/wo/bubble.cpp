//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#include "vreng.hpp"
#include "bubble.hpp"
#include "txf.hpp"	// TexFont
#include "world.hpp"	// current
#include "user.hpp"	// localuser


const OClass Bubble::oclass(BUBBLE_TYPE, "Bubble", NULL);

const float Bubble::BUBBLETTL = 60.;	// 1 min
const float Bubble::BUBBLESCALE = 1/4.;	// bubble scale

// local
static uint16_t oid = 0;


void Bubble::defaults()
{
  verso = false;        // recto
  loaded = false;
  txf = NULL;
  scale = BUBBLESCALE;
  state = INACTIVE;
  shiftx = shifty = shiftz = 0.;
  strcpy(names.url, DEF_URL_FONT);      // font
  color[0] = 1; color[1]=color[2] = 0;	// red
}

/* position above the avatar's head */
void Bubble::setPosition()
{
                          pos.z  = localuser->pos.z + localuser->height/2 + 0.10;
                          pos.x  = localuser->pos.x;
                          pos.ax = M_PI_2; // vertical
  if (side == BUBBLEBACK) pos.y  = localuser->pos.y;
  else                    pos.y  = localuser->pos.y;
  if (side == BUBBLEBACK) pos.az = localuser->pos.az - M_PI_2;
  else                    pos.az = localuser->pos.az + M_PI_2;
}

/** Make translucid bubble */
void Bubble::makeSolid()
{
  char s[BUFSIZ];
  float r = MAX(shifty, 2*Text::GLYPHSIZ);

  // obloid
  sprintf(s, "solid shape=\"sphere\" rel=\"0 0 0 0 0\" r=\"%f\" sx=\"2.5\" sy=\".4\" sz=\".5\" dif=\"pink\" a=\".3\" />", r);
  parse()->parseSolid(s, SEP, this);
  // arrow
  sprintf(s, "solid shape=\"cone\" rel=\"0 0 -.1 0 1.57\" rb=\".03\" rt=\"0\" h=\".1\" sy=\".3\" dif=\"pink\" a=\".3\" />");
  parse()->parseSolid(s, SEP, this);
}

void Bubble::behavior()
{
  enableBehavior(DYNAMIC);      // dynamicaly introduced
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(SPECIFIC_RENDER);
  setRenderPrior(RENDER_HIGH);

  initializeMobileObject(BUBBLETTL);
  initImposedMovement(BUBBLETTL);
  createPermanentNetObj(PROPS, ++oid);
}

/** Created by user */
Bubble::Bubble(User *user, char *_text, float *_color, bool _side)
{
  defaults();
  side = _side;
  text = new char[strlen(_text)+1];
  strcpy(text, _text);
  for (int i=0; i<3; i++) color[i] = _color[i];
  shifty = ((strlen(text)+1)/2)*Text::GLYPHSIZ;
  shiftz = -0.02; // -2 cm

  makeSolid();
  setPosition();
  state = ACTIVE;
  behavior();
  updatePosition();
  loadFont();
}

void Bubble::updateTime(time_t sec, time_t usec, float *lasting)
{
  if (! updateLasting(sec, usec, lasting)) {
    /* the text has spent its live time, it must be destroyed */
    toDelete(); // delete Bubble
    localuser->resetBubble();
    state = INACTIVE;
  }
}

void Bubble::changePosition(float lasting)
{
  if (state == ACTIVE) setPosition();
}

bool Bubble::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Bubble::quit()
{
  oid = 0;
}

void Bubble::funcs() {}
