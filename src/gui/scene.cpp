//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2022 Philippe Dax
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
//
//  scene.cpp : scene for the VREng GUI
//
//  VREng Project
//  Author: Eric Lecolinet - ENST Paris
//---------------------------------------------------------------------------
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


/** Constructor */
Scene::Scene(Widgets* _gw) :
 gw(*_gw), 
 is_visible(true),		// should be set to false when the window is iconified !
 is_initialized(false),
 is_launched(false),
 cycles(0),
 net_delay(500)
{
  is_hudvisible = true;		// hud visible by default
  background = UBackground::blue;

  addAttr(background);
  addAttr(usize(g.pref.width3D, g.pref.height3D));

  hudbox.addAttr(upos(1, 1)	// position relatively to the canvas : up left corner
                 + UOrient::vertical
                 + UHalign::left
                 + UFont::small
                 + UColor::yellow   // size & color of the text
                );
  hudbox.add(hud_row1 + hud_row2 + hud_row3 + hud_row4 + hud_row5 + hud_row6);
  add(hudbox);			// add the hudbox to the scene

  message.addAttr(UFont::bold + UFont::xx_large + UColor::orange + uhcenter() + uvcenter());
  message.add("Please wait, VReng is coming up...");
  add(uhflex() + uvflex() + message);
  
  // Paint and Resize functions:
  // - are callback functions (paintCB and resizeCB) if the Scene is a UBox 
  // - are redefinitions of paintGL and resizeGL if the Scene is a UGlcanvas
  addAttr(  UOn::paint  / ucall(this, &Scene::paintCB)
          + UOn::resize / ucall(this, &Scene::resizeCB));
}

/** Sets the background color */
void Scene::setBackground(UColor& c)
{
  background.setColor(c);
}

/** Gets Scene */
void Scene::getScene(GLint& x, GLint& y, GLsizei& w, GLsizei& h)
{
  UView* v = getView();
  if (! v) {
    x = y = w = h = 0;
  }
  else {
    UPoint pos = v->getGLPos();
    x = int(pos.x);
    y = int(pos.y);
    w = int(v->getWidth());
    h = int(v->getHeight());
  }
}

/** Sets Scene viewport */
void Scene::setScene(GLint x, GLint y, GLsizei w, GLsizei h)
{
  UView* v = getView();
  if (v) {
    UPoint pos = v->getGLPos();
    glViewport(int(pos.x+x), int(pos.y+y), w, h);
  }
}

/* in UBox mode:
 * - GLSection sets the GL viewport, the clipping planes and calls push/popAttrib()
 * - paintGL performs GLSection, calls Render::cameraUser() then paints the scene
 * - resizeGL just changes ::g.pref.width3D,/height3D
 * The main difference is that GLSection must be done each time we paint because
 * only one window (and one GL context) is used
 */
GLSection::GLSection(Scene* s) :
 UGraph::Glpaint(s->getView(), true)
{}

/** Paints scene CB */
void Scene::paintCB(UPaintEvent& e)
{
  GLSection gls(this);

  if (is_initialized) {
    ::g.render.cameraUser();
  }
  if (! is_launched) {
    UAppli::addTimeout(500, 1, ucall(this, &Scene::init));
    is_launched = true;
  }
  if (! is_visible || ! is_initialized) return;

  // at least one postponed Key Release event
  if (gw.pendingPostponedKRs()) {
    gw.flushPostponedKRs();
  }

  //
  // Main loop of the engine
  //

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
    refreshHud();
    hudbox.show(true);
  }
  else {
    hudbox.show(false);
  }

  // check if video capture is running
  if (gw.capture.isCapturingVideo()) {
    gw.capture.writeVideoFrame();
  }

  cycles++;		// increments cycles (fps)
}

/** Resizes scene CB */
void Scene::resizeCB(UResizeEvent& e)
{
  resize(e, int(e.getView()->getWidth()), int(e.getView()->getHeight()));
}

/**
 * netTimeout can't be called directly because a callback function must have 1 or 2
 * arguments (the arg of netTimeoutCB is not used by syntactically required)
 */
void Scene::netTimeoutCB()
{
  ::netTimeout();  // checks if various updates are needed
}

/** Inits scene */
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

/** Resizes scene */
void Scene::resize(UResizeEvent& e, int width, int height)
{
  ::g.pref.width3D  = width;
  ::g.pref.height3D = height;
  UAppli::setFocus(e.getView()); // gives the input focus to this view
}

/** Refresh the hud corner */
void Scene::refreshHud()
{
  char row[32];
  
  // rate
  sprintf(row, "rate:   %.1f fps", ::g.timer.rate());
  hud_row1 = row;
  
  // world
  sprintf(row, "world:  %s", localuser->worldName());
  hud_row2 = row;

  // user
  sprintf(row, "user:   %.1f %.1f %.1f %.0f",
          localuser->pos.x, localuser->pos.y, localuser->pos.z, RAD2DEG(localuser->pos.az));
  hud_row3 = row;
  
  // obj
  WO* obj = ::g.gui.selected_object;
  if (obj) {
    // object
    sprintf(row, "obj:    %.1f %.1f %.1f %.0f",
            obj->pos.x, obj->pos.y, obj->pos.z, RAD2DEG(obj->pos.az));
    hud_row4 = row;
    
    // bbox
    sprintf(row, "bbox:   %.1f %.1f %.1f",
            obj->pos.bbs.v[0], obj->pos.bbs.v[1], obj->pos.bbs.v[2]);
    hud_row5 = row;
    
    // dist
    float dist = sqrt((localuser->pos.x-obj->pos.x)*(localuser->pos.x-obj->pos.x) +
                      (localuser->pos.y-obj->pos.y)*(localuser->pos.y-obj->pos.y) +
                      (localuser->pos.z-obj->pos.z)*(localuser->pos.z-obj->pos.z)
                     );
    sprintf(row, "dist:   %.1f", dist);
    hud_row6 = row;
  }
}

/** Toggles the hud */
void Scene::toggleHud()
{
  is_hudvisible ^= 1;
}
