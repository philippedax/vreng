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
/*
 *  widgets.cpp : Ubit widgets for the VREng GUI
 *
 *  VREng / Ubit Project
 *  Authors: Eric Lecolinet and Philippe Dax
 *
 *  Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
 *  Please refer to the Ubit GUI Toolkit Home Page for details.
 *
 *  (C) 2002/2009 Eric Lecolinet - ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */

#include "vreng.hpp"
#include "widgets.hpp"
#include "scene.hpp"	// new Scene
#include "gui.hpp"
#include "navig.hpp"
#include "panels.hpp"
#include "message.hpp"
#include "theme.hpp"
#include "capture.hpp"
#include "useraction.hpp"
#include "move.hpp"	// changeKey

#if (UBIT_VERSION_MAJOR < 6 || UBIT_VERSION_MAJOR >= 6 && UBIT_WITH_X11) //UBIT6
#include <X11/keysym.h>	// XK_*
#endif

#include "pref.hpp"	// width3Dm height3D
#include "env.hpp"	// menu
#include "universe.hpp" // Universe
#include "world.hpp"	// World
#include "user.hpp"	// localuser
#include "cart.hpp"	// cart::DROP
#include "mirage.hpp"	// Mirage
#include "wall.hpp"	// Wall
#include "ball.hpp"	// Ball
#include "ground.hpp"	// Ground
#include "model.hpp"	// Model
#include "step.hpp"	// Step
#include "thing.hpp"	// Thing
#include "gate.hpp"	// Gate
#include "http.hpp"	// httpOpen
#include "grid.hpp"	// buildBox
#include "file.hpp"	// openFile
#include "cache.hpp"	// openCache
#include "app.hpp"	// TOOLS
#include "browser.hpp"	// start
#include "audio.hpp"	// start
#include "video.hpp"	// start
#include "wb.hpp"	// start
#include "mp3.hpp"	// start
#include "wav.hpp"	// start
#include "mpeg.hpp"	// start
#include "modeler.hpp"	// start
#include "ps.hpp"	// start
#include "pdf.hpp"	// start
#include "office.hpp"	// start
#include "channel.hpp"	// current
#include "cache.hpp"	// setCacheName
#include "render.hpp"
#include "move.hpp"	// changeKey
#include "file.hpp"	// openFile

// Text files
#include "README.t"
#include "COPYRIGHT.t"
#include "COPYING.t"
#include "ChangeLog.t"
#include "CONFIG_H.t"
#include "TODO.t"
#include "DTD.t"

#include <ubit/ubit.hpp>
using namespace ubit;


Widgets::Widgets(Gui* _gui) :    // !BEWARE: order matters!
gui(*_gui),
putinfo(*new PutInfo),
capture(*new Capture),
scene(*new Scene(this)),
navig(*new Navig(this, scene)),
source_dialog(*new UOptionDialog("World source")),
worlds_dialog(*new UOptionDialog("World list")),
prefs_dialog(createPrefsDialog()),
settings_dialog(createSettingsDialog()),
grid_dialog(createGridDialog()),
tool_dialog(createToolDialog()),
addobj_dialog(createAddobjDialog()),
messages(*new Messages(this)),
panels(*new Panels(this, scene)),
infobar(createInfobar()),
menubar(createMenubar()),
postponedKRmask(0),
postponedKRcount(0)
{
  worlds.addAttr(UOrient::vertical + utop());
  carts.addAttr(UOrient::vertical + utop());
  avatars.addAttr(UOrient::vertical + utop());

  // main box
  addAttr(g.theme.mainStyle);
  add(utop() + menubar + infobar
      + uvflex()
      + uhbox(uvflex() + uhflex() + uhspacing(0)
              + uvbox(uhflex() + uvflex() + scene + ubottom() + panels.main_panel)
              + uright() + panels.right_panel)
      );
  // process ubit messages coming from other applications
  UAppli::onMessage("file",   ucall(this, &Widgets::openMessage));
  UAppli::onMessage("open",   ucall(this, &Widgets::openMessage));
  UAppli::onMessage("put",    ucall(this, &Widgets::putMessage));
  UAppli::onMessage("get",    ucall(this, &Widgets::getMessage));
  UAppli::onMessage("move",   ucall(this, &Widgets::moveMessage));
  UAppli::onMessage("back",   ucall(this, &Widgets::backCB));
  UAppli::onMessage("forward",ucall(this, &Widgets::forwardCB));
  UAppli::onMessage("home",   ucall(this, &Widgets::homeCB));
}


UBox& Widgets::createInfobar()
{
  UBox& navigbox =
  uhbox(upadding(8,0) + uhspacing(8) + uleft() + UFont::bold
        + uitem(USymbol::left + UBackground::green  + utip("Prev world") + ucall(this, &Widgets::backCB))
        + uitem(USymbol::up   + UBackground::green  + utip("Home world") + ucall(this, &Widgets::homeCB))
        + uitem(USymbol::right + UBackground::green + utip("Next world") + ucall(this, &Widgets::forwardCB))
        + usepar()
        );
  // infos initially contains the welcome message, its changed dynamically when objects are selected
  infos.addAttr(UOrient::horizontal + uleft());
  infos.add(uleft() + UPix::ray + UColor::red + UFont::bold + " Welcome to VREng " + UPix::ray);
  return uhbox(uvspacing(2) + navigbox + infos);
}

void Widgets::setInfobar(UBox* content)
{
  infos.removeAll();
  if (content) infos.add(content);
}


UBox& Widgets::createMenubar()
{
  UMenu& file_menu = createFileMenu();
  file_menu.addAttr(g.theme.menuStyle);

  UMenu& view_menu =
  umenu(g.theme.menuStyle
        + ubutton(g.theme.Edit  + "Source"      + ucall(this, &Widgets::openSourceDialog))
        + ubutton(g.theme.List  + "Worlds"      + ucall(this, &Widgets::openWorldsDialog))
        + ubutton(g.theme.Prefs + "Preferences" + prefs_dialog)
        );

  UMenu& tool_menu =
  umenu(g.theme.menuStyle
        + ubutton(g.theme.Prefs  + " Settings " + settings_dialog)
        + ubutton(g.theme.Grid2D + " Grid "     + grid_dialog)
        + ubutton(g.theme.Tools  + " Tools "    + tool_dialog)
        + ubutton(g.theme.AddObj + " Addobj "   + addobj_dialog)
        );

  UMenu& history_menu =
  umenu(g.theme.menuStyle
        + ubutton("Previous World"   + ucall(this, &Widgets::backCB))
        + ubutton("Next World"       + ucall(this, &Widgets::forwardCB))
        + ubutton("Home"             + ucall(this, &Widgets::homeCB))
        + ubutton("Visited Worlds >" + umenu(g.theme.menuStyle + worlds))
        );

  UMenu& help_menu =
  umenu(g.theme.menuStyle
        + ubutton("Home Page" + ucall(this, &Widgets::helpCB))
        + ubutton("README"    + ucall("README", README, &Widgets::showInfoDialog))
        + ubutton("ChangeLog" + ucall("ChangeLog", ChangeLog, &Widgets::showInfoDialog))
        + ubutton("config.h"  + ucall("config.h", CONFIG_H, &Widgets::showInfoDialog))
        + ubutton("DTD"       + ucall("DTD", DTD, &Widgets::showInfoDialog))
        + ubutton("TODO"      + ucall("TODO", TODO, &Widgets::showInfoDialog))
        + ubutton("COPYRIGHT" + ucall("COPYRIGHT", COPYRIGHT, &Widgets::showInfoDialog))
        + ubutton("LICENSE"   + ucall("LICENSE", COPYING, &Widgets::showInfoDialog))
        );

  // ===== Menubar ======
  UMenubar& menu_bar =
  umenubar(ubutton("File"    + file_menu)
         + ubutton("View"    + view_menu)
         + ubutton("Go"      + ucall(this, &Widgets::destinationsDialog))
         + ubutton("Tools"   + tool_menu)
         + ubutton("History" + history_menu)
          );

  menu_bar.add(ubutton("Marks" + createMarkMenu()));
  addDynamicMenus(menu_bar, ::g.env.menu());
  menu_bar.add(ubutton("Help" + help_menu));
  return menu_bar;
}


