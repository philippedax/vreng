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
#include "guide.hpp"
#include "user.hpp"	// USER_TYPE
#include "move.hpp"	// gotoFront


const OClass Guide::oclass(GUIDE_TYPE, "Guide", Guide::creator);

const uint8_t Guide::GUIDE_MAX = 32;
const float Guide::GUIDE_DELTAZ = 0.02;
const float Guide::GUIDE_COLOR[] = {0, 1, 0};

// local
static uint16_t oid = 0;
static Guide *guide = NULL;
static float path[32][5];	///< array of positions-speed-pause in the path
static float color[3];		///< path's color
static float origin[4];		///< origin position of guide
static float userpos[4];	///< initial position of user


/* creation from a file */
WObject * Guide::creator(char *l)
{
  return new Guide(l);
}

void Guide::defaults()
{
  seg = 0;
  segs = 0;
  dlist = -1;
  oneway = true;
  perpetual = false;
  stuck = false;
  restored = false;
  show = true;
  pause = false;

  for (int i=0; i < GUIDE_MAX; i++) for (int j=0; j<5; j++) path[i][j] = 0;
  for (int i=0; i<3; i++) color[i] = GUIDE_COLOR[i];
}

void Guide::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "path=")) l = parse()->parseGuide(l, &path[1], &segs);
    else if (! stringcmp(l, "color")) l = parse()->parseVector3f(l, color, "color");
    else if (! stringcmp(l, "mode=")) {
      char modestr[16];
      l = parse()->parseString(l, modestr, "mode");
      if      (! stringcmp(modestr, "one-way"))   oneway = true;
      else if (! stringcmp(modestr, "perpetual")) perpetual = true;
    }
  }
  end_while_parse(l);

  if (segs == 0) {
    warning("no path");
    return;
  }

  // start segment path[0] = guide initial
  path[0][0] = pos.x;
  path[0][1] = pos.y;
  path[0][2] = pos.z;
  path[0][3] = path[1][3];
  path[0][4] = 1;	// tempo 1 sec

  segs++;	// end of path
  if (perpetual) {
    oneway = false;
  }
  if (oneway) {
    path[segs][3] = 0;
    path[segs][4] = 0;
  }
  else {
    // round-trip: end segment = begin segment
    for (int i=0; i<5 ; i++) path[segs+1][i] = path[0][i];
    path[segs+1][3] = 0;  // speed = 0 for end segment
    path[segs+1][4] = 0;
    segs++;
  }
}

