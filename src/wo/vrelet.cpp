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
#include "vrelet.hpp"
#include "world.hpp"	// current
#include "user.hpp"	// localuser
#include "url.hpp"	// abs
#include "gui.hpp"	// setToVrelet
#include <list>

using namespace std;

const OClass Vrelet::oclass(VRELET_TYPE, "Vrelet", Vrelet::creator);

void Vrelet::funcs() {}


/* creation from a file */
WObject * Vrelet::creator(char *l)
{
  return new Vrelet(l);
}

void Vrelet::defaults()
{
  incrx = 0; incry = 0;
  fx = fy = 1.;
}

/* Vrelet parser */
void Vrelet::parser(char *l)
{
  bool goturl = false;

  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (! l) break;
    if (! stringcmp(l, "class=")) l = parse()->parseString(l, app, "class");	// java class
    else if (! stringcmp(l, "incrx")) l = parse()->parseInt(l, &incrx, "incrx");
    else if (! stringcmp(l, "incry")) l = parse()->parseInt(l, &incry, "incry");
    else if (! stringcmp(l, "fx")) l = parse()->parseFloat(l, &fx, "fx");
    else if (! stringcmp(l, "fy")) l = parse()->parseFloat(l, &fy, "fy");
    else if (! stringcmp(l, "base")) {
      // url in the .vre file, try to find out the codebase URL
      l = parse()->parseString(l, url, "base");
      goturl = true;
    }
  }
  end_while_parse(l);

  if (! goturl) { // there's none in the .vre file.
    char l2[URL_LEN];
    char *last = NULL;
    // if the world's url is like [...]/vre/world.vre
    // try to build an url like [...]/jar/vrengapp.jar
    strcpy(l2, World::current()->getUrl());
    if ((last = strrchr(l2, '/'))) *last = '\0';
    if ((last = strrchr(l2, '/')) && (! stringcmp(last, "/vre"))) {
      *last = '\0';
      Url::abs(l2, url);
      strcat(url, DEF_URL_JAR);
    }
    else { // neither of the above methods worked, just put a default url
      sprintf(url, "http://%s/%s%s", ::g.server, ::g.urlpfx, DEF_URL_JAR);
    }
  }
}

Vrelet::Vrelet(char *l)
{
  o2 = NULL;
  wantDelta = false;
  needRedraw = false;
  dlist = 0;

  parser(l);

  // Vrelet objects can't currently collide with anything, they move through them
  enableBehavior(COLLIDE_GHOST);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);

  Vjc::startApp(this);
  ::g.gui.setToVrelet(this);
}

/* Send a notification to the child saying a click occured */
void Vrelet::sendClick(int x, int y)
{
  VjcMessage *msg = new VjcMessage(this, VJC_MSGT_CLICK, VJC_MSGV_CLICK);
  msg->put32(x);
  msg->put32(y);
  msg->sendData();
  delete msg;
}

/* Send some object's position to the child */
void Vrelet::sendPos(WObject *po)
{
  VjcMessage *msg = new VjcMessage(this, VJC_MSGT_POS, VJC_MSGV_SET);

  msg->putOID(po);
  msg->putPos(po);
  msg->sendData();
  delete msg;
}

/* Same as sendPos, except that no position is sent */
void Vrelet::sendPosError(uint8_t type, uint32_t ssrc, uint16_t port, uint16_t id)
{
  VjcMessage *msg = new VjcMessage(this, VJC_MSGT_POS, VJC_MSGV_ERROR);

  msg->put8(type);
  msg->put32(ssrc);
  msg->put16(port);
  msg->put16(id);
  msg->sendData();
  delete msg;
}

/* Answer a child's query */
void Vrelet::answerTypeQuery(int _type)
{
  VjcMessage *msg = new VjcMessage(this, VJC_MSGT_QUERY, VJC_MSGV_QANS);

  if (_type > 0) {
    int cnt = 0;
    for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
      if ((*it)->type == _type) cnt++;
    }
    msg->put32(cnt);
    for (list<WObject*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
      if ((*it)->type == _type) msg->putOID(*it);
    }
  }
  else {
    msg->put32(1);
    msg->putOID(localuser);
  }
  msg->sendData();
  delete msg;
}

