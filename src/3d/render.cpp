//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "render.hpp"
#include "solid.hpp"	// Solid, object()
#include "scene.hpp"	// setViewpoint
#include "texture.hpp"	// init
#include "wobject.hpp"	// WObject
#include "world.hpp"	// current
#include "bgcolor.hpp"	// Bgcolor
#include "user.hpp"	// USER_TYPE
#include "ground.hpp"	// GROUND_TYPE
#include "model.hpp"	// MODEL_TYPE
#include "guide.hpp"	// GUIDE_TYPE
#include "pref.hpp"	// ::g.pref
#include "grid.hpp"	// Grid
#include "axis.hpp"	// Axis
#include "light.hpp"	// lights

#include <vector>
#include <list>
#include <algorithm>

using namespace std;


// local
const int Render::SEL_BUFSIZ = (4*128);		// 128 names
static GLuint selbuf[4*128];			// 128 objects

extern struct Render::sCamera cam_user;		// camera.cpp


/* Initialization. */
Render::Render()
{
  first_bb = false;	// mono solid
  flash = false;

  // camera defaults
  view = VIEW_FIRST_PERSON;
  viewMap = false;
  viewSat = false;
  viewObj = false;
  third_yRot = 0;
  third_xRot = 0;
  third_Near = 0;
  turna = 0;		// turn around angle
  pitch = 0;
  
  // texture cache
  Texture::init();
}

GLint Render::haveDepth()
{
  GLint depth = 0;
  glGetIntegerv(GL_DEPTH_BITS, &depth);
  return depth;
}

GLint Render::haveTextures()
{
  GLint textures = 0;
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textures);
  return textures;
}

GLint Render::haveStencil()
{
  GLint stencil = 0;
  glEnable(GL_STENCIL_TEST);
  glGetIntegerv(GL_STENCIL_BITS, &stencil);
  return stencil;
}

GLint Render::haveClips()
{
  GLint clips = 0;
  glGetIntegerv(GL_MAX_CLIP_PLANES, &clips);
  return clips;
}

/* OpenGL initialization. */
void Render::init(bool _quality)
{
  static bool configured = false;

  if (! configured) {
    glFrontFace(GL_CCW);		// trigo counter clock wise
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_CULL_FACE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
    // anti aliasing
    if (_quality) {
      glEnable(GL_LINE_SMOOTH);
      glEnable(GL_POLYGON_SMOOTH);
    }
    else {
      glDisable(GL_LINE_SMOOTH);
      glDisable(GL_POLYGON_SMOOTH);
    }
  
    GLenum hint = _quality ? GL_NICEST : GL_FASTEST;
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, hint);
    glHint(GL_POINT_SMOOTH_HINT, hint);
    glHint(GL_LINE_SMOOTH_HINT, hint);
    glHint(GL_POLYGON_SMOOTH_HINT, hint);
    glHint(GL_FOG_HINT, hint);

    if (::g.pref.infogl == true)
      echo("version=%s vendor=%s renderer=%s, depth=%d textures=%d stencil=%d clips=%d",
           glGetString(GL_VERSION),
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER),
           haveDepth(),
           haveTextures(),
           haveStencil(),
           haveClips()
          );

    solidList.clear();	// clear solidList
    configured = true;
  }
  camera();
}

void Render::materials()
{
  const GLfloat ambient[] = {0.2, 0.2, 0.2, 1};	// dark grey
  const GLfloat color[] =   {0.8, 0.8, 0.8, 1};	// light grey

  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
}

// DEBUG //
#if 0 // set to 1 to force debug tracing
#define DBG_VGL DBG_FORCE
#endif //debug

/** Records object number into the selection buffer */
void Render::putSelbuf(WObject *obj)
{
  if (obj->isSelectable()) {
    glPopName();
    glPushName((GLuint) obj->num); // push number
  }
}

// Compares distances  : called by sort()
bool Render::compDist(const void *t1, const void *t2)
{
  Solid *s1 = (Solid *) t1;
  Solid *s2 = (Solid *) t2;

  // sort by decreasing order : furthest -> nearest
  return (s1->userdist > s2->userdist);
}

// Compares surfaces  : called by sort()
bool Render::compSize(const void *t1, const void *t2)
{
  Solid *s1 = (Solid *) t1;
  Solid *s2 = (Solid *) t2;

  // sort by decreasing order : largest -> smallest
  return (s1->surfsize > s2->surfsize);
}

