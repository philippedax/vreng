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


const OClass Bubble::oclass(BUBBLE_TYPE, "Bubble", NULL);

const float Bubble::BUBBLETTL = 15.;	// 15 sec
const float Bubble::BUBBLESCALE = 1/4.;	// bubble scale

// local
static uint16_t oid = 0;


void Bubble::defaults()
{
  face = false;        // recto
  scale = BUBBLESCALE;
  bubtext = NULL;
  text = NULL;
  state = INACTIVE;
  strcpy(names.url, DEF_URL_FONT);      // font
}

/* position above the avatar's head */
void Bubble::setPosition()
{
  //dax pos.z  = localuser->pos.z + localuser->height/2 + 0.10;
  pos.z  = localuser->height + 0.10;
  pos.x  = localuser->pos.x;
  pos.ax = M_PI_2;	// vertical
  if (face == BUBBLEBACK)
    pos.y  = localuser->pos.y;
  else
    pos.y  = -localuser->pos.y;
  if (face == BUBBLEBACK)
    pos.az = localuser->pos.az - M_PI_2;
  else
    pos.az = localuser->pos.az + M_PI_2;
}

/** Make translucid bubble */
void Bubble::makeSolid()
{
  char s[256];

  float r = MAX((strlen(text)+1) * Text::GLYPHSIZ / 2.5, 2*Text::GLYPHSIZ); // A REVOIR !!!
  float a = 0.3;	// transparency

  // bubble glob (obloid)
  sprintf(s, "solid shape=\"sphere\" rel=\"0 0 0 0 0\" r=\"%f\" sx=\"2.5\" sy=\".6\" sz=\".1\" dif=\"pink\" a=%f />", r, a);
  parse()->parseSolid(s, SEP, this);

  // arrow
  sprintf(s, "solid shape=\"cone\" rel=\"0 0 +.1.0 0 1.57\" rb=\".03\" rt=\"0\" h=\".15\" sy=\".2\" dif=\"pink\" a=%f />", a);
  parse()->parseSolid(s, SEP, this);
}

void Bubble::behavior()
{
  enableBehavior(DYNAMIC);      // dynamicaly introduced
  enableBehavior(NO_BBABLE);

  initMobileObject(BUBBLETTL);
  initImposedMovement(BUBBLETTL);		// alive delay
  createPermanentNetObject(PROPS, ++oid);
}

/** Created by user */
Bubble::Bubble(User *user, char *_text, const float *_color, bool _face)
{
  defaults();
  face = _face;
  text = strdup(_text);
  setPosition();

  // adjusting text
  Pos postext = pos;
  postext.y += (strlen(_text)+2) * Text::GLYPHSIZ / 2;
  postext.z += -0.02;	// -2 cm
  //error("text : %.2f %.2f %.2f",postext.x,postext.y,postext.z);

  makeSolid();
  state = ACTIVE;
  behavior();
  updatePosition();

  bubtext = new Text(_text, postext, scale, _color, _face);
  if (bubtext)
    bubtext->setPos(postext.x, postext.y, postext.z, postext.az, postext.ax);

#if 0 //dax3
  Text::loadFont();		// method in parent Text
#endif
}

void Bubble::updateTime(time_t sec, time_t usec, float *lasting)
{
  if (! updateLasting(sec, usec, lasting)) {
    /* the text has spent its live time, it must be destroyed */
    toDelete(); 	// delete Bubble
    if (bubtext) {
      bubtext->toDelete();	// delete text inside Bubble
    }
    localuser->resetBubble();
    state = INACTIVE;
  }
}

void Bubble::changePosition(float lasting)
{
  //dax if (state == ACTIVE) setPosition();
}

bool Bubble::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Bubble::render()
{
  if (state == INACTIVE) return;

  //dax if (dlists[0]) {      // is bubble present ?
    //dax2 error("dlists = %d %d %d", dlists[0], dlists[1], dlists[2]);
    glPushMatrix();
     glRotatef(RAD2DEG(pos.az), 0, 0, 1);
     glTranslatef(pos.x, pos.y, pos.z);
     glDepthMask(GL_FALSE);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     //dax glCallList(dlists[0]);     // display bubble glob
     //dax glCallList(dlists[1]);     // display bubble arrow
     //dax glCallList(dlists[2]);     // display bubble text

     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
    glPopMatrix();
  //dax }
}

void Bubble::quit()
{
  oid = 0;
  if (text)
    free(text);
}

void Bubble::funcs() {}
