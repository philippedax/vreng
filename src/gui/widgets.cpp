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
#include "gui.hpp"	// Gui
#include "navig.hpp"	// Navig
#include "panels.hpp"	// Panels
#include "message.hpp"	// Message
#include "theme.hpp"	// Theme
#include "user.hpp"	// UserAction
#include "move.hpp"	// changeKey
#include "vnc.hpp"	// Vnc
#include "capture.hpp"	// Capture
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
#include "grid.hpp"	// gridBox
#include "file.hpp"	// openFile
#include "cache.hpp"	// openCache
#include "tool.hpp"	// TOOLS
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
#include "render.hpp"	// g.render.bufferSelection
#include "move.hpp"	// changeKey
#include "file.hpp"	// openFile
#include "icon.hpp"	// user

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
 putinfo(*new Message2),
 capture(*new Capture),
 scene(*new Scene(this)),
 navig(*new Navig(this, scene)),
 source_dialog(*new UOptionDialog("World source")),
 worlds_dialog(*new UOptionDialog("World list")),
 prefs_dialog(prefsDialog()),
 settings_dialog(settingsDialog()),
 grid_dialog(gridDialog()),
 tool_dialog(toolDialog()),
 addobj_dialog(addobjDialog()),
 message(*new Message(this)),
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
  UAppli::onMessage("back",   ucall(this, &Widgets::prevCB));
  UAppli::onMessage("forward",ucall(this, &Widgets::nextCB));
  UAppli::onMessage("home",   ucall(this, &Widgets::homeCB));
}


UBox& Widgets::createInfobar()
{
  UBox& navigbox =
  uhbox(upadding(8,0) + uhspacing(8) + uleft() + UFont::bold
        + uitem(USymbol::left + UBackground::green  + utip("Prev world") + ucall(this, &Widgets::prevCB))
        + uitem(USymbol::up   + UBackground::green  + utip("Home world") + ucall(this, &Widgets::homeCB))
        + uitem(USymbol::right + UBackground::green + utip("Next world") + ucall(this, &Widgets::nextCB))
        + usepar()
        );
  // infos initially contains the welcome message,
  // its changed dynamically when objects are selected
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
  UMenu& file_menu = fileMenu();
  file_menu.addAttr(g.theme.menuStyle);

  UMenu& view_menu =
  umenu(g.theme.menuStyle
        + ubutton(g.theme.Edit  + "Source"      + ucall(this, &Widgets::sourceDialog))
        + ubutton(g.theme.List  + "Worlds"      + ucall(this, &Widgets::worldsDialog))
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
        + ubutton("Previous World"   + ucall(this, &Widgets::prevCB))
        + ubutton("Next World"       + ucall(this, &Widgets::nextCB))
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
           + ubutton("Go"      + ucall(this, &Widgets::goDialog))
           + ubutton("Tools"   + tool_menu)
           + ubutton("History" + history_menu)
         );

  menu_bar.add(ubutton("Marks" + markMenu()));
  dynamicMenus(menu_bar, ::g.env.menu());
  menu_bar.add(ubutton("Help" + help_menu));
  return menu_bar;
}


static void functionMenu(Widgets*)
{
  trace(DBG_FORCE, "functionMenu OK");
}

/** Adds dynamic menus */
void Widgets::dynamicMenus(UMenubar& menu_bar, const char* filename)
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
UMenu& Widgets::markMenu()
{
  UBox& mark_box = uvbox();
  mark_box.add(ubutton(UBackground::none
                       + "Add Worldmark"
                       + ucall(this, &Widgets::markCB))
                      );
  UMenu& mark_menu = umenu(g.theme.menuStyle
                           + uscrollpane(usize().setHeight(80)
                           + UBackground::none
                           + mark_box)
                          );

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
  gw->removeAll(true);	//FIXME! sometimes segfault in Ubit
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

/** do an action on local user */
void Widgets::callAction(int numaction)
{
  if (! localuser)  return;

  struct timeval t;
  gettimeofday(&t, NULL);
  localuser->specialAction(numaction, NULL, t.tv_sec, t.tv_usec);	// do action
}

/** do an action on current object */
static void objectActionCB(int numaction)
{
  WObject* object = g.gui.getSelectedObject();
  if (object) {
    struct timeval t;
    gettimeofday(&t, NULL);
    object->specialAction(numaction, NULL, t.tv_sec, t.tv_usec);	// do action
  }
}

/** returns info about the pointed object */
WObject* Widgets::pointedObject(int x, int y, ObjInfo *objinfo, int z)
{
  trace(DBG_GUI, "Pointed: clic=%d,%d %d", x, y, z);
  static char *classname = 0, *instancename = 0, *actionnames = 0;

  // Interaction GUI <--> 3D
  uint16_t num = g.render.bufferSelection(x, y);	// find object number in the Z-buffer
  trace(DBG_GUI, "num=%d", num);

  WObject* object = WObject::byNum(num);

  if (! object) {
    objinfo[0].name = (char*) "World";	// avoid segfault
    objinfo[1].name = (char*) World::current()->getName();
    objinfo[2].name = NULL;	// NULL terminaison
    return NULL;
  }
  if (! object->isVisible()) {
    return NULL;	// invisible
  }

  // an object has been selected
  // get the object hname
  object->getObjectHumanName(&classname, &instancename, &actionnames);
  if (classname == NULL)  return NULL;
  objinfo[0].name = classname;
  if (instancename == NULL) instancename = (char *)"";
  objinfo[1].name = instancename;   // TO BE COMPLETED
  if (::g.pref.dbgtrace) trace(DBG_FORCE, "Pointed: %s", classname);

  // get actions of this object
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
  if (localuser) {
    localuser->setRayDirection(x, y);	// play the ray
  }
}

//
// CALLBACKS
//

void Widgets::homeCB()
{
  char chan_str[CHAN_LEN];

  sprintf(chan_str, "%s/%u/%d", Universe::current()->group, Universe::current()->port, Channel::currentTtl());
  trace(DBG_IPMC, "WO: goto %s at %s", Universe::current()->url, chan_str);

  World::current()->quit();
  delete Channel::current();  // delete Channel
  World::enter(Universe::current()->url, chan_str, true);
  World::current()->setChanAndJoin(chan_str);  // join new channel

  if (audioactive) Audio::start(chan_str);
}

void Widgets::prevCB()
{
  World::goPrev();
}

void Widgets::nextCB()
{
  World::goNext();
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
      notice("world %s saved", vreout);
    }
  }
}