// Compares nbframes  : called by sort()
bool Render::compFrame(const void *t1, const void *t2)
{
  Solid *s1 = (Solid *) t1;
  Solid *s2 = (Solid *) t2;

  // sort by increasing order : less -> more
  return (s1->nbframes < s2->nbframes);
}

// Renders opaque solids
void Render::renderOpaque(bool mini)
{
  trace2(DBG_VGL, "\nopaque: ");
  // sort opaqueList
  if (! mini) {
    opaqueList.sort(compDist);	// sort distances decreasingly : large surfaces overlap
    opaqueList.sort(compSize);	// sort surfaces decreasingly : fix overlaping
    opaqueList.sort(compFrame);	// sort nbframes increasingly : fix overlaping (see maze.vre)
  }

  for (list<Solid*>::iterator it = opaqueList.begin(); it != opaqueList.end() ; ++it) {
    materials();
    putSelbuf((*it)->object());		// records the name before displaying it

    if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {	// specific render
      (*it)->object()->render();
      if ( (*it)->object()->type == GUIDE_TYPE ) {	// exception: guide is both
        (*it)->displaySolid(Solid::OPAQUE);
      }
      trace2(DBG_VGL, " %s:%.1f", (*it)->object()->getInstance(), (*it)->userdist);
    }
    else {	// general render, no specific render
      if ((*it)->nbsolids == 1) {		// mono solid
        (*it)->displaySolid(Solid::OPAQUE);
        trace2(DBG_VGL, " %s:%.1f", (*it)->object()->getInstance(), (*it)->userdist);
      }
      else {					// multi solids
        (*it)->displaySolid(Solid::OPAQUE);	// main solid first
        trace2(DBG_VGL, " %s:%.1f:%.1f#%d", (*it)->object()->getInstance(), (*it)->userdist, (*it)->surfsize, (*it)->nbsolids);

        for (list<Solid*>::iterator jt = relsolidList.begin(); jt != relsolidList.end() ; ++jt) {
          (*jt)->displaySolid(Solid::OPAQUE);
          (*jt)->setRendered(true);
          trace2(DBG_VGL, " %s:%.1f:%.1f#%d", (*jt)->object()->getInstance(), (*jt)->userdist, (*jt)->surfsize, (*jt)->nbsolids);
        }
      }
    }
    (*it)->setRendered(true);
  }
}

// Renders transparent solids sorted from the furthest to the nearest
void Render::renderTransparent(bool mini)
{
  trace2(DBG_VGL, "\ntransparent: ");
  // build transparentList from solidList
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    if ( (*it)->isOpaque() == false &&
         (*it)->isVisible() &&
         ! (*it)->isRendered() &&
         ! (*it)->object()->isRemoved() ) {
      transparentList.push_back(*it);	// add to transparent list
    }
  }

  // sort transparentList
  if (! mini) {
    transparentList.sort(compSize);	// sort surfaces decreasingly
    transparentList.sort(compDist);	// sort distances decreasingly
  }

  // render transparentList
  for (list<Solid*>::iterator it = transparentList.begin(); it != transparentList.end() ; ++it) {
    putSelbuf((*it)->object());		// records the name before displaying it

    if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object()->render();
      trace2(DBG_VGL, " %s:%.1f", (*it)->object()->getInstance(), (*it)->userdist);
    }
    else {
      if ((*it)->nbsolids == 1) {		// mono solid
        (*it)->displaySolid(Solid::TRANSPARENT);
        trace2(DBG_VGL, " %s:%.1f", (*it)->object()->getInstance(), (*it)->userdist);
      }
      else {					// multi solids
        (*it)->displaySolid(Solid::TRANSPARENT);
        trace2(DBG_VGL, " %s:%.1f#%d", (*it)->object()->getInstance(), (*it)->userdist, (*it)->nbsolids);

        for (list<Solid*>::iterator jt = relsolidList.begin(); jt != relsolidList.end() ; ++jt) {
          (*jt)->displaySolid(Solid::TRANSPARENT);
          (*jt)->setRendered(true);
          trace2(DBG_VGL, " %s:%.1f#%d", (*jt)->object()->getInstance(), (*jt)->userdist, (*jt)->nbsolids);
        }
      }
    }
    (*it)->setRendered(true);
  }
}

