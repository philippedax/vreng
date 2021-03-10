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
#include "text.hpp"	// Text
#include "world.hpp"	// current
#include "user.hpp"	// localuser
#include "solid.hpp"	// getDlist


const OClass Bubble::oclass(BUBBLE_TYPE, "Bubble", NULL);

const float Bubble::BUBBLETTL = 15.;	// 15 sec
const float Bubble::BUBBLESCALE = 1/4.;	// bubble scale

// local
static uint16_t oid = 0;


void Bubble::defaults()
{
  verso = false;        // recto
  dlists[0] = 0;
  dlists[1] = 0;
  dlists[2] = 0;
  scale = BUBBLESCALE;
  mess = NULL;
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
  char s[256];

  float r = MAX(shifty, 2*Text::GLYPHSIZ);
  float a = 0.4;	// transparency

  // bubble glob (obloid)
  sprintf(s, "solid shape=\"sphere\" rel=\"0 0 0 0 0\" r=\"%f\" sx=\"2.5\" sy=\".6\" sz=\".1\" dif=\"pink\" a=%f />", r, a);
  parse()->parseSolid(s, SEP, this);
  //dax2 dlists[0] = getSolid()->getDlist();

  // arrow
  sprintf(s, "solid shape=\"cone\" rel=\"0 0 -.0.4 0 1.57\" rb=\".03\" rt=\"0\" h=\".15\" sy=\".2\" dif=\"pink\" a=%f />", a);
  parse()->parseSolid(s, SEP, this);
  //dax2 dlists[1] = getSolid()->getDlist();
}

void Bubble::behavior()
{
  enableBehavior(DYNAMIC);      // dynamicaly introduced
  enableBehavior(NO_BBABLE);
  setRenderPrior(PRIOR_MEDIUM);

  initMobileObject(BUBBLETTL);
  initImposedMovement(BUBBLETTL);		// alive delay
  createPermanentNetObject(PROPS, ++oid);
}

/** Created by user */
Bubble::Bubble(User *user, char *_text, float *_color, bool _side)
{
  defaults();
  side = _side;
  text = new char[strlen(_text)+1];
  strcpy(text, _text);
  shifty = (strlen(text)+1) * Text::GLYPHSIZ /2;
  shiftz = -0.25;	// -25 cm / -2 cm
  for (int i=0; i<3; i++)
    color[i] = _color[i];

  makeSolid();
  setPosition();
  state = ACTIVE;
  behavior();
  updatePosition();

#if 1 //dax3
  mess = new Text(text, pos, scale, color);
  if (mess)
    mess->setPos(pos.x, pos.y + shifty, pos.z + shiftz, pos.az, pos.ax);
#else
  Text::loadFont();		// method in parent Text
#endif
}

void Bubble::updateTime(time_t sec, time_t usec, float *lasting)
{
  if (! updateLasting(sec, usec, lasting)) {
    /* the text has spent its live time, it must be destroyed */
    toDelete(); // delete Bubble
    if (mess) {
      mess->toDelete();
    }
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

void Bubble::render()
{
  if (state == INACTIVE) return;

  if (dlists[0]) {      // is bubble present ?
    //dax2 error("dlists = %d %d %d", dlists[0], dlists[1], dlists[2]);
    glPushMatrix();
     glRotatef(RAD2DEG(pos.az), 0, 0, 1);
     glTranslatef(pos.x, pos.y, pos.z);
     glDepthMask(GL_FALSE);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     glCallList(dlists[0]);     // display bubble glob
     glCallList(dlists[1]);     // display bubble arrow
     glCallList(dlists[2]);     // display bubble text

     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
    glPopMatrix();
  }
}

void Bubble::quit()
{
  oid = 0;
}

void Bubble::funcs() {}