void Widgets::helpCB()
{
  UStr cmd = "IFS=' '; firefox -remote 'openURL(http://"
              & UStr(Universe::current()->server)
              & "/)' &";
  system(cmd.c_str());
}

void Widgets::prefCB(int tool)
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

void Widgets::audioCB(bool on)
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

void Widgets::videoCB(bool on)
{
  if (on) Video::start(World::current()->getChan());
  else    Video::quit();
}

void Widgets::whiteboardCB(bool on)
{
  if (on) Wb::start(World::current()->getChan());
  else    Wb::quit();
}

void Widgets::modelerCB(bool on)
{
  if (on) Modeler::start();
  else    Modeler::quit();
}

void Widgets::markCB()
{
  FILE *fp;
  char mark[URL_LEN + CHAN_LEN + 2];
  char buf[URL_LEN + CHAN_LEN + 2];

  sprintf(mark, "%s %s\n", World::current()->getUrl(), World::current()->getChan());
  if ((fp = File::openFile(::g.env.worldmarks(), "r")) != NULL) {
    while (fgets(buf, sizeof(buf), fp)) {
      if (! strcmp(buf, mark)) {
        File::closeFile(fp);
        return;
      }
    }
    File::closeFile(fp);
  }
  if ((fp = File::openFile(::g.env.worldmarks(), "a")) != NULL) {
    fputs(mark, fp);
    File::closeFile(fp);
    markMenu();
  }
}

//---------------------------------------------------------------------------
/*
 *  Key management and correspondance with VREng
 */

void Widgets::setKey(int key, int ispressed)
{
  struct timeval t;

  gettimeofday(&t, NULL);       // get time
  changeKey(key, ispressed, t.tv_sec, t.tv_usec);
}

/*
 * converts the X keysym into a Vreng change key (vrkey) and returns
 * a keymask which is an hexa value for marking released keys in the KRmask
 */
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
    Widgets::callAction(UserAction::UA_ASPEEDLESS); return 0; 		    // decrease aspeed
  }
  else if (keycode == UKey::Tab) {
    Widgets::callAction(UserAction::UA_ASPEEDMORE); return 0; 		    // increase aspeed
  }
  else {
    switch (keychar) {
      case '<': keymask = 1<<4;          vrkey = KEY_SG;	break;	  // left translation
      case '>': keymask = 1<<5;          vrkey = KEY_SD;	break;    // right translation
      case 'l': keymask = 1<<9;          vrkey = KEY_TL;	break;    // tilt left
      case 'r': keymask = 1<<10;         vrkey = KEY_TR;	break; 	  // tilt right
      case 'u': keymask = 1<<11 | 1<<13; vrkey = KEY_JU;	break;    // up translation
      case ' ': keymask = 1<<13;         vrkey = KEY_VI;	break;    // accelerator
      case '=': Widgets::callAction(UserAction::UA_FOVYDEF);    return 0; // original fovy
      case '-': Widgets::callAction(UserAction::UA_FOVYLESS);   return 0; // decrease fovy
      case '+': Widgets::callAction(UserAction::UA_FOVYMORE);   return 0; // increase fovy
      case '.': Widgets::callAction(UserAction::UA_LSPEEDDEF);  return 0; // original lspeed
      case 's': Widgets::callAction(UserAction::UA_LSPEEDLESS); return 0; // decrease lspeed
      case 'f': Widgets::callAction(UserAction::UA_LSPEEDMORE); return 0; // increase lspeed
      case ',': Widgets::callAction(UserAction::UA_ASPEEDDEF);  return 0; // original aspeed
      case 'b': Widgets::callAction(UserAction::UA_BULLET);     return 0; // launche bullet
      case 'd': Widgets::callAction(UserAction::UA_DART);       return 0; // launche dart
      case 'v': Widgets::callAction(UserAction::UA_SWITCHVIEW); return 0; // switch view
      case 'x': Widgets::callAction(UserAction::UA_TPVIEWROTL); return 0; // rot left
      case 'c': Widgets::callAction(UserAction::UA_TPVIEWROTR); return 0; // rot right
      case 'q': Widgets::callAction(UserAction::UA_TPVIEWROTU); return 0; // rot up
      case 'w': Widgets::callAction(UserAction::UA_TPVIEWROTD); return 0; // rot down
      case 'p': Widgets::callAction(UserAction::UA_TPVIEWROTN); return 0; // rot near
      case 'm': Widgets::callAction(UserAction::UA_TPVIEWROTF); return 0; // rot far
      case 'D': Widgets::callAction(UserAction::UA_PITCHMORE);  return 0; // increase pitch
      case 'U': Widgets::callAction(UserAction::UA_PITCHLESS);  return 0; // decrease pitch
      case 'R': Widgets::callAction(UserAction::UA_ROLLMORE);   return 0; // increase roll
      case 'L': Widgets::callAction(UserAction::UA_ROLLLESS);   return 0; // decrease roll
      case '^': Widgets::callAction(UserAction::UA_FLYAWAY);    return 0; // flyaway
      case '$': Widgets::callAction(UserAction::UA_TOLAND);     return 0; // toland
      default: return 0; 						  // undefined key
    }
  }
  return keymask;
}

