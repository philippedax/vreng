//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// render.cpp
//
// Rendering management
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "render.hpp"	// solid.hpp
#include "scene.hpp"	// getScene
#include "texture.hpp"	// init
#include "object.hpp"	// Object
#include "world.hpp"	// current
#include "bgcolor.hpp"	// Bgcolor
#include "user.hpp"	// USER_TYPE
#include "ground.hpp"	// GROUND_TYPE
#include "model.hpp"	// MODEL_TYPE
#include "pref.hpp"	// ::g.pref
#include "grid.hpp"	// Grid
#include "axis.hpp"	// Axis
#include "light.hpp"	// lights
#include "flare.hpp"	// render

#include <vector>	// vector
#include <list>		// list
#include <algorithm>	// qsort


// local
#define SEL_MAXNAMES    1024			// max names in buffer selection
const int Render::SEL_BUFSIZ=4*SEL_MAXNAMES;	// 1024 names
static GLuint selbuf[4*SEL_MAXNAMES];		// 1024 objects

extern struct Render::sCamera camera;		// camera.cpp


#if 0 //debug set to 1 to force debug tracing
#define DBG_3D DBG_FORCE
#endif //debug

/** Initialization. */
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

/** OpenGL initialization. */
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

    if (::g.pref.infogl == true) {
      echo("version=%s vendor=%s renderer=%s, depth=%d textures=%d stencil=%d clips=%d",
           glGetString(GL_VERSION),
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER),
           haveDepth(),
           haveTextures(),
           haveStencil(),
           haveClips()
          );
    }
    solidList.clear();	// clear solidList
    configured = true;
  }
  cameraUser();
}

/** Sets default materials */
void Render::materials()
{
  const GLfloat greyZ[] = {.2, .2, .2, 1};	// dark grey
  const GLfloat grey8[] = {.8, .8, .8, 1};	// light grey

  glMaterialfv(GL_FRONT, GL_AMBIENT, greyZ);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, grey8);
}

/** Records object number into the selection buffer */
void Render::recordObject(Object *o)
{
  if (! o->isBehavior(UNSELECTABLE)) {
    glPopName();
    glPushName((GLuint) o->num); // push number
  }
}

/** Compares distances  : called by sort() */
bool Render::compDist(const void *_s1, const void *_s2)
{
  Solid *s1 = (Solid *) _s1;
  Solid *s2 = (Solid *) _s2;

  // sort by decreasing order : furthest -> nearest
  return (s1->userdist > s2->userdist);
}

/** Compares surfaces  : called by sort() */
bool Render::compSize(const void *_s1, const void *_s2)
{
  Solid *s1 = (Solid *) _s1;
  Solid *s2 = (Solid *) _s2;

  // sort by decreasing order : largest -> smallest
  return (s1->surfsize > s2->surfsize);
}

/** Compares nbframes  : called by sort() */
bool Render::compFrame(const void *_s1, const void *_s2)
{
  Solid *s1 = (Solid *) _s1;
  Solid *s2 = (Solid *) _s2;

  // sort by increasing order : less -> more
  return (s1->nbframes < s2->nbframes);
}