/* Deal with input from the client app */
void Vrelet::processClient()
{
  bool processed = false;
  int tag;
  V3 color = setV3(0, 0, 0);	//black;

  // check if the client has sent anything
  VjcMessage *msg;
  if ((msg = Vjc::getData(this)) == NULL) return;
  tVjcHeader header = msg->getHeader();

  // Process the client's data
  switch (header.msg_type) {

  case VJC_MSGT_ADD:
    tag = msg->read16();
    color = msg->readPoint3D();

    if ( (header.msg_id == VJC_MSGV_LINE)
    ||   (header.msg_id == VJC_MSGV_LOOP)
    ||   (header.msg_id == VJC_MSGV_FILL) ) {
      // create a new line
      trace(DBG_IFC, "vrelet get new line");
      o2 = newObject2D(o2, header.msg_id, tag, color);
      while (msg->hasData(2*sizeof(int32_t))) {
        addPoint(o2, msg->readPoint2D());
      }
      processed = true;
    }
    else if (header.msg_id == VJC_MSGV_CIRCLE) {
      trace(DBG_IFC, "vrelet get new circle");
      // Create a new circle
      o2 = newObject2D(o2, header.msg_id, tag, color);
      addPoint(o2, msg->readPoint2D());
      addPoint(o2, msg->readPoint2D());
      processed = true;
    }
    needRedraw = true;
    break;

  case VJC_MSGT_DEL:
    if (header.msg_id == VJC_MSGV_DELALL) {
      freeObject2DList(o2);
      o2 = NULL;
    }
    else {
      tag = msg->read16();
      o2 = removeObject2D(o2, header.msg_id, tag);
    }
    needRedraw = true;
    break;

  case VJC_MSGT_POS: {
    // get the objet ID
    uint8_t  type = msg->read8();
    uint32_t src  = msg->read32();
    uint16_t port = msg->read16();
    uint16_t id   = msg->read16();

    // locate the object
    WObject *who = ((type==0) ? this : OList::findMobile(type, src, port, id));
    if (! who) {
      // we didn't find anything that matched
      sendPosError(type, src, port, id);
    }
    else if (header.msg_id == VJC_MSGV_ASK) {
      // message was a query - send the data back
      sendPos(who);
      processed = true;
    }
    else if ( (header.msg_id == VJC_MSGV_SET)
           || (header.msg_id == VJC_MSGV_UPD) ) {
      // message was a move, get the movement information
      copyV3(&posDelta, msg->readDelta());
      copyV3(&angDelta, msg->readDelta());

      if (who == this) {	// move myself (vrelet object)
        wantDelta = header.msg_id;
      }
      else {			// push someone else (vreng object)
        WObject *pold = new WObject();	// needed for collision

        // make the changes
        who->copyPositionAndBB(pold);
        deltaPos(who);

        // update the object in the Vreng 3D thingies
        who->updatePositionAndGrid(pold);

        // check if we need to move the camera (user)
        if (who == localuser) who->updateCamera(pos);

        // propagate the changes
        OList *vicinityList = who->getVicinity(pold);
        who->generalIntersect(pold, vicinityList);
        vicinityList->remove();
        delete pold;
      }
      processed = true;
    }
    break;
  }

  case VJC_MSGT_QUERY:
    if (header.msg_id == VJC_MSGV_QTYPE) {
      int type = msg->read32();
      answerTypeQuery(type);
      processed = true;
    }
    break;
  }

  if (! processed) { // ignore header.msg_type
    if (msg) delete msg;
    msg = NULL;
  }
}