static void functionMenu(Widgets*)
{
  trace(DBG_FORCE, "functionMenu OK");
}

void Widgets::addDynamicMenus(UMenubar& menu_bar, const char* filename)
{
  FILE* menufp = null;
  UMenu* dynMenu = null;
  char attr[100], val[100];

  if ((menufp = File::openFile(filename, "r"))) {
    fscanf(menufp, "%s %s", attr, val);
    //cerr << "dynmenu: " << attr << " " <<val << endl;
    while (!feof(menufp)) {
      if (strcmp(attr, "Menu") == 0) {
        dynMenu = new UMenu();
        dynMenu->addAttr(g.theme.menuStyle);
        menu_bar.add(ubutton(ustr(val) + dynMenu));
      }
      else if (strcmp(attr, "MenuItem") == 0  && dynMenu) {
        UButton& btn = ubutton(val);
        dynMenu->add(btn);
        if (strcmp(val, "functionMenu") == 0) btn.add(ucall(this, functionMenu));
      }
      fscanf(menufp, "%s %s", attr, val);
    }
    File::closeFile(menufp);
  }
}

// Mark menu
UMenu& Widgets::createMarkMenu()
{
  UBox& mark_box = uvbox();
  mark_box.add(ubutton(UBackground::none + "Add Worldmark"
                       + ucall(this, &Widgets::addMarkCB)));
#if 0 //DAX !!! BUG1 !!!
  // Si on fait ca, le ubutton n'a plus le background du menu : full transparent 
  UScrollpane& mark_pane = uscrollpane(g.theme.menuStyle + usize().setHeight(80) + UBackground::none + mark_box);
  mark_pane.showHScrollButtons(false);
  mark_pane.getHScrollbar()->show(false);
  UMenu& mark_menu
  = umenu(g.theme.menuStyle + UBackground::none + mark_pane);
#else
  UMenu& mark_menu
  = umenu(g.theme.menuStyle + uscrollpane(usize().setHeight(80) + UBackground::none + mark_box));
#endif

  FILE *fp = null;
  char buf[URL_LEN + CHAN_LEN + 2];
  if ((fp = File::openFile(::g.env.worldmarks(), "r"))) {
    while (fgets(buf, sizeof(buf), fp)) {
      char *p = strchr(buf, ' ');
      if (p) *p ='\0';
      // Je ne comprends pas pourquoi le callback ci-dessous n'est jamais appele
      mark_box.add(uitem(buf) + ucall(&gui, (const UStr&)buf, &Gui::gotoWorld));
    }
    File::closeFile(fp);
  }
  return mark_menu;
}

//UMenu* Widgets::getOpenedMenu() {return navig.opened_menu;}
void Widgets::showInfoDialog(const char* title, const char* message)
{
  UDialog::showMessageDialog(title, uscrollpane(usize(450, 500) + utextarea(message)));
}


GuiItem::GuiItem(UArgs a) : UButton(a)
{
  addAttr(UBorder::none + UOn::enter / UBackground::blue + UOn::arm / UBackground::blue);
}

static void setUser(UBox *gu, User *user)
{
  if (! user)  return;
  gu->add(ustr(user->getInstance())
          + umenu(UFont::bold + UColor::navy
                  + uhbox(" Name: "  + UFont::plain + user->getInstance())
                  + uhbox(" World: " + UFont::plain + user->names.world)
                  + uhbox(" Email: " + UFont::plain + user->email)
                  + uhbox(" Cname: " + UFont::plain + user->rtcpname)
                  + uhbox(" Vreng: " + UFont::plain + user->tool)
                  + uhbox(" Web: "   + UFont::plain + user->web)
                  )
          );
}

GuiItem *Widgets::addUser(User *user)
{
  if (! user)  return null;
  GuiItem *gu = new GuiItem();
  setUser(gu, user);
  avatars.add(*gu);
  return gu;
}

void Widgets::updateUser(GuiItem* gu, User *user)
{
  if (! user)  return;
  gu->removeAll(true);
  setUser(gu, user);
}

void Widgets::removeUser(GuiItem* gu)
{
  if (! gu)  return;
  avatars.remove(*gu);
}

static void setWorld(GuiItem* gw, World *world, bool isCurrent)
{
  if (! gw || !world)  return;
  UFont *ft = isCurrent ? &UFont::bold : &UFont::plain;
  gw->add(ft
          + ustr(world->getName())
          + umenu(UFont::bold + UColor::navy
                  + uhbox(" URL: " + UFont::plain + world->getUrl())
                  + uhbox(" Channel: " + UFont::plain + world->getChan())
                  )
          );
}

GuiItem *Widgets::addWorld(World *world, bool isCurrent)
{
  if (! world)  return null;
  GuiItem *gw = new GuiItem();
  setWorld(gw, world, isCurrent);
  worlds.add(*gw);
  return gw;
}

void Widgets::updateWorld(World *world, bool isCurrent)
{
  if (! world || ! world->isGui())  return;
  GuiItem *gw = world->getGui();
  if (! gw)  return;
#if 0 //FIXME! sometimes segfault in Ubit
  gw->removeAll(true);
#endif
  setWorld(gw, world, isCurrent);
  gw->update();
}

void Widgets::removeWorld(World *world)
{
  if (! world)  return;
  GuiItem *gw = world->getGui();
  if (! gw)  return;
  worlds.remove(*gw);
  world->resetGui();
}

// CALLBACKS

void Widgets::homeCB()
{
  char chan_home_str[CHAN_LEN];
  sprintf(chan_home_str, "%s/%u/%d", Universe::current()->group, Universe::current()->port, Channel::currentTtl());
  trace(DBG_IPMC, "WO: goto %s at %s", Universe::current()->url, chan_home_str);

  World::current()->quit();
  delete Channel::current();  // delete Channel
  World::enter(Universe::current()->url, chan_home_str, true);
  World::current()->setChanAndJoin(chan_home_str);  // join new channel

  if (audioactive) Audio::start(chan_home_str);
}

