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
#include "pref.hpp"	// width3D
#include "grid.hpp"	// Grid
#include "axis.hpp"	// Axis
#include "light.hpp"	// lights

#include <vector>
#include <list>

using namespace std;


// local
const int Render::SEL_BUFSIZ = (4*1024);	// 1024 names
static GLuint selbuf[4*1024];			// 1024 objects

extern struct Render::sCamera cam_user;


/* Initialization. */
Render::Render()
{
  first_bbox = false;	// mono solid
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
void Render::config(bool _quality)
{
  static bool configured = false;
  if (! configured) {
    glFrontFace(GL_CCW);		// trigo
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
      trace(DBG_FORCE, "version=%s vendor=%s renderer=%s, depth=%d texturess=%d stencil=%d clips=%d", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER), haveDepth(), haveTextures(), haveStencil(), haveClips());
    configured = true;
  }
  camera();
}

void Render::stat()
{
  GLint dlist = glGenLists(1);
  trace(DBG_FORCE, "### Graphic ###\ndisplay-lists : %d", --dlist);
}

void Render::setFlash()
{
  flash = true;
}

void Render::materials()
{
  const GLfloat ambient[] = {0.2, 0.2, 0.2, 1};
  const GLfloat color[] =   {0.9, 0.9, 0.9, 1};

  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT, GL_SPECULAR, color);
  //glMaterialfv(GL_FRONT, GL_EMISSION, color);
}

/** puts object into buffer selection */
void Render::putSelbuf(WObject *po)
{
  //error("zname: %s %d", po->names.instance, po->num);
  if (po && po->isValid() && po->isSelectable()) {
    glPopName();
    glPushName((GLuint) (long)po->num & 0xffffffff); // hack for 64 bits
  }
}

/**
 * Specific rendering to do by objects themselves
 * by scanning objects lists
 * Note: not seemed to be the better solution (world is dark)
 */
// Renders special objects
void Render::specificObjects(uint32_t num, uint8_t pri)
{
  for (list<WObject*>::iterator o = renderList.begin(); o != renderList.end(); ++o) {
    if (! (*o)->isValid() || (*o)->removed) continue;
    if (
        (*o)->num == num
        //dax
        && (*o)->prior == pri
       ) {
      //dax1
      if (pri != WObject::RENDER_HIGH)
        materials();
      putSelbuf(*o);
      (*o)->render();		// rendering done by object itself
      trace2(DBG_VGL, " %s", (*o)->typeName());
    }
  }
}

// Renders still objects
void Render::specificStill(uint32_t num, uint8_t pri)
{
  for (list<WObject*>::iterator o = stillList.begin(); o != stillList.end(); ++o) {
    if (! (*o)->isValid() || (*o)->removed) continue;
    if (   (*o)
        && (*o)->num == num
        && (*o)->isBehavior(SPECIFIC_RENDER)
        && (*o)->prior == pri
       ) {
      //dax1
      if (pri != WObject::RENDER_HIGH)
        materials();
      putSelbuf(*o);
      (*o)->render();		// rendering done by object itself
      trace2(DBG_VGL, " %s", (*o)->typeName());
    }
  }
}

// Renders mobile objects
void Render::specificMobile(uint32_t num, uint8_t pri)
{
  for (list<WObject*>::iterator o = mobileList.begin(); o != mobileList.end(); ++o) {
    if (! (*o)->isValid() || (*o)->removed) continue;
    if (   (*o)
        && (*o)->num == num
        && (*o)->isBehavior(SPECIFIC_RENDER)
        && (*o)->prior == pri
       ) {
      //dax1
      if (pri != WObject::RENDER_HIGH)
        materials();
      putSelbuf(*o);
      (*o)->render();		// rendering done by object itself
      trace2(DBG_VGL, " %s", (*o)->typeName());
    }
  }
}

