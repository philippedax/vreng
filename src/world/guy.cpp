//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// guy.cpp
//
// Guy handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "guy.hpp"
#include "http.hpp"	// httpOpen
#include "draw.hpp"	// Draw
#include "timer.hpp"	// isRate
#include "user.hpp"	// localuser
#include "cache.hpp"	// open
#include "netobj.hpp"	// netop
#include "pref.hpp"	// trace


const OClass Guy::oclass(GUY_TYPE, "Guy", Guy::creator, Guy::replicator);

uint16_t Guy::RATE = 10;
uint8_t Guy::GUY_PARTS = 9;
uint8_t Guy::OVERSAMPLE = 10;
const char Guy::DEF_URL_GUY[] = "/cset/walking.cset";
const float Guy::SKIN_COLOR[] = {.8, .6, .6, 1};
const float Guy::BUST_COLOR[] = {.9, .5, .1, 1};
const float Guy::LEGS_COLOR[] = {.1, .3, .9, 1};
const float Guy::FEET_COLOR[] = {.0, .0, .0, 1};


/** Creation from a file */
Object * Guy::creator(char *l)
{
  return new Guy(l);
}

void Guy::defaults()
{
  animing = walking = 0;
  sex = 0; //male
  showing = false;
  flying = false;
  for (int i=0; i<4; i++) {
    skin_color[i] = SKIN_COLOR[i];
    bust_color[i] = BUST_COLOR[i];
    legs_color[i] = LEGS_COLOR[i];
    feet_color[i] = FEET_COLOR[i];
  }
}

/** Parses a vre line */
void Guy::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url"))   l = parseUrl(l, name.url);
    else if (! stringcmp(l, "anim=")) l = parseBool(l, &animing, "anim");
    else if (! stringcmp(l, "walk=")) l = parseBool(l, &walking, "walk");
    else if (! stringcmp(l, "sex="))  l = parseBool(l, &sex, "sex");
    else if (! stringcmp(l, "skin=")) l = parseVector3f(l, skin_color, "skin");
    else if (! stringcmp(l, "bust=")) l = parseVector3f(l, bust_color, "bust");
    else if (! stringcmp(l, "legs=")) l = parseVector3f(l, legs_color, "legs");
    else if (! stringcmp(l, "feet=")) l = parseVector3f(l, feet_color, "feet");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

/** Creates a solid */
void Guy::geometry()
{
  char s[128];

  sprintf(s, "geom shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />",
          BUST_W, HIP_R*2, HEAD_R*2+NECK_H+BUST_H+ULEG_H+LLEG_H+FOOT_H);
  parseSolid(s);
}

/** Sets behaviors */
void Guy::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(PERMANENT_MOVEMENT);	// gravity
  enableBehavior(SPECIFIC_RENDER);
}

/** Do specific initializations */
void Guy::inits()
{
  mobileObject(0);

  step = CYCLES/2;
  incstep = 1.;
  dlist = -1;

  draw();

  Http::httpOpen(name.url, reader, this, 0);

  computeCurve();
  netop = createNetObj(PROPS);
}

/** Constructor from vre file */
Guy::Guy(char *l)
{
  parser(l);
  behaviors();
  geometry();
  inits();
  control = false;
}

/** Constructor from user */
Guy::Guy()
{
  defaults();
  strcpy(name.url, DEF_URL_GUY);
  behaviors();
  geometry();
  inits();
  control = true;

  setActionFunc(GUY_TYPE, 1, _Action NULL, "");  // cancel
  setActionFunc(GUY_TYPE, 2, _Action NULL, "");  // cancel
}

/** Replication from the network */
Object * Guy::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Guy(type_id, noid, pp);
}

Guy::Guy(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);

  netop = replicate(PROPS, _noid);
  netop->getAllProperties(pp);

  defaults();
  geometry();
  behaviors();
  mobileObject(0);
}

