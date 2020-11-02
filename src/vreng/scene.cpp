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
#include "render.hpp"	// render.config
#include "user.hpp"	// position
#include "pref.hpp"	// width3D
#include "event.hpp"	// NetTimeout
#include "capture.hpp"  // isCapturingVideo
#include "timer.hpp"


/* constructor */
Scene::Scene(Widgets* _gw) :
gw(*_gw), 
is_visible(true),    // should be set to false when the window is iconified !!!!
is_initialized(false),
is_startCB_launched(false),
net_delay(500),
cycles(0)
{
  hudvisible = true;
  background = UBackground::blue;
  addAttr(background);
  addAttr(usize(g.pref.width3D, g.pref.height3D));

  hudbox.addAttr(upos(1, 1)  // position relatively to the canvas : up left corner
                 + UOrient::vertical + UHalign::left
                 + UFont::small + UColor::yellow   // size & color of the text
                 );
  hudbox.add(hud_line1 + hud_line2 + hud_line3 + hud_line4 + hud_line5);
  add(hudbox);	// add the hudbox to the scene

  message.addAttr(UFont::bold + UFont::xx_large + UColor::orange + uhcenter() + uvcenter());
  message.add("Please wait, VReng is coming up...");
  add(uhflex() + uvflex() + message);
  
  // Paint and Resize functions:
  // - are callback functions (paintCB and resizeCB) if the Scene is a UBox 
  // - are redefinitions of paintGL and resizeGL if the Scene is a UGlcanvas
  addAttr(UOn::paint / ucall(this, &Scene::paintCB)
          + UOn::resize / ucall(this, &Scene::resizeCB));
}

void Scene::setBackground(UColor& c)
{
  background.setColor(c);
}

void Scene::getCoords(GLint coords[4])
{
  UView* v = getView();
  if (!v) coords[0] = coords[1] = coords[2] = coords[3] = 0;
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
  if (!v) x = y = w = h = 0;
  else {
    UPoint pos = v->getGLPos();
    x = (GLint) pos.x;
    y = (GLint) pos.y;
    w = (GLsizei) v->getWidth();
    h = (GLsizei) v->getHeight();
  }
}

void Scene::setViewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
  UView* v = getView();
  if (v) {
    UPoint pos = v->getGLPos();
    glViewport((GLint) (pos.x+x), (GLint) (pos.y+y), w, h);
  }
}

// in UBox mode:
// - GLSection sets the GL viewport, the clipping planes et calls push/popAttrib()
// - paintGL performs GLSection, calls Render::camera() then paints the scene
// - resizeGL just changes ::g.pref.width3D,/height3D
// The main difference is that GLSection must be done each time we paint because
// only one window (and one GL context) is used

GLSection::GLSection(Scene* s) 
: UGraph::Glpaint(s->getView(), true) {}

void Scene::paintCB(UPaintEvent& e)
{
  // client GL graphics can be performed until the current function returns
  GLSection gls(this);
  if (is_initialized) ::g.render.camera();
  paintScene();
}

void Scene::resizeCB(UResizeEvent& e)
{
  resizeScene(e, int(e.getView()->getWidth()), int(e.getView()->getHeight()));
}

// NetTimeout can't be called directly because a callback function must have 1 or 2
// arguments (the arg of netTimeoutCB is not used by syntactically required)
void Scene::netTimeoutCB()
{
  ::NetTimeout();  // checks if various updates are needed
}

void Scene::init()
{
  GLSection gls(this);
  //NB: render.config() must be called before world.init() which is called in startCB())
  ::g.render.config(::g.pref.quality3D); 

  // complete the initialization of VRENG
  ::g.startCB();

  // timer qui lance le reaffichage de la scene (repaint) tous les delta t
  render_timer.onAction(ucall(this, &UBox::repaint));

  // le 1er arg. est le delai entre deux frames successives (si besoin est, changer
  // cette valeur au lieu de mettre un select() dans paintGL, ce qui est incorrect) 
  // le 2e arg., -1, signifie qu'on repete indefiniement
  render_timer.start((time_t) (::g.pref.frame_delay/1000.), (int) (-1));

  // timer qui execute NetTimeout() tous les delta t
  net_timer.onAction(ucall(this, &Scene::netTimeoutCB));
  net_timer.start(net_delay, -1);
  message.show(false);
  is_initialized = true; // the scene is initialized and ready for rendering
}

void Scene::resizeScene(UResizeEvent& e, int width, int height)
{
  ::g.pref.width3D  = width;
  ::g.pref.height3D = height;
  UAppli::setFocus(e.getView());    // gives the input focus to this view
}

/*
 * Main loop of the engine
 */
void Scene::paintScene()
{
  if (! is_startCB_launched) {
    is_startCB_launched = true;
    UAppli::addTimeout(500, 1, ucall(this, &Scene::init));
  }
  if (! is_visible || ! is_initialized) return;

  // at least one postponed Key Release event
  if (gw.pendingPostponedKRs()) gw.flushPostponedKRs();

  // Computes current world
  ProfileTime& tsimul = ::g.times.simul;
  tsimul.start();
  if (World::current())
    World::current()->compute(tsimul.start_time.tv_sec, tsimul.start_time.tv_usec);
  tsimul.stop();
  trace(DBG_WO, "world computed");
  
  // General rendering
  ProfileTime& trender = ::g.times.render;
  trender.start();
  ::g.render.render();
  trender.stop();

  // Displays misc infos
  if (hudvisible) {
    updateHud();
    hudbox.show(true);
  }
  else hudbox.show(false);

  // check if video capture is running
  if (gw.capture.isCapturingVideo()) gw.capture.writeVideoFrame();

  cycles++;		// increments cycles
}

void Scene::updateHud()
{
  char buf[1000];
  
  sprintf(buf, "Rate:   %.1f fps", ::g.times.getRate());
  hud_line1 = buf;
  
  if (! localuser) return;
  sprintf(buf, "User:   %.2f %.2f %.2f %.0f",
          localuser->pos.x, localuser->pos.y, localuser->pos.z, RAD2DEG(localuser->pos.az));
  hud_line2 = buf;
  
  WObject* obj = ::g.gui.selected_object;
  if (obj) {
    sprintf(buf, "Object: %.2f %.2f %.2f %.0f",
            obj->pos.x, obj->pos.y, obj->pos.z, RAD2DEG(obj->pos.az));
    hud_line3 = buf;
    
    sprintf(buf, "BBox:   %.2f %.2f %.2f",
            obj->pos.bbsize.v[0], obj->pos.bbsize.v[1], obj->pos.bbsize.v[2]);
    hud_line4 = buf;
    
    float dist = sqrt((localuser->pos.x-obj->pos.x)*(localuser->pos.x-obj->pos.x) +
                      (localuser->pos.y-obj->pos.y)*(localuser->pos.y-obj->pos.y) +
                      (localuser->pos.z-obj->pos.z)*(localuser->pos.z-obj->pos.z));
    sprintf(buf, "Dist:   %.2f", dist);
    hud_line5 = buf;
  }
}

void Scene::toggleHud()
{
  hudvisible ^= 1;
}
