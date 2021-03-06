//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
  face = BUBBLERECTO;        // recto
  scale = BUBBLESCALE;
  bubtext = NULL;
  text = NULL;
  state = INACTIVE;
  strcpy(names.url, DEF_URL_FONT);      // font
}

/* position above the avatar's head */
void Bubble::setPosition()
{
  pos.z  = localuser->height + 0.10;
  pos.x  = localuser->pos.x;
  pos.ax = M_PI_2;	// vertical
  if (face == BUBBLEVERSO)
    pos.y  = localuser->pos.y;
  else
    pos.y  = -localuser->pos.y;
  if (face == BUBBLEVERSO)
    pos.az = localuser->pos.az - M_PI_2;
  else
    pos.az = localuser->pos.az + M_PI_2;
  //error("bubble: %.1f %.1f %.1f", pos.x,pos.y,pos.z);
}

/** Make transparent bubble */
void Bubble::makeSolid()
{
  char s[256];

  float r = MAX((strlen(text)+1) * Text::GLYPHSIZ / 2.5, 2*Text::GLYPHSIZ); // A REVOIR !!!
  float a = 0.3;	// transparency

  // bubble glob (obloid)
  sprintf(s, "solid shape=\"sphere\" r=\"%f\" sx=\"2.5\" sy=\".6\" sz=\".1\" dif=\"pink\" a=%f />", r, a);
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
  makeSolid();
  state = ACTIVE;
  behavior();
  updatePosition();

  // adjusting text position
  postext = pos;
  postext.y += (strlen(text)+2) * Text::GLYPHSIZ / 2;
  postext.z += -0.02;	// -2 cm
  //error("text : %.2f %.2f %.2f",postext.x,postext.y,postext.z);

  bubtext = new Text(text, postext, scale, _color, _face);
  if (bubtext) {
    bubtext->setPos(postext.x, postext.y, postext.z, postext.az, postext.ax);
  }
}

void Bubble::changePosition(float lasting)
{
  if (state == ACTIVE) {
    setPosition();
  }
  if (bubtext) {
    postext = pos;
    postext.y += (strlen(text)+2) * Text::GLYPHSIZ / 2;
    postext.z += -0.02;	// -2 cm
    bubtext->setPos(postext.x, postext.y, postext.z, postext.az, postext.ax);
  }
}

void Bubble::updateTime(time_t sec, time_t usec, float *lasting)
{
  if (! updateLasting(sec, usec, lasting)) {
    /* the text has spent its live time, it must be destroyed */
    if (bubtext) {
      //error("bubble: delet text %s", text);
      bubtext->expire();	// delete text inside Bubble
    }
    toDelete(); 	// delete Bubble
    localuser->resetBubble();
    bubtext = NULL;
    state = INACTIVE;
  }
}

bool Bubble::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Bubble::quit()
{
  oid = 0;
  if (text) {
    free(text);
  }
}

void Bubble::funcs()
{}
