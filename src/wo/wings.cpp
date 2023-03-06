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
#include "wings.hpp"
#include "user.hpp"	// User
#include "draw.hpp"	// sphere


const OClass Wings::oclass(WINGS_TYPE, "Wings", Wings::creator);

//local
static Wings *pwings = NULL;
static uint16_t oid = 0;

struct sWings {
  uint8_t wings_id;
  const char wings_str[16];
};
  
static struct sWings wingss[] = {
  {Wings::BIRD,       "bird"},
  {Wings::BUTTERFLY,  "butterfly"},
  {Wings::LIBELLULE,  "libellule"},
  {Wings::ANGEL,      "angel"}, 
  {Wings::EAGLE,      "eagle"},
  {Wings::HELICOPTER, "helicopter"},
  {Wings::NOWINGS,    ""},
};


WObject * Wings::creator(char *l)
{ 
  return new Wings(l);
}

void Wings::defaults()
{
  article = Cloth::WINGS;
  ttl = MAXFLOAT;
  dx = -0.10;	// avatar's back
  dy = 0;
  dz = 0.55;
  dax = 0;
  day = -45;	// -90
  daz = 90;
  angle = 0;	// open
  scale = 1;
  model = NONE;
}

/* solid geometry */
void Wings::makeSolid()
{
  char s[256];
  sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" />",.6,.1,.6);
  parseSolid(s);
}

uint8_t Wings::getModel(const char *name)
{
  struct sWings *pwingss = wingss;

  if (name) {
    for ( ; pwingss; pwingss++) {
      if (! strcmp(name, pwingss->wings_str))
        return pwingss->wings_id;
    }
  }
  return NONE;
}

/* Created from file */
void Wings::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);	// <solid ... />
    if (!l) break;
    if (! stringcmp(l, "scale=")) {
      l = parse()->parseFloat(l, &scale, "scale");
    }
    else if (! stringcmp(l, "model=")) {
      l = parse()->parseString(l, modelname, "model");
      if      (! stringcmp(modelname, "bird"))       model = BIRD;
      else if (! stringcmp(modelname, "butterfly"))  model = BUTTERFLY;
      else if (! stringcmp(modelname, "libellule"))  model = LIBELLULE;
      else if (! stringcmp(modelname, "angel"))      model = ANGEL;
      else if (! stringcmp(modelname, "eagle"))      model = EAGLE;
      else if (! stringcmp(modelname, "helicopter")) model = HELICOPTER;
      else model = BIRD;
    }
  }
  end_while_parse(l);
}

/* Created from file */
Wings::Wings(char *l)
{
  parser(l);
  pos.ax -= M_PI_2;
  active = false;
  taken = false;
  behavior();
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);

  draw();
}

/* Called by bird and drone */
Wings::Wings(uint8_t _model, float _scale)
{
  model = _model;
  active = true;
  taken = false;
  behavior();
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  //pos.ax -= M_PI_2;
  pos.az -= M_PI_2;
  scale = _scale;

  draw();
}

/* Called by solid shape=wings */
Wings::Wings()
{
  model = BIRD;
  active = true;
  taken = false;
  behavior();
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  pos.ax -= M_PI_2;
  pos.az -= M_PI_2;
  scale = .3;

  draw();
}

/* Regenerated by world via VRSql */
Wings::Wings(User *user, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // name transmitted
  char *p;
  if (!str) return;

  strcpy(names.given, str);
  strcpy(names.type, typeName());     // need names.type for MySql
  p = strchr(str, '&');
  *p = '\0';
  strcpy(modelname, str);

  pwings = this;
  defaults();
  active = false;
  taken = true;
  model = getModel(modelname);
  makeSolid();
  setName(modelname);
  setOwner();
  getPersist();
  behavior();
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
  inits();

  draw();
}

void Wings::start()
{
  enablePermanentMovement();
}

void Wings::stop()
{
  disablePermanentMovement();
}

void Wings::draw()
{
  draw(model);
}