void Widgets::backCB()
{
  World::goBack();
}

void Widgets::forwardCB()
{
  World::goForward();
}

void Widgets::saveCB()
{
  char vrein[PATH_LEN], vreout[PATH_LEN], buf[BUFSIZ];
  World *world = World::current();
  if (! world) return;

  Cache::setCacheName(world->getUrl(), vrein);

  FILE *fi, *fo;
  sprintf(vreout, "%s.vre", world->getName());
  if ((fo = File::openFile(vreout, "w"))) {
    if ((fi = File::openFile(vrein, "r"))) {
      while (fgets(buf, sizeof(buf), fi)) fputs(buf, fo);
      File::closeFile(fo);
      File::closeFile(fi);
      notice("file %s saved", vreout);
    }
  }
}

void Widgets::helpCB()
{
  UStr cmd = "IFS=' '; firefox -remote 'openURL(http://"
              & UStr(Universe::current()->name)
              & "/)' &";
  system(cmd.c_str());
}

void Widgets::setPrefCB(int tool)
{
  if (tool & AUDIO_MASK)      Audio::init(tool);
  if (tool & VIDEO_MASK)      Video::init(tool);
  if (tool & WHITEBOARD_MASK) Wb::init(tool);
  if (tool & HTML_MASK)       Browser::init(tool);
  if (tool & MODELER_MASK)    Modeler::init(tool);
  if (tool & MP3_MASK)        Mp3::init(tool);
  if (tool & MP3_MASK)        Wav::init(tool);
  if (tool & MPEG_MASK)       Mpeg::init(tool);
  if (tool & PS_MASK)         Ps::init(tool);
  if (tool & PDF_MASK)        Pdf::init(tool);
  if (tool & OFFICE_MASK)     Office::init(tool);
}

void Widgets::setAudioCB(bool on)
{
  if (on) {
    audioactive = true;
    Audio::start(World::current()->getChan());
  }
  else {
    Audio::quit();
    audioactive = false;
  }
}

void Widgets::setVideoCB(bool on)
{
  if (on) Video::start(World::current()->getChan());
  else    Video::quit();
}

void Widgets::setWhiteboardCB(bool on)
{
  if (on) Wb::start(World::current()->getChan());
  else    Wb::quit();
}

void Widgets::setModelerCB(bool on)
{
  if (on) Modeler::start();
  else    Modeler::quit();
}

void Widgets::addMarkCB()
{
  FILE *fp;
  char mark[URL_LEN + CHAN_LEN + 2];
  char buf[URL_LEN + CHAN_LEN + 2];

  sprintf(mark, "%s %s\n", World::current()->getUrl(), World::current()->getChan());
  if ((fp = File::openFile(::g.env.worldmarks(), "r")) != NULL) {
    while (fgets(buf, sizeof(buf), fp)) {
      if (!strcmp(buf, mark)) {
        File::closeFile(fp);
        return;
      }
    }
    File::closeFile(fp);
  }
  if ((fp = File::openFile(::g.env.worldmarks(), "a")) != NULL) {
    fputs(mark, fp);
    File::closeFile(fp);
    createMarkMenu();
  }
}

void Widgets::setKey(int key, int ispressed)
{
  struct timeval t;
  gettimeofday(&t, NULL);       // get time
  changeKey(key, ispressed, t.tv_sec, t.tv_usec);
}

void Widgets::callAction(int numaction)
{
  struct timeval t;
  gettimeofday(&t, NULL);

  if (! localuser)  return;
  localuser->specialAction(numaction, NULL, t.tv_sec, t.tv_usec);
}

// with current solid!
static void objectActionCB(int numaction)
{
  WObject* sel_obj = g.gui.getSelectedObject();
  if (sel_obj) {
    struct timeval t;
    gettimeofday(&t, NULL);
    sel_obj->specialAction(numaction, NULL, t.tv_sec, t.tv_usec);
  }
}

// returns info about the pointed object but do NOT select it
WObject* Widgets::getPointedObject(int x, int y, ObjInfo *objinfo, int z)
{
  trace(DBG_GUI, "Pointed: clic=%d,%d %d", x, y, z);
  static char *classname = 0, *instancename = 0, *actionnames = 0;

  uint16_t num = g.render.getBufferSelection(x, y, z);
  WObject* object = WObject::byNum(num);

  if (! object) {
    objinfo[0].name = (char*) "World";	// avoid segfault
    objinfo[1].name = (char*) World::current()->getName();
    objinfo[2].name = NULL;	// NULL terminaison
    return NULL;
  }
  if (! object->isVisible())  return NULL;	// invisible
  /* an object has been selected */
  if (::g.pref.dbgtrace) trace(DBG_FORCE, "Pointed: obj=%p", object);
  /* get the object hname or avatar name */
  object->getObjectHumanName(&classname, &instancename, &actionnames);

  if (classname == NULL)  return NULL;
  objinfo[0].name = classname;
  if (instancename == NULL) instancename = (char *)"";
  objinfo[1].name = instancename;   // TO BE COMPLETED
  if (::g.pref.dbgtrace) error("Pointed: %s:%s %p", classname, instancename, actionnames);

  int i = 0;
  if (actionnames) {
    for (i = 0; i < ACTIONSNUMBER ; i++) {
      if (*actionnames) {
        objinfo[i+2].name = actionnames;
        objinfo[i+2].fun  = objectActionCB;
        objinfo[i+2].farg = i;
        trace(DBG_GUI, "i=%d act=%s", i, actionnames);
      }
      actionnames += ACTIONNAME_LEN;	// AWFULL !!!
    }
  }
  objinfo[i+2].name = NULL; // NULL terminated
  return object;
}

void Widgets::setRayDirection(int x, int y)
{
  if (! localuser)  return;
  localuser->setRayDirection(x, y);
}

//---------------------------------------------------------------------------
/*
 *  keys.cpp : key management and correspondance with VREng
 *  NOTE: this file should be common to all X-Window GUI variants
 */

static long convertKey(long keycode, int keychar, int& vrkey);

/** Use this callback function for handling AUTOREPEAT Key event
 * Autorepeat is logically suppressed by annulating corresponding Press
 * and Release events.
 * Principle: Release events are postponed and temporarily stored in KRmask
 * They are processed when coming back to the RenderingWorkProc of the
 * mainLoop if not annulated in the meantime by subsequent Press events.
 */