/** Use this callback function for handling AUTOREPEAT Key event
 * Autorepeat is logically suppressed by annulating corresponding Press and Release events.
 * Principle: Release events are postponed and temporarily stored in KRmask
 * They are processed when coming back to the RenderingWorkProc of the
 * mainLoop if not annulated in the meantime by subsequent Press events.
 */
void Widgets::processKey(long keysym, int keychar, bool press)
{
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
      //fprintf(stderr, "KPress change or activate Key( %d )\n", vrkey);
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

//---------------------------------------------------------------------------
/*
 *  dialog.cpp : Ubit dialogs for the VREng GUI
 */


/** Displays preferences */
UDialog& Widgets::prefsDialog()
{
  UBox& settings_box = uvbox(UBackground::white + upadding(2,2));

  UDialog* prefs_dialog =
  new UOptionDialog("Preferences",
                    uvbox(ulabel("Preferences: " + UColor::red + ::g.env.prefs())
                          + uvflex()
                          + uscrollpane(usize(400,300) + settings_box)),
                    UArgs::none, UArgs::none);

  FILE *fp;
  char buf[128];
  if ((fp = File::openFile(::g.env.prefs(), "r"))) {
    while (fgets(buf, sizeof(buf), fp)) {
      if (isalpha(*buf)) settings_box.add(uitem(UColor::red + UFont::bold + buf));
      else               settings_box.add(uitem(UColor::black + buf));
    }
    File::closeFile(fp);
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
  while (fgets(line, sizeof(line), fpcache)) {
    source_box->add(uitem(UColor::black + line));
  }
  source_box->setAutoUpdate(true);
  source_box->update();
}

/** Displays list of worlds */
static void goHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *univ_box = (UBox *) box;
  char line[URL_LEN + CHAN_LEN +2];

  for (int i=0 ; http->nextLine(line) && i < MAX_WORLDS ; i++) {
    UStr& url = ustr();

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

    univ_box->add(UBackground::black //+ ualpha(0.5)
                  + uitem(UBackground::none + UColor::green + UFont::bold + UFont::large
                  + name
                  + ucall(&g.gui, (const UStr&) url, &Gui::gotoWorld))
                 );
  }
}

/** Displays list of worlds and their urls */
static void universeHttpReader(void *box, Http *http)
{
  if (! http) return;
  UBox *univ_box = (UBox *) box;
  char line[URL_LEN + CHAN_LEN +2];

  for (int i=0 ; http->nextLine(line) && i < MAX_WORLDS ; i++) {
    UStr& url = ustr();
    UStr& chan = ustr();

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

    univ_box->add(uitem(UColor::navy + UFont::bold
                        + url
                        + " " + UFont::plain
                        + chan
                        + ucall(&g.gui, (const UStr&) url, &Gui::gotoWorld))
                       );
  }
}

/** Dialog box for vre source */
void Widgets::sourceDialog()
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
void Widgets::goDialog()
{
  char univ_url[URL_LEN];
  char fmt[64];

  if (! strncmp(Universe::current()->server, "http://", 7))
    sprintf(fmt, "%s", "%s%s/vacs/v%d/worlds");
  else
    sprintf(fmt, "%s%s", "http://", "%s/%s/vacs/v%d/worlds");
  sprintf(univ_url, fmt, Universe::current()->server,
                         Universe::current()->urlpfx,
                         Universe::current()->version);
  //error("url: %s", univ_url);

  UBox& box = uvbox(g.theme.scrollpaneStyle);
  if (Http::httpOpen(univ_url, goHttpReader, &box, 0) < 0) {
    delete &box;
    return;
  }
  worlds_dialog.setMessage(uscrollpane(usize(120,400) + box));
  worlds_dialog.show();
}

/** Dialog box for worlds list and their urls */
void Widgets::worldsDialog()
{
  char univ_url[URL_LEN];
  char fmt[64];

  if (! strncmp(Universe::current()->server, "http://", 7))
    sprintf(fmt, "%s", "%s%s/vacs/v%d/worlds");
  else
    sprintf(fmt, "%s%s", "http://", "%s/%s/vacs/v%d/worlds");
  sprintf(univ_url, fmt, Universe::current()->server,
                         Universe::current()->urlpfx,
                         Universe::current()->version);

  UBox* box = new UTextarea;
  if (Http::httpOpen(univ_url, universeHttpReader, box, 0) < 0) {
    delete box;
    return;
  }
  worlds_dialog.setMessage(uscrollpane(usize(450,350) + box));
  worlds_dialog.show();
}

UDialog& Widgets::settingsDialog()
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
                       + UOn::select / ucall(this, RAT_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Vat" + sel_audio_live
                       + UOn::select / ucall(this, VAT_TOOL, &Widgets::prefCB))
           + ucheckbox("Fphone" + sel_audio_live
                       + UOn::select / ucall(this, FPHONE_TOOL, &Widgets::prefCB))
        )
   + uhbox(UBorder::shadowOut)
   + uhbox("Video live      : " + UFont::plain
           + ucheckbox("Vic" + sel_video_live
                       + UOn::select / ucall( this, VIC_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Vlc" + sel_video_live
                       + UOn::select / ucall(this, VLCMC_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Whiteboard      : " + UFont::plain
           + ucheckbox("Wb" + sel_wb
                       + UOn::select / ucall(this, WB_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Wbd" + sel_wb
                       + UOn::select / ucall(this, WBD_TOOL, &Widgets::prefCB))
           + ucheckbox("Nte" + sel_wb
                       + UOn::select / ucall(this, NTE_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Browser         : " + UFont::plain
           + ucheckbox("Firefox" + sel_browser
                       + UOn::select / ucall(this, FIREFOX_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Chrome" + sel_browser
                       + UOn::select / ucall(this, CHROME_TOOL, &Widgets::prefCB))
           + ucheckbox("Safari" + sel_browser
                       + UOn::select / ucall(this, SAFARI_TOOL, &Widgets::prefCB))
           + ucheckbox("Opera" + sel_browser
                       + UOn::select / ucall(this, OPERA_TOOL, &Widgets::prefCB))
           + ucheckbox("I-Explorer" + sel_browser
                       + UOn::select / ucall(this, IEXPLORER_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Audio streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &Widgets::prefCB)
                       ).setSelected()
           + ucheckbox("Mpg123" + sel_audio_streaming
                       + UOn::select / ucall(this, MPG123_TOOL, &Widgets::prefCB))
           + ucheckbox("Freeamp" + sel_audio_streaming
                       + UOn::select / ucall(this, FREEAMP_TOOL, &Widgets::prefCB))
           + ucheckbox("Quicktime" + sel_audio_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Video streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Mpegplay" + sel_video_streaming
                       + UOn::select / ucall(this, MPEGPLAY_TOOL, &Widgets::prefCB))
           + ucheckbox("Quicktime" + sel_video_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Modeler         : " + UFont::plain
           + ucheckbox("Vred" + sel_modeler
                       + UOn::select / ucall(this, VRED_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Vrem" + sel_modeler
                       + UOn::select / ucall(this, VREM_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("PsPdf           : " + UFont::plain
           + ucheckbox("Evince" + sel_pspdf
                       + UOn::select / ucall(this, EVINCE_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Gv" + sel_pspdf
                       + UOn::select / ucall(this, GV_TOOL, &Widgets::prefCB))
           + ucheckbox("Ghostscript" + sel_pspdf
                       + UOn::select / ucall(this, GHOSTVIEW_TOOL, &Widgets::prefCB))
           + ucheckbox("Acrobat" + sel_pspdf
                       + UOn::select / ucall(this, ACROBAT_TOOL, &Widgets::prefCB))
           + ucheckbox("Xpdf" + sel_pspdf
                       + UOn::select / ucall(this, XPDF_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Office          : " + UFont::plain
           + ucheckbox("LibreOffice" + sel_office
                       + UOn::select / ucall(this, LIBROFFICE_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("MsOffice" + sel_office
                       + UOn::select / ucall(this, MSOFFICE_TOOL, &Widgets::prefCB))
           + ucheckbox("OpenOffice" + sel_office
                       + UOn::select / ucall(this, OPENOFFICE_TOOL, &Widgets::prefCB))
           + ucheckbox("StarOffice" + sel_office
                       + UOn::select / ucall(this, STAROFFICE_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox("Session         : " + UFont::plain
           + ucheckbox("Ssh" + sel_session
                       + UOn::select / ucall(this, SSH_TOOL, &Widgets::prefCB)
                       ).setSelected()
           + ucheckbox("Telnet" + sel_session
                       + UOn::select / ucall(this, TELNET_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
  );
}

UDialog& Widgets::gridDialog()
{
  //dax return udialog(Grid::grid()->gridBox());
  UDialog &griddial = udialog(Grid::grid()->gridBox());
  griddial.show(true);
  //dax return griddial;
  return udialog(Grid::grid()->gridBox());
}

UDialog& Widgets::toolDialog()
{
  return udialog
  (  ubutton("Audio"      + ucall(this, true, &Widgets::audioCB))
   + ubutton("Video"      + ucall(this, true, &Widgets::videoCB))
   + ubutton("Whiteboard" + ucall(this, true, &Widgets::whiteboardCB))
   + ubutton("Modeler"    + ucall(this, true, &Widgets::modelerCB))
   + uhbox(UBorder::shadowIn)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
  );
}

//---------------------------------------------------------------------------
/*
 * Addobj
 *
 * Dynamicaly objects creation
 */

/* items */
enum {
  NONE,
  WALL, THING, MIRAGE, BALL, STEP, GROUND, GATE,
  BOX, SPHERE, CONE, CYLINDER, DISK, TORUS,
  BLACK, RED, GREEN, BLUE, PURPLE, YELLOW, CYAN, WHITE,
  TINY, SMALL, MEDIUM, BIG, BIGEST,
  OPAQUE, OPAQUE8, OPAQUE6, OPAQUE4, OPAQUE2, OPAQUE0,
  WOOD, MARBLE, BRICK, STUC, GRASS, PAPER, WATER, CLOUD,
  MODEL, MAN, CAR, SHRUB, TREE, PENGUIN,
  CHAIR_WOOD, TABLE_WOOD, TABLE_METAL, TABLE_GLASS,
  END
};

// Local
static uint8_t objtype = THING;	// thing (for multiple solids)
static char shape[16] = "box";	// box
static char texture[128] = "";	// empty
static V3 color;		// white
static float alpha = 1;		// opaque
static float size = .5;		// medium

static const char chair_wood[] = "\
<solid dim=\".25 .25 .01\" dif=\".5 .3 .1\" zp=\"/gif/wood.gif\" xp=\"/gif/wood.gif\" xn=\"/gif/wood.gif\" yp=\"/gif/wood.gif\" yn=\"/gif/wood.gif\" />\n\
<solid dim=\".02 .25 .25\" rel=\".12 0 .12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.12 .12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".12 .12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.12 -.12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".12 -.12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
";
static const char table_wood[] = "\
<solid dim=\".40 .80 .02\" dif=\".5 .3 .1\" yp=\"/gif/blondwood.gif\" xp=\"/gif/blondwood.gif\" xn=\"/gif/blondwood.gif\" yp=\"/gif/blondwood.gif\" yn=\"/gif/blondwood.gif\" />\n\
<solid dim=\".02 .02 .40\" rel=\"-.18 .38 -.20 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\".18 .38 -.20 0 0\"  dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\"-.18 -.38 -.20 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".02 .02 .40\" rel=\".18 -.38 -.20 0 0\" dif=\".5 .3 .1\" />\n\
";
static const char table_metal[] = "\
<solid dim=\".70 .3 .02\" dif=\".5 .5 .5\" />\n\
<solid dim=\".02,.02 .35\" dif=\".5 .5 .5\" rel=\"-.34 -.14 -.17 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\"-.34 .14 -.17 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\".34 -.14 -.17 0 0\" />\n\
<solid dim=\".02 .02 .35\" dif=\".5 .5 .5\" rel=\".34 .14 -.17 0 0\" />\n\
";
static const char table_glass[] = "\
<solid dim=\".5,.3 .02\" dif=\"0 1 0\" spe=\"0 1 1\" a=\".3\" />\n\
<solid shape=\"cone\" rb=\".1\" rt=\".1\" h=\".30\" dif=\"0 1 0\" spe=\"0 1 1\" a=\".5\" rel=\"0 0 -.30 0 0\" />\n\
";

/* default values */
static void defaultAddobj()
{
  objtype = THING;	// thing (default for muliple solids)
  strcpy(shape, "box");	// box
  *texture = '\0';	// no textures
  alpha = 1;		// opaque
  size = .5;		// 50 cm
  color = setV3(1,1,1);	// white
}

/* set values */
static void setVal(int item) {
  switch (item) {
    // objtypes :
    case THING :
    case MIRAGE :
    case WALL :
    case BALL :
    case STEP :
    case GROUND :
      objtype = item;
      break;

    // shapes
    case BOX :      sprintf(shape, "box"); break;
    case SPHERE :   sprintf(shape, "sphere"); break;
    case CONE :     sprintf(shape, "cone"); break;
    case CYLINDER : sprintf(shape, "cylinder"); break;
    case DISK :     sprintf(shape, "disk"); break;
    case TORUS :    sprintf(shape, "torus"); break;

    // colors
    case BLACK :  color = setV3(0,0,0); break;
    case RED :    color = setV3(1,0,0); break;
    case GREEN :  color = setV3(0,1,0); break;
    case BLUE :   color = setV3(0,0,1); break;
    case PURPLE : color = setV3(1,0,1); break;
    case YELLOW : color = setV3(1,1,0); break;
    case CYAN :   color = setV3(0,1,1); break;
    case WHITE :  color = setV3(1,1,1); break;

    // textures
    case NONE :   sprintf(texture, ""); break;
    case WOOD :   sprintf(texture, "/gif/wood.gif"); break;
    case MARBLE : sprintf(texture, "/gif/marble.gif"); break;
    case STUC :   sprintf(texture, "/gif/stuc.gif"); break;
    case BRICK :  sprintf(texture, "/gif/brick.gif"); break;
    case GRASS :  sprintf(texture, "/gif/grass.gif"); break;
    case PAPER :  sprintf(texture, "/gif/paper.gif"); break;
    case WATER :  sprintf(texture, "/gif/water.gif"); break;
    case CLOUD :  sprintf(texture, "/gif/clouds.gif"); break;
    //dax case CHECK :  sprintf(texture, "/gif/check.gif"); break;

    // alpha
    case OPAQUE  : alpha = 1.; break;
    case OPAQUE8 : alpha = .8; break;
    case OPAQUE6 : alpha = .6; break;
    case OPAQUE4 : alpha = .4; break;
    case OPAQUE2 : alpha = .2; break;
    case OPAQUE0 : alpha = .0; break;

    // sizes
    case TINY :   size = .12; break;
    case SMALL :  size = .25; break;
    case MEDIUM : size = .50; break;
    case BIG :    size = 1.0; break;
    case BIGEST : size = 2.0; break;

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

/* callbach witch build <solid ... \> */
void Widgets::newObjectCB()
{
  char solid[BUFSIZ], url[128];
  float r = .2 * size;
  float scale = 1;
  V3 c = color;

  solid[0] = url[0] = '\0';

  // Shapes
  if (! strcmp(shape, "box")) {
    sprintf(solid, "solid shape=\"box\" dim=\"%f %f %f\" dif=\"%f %f %f\" tx_front=\"%s\" a=\"%f\" />", r,r,r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "sphere")) {
    sprintf(solid, "solid shape=\"sphere\" r=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\" />", r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "cone")) {
    sprintf(solid, "solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\" />", r,r/2,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "cylinder")) {
    sprintf(solid, "solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\" />", r,r,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "disk")) {
    sprintf(solid, "solid shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\" />", r,2*r, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  else if (! strcmp(shape, "torus")) {
    sprintf(solid, "solid shape=\"torus\" ri=\"%f\" re=\"%f\" dif=\"%f %f %f\" tx=\"%s\" a=\"%f\" />", r,r/5, c.v[0],c.v[1],c.v[2], texture, alpha);
  }
  if (! strcmp(shape, "man")) {
    sprintf(solid, "solid shape=\"man\" dim=\".2 .1 .85\" />");
  }
  if (! strcmp(shape, "car")) {
    sprintf(solid, "solid shape=\"car\" dim=\".7 .7 .7\" dif=\"%f %f %f\" a=\"%f\" />", c.v[0],c.v[1],c.v[2], alpha);
  }

  // Models
  else if (! strcmp(shape, "shrub")) {
    objtype = MODEL;
    sprintf(url, "/3ds/tree_1.3ds");
    scale = .03;
  }
  else if (! strcmp(shape, "tree")) {
    objtype = MODEL;
    sprintf(url, "/3ds/tree_2.3ds");
    scale = .03;
  }
  else if (! strcmp(shape, "penguin")) {
    objtype = MODEL;
    sprintf(url, "/lwo/penguin.lwo");
    scale = .3;
  }

  // Compound solids
  else if (! strcmp(shape, "chair_wood")) {
    objtype = THING;
    sprintf(solid, "%s", chair_wood);
  }
  else if (! strcmp(shape, "table_wood")) {
    objtype = THING;
    sprintf(solid, "%s", table_wood);
  }
  else if (! strcmp(shape, "table_metal")) {
    objtype = THING;
    sprintf(solid, "%s", table_metal);
  }
  else if (! strcmp(shape, "table_glass")) {
    objtype = THING;
    sprintf(solid, "%s", table_glass);
  }

  if (!solid) {
    printf("solid is null\n");
    defaultAddobj();	// reset to default values
    return;
  }

  // Calls constructor of the selectioned object
  if (! localuser) return;

  switch (objtype) {
  case THING:	new Thing(localuser, solid);	break;
  case WALL:	new Wall(localuser, solid);	break;
  case MIRAGE:	new Mirage(localuser, solid);	break;
  case BALL:	new Ball(localuser, solid);	break;
  case STEP:	new Step(localuser, solid);	break;
  case MODEL:	new Model(localuser, url, scale); break;
  case GROUND:
    sprintf(solid, "solid dim=\"%f %f %f\" dif=\"%f %f %f\" zp=\"%s\"/>", 10.,10.,.1, c.v[0],c.v[1],c.v[2], texture);
    new Ground(localuser, solid); break;
  }

  defaultAddobj();	// reset to default values
}

UDialog& Widgets::addobjDialog()
{
  URadioSelect
    &sel_objtype  = uradioSelect(),
    &sel_shape    = uradioSelect(),
    &sel_color    = uradioSelect(),
    &sel_tex      = uradioSelect(),
    &sel_alpha    = uradioSelect(),
    &sel_size     = uradioSelect(),
    &sel_model    = uradioSelect(),
    &sel_compound = uradioSelect() ;

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
              + ucheckbox("Thing" + sel_objtype
                          + UOn::select / ucall((int)THING, setVal)).setSelected()
              + ucheckbox("Wall" + sel_objtype
                          + UOn::select / ucall((int)WALL, setVal))
              + ucheckbox("Mirage" + sel_objtype
                          + UOn::select / ucall((int)MIRAGE, setVal))
              + ucheckbox("Ball" + sel_objtype
                          + UOn::select / ucall((int)BALL, setVal))
              + ucheckbox("Step" + sel_objtype
                          + UOn::select / ucall((int)STEP, setVal))
              + ucheckbox("Ground" + sel_objtype
                          + UOn::select / ucall((int)GROUND, setVal))
              //+ ucheckbox("Gate" + sel_objtype
              //            + UOn::select / ucall((int)GATE, setVal))
            )
     + uhbox(UBorder::shadowOut)
     + uhbox("Shape :  " + UFont::plain
             + ucheckbox("Cube" + sel_shape
                         + UOn::select / ucall((int)BOX, setVal)).setSelected()
             + ucheckbox("Sphere" + sel_shape
                         + UOn::select / ucall((int)SPHERE, setVal))
             + ucheckbox("Cone" + sel_shape
                         + UOn::select / ucall((int)CONE, setVal))
             + ucheckbox("Cylinder" + sel_shape
                         + UOn::select / ucall((int)CYLINDER, setVal))
             + ucheckbox("Disk" + sel_shape
                         + UOn::select / ucall((int)DISK, setVal))
             + ucheckbox("Torus" + sel_shape
                         + UOn::select / ucall((int)TORUS, setVal))
            )
     + uhbox(UBorder::shadowOut)
     + uhbox("Color :  " + UFont::plain
             + ucheckbox("White" + sel_color
                         + UOn::select / ucall((int)WHITE, setVal)).setSelected()
             + ucheckbox("Black" + sel_color
                         + UOn::select / ucall((int)BLACK, setVal))
             + ucheckbox("Red" + sel_color
                         + UOn::select / ucall((int)RED, setVal))
             + ucheckbox("Green" + sel_color
                         + UOn::select / ucall((int)GREEN, setVal))
             + ucheckbox("Blue" + sel_color
                         + UOn::select / ucall((int)BLUE, setVal))
             + ucheckbox("Purple" + sel_color
                         + UOn::select / ucall((int)PURPLE, setVal))
             + ucheckbox("Yellow" + sel_color
                         + UOn::select / ucall((int)YELLOW, setVal))
             + ucheckbox("Cyan" + sel_color
                         + UOn::select / ucall((int)CYAN, setVal))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Texture : " + UFont::plain
             + ucheckbox("None" + sel_tex
                         + UOn::select / ucall((int)WOOD, setVal))
             + ucheckbox("Wood" + sel_tex
                         + UOn::select / ucall((int)WOOD, setVal))
             + ucheckbox("Marble" + sel_tex
                         + UOn::select / ucall((int)MARBLE, setVal))
             + ucheckbox("Stuc" + sel_tex
                         + UOn::select / ucall((int)STUC, setVal))
             + ucheckbox("Brick" + sel_tex
                         + UOn::select / ucall((int)BRICK, setVal))
             + ucheckbox("Grass" + sel_tex
                         + UOn::select / ucall((int)GRASS, setVal))
             + ucheckbox("Paper" + sel_tex
                         + UOn::select / ucall((int)PAPER, setVal))
             + ucheckbox("Water" + sel_tex
                         + UOn::select / ucall((int)WATER, setVal))
             + ucheckbox("Cloud" + sel_tex
                         + UOn::select / ucall((int)CLOUD, setVal))
             //dax + ucheckbox("Check" + sel_tex
             //dax             + UOn::select / ucall((int)CHECK, setVal))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Alpha :  " + UFont::plain
             + ucheckbox("Opaque" + sel_alpha 
                         + UOn::select / ucall((int)OPAQUE, setVal)).setSelected()
             + ucheckbox(".8" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE8, setVal))
             + ucheckbox(".6" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE6, setVal))
             + ucheckbox(".4" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE4, setVal))
             + ucheckbox(".2" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE2, setVal))
             + ucheckbox("Invisible" + sel_alpha
                         + UOn::select / ucall((int)OPAQUE0, setVal))
            )
    + uhbox(UBorder::shadowOut)
    + uhbox("Size :   " + UFont::plain
             + ucheckbox("Tiny" + sel_size
                         + UOn::select / ucall((int)TINY, setVal))
             + ucheckbox("Small" + sel_size 
                         + UOn::select / ucall((int)SMALL, setVal)).setSelected()
             + ucheckbox("Normal" + sel_size
                         + UOn::select / ucall((int)MEDIUM, setVal))
             + ucheckbox("Big" + sel_size
                         + UOn::select / ucall((int)BIG, setVal))
             + ucheckbox("Huge" + sel_size
                         + UOn::select / ucall((int)BIGEST, setVal))
            )
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Models")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Model :  " + UFont::plain
             + ucheckbox("Car" + sel_model
                         + UOn::select / ucall((int)CAR, setVal))
             + ucheckbox("Man" + sel_model
                         + UOn::select / ucall((int)MAN, setVal))
             + ucheckbox("Shrub" + sel_model
                         + UOn::select / ucall((int)SHRUB, setVal))
             + ucheckbox("Tree" + sel_model
                         + UOn::select / ucall((int)TREE, setVal))
             + ucheckbox("Penguin" + sel_model
                         + UOn::select / ucall((int)PENGUIN, setVal))
            )
     + uhbox(UBorder::shadowOut)
     + UFont::bold
     + ulabel("Compound solids")
     + UFont::plain
     + uhbox(UBorder::shadowOut)
     + uhbox("Solids :  " + UFont::plain
             + ucheckbox("Chair wood" + sel_compound
                         + UOn::select / ucall((int)CHAIR_WOOD, setVal))
             + ucheckbox("Table wood" + sel_compound
                         + UOn::select / ucall((int)TABLE_WOOD, setVal))
             + ucheckbox("Table metal" + sel_compound
                         + UOn::select / ucall((int)TABLE_METAL, setVal))
             + ucheckbox("Table glass" + sel_compound
                         + UOn::select / ucall((int)TABLE_GLASS, setVal))
            )
    + uhbox(UBorder::shadowOut)
    + uhcenter()
    + uhbox(uhflex()
      + ubutton(UFont::bold + uhcenter()
                + " Add " 
                + ucall(this, &Widgets::newObjectCB))
      + ubutton(UFont::bold + uhcenter()
                + " Cancel "
                + ucloseWin()))
    );

  return udialog(addobjBox);
}

//---------------------------------------------------------------------------

UMenu& Widgets::fileMenu()
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
                            + ucall(&putinfo,&Message2::putIconCB))); //buttons

  // Put & Publish File
  UBox& putfile_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(25, UFont::bold + "Input File" + UFont::plain + " (required)" )
               + uhflex() + utextfield(65, putinfo.file))
        + uhbox( ulabel(25, UFont::bold +"Output File" + UFont::plain + " (public location)" )
                + uhflex() + utextfield(65, putinfo.ofile))
        + uhbox(ulabel(25, UFont::bold + "Alias" + UFont::plain + " (short name)" )
                + uhflex() + utextfield(65, putinfo.name))
       );

  UDialog* putfile_dialog =
  new UOptionDialog("Put new File document",  // title
                    putfile_box,  // message
                    UArgs::none,  // no icon
                    ubutton("  Put & Publish File  "
                            + ucall(&putinfo, &Message2::putIconCB)));

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

//---------------------------------------------------------------------------

void Widgets::putMessage(UMessageEvent& e)
{
  const UStr* arg = e.getMessage();

  if (!arg || arg->empty())
    return;

  UStr file_name = arg->basename();
  UStr val;

  if (file_name.empty()) val = "<url=\"" & *arg & "\">";
  else                   val = "<url=\"" & *arg & "\">&<name=\"" & file_name & "\">";
  if (! Cache::check(arg->c_str()))
    return;    // bad url
  putinfo.putIcon(val);
}

void Widgets::openMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();

  if (!msg || msg->empty())
    return;
  gui.gotoWorld(*msg);
}

void Widgets::moveMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();

  if (!msg || msg->empty())
    return;
  const UStr& arg = *msg;

  if (arg == "left 1")            setKey(KEY_SG, TRUE);
  else if (arg == "left 0")       setKey(KEY_SG, FALSE);
  else if (arg == "right 1")      setKey(KEY_SD, TRUE);
  else if (arg == "right 0")      setKey(KEY_SD, FALSE);
  else if (arg == "forward 1")    setKey(KEY_AV, TRUE);
  else if (arg == "forward 0")    setKey(KEY_AV, FALSE);
  else if (arg == "backward 1")   setKey(KEY_AR, TRUE);
  else if (arg == "backward 0")   setKey(KEY_AR, FALSE);
  else if (arg == "up 1")         setKey(KEY_JU, TRUE);
  else if (arg == "up 0")         setKey(KEY_JU, FALSE);
  else if (arg == "down 1")       setKey(KEY_JD, TRUE);
  else if (arg == "down 0")       setKey(KEY_JD, FALSE);
  else if (arg == "turn_left 1")  setKey(KEY_GA, TRUE);
  else if (arg == "turn_left 0")  setKey(KEY_GA, FALSE);
  else if (arg == "turn_right 1") setKey(KEY_DR, TRUE);
  else if (arg == "turn_right 0") setKey(KEY_DR, FALSE);
}

void Widgets::getMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();

  if (!msg || msg->empty())
    return;

  // a completer
  //cerr << "get: " << *selected_object_url << endl;
}

//---------------------------------------------------------------------------

VncDialog* VncDialog::vnc_dialog = null;

void VncDialog::create(Widgets* gw, Vnc* vnc)
{
  if (!gw) return;
  if (!vnc_dialog) {
    vnc_dialog = new VncDialog(gw, vnc);
    gw->add(vnc_dialog);
    vnc_dialog->centerOnScreen();
  }
  vnc_dialog->show(true);
}

VncDialog::VncDialog(Widgets* _gw, Vnc* _vnc) : vnc(_vnc)
{
  vnc_port = "5901";
  setTitle("VNC Server");
  setMessage(uvbox(uhflex() + uvflex()
                   + uhbox(ulabel(14, UFont::bold + "Server name:")
                           + utextfield(25, vnc_server))
                   + uhbox(ulabel(14, UFont::bold + "Port number:")
                           + utextfield(vnc_port))
                   + uhbox(ulabel(14, UFont::bold + "Password:")
                           + utextfield(vnc_passwd))
                  )
            );
  setButtons(ubutton("Connect" + ucloseWin() + ucall(vnc_dialog, &VncDialog::convert))
             + " "
             + ubutton("Cancel" + ucloseWin()));
  }

void VncDialog::convert()
{
  if (vnc) vnc->convert(vnc_server.c_str(), vnc_port.c_str(), vnc_passwd.c_str());
}