/** Reads curves file */
void Guy::reader(void *_guy, Http *http)
{
  Guy *guy = static_cast<Guy *>(_guy);
  if (! guy) return;
  int pts = 0;

  Cache *cache = new Cache();
  FILE *f = cache->open(http->url, http);
  if (! f) {
    error("can't open %s", http->url);
    return;
  }

  char *l, line[256];
  fgets(line, sizeof(line), f);		// numjoints
  line[strlen(line) - 1] = '\0';
  guy->numjoints = atoi(line);
  guy->curve = new tCset[guy->numjoints];
  fgets(line, sizeof(line), f);		// skip unused line
  for (int j=0; j < guy->numjoints; j++) {
    fgets(line, sizeof(line), f);	// numpoints
    line[strlen(line) - 1] = '\0';
    pts = atoi(line);
    if (pts < 4 || pts > MAX_POINTS) {
      error("wrong CSet file, numjoints=%d, pts=%d", guy->numjoints, pts);
      guy->pose();
    }
    else {
      guy->curve[j].numpoints = pts;
      fgets(line, sizeof(line), f);	// coords of joinpoints
      line[strlen(line) - 1] = '\0';
      l = strtok(line, " ");
      for (int i=0; i < pts; i++) {
        guy->curve[j].coords[i] = static_cast<float>(atof(l));
        l = strtok(NULL, " ");
      }
      fgets(line, sizeof(line), f);	// angles
      line[strlen(line) - 1] = '\0';
      l = strtok(line, " ");
      for (int i=0; i < pts; i++) {
        guy->curve[j].angles[i] = static_cast<float>(atof(l));
        l = strtok(NULL, " ");
      }
    }
  }
  cache->close();
  delete cache;
  return;

}

/** Computes curves */
void Guy::computeCurve()
{
  float pointset[3][4];
  float vprod[3][4], tm[4], vec[3];
  float tinc = 1./CYCLES/OVERSAMPLE;
  float basis[4][4] = { {-1, 3, -3, 1}, {3, -6, 3, 0}, {-3, 3, 0, 0}, {1, 0, 0, 0} };

  for (int join=0; join < numjoints; join++) {
    for (int i=0; i<4; i++) {
      pointset[2][i] = 0;
    }
    int newindex, lastindex = -1;
    for (int p=0; p < curve[join].numpoints; p += 3) {
      float t = 0.;
      for (int i=0; i<4; i++) {
        pointset[0][i] = curve[join].coords[p + i];
        pointset[1][i] = curve[join].angles[p + i];
      }
      mulM3M4(pointset, basis, vprod);

      while (t <= 1) {
        tm[0] = t*t*t;
        tm[1] = t*t;
        tm[2] = t;
        tm[3] = 1;
        mulM3V4(vprod, tm, vec);
        newindex = int( (vec[0] * (CYCLES-1)) );
        if (newindex > lastindex) {  // go at least one
          newindex %= CYCLES;  // avoid out of bounds
          cycles[L_SIDE][join][newindex] = vec[1];
          lastindex++;
        }
        t += tinc;
      }
    }
    for (int i=0; i < CYCLES; i++) {  // copy to other leg, out-o-phase
      cycles[R_SIDE][join][i] = cycles[L_SIDE][join][(i+(CYCLES/2))%CYCLES];
    }
  }
}

/** pose */
void Guy::pose()
{
  for (int i=0; i < numjoints; i++) {
    curve[i].numpoints = 4;
    curve[i].coords[0] = 0;   curve[i].angles[0] = 0;
    curve[i].coords[1] = 0.2; curve[i].angles[1] = 0;
    curve[i].coords[2] = 0.8; curve[i].angles[2] = 0;
    curve[i].coords[3] = 1.0; curve[i].angles[3] = 0;
  }
}

/** Updates to the network */
bool Guy::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/** Do eaquations when permanently moving */
void Guy::permanent(float lasting)
{
  if (animing) {
    step = int( fmod((double) step + incstep, CYCLES) );
  }
  if (control && localuser) {
    pos.x = localuser->pos.x;
    pos.y = localuser->pos.y;
    pos.z = localuser->pos.z;
    pos.az = localuser->pos.az - M_PI_2;
    updatePos();
  }
}

void Guy::draw_bust()
{
  glPushMatrix();
   glScalef(.8, 1, .5); // glScalef(BUST_W, BUST_H, ULEG_R);
   glRotatef(-90, 1, 0, 0);
   //Draw::cylinder(HIP_R*2 - 0.05, BUST_W, BUST_H, 16, 16, 0);
   Draw::cylinder(HIP_R*2, BUST_W, BUST_H, 16, 16, 0);
  glPopMatrix();
}

void Guy::draw_brea()
{
  glPushMatrix();
   //glTranslatef(0, BUST_H - NECK_H*3, BREA_R);
   glTranslatef(0, BUST_L, -BREA_R);
   Draw::cylinder(BREA_R, 0, -BREA_R*1.5, 8, 8, 0);
  glPopMatrix();
}