void Widgets::processKey(long keysym, int keychar, bool press)
{
  // converts the X keysym into a Vreng change key (vrkey) and returns
  // a keymask which is an hexa value for marking released keys in the KRmask
  int vrkey;
  long keymask = convertKey(keysym, keychar, vrkey);
  if (keymask == 0) return;    // return if null (undefined or not a vrkey)

  if (postponedKRcount < 0) {
    fprintf(stderr, "!negative KRcount => reset\n"); // should never happen!
    postponedKRmask = 0;
    postponedKRcount = 0;
  }

  if (press) {
    if (postponedKRmask & keymask) {
      // le Press annule le Release correspondant dans le KRmask
      postponedKRmask &= ~keymask;	// remove keymask from KRmask
      postponedKRcount--;		// une touche en moins dans KRlist
    }
    else {  // traitement normal d'un Press
      //fprintf(stderr, "KPress change or activate Key( %d ) \n", vrkey);
      if (vrkey >= MAXKEYS || vrkey < 0)
        return;

      struct timeval time;
      gettimeofday(&time, NULL);
      changeKey(vrkey, press, time.tv_sec, time.tv_usec);
    }
  }
  else { // release
    // too many keys stored ==> flush the KRlist
    if (postponedKRcount > KRKey::KRBUF_MAX) {
      fprintf(stderr, "too many keys stored => flush: KRmask=%lx\n", postponedKRmask);
      flushPostponedKRs();
    }
    else { // normal case: postpone until we come back to the mainLoop
      // so that this Key Release event can possibly be annulate
      // if there is a subsequent (and corresponding) Key Press event
      postponedKRmask |= keymask;	// add keymask to KRmask

      // add this event to the KRlist of postponed Key Releases
      postponedKRbuf[postponedKRcount].vrkey = vrkey;
      gettimeofday(&(postponedKRbuf[postponedKRcount].time), NULL);
      postponedKRcount++;
    }
  }
}

void Widgets::flushPostponedKRs()
{
  for (int ix = 0; ix < postponedKRcount; ix++) {
    KRKey &k = postponedKRbuf[ix];
    changeKey(k.vrkey, false, k.time.tv_sec, k.time.tv_usec);
  }
  postponedKRmask = 0;
  postponedKRcount = 0;
}

static long convertKey(long keycode, int keychar, int& vrkey)
{
  long keymask = 0;
  vrkey = 0;
  
  if      (keycode == UKey::Up)       { keymask = 1<<0;  vrkey = KEY_AV; }  // move forward
  else if (keycode == UKey::Down)     { keymask = 1<<1;  vrkey = KEY_AR; }  // move backward
  else if (keycode == UKey::Left)     { keymask = 1<<2;  vrkey = KEY_GA; }  // turn left
  else if (keycode == UKey::Right)    { keymask = 1<<3;  vrkey = KEY_DR; }  // turn right
  else if (keycode == UKey::PageUp)   { keymask = 1<<11; vrkey = KEY_JU; }  // move up
  else if (keycode == UKey::PageDown) { keymask = 1<<12; vrkey = KEY_JD; }  // move down
  else if (keycode == UKey::Insert)   { keymask = 1<<6;  vrkey = KEY_MT; }  // roll up
  else if (keycode == UKey::Delete)   { keymask = 1<<7;  vrkey = KEY_DE; }  // roll down
  else if (keycode == UKey::Home)     { keymask = 1<<8;  vrkey = KEY_HZ; }  // stand up
  else if (keycode == UKey::End)      { keymask = 1<<13; vrkey = KEY_VI; }  // accelerator
  else if (keycode == UKey::BackSpace) {
    Widgets::callAction(UserAction::ASPEEDLESS); return 0; // decrease aspeed
  }
  else if (keycode == UKey::Tab) {
    Widgets::callAction(UserAction::ASPEEDMORE); return 0; // increase aspeed
  }
  else {
    switch (keychar) {
      case '<': keymask = 1<<4;          vrkey = KEY_SG; break;  // left translation
      case '>': keymask = 1<<5;          vrkey = KEY_SD; break;  // right translation
      case 'l': keymask = 1<<9;          vrkey = KEY_TL; break;  // tilt left
      case 'r': keymask = 1<<10;         vrkey = KEY_TR; break;  // tilt right
      case 'u': keymask = 1<<11 | 1<<13; vrkey = KEY_JU; break;  // up translation
      case ' ': keymask = 1<<13;         vrkey = KEY_VI; break;  // accelerator
      case '=': Widgets::callAction(UserAction::FOVYDEF);    return 0;  // original fovy
      case '-': Widgets::callAction(UserAction::FOVYLESS);   return 0;  // decrease fovy
      case '+': Widgets::callAction(UserAction::FOVYMORE);   return 0;  // increase fovy
      case '.': Widgets::callAction(UserAction::LSPEEDDEF);  return 0;  // original lspeed
      case 's': Widgets::callAction(UserAction::LSPEEDLESS); return 0;  // decrease lspeed
      case 'f': Widgets::callAction(UserAction::LSPEEDMORE); return 0;  // increase lspeed
      case ',': Widgets::callAction(UserAction::ASPEEDDEF);  return 0;  // original aspeed
      case 'b': Widgets::callAction(UserAction::BULLET);     return 0;  // launche bullet
      case 'd': Widgets::callAction(UserAction::DART);       return 0;  // launche dart
      case 'v': Widgets::callAction(UserAction::SWITCHVIEW); return 0;  // switch view
      case 'x': Widgets::callAction(UserAction::TPVIEWROTL); return 0;
      case 'c': Widgets::callAction(UserAction::TPVIEWROTR); return 0;
      case 'q': Widgets::callAction(UserAction::TPVIEWROTU); return 0;
      case 'w': Widgets::callAction(UserAction::TPVIEWROTD); return 0;
      case 'p': Widgets::callAction(UserAction::TPVIEWROTN); return 0;
      case 'm': Widgets::callAction(UserAction::TPVIEWROTF); return 0;
      case 'D': Widgets::callAction(UserAction::PITCHMORE);  return 0;  // increase pitch
      case 'U': Widgets::callAction(UserAction::PITCHLESS);  return 0;  // decrease pitch
      case 'R': Widgets::callAction(UserAction::ROLLMORE);   return 0;  // increase roll
      case 'L': Widgets::callAction(UserAction::ROLLLESS);   return 0;  // decrease roll
      default: return 0; break; // == 0 => undefined key
    }
  }
  return keymask;
}

//---------------------------------------------------------------------------
/*
 *  dialog.cpp : Ubit dialogs for the VREng GUI
 */


/** Displays preferences */
UDialog& Widgets::createPrefsDialog()
{
  UBox& settings_box = uvbox(UBackground::white + upadding(2,2));

  UDialog* prefs_dialog =
  new UOptionDialog("Preferences",
                    uvbox(ulabel("Preferences: " + UColor::red + ::g.env.prefs())
                          + uvflex()
                          + uscrollpane(usize(400,300) + settings_box)),
                    UArgs::none, UArgs::none);

  FILE *fpprefs;
  char bufprefs[128];
  if ((fpprefs = File::openFile(::g.env.prefs(), "r"))) {
    while (fgets(bufprefs, sizeof(bufprefs), fpprefs)) {
      if (isalpha(*bufprefs)) settings_box.add(uitem(UColor::red + UFont::bold + bufprefs));
      else                    settings_box.add(uitem(UColor::black + bufprefs));
    }
    File::closeFile(fpprefs);
  }
  return *prefs_dialog;
}

