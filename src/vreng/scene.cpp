//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
/*
 *  scene.cpp : scene for the VREng GUI
 *  VREng Project
 *  Author: Eric Lecolinet - ENST Paris
 *
 *  WWW: http://www.enst.fr/~elc/ubit  http://vreng.enst.fr/
 */
#include <ubit/ugl.hpp>
#include "vreng.hpp"
#include "scene.hpp"
#include "gui.hpp"	// selected_objets
#include "widgets.hpp"  // Widgets
#include "world.hpp"	// compute
#include "universe.hpp"	// current
#include "render.hpp"	// render.init
#include "user.hpp"	// position
#include "pref.hpp"	// width3D
#include "event.hpp"	// netTimeout
#include "capture.hpp"  // isCapturingVideo
#include "timer.hpp"	// ::g.timer


/* constructor */
Scene::Scene(Widgets* _gw) :
  gw(*_gw), 
  is_visible(true),    // should be set to false when the window is iconified !!!!
  is_initialized(false),
  is_initCB_launched(false),
  cycles(0),
  net_delay(500)
{
  background = UBackground::blue;
  addAttr(background);
  addAttr(usize(g.pref.width3D, g.pref.height3D));

  hudbox.addAttr(upos(1, 1)  // position relatively to the canvas : up left corner
                 + UOrient::vertical + UHalign::left
                 + UFont::small + UColor::yellow   // size & color of the text
                 );
  hudbox.add(hud_line1 + hud_line2 + hud_line3 + hud_line4 + hud_line5);
  add(hudbox);	// add the hudbox to the scene
  is_hudvisible = true;

  message.addAttr(UFont::bold + UFont::xx_large + UColor::orange + uhcenter() + uvcenter());
  message.add("Please wait, VReng is coming up...");
  add(uhflex() + uvflex() + message);
  
  // Paint and Resize functions:
  // - are callback functions (paintCB and resizeCB) if the Scene is a UBox 
  // - are redefinitions of paintGL and resizeGL if the Scene is a UGlcanvas
  addAttr(UOn::paint / ucall(this, &Scene::paintCB)
          + UOn::resize / ucall(this, &Scene::resizeCB));
}

///< checks whether the scene is initialized.
bool Scene::isInitialized()
{
  return is_initialized;
}

void Scene::setNetDelay(int msec)
{
  net_delay = msec;
}

int Scene::getNetDelay() const
{
  return net_delay;
}

void Scene::setBackground(UColor& c)
{
  background.setColor(c);
}

void Scene::getCoords(GLint coords[4])
{
  UView* v = getView();
  if (!v) {
    coords[0] = coords[1] = coords[2] = coords[3] = 0;
  }
  else {
    UPoint pos = v->getGLPos();
    coords[0] = (GLint) pos.x;
    coords[1] = (GLint) pos.y;
    coords[2] = (GLsizei) v->getWidth();
    coords[3] = (GLsizei) v->getHeight();
  }
}

void Scene::getCoords(GLint& x, GLint& y, GLsizei& w, GLsizei& h)
{
  UView* v = getView();
  if (!v) {
    x = y = w = h = 0;
  }
  else {
    UPoint pos = v->getGLPos();
    x = (GLint) pos.x;
    y = (GLint) pos.y;
    w = (GLsizei) v->getWidth();
    h = (GLsizei) v->getHeight();
  }
}

/* set viewport */
void Scene::setViewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
  UView* v = getView();
  if (v) {
    UPoint pos = v->getGLPos();
    glViewport((GLint) (pos.x+x), (GLint) (pos.y+y), w, h);
  }
}

/* in UBox mode:
 * - GLSection sets the GL viewport, the clipping planes and calls push/popAttrib()
 * - paintGL performs GLSection, calls Render::camera() then paints the scene
 * - resizeGL just changes ::g.pref.width3D,/height3D
 * The main difference is that GLSection must be done each time we paint because
 * only one window (and one GL context) is used
 */
GLSection::GLSection(Scene* s) 
: UGraph::Glpaint(s->getView(), true)
{}

/* Paints scene CB */
void Scene::paintCB(UPaintEvent& e)
{
  GLSection gls(this);
  if (is_initialized) {
    ::g.render.camera();
  }
  loopScene();
}