// Renders invisible objects
void Render::specificInvisible(uint32_t num, uint8_t pri)
{
  for (list<WObject*>::iterator o = invisibleList.begin(); o != invisibleList.end() ; ++o) {
    if (! (*o)->isValid() || (*o)->removed) continue;
    if (   (*o)
        && (*o)->num == num
        && (*o)->isBehavior(SPECIFIC_RENDER)
        && (*o)->prior == pri
       ) {
      putSelbuf(*o);
      (*o)->render();		// rendering done by object itself
      trace2(DBG_VGL, " %s", (*o)->typeName());
    }
  }
}

// Renders fluid objects
void Render::specificFluid(uint32_t num, uint8_t pri)
{
  for (list<WObject*>::iterator o = fluidList.begin(); o != fluidList.end() ; ++o) {
    if (! (*o)->isValid() || (*o)->removed) continue;
    if (   (*o)
        && (*o)->num == num
        && (*o)->isBehavior(SPECIFIC_RENDER)
        && (*o)->prior == pri
       ) {
      putSelbuf(*o);
      (*o)->render();		// rendering done by object itself
      trace2(DBG_VGL, " %s", (*o)->typeName());
    }
  }
}

// Renders opaque objects
void Render::objectsOpaque(bool zsel, uint8_t pri)
{
  for (list<WObject*>::iterator o = objectList.begin(); o != objectList.end() ; o++) {
    if (!zsel) continue;  // zbuf selection
    if ((*o)->type == USER_TYPE) continue;	// localuser
    if (   (*o)
        && (*o)->isOpaque()
        && (*o)->isVisible()
        && (*o)->prior == pri
       ) {
      putSelbuf(*o);
      if ((*o)->isBehavior(SPECIFIC_RENDER))
        (*o)->render();
      else
        (*o)->solid->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::OPAQUE);
      trace2(DBG_VGL, " %s", (*o)->getInstance());
      if (! relsolidList.empty()) {
        for (list<Solid*>::iterator s = relsolidList.begin(); s != relsolidList.end() ; s++) {
          //trace2(DBG_FORCE, "rel %s %d ", (*o)->getInstance(), relsolidList.size());
          if ((*o)->isBehavior(SPECIFIC_RENDER))
            (*s)->object()->render();
          else
            (*s)->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::OPAQUE);
        }
      }
    }
  }
}

// Renders opaque solids
void Render::solidsOpaque(bool zsel, list<Solid*>::iterator su, uint8_t pri)
{
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    if (s == su) continue;	// skip localuser
    //TODO if ((*s)->object()->isSeen() == false) continue;  // not seen
    if (   (*s)
        && (*s)->isOpaque()
        && (*s)->isVisible()
        && (*s)->object()->prior == pri
       ) {
      /* FIXME!
       * if the following 3 lines are presents then  water ondulation is ok but hats are not ok
       * if the following 3 lines are commented then water ondulation is not ok but hats are ok
       */
      if ((*s)->object()->isBehavior(SPECIFIC_RENDER))
        (*s)->object()->render();
      else
        (*s)->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::OPAQUE);
        //dax (*s)->display3D(Solid::DISPLAY, Solid::OPAQUE);
      trace2(DBG_VGL, " %s %d", (*s)->object()->getInstance(), solidList.size());
    }
  }
}

// Renders translucid solids
void Render::solidsTranslucid(bool zsel, list<Solid*>::iterator su, uint8_t pri)
{
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    if (s == su) continue;	// FIXME! sun flary doesn't appear
    //dax0
    if (s == su && !zsel) continue;
    if (   (*s)
        && (*s)->isOpaque() == false
        && (*s)->isVisible()
        //dax
        && (*s)->object()->prior == pri
       ) {
      if ((*s)->object()->isBehavior(SPECIFIC_RENDER))
        (*s)->object()->render();
      else
        (*s)->display3D(zsel ? Solid::SELECT : Solid::DISPLAY, Solid::TRANSLUCID);
      trace2(DBG_VGL, " %s", (*s)->object()->getInstance());
    }
  }
}

