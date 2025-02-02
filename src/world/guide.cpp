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
// guide.cpp
//
// Guide handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "guide.hpp"
#include "user.hpp"	// USER_TYPE


const OClass Guide::oclass(GUIDE_TYPE, "Guide", Guide::creator);

#define GUIDE_MAX 64


const uint8_t Guide::GUIDE_DIM = GUIDE_MAX;
const float Guide::GUIDE_DELTAZ = 0.02;
const float Guide::GUIDE_COLOR[] = {0, 1, 0};

// local
static uint16_t oid = 0;
static Guide *guide = NULL;
static float color[3];		///< path's color
static float origin[4];		///< origin position of guide
static float userpos[4];	///< initial position of user

/*
 * path format:
 *
 * path="point1, point2, ..., pointn"
 * pointi = xi yi zi speedi, delayi
 * seg0 = point1 - origin (pos)
 * segi = pointi+1 - pointi
 * guide = origin------point1------point2------ ... ------pointn
 *                seg0        seg1        seg2       segn-1
 */
static float path[GUIDE_MAX][5]; ///< array of position[syz]-speed-pause in the path


/* creation from a file */
Object * Guide::creator(char *l)
{
  return new Guide(l);
}

void Guide::defaults()
{
  seg = 0;
  segs = 0;
  dlist = -1;
  oneway = true;
  testing = false;
  stuck = false;
  restored = false;
  show = true;
  pause = false;

  for (int i=0; i < GUIDE_DIM; i++) {
    for (int j=0; j<5; j++) {
      path[i][j] = 0;
    }
  }
  for (int i=0; i<3; i++) {
    color[i] = GUIDE_COLOR[i];
  }
}

void Guide::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "path=")) l = parse()->parseGuide(l, &path[1], &segs);
    else if (! stringcmp(l, "color")) l = parseVector3f(l, color, "color");
    else if (! stringcmp(l, "mode=")) {
      char mode[16];
      l = parseString(l, mode, "mode");
      if      (! stringcmp(mode, "one-way")) oneway = true;
      else if (! stringcmp(mode, "testing")) testing = true;
    }
  }
  end_while_parse(l);

  if (segs == 0) {
    error("no guide path defined");
    return;
  }
  segs++;	// end of path

  // start segment path[0] = guide initial
  path[0][0] = pos.x;
  path[0][1] = pos.y;
  path[0][2] = pos.z;
  path[0][3] = path[1][3];
  path[0][4] = 1;

  if (testing) {	// without localuser
    oneway = false;
  }
  if (oneway) {
    path[segs][3] = 0;
    path[segs][4] = 0;
  }
  else {		// round-trip
    // end segment = begin segment
    for (int i=0; i<5 ; i++) {
      path[segs+1][i] = path[0][i];
    }
    path[segs+1][3] = 0;  // speed = 0 for end segment
    path[segs+1][4] = 0;
    segs++;
  }
}

void Guide::behaviors()
{
  enableBehavior(PERMANENT_MOVEMENT);	// guide movement
  enableBehavior(SPECIFIC_RENDER);	// ramp rendering
  enableBehavior(MIX_RENDER);		// skate rendering
}

Guide::Guide(char *l)
{
  guide = this;

  parser(l);
  behaviors();

  mobileObject(0);
  createNetObj(PROPS, ++oid);

  pos.az = atan((path[seg+1][1]-path[seg][1]) / (path[seg+1][0]-path[seg][0]));
  origin[0] = pos.x;
  origin[1] = pos.y;
  origin[2] = pos.z;
  origin[3] = pos.az;

  draw(color);	// draw path
}

void Guide::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

/* Stucks the user on the guide */
void Guide::stuckUser()
{
  // save initial position of the user
  userpos[0] = localuser->pos.x;
  userpos[1] = localuser->pos.y;
  userpos[2] = localuser->pos.z;
  userpos[3] = localuser->pos.az;
  updatePositionAndGrid(pos);

  localuser->pos.x = pos.x;
  localuser->pos.y = pos.y;
  localuser->pos.z += (pos.z + pos.bbs.v[2] * 2);  // jump on the skate
  localuser->pos.az = pos.az;
  localuser->updatePositionAndGrid(localuser->pos);

  stuck = true;
}

void sigguide(int s)
{
  guide->pause = false;
}

/* Do the elementary movement */
void Guide::motion(float *dx, float *dy, float *dz)
{
  float nn = sqrt( (path[seg+1][0]-path[seg][0]) * (path[seg+1][0]-path[seg][0]) +
                   (path[seg+1][1]-path[seg][1]) * (path[seg+1][1]-path[seg][1]) +
                   (path[seg+1][2]-path[seg][2]) * (path[seg+1][2]-path[seg][2])
                 );

  if (pause) {
    *dx = *dy = *dz = 0;
  }
  else {
    *dx = ((path[seg+1][0] - path[seg][0]) * path[seg][3]) / nn;
    *dy = ((path[seg+1][1] - path[seg][1]) * path[seg][3]) / nn;
    *dz = ((path[seg+1][2] - path[seg][2]) * path[seg][3]) / nn;
    pos.x += (*dx);
    pos.y += (*dy);
    pos.z += (*dz);
    updatePositionAndGrid(pos);
  }
}

