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
#include "render.hpp"
#include "scene.hpp"	// getScene
#include "solid.hpp"	// Solid, object()
#include "texture.hpp"	// initCache
#include "wobject.hpp"	// WObject
#include "world.hpp"	// current
#include "bgcolor.hpp"	// Bgcolor
#include "user.hpp"	// USER_TYPE
#include "mirror.hpp"	// MIRROR_TYPE
#include "vicinity.hpp"	// Vicinity
#include "pref.hpp"	// ::g.pref
#include "grid.hpp"	// Grid
#include "axis.hpp"	// Axis
#include "light.hpp"	// lights

#include <vector>
#include <list>

using namespace std;


// local
const int Render::SEL_BUFSIZ = (4*1024);	// 1024 names
static GLuint selbuf[4*1024];			// 1024 objects

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
  thirdPerson_yRot = 0;
  thirdPerson_xRot = 0;
  thirdPerson_Near = 0;
  turnAround = 0;
  pitch = 0;
  
  // texture cache
  Texture::initCache();
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
      trace(DBG_FORCE, "version=%s vendor=%s renderer=%s, depth=%d textures=%d stencil=%d clips=%d",
            glGetString(GL_VERSION),
            glGetString(GL_VENDOR),
            glGetString(GL_RENDERER),
            haveDepth(),
            haveTextures(),
            haveStencil(),
            haveClips());

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
  //dax2 glMaterialfv(GL_FRONT, GL_SPECULAR, color);
  //dax2 glMaterialfv(GL_FRONT, GL_EMISSION, color);
}

/** Puts object number into the buffer selection */
void Render::putSelbuf(WObject *po)
{
  if (po && po->isValid() && po->isSelectable()) {
    glPopName();
    glPushName((GLuint) (long) po->num & 0xffffffff); // hack for 64 bits
  }
}

// DEBUG //
#if 1 //dax0 set to 1 to debug
#define DBG_VGL DBG_FORCE
#endif

/**
 * Specific rendering to do by objects themselves
 * by scanning objects lists
 */

// Renders specific objects
void Render::renderSpecific()  //dax8
{
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end(); ++it) {
    if ( (*it)->object()->isBehavior(SPECIFIC_RENDER) &&
         !(*it)->isRendered() &&
         !(*it)->isflashy &&
         !(*it)->isflary ) {
      (*it)->setRendered(true);
      //dax7 materials();
      putSelbuf((*it)->object());
      (*it)->object()->render();	// render done by object itself
      trace2(DBG_VGL, " %s", (*it)->object()->getInstance());
    }
  }
}

// Renders opaque solids without prior
void Render::renderOpaque(bool zsel)
{
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    if ( (*it)->isOpaque() &&
         (*it)->isVisible() &&
         !(*it)->isRendered() &&
         !(*it)->object()->removed &&
         !(*it)->isflashy &&
         !(*it)->isflary ) {
      putSelbuf((*it)->object());
      //dax7 materials();
      if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
        (*it)->object()->render();
      }
      else {
        (*it)->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::OPAQUE);
      }
      //dax8 hack! FIXME!
      if ((*it)->object()->typeName() != "Clock" && (*it)->object()->typeName() != "Guy")
      (*it)->setRendered(true);
      trace2(DBG_VGL, " %s/%s", (*it)->object()->typeName(), (*it)->object()->getInstance());
    }
  }
}

// Renders opaque solids
void Render::renderOpaque(bool zsel, list<Solid*>::iterator su, uint8_t pri)
{
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    //TODO if ((*it)->object()->isSeen() == false) continue;  // not seen
    if ( (*it)->isOpaque() &&
         (*it)->isVisible() &&
         !(*it)->isRendered() &&
         !(*it)->object()->removed &&
         !(*it)->isflashy &&
         !(*it)->isflary &&
         (*it)->object()->prior == pri	//dax8 if commented no clock !!!
       ) {
      putSelbuf((*it)->object());
      //dax7 materials();
      /* FIXME!
       * if the following 3 lines are presents then  water ondulation is ok but hats are not ok
       * if the following 3 lines are commented then water ondulation is not ok but hats are ok
       */
      if ((*it)->object()->isBehavior(SPECIFIC_RENDER)) {
        (*it)->object()->render();
      }
      else {
        (*it)->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::OPAQUE);
      }
      (*it)->setRendered(true);
      trace2(DBG_VGL, " %s/%s", (*it)->object()->typeName(), (*it)->object()->getInstance());
    }
  }
}

// Compares distances  : called by sort()
int Render::compDist(const void *t1, const void *t2)
{
  Solid *s1 = (Solid *) t1;
  Solid *s2 = (Solid *) t2;

  // sort by decreasing order : furthest -> nearest
  if (s1->userdist < s2->userdist)
    return 1;
  else if (s1->userdist > s2->userdist)
    return -1;
  else
    return 0;
}