/** Displays vre source */
static void sourceHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *source_box = (UBox *) box;
  char line[BUFSIZ];

  source_box->setAutoUpdate(false);

  FILE *fpcache = Cache::openCache(http->url, http);
  while (fgets(line, sizeof(line), fpcache))
    source_box->add(uitem(UColor::black + line));
  source_box->setAutoUpdate(true);
  source_box->update();
}

/** Displays list of worlds */
static void destinationsHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *universe_box = (UBox *) box;
  char line[URL_LEN + CHAN_LEN +2];

  for (int i=0 ; http->nextLine(line) && i < MAX_WORLDS ; i++) {
    UStr& url = ustr();

    //printf("line= %s\n", line);
    char tmpline[URL_LEN + CHAN_LEN +2];
    char tmpname[URL_LEN + 2];
    strcpy(tmpline, line);
    char *p = strchr(tmpline, ' ');
    if (p) *p = '\0';
    else {
      p = strchr(tmpline, '\t');
      if (p) *p = '\0';
    }
    strcpy(tmpname, tmpline);
    char *name = strrchr(tmpname, '/');
    if (name) *name++ = '\0';
    p = strchr(name, '.');
    if (p) *p = '\0';
    url = tmpline;

    universe_box->add(UBackground::black //+ ualpha(0.5)
                      + uitem(UBackground::none + UColor::green + UFont::bold + UFont::large + name
                      + ucall(&g.gui, (const UStr&)url, &Gui::gotoWorld))
                     );
  }
}

/** Displays list of worlds */
static void universeHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *universe_box = (UBox *) box;
  char line[URL_LEN + CHAN_LEN +2];

  for (int i=0 ; http->nextLine(line) && i < MAX_WORLDS ; i++) {
    UStr& url = ustr();
    UStr& chan = ustr();

    //printf("line: %s\n", line);
    char tmpline[URL_LEN + CHAN_LEN +2];
    strcpy(tmpline, line);
    char *p = strchr(tmpline, ' ');
    if (p) *p = '\0';
    else {
      p = strchr(tmpline, '\t');
      if (p) *p = '\0';
    }
    if (p) chan = ++p;
    url = tmpline;

    universe_box->add(uitem(UColor::navy + UFont::bold + url
                            + " " + UFont::plain + chan
                            + ucall(&g.gui, (const UStr&)url, &Gui::gotoWorld))
                      );
  }
}

/** Dialog box for vre source */
void Widgets::openSourceDialog()
{
  World *world = World::current();
  if (! world) return;

  UBox& box = uvbox(UBackground::white);
  if (Http::httpOpen(world->getUrl(), sourceHttpReader, &box, -1) < 0) { // -1 = no cache
    delete &box;
    return;
  }
  source_dialog.setMessage(uscrollpane(usize(450,350) + UBackground::white + box));
  source_dialog.show();
}

/** Dialog box for worlds list */
void Widgets::destinationsDialog()
{
  char universe_url[URL_LEN];
  char str[URL_LEN];
  if (!strncmp(Universe::current()->name, "http://", 7))
    sprintf(str, "%s", "%s%s/vacs/v%d/worlds");
  else
    sprintf(str, "%s%s", "http://", "%s%s/vacs/v%d/worlds");
  sprintf(universe_url, str, Universe::current()->name, Universe::current()->urlpfx, Universe::current()->version);
  UBox& box = uvbox(g.theme.scrollpaneStyle);
  if (Http::httpOpen(universe_url, destinationsHttpReader, &box, 0) < 0) {
    delete &box;
    return;
  }
  printf("destinations: universe=%s name=%s pfx=%s : open OK\n", universe_url, Universe::current()->name, Universe::current()->urlpfx);
  worlds_dialog.setMessage(uscrollpane(usize(120,400) + box));
  worlds_dialog.show();
}

/** Dialog box for worlds list */
void Widgets::openWorldsDialog()
{
  char universe_url[URL_LEN];
  sprintf(universe_url, "%s%s/vacs/v%d/worlds",
                      Universe::current()->name,
                      Universe::current()->urlpfx,
                      Universe::current()->version);

  UBox* box = new UTextarea;
  if (Http::httpOpen(universe_url, universeHttpReader, box, 0) < 0) {
    delete box;
    return;
  }
  worlds_dialog.setMessage(uscrollpane(usize(450,350) + box));
  worlds_dialog.show();
}