/* Resizes scene CB */
void Scene::resizeCB(UResizeEvent& e)
{
  resize(e, int(e.getView()->getWidth()), int(e.getView()->getHeight()));
}

// netTimeout can't be called directly because a callback function must have 1 or 2
// arguments (the arg of netTimeoutCB is not used by syntactically required)
void Scene::netTimeoutCB()
{
  ::netTimeout();  // checks if various updates are needed
}

/* Inits scene */
void Scene::init()
{
  GLSection gls(this);

  //NB: render.init() must be called before world.init() which is called in initCB())
  ::g.render.init(::g.pref.quality3D); 

  // complete the initialization of VREng
  ::g.initCB();

  // timer qui lance le reaffichage de la scene (repaint) tous les delta t
  render_timer.onAction(ucall(this, &UBox::repaint));

  // le 1er arg. est le delai entre deux frames successives (si besoin est, changer
  // cette valeur au lieu de mettre un select() dans paintGL, ce qui est incorrect) 
  // le 2e arg., -1, signifie qu'on repete indefiniement
  render_timer.start((time_t) (::g.pref.frame_delay/1000.), (int) (-1));

  // timer qui execute netTimeout() tous les delta t
  net_timer.onAction(ucall(this, &Scene::netTimeoutCB));
  net_timer.start(net_delay, -1);
  message.show(false);
  is_initialized = true;	// the scene is initialized and ready for rendering
}

void Scene::resize(UResizeEvent& e, int width, int height)
{
  ::g.pref.width3D  = width;
  ::g.pref.height3D = height;
  UAppli::setFocus(e.getView()); // gives the input focus to this view
}

/*
 * Main loop of the engine
 */
void Scene::loopScene()
{
  if (! is_initCB_launched) {
    UAppli::addTimeout(500, 1, ucall(this, &Scene::init));
    is_initCB_launched = true;
  }
  if (! is_visible || ! is_initialized) return;

  // at least one postponed Key Release event
  if (gw.pendingPostponedKRs()) {
    gw.flushPostponedKRs();
  }

  // Computes current world
  ProfileTime& tsimul = ::g.timer.simul;
  tsimul.start();
  if (World::current()) {
    World::current()->compute(tsimul.start_time.tv_sec, tsimul.start_time.tv_usec);
  }
  tsimul.stop();
  
  // General rendering
  ProfileTime& trender = ::g.timer.render;
  trender.start();
  ::g.render.render();
  trender.stop();

  // Displays misc infos in the hud
  if (is_hudvisible) {
    updateHud();
    hudbox.show(true);
  }
  else {
    hudbox.show(false);
  }

  // check if video capture is running
  if (gw.capture.isCapturingVideo()) {
    gw.capture.writeVideoFrame();
  }

  cycles++;		// increments cycles
}

void Scene::updateHud()
{
  char line[1000];
  
  sprintf(line, "Rate:   %.1f fps", ::g.timer.rate());
  hud_line1 = line;
  
  if (! localuser) return;
  sprintf(line, "User:   %.1f %.1f %.1f %.0f",
          localuser->pos.x, localuser->pos.y, localuser->pos.z, RAD2DEG(localuser->pos.az));
  hud_line2 = line;
  
  WObject* obj = ::g.gui.selected_object;
  if (obj) {
    sprintf(line, "Object: %.1f %.1f %.1f %.0f",
            obj->pos.x, obj->pos.y, obj->pos.z, RAD2DEG(obj->pos.az));
    hud_line3 = line;
    
    sprintf(line, "BBox:   %.1f %.1f %.1f",
            obj->pos.bbs.v[0], obj->pos.bbs.v[1], obj->pos.bbs.v[2]);
    hud_line4 = line;
    
    float dist = sqrt((localuser->pos.x-obj->pos.x)*(localuser->pos.x-obj->pos.x) +
                      (localuser->pos.y-obj->pos.y)*(localuser->pos.y-obj->pos.y) +
                      (localuser->pos.z-obj->pos.z)*(localuser->pos.z-obj->pos.z));
    sprintf(line, "Dist:   %.1f", dist);
    hud_line5 = line;
  }
}

void Scene::toggleHud()
{
  is_hudvisible ^= 1;
}