// Renders model solids sorted from the furthest to the nearest
void Render::renderGround()
{
  trace2(DBG_VGL, "\nground: ");
  groundList.sort(compDist);		// sort distances decreasingly
  for (list<Solid*>::iterator it = groundList.begin(); it != groundList.end() ; ++it) {
    materials();
    putSelbuf((*it)->object());		// records the name before displaying it
    if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object()->render();
    }
    else {
      (*it)->displaySolid(Solid::OPAQUE);
    }
    (*it)->setRendered(true);
    trace2(DBG_VGL, " %s:%.1f", (*it)->object()->getInstance(), (*it)->userdist);
  }
}

// Renders model solids sorted from the furthest to the nearest
void Render::renderModel()
{
  trace2(DBG_VGL, "\nmodel: ");
  modelList.sort(compDist);		// sort distances decreasingly
  for (list<Solid*>::iterator it = modelList.begin(); it != modelList.end() ; ++it) {
    materials();
    putSelbuf((*it)->object());		// records the name before displaying it
    if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object()->render();
    }
    else {
      (*it)->displaySolid(Solid::OPAQUE);
    }
    (*it)->setRendered(true);
    trace2(DBG_VGL, " %s:%.1f", (*it)->object()->getInstance(), (*it)->userdist);
  }
}

// Renders user
void Render::renderUser()
{
  trace2(DBG_VGL, "\nuser: ");
  for (list<Solid*>::iterator it = userList.begin(); it != userList.end() ; ++it) {
    putSelbuf((*it)->object());		// records the name before displaying it
    if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
      //echo("specific: %s/%s", (*it)->object()->typeName(), (*it)->object()->getInstance());
      (*it)->object()->render();
    }
    else {
      //echo("general: %s/%s", (*it)->object()->typeName(), (*it)->object()->getInstance());
      (*it)->displaySolid(Solid::USER);
    }
    (*it)->setRendered(true);
    trace2(DBG_VGL, " %s", (*it)->object()->getInstance());
    break;	// only one
  }
}

// Renders flary solids
void Render::renderFlary()
{
  trace2(DBG_VGL, "\nflary: ");
  for (list<Solid*>::iterator it = flaryList.begin(); it != flaryList.end() ; ++it) {
    if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object()->render();
    }
    else {
      (*it)->displaySolid(Solid::FLASH);
    }
    (*it)->setRendered(true);
    trace2(DBG_VGL, " %s", (*it)->object()->getInstance());
  }
}

/**
 *
 * General Rendering
 * 
 * Render all objects and their solids.
 * - makes specific rendering for special objects (SPECIFIC_RENDER)
 * - else renders solids in their own displaylists
 */
void Render::renderSolids(bool mini)
{
  // build rendering lists from solidList
  opaqueList.clear();
  transparentList.clear();
  groundList.clear();
  modelList.clear();
  userList.clear();
  flaryList.clear();

  // for all solids
  // set setRendered=false updateDist and find the localuser solid
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    (*it)->setRendered(false);
    (*it)->updateDist();
    if ( (*it)->isOpaque() && (*it)->isVisible() ) {
      if ( (*it)->isFlashy() || (*it)->isFlary() ) {
        flaryList.push_back(*it);	// add to flary list
      }
      else if ( (*it)->object()->type == GROUND_TYPE ) {
        groundList.push_back(*it);	// add to ground list
      }
      else if ( (*it)->object()->type == MODEL_TYPE ) {
        modelList.push_back(*it);	// add to model list
      }
      else if ( (*it)->object()->type == USER_TYPE ) {
        userList.push_back(*it);	// add to user list
      }
      else {
        opaqueList.push_back(*it);	// add to opaque list
      }
    }
  }

  // order is important !!!
  renderGround();	// renders ground solids first
  renderOpaque(mini);	// renders opaque solids
  renderTransparent(mini);	// renders transparent solids
  if (! mini) {
    renderModel();	// renders model solids
    renderFlary();	// renders flary solids
    renderUser();	// renders localuser last
  }
}


/*
 * General rendering loop.
 */
void Render::render()
{
  Texture::update();		// textures
  cameraPosition();		// camera position
  clearBuffer();		// background color
  lighting();			// general lighting
  renderSolids(0);		// solids rendering
  Grid::grid()->render();	// render grid
  Axis::axis()->render();	// render axis
  scissors();			// render scissors
  trace2(DBG_VGL, "\n");	// end of trace2 !
}