/** Renders opaque solids */
void Render::renderOpaque(bool mini)
{
  trace2(DBG_3D, "\nopaque: ");
  // sort opaqueList
  if (! mini) {
    opaqueList.sort(compDist);	// sort distances decreasingly : large surfaces overlap
    opaqueList.sort(compSize);	// sort surfaces decreasingly : fix overlaping
    opaqueList.sort(compFrame);	// sort nbframes increasingly : fix overlaping (see maze.vre)
  }

  for (std::list<Solid*>::iterator it = opaqueList.begin(); it != opaqueList.end() ; ++it) {
    materials();
    recordObject((*it)->object);		// records the name before displaying it

    if ((*it)->object->isBehavior(SPECIFIC_RENDER)) {	// specific render
      trace2(DBG_3D, " %s:%.1f", (*it)->object->objectName(), (*it)->userdist);
      (*it)->object->render();
      if ((*it)->object->isBehavior(MIX_RENDER)) {	// mix render
        displaySolid((*it), OPAQUE);
      }
    }
    else {	// general render, no specific render
      if ((*it)->nbsolids == 1) {		// mono solid
        displaySolid((*it), OPAQUE);
      }
      else {					// multi solids
        glPushMatrix();
        displaySolid((*it), OPAQUE);	// main solid first
        trace2(DBG_3D, " %s:%.1f:%.1f#%d", (*it)->object->objectName(), (*it)->userdist, (*it)->surfsize, (*it)->nbsolids);

        for (std::list<Solid*>::iterator jt = relsolidList.begin(); jt != relsolidList.end() ; ++jt) {
          displaySolid((*it), OPAQUE);
          (*jt)->rendered = true;
          trace2(DBG_3D, " %s:%.1f:%.1f#%d", (*jt)->object->objectName(), (*jt)->userdist, (*jt)->surfsize, (*jt)->nbsolids);
        }
        glPopMatrix();
      }
    }
    (*it)->rendered = true;
  }
}

/** Renders transparent solids sorted from the furthest to the nearest */
void Render::renderTransparent(bool mini)
{
  trace2(DBG_3D, "\ntransparent: ");
  // build transparentList from solidList
  for (std::list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    if ( (*it)->opaque == false &&
         (*it)->visible &&
         ! (*it)->rendered &&
         ! (*it)->object->removed ) {
      transparentList.push_back(*it);	// add to transparent list
    }
  }

  // sort transparentList
  if (! mini) {
    transparentList.sort(compSize);	// sort surfaces decreasingly
    transparentList.sort(compDist);	// sort distances decreasingly
  }

  // render transparentList
  for (std::list<Solid*>::iterator it = transparentList.begin(); it != transparentList.end() ; ++it) {
    recordObject((*it)->object);	// records the name before displaying it

    if ((*it)->object->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object->render();
      if ((*it)->object->isBehavior(MIX_RENDER)) {	// mix render
        displaySolid((*it), TRANSPARENT);
      }
      trace2(DBG_3D, " %s:%.1f", (*it)->object->objectName(), (*it)->userdist);
    }
    else {
      if ((*it)->nbsolids == 1) {	// mono solid
        displaySolid((*it), TRANSPARENT);
        trace2(DBG_3D, " %s:%.1f", (*it)->object->objectName(), (*it)->userdist);
      }
      else {				// multi solids
        glPushMatrix();
        displaySolid((*it), TRANSPARENT);
        trace2(DBG_3D, " %s:%.1f#%d", (*it)->object->objectName(), (*it)->userdist, (*it)->nbsolids);

        for (std::list<Solid*>::iterator jt = relsolidList.begin(); jt != relsolidList.end() ; ++jt) {
          displaySolid((*it), TRANSPARENT);
          (*jt)->rendered = true;
          trace2(DBG_3D, " %s:%.1f#%d", (*jt)->object->objectName(), (*jt)->userdist, (*jt)->nbsolids);
        }
        glPopMatrix();
      }
    }
    (*it)->rendered = true;
  }
}

/** Renders ground solids sorted from the furthest to the nearest */
void Render::renderGround()
{
  trace2(DBG_3D, "\nground: ");
  groundList.sort(compDist);		// sort distances decreasingly
  for (std::list<Solid*>::iterator it = groundList.begin(); it != groundList.end() ; ++it) {
    materials();
    recordObject((*it)->object);	// records the name before displaying it
    if ((*it)->object->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object->render();
    }
    else {
      displaySolid((*it), OPAQUE);
    }
    (*it)->rendered = true;
    trace2(DBG_3D, " %s:%.1f", (*it)->object->objectName(), (*it)->userdist);
  }
}