/* React to a user click on our surface */
void Vrelet::click(V3 dir)
{
  // calculate the intersection between the click vector and our primary surface

  // get two principal vectors and a normal one
  V3 v, w, normal;
  getSurfVecs(pos, &v, &w, &normal);

  // check whether the click comes from the right side of the surface
  float sp = normal.v[0]*dir.v[0] + normal.v[1]*dir.v[1] + normal.v[2]*dir.v[2];
  if (sp < 0) { warning("Vrelet::click: bad side!"); return; }

  // eye position
  V3 e = setV3(localuser->pos.x, localuser->pos.y, localuser->pos.z + localuser->height/2 - 0.10);

  // object's center coordinates
  V3 c = setV3(pos.x, pos.y, pos.z);

  // determine (x,y) coord. relativly to our surface
  float det = detV3(dir, v, w);
  float varx = calcV3(c, e, w, dir) / det;
  float vary = calcV3(c, e, dir, v) / det;

  // scale these values
  int x = (int) (varx * (float) incrx / fx);
  int y = (int) (vary * (float) incry / fy);

  // notify the child
  trace(DBG_IFC, "Vrelet::click: x=%d y=%d", x, y);
  sendClick(x, y);
}

/* Dummy: always say we need to move */
void Vrelet::updateTime(time_t s, time_t us, float *lasting)
{
  *lasting = 1.;
}

void Vrelet::setPos(WObject *po)
{
  po->pos.x = posDelta.v[0];
  po->pos.y = posDelta.v[1];
  po->pos.z = posDelta.v[2];
  po->pos.az = angDelta.v[0];
  po->pos.ay = 0;
  po->pos.ax = angDelta.v[1];
}

void Vrelet::deltaPos(WObject *po)
{
  po->pos.x += posDelta.v[0];
  po->pos.y += posDelta.v[1];
  po->pos.z += posDelta.v[2];
  po->pos.az += angDelta.v[0];
  po->pos.ay += 0;
  po->pos.ax += angDelta.v[1];
}

/* Return yes if the child has sent a delta request */
bool Vrelet::isMoving()
{
  processClient();
  return (wantDelta > 0 || needRedraw);
}

/* Propagate the last deltas to the object's position */
void Vrelet::changePosition(float lasting)
{
  if      (wantDelta == VJC_MSGV_SET) setPos(this);
  else if (wantDelta == VJC_MSGV_UPD) deltaPos(this);
  wantDelta = 0;

  // have things changed since last time we build the dlist ?
  if (needRedraw) {
    needRedraw = false;

    draw();
  }
}

/* Send the client a notification that a thing just entered/exited its BB */
void Vrelet::sendIntersect(WObject *pcur, WObject *pold, int inOrOut)
{
  VjcMessage *msg = new VjcMessage(this, VJC_MSGT_ISEC, inOrOut);

  //echo("sendIntersect: %d name=%s", inOrOut, pcur->getInstance());
  msg->putOID(pcur);
  msg->putPos(pcur);
  msg->putPos(pold);
  msg->sendData();
  delete msg;
}

/* An ingoing intersection occured */
bool Vrelet::whenIntersect(WObject *pcur, WObject *pold)
{
  sendIntersect(pcur, pold, VJC_MSGV_ISECIN);
  return true;
}

/* An outgoing intersection occured */
bool Vrelet::whenIntersectOut(WObject *pcur, WObject *pold)
{
  sendIntersect(pcur, pold, VJC_MSGV_ISECOUT);
  return true;
}

/* Turn the child on */
void Vrelet::quit()
{
  glDeleteLists(dlist, 1);
  Vjc::stopApp(this);
  ::g.gui.setToVrelet(NULL);
}

/*
 * 2D Object manipulation
 */

/* 2D Creation */
tObject2D * Vrelet::newObject2D(tObject2D *o2l, int _type, int _tag, V3 _color)
{
  tObject2D *o2 = new tObject2D[1];
  o2->type = _type;
  o2->tag = _tag;
  copyV3(&o2->color, _color);
  o2->points = NULL;
  o2->next = o2l;
  return o2;
}