// Renders translucid solids sorted from the furthest to the nearest
void Render::renderTranslucid(bool zsel)
{
  // build translucidList from solidList
  translucidList.clear();
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    if ( (*it) && (*it)->isOpaque() == false && (*it)->isVisible() && !(*it)->object()->removed ) {
      translucidList.push_back(*it);	// add to translucid list
      putSelbuf((*it)->object());
    }
  }

  // sort translucidList
  //dax7 qsort(translucidList.begin(), translucidList.size(), sizeof(Solid), compDist);
  translucidList.sort(compDist);

  // render translucidList
  for (list<Solid*>::reverse_iterator it = translucidList.rbegin(); it != translucidList.rend() ; ++it) {
    if (! (*it)->object()->isBehavior(SPECIFIC_RENDER)) {
      (*it)->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::TRANSLUCID);
    } //dax7 bubble case
    else {
      (*it)->object()->render();
    }
    trace2(DBG_VGL, " %s:%.2f", (*it)->object()->getInstance(), (*it)->userdist);
  }
}

/*
 * Rendering
 * 
 * Render all objects and their solids.
 * - renders objects in displaylists
 * - makes specific rendering for special objects
 */
void Render::rendering(bool zsel=false)
{
  uint32_t objectsnumber = WObject::getObjectsNumber();

  // Set setRendered=false for all solids and find the localuser solid
  list<Solid*>::iterator su;
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    (*it)->setRendered(false);
    if ((*it)->object()->type == USER_TYPE) {
      if ((*it)->object() == localuser) {
        su = it;	// localuser
      }
    }
  }

  //dax8 trace2(DBG_VGL, "\nspecif: ");
  //dax8 renderSpecific();	// no guys text water, hats ok
  trace2(DBG_VGL, "\nopaq-1: ");
#if 1 //dax7 opaq
  renderOpaque(zsel);	// no clock
#else
  renderOpaque(zsel, su, WObject::PRIOR_MEDIUM);	// ok clock
#endif //dax7
  trace2(DBG_VGL, "\nopaq-2: ");
#if 1 //dax8 if 0 no clock
#if 1 //dax7 opaq 0 or 1 works
  renderOpaque(zsel);
#else
  renderOpaque(zsel, su, WObject::PRIOR_HIGH);
#endif //dax7
#endif //dax8
  //dax8 trace2(DBG_VGL, "\nspecif: ");
  //dax8 renderSpecific();	// guys are black

  // renders translucid solids
  trace2(DBG_VGL, "\ntran: ");
  renderTranslucid(zsel);

  trace2(DBG_VGL, "\n");	// end of trace !

  // Renders flashy edges and ray impacts
  for (list<Solid*>::iterator it = solidList.begin(); it != solidList.end() ; ++it) {
    if ( ((*it)->isflashy || (*it)->isflary) ) {
      (*it)->display3D(Solid::DISPLAY, Solid::FLASHRAY);
    }
  }

  // if buffer selection mode is on, don't do anything else: end
  if (zsel)  return;

  // Renders localuser last
  if (localuser) {
    (*su)->display3D(Solid::DISPLAY, Solid::LOCALUSER);
  }
}


/*
 * General rendering loop.
 */
void Render::render()
{
  Texture::update();		// textures
  cameraPosition();		// camera position
  clearGLBuffer();		// background color
  lighting();			// general lighting
  rendering(0);			// solids rendering
  Grid::grid()->render();	// grid
  Axis::axis()->render();	// axis
  satellite();                  // launch a satellite camera
  map();                        // display map
}

void Render::minirender()
{
  cameraPosition();		// camera position
  clearGLBuffer();		// background color
  rendering(0);			// solids rendering
}

void Render::clearGLBuffer()
{
  if (flash) {
    ::g.gui.getScene()->setBackground(UColor::white);
    flash = false;	// flash done
    return;
  }
  else {
    Bgcolor* bgcolor = World::current()->backgroundColor();
    if (bgcolor) {
      UColor bg(bgcolor->color[0], bgcolor->color[1], bgcolor->color[2]);
      ::g.gui.getScene()->setBackground(bg);
    }
  }
  glClearDepth(1);	// depth of Z-buffer to clear
  //dax5 glClearDepth(0);	// depth of Z-buffer to clear

  // !NOTICE: do not do GL_COLOR_BUFFER_BIT because this would clear the color
  // of the whole window (including the background color of the widgets)
  glClear(
            //GL_COLOR_BUFFER_BIT |
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
  //trace2(DBG_FORCE, "\n*** light:");
  for (list<WObject*>::iterator l = lightList.begin(); l != lightList.end() ; ++l) {
    if (*l && (*l)->isValid()) { //FIXME segfault sometimes
      (*l)->lighting();
      //trace2(DBG_FORCE, " %s", (*l)->getInstance());
    }
  }
  //printf("\n");
}

/* highlight same type object. */
void Render::setAllTypeFlashy(char *object_type, int typeflash)
{
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    if (! strcasecmp((*s)->object()->typeName(), object_type)) {
      switch (typeflash) {
      case 0: if (*s) (*s)->isflashy = false; break;
      case 1: if (*s) (*s)->isflashy = true; break;
      }
    }
  }
}

/*
 * 3D Selection with picking method.
 * returns object number selectionned
 * called by camera and getPointedObject (widgets.cpp)
 */