/** Renders model solids sorted from the furthest to the nearest */
void Render::renderModel()
{
  trace2(DBG_3D, "\nmodel: ");
  modelList.sort(compDist);		// sort distances decreasingly
  for (std::list<Solid*>::iterator it = modelList.begin(); it != modelList.end() ; ++it) {
    materials();
    recordObject((*it)->object);	// records the name before displaying it
    //ok echo("%s", (*it)->object->objectName());
    if ((*it)->object->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object->render();
    }
    else {
      displaySolid((*it), OPAQUE);
    }
    (*it)->rendered = true;
    trace2(DBG_3D, " %s:%.1f", (*it)->object->objectName(), (*it)->userdist);
  }
}

/** Renders user */
void Render::renderUser()
{
  trace2(DBG_3D, "\nuser: ");
  for (std::list<Solid*>::iterator it = userList.begin(); it != userList.end() ; ++it) {
    recordObject((*it)->object);	// records the name before displaying it
    if ((*it)->object->isBehavior(SPECIFIC_RENDER)) {
      //echo("specific: %s/%s", (*it)->object->typeName(), (*it)->object->objectName());
      (*it)->object->render();
    }
    else {
      displaySolid((*it), USER);
    }
    (*it)->rendered = true;
    trace2(DBG_3D, " %s", (*it)->object->objectName());
    break;	// only one
  }
}