/*
 * Rendering specific
 */
void Render::specificRender(uint32_t num, uint8_t pri)
{
  switch (pri) {
  case WObject::RENDER_LOW:	// low render
    specificStill(num, pri);	// still objects : walls
    specificMobile(num, pri);	// mobile objects : particle
    specificInvisible(num, pri);// invisible objects : sun
    //dax specificFluid(num, pri);	// fluid objects : water
    break;
  case WObject::RENDER_NORMAL:	// normal render
    specificStill(num, pri);	// still objects
    specificMobile(num, pri);	// mobile objects
    specificInvisible(num, pri);// invisible objects : transform
    specificFluid(num, pri);	// fluid objects : water
    break;
  case WObject::RENDER_HIGH:	// high render : models
    specificStill(num, pri);	// still objects
    specificMobile(num, pri);	// mobile objects
    specificInvisible(num, pri);
    specificFluid(num, pri);	// fluid objects
    break;
  }
}

/*
 * Render all objects and their solids.
 * - renders objects in displaylists
 * - makes specific rendering for special objects
 */
void Render::rendering(bool zsel=false)
{
  uint32_t objectsnumber = WObject::getObjectsNumber();

  // Find the localuser object
  list<WObject*>::iterator wu;
  for (list<WObject*>::iterator o = objectList.begin(); o != objectList.end() ; o++) {
    if ((*o)->type == USER_TYPE) {
      wu = o;	// localuser
    }
  }
  // Find the localuser solid
  list<Solid*>::iterator su;
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    if ((*s)->object()->type == USER_TYPE) {
      if (! strcmp((*s)->object()->getInstance(), localuser->getInstance())) {
        su = s;	// localuser
      }
    }
  }

#if 1 //dax2
  //// prior LOW == 0
  //
  trace2(DBG_VGL, "\n--- LOW");
  trace2(DBG_VGL, "\nspeci: ");
  for (uint32_t i=1; i < objectsnumber; i++) { // for all objects LOW
    specificRender(i, WObject::RENDER_LOW);	// particules
  }
  trace2(DBG_VGL, "\nopaque: ");
  solidsOpaque(zsel, su, WObject::RENDER_LOW);
  trace2(DBG_VGL, "\ntranslucid: ");
  solidsTranslucid(zsel, su, WObject::RENDER_LOW);
#endif

  //// prior NORMAL == 1
  //
  trace2(DBG_VGL, "\n--- NORMAL");
  trace2(DBG_VGL, "\nspeci: ");
  for (uint32_t i=1; i < objectsnumber; i++) { // for all objects NORMAL
    specificRender(i, WObject::RENDER_NORMAL);
    //dax specificObjects(i, WObject::RENDER_NORMAL);
  }
  trace2(DBG_VGL, "\nopaque: ");
  solidsOpaque(zsel, su, WObject::RENDER_NORMAL);
  //dax objectsOpaque(zsel, WObject::RENDER_NORMAL);
  trace2(DBG_VGL, "\ntranslucid: ");
  solidsTranslucid(zsel, su, WObject::RENDER_NORMAL);

#if 1 //dax2
  //// prior HIGH == 2
  //
  trace2(DBG_VGL, "\n--- HIGH");
  trace2(DBG_VGL, "\nspeci: ");
  for (uint32_t i=1; i < objectsnumber; i++) { // for all objects HIGH
    specificRender(i, WObject::RENDER_HIGH);
  }
  trace2(DBG_VGL, "\nspeci: ");
  solidsOpaque(zsel, su, WObject::RENDER_HIGH);
  trace2(DBG_VGL, "\ntranslucid: ");
  solidsTranslucid(zsel, su, WObject::RENDER_HIGH);
  trace2(DBG_VGL, "\n");