void Guy::draw_neck()
{
  glPushMatrix();
   glTranslatef(0, BUST_H, 0);
   glRotatef(-90, 1, 0, 0);
   Draw::cylinder(NECK_R, NECK_R, NECK_H, 8, 8, 0);
  glPopMatrix();
}

void Guy::draw_head()
{
  glPushMatrix();
   glTranslatef(0, BUST_H + NECK_H + HEAD_R, 0);
   Draw::sphere(HEAD_R, 16, 16, 0);
  glPopMatrix();
}

void Guy::draw_uleg()
{
  // knee
  glPushMatrix();
   glTranslatef(0, -(HIP_R + ULEG_H -KNEE_R), 0);
   Draw::sphere(KNEE_R, 8, 8, 0);
  glPopMatrix();

  // hip
  glPushMatrix();
   glTranslatef(0, -HIP_R, 0);
   Draw::sphere(HIP_R, 8, 8, 0);
  glPopMatrix();

  // thigh
  glPushMatrix();
   glRotatef(90, 1, 0, 0);
   Draw::sphere(HIP_R, 8, 8, 0);
   Draw::cylinder(HIP_R, KNEE_R, ULEG_H, 8, 8, 0);
  glPopMatrix();
}

void Guy::draw_lleg()
{
  // calf
  glPushMatrix();
   glRotatef(90, 1, 0, 0);
   Draw::cylinder(KNEE_R, ANKLE_R, LLEG_H, 8, 8, 0);
  glPopMatrix();
}

void Guy::draw_foot()
{
  glPushMatrix();
   glTranslatef(0, 0, -ANKLE_R/2);
   Draw::sphere(ANKLE_R, 16, 16, 0);
   glRotatef(180, 1, 0, 0);
   glScalef(1.2, .6, 1);
   Draw::cylinder(ANKLE_R, ANKLE_R, FOOT_H, 8, 8, 0);
  glPopMatrix();
}

void Guy::draw_uarm()
{
  // elbow
  glPushMatrix();
   glTranslatef(0, -(SHOULDER_R + UARM_H), 0);
   Draw::sphere(ELBOW_R, 8, 8, 0);
  glPopMatrix();

  // shoulder
  glTranslatef(0, -SHOULDER_R, 0);
  Draw::sphere(SHOULDER_R, 8, 8, 0);

  // upper arm
  glPushMatrix();
   glRotatef(90, 1, 0, 0);
   Draw::cylinder(SHOULDER_R, ELBOW_R, UARM_H, 8, 8, 0);
  glPopMatrix();
}

void Guy::draw_larm()
{
  glPushMatrix();
   glRotatef(90, 1, 0, 0);
   Draw::cylinder(ELBOW_R, WRIST_R, LARM_H, 8, 8, 0);
  glPopMatrix();
}

/** Draws the Guy into a display-list */
void Guy::draw()
{
  if (dlist != -1)
    glDeleteLists(dlist, GUY_PARTS);

  dlist = glGenLists(GUY_PARTS);
  glNewList(dlist + BUST, GL_COMPILE); draw_bust(); glEndList();
  glNewList(dlist + BREA, GL_COMPILE); draw_brea(); glEndList();
  glNewList(dlist + NECK, GL_COMPILE); draw_neck(); glEndList();
  glNewList(dlist + HEAD, GL_COMPILE); draw_head(); glEndList();
  glNewList(dlist + ULEG, GL_COMPILE); draw_uleg(); glEndList();
  glNewList(dlist + LLEG, GL_COMPILE); draw_lleg(); glEndList();
  glNewList(dlist + FOOT, GL_COMPILE); draw_foot(); glEndList();
  glNewList(dlist + UARM, GL_COMPILE); draw_uarm(); glEndList();
  glNewList(dlist + LARM, GL_COMPILE); draw_larm(); glEndList();
}

void Guy::render_bust()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT, bust_color);
  glPushMatrix();
   glCallList(dlist + BUST);
  glPopMatrix();
}

void Guy::render_neck()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT, skin_color);
  glPushMatrix();
   glCallList(dlist + NECK);
  glPopMatrix();
}

void Guy::render_brea(bool side)
{
  if (sex) {
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bust_color);
    glPushMatrix();
     if (side == L_SIDE)
      glTranslatef(BREA_R, 0, 0);
     else
      glTranslatef(-BREA_R, 0, 0);
     glCallList(dlist + BREA);
    glPopMatrix();
  }
}