/** Renders flary solids */
void Render::renderFlary()
{
  trace2(DBG_3D, "\nflary: ");
  for (std::list<Solid*>::iterator it = flaryList.begin(); it != flaryList.end() ; ++it) {
    if ((*it)->object->isBehavior(SPECIFIC_RENDER)) {
      (*it)->object->render();
    }
    else {
      displaySolid((*it), FLASH);
    }
    (*it)->rendered = true;
    trace2(DBG_3D, " %s", (*it)->object->objectName());
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
  // sets rendered=false updateDist and find the localuser solid
  for (std::list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    (*it)->rendered = false;
    (*it)->updateDist();
    if ( (*it)->opaque && (*it)->visible ) {
      if ( (*it)->flashy || (*it)->flary ) {
        flaryList.push_back(*it);	// add to flary list
      }
      else if ( (*it)->object->type == GROUND_TYPE ) {
        groundList.push_back(*it);	// add to ground list
      }
      else if ( (*it)->object->type == MODEL_TYPE ) {
        modelList.push_back(*it);	// add to model list
      }
      else if ( (*it)->object->type == USER_TYPE ) {
        userList.push_back(*it);	// add to user list
      }
      else {
        opaqueList.push_back(*it);	// add to opaque list
      }
    }
  }

  // order is important !!!
  renderGround();		// renders ground solids first
  renderOpaque(mini);		// renders opaque solids
  renderTransparent(mini);	// renders transparent solids
  renderModel();		// renders model solids
  renderFlary();		// renders flary solids
  renderUser();			// renders localuser last
}


/**
 * General rendering loop.
 */
void Render::render()
{
  Texture::update();		// textures
  cameraPosition();		// camera user's eyes position
  refreshBackground();		// background color
  lighting();			// general lighting
  renderSolids(0);		// solids rendering
  Grid::grid()->render();	// render grid
  Axis::axis()->render();	// render axis
  scissors();			// render scissors
  trace2(DBG_3D, "\n");		// end of trace2 !
}

void Render::minirender()
{
  cameraPosition();		// camera user's eyes position
  refreshBackground();		// background color
  renderSolids(1);		// solids rendering
}

/** Renders scissors */
void Render::scissors()
{
  showSat();			// launch a satellite camera
  showMap();			// display map
  //dax showView();		// display view from object
}

/** Clears GL Buffer */
void Render::refreshBackground()
{
  if (flash) {
    ::g.gui.scene()->setBackground(UColor::white);
    flash = false;	// flash done
    return;
  }
  Bgcolor* bgcolor = World::current()->bgcolor;
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

/** Makes lightings */
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
  //trace2(DBG_3D, "\nlight:");
  for (std::vector<Object*>::iterator it = lightList.begin(); it != lightList.end() ; ++it) {
    if ((*it)->num) {	 //FIXME segfault sometimes : num replaces isValid()
      (*it)->lighting();
      //trace2(DBG_3D, " %s", (*it)->objectName());
    }
  }
}

/** Highlights same type of object. */
void Render::highlight(char *object_type, int flash)
{
  for (std::list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; it++) {
    if (! strcasecmp((*it)->object->typeName(), object_type)) {
      switch (flash) {
      case 0: if (*it) (*it)->flashy = false; break;
      case 1: if (*it) (*it)->flashy = true; break;
      }
    }
  }
}

/**
 * 3D Selection using picking algorithm.
 * Returns object number selectionned
 * called by camera and getPointedObject (widgets.cpp)
 */
uint16_t Render::bufferSelection(GLint x, GLint y)
{
  return bufferSelection(x, y, 0);
}

uint16_t Render::bufferSelection(GLint x, GLint y, GLint depth)
{
  //echo("-- depth: %d", depth);
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
  GLfloat top = camera.near * tan(camera.fovy * M_PI_180);
  GLfloat bot = -top;
  GLfloat ratio = static_cast<GLfloat> (w) / static_cast<GLfloat> (h);
  GLfloat right = top * ratio;
  GLfloat left = -right;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(left + (x-0.1) * (right-left) / (w-1),
	    left + (x+0.1) * (right-left) / (w-1),
	    top -  (y+0.1) * (top-bot) / (h-1),
	    top -  (y-0.1) * (top-bot) / (h-1),
	    camera.near, camera.far);
  glTranslatef(0, 0, -camera.near);

  cameraPosition();	// user"s eyes position

  // redraw the objects into the selection buffer
  renderSolids(1);	// mini render

  // we put the normal mode back
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // scan the selection buffer, find the nearest hit
  GLint hits = glRenderMode(GL_RENDER);
  GLuint** hitlist = new GLuint*[hits];
  GLuint *psel = selbuf;

  for (int hit=0; hit < hits; hit++) {
    if (::g.pref.trace) {
      echo("hit: %d/%d num=%d min=%u %s",
            hit, hits, psel[3], psel[1],
            Object::byNum(psel[3])->objectName());
    }
    hitlist[hit] = psel;
    psel += 3 + psel[0];	// next hit
  }

  uint16_t objnum_nearest = 0;
  uint16_t next = 0;

  if (hits > 0) {
    std::qsort((void *)hitlist, hits, sizeof(GLuint *), compareHit);
    int n = depth % hits;
    objnum_nearest = hitlist[n][3];
    if (hits > 1) next = hitlist[1][3];
    if (::g.pref.trace) {
      if (hits > 1) {
        echo("nearest: %d/%s next %d/%s",
              objnum_nearest, Object::byNum(objnum_nearest)->objectName(),
              next, Object::byNum(next)->objectName());
      }
      else {
        echo("nearest: %d/%s",
              objnum_nearest, Object::byNum(objnum_nearest)->objectName());
      }
    }
  }
  if (hitlist) delete[] hitlist;

  return objnum_nearest;
}

/** Qsort function for elements of the selection buffer. */
int Render::compareHit(const void *t1, const void *t2)
{
  GLuint s1 = (*((GLuint **)t1))[1];	// please don't touch that
  GLuint s2 = (*((GLuint **)t2))[1];

  if (s1 > s2)  return 1;
  else if (s1 < s2)  return -1;
  else  return 0;
}

/** Computes and returns bbmax, bbmin in Solid::parser. */
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

/** Computes min/max bounding box. */
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

/** Computes min/max bounding box. */
void Render::setBB(const V3 *v)
{
  if (first_bb) {
    for (int i=0; i<3; i++) {
      bbox_min.v[i] = v->v[i];
      bbox_max.v[i] = v->v[i];
    }
    first_bb = false;
  }
  else {
    for (int i=0; i<3; i++) {
      bbox_min.v[i] = MIN(v->v[i], bbox_min.v[i]);
      bbox_max.v[i] = MAX(v->v[i], bbox_max.v[i]);
    }
  }
}

/** Sets min/max bounding box. */
void Render::setBB(GLfloat w, GLfloat d, GLfloat h)
{
  bbox_max.v[0] = w;
  bbox_max.v[1] = d;
  bbox_max.v[2] = h;
  bbox_min.v[0] = -w;
  bbox_min.v[1] = -d;
  bbox_min.v[2] = -h;
}

/** Updates min/max bounding box according to its orientation. */
void Render::updBB(GLfloat az)
{
  bbox_max.v[0] *= cos(az);
  bbox_max.v[1] *= sin(az);
  bbox_min.v[0] *= cos(az);
  bbox_min.v[1] *= sin(az);
}

/**
 * Converts (wx,wy) window coordinates into a vector (dir) representing
 * the direction of the line from the user's eyes to the point he clicked on.
 * Vector dir is returned.
 * Static method.
 */
void Render::clickDirection(GLint wx, GLint wy, V3 *dir)
{
  GLint vp[4];

  ::g.gui.scene()->getScene(vp[0], vp[1], vp[2], vp[3]);	// get viewport

  GLfloat ex = localuser->pos.x + User::NEAR;
  GLfloat ey = localuser->pos.y;
  GLfloat ez = localuser->pos.z + localuser->height/2 - 0.10;
  GLfloat tx = static_cast<GLfloat> (User::FAR);
  GLfloat ty = vp[2]/2 - static_cast<GLfloat> (wx);
  GLfloat tz = vp[3]/2 - static_cast<GLfloat> (wy);

  if (ty < 0) ty = MAX(ty, -User::FAR); else ty = MIN(ty, User::FAR);
  if (tz < 0) tz = MAX(tz, -User::FAR); else tz = MIN(tz, User::FAR);
  dir->v[0] = static_cast<GLfloat> (tx - ex);
  dir->v[1] = static_cast<GLfloat> (ty - ey);
  dir->v[2] = static_cast<GLfloat> (tz - ez);
  //echo("wx=%d wy=%d dir=%.1f %.1f %.1f", wx, wy,dir->v[0], dir->v[1] ,dir->v[2]);
}

/** Gets the object list where each object have a type present in the given list. */
Object** Render::getVisibleObjects(char **listtype, int nbr, int *nbelems)
{
  int hits = 0;
  int nb = 0;

  Object **drawedlist = Render::getDrawedObjects(&hits);
  Object **objlist = static_cast<Object**>(malloc(hits*sizeof(Object*)));

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
  objlist = static_cast<Object**>(realloc(objlist, nb * sizeof(Object*)));

  *nbelems = nb;
  if (drawedlist) delete[] drawedlist;
  drawedlist = NULL;
  return objlist;
}

/** Returns the list of drawed object on the user's screen. */
Object** Render::getDrawedObjects(int *nbhit)
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
  GLfloat top = camera.near * tan(camera.fovy * M_PI_180);
  GLfloat bot = -top;
  GLfloat ratio = static_cast<GLfloat> (w) / static_cast<GLfloat> (h);
  GLfloat right = top * ratio;
  GLfloat left = -right;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(left + ((w/2)-(w/2)) * (right-left) / (w-1),
	    left + ((w/2)+(w/2)) * (right-left) / (w-1),
	    top -  ((h/2)+(h/2)) * (top-bot) / (h-1),
	    top -  ((h/2)-(h/2)) * (top-bot) / (h-1),
	    camera.near, camera.far);
  glTranslatef(0, 0, -camera.near);

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
  std::qsort((void *) hitlist, hits, sizeof(GLuint *), compareHit);

  int usercount = 0;
  if (! strcasecmp((Object::byNum((uint16_t) hitlist[0][3]))->typeName(), "User")) {
    hitlist[0] = NULL;
    usercount = 1;
  }

  Object** selectlist = new Object*[hits - usercount];

  for (int i=0; i < hits; i++) {
    if (hitlist[i]) {
      selectlist[i] = Object::byNum(hitlist[i][3]);
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

/** Prints stat */
void Render::stat()
{
  GLint dl = glGenLists(1);
  echo("### Graphic ###\ndisplay-lists   : %d", --dl);
}

/** Debug show solid list */
void Render::showSolidList()
{
  for (std::list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    echo("solidList: %s->%s", (*s)->object->typeName(), (*s)->object->objectName());
    if (! strcasecmp((*s)->object->typeName(), "User")) {
      echo("User: %.1f %.1f %.1f %.1f (%d)",
	   (*s)->object->pos.x,
	   (*s)->object->pos.y,
	   (*s)->object->pos.z,
	   (*s)->object->pos.az,
	   (*s)->visible
	  );
    }
  }
}

//---------------------------------------------------------------------------
//
// Rendering solids
//

/** Renders a solid */
void Render::displaySolid(Solid *s, int render_mode)
{
  if (s->blinking && (! s->toggleBlinking())) {
    return;		// pass one turn
  }
  switch (render_mode) {
  case OPAQUE:		// display opaque solids
    if (s->reflexive)
      displayList(s, REFLEXIVE);
    else
      displayList(s, NORMAL);
    break;
  case TRANSPARENT:	// display transparent solids 
    if (s->reflexive)
      displayList(s, REFLEXIVE);
    else
      displayList(s, NORMAL);
    break;
  case FLASH:		// display flashy edges
    if (s->flashy)
      displayList(s, FLASHY);
    if (s->flary)
      displayFlary(s);	// display attached flares
    if (s->ray_dlist)
      displayRay(s);
    break;
  case USER:		// display local user last
    displayList(s, NORMAL);
    break;
  }
}

void Render::displayRay(Solid *s)
{
  glPushAttrib(GL_LINE_BIT);
  glPushMatrix();
   glDisable(GL_LIGHTING);
   glEnable(GL_LINE_STIPPLE);

   glCallList(s->ray_dlist);

   glDisable(GL_LINE_STIPPLE);
   glEnable(GL_LIGHTING);
  glPopMatrix();
  glPopAttrib();
}

void Render::displayFlary(Solid *s)
{
  if (s->object->flare) {
    displayList(s, NORMAL);			// object alone

    glPushMatrix();
     glRotatef(RAD2DEG(localuser->pos.az), 0, 0, -1);	// one degree
     glTranslatef(s->object->pos.x, s->object->pos.y, s->object->pos.z);

     s->object->flare->render(s->object->pos);	// render the flare

    glPopMatrix();
  }
}

/** Renders a solid calling its displist. */
int Render::displayList(Solid *s, int display_mode = NORMAL)
{
  if (! s->displist)  return 0;			// no displaylist
  if (s->displist[s->frame] <= 0 )  return 0;	// no frame

  GLint bufs = GL_NONE;

  glPushMatrix();
  {
   // Transposes vreng to openGl coordinates system
   GLfloat gl_mat[16];
   M4toV16(&s->matpos, gl_mat);
   glMultMatrixf(gl_mat);	// openGl coordinates

   switch (display_mode) {
   case NORMAL:
     if (s->object->type == USER_TYPE) {	// if localuser
       User *user = static_cast<User *> (s->object);
       glPushMatrix();
        glTranslatef(user->pos.x, user->pos.y, user->pos.z);     // x y z
        glCallList(s->displist[s->frame]);	// display the localuser here !!!
       glPopMatrix();
     }
     else {			// normal object
       glEnable(GL_DEPTH_TEST);
       glDepthFunc(GL_LESS);
       glDepthMask(GL_TRUE);
       glPushMatrix();
        if ( s->rel[0] || s->rel[1] || s->rel[2] ) {	// relative positions
          glTranslatef(s->rel[0], s->rel[1], s->rel[2]);
        }
        if ( s->rel[3] || s->rel[4] ) {			// relative orientations
          glRotatef(RAD2DEG(s->rel[3]), 0, 0, 1);	// rel az
          glRotatef(RAD2DEG(s->rel[4]), 1, 0, 0);	// rel ax
        }
        if ( s->scalex != 1 || s->scaley != 1 || s->scalez != 1 ) {	// scaling
          glScalef(s->scalex, s->scaley, s->scalez);
        }
       glPopMatrix();
       if (s->texid >= 0) {			// textured
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, s->texid);
       }
       if (s->alpha < 1) {			// transparency
         glDepthMask(GL_FALSE);
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// without effect
       }
       if (s->fog[0] > 0) {			// fog effect
         glEnable(GL_FOG);
         glFogi(GL_FOG_MODE, GL_EXP);
         glFogf(GL_FOG_DENSITY, s->fog[0]);
         glFogfv(GL_FOG_COLOR, &s->fog[1]);
       }
       //echo("%s", s->object->objectName());
       glCallList(s->displist[s->frame]);	// display the solid here !!!
     }
     break;
   case FLASHY:
     glPushMatrix();
      glPolygonOffset(2., 1.);			// factor=2 unit=1
      glDisable(GL_POLYGON_OFFSET_FILL);	// wired mode
      glColor3fv(s->flashcol);
      glLineWidth(1);
      glScalef(1.03, 1.03, 1.03);		// 3%100 more
      glPolygonMode(GL_FRONT, GL_LINE);
      glCallList(s->displist[s->frame]);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glPopMatrix();
     break;
   case REFLEXIVE:
    glPushMatrix();
     glGetIntegerv(GL_DRAW_BUFFER, &bufs); 	// get the current color buffer being drawn to
     glClearStencil(0);				// set the clear value
     glClear(GL_STENCIL_BUFFER_BIT);		// clear the stencil buffer
     glStencilFunc(GL_ALWAYS, 1, 1);		// always pass the stencil test
     glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // operation to modify the stencil buffer
     glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
     glDrawBuffer(GL_NONE);			// disable drawing to the color buffer
     glEnable(GL_STENCIL_TEST);			// enable stencil
     glCallList(s->displist[s->frame]);		// display the mirror inside the stencil
     glStencilFunc(GL_ALWAYS, 1, 1);		// always pass the stencil test
     glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
     glDrawBuffer((GLenum) bufs);		// reenable drawing to color buffer
     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); 	// make stencil buffer read only
     glPushMatrix();
      GLdouble eqn[4] = {-0,-0,-1,0};		// clipping plane
      glClipPlane(GL_CLIP_PLANE0, eqn);
      glEnable(GL_CLIP_PLANE0);			// enable clipping
      glStencilFunc(GL_EQUAL, 1, 1);		// draw only where the stencil == 1
      s->object->render();			// display the mirrored scene by mirror itself
      glDisable(GL_CLIP_PLANE0);
     glPopMatrix();
     glDisable(GL_STENCIL_TEST);		// disable the stencil
     glDepthMask(GL_FALSE);
     glEnable(GL_BLEND);			// mirror shine effect
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthFunc(GL_LEQUAL);
     glCallList(s->displist[s->frame]);		// display the physical mirror
     glDepthFunc(GL_LESS);
     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
    glPopMatrix();
    break;
   }
   if (*s->fog > 0) {			// fog effect
     glDisable(GL_FOG);
   }
   if (s->texid >= 0) {			// textured
     glDisable(GL_TEXTURE_2D);
     //glDisable(GL_LIGHTING);
   }
   if (s->alpha < 1) {			// transparency
     glDisable(GL_BLEND);
     glDepthMask(GL_TRUE);
   }
  }
  glPopMatrix();
  return s->displist[s->frame];		// displaylist number
}