Guide::Guide(char *l)
{
  parser(l);

  guide = this;

  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
  if (perpetual) {
    enablePermanentMovement();
  }

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

/** Imposed movements  - notused */
void Guide::changePosition(float lasting)
{
  error("changePosition");
  pos.az += lasting * move.aspeed.v[0];
  updatePosition();

  localuser->pos.az = pos.az;
  localuser->updatePositionAndGrid(localuser->pos);
}

void Guide::motion()
{
  float nn = sqrt((path[seg+1][0]-path[seg][0]) * (path[seg+1][0]-path[seg][0]) +
                  (path[seg+1][1]-path[seg][1]) * (path[seg+1][1]-path[seg][1]) +
                  (path[seg+1][2]-path[seg][2]) * (path[seg+1][2]-path[seg][2]));

  if (! pause) {
    updatePositionAndGrid(pos);
    pos.x += ((path[seg+1][0] - path[seg][0]) * path[seg][3] / 100) / nn;
    pos.y += ((path[seg+1][1] - path[seg][1]) * path[seg][3] / 100) / nn;
    pos.z += ((path[seg+1][2] - path[seg][2]) * path[seg][3] / 100) / nn;
    updatePosition();
  }
}

void Guide::motion(float *dx, float *dy, float *dz)
{
  float nn = sqrt((path[seg+1][0]-path[seg][0]) * (path[seg+1][0]-path[seg][0]) +
                  (path[seg+1][1]-path[seg][1]) * (path[seg+1][1]-path[seg][1]) +
                  (path[seg+1][2]-path[seg][2]) * (path[seg+1][2]-path[seg][2]));

  if (pause) {
    *dx = *dy = *dz = 0;
  }
  else {
    updatePositionAndGrid(pos);
    *dx = ((path[seg+1][0] - path[seg][0]) * path[seg][3] / 50) / nn;
    *dy = ((path[seg+1][1] - path[seg][1]) * path[seg][3] / 50) / nn;
    *dz = ((path[seg+1][2] - path[seg][2]) * path[seg][3] / 50) / nn;
    pos.x += *dx;
    pos.y += *dy;
    pos.z += *dz;
    updatePosition();
  }
}

void Guide::changePermanent(float lasting)
{
  if (perpetual) {
    if (path[seg][3]) {
      motion();
      if ((floor(pos.x) == path[seg+1][0]) &&
          (floor(pos.y) == path[seg+1][1])) {
        seg++;
      }
    }
    if (seg >= segs) seg = 0;
  }
}

void sigguide(int s)
{
  guide->pause = false;
}

/**
 * user intersects: follow
 * by moving the guide first
 */
bool Guide::whenIntersect(WObject *pcur, WObject *pold)
{
  static bool once = true;

  if (perpetual) return true;

  if (pcur->type != USER_TYPE) {
    pold->copyPositionAndBB(pcur);
    return true;
  }
  // user only
  stuck = true;
  if (restored) {
    once = true;
    restored = false;
  }

  if (once) {
    // save initial position of the user
    userpos[0] = pold->pos.x;
    userpos[1] = pold->pos.y;
    userpos[2] = pold->pos.z;
    userpos[3] = pold->pos.az;
    localuser->pos.x = pos.x;
    localuser->pos.y = pos.y;
    localuser->pos.z += (pos.z + pos.bbs.v[2]);  // jump on the skate
    localuser->pos.az = pos.az;
    localuser->updatePositionAndGrid(localuser->pos);
    if (path[seg][4]) {	// pause
      pause = true;
      signal(SIGALRM, sigguide);
      alarm((uint32_t) path[seg][4]);	// set delay
    }
    once = false;
  }

  if (path[seg][3]) {
    float dx, dy, dz;
    motion(&dx, &dy, &dz);
    // user follows the guide
    localuser->pos.x += dx;
    localuser->pos.y += dy;
    localuser->pos.z += dz + .05;  // + 1cm
    localuser->updatePositionAndGrid(localuser->pos);
    //error("follow: %.2f %.2f %.2f, %.3f %.3f %.3f", localuser->pos.x,localuser->pos.y,localuser->pos.z,dx,dy,dz);
    if (localuser->pos.x == pold->pos.x && localuser->pos.y == pold->pos.y) {
      pold->copyPositionAndBB(localuser);
    }
    else {
      localuser->pos.z += GUIDE_DELTAZ;
    }
    updatePositionAndGrid(pold->pos);  //HACK! I don't know why!
    localuser->updatePositionAndGrid(localuser->pos);

    if ((floor(pos.x) == path[seg+1][0]) &&
        (floor(pos.y) == path[seg+1][1])) {  // new segment
      seg++;  // next segment
      if (path[seg][4]) {  // pause
        pause = true;
        signal(SIGALRM, sigguide);
        alarm((uint32_t) path[seg][4]);  // set delay
        float azn =  atan((path[seg+1][1] - path[seg][1]) /
                          (path[seg+1][0] - path[seg][0]));
        if ((path[seg+1][0] - path[seg][0]) < 0) {
          azn += M_PI;
        }
#if 0 //dax
        float azo =  atan((path[seg][1] - path[seg-1][1]) /
                          (path[seg][0] - path[seg-1][0]));
        if ((path[seg][0] - path[seg-1][0]) < 0)  azo += M_PI;
#endif
        float da = deltaAngle(azn, 0);
        move.aspeed.v[0] = da / path[seg][4];
      }
      else {
        signal(SIGALRM, SIG_IGN);
        motion(&dx, &dy, &dz);
        localuser->pos.x += dx;
        localuser->pos.y += dy;
        localuser->pos.z += dz;
        localuser->updatePositionAndGrid(localuser->pos);
      }

      // new orientation
      float az = atan((path[seg+1][1] - path[seg][1]) /
                      (path[seg+1][0] - path[seg][0]));
      if ((path[seg+1][0] - path[seg][0]) < 0) {
        az += M_PI;
      }
      pos.az = az;
      localuser->pos.az = pos.az;	// user takes same orientation than guide
    }
    // update user position
    localuser->updatePositionAndGrid(localuser->pos);
  }
  else {
    once = true;
    restore((User *)localuser);
  }
  return true;
}

void Guide::restore(User *user)
{
  restored = true;
  seg = 0;
  pos.x  = origin[0];
  pos.y  = origin[1];
  pos.z  = origin[2];
  pos.az = origin[3];
  updatePositionAndGrid(pos);

  if (oneway)
    warning("end of trip");
  else {
    user->pos.x = userpos[0];
    user->pos.y = userpos[1];
    user->pos.z = userpos[2];
    user->pos.az = userpos[3];
    user->updatePositionAndGrid(user->pos);
    warning("end of tour");
  }
}

bool Guide::whenIntersectOut(WObject *pcur, WObject *pold)
{
  //error("out guide");
  if (pcur->type == USER_TYPE) {
    stuck = false;
    return true;
  }
  return false;
}

bool Guide::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Guide::draw(float *color)
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glLineWidth(8);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0x3333);
  glBegin(GL_LINE_LOOP);
  for (int i=0; i < segs ; i++) {
    glColor3fv(color);
    glVertex3f(path[i][0], path[i][1], path[i][2]);
  }
  glDisable(GL_LINE_STIPPLE);
  glEnd();
  glEndList();
}

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

void Guide::showhide(Guide *o, void *d, time_t s, time_t u)
{
  o->show ^= 1;
}

void Guide::pausecontinue(Guide *o, void *d, time_t s, time_t u)
{
  o->pause ^= 1;
}

void Guide::reset(Guide *o, void *d, time_t s, time_t u)
{
  o->restore(localuser);
}

void Guide::quit()
{
  glDeleteLists(dlist, 1);
  oid = 0;
  signal(SIGALRM, SIG_IGN);
}

void Guide::funcs()
{
  setActionFunc(GUIDE_TYPE, 0, WO_ACTION gotoFront, "Approach");
  setActionFunc(GUIDE_TYPE, 1, WO_ACTION showhide, "Show/Hide");
  setActionFunc(GUIDE_TYPE, 2, WO_ACTION pausecontinue, "Pause/Continue");
  setActionFunc(GUIDE_TYPE, 3, WO_ACTION reset, "Reset");
}