uint16_t Render::bufferSelection(GLint x, GLint y, GLint z)
{
  // set selection mode
  memset(selbuf, 0, sizeof(selbuf));
  glSelectBuffer(sizeof(selbuf), selbuf);
  glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(0);

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  GLint width  = vp[2];
  GLint height = vp[3];
  GLfloat top = cam_user.near * tan(cam_user.fovy * M_PI_180);
  GLfloat bottom = -top;
  GLfloat ratio = (GLfloat) width / (GLfloat) height;
  GLfloat right = top * ratio;
  GLfloat left = -right;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(left + (x-0.1) * (right-left) / (width-1),
	    left + (x+0.1) * (right-left) / (width-1),
	    top -  (y+0.1) * (top-bottom) / (height-1),
	    top -  (y-0.1) * (top-bottom) / (height-1),
	    cam_user.near, cam_user.far);
  glTranslatef(0, 0, -cam_user.near);

  // eye position
  cameraPosition();

  // redraw the objects into the selection buffer
  rendering(true);	// zsel true

  // we put the normal mode back
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // scan the selection buffer, find the nearest hit
  GLint hits = glRenderMode(GL_RENDER);
  GLuint** hitlist = new GLuint*[hits];
  GLuint *psel = selbuf;
  for (int i=0; i < hits; i++) {
    if (::g.pref.dbgtrace) error("hit=%d/%d numobject=%d", i, hits, psel[3]);
    hitlist[i] = psel;
    psel += 3 + psel[0];	// next hit
  }

  uint16_t selected = 0;

  if (hits > 0) {
    qsort((void *)hitlist, hits, sizeof(GLuint *), compareHit);
    int n = z % hits;
    selected = hitlist[n][3];
  }
  if (hitlist) delete[] hitlist;

  return selected;
}

/* Qsort function for elements of the selection buffer. */
int Render::compareHit(const void *t1, const void *t2)
{
  GLuint s1 = (*((GLuint **)t1))[1];	// please don't touch that
  GLuint s2 = (*((GLuint **)t2))[1];

  if (s1 > s2)  return 1;
  if (s1 < s2)  return -1;
  return 0;
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
  ::g.gui.getScene()->getCoords(vp[0], vp[1], vp[2], vp[3]);	// get viewport

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
  //error("wx=%d wy=%d o1=%.2f %.2f %.2f o2=%.2f %.2f %.2f dir=%.2f %.2f %.2f", wx,wy,ex,ey,ez,tx,ty,tz,dir->v[0],dir->v[1],dir->v[2]);
}

void Render::addToList(Solid* solid)
{
  solidList.push_back(solid);
}

void Render::delFromList(Solid* solid)
{
  solidList.remove(solid);
}

/*
 * Get the object list where each object have a type present in the given list.
 */
WObject** Render::getVisibleObjects(char **listtype, int nbr, int *nbelems)
{
  int hits = 0;
  int nb = 0;

  WObject **drawedlist = Render::getDrawedObjects(&hits);
#if 0 //DAXVECTOR
  vector<WObject**> objlist(hits);
#else
  WObject **objlist = (WObject**) malloc(hits*sizeof(WObject*));
#endif

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
#if 0 //DAXVECTOR
  objlist.resize(nb);
#else
  objlist = (WObject**) realloc(objlist, nb * sizeof(WObject*));
#endif

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

  GLint width = vp[2];
  GLint height = vp[3];
  GLfloat top = cam_user.near * tan(cam_user.fovy * M_PI_180);
  GLfloat bottom = -top;
  GLfloat ratio = (GLfloat) width / (GLfloat) height;
  GLfloat right = top * ratio;
  GLfloat left = -right;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glFrustum(left + ((width/2.0)-(width/2.0)) * (right-left) / (width-1),
	    left + ((width/2.0)+(width/2.0)) * (right-left) / (width-1),
	    top -  ((height/2.0)+(height/2.0)) * (top-bottom) / (height-1),
	    top -  ((height/2.0)-(height/2.0)) * (top-bottom) / (height-1),
	    cam_user.near, cam_user.far);
  glTranslatef(0, 0, -cam_user.near);

  // eye position
  cameraPosition();

  // redraws the objects into the selection buffer
  rendering(true);	// zsel true

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
    if (hitlist[i])
      selectlist[i] = WObject::byNum(hitlist[i][3]);
  }
  if (hitlist) delete[] hitlist;
  *nbhit = hits - usercount;

  return selectlist;
}

void Render::analyseScene(char* nameobj)
{
  Vicinity* vicin = new Vicinity(nameobj);
  vicin->analyseScene();
}

void Render::setFlash()
{
  flash = true;
}

void Render::stat()
{
  GLint dlist = glGenLists(1);
  trace(DBG_FORCE, "### Graphic ###\ndisplay-lists : %d", --dlist);
}

/* Debug */
void Render::showSolidList()
{
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    trace(DBG_FORCE, "solidList: %s->%s", (*s)->object()->typeName(),(*s)->object()->getInstance());
    if (! strcasecmp((*s)->object()->typeName(), "User")) {
      error("User: %.2f,%.2f,%.2f %.2f,%.2f %d",
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
  Texture::closeCache();
}