#endif

  // Renders flashy edges and ray impact
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    if ((*s) && ((*s)->isflashy || (*s)->isflary)) {
      (*s)->display3D(Solid::DISPLAY, Solid::FLASHRAY);
    }
  }

  // if buffer selection mode is on, don't do anything else: end
  if (zsel)  return;

  // Renders localuser last
  if (localuser)
    (*su)->display3D(Solid::DISPLAY, Solid::LOCALUSER);
}


/*
 * General rendering.
 */
void Render::render()
{
  Texture::update();		// textures
  cameraPosition();		// camera position
  clearGLBuffer();		// background color
  lighting();			// general lighting
  rendering(false);		// general rendering
  Grid::grid()->render();	// grid
  Axis::axis()->render();	// axis
  satellite();                  // launch a satellite camera
  map();                        // display map
}

void Render::minirender()
{
  cameraPosition();		// camera position
  clearGLBuffer();		// background color
  rendering(false);		// general rendering
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

  // !NOTICE: do not do GL_COLOR_BUFFER_BIT because this would clear the color
  // of the whole window (including the background color of the widgets)
  glClear(  //GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT
          | GL_STENCIL_BUFFER_BIT
          | GL_ACCUM_BUFFER_BIT
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

  /* renders other lights for example sun, moon, lamp */
#if 0 //DEBUG
  trace2(DBG_VGL, "\n*** light:");
  for (list<WObject*>::iterator l = lightList.begin(); l != lightList.end() ; ++l) {
    if (*l && (*l)->isValid()) { //FIXME segfault sometimes
      (*l)->lighting();
      trace2(DBG_VGL, " %s", (*l)->typeName());
    }
  }
  printf("\n");
#endif
#if 0 //DEBUG
  Light::lights();
#endif
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
 * returns number of hits (objects selectionned)
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
  if (first_bbox) {
    for (int i=0; i<3; i++) {
      bbox_min.v[i] = v[i];
      bbox_max.v[i] = v[i];
    }
    first_bbox = false;
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

void Render::addSolidInList(Solid* solid)
{
  solidList.push_back(solid);
}

void Render::removeSolidFromList(Solid* solid)
{
  solidList.remove(solid);
}

/*
 * Get the object list where each object have a type present in the given list.
 */
WObject** Render::getVisibleTypedObjects(char **listtype, int nbr, int *nbelems)
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
  if (hits < 0)  return NULL;

  GLuint** hitlist = new GLuint*[hits];
  GLuint *psel = selbuf;
  for (int i=0; i < hits; i++) {
    hitlist[i] = psel;
    psel += 3 + psel[0];	// next hit
  }
  qsort((void *) hitlist, hits, sizeof(GLuint *), compareHit);

  int count = 0;
  if (! strcasecmp((WObject::byNum((uint16_t) hitlist[0][3]))->typeName(), "User")) {
    hitlist[0] = NULL;
    count++;
  }

  WObject** selectlist = new WObject*[hits-count];

  for (int i=0; i < hits; i++) {
    if (hitlist[i]) selectlist[i] = WObject::byNum(hitlist[i][3]);
  }
  if (hitlist) delete[] hitlist;
  *nbhit = hits - count;
  return selectlist;
}

void Render::analyseUserScene(char* nameobj)
{
  Vicinity* vicin = new Vicinity(nameobj);
  vicin->analyseUserScene();
}

/* Debug */
void Render::showSolidList()
{
  for (list<Solid*>::iterator s = solidList.begin(); s != solidList.end() ; s++) {
    trace(DBG_FORCE, "solidList: %s->%s", (*s)->object()->typeName(),(*s)->object()->getInstance());
    if (! strcasecmp((*s)->object()->typeName(), "User")) {
      error("User: %.2f,%.2f,%.2f %.2f,%.2f,%.2f %d",
	    (*s)->object()->pos.x,
	    (*s)->object()->pos.y,
	    (*s)->object()->pos.z,
	    (*s)->object()->pos.ax,
	    (*s)->object()->pos.ay,
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