void Render::minirender()
{
  cameraPosition();		// camera position
  clearBuffer();		// background color
  renderSolids(1);		// solids rendering
}

void Render::scissors()
{
  showSat();			// launch a satellite camera
  showMap();			// display map
  //dax showView();			// display view from object
}

void Render::clearBuffer()
{
  if (flash) {
    ::g.gui.scene()->setBackground(UColor::white);
    flash = false;	// flash done
    return;
  }
  Bgcolor* bgcolor = World::current()->backgroundColor();
  if (bgcolor) {
    UColor bg(bgcolor->color[0], bgcolor->color[1], bgcolor->color[2]);
    ::g.gui.scene()->setBackground(bg);
  }
  else {
    ::g.gui.scene()->setBackground(UColor::black);
  }
  glClearDepth(1);	// depth of Z-buffer to clear

  // !NOTICE: do not do GL_COLOR_BUFFER_BIT because this would clear the color
  // of the whole window (including the background color of the widgets)
  glClear(  //GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
          | GL_STENCIL_BUFFER_BIT
          //| GL_ACCUM_BUFFER_BIT
         );
}

void Render::lighting()
{
  const GLfloat light0_ambient[]  = {0.3, 0.3, 0.3, 1};
  const GLfloat light0_diffuse[]  = {1, 1, 1, 1};
  const GLfloat light0_specular[] = {1, 1, 1, 1};
  const GLfloat light0_position[] = {1, -1, 2, 0}; // directionnel
  const GLfloat light1_ambient[]  = {0.2, 0.2, 0.2, 1};
  const GLfloat light1_diffuse[]  = {1, 1, 1, 1};
  const GLfloat light1_specular[] = {1, 1, 1, 1};
  const GLfloat light1_position[] = {-0.5, 0.7, -1, 0};

  glEnable(GL_NORMALIZE);

  if (glIsEnabled(GL_LIGHT2)) {	//Hack! see light.cpp
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
  }
  else {	// general case
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT3);	// sun
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
  }
  glEnable(GL_LIGHTING);

  // renders other lights for example sun, moon, lamp
  //trace2(DBG_VGL, "\nlight:");
  for (list<WObject*>::iterator it = lightList.begin(); it != lightList.end() ; ++it) {
    if ((*it)->num) {	 //FIXME segfault sometimes : num replaces isValid()
      (*it)->lighting();
      //trace2(DBG_VGL, " %s", (*it)->getInstance());
    }
  }
}

/* highlight same type object. */
void Render::setAllTypeFlashy(char *object_type, int typeflash)
{
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; it++) {
    if (! strcasecmp((*it)->object()->typeName(), object_type)) {
      switch (typeflash) {
      case 0: if (*it) (*it)->isflashy = false; break;
      case 1: if (*it) (*it)->isflashy = true; break;
      }
    }
  }
}

/*
 * 3D Selection using picking method.
 * Returns object number selectionned
 * called by camera and getPointedObject (widgets.cpp)
 */
uint16_t Render::bufferSelection(GLint x, GLint y)
{
  return bufferSelection(x, y, 0);
}