void Wings::draw(uint8_t _model)
{
  //center wing
  dlist_center = glGenLists(1);
  glNewList(dlist_center, GL_COMPILE);
  glBegin(GL_POLYGON);
  switch (_model) {
  case BIRD :
    glColor3f(.5, .5, .5);
    Draw::sphere(.05, 8, 8, 0); // head
    glRectf(-.04, -.5, .04, 0); //body
    break;
  case BUTTERFLY :
    glColor3f(.7, .7, .4);
    Draw::sphere(.03, 8, 8, 0); // head
    glRectf(-.02, -.6, .02, 0); //body
    break;
  case HELICOPTER :
    glColor3f(.3, .3, .3); glVertex2f(0, 0);
    glColor3f(.7, .7, .7); glVertex2f(.5, .1);
    glColor3f(.7, .7, .7); glVertex2f(1, .1);
    glColor3f(.7, .7, .7); glVertex2f(1, -.1);
    glColor3f(.7, .7, .7); glVertex2f(.5, -.1);
    glColor3f(.3, .3, .3); glVertex2f(0, 0);
    glColor3f(.7, .7, .7); glVertex2f(.1, .5);
    glColor3f(.7, .7, .7); glVertex2f(.1, 1);
    glColor3f(.7, .7, .7); glVertex2f(-.1, 1);
    glColor3f(.7, .7, .7); glVertex2f(-.1, .5);
    glColor3f(.3, .3, .3); glVertex2f(0, 0);
    glColor3f(.7, .7, .7); glVertex2f(-.5, .1);
    glColor3f(.7, .7, .7); glVertex2f(-1, .1);
    glColor3f(.7, .7, .7); glVertex2f(-1, -.1);
    glColor3f(.7, .7, .7); glVertex2f(-.5, -.1);
    glColor3f(.3, .3, .3); glVertex2f(0, 0);
    glColor3f(.7, .7, .7); glVertex2f(-.1, -.5);
    glColor3f(.7, .7, .7); glVertex2f(-.1, -1);
    glColor3f(.7, .7, .7); glVertex2f(.1, -1);
    glColor3f(.7, .7, .7); glVertex2f(.1, -.5);
    glColor3f(.3, .3, .3); glVertex2f(0, 0);
    Draw::sphere(.1, 8, 8, 0);
  default:
    break;
  }
  glEnd();
  glEndList();

  //left wing
  dlist_right = glGenLists(1);
  glNewList(dlist_right, GL_COMPILE);
  glBegin(GL_POLYGON);
  switch (_model) {
  case BIRD :
    glColor3f(.9, .9, .9); glVertex2f(0, 0);
    glColor3f(.6, .6, .9); glVertex2f(0.4, 0.2);
    glColor3f(.0, .0, .9); glVertex2f(0.7, 0);
    glColor3f(.6, .6, .9); glVertex2f(0.7, -0.2);
    glColor3f(.6, .6, .6); glVertex2f(0.4, -0.2);
    glColor3f(.6, .6, .9); glVertex2f(0, -0.4);
    break;
  case BUTTERFLY :
    glColor3f(.2, .6, .7); glVertex2f(0, 0);
    glColor3f(.3, .6, .4); glVertex2f(0.2, 0.1);
    glColor3f(.4, .6, .5); glVertex2f(0.4, 0.25);
    glColor3f(.5, .6, .5); glVertex2f(0.6, 0.15);
    glColor3f(.5, .6, .5); glVertex2f(0.63, 0.1);
    glColor3f(.5, .6, .5); glVertex2f(0.6, -0.4);
    glColor3f(.6, .6, .6); glVertex2f(0.5, -0.5);
    glColor3f(.6, .6, .0); glVertex2f(0, -0.6);
    glColor3f(.6, .6, .6); glVertex2f(0.5, -0.5);
    glColor3f(.5, .6, .5); glVertex2f(0.5, -0.57);
    glColor3f(.8, .7, .6); glVertex2f(0.4, -0.8);
    glColor3f(.8, .7, .6); glVertex2f(0.3, -0.8);
    glColor3f(.6, .6, .0); glVertex2f(0, -0.6);
    break;
  case LIBELLULE :
    glColor3f(0, 1, 0); glVertex2f(0, 0);
    glColor3f(0, 0, 1); glVertex2f(.4, .1);
    glColor3f(0, 1, 0); glVertex2f(.6, .1);
    glColor3f(0, 0, 1); glVertex2f(1, -.025);
    glColor3f(0, 1, 0); glVertex2f(1, -.075);
    glColor3f(0, 0, 1); glVertex2f(.6, -.2);
    glColor3f(0, 1, 0); glVertex2f(.4, -.2);
    glColor3f(0, 0, 1); glVertex2f(0, -.1);
    glColor3f(0, 0, 1); glVertex2f(0, -.2);
    glColor3f(0, 1, 0); glVertex2f(.4, -.1);
    glColor3f(0, 0, 1); glVertex2f(.6, -.1);
    glColor3f(0, 1, 0); glVertex2f(1, -.23);
    glColor3f(0, 0, 1); glVertex2f(1, -.26);
    glColor3f(0, 1, 0); glVertex2f(.6, -.4);
    glColor3f(0, 0, 1); glVertex2f(.4, -.3);
    glColor3f(0, 1, 0); glVertex2f(0, -.3);
    break;
  case ANGEL :
    glColor3f(1, 1, 0); glVertex2f(0, 0);
    glColor3f(1, 1, 1); glVertex2f(.4, .2);
    glColor3f(1, 1, 0); glVertex2f(.6, .1);
    glColor3f(1, 1, 1); glVertex2f(.6, -.4);
    glColor3f(1, 1, 0); glVertex2f(.6, -.9);
    glColor3f(1, 1, 1); glVertex2f(.4, -.2);
    glColor3f(1, 1, 0); glVertex2f(.3, -.8);
    glColor3f(1, 1, 1); glVertex2f(.2, -.2);
    glColor3f(1, 1, 0); glVertex2f(.1, -.6);
    glColor3f(1, 1, 1); glVertex2f(0, -.3);
    break;
  case EAGLE :
    glColor3f(1, 0, 1); glVertex2f(0, 0);
    glColor3f(1, 0, 1); glVertex2f(.05, 0);
    glColor3f(1, 0, 1); glVertex2f(.05, .05);
    glColor3f(1, 0, 1); glVertex2f(.1, .1);
    glColor3f(1, 0, 1); glVertex2f(.13, .2);
    glColor3f(1, 0, 1); glVertex2f(.1, .27);
    glColor3f(1, 0, 1); glVertex2f(0, .3);
    glColor3f(1, 0, 1); glVertex2f(.1, 0);
    glColor3f(1, 0, 1); glVertex2f(.2, -.1);
    glColor3f(1, 0, 1); glVertex2f(.18, -.3);
    glColor3f(1, 0, 1); glVertex2f(.17, -.5);
    glColor3f(1, 0, 1); glVertex2f(.18, -.6);
    glColor3f(1, 0, 1); glVertex2f(.20, -.7);
    glColor3f(1, 0, 1); glVertex2f(.19, -.71);
    glColor3f(1, 0, 1); glVertex2f(.18, -.66);
    glColor3f(1, 0, 1); glVertex2f(.19, -.74);
    glColor3f(1, 0, 1); glVertex2f(.18, -.75);
    glColor3f(1, 0, 1); glVertex2f(.17, -.68);
    glColor3f(1, 0, 1); glVertex2f(.16, -.75);
    glColor3f(1, 0, 1); glVertex2f(.15, -.76);
    glColor3f(1, 0, 1); glVertex2f(.15, -.68);
    glColor3f(1, 0, 1); glVertex2f(.14, -.72);
    glColor3f(1, 0, 1); glVertex2f(.14, -.73);
    glColor3f(1, 0, 1); glVertex2f(.135, -.72);
    glColor3f(1, 0, 1); glVertex2f(.130, -.72);
    glColor3f(1, 0, 1); glVertex2f(.140, -.60);
    glColor3f(1, 0, 1); glVertex2f(.1, -.20);
    glColor3f(1, 0, 1); glVertex2f(.08, -.60);
    glColor3f(1, 0, 1); glVertex2f(.12, -.80);
    glColor3f(1, 0, 1); glVertex2f(.14, -1.10);
    glColor3f(1, 0, 1); glVertex2f(.16, -1.60);
    glColor3f(1, 0, 1); glVertex2f(.20, -1.63);
    glColor3f(1, 0, 1); glVertex2f(.19, -1.65);
    glColor3f(1, 0, 1); glVertex2f(.13, -1.63);
    glColor3f(1, 0, 1); glVertex2f(.10, -1.60);
    glColor3f(1, 0, 1); glVertex2f(.10, -1.60);
    glColor3f(1, 0, 1); glVertex2f(.0, -.80);
    break;
  default:
    break;
  }
  glEnd();
  glEndList();

  //right wing
  dlist_left = glGenLists(1);
  glNewList(dlist_left, GL_COMPILE);
  glBegin(GL_POLYGON);
  switch (_model) {
  case BIRD :
    glColor3f(.9, .9, .9); glVertex2f(0, 0);
    glColor3f(.6, .6, .9); glVertex2f(-0.4, 0.2);
    glColor3f(.0, .0, .9); glVertex2f(-0.7, 0);
    glColor3f(.6, .6, .9); glVertex2f(-0.7, -0.2);
    glColor3f(.6, .6, .6); glVertex2f(-0.4, -0.2);
    glColor3f(.6, .6, .9); glVertex2f(0, -0.4);
    break;
  case BUTTERFLY :
    glColor3f(.2, .6, .7); glVertex2f(0, 0);
    glColor3f(.3, .6, .4); glVertex2f(-0.2, 0.1);
    glColor3f(.4, .6, .5); glVertex2f(-0.4, 0.25);
    glColor3f(.5, .6, .5); glVertex2f(-0.6, 0.15);
    glColor3f(.5, .6, .5); glVertex2f(-0.63, 0.1);
    glColor3f(.5, .6, .5); glVertex2f(-0.6, -0.4);
    glColor3f(.6, .6, .6); glVertex2f(-0.5, -0.5);
    glColor3f(.6, .6, .0); glVertex2f(0, -0.6);
    glColor3f(.6, .6, .6); glVertex2f(-0.5, -0.5);
    glColor3f(.5, .6, .5); glVertex2f(-0.5, -0.57);
    glColor3f(.8, .7, .6); glVertex2f(-0.4, -0.8);
    glColor3f(.8, .7, .6); glVertex2f(-0.3, -0.8);
    glColor3f(.6, .6, .6); glVertex2f(0, -0.6);
    break;
  case LIBELLULE :
    glColor3f(0, 1, 0); glVertex2f(0, 0);
    glColor3f(0, 0, 1); glVertex2f(-.4, .1);
    glColor3f(0, 1, 0); glVertex2f(-.6, .1);
    glColor3f(0, 0, 1); glVertex2f(-1, -.025);
    glColor3f(0, 1, 0); glVertex2f(-1, -.075);
    glColor3f(0, 0, 1); glVertex2f(-.6, -.2);
    glColor3f(0, 1, 0); glVertex2f(-.4, -.2);
    glColor3f(0, 0, 1); glVertex2f(0, -.1);
    glColor3f(0, 0, 1); glVertex2f(0, -.2);
    glColor3f(0, 1, 0); glVertex2f(-.4, -.1);
    glColor3f(0, 0, 1); glVertex2f(-.6, -.1);
    glColor3f(0, 1, 0); glVertex2f(-1, -.23);
    glColor3f(0, 0, 1); glVertex2f(-1, -.26);
    glColor3f(0, 1, 0); glVertex2f(-.6, -.4);
    glColor3f(0, 0, 1); glVertex2f(-.4, -.4);
    glColor3f(0, 1, 0); glVertex2f(-.0, -.3);
    break;
  case ANGEL :
    glColor3f(1, 1, 0); glVertex2f(0, 0);
    glColor3f(1, 1, 1); glVertex2f(-.4, .2);
    glColor3f(1, 1, 0); glVertex2f(-.6, .1);
    glColor3f(1, 1, 1); glVertex2f(-.6, -.4);
    glColor3f(1, 1, 0); glVertex2f(-.6, -.9);
    glColor3f(1, 1, 1); glVertex2f(-.4, -.2);
    glColor3f(1, 1, 0); glVertex2f(-.3, -.8);
    glColor3f(1, 1, 1); glVertex2f(-.2, -.2);
    glColor3f(1, 1, 0); glVertex2f(-.1, -.6);
    glColor3f(1, 1, 1); glVertex2f(0, -.3);
    break;
  case EAGLE :
    glColor3f(1, 0, 1); glVertex2f(0, 0);
    glColor3f(1, 0, 1); glVertex2f(-.05, 0);
    glColor3f(1, 0, 1); glVertex2f(-.05, .05);
    glColor3f(1, 0, 1); glVertex2f(-.1, .1);
    glColor3f(1, 0, 1); glVertex2f(-.13, .2);
    glColor3f(1, 0, 1); glVertex2f(-.1, .27);
    glColor3f(1, 0, 1); glVertex2f(-0, .3);
    glColor3f(1, 0, 1); glVertex2f(-.1, 0);
    glColor3f(1, 0, 1); glVertex2f(-.2, -.1);
    glColor3f(1, 0, 1); glVertex2f(-.18, -.3);
    glColor3f(1, 0, 1); glVertex2f(-.17, -.5);
    glColor3f(1, 0, 1); glVertex2f(-.18, -.6);
    glColor3f(1, 0, 1); glVertex2f(-.20, -.7);
    glColor3f(1, 0, 1); glVertex2f(-.19, -.71);
    glColor3f(1, 0, 1); glVertex2f(-.18, -.66);
    glColor3f(1, 0, 1); glVertex2f(-.19, -.74);
    glColor3f(1, 0, 1); glVertex2f(-.18, -.75);
    glColor3f(1, 0, 1); glVertex2f(-.17, -.68);
    glColor3f(1, 0, 1); glVertex2f(-.16, -.75);
    glColor3f(1, 0, 1); glVertex2f(-.15, -.76);
    glColor3f(1, 0, 1); glVertex2f(-.15, -.68);
    glColor3f(1, 0, 1); glVertex2f(-.14, -.72);
    glColor3f(1, 0, 1); glVertex2f(-.14, -.73);
    glColor3f(1, 0, 1); glVertex2f(-.135, -.72);
    glColor3f(1, 0, 1); glVertex2f(-.130, -.72);
    glColor3f(1, 0, 1); glVertex2f(-.140, -.60);
    glColor3f(1, 0, 1); glVertex2f(-.1, -.20);
    glColor3f(1, 0, 1); glVertex2f(-.08, -.60);
    glColor3f(1, 0, 1); glVertex2f(-.12, -.80);
    glColor3f(1, 0, 1); glVertex2f(-.14, -1.10);
    glColor3f(1, 0, 1); glVertex2f(-.16, -1.60);
    glColor3f(1, 0, 1); glVertex2f(-.20, -1.63);
    glColor3f(1, 0, 1); glVertex2f(-.19, -1.65);
    glColor3f(1, 0, 1); glVertex2f(-.13, -1.63);
    glColor3f(1, 0, 1); glVertex2f(-.10, -1.60);
    glColor3f(1, 0, 1); glVertex2f(-.10, -1.60);
    glColor3f(1, 0, 1); glVertex2f(-.0, -.80);
    break;
  default:
    break;
  }
  glEnd();
  glEndList();
}