/* Remove one object from a list */
tObject2D * Vrelet::removeObject2D(tObject2D *o2l, int _type, int _tag)
{
  tObject2D *ret = o2l;
  tObject2D *prev = NULL;

  for (tObject2D *pl = o2l; pl; pl = pl->next) {
    if ((pl->type == _type) && (pl->tag == _tag)) {
      if (! prev)
        ret = pl->next;
      else {
        prev->next = pl->next;
        ret = o2l;
      }
      freeObject2D(pl);
        return ret;
    }
    prev = pl;
  }
  warning("attempt to remove a non-existing 2d object (%d %d)", _type, _tag);
  return o2l;
}

/* Free an object's point list */
void Vrelet::freePointList(tPointList *pts)
{
  while (pts) {
    tPointList *tmp;
    tmp = pts->next;
    delete[] pts;
    pts = tmp;
  }
}

/* Free an object2D and its point list */
void Vrelet::freeObject2D(tObject2D *_o2)
{
  if (! _o2) warning("attempt to free a null 2d object");
  else {
    freePointList(_o2->points);
    delete[] _o2;
    _o2 = NULL;
  }
}

/* Free an object2d list */
void Vrelet::freeObject2DList(tObject2D *o2l)
{
  tObject2D *pl = o2l;
  tObject2D *todel = o2l;

  while (pl) {
    todel = pl;
    pl = pl->next;
    freeObject2D(todel);
  }
}

/* Add a point to the point list */
void Vrelet::addPoint(tObject2D *_o2, V3 _point)
{
  tPointList *pts = new tPointList[1];

  trace(DBG_IFC, "addPoint: px=%.2f py=%.2f", _point.v[0], _point.v[1]);
  copyV3(&pts->point, _point);
  pts->next = _o2->points;
  _o2->points = pts;
}

/* Draws the Vrelet's 2D object list */
void Vrelet::draw()
{
  float rx = incrx / fx;
  float ry = incry / fy;

  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glTranslatef(0, 0, 0.02); // extra z so that the figures are drawn slightly
		            // above the surface
  while (o2) {
    trace(DBG_IFC, "draw2DList: o2->type=%d", o2->type);
    switch (o2->type) {

      case VJC_MSGV_LINE:
      case VJC_MSGV_LOOP:
      case VJC_MSGV_FILL: {
        tPointList *pts = o2->points;
        if (! pts) return;

        glColor3f(o2->color.v[0], o2->color.v[1], o2->color.v[2]);

        if (o2->type == VJC_MSGV_LINE)
          glBegin(GL_LINE_STRIP);
        else if (o2->type == VJC_MSGV_LOOP)
          glBegin(GL_LINE_LOOP);
        else
          glBegin(GL_POLYGON);

        for ( ; pts ; pts = pts->next) {
          glVertex2f(pts->point.v[0]/rx, pts->point.v[1]/ry);
        }
        glEnd();
        }
        break;

      case VJC_MSGV_CIRCLE: {
        V3 diam = o2->points->point;
        V3 center = o2->points->next->point;
        float arc = M_2PI / 20;

        glBegin(GL_LINE_LOOP);
        glColor3f(o2->color.v[0], o2->color.v[1], o2->color.v[2]);
        for (int i=19; i>=0; i--) {
          glVertex2f(center.v[0]/rx+diam.v[0]*cos(i*arc)/rx,
                     center.v[1]/ry+diam.v[1]*sin(i*arc)/rx);
        }
        glEnd();
        }
        break;
    }
    o2 = o2->next;
  }
  glEndList();
}

/* Displays the Vrelet's 2D object list */
void Vrelet::render()
{
  if (dlist) {
    glPushMatrix();
    glLoadName(dlist);
    float gl_mat[16];
    M4 posmat;
    getPosition(posmat);
    M4toV16(&posmat, gl_mat);
    glMultMatrixf(gl_mat);
    glCallList(dlist);
    glPopMatrix();
  }
}