uint16_t Render::bufferSelection(GLint x, GLint y, GLint depth)
{
  GLint max, cur = 0;
  glGetIntegerv(GL_NAME_STACK_DEPTH, &cur);
  glGetIntegerv(GL_MAX_NAME_STACK_DEPTH, &max);
  //echo("bufselect: x=%d y=%d names:%d/%d", x, y, cur, max);

  // set selection mode
  memset(selbuf, 0, sizeof(selbuf));
  glSelectBuffer(sizeof(selbuf), selbuf);
  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  GLint w = vp[2];
  GLint h = vp[3];
  GLfloat top = cam_user.near * tan(cam_user.fovy * M_PI_180);
  GLfloat bot = -top;
  GLfloat ratio = (GLfloat) w / (GLfloat) h;
  GLfloat right = top * ratio;
  GLfloat left = -right;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(left + (x-0.1) * (right-left) / (w-1),
	    left + (x+0.1) * (right-left) / (w-1),
	    top -  (y+0.1) * (top-bot) / (h-1),
	    top -  (y-0.1) * (top-bot) / (h-1),
	    cam_user.near, cam_user.far);
  glTranslatef(0, 0, -cam_user.near);

  // user"s eyes position
  cameraPosition();

  // redraw the objects into the selection buffer
  renderSolids(1);	// mini render

  // we put the normal mode back
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // scan the selection buffer, find the nearest hit
  GLint hits = glRenderMode(GL_RENDER);
  GLuint** hitlist = new GLuint*[hits];
  GLuint *psel = selbuf;
  for (int i=0; i < hits; i++) {
    if (::g.pref.dbgtrace) {
      echo("hit=%d/%d num=%d min=%ud name=%s/%s",
            i, hits, psel[3], psel[1],
            WObject::byNum(psel[3])->typeName(),WObject::byNum(psel[3])->getInstance());
    }
    hitlist[i] = psel;
    psel += 3 + psel[0];	// next hit
  }

  uint16_t nearest = 0;

  if (hits > 0) {
    qsort((void *)hitlist, hits, sizeof(GLuint *), compareHit);
    int n = depth % hits;
    nearest = hitlist[n][3];
    if (::g.pref.dbgtrace) echo("n=%d num=%d name=%s", n, nearest, WObject::byNum(nearest)->getInstance());
  }
  if (hitlist) delete[] hitlist;

  return nearest;
}

/* Qsort function for elements of the selection buffer. */
int Render::compareHit(const void *t1, const void *t2)
{
  GLuint s1 = (*((GLuint **)t1))[1];	// please don't touch that
  GLuint s2 = (*((GLuint **)t2))[1];

  if (s1 > s2)  return 1;
  else if (s1 < s2)  return -1;
  else  return 0;
}

/* Computes and returns bbmax, bbmin in Solid::parser. */
void Render::getBB(V3& bbmax, V3& bbmin, bool framed)
{
  if (! framed) {	// one frame
    bbmin = bbox_min;
    bbmax = bbox_max;
  }
  else {		// frames
    for (int i=0; i<3; i++) {
      bbmin.v[i] = MIN(bbox_min.v[i], bbmin.v[i]);
      bbmax.v[i] = MAX(bbox_max.v[i], bbmax.v[i]);
    }
  }
}

/* Computes bounding box. */
void Render::setBB(const GLfloat *v)
{
  if (first_bb) {
    for (int i=0; i<3; i++) {
      bbox_min.v[i] = v[i];
      bbox_max.v[i] = v[i];
    }
    first_bb = false;
  }
  else {
    for (int i=0; i<3; i++) {
      bbox_min.v[i] = MIN(v[i], bbox_min.v[i]);
      bbox_max.v[i] = MAX(v[i], bbox_max.v[i]);
    }
  }
}

/* Sets bounding box. */
void Render::setBB(GLfloat w, GLfloat d, GLfloat h)
{
  bbox_max.v[0] = w;
  bbox_max.v[1] = d;
  bbox_max.v[2] = h;
  bbox_min.v[0] = -w;
  bbox_min.v[1] = -d;
  bbox_min.v[2] = -h;
}

/* Updates bounding box according to its orientation. */
void Render::updateBB(GLfloat az)
{
  bbox_max.v[0] *= cos(az);
  bbox_max.v[1] *= sin(az);
  bbox_min.v[0] *= cos(az);
  bbox_min.v[1] *= sin(az);
}

/*
 * Converts (wx,wy) window coordinates into a vector (dir) representing
 * the direction of the line from the user's eyes to the point he clicked on.
 * Vector dir is returned.
 * Static method.
 */
void Render::clickDirection(GLint wx, GLint wy, V3 *dir)
{
  GLint vp[4];

  //dax glGetIntegerv(GL_VIEWPORT, vp);
  ::g.gui.scene()->getCoords(vp[0], vp[1], vp[2], vp[3]);	// get viewport

  GLfloat ex = localuser->pos.x + User::NEAR;
  GLfloat ey = localuser->pos.y;
  GLfloat ez = localuser->pos.z + localuser->height/2 - 0.10;
  GLfloat tx = (GLfloat) User::FAR;
  GLfloat ty = vp[2]/2 - (GLfloat) wx;
  GLfloat tz = vp[3]/2 - (GLfloat) wy;

  if (ty < 0) ty = MAX(ty, -User::FAR); else ty = MIN(ty, User::FAR);
  if (tz < 0) tz = MAX(tz, -User::FAR); else tz = MIN(tz, User::FAR);
  dir->v[0] = (GLfloat) (tx - ex);
  dir->v[1] = (GLfloat) (ty - ey);
  dir->v[2] = (GLfloat) (tz - ez);
  //echo("wx=%d wy=%d o1=%.2f %.2f %.2f o2=%.2f %.2f %.2f dir=%.2f %.2f %.2f", wx,wy,ex,ey,ez,tx,ty,tz,dir->v[0],dir->v[1],dir->v[2]);
}