void Wings::changePermanent(float lasting)
{
  if (! active) return;

  static int sign = 1;

  if (taken) {
    pos.x = localuser->pos.x + dx;
    pos.y = localuser->pos.y + dy;
    pos.z = localuser->pos.z + dz;
    pos.ax = localuser->pos.ax + DEG2RAD(dax);
    pos.ay = localuser->pos.ay + DEG2RAD(day);
    pos.az = localuser->pos.az + DEG2RAD(daz);
  }
  updatePosition();

  switch (model) {
  case BIRD :
    if (angle > 30)  sign = -1;
    if (angle < -30) sign = 1;
    break;
  case BUTTERFLY :
    if (angle > 60)  sign = -1;
    if (angle < 0)   sign = 1;
    break;
  case LIBELLULE :
    if (angle > 15)  sign = -1;
    if (angle < -15) sign = 1;
    break;
  case ANGEL :
    if (angle > 30)  sign = -1;
    if (angle < 0)   sign = 1;
    break;
  case EAGLE :
    sign = 0;
  case HELICOPTER :
    sign = 1;
    angle += sign * 6;
    break;
  }
  angle += sign * 6;
}

void Wings::render()
{
  render(model);
}

void Wings::render(uint8_t _model)
{
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);

  glTranslatef(pos.x, pos.y, pos.z);
  glRotatef(RAD2DEG(pos.ax), 1, 0, 0);
  glRotatef(RAD2DEG(pos.ay), 0, 1, 0);
  glRotatef(RAD2DEG(pos.az), 0, 0, 1);
  switch (_model) {
  case HELICOPTER:
    glRotatef(90, 1, 0, 0);
    glPushMatrix();
     glRotatef(angle, 0, 0, 1);
     glCallList(dlist_center);
    glPopMatrix();
    break;
  default:
    glScalef(scale, -scale, scale);
    glPushMatrix();
     glCallList(dlist_center);
    glPopMatrix();
    glPushMatrix();
     glRotatef(angle, 0, 1, 0);
     glCallList(dlist_left);
    glPopMatrix();
    glPushMatrix();
     glRotatef(-angle, 0, 1, 0);
     glCallList(dlist_right);
    glPopMatrix();
    break;
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void Wings::quit()
{
  pwings = NULL;
  oid = 0;
  savePersistency();
  if (dlist_center > 0) glDeleteLists(dlist_center, 1);
  if (dlist_right > 0) glDeleteLists(dlist_right, 1);
  if (dlist_left > 0) glDeleteLists(dlist_left, 1);
}

/* wear */
void Wings::wear()
{
  if (taken) takeoff();
  defaults();
  active = true;
  taken = true;
  model = getModel(modelname);
  setName(modelname);
  setOwner();
  setPersist();
  behavior();
  enableBehavior(SPECIFIC_RENDER);
  inits();
  addToWearList();
}

/* takeoff */
void Wings::takeoff()
{
  // restore original position
  restorePosition();
  delPersist();
  delFromWearList();
  active = false;
  taken = false;
}

/* wear: indirectly called by user */
void Wings::wear_cb(Wings *wings, void *d, time_t s, time_t u)
{
  wings->wear();
}

/* takeoff: indirectly called by user */
void Wings::takeoff_cb(Wings *wings, void *d, time_t s, time_t u)
{
  wings->takeoff();
}

/* Creation: this method is invisible: called by the World */
void Wings::recreate_cb(User *pu, void *d, time_t s, time_t u)
{
  new Wings(pu, d, s, u);
}

void Wings::funcs()
{
  getPropertyFunc(WINGS_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(WINGS_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(WINGS_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(WINGS_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(WINGS_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(WINGS_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(WINGS_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(WINGS_TYPE, PROPZ, _Payload put_z); 
  putPropertyFunc(WINGS_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(WINGS_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(WINGS_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(WINGS_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(WINGS_TYPE, WEAR, _Action wear_cb, "Wear");
  setActionFunc(WINGS_TYPE, TAKEOFF, _Action takeoff_cb, "Takeoff");
  setActionFunc(WINGS_TYPE, RECREATE, _Action recreate_cb, "");
}