void Guy::render_head()
{
  if (control && localuser->hasHead()) return;

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, skin_color);
  glPushMatrix();
   glCallList(dlist + HEAD);
  glPopMatrix();
}

void Guy::render_leg(bool side)
{
  glMaterialfv(GL_FRONT, GL_AMBIENT, legs_color);
  glPushMatrix();
   if (side == L_SIDE) {
     glTranslatef(BUST_W/4., HIP_R/2, 0);
   }
   else {
     glTranslatef(-BUST_W/4., HIP_R/2, 0);
   }

   // upper leg: rotates around the X axis
   glRotatef(cycles[side][CSET_ULEG][step], 1, 0, 0);
   glPushMatrix();
    glCallList(dlist + ULEG);
   glPopMatrix();

   // lower leg: rotates around the X axis
   glTranslatef(0, -(ULEG_H + KNEE_R), 0);
   glRotatef(cycles[side][CSET_LLEG][step], 1, 0, 0);
   glPushMatrix();
    glCallList(dlist + LLEG);
   glPopMatrix();

   // foot: rotates around the X axis
   glMaterialfv(GL_FRONT, GL_AMBIENT, feet_color);
   glTranslatef(0, -(ULEG_H + LLEG_H + ANKLE_R)/2, 0);
   glRotatef(cycles[side][CSET_FOOT][step], 1, 0, 0);
   glPushMatrix();
    glCallList(dlist + FOOT);
   glPopMatrix();
  glPopMatrix();
}

void Guy::render_arm(bool side)
{
  glMaterialfv(GL_FRONT, GL_AMBIENT, bust_color);
  glPushMatrix();
   glTranslatef(0, BUST_H, 0);
   if (side == L_SIDE) {
     glTranslatef(BUST_W - SHOULDER_R/2 -0.03, 0, 0);
   }
   else {
     glTranslatef(-BUST_W + SHOULDER_R/2 +0.03, 0, 0);
   }

   // upper arm: rotates around the X axis
   if (flying) {
     glRotatef(135, 1, 0, 0);
   }
   else if (showing && side == R_SIDE) {  // right arm
     glRotatef(90, 1, 0, 0);
   }
   else {
     glRotatef(cycles[side][CSET_UARM][step], 1, 0, 0);
   }
   glPushMatrix();
    glCallList(dlist + UARM);
   glPopMatrix();

   // lower arm: rotates around the X axis
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, skin_color);
   glTranslatef(0, -(UARM_H + ELBOW_R), 0);
   if (flying || (showing && side == R_SIDE)) {
     glRotatef(0, 1, 0, 0);
   }
   else {
     glRotatef(cycles[side][CSET_LARM][step], 1, 0, 0);
   }
   glPushMatrix();
    glCallList(dlist + LARM);
   glPopMatrix();
  glPopMatrix();
}

/** Renders the Guy */
void Guy::render()
{
  if (localuser->guy && ! localuser->visible) return;

  glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glRotatef(RAD2DEG(pos.az), 0, 0, 1);
   glRotatef(90 + RAD2DEG(pos.ax), 1, 0, 0);	// stand up

   render_bust();
   render_neck();
   render_brea(0);
   render_brea(1);
   render_head();
   render_leg(0);
   render_leg(1);
   render_arm(0);
   render_arm(1);
  glPopMatrix();
}

void Guy::setAniming(bool flag)
{
  animing = flag;
}

bool Guy::isAnim() const
{
  return (animing);
}

void Guy::setFlying(bool flag)
{
  flying = flag;
}

bool Guy::isFlying() const
{
  return (flying);
}

void Guy::setShowing(bool flag)
{
  showing = flag;
}

/** Quits - destructor */
Guy::~Guy()
{
  if (curve) delete[] curve;
}

void Guy::animate_cb(Guy *guy, void *d, time_t s, time_t u)
{
  guy->animing = 1 - guy->animing;
}

void Guy::walking_cb(Guy *guy, void *d, time_t s, time_t u)
{
  guy->walking = 1 - guy->walking;
}

void Guy::funcs()
{
  getPropFunc(GUY_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(GUY_TYPE, PROPZ, _Payload get_z);
  getPropFunc(GUY_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(GUY_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(GUY_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(GUY_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(GUY_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(GUY_TYPE, PROPZ, _Payload put_z);
  putPropFunc(GUY_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(GUY_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(GUY_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(GUY_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(GUY_TYPE, 0, _Action animate_cb, "Anim");
  //setActionFunc(GUY_TYPE, 1, _Action walking_cb, "Walk");
}