UDialog& Widgets::createSettingsDialog()
{
  URadioSelect
    &sel_audio_live      = uradioSelect(),
    &sel_video_live      = uradioSelect(),
    &sel_wb              = uradioSelect(),
    &sel_browser         = uradioSelect(),
    &sel_audio_streaming = uradioSelect(),
    &sel_video_streaming = uradioSelect(),
    &sel_modeler         = uradioSelect(),
    &sel_pspdf           = uradioSelect(),
    &sel_office          = uradioSelect(),
    &sel_session         = uradioSelect();

  return udialog
  (
   uhbox("Audio live        : " + UFont::plain
           + ucheckbox("Rat" + sel_audio_live
                       + UOn::select / ucall(this, RAT_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Vat" + sel_audio_live
                       + UOn::select / ucall(this, VAT_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Fphone" + sel_audio_live
                       + UOn::select / ucall(this, FPHONE_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Video live      : " + UFont::plain
           + ucheckbox("Vic" + sel_video_live
                       + UOn::select / ucall( this, VIC_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Vlc" + sel_video_live
                       + UOn::select / ucall(this, VLCMC_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Whiteboard      : " + UFont::plain
           + ucheckbox("Wb" + sel_wb
                       + UOn::select / ucall(this, WB_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Wbd" + sel_wb
                       + UOn::select / ucall(this, WBD_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Nte" + sel_wb
                       + UOn::select / ucall(this, NTE_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Browser         : " + UFont::plain
           + ucheckbox("Firefox" + sel_browser
                       + UOn::select / ucall(this, FIREFOX_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Chrome" + sel_browser
                       + UOn::select / ucall(this, CHROME_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Safari" + sel_browser
                       + UOn::select / ucall(this, SAFARI_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Opera" + sel_browser
                       + UOn::select / ucall(this, OPERA_TOOL, &Widgets::setPrefCB))
           + ucheckbox("I-Explorer" + sel_browser
                       + UOn::select / ucall(this, IEXPLORER_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Audio streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &Widgets::setPrefCB)
                       ).setSelected()
           + ucheckbox("Mpg123" + sel_audio_streaming
                       + UOn::select / ucall(this, MPG123_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Freeamp" + sel_audio_streaming
                       + UOn::select / ucall(this, FREEAMP_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Quicktime" + sel_audio_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Video streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Mpegplay" + sel_video_streaming
                       + UOn::select / ucall(this, MPEGPLAY_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Quicktime" + sel_video_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Modeler         : " + UFont::plain
           + ucheckbox("Vred" + sel_modeler
                       + UOn::select / ucall(this, VRED_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Vrem" + sel_modeler
                       + UOn::select / ucall(this, VREM_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("PsPdf           : " + UFont::plain
           + ucheckbox("Evince" + sel_pspdf
                       + UOn::select / ucall(this, EVINCE_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("Gv" + sel_pspdf
                       + UOn::select / ucall(this, GV_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Ghostscript" + sel_pspdf
                       + UOn::select / ucall(this, GHOSTVIEW_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Acrobat" + sel_pspdf
                       + UOn::select / ucall(this, ACROBAT_TOOL, &Widgets::setPrefCB))
           + ucheckbox("Xpdf" + sel_pspdf
                       + UOn::select / ucall(this, XPDF_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Office          : " + UFont::plain
           + ucheckbox("LibreOffice" + sel_office
                       + UOn::select / ucall(this, LIBROFFICE_TOOL, &Widgets::setPrefCB))
                       .setSelected()
           + ucheckbox("MsOffice" + sel_office
                       + UOn::select / ucall(this, MSOFFICE_TOOL, &Widgets::setPrefCB))
           + ucheckbox("OpenOffice" + sel_office
                       + UOn::select / ucall(this, OPENOFFICE_TOOL, &Widgets::setPrefCB))
           + ucheckbox("StarOffice" + sel_office
                       + UOn::select / ucall(this, STAROFFICE_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + uhbox("Session         : " + UFont::plain
           + ucheckbox("Ssh" + sel_session
                       + UOn::select / ucall(this, SSH_TOOL, &Widgets::setPrefCB)
                       ).setSelected()
           + ucheckbox("Telnet" + sel_session
                       + UOn::select / ucall(this, TELNET_TOOL, &Widgets::setPrefCB))
           )
   + uhbox(UBorder::shadowOut)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
   );
}

UDialog& Widgets::createGridDialog()
{
  return udialog(Grid::grid()->buildBox());
}

UDialog& Widgets::createToolDialog()
{
  return udialog
  (  ubutton("Audio"      + ucall(this, true, &Widgets::setAudioCB))
   + ubutton("Video"      + ucall(this, true, &Widgets::setVideoCB))
   + ubutton("Whiteboard" + ucall(this, true, &Widgets::setWhiteboardCB))
   + ubutton("Modeler"    + ucall(this, true, &Widgets::setModelerCB))
   + uhbox(UBorder::shadowIn)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
   );
}

//---------------------------------------------------------------------------
/*
 * addbj
 */
#ifdef HUGE
#undef HUGE
#endif

/* items */
enum {
  NONE,
  THING, MIRAGE, WALL, GROUND, BALL, STEP, GATE,
  CUBE, SPHERE, CONE, CYLINDER, DISK, TORUS,
  MODEL, MAN, CAR, SHRUB, TREE, PENGUIN,
  CHAIR_WOOD, TABLE_WOOD, TABLE_METAL, TABLE_GLASS,
  BLACK, RED, GREEN, BLUE, PURPLE, YELLOW, CYAN, WHITE,
  TINY, SMALL, MEDIUM, BIG, HUGE,
  WOOD, MARBLE, BRICK, STUC, GRASS, PAPER, WATER, CLOUD, CHECK,
  OPAQUE, OPAQUE8, OPAQUE6, OPAQUE4, OPAQUE2, OPAQUE0,
  END
};

// Local
static uint8_t obj = THING;	// thing
static char shape[16] = "cube";	// box
static char texture[128] = "";	// empty
static V3 color;		// black
static float alpha = 1;		// opaque
static float size = .5;		// small

static const char chair_wood[] = "\
<solid dim=\".25 .25 .01\" dif=\".5 .3 .1\" zp=\"/gif/wood.gif\" />\n\
<solid dim=\".02 .25 .25\" rel=\".24 0 .24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.24 .24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".24 .24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.24 -.24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".24 -.24 -.24 0 0\" dif=\".5 .3 .1\" />\n\
";

static const char table_wood[] = "\
<solid dim=\".40 .80 .02\" yp=\"/gif/blondwood.gif\" xp=\"/gif/blondwood.gif\" xn=\"/gif/blondwood.gif\" yn=\"/gif/blondwood.gif\" zp=\"/gif/blondwood.gif\" />\n\
<solid dim=\".02 .02 .40\" rel=\"-.36 .76 -.39 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\".36 .76 -.39 0 0\"  dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\"-.36 -.76 -.39 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\".36 -.76 -.39 0 0\" dif=\".5 .3 .1\" />\n\
";

static const char table_metal[] = "\
<solid dim=\".70 .3 .02\" dif=\".5 .5 .5\" />\n\
<solid dim=\".02,.02 .35\" dif=\".5 .5 .5\" rel=\"-.68 -.28 -.35 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\"-.68 .28 -.35 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\".68 -.28 -.35 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\".68 .28 -.35 0 0\" />\n\
";

static const char table_glass[] = "\
<solid dim=\".5,.3 .02\" dif=\"0 1 0\" spe=\"0 1 1\" a=\".4\" />\n\
<solid shape=\"cone\" rb=\".1\" rt=\".1\" h=\".30\" dif=\"0 1 0\" spe=\"0 1 1\" a=\".5\" rel=\"0 0 -.30 0 0\" />\n\
";

static void resetForm() {
  obj = THING;
  strcpy(shape, "cube");
  *texture = '\0';
  alpha = 1;
  size = .5;
  color = newV3(1, 1, 1);
}

/** pour l'addition d'objet */
static void setForm(int item) {
  switch (item) {
    // objs :
    case THING : case MIRAGE : case WALL : case GROUND : case BALL : case STEP :
      obj = item; break;

    // shapes
    case SPHERE :   sprintf(shape, "sphere"); break;
    case CUBE :     sprintf(shape, "cube"); break;
    case CONE :     sprintf(shape, "cone"); break;
    case CYLINDER : sprintf(shape, "cylinder"); break;
    case DISK :     sprintf(shape, "disk"); break;
    case TORUS :    sprintf(shape, "torus"); break;

    // colors
    case BLACK :  color = newV3(0,0,0); break;
    case RED :    color = newV3(1,0,0); break;
    case GREEN :  color = newV3(0,1,0); break;
    case BLUE :   color = newV3(0,0,1); break;
    case PURPLE : color = newV3(1,0,1); break;
    case YELLOW : color = newV3(1,1,0); break;
    case CYAN :   color = newV3(0,1,1); break;
    case WHITE :  color = newV3(1,1,1); break;

    // textures
    case WOOD :   sprintf(texture, "/gif/wood.gif"); break;
    case MARBLE : sprintf(texture, "/gif/marble.gif"); break;
    case STUC :   sprintf(texture, "/gif/stuc.gif"); break;
    case BRICK :  sprintf(texture, "/gif/brick.gif"); break;
    case GRASS :  sprintf(texture, "/gif/grass.gif"); break;
    case PAPER :  sprintf(texture, "/gif/paper.gif"); break;
    case WATER :  sprintf(texture, "/gif/water.gif"); break;
    case CLOUD :  sprintf(texture, "/gif/clouds.gif"); break;
    case CHECK :  sprintf(texture, "/gif/check.gif"); break;

    // alpha
    case OPAQUE  : alpha = 1; break;
    case OPAQUE8 : alpha = .8; break;
    case OPAQUE6 : alpha = .6; break;
    case OPAQUE4 : alpha = .4; break;
    case OPAQUE2 : alpha = .2; break;
    case OPAQUE0 : alpha = .0; break;

    // sizes
    case TINY :   size = .125; break;
    case SMALL :  size = .25; break;
    case MEDIUM : size = .5; break;
    case BIG :    size = 1; break;
    case HUGE :   size = 2; break;

    // models
    case MAN :     sprintf(shape, "man"); break;
    case CAR :     sprintf(shape, "car"); break;
    case SHRUB :   sprintf(shape, "shrub"); break;
    case TREE :    sprintf(shape, "tree"); break;
    case PENGUIN : sprintf(shape, "penguin"); break;

    // solids
    case CHAIR_WOOD :  sprintf(shape, "chair_wood"); break;
    case TABLE_WOOD :  sprintf(shape, "table_wood"); break;
    case TABLE_METAL : sprintf(shape, "table_metal"); break;
    case TABLE_GLASS : sprintf(shape, "table_glass"); break;
  }
}

void Widgets::addNewObjectCB()
{
  char s[1024], url[128];
  float r = .2 * size;
  float scale = 1;
  V3 c = color;

  s[0] = url[0] = '\0';

  // Shapes
  if (! strcmp(shape, "cube")) {
    sprintf(s, "solid shape=\"box\" dim=\"%f %f %f\" dif=\"%f %f %f\" tx_front=\"%s\" a=\"%f\"/>", r,r,r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "sphere")) {
    sprintf(s, "solid shape=\"sphere\" r=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "cone")) {
    sprintf(s, "solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,r/2,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "cylinder")) {
    sprintf(s, "solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,r,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "disk")) {
    sprintf(s, "solid shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "torus")) {
    sprintf(s, "solid shape=\"torus\" ri=\"%f\" re=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\"/>", r,r/5, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  if (! strcmp(shape, "man")) {
    sprintf(s, "solid shape=\"man\" dim=\".8 .15 .85\" a=\"%f\"/>", alpha);
  }
  if (! strcmp(shape, "car")) {
    sprintf(s, "solid shape=\"car\" dim=\".7 .7 .7\" dif=\"%f %f %f\" a=\"%f\"/>", c.v[0],c.v[1],c.v[2], alpha);
  }

  // Models
  else if (! strcmp(shape, "shrub")) {
    obj = MODEL;
    sprintf(url, "/3ds/tree_1.3ds");
    scale = .05;
  }
  else if (! strcmp(shape, "tree")) {
    obj = MODEL;
    sprintf(url, "/3ds/tree_2.3ds");
    scale = .07;
  }
  else if (! strcmp(shape, "penguin")) {
    obj = MODEL;
    sprintf(url, "/lwo/penguin.lwo");
    scale = .3;
  }
  else if (! strcmp(shape, "chair_wood")) {
    sprintf(s, "%s", chair_wood);
  }
  else if (! strcmp(shape, "table_wood")) {
    sprintf(s, "%s", table_wood);
  }
  else if (! strcmp(shape, "table_metal")) {
    sprintf(s, "%s", table_metal);
  }
  else if (! strcmp(shape, "table_glass")) {
    sprintf(s, "%s", table_glass);
  }

  // Calls constructor of the selectioned object
  if (! localuser) return;

  switch (obj) {
  case MIRAGE:
    new Mirage(localuser, s); break;
  case WALL:
    new Wall(localuser, s); break;
  case GROUND:
    sprintf(s, "solid dim=\"%f %f %f\" dif=\"%f %f %f\" zp=\"%s\"/>", 10.,10.,.1, c.v[0],c.v[1],c.v[2], texture);
    new Ground(localuser, s); break;
  case BALL:
    new Ball(localuser, s); break;
  case THING:
    new Thing(localuser, s); break;
  case STEP:
    new Step(localuser, s); break;
  case MODEL:
    new Model(localuser, url, scale); break;
  }

  resetForm();	// reset to default values
}

UDialog& Widgets::createAddobjDialog()
{
  URadioSelect
    &sel_obj   = uradioSelect(),
    &sel_shape = uradioSelect(),
    &sel_color = uradioSelect(),
    &sel_tex   = uradioSelect(),
    &sel_alpha = uradioSelect(),
    &sel_size  = uradioSelect(),
    &sel_model = uradioSelect(),
    &sel_solid = uradioSelect() ;

  UBox* addobjBox = new UBox
    (UFont::bold
     + UFont::x_large
     + ulabel("Addobj")
     + UFont::medium
     + UFont::plain
     + UColor::navy + UFont::bold
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Simple solids")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Object : " + UFont::plain
              + ucheckbox("Thing" + sel_obj
                          + UOn::select / ucall((int)THING, setForm)
                          ).setSelected()
              + ucheckbox("Mirage" + sel_obj
                          + UOn::select / ucall((int)MIRAGE, setForm))
              + ucheckbox("Wall" + sel_obj
                          + UOn::select / ucall((int)WALL, setForm))
              + ucheckbox("Ground" + sel_obj
                          + UOn::select / ucall((int)GROUND, setForm))
              + ucheckbox("Ball" + sel_obj
                          + UOn::select / ucall((int)BALL, setForm))
              + ucheckbox("Step" + sel_obj
                          + UOn::select / ucall((int)STEP, setForm))
              //+ ucheckbox("Gate" + sel_obj
              //            + UOn::select / ucall((int)GATE, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + uhbox("Shape :  " + UFont::plain
             + ucheckbox("Cube" + sel_shape
                         + UOn::select / ucall((int)CUBE, setForm)
                         ).setSelected()
             + ucheckbox("Sphere" + sel_shape
                         + UOn::select / ucall((int)SPHERE, setForm))
             + ucheckbox("Cone" + sel_shape
                         + UOn::select / ucall((int)CONE, setForm))
             + ucheckbox("Cylinder" + sel_shape
                         + UOn::select / ucall((int)CYLINDER, setForm))
             + ucheckbox("Disk" + sel_shape
                         + UOn::select / ucall((int)DISK, setForm))
             + ucheckbox("Torus" + sel_shape
                         + UOn::select / ucall((int)TORUS, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + uhbox("Color :  " + UFont::plain
             + ucheckbox("Black" + sel_color
                         + UOn::select / ucall((int)BLACK, setForm)
                         ).setSelected()
             + ucheckbox("White" + sel_color
                         + UOn::select / ucall((int)WHITE, setForm))
             + ucheckbox("Red" + sel_color
                         + UOn::select / ucall((int)RED, setForm))
             + ucheckbox("Green" + sel_color
                         + UOn::select / ucall((int)GREEN, setForm))
             + ucheckbox("Blue" + sel_color
                         + UOn::select / ucall((int)BLUE, setForm))
             + ucheckbox("Purple" + sel_color
                         + UOn::select / ucall((int)PURPLE, setForm))
             + ucheckbox("Yellow" + sel_color
                         + UOn::select / ucall((int)YELLOW, setForm))
             + ucheckbox("Cyan" + sel_color
                         + UOn::select / ucall((int)CYAN, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Texture : " + UFont::plain
             + ucheckbox("Wood" + sel_tex
                         + UOn::select / ucall((int)WOOD, setForm)
                         ).setSelected()
             + ucheckbox("Marble" + sel_tex
                         + UOn::select / ucall((int)MARBLE, setForm))
             + ucheckbox("Stuc" + sel_tex
                         + UOn::select / ucall((int)STUC, setForm))
             + ucheckbox("Brick" + sel_tex
                         + UOn::select / ucall((int)BRICK, setForm))
             + ucheckbox("Grass" + sel_tex
                         + UOn::select / ucall((int)GRASS, setForm))
             + ucheckbox("Paper" + sel_tex
                         + UOn::select / ucall((int)PAPER, setForm))
             + ucheckbox("Water" + sel_tex
                         + UOn::select / ucall((int)WATER, setForm))
             + ucheckbox("Cloud" + sel_tex
                         + UOn::select / ucall((int)CLOUD, setForm))
             + ucheckbox("Check" + sel_tex
                         + UOn::select / ucall((int)CHECK, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Alpha :  " + UFont::plain
             + ucheckbox("Opaque" + sel_alpha 
                         + UOn::select / ucall((int)OPAQUE, setForm)
                         ).setSelected()
             + ucheckbox(".8" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE8, setForm))
             + ucheckbox(".6" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE6, setForm))
             + ucheckbox(".4" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE4, setForm))
             + ucheckbox(".2" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE2, setForm))
             + ucheckbox("Translucid" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE0, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Size :   " + UFont::plain
             + ucheckbox("Tiny" + sel_size
                         + UOn::select / ucall((int)TINY, setForm))
             + ucheckbox("Small" + sel_size 
                         + UOn::select / ucall((int)SMALL, setForm)
                         ).setSelected()
             + ucheckbox("Normal" + sel_size
                         + UOn::select / ucall((int)MEDIUM, setForm))
             + ucheckbox("Big" + sel_size
                         + UOn::select / ucall((int)BIG, setForm))
             + ucheckbox("Huge" + sel_size
                         + UOn::select / ucall((int)HUGE, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Models")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Model :  " + UFont::plain
             + ucheckbox("Car" + sel_model
                         + UOn::select / ucall((int)CAR, setForm))
             + ucheckbox("Man" + sel_model
                         + UOn::select / ucall((int)MAN, setForm))
             + ucheckbox("Shrub" + sel_model
                         + UOn::select / ucall((int)SHRUB, setForm))
             + ucheckbox("Tree" + sel_model
                         + UOn::select / ucall((int)TREE, setForm))
             + ucheckbox("Penguin" + sel_model
                         + UOn::select / ucall((int)PENGUIN, setForm))
            )
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Compound solids")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Solids :  " + UFont::plain
             + ucheckbox("Chair wood" + sel_solid
                         + UOn::select / ucall((int)CHAIR_WOOD, setForm))
             + ucheckbox("Table wood" + sel_solid
                         + UOn::select / ucall((int)TABLE_WOOD, setForm))
             + ucheckbox("Table metal" + sel_solid
                         + UOn::select / ucall((int)TABLE_METAL, setForm))
             + ucheckbox("Table glass" + sel_solid
                         + UOn::select / ucall((int)TABLE_GLASS, setForm))
            )
    + uhbox(UBorder::shadowOut)
    + uhcenter()
    + uhbox(uhflex()
      + ubutton(UFont::bold + uhcenter() + " Add " 
                + ucall(this, &Widgets::addNewObjectCB))
      + ubutton(UFont::bold + uhcenter() + " Cancel " + ucloseWin()))
    );

  return udialog(addobjBox);
}

//---------------------------------------------------------------------------

UMenu& Widgets::createFileMenu()
{
  // Open a vreng URL
  UStr& url_or_name = ustr();

  UDialog* openvre_dialog =  // args are: title, content, icon, buttons
  new UOptionDialog("Open Vreng URL",
                     ulabel(UFont::bold + "URL or name: ")
                     // space for 80 chars, shows and edits string 'url_or_name'
                     + uhflex() + utextfield(80, url_or_name),
                     UArgs::none,
                     uhbox(ubutton("  Open  " + ucall(&gui, (const UStr&)url_or_name, &Gui::gotoWorld))
                           + ubutton("  Cancel  " + ucloseWin()))
                     );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Put & Publish URL
  UBox& puturl_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(20, UFont::bold + "Url" + UFont::plain + " (required)" )
                + uhflex() + utextfield(65, putinfo.url))
        + uhbox(ulabel(20, UFont::bold + "Alias" + UFont::plain + " (short name)" )
                + uhflex() + utextfield(65, putinfo.name))
        + uhbox(ulabel(20, UFont::bold + "Icon" + UFont::plain + " (optional)" )
                + uhflex() + utextfield(65, putinfo.icon))
        );

  UDialog* puturl_dialog =
  new UOptionDialog("Put new URL document", //title
                    puturl_box,		// message
                    UArgs::none,	// no icon
                    ubutton("  Put & Publish URL  "
                            + ucall(&putinfo,&PutInfo::putIconCB))); //buttons

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Put & Publish File
  UBox& putfile_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(25, UFont::bold + "Input File" + UFont::plain + " (required)" )
               + uhflex() + utextfield(65, putinfo.file))
        + uhbox( ulabel(25, UFont::bold +"Output File" + UFont::plain + " (public location)" )
                + uhflex() + utextfield(65, putinfo.ofile))
        + uhbox(ulabel(25, UFont::bold + "Alias" + UFont::plain + " (short name)" )
                + uhflex() + utextfield(65, putinfo.name))
        + uhbox(ulabel(25, UFont::bold + "Icon" + UFont::plain + " (optional)" )
                + uhflex() + utextfield(65, putinfo.icon))
        );

  UDialog* putfile_dialog =
  new UOptionDialog("Put new File document",  // title
                    putfile_box,  // message
                    UArgs::none,  // no icon
                    ubutton("  Put & Publish File  "
                            + ucall(&putinfo, &PutInfo::putIconCB)));

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create File menu
  return umenu(ubutton(g.theme.World  + " Open Vreng URL..." + openvre_dialog)
             + ubutton(g.theme.Save + " Save Vreng File" + ucall(this, &Widgets::saveCB))
             + usepar()
             + ubutton(g.theme.Doc  + " Put & Publish URL..." + puturl_dialog)
             + ubutton(g.theme.Book + " Put & Publish File..." + putfile_dialog)
             + usepar()
             + ubutton(g.theme.Exit + " Quit" + ucall(0/*status*/, Global::quitVreng))
             );
}