void Guide::progress(Object *po)
{
  float dx, dy, dz;

  motion(&dx, &dy, &dz);

  // user follows the guide
  localuser->pos.x += dx;
  localuser->pos.y += dy;
  localuser->pos.z += dz + .09;  // + 6cm
  //dax localuser->pos.z += (pos.z + pos.bbs.v[2] * 2);
  localuser->updatePositionAndGrid(localuser->pos);
  updatePositionAndGrid(po->pos);
  localuser->updatePositionAndGrid(localuser->pos);

  if ((floor(pos.x) == path[seg+1][0]) &&
      (floor(pos.y) == path[seg+1][1])) {  // is next segment?
    seg++;	// next segment
    //echo("seg=%d/%d", seg, segs);
    if (path[seg][4]) {		// is pause?
      signal(SIGALRM, sigguide);
      alarm((uint32_t) path[seg][4]);  // set delay
      pause = true;
    }
    else {	// no pause
      signal(SIGALRM, SIG_IGN);
      pause = false;
    }
    // orientation
    float az = atan((path[seg+1][1] - path[seg][1]) /
                    (path[seg+1][0] - path[seg][0]));
    if ((path[seg+1][0] - path[seg][0]) < 0) {
      az += M_PI;
    }
    pos.az = az;
    localuser->pos.az = pos.az;	// user takes same orientation than guide
    // angular speed
    float da = deltaAngle(az, 0);
    move.aspeed.v[0] = da / path[seg][4];
  }
  else {	// same segment
    updatePositionAndGrid(pos);	// update position
    localuser->updatePositionAndGrid(localuser->pos);
  }
}

void Guide::changePermanent(float lasting)
{
  if (testing) {
    if (path[seg][3]) {	// speed present
      float dx, dy, dz;

      motion(&dx, &dy, &dz);

      if ((floor(pos.x) == path[seg+1][0]) &&
          (floor(pos.y) == path[seg+1][1])) {
        seg++;
      }
    }
    if (seg >= segs) {
      seg = 0;
      testing = false;
      restore();
    }
  }
}

/**
 * user intersects: follows guide
 * moving the guide first
 */
bool Guide::whenIntersect(Object *pcur, Object *pold)
{
  static bool once = true;

  if (testing) {
    return false;	// guide only
  }

  if (pcur->type != USER_TYPE) {
    pold->copyPositionAndBB(pcur);
    return true;
  }

  //
  // collision with user only
  //
  if (once) {
    once = false;
    stuckUser();
    if (path[seg][4]) {	// pause
      signal(SIGALRM, sigguide);
      alarm((uint32_t) path[seg][4]);	// set delay
      pause = true;
    }
  }
  if (restored) {
    once = true;
    restored = false;
  }
  if (path[seg][3]) {	// speed present
    progress(pold);
  }
  else {
    once = true;
    restore();
  }
  return true;
}

void Guide::restore()
{
  restored = true;
  seg = 0;
  pos.x  = origin[0];
  pos.y  = origin[1];
  pos.z  = origin[2];
  pos.az = origin[3];
  updatePositionAndGrid(pos);

  if (! oneway) {
    localuser->pos.x = userpos[0];
    localuser->pos.y = userpos[1];
    localuser->pos.z = userpos[2];
    localuser->pos.az = userpos[3];
    localuser->updatePositionAndGrid(localuser->pos);
    //echo("end of trip");
  }
  stuck = false;
}

bool Guide::whenIntersectOut(Object *pcur, Object *pold)
{
  //echo("out guide");
  if (pcur->type == USER_TYPE) {
    stuck = false;
    return true;
  }
  return false;
}

bool Guide::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Guide::draw(float *color)
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glLineWidth(5);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0x5555);
  glBegin(GL_LINE_LOOP);
  for (int i=0; i < segs ; i++) {
    glColor3fv(color);
    glVertex3f(path[i][0], path[i][1], path[i][2]);
  }
  glDisable(GL_LINE_STIPPLE);
  glEnd();
  glEndList();
}

/* Renders the ramp */
void Guide::render()
{
  if (! show || dlist < 0) return;

  glPushAttrib(GL_LINE_BIT);
  glPushMatrix();
   glDisable(GL_LIGHTING);

   glCallList(dlist);

   glEnable(GL_LIGHTING);
  glPopMatrix();
  glPopAttrib();
}

void Guide::quit()
{
  glDeleteLists(dlist, 1);
  oid = 0;
  signal(SIGALRM, SIG_IGN);
}

void Guide::visit_cb(Guide *o, void *d, time_t s, time_t u)
{
  if (! o->stuck) {
    o->stuckUser();
  }
}

void Guide::showhide_cb(Guide *o, void *d, time_t s, time_t u)
{
  o->show ^= 1;
}

void Guide::pausecontinue_cb(Guide *o, void *d, time_t s, time_t u)
{
  o->pause ^= 1;
}

void Guide::test_cb(Guide *o, void *d, time_t s, time_t u)
{
  o->testing ^= 1;
}

void Guide::reset_cb(Guide *o, void *d, time_t s, time_t u)
{
  o->restore();
}

void Guide::funcs()
{
  getPropFunc(GUIDE_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(GUIDE_TYPE, PROPZ, _Payload get_z);
  getPropFunc(GUIDE_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(GUIDE_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(GUIDE_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(GUIDE_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(GUIDE_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(GUIDE_TYPE, PROPZ, _Payload put_z);
  putPropFunc(GUIDE_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(GUIDE_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(GUIDE_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(GUIDE_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(GUIDE_TYPE, 0, _Action visit_cb, "Visit");
  setActionFunc(GUIDE_TYPE, 1, _Action showhide_cb, "Show");
  setActionFunc(GUIDE_TYPE, 2, _Action pausecontinue_cb, "Pause");
  setActionFunc(GUIDE_TYPE, 3, _Action test_cb, "Test");
  setActionFunc(GUIDE_TYPE, 4, _Action reset_cb, "Reset");
}