/*
 * Get the object list where each object have a type present in the given list.
 */
WObject** Render::getVisibleObjects(char **listtype, int nbr, int *nbelems)
{
  int hits = 0;
  int nb = 0;

  WObject **drawedlist = Render::getDrawedObjects(&hits);
  WObject **objlist = (WObject**) malloc(hits*sizeof(WObject*));

  for (int i=0; i < hits ; i++) {
    for (int j=0; j < nbr ; j++) {
      //if the object has the same type and is in the sight view
      if ((drawedlist[i])
	  && (! strcasecmp(listtype[j], drawedlist[i]->typeName()))
	  && (drawedlist[i]->isSeen())) {
	objlist[nb] = drawedlist[i];
	nb++;
	break;
      }
    }
  }
  objlist = (WObject**) realloc(objlist, nb * sizeof(WObject*));

  *nbelems = nb;
  if (drawedlist) delete[] drawedlist;
  drawedlist = NULL;
  return objlist;
}

/* Returns the list of drawed object on the user's screen. */
WObject** Render::getDrawedObjects(int *nbhit)
{
  // set selection mode
  glSelectBuffer(sizeof(selbuf), selbuf);
  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  GLint w = vp[2];
  GLint h = vp[3];
  GLfloat top = cam_user.near * tan(cam_user.fovy * M_PI_180);
  GLfloat bot = -top;
  GLfloat ratio = (GLfloat) w / (GLfloat) h;
  GLfloat right = top * ratio;
  GLfloat left = -right;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(left + ((w/2)-(w/2)) * (right-left) / (w-1),
	    left + ((w/2)+(w/2)) * (right-left) / (w-1),
	    top -  ((h/2)+(h/2)) * (top-bot) / (h-1),
	    top -  ((h/2)-(h/2)) * (top-bot) / (h-1),
	    cam_user.near, cam_user.far);
  glTranslatef(0, 0, -cam_user.near);

  // eyes position
  cameraPosition();

  // redraws the objects into the selection buffer
  renderSolids(1);	// mini render

  // we put the normal mode back
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // scan the selection buffer
  GLint hits = glRenderMode(GL_RENDER);
  if (hits <= 0)  return NULL;

  GLuint** hitlist = new GLuint*[hits];
  GLuint *psel = selbuf;
  for (int i=0; i < hits; i++) {
    hitlist[i] = psel;
    psel += 3 + psel[0];	// next hit
  }
  qsort((void *) hitlist, hits, sizeof(GLuint *), compareHit);

  int usercount = 0;
  if (! strcasecmp((WObject::byNum((uint16_t) hitlist[0][3]))->typeName(), "User")) {
    hitlist[0] = NULL;
    usercount = 1;
  }

  WObject** selectlist = new WObject*[hits - usercount];

  for (int i=0; i < hits; i++) {
    if (hitlist[i]) {
      selectlist[i] = WObject::byNum(hitlist[i][3]);
    }
  }
  if (hitlist) delete[] hitlist;
  *nbhit = hits - usercount;

  return selectlist;
}

void Render::setFlash()
{
  flash = true;
}

void Render::stat()
{
  GLint dlist = glGenLists(1);
  echo("### Graphic ###\ndisplay-lists   : %d", --dlist);
}

/* Debug */
void Render::showSolidList()
{
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    echo("solidList: %s->%s", (*s)->object()->typeName(),(*s)->object()->getInstance());
    if (! strcasecmp((*s)->object()->typeName(), "User")) {
      echo("User: %.1f %.1f %.1f %.1f %.1f (%d)",
	   (*s)->object()->pos.x,
	   (*s)->object()->pos.y,
	   (*s)->object()->pos.z,
	   (*s)->object()->pos.ax,
	   (*s)->object()->pos.az,
	   (*s)->isVisible()
	  );
    }
  }
}

void Render::quit()
{
  //dax Texture::quit();
}
