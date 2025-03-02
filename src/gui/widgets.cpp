// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
//  widgets.cpp : Ubit widgets for the VREng GUI
//
//  VREng / Ubit Project
//  Authors: Eric Lecolinet and Philippe Dax
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "widgets.hpp"
#include "matvec.hpp"   // V3 M4
#include "scene.hpp"	// new Scene
#include "gui.hpp"	// Gui
#include "navig.hpp"	// Navig
#include "panels.hpp"	// Panels
#include "message.hpp"	// Message
#include "theme.hpp"	// Theme
#include "user.hpp"	// User localuser
#include "move.hpp"	// changeKey
#include "vnc.hpp"	// Vnc
#include "pref.hpp"	// width3Dm height3D
#include "env.hpp"	// menu
#include "http.hpp"	// httpOpen
#include "file.hpp"	// open, close
#include "cache.hpp"	// open, close
#include "universe.hpp" // Universe
#include "world.hpp"	// World
#include "render.hpp"	// g.render.bufferSelection
#include "env.hpp"	// vrengStats
#include "stat.hpp"	// openStats, getStats
#include "channel.hpp"	// current
#include "capture.hpp"	// Capture
#include "wall.hpp"	// Wall
#include "mirage.hpp"	// Mirage
#include "ball.hpp"	// Ball
#include "ground.hpp"	// Ground
#include "model.hpp"	// Model
#include "step.hpp"	// Step
#include "thing.hpp"	// Thing
#include "gate.hpp"	// Gate
#include "grid.hpp"	// gridBox
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

#include <vector>

// Text files
#include "README.h"
#include "COPYRIGHT.h"
#include "LICENSE.h"
#include "CHANGELOG.h"
#include "TODO.h"
#include "DTD.h"
//#include "CONFIG_H.h"

using namespace ubit;


/** Constructor
 * Creates all widgets needed for the GUI
 */
Widgets::Widgets(Gui* _gui) :    // !BEWARE: order matters!
 gui(*_gui),
 scene(*new Scene(this)),
 navig(*new Navig(this, scene)),
 source_dialog(*new UOptionDialog("World source")),
 objects_dialog(*new UOptionDialog("World objects")),
 worlds_dialog(*new UOptionDialog("World list")),
 stats_dialog(*new UOptionDialog("Stats")),
 mess_dialog(*new UOptionDialog("Messages")),
 prefs_dialog(prefsDialog()),
 settings_dialog(settingsDialog()),
 grid_dialog(gridDialog()),
 tool_dialog(toolDialog()),
 addobj_dialog(addobjDialog()),
 message(*new Message(this)),
 putinfo(*new Message2),
 panels(*new Panels(this, scene)),
 infobar(createInfobar()),
 menubar(createMenubar()),
 capture(*new Capture),
 postponedKRmask(0),
 postponedKRcount(0)
{
  //
  // main box
  //
  addAttr(g.theme.mainStyle);
  add(  utop()
      + menubar
      + infobar
      + uvflex()
      + uhbox(  uvflex()
              + uhflex()
              + uhspacing(0)
              + uvbox(  uhflex()
                      + uvflex()
                      + scene
                      + ubottom()
                      + panels.main_panel
                     )
              + uright()
              + panels.right_panel
             )
     );

  // right panel init
  worlds.addAttr(UOrient::vertical + utop());
  basket.addAttr(UOrient::vertical + utop());
  avatars.addAttr(UOrient::vertical + utop());

  // process ubit messages coming from other applications
  UAppli::onMessage("file",    ucall(this, &Widgets::openMessage));
  UAppli::onMessage("open",    ucall(this, &Widgets::openMessage));
  UAppli::onMessage("put",     ucall(this, &Widgets::putMessage));
  UAppli::onMessage("get",     ucall(this, &Widgets::getMessage));
  UAppli::onMessage("move",    ucall(this, &Widgets::moveMessage));
  UAppli::onMessage("back",    ucall(this, &Widgets::prevCB));
  UAppli::onMessage("forward", ucall(this, &Widgets::nextCB));
  UAppli::onMessage("home",    ucall(this, &Widgets::homeCB));
}

/** Creates Infobar = navig_box + info_box */
UBox& Widgets::createInfobar()
{
  UBox& navig_box = uhbox(  upadding(8,0)
                          + uhspacing(8)
                          + uleft()
                          + UFont::bold
                          + uitem(  g.theme.Left	//USymbol::left
                                  + utip("Prev world")
                                  + ucall(this, &Widgets::prevCB)
                                 )
                          + "   "
                          + uitem(  g.theme.Up		//USymbol::up
                                  + utip("Home world")
                                  + ucall(this, &Widgets::homeCB)
                                 )
                          + "   "
                          + uitem(  g.theme.Right	//USymbol::right
                                  + utip("Next world")
                                  + ucall(this, &Widgets::nextCB)
                                 )
                         );

  // info_box initially contains the welcome message,
  // its changed dynamically when objects are selected
  info_box.addAttr(UOrient::horizontal + uleft());
  info_box.add(  uleft()
                + UColor::white
                + UFont::bold
                + UFont::large
                + "Welcome to VREng"
               );

  return uhbox(uvspacing(2) + navig_box + info_box);
}

void Widgets::setInfobar(UBox* content)
{
  info_box.removeAll();
  if (content) info_box.add(content);
}

/**
 * Creates Menubar on top of the window
 *
 * [File] [View] [Worlds] [Tool] [Mark] [About]
 */
UBox& Widgets::createMenubar()
{
  UMenu& file_menu = fileMenu();
  file_menu.addAttr(g.theme.menuStyle);

  UMenu& view_menu =
  umenu(  g.theme.menuStyle
        + ubutton(g.theme.List  + "Source"      + ucall(this, &Widgets::sourceDialog))
        + ubutton(g.theme.List  + "Objects"     + ucall(this, &Widgets::objectsDialog))
        + ubutton(g.theme.List  + "Worlds"      + ucall(this, &Widgets::worldsDialog))
        + ubutton(g.theme.List  + "Stats"       + ucall(this, &Widgets::statsDialog))
        + ubutton(g.theme.List  + "Messages"    + ucall(this, &Widgets::messDialog))
        + ubutton(g.theme.Prefs + "Preferences" + prefs_dialog)
       );

  UMenu& worlds_menu =
  umenu(  g.theme.menuStyle
        + ubutton("Goto World"       + ucall(this, &Widgets::gotoDialog))
        + ubutton("Previous World"   + ucall(this, &Widgets::prevCB))
        + ubutton("Next World"       + ucall(this, &Widgets::nextCB))
        + ubutton("Home"             + ucall(this, &Widgets::homeCB))
        + ubutton("Visited Worlds >" + umenu(g.theme.menuStyle + worlds))
       );

  UMenu& tool_menu =
  umenu(  g.theme.menuStyle
        + ubutton(g.theme.Prefs  + " Settings "    + settings_dialog)
        + ubutton(g.theme.Grid2D + " Grid "        + grid_dialog)
        + ubutton(g.theme.Tools  + " Tools "       + tool_dialog)
        + ubutton(g.theme.AddObj + " Add objects " + addobj_dialog)
       );

  UMenu& mark_menu = markMenu();
  mark_menu.addAttr(g.theme.menuStyle);

  UMenu& about_menu =
  umenu(  g.theme.menuStyle
        + ubutton("README"    + ucall("README",    README,    &showInfoDialog))
        + ubutton("COPYRIGHT" + ucall("COPYRIGHT", COPYRIGHT, &showInfoDialog))
        + ubutton("LICENSE"   + ucall("LICENSE",   LICENSE,   &showInfoDialog))
        + ubutton("DTD"       + ucall("DTD",       DTD,       &showInfoDialog))
        + ubutton("CHANGELOG" + ucall("CHANGELOG", CHANGELOG, &showInfoDialog))
        + ubutton("TODO"      + ucall("TODO",      TODO,      &showInfoDialog))
        //+ ubutton("config.h"  + ucall("config.h",  CONFIG_H,  &showInfoDialog))
        //+ ubutton("Home Page" + ucall(this, &siteCB))
       );

  // ===== Menubar ======
  UMenubar& menu_bar = umenubar(  UFont::bold
                                + UFont::large
                                + ubutton("File"    + file_menu)
                                + ubutton("View"    + view_menu)
                                //+ ubutton("Goto"  + ucall(this, &Widgets::gotoDialog))
                                + ubutton("Worlds"  + worlds_menu)
                                + ubutton("Tool"    + tool_menu)
                                + ubutton("Mark"    + mark_menu)
                                + ubutton("About"   + about_menu)
                               );
  return menu_bar;
}

static void functionMenu(Widgets*)
{
  trace1(DBG_FORCE, "functionMenu OK");
}

/** Adds dynamic menus */
void Widgets::dynamicMenus(UMenubar& menubar, const char* filename)
{
  FILE* menufp = null;
  UMenu* dyna_menu = null;
  char attr[100], val[100];

  File *file = new File();
  if ((menufp = file->open(filename, "r"))) {
    fscanf(menufp, "%s %s", attr, val);
    while (! feof(menufp)) {
      if (! strcmp(attr, "Menu")) {
        dyna_menu = new UMenu();
        dyna_menu->addAttr(g.theme.menuStyle);
        menubar.add(ubutton(ustr(val) + dyna_menu));
      }
      else if (! strcmp(attr, "MenuItem") && dyna_menu) {
        UButton& btn = ubutton(val);
        dyna_menu->add(btn);
        if (! strcmp(val, "functionMenu")) {
          btn.add(ucall(this, functionMenu));
        }
      }
      fscanf(menufp, "%s %s", attr, val);
    }
    file->close();
    delete file;
  }
}

/** Mark menu */
UMenu& Widgets::markMenu()
{
  UBox& mark_box = uvbox();
  mark_box.add(ubutton(  UBackground::none
                       + "Add Worldmark"
                       + ucall(this, &Widgets::markCB)
                      )
              );
  UMenu& mark_menu = umenu(  g.theme.menuStyle
                           + uscrollpane(  usize().setHeight(100)
                                         + UBackground::none
                                         + mark_box
                                        )
                          );
  File *file = new File();
  FILE *fp = null;
  char line[URL_LEN + CHAN_LEN + 2];
  if ((fp = file->open(::g.env.worldmarks(), "r"))) {
    while (fgets(line, sizeof(line), fp)) {
      UStr& url = ustr();
      char *p = strchr(line, ' ');
      if (p) *p ='\0';
      url = line;
      //echo("mark: %s", url.c_str());
      mark_box.add(uitem(  url
                         + ucall(&gui, static_cast<const UStr&> (url), &Gui::openWorld)
                        )
                  );
    }
    file->close();
    delete file;
  }
  return mark_menu;
}

/** Item constructor */
GuiItem::GuiItem(UArgs a) : UButton(a)
{
  addAttr(  UBorder::none
          + UOn::enter / UBackground::blue
          + UOn::arm / UBackground::blue
         );
}

/** Sets user's infos */
static void setUser(UBox *gu, User *user)
{
  if (! user)  return;

  gu->add(  ustr(user->objectName())
          + umenu(  UFont::bold + UColor::navy
                  + uhbox(" Name: "  + UFont::plain + user->objectName())
                  + uhbox(" World: " + UFont::plain + user->name.world)
                  + uhbox(" Email: " + UFont::plain + user->email)
                  + uhbox(" Cname: " + UFont::plain + user->rtcpname)
                  + uhbox(" Vreng: " + UFont::plain + user->tool)
                  + uhbox(" Web: "   + UFont::plain + user->web)
                 )
         );
}

/** Adds an avatar item */
GuiItem *Widgets::addAvatar(User *user)
{
  if (! user)  return null;

  GuiItem *gu = new GuiItem();
  setUser(gu, user);
  avatars.add(*gu);
  return gu;
}

/** Updates an avatar item */
void Widgets::updateAvatar(GuiItem* gu, User *user)
{
  if (! user)  return;

  gu->removeAll(true);
  setUser(gu, user);
}

/** Removes an avatar item */
void Widgets::removeAvatar(GuiItem* gu)
{
  if (! gu)  return;
  avatars.remove(*gu);
}

/** Sets a world item */
static void setWorld(GuiItem* gw, World *world, bool isCurrent)
{
  if (! gw || !world)  return;

  UFont *font = isCurrent ? &UFont::bold : &UFont::plain;
  gw->add(font
          + ustr(world->name)
          + umenu(  UFont::bold
                  + UColor::navy
                  + uhbox(" Url: "     + UFont::plain + world->url)
                  + uhbox(" Channel: " + UFont::plain + world->chan)
                 )
         );
}

/** Adds a world item */
GuiItem *Widgets::addWorld(World *world, bool isCurrent)
{
  if (! world)  return null;

  GuiItem *gw = new GuiItem();
  setWorld(gw, world, isCurrent);
  worlds.add(*gw);
  return gw;
}

/** Updates a world item */
void Widgets::updateWorld(World *world, bool isCurrent)
{
  if (! world)  return;

  GuiItem *gw = world->guip;
  if (! gw)  return;
  gw->removeAll(true);	//FIXME! sometimes segfault in Ubit
  setWorld(gw, world, isCurrent);
  gw->update();
}

/** Removes a world item */
void Widgets::removeWorld(World *world)
{
  if (! world)  return;

  GuiItem *gw = world->guip;
  if (! gw)  return;
  worlds.remove(*gw);
  world->guip = NULL;
}

/** Makes an action on selected object */
static void objectActionCB(int numaction)
{
  Object* object = g.gui.getSelectedObject();
  if (object) {
    struct timeval t;
    gettimeofday(&t, NULL);
    object->specialAction(numaction, NULL, t.tv_sec, t.tv_usec);	// do action
  }
}

/** Makes an action on local user */
void Widgets::action(int numaction)
{
  if (! localuser)  return;

  struct timeval t;
  gettimeofday(&t, NULL);
  localuser->specialAction(numaction, NULL, t.tv_sec, t.tv_usec);	// do action
}

/** Returns info about the pointed object */
Object* Widgets::pointedObject(int x, int y, ObjInfo *objinfo, int z)
{
  static char *classname = 0, *currentname = 0, *actionnames = 0;

  //
  // Interaction GUI <--> 3D
  //
  uint16_t objnum = ::g.render.bufferSelection(x, y, z);	// get object's num from Z-buffer
  //echo("pointed: clic=%d %d %d objnum=%d", x, y, z, objnum);
  if (objnum == 0) {
    return NULL;
  }

  Object* object = Object::byNum(objnum);

  if (! object) {
    objinfo[0].name = const_cast<char*> ("World");	// avoid segfault
    objinfo[1].name = const_cast<char*> (World::current()->name);
    objinfo[2].name = NULL;	// NULL terminaison
    return NULL;
  }
  if (! object->isVisible()) {
    return NULL;	// invisible
  }

  // an object has been selected
  // get the object's names
  object->getObjectNames(&classname, &currentname, &actionnames);
  if (! classname) {
    return NULL;
  }

  objinfo[0].name = classname;
  if (! currentname) currentname = const_cast<char *> ("");
  objinfo[1].name = currentname;
  if (::g.pref.trace) echo("pointed: %s.%s", classname, currentname);

  // get actions of this object
  int i = 0;
  if (actionnames) {
    for (i=0; i < ACTIONSNUMBER ; i++) {
      if (*actionnames) {
        objinfo[i+2].name = actionnames;
        objinfo[i+2].fun  = objectActionCB;
        objinfo[i+2].farg = i;
        //echo("i=%d a=%s", i, actionnames);
      }
      actionnames += ACTIONNAME_LEN;	// AWFULL !!!
    }
  }
  objinfo[i+2].name = NULL; // NULL terminated

  return object;
}

/** Launches a ray */
void Widgets::setRayDirection(int x, int y)
{
  if (localuser) {
    localuser->setRayDirection(x, y);	// play the ray
  }
}


//---------------------------------------------------------------------------
//
// CALLBACKS
//

/** Home world */
void Widgets::homeCB()
{
  char chan_str[CHAN_LEN];

  sprintf(chan_str, "%s/%u/%d", Universe::current()->grpstr, Universe::current()->port, Channel::currentTtl());
  trace1(DBG_IPMC, "home: %s at %s", ::g.url, chan_str);

  World::current()->quit();
  delete Channel::current();		// delete Channel
  World::enter(::g.url, chan_str, true);
  World::current()->joinChan(chan_str); // join new channel

  if (audioactive) Audio::start(chan_str);
}

/** Previous world */
void Widgets::prevCB()
{
  World::goPrev();
}

/** Next world */
void Widgets::nextCB()
{
  World::goNext();
}

/** Cleans current world from cache */
void Widgets::cleanCB()
{
  char cachename[PATH_LEN];

  World *world = World::current();
  if (! world) return;
  sprintf(cachename, "%s/%s.vre", ::g.env.cache(), world->name);
  //echo("unlink %s", cachename);
  unlink(cachename);
}

/** Saves current world */
void Widgets::saveCB()
{
  char vrein[PATH_LEN] = {0};
  char vreout[PATH_LEN];
  char buf[BUFSIZ];

  World *world = World::current();
  if (! world) return;

  Cache::setCachePath(world->url, vrein);

  FILE *fi, *fo;
  sprintf(vreout, "%s.vre", world->name);
  File *filein = new File();
  File *fileout = new File();
  if ((fo = fileout->open(vreout, "w"))) {
    if ((fi = filein->open(vrein, "r"))) {
      while (fgets(buf, sizeof(buf), fi)) {
        fputs(buf, fo);
      }
      fileout->close();
      filein->close();
      delete fileout;
      delete filein;
      echo("world %s saved", vreout);
    }
  }
}

/** Displays web page */
void Widgets::siteCB()
{
  UStr cmd = "IFS=' '; firefox -remote 'openURL(http://"
              & UStr(::g.server)
              & "/)' &";
  system(cmd.c_str());
}

/** Preferences tools */
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

/** Starts audio tool */
void Widgets::audioCB(bool on)
{
  if (on) {
    audioactive = true;
    Audio::start(World::current()->chan);
  }
  else {
    Audio::quit();
    audioactive = false;
  }
}

/** Starts video tool */
void Widgets::videoCB(bool on)
{
  if (on) Video::start(World::current()->chan);
  else    Video::quit();
}

/** Starts whiteboard tool */
void Widgets::whiteboardCB(bool on)
{
  if (on) Wb::start(World::current()->chan);
  else    Wb::quit();
}

/** Starts modeler tool */
void Widgets::modelerCB(bool on)
{
  if (on) Modeler::start();
  else    Modeler::quit();
}

/** Marks current world */
void Widgets::markCB()
{
  FILE *fp;
  char mark[URL_LEN + CHAN_LEN + 2];
  char line[URL_LEN + CHAN_LEN + 2];

  sprintf(mark, "%s %s\n", World::current()->url, World::current()->chan);
  File *file = new File();
  if ((fp = file->open(::g.env.worldmarks(), "r"))) {
    while (fgets(line, sizeof(line), fp)) {
      if (! strcmp(line, mark)) {
        file->close();
        delete file;
        return;
      }
    }
    file->close();
    delete file;
  }
  if ((fp = file->open(::g.env.worldmarks(), "a"))) {
    fputs(mark, fp);
    file->close();
    delete file;
    markMenu();
  }
}


//---------------------------------------------------------------------------
//
//  Keys management and correspondance with VREng keys
//

void Widgets::setKey(int key, int ispressed)
{
  struct timeval t;

  gettimeofday(&t, NULL);       // get time
  changeKey(key, ispressed, t.tv_sec, t.tv_usec);
}

/**
 * Converts the X keysym into a Vreng key (vrkey) and returns
 * a keymask which is an hexa value for marking released keys in the KRmask
 */
static long convertKey(const int keysym, int keychar, int& vrkey)
{
  vrkey = 0;

  // note: here we can't use a switch because keysym (see src/ubit/ukey.hpp)
  // is not an integral constant expression, so we use if .. else if construction

  if      (keysym == UKey::Up) {       vrkey = KEY_FW; return 1<<0; }	// move forward
  else if (keysym == UKey::Down) {     vrkey = KEY_BW; return 1<<1; }	// move backward
  else if (keysym == UKey::Left) {     vrkey = KEY_LE; return 1<<2; }	// turn left
  else if (keysym == UKey::Right) {    vrkey = KEY_RI; return 1<<3; }	// turn right
  else if (keysym == UKey::PageUp) {   vrkey = KEY_UP; return 1<<11; }	// move up
  else if (keysym == UKey::PageDown) { vrkey = KEY_DO; return 1<<12; }	// move down
  else if (keysym == UKey::Insert) {   vrkey = KEY_MU; return 1<<6; }	// roll up
  else if (keysym == UKey::Delete) {   vrkey = KEY_MD; return 1<<7; }	// roll down
  else if (keysym == UKey::Home) {     vrkey = KEY_HO; return 1<<8; }	// stand up
  else if (keysym == UKey::End) {      vrkey = KEY_SP; return 1<<13; }	// accelerator
  else if (keysym == UKey::BackSpace) { Widgets::action(User::U_ASPEEDLESS); } // decrease aspeed
  else if (keysym == UKey::Tab) {       Widgets::action(User::U_ASPEEDMORE); } // increase aspeed
  else {
    switch (keychar) {
      case '<': vrkey = KEY_ML;				return 1<<4;	// left translation
      case '>': vrkey = KEY_MR;				return 1<<5;	// right translation
      case 'l': vrkey = KEY_TL;				return 1<<9;	// tilt left
      case 'r': vrkey = KEY_TR;				return 1<<10;	// tilt right
      case 'u': vrkey = KEY_UP;				return 1<<11;	// up translation
      case ' ': vrkey = KEY_SP;				return 1<<13;	// accelerator
      case '=': Widgets::action(User::U_FOVYDEF);	return 0;	// original fovy
      case '-': Widgets::action(User::U_FOVYLESS);	return 0;	// decrease fovy
      case '+': Widgets::action(User::U_FOVYMORE);	return 0;	// increase fovy
      case '.': Widgets::action(User::U_LSPEEDDEF);	return 0;	// original lspeed
      case 's': Widgets::action(User::U_LSPEEDLESS);	return 0;	// decrease lspeed
      case 'f': Widgets::action(User::U_LSPEEDMORE);	return 0;	// increase lspeed
      case ',': Widgets::action(User::U_ASPEEDDEF);	return 0;	// original aspeed
      case 'b': Widgets::action(User::U_BULLET);	return 0;	// launche bullet
      case 'd': Widgets::action(User::U_DART);		return 0;	// launche dart
      case 'v': Widgets::action(User::U_SWITCHVIEW);	return 0;	// switch view
      case 'x': Widgets::action(User::U_TPVIEWROTL);	return 0;	// rot left
      case 'c': Widgets::action(User::U_TPVIEWROTR);	return 0;	// rot right
      case 'q': Widgets::action(User::U_TPVIEWROTU);	return 0;	// rot up
      case 'w': Widgets::action(User::U_TPVIEWROTD);	return 0;	// rot down
      case 'p': Widgets::action(User::U_TPVIEWROTN);	return 0;	// rot near
      case 'm': Widgets::action(User::U_TPVIEWROTF);	return 0;	// rot far
      case 'D': Widgets::action(User::U_PITCHMORE);	return 0;	// increase pitch
      case 'U': Widgets::action(User::U_PITCHLESS);	return 0;	// decrease pitch
      case 'R': Widgets::action(User::U_ROLLMORE);	return 0;	// increase roll
      case 'L': Widgets::action(User::U_ROLLLESS);	return 0;	// decrease roll
      case '^': Widgets::action(User::U_FLYAWAY);	return 0;	// flyaway
      case '$': Widgets::action(User::U_TOLAND);	return 0;	// toland
    }
  }
  return 0;
}

/** Use this callback function for handling AUTOREPEAT Key event
 * Autorepeat is logically suppressed by annulating corresponding Press and Release events.
 * Principle: Release events are postponed and temporarily stored in KRmask
 * They are processed when coming back to the RenderingWorkProc of the
 * mainLoop if not annulated in the meantime by subsequent Press events.
 */
void Widgets::processKey(const int keysym, int keychar, bool press)
{
  int vrkey;
  long keymask = convertKey(keysym, keychar, vrkey);

  if (keymask == 0) return;		// return if null (undefined or not a vrkey)

  if (postponedKRcount < 0) {
    echo("!negative KRcount => reset"); // should never happen!
    postponedKRmask = 0;
    postponedKRcount = 0;
  }

  if (press) {
    if (postponedKRmask & keymask) {
      // le Press annule le Release correspondant dans le KRmask
      postponedKRmask &= ~keymask;	// remove keymask from KRmask
      postponedKRcount--;		// une touche en moins dans KRlist
    }
    else {	// Press
      //echo("KPress change or activate Key (%d)", vrkey);
      if (vrkey >= MAXKEYS || vrkey < 0) {
        return;
      }

      struct timeval time;
      gettimeofday(&time, NULL);
      changeKey(vrkey, press, time.tv_sec, time.tv_usec);
    }
  }
  else {	// Release
    if (postponedKRcount > KRKey::KRBUF_MAX) {
      echo("too many keys stored => flush: KRmask=%lx", postponedKRmask);
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

/** Flushes keys */
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
//
//  Dialog Boxes
//

/** Shows infos */
void Widgets::showInfoDialog(const char* title, const char* message)
{
  UDialog::showMessageDialog(title, uscrollpane(usize(450, 500) + utextarea(message)));
}

/** Displays preferences */
UDialog& Widgets::prefsDialog()
{
  UBox& settings_box = uvbox(UBackground::white + upadding(2,2));

  UDialog* prefs_dial =
  new UOptionDialog("Preferences",
                     uvbox(  ulabel("Preferences: "
                           + UColor::red
                           + ::g.env.prefs())
                           + uvflex()
                           + uscrollpane(usize(400, 300) + settings_box)
                         ),
                    UArgs::none,
                    UArgs::none
                  );

  FILE *fp;
  char line[128];

  File *file = new File();
  if ((fp = file->open(::g.env.prefs(), "r"))) {
    while (fgets(line, sizeof(line), fp)) {
      if (isalpha(*line)) settings_box.add(uitem(UColor::red + UFont::bold + line));
      else                settings_box.add(uitem(UColor::black + line));
    }
    file->close();
    delete file;
  }
  return *prefs_dial;
}

/** Displays vre source */
static void sourceHttpReader(void *box, Http *http)
{
  if (! http) return;

  UBox *source_box = static_cast<UBox *> (box);
  char line[256];

  source_box->setAutoUpdate(false);

  Cache *cache = new Cache();
  FILE *fp = cache->open(http->url, http);
  while (fgets(line, sizeof(line), fp)) {
    source_box->add(uitem(UColor::black + line));
  }
  cache->close();
  delete cache;
  source_box->setAutoUpdate(true);
  source_box->update();
}

/** Displays list of worlds */
static void gotoHttpReader(void *box, Http *http)
{
  if (! http) return;

  UBox *worlds_box = static_cast<UBox *> (box);
  char line[URL_LEN + CHAN_LEN +2];

  Cache *cache = new Cache();
  FILE *fp = cache->open(http->url, http);
  while (cache->nextLine(fp, line)) {
    if (*line == '#') continue;		// commented line

    UStr& url = ustr();
    char tmpline[URL_LEN + CHAN_LEN +2];

    strcpy(tmpline, line);
    char *p = strchr(tmpline, ' ');
    if (p) *p = '\0';
    else {
      p = strchr(tmpline, '\t');
      if (p) *p = '\0';
    }
    char tmpname[URL_LEN + 2];
    strcpy(tmpname, tmpline);
    char *worldname = strrchr(tmpname, '/');
    if (worldname) {
      *worldname++ = '\0';
      p = strchr(worldname, '.');
      if (p) *p = '\0';
      url = tmpline;
      //echo("goto: %s", url.c_str());

      worlds_box->add(  UBackground::white
                      + uitem(  UColor::navy
                              + UFont::bold
                              + UFont::large
                              + worldname
                              + ucall(&g.gui, static_cast<const UStr&> (url), &Gui::openWorld)
                             )
                     );
    }
  }
  cache->close();
  delete cache;
}

/** Displays list of worlds and their urls */
static void worldsHttpReader(void *box, Http *http)
{
  if (! http) return;

  UBox *worlds_box = static_cast<UBox *> (box);
  char line[URL_LEN + CHAN_LEN +2];

  Cache *cache = new Cache();
  FILE *fp = cache->open(http->url, http);
  while (cache->nextLine(fp, line)) {
    if (*line == '#') continue;		// commented line

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

    worlds_box->add(uitem(  UColor::navy
                          + UFont::bold
                          + url
                          + " "
                          + UFont::plain
                          + chan
                          + ucall(&g.gui, static_cast<const UStr&> (url), &Gui::openWorld)
                         )
                  );
  }
  cache->close();
  delete cache;
}

/** Dialog box to display vre source */
void Widgets::sourceDialog()
{
  World *world = World::current();
  if (! world) return;

  UBox& source_box = uvbox(UBackground::white);
  if (Http::httpOpen(world->url, sourceHttpReader, &source_box, 0) < 0) { // -1: no cache
    delete &source_box;
    return;
  }
  source_dialog.setMessage(uscrollpane(  usize(450, 350)
                                       + UBackground::white
                                       + source_box
                                      )
                          );
  source_dialog.show();
}

/** Dialog box to display present objects in the current world */
void Widgets::objectsDialog()
{
  char line[64];

  UBox& objects_box = uvbox(UBackground::none);
  for (std::vector<Object*>::iterator it = objectList.begin(); it != objectList.end(); ++it) {
    sprintf(line, "%s:%s", (*it)->typeName(), (*it)->objectName());
    objects_box.add(uitem(UColor::black + line));
  }
  objects_dialog.setMessage(uscrollpane(  usize(150, 350)
                                        + UBackground::none
                                        + objects_box
                                       )
                           );
  objects_dialog.show();
}

/** Dialog box to display current stats */
void Widgets::statsDialog()
{
  FILE *fin = NULL;
  char line[64];

  writeStats(::g.env.stats());
  UBox& stats_box = uvbox(UBackground::none);
  fin = openStats(::g.env.stats());
  while (getStats(fin, line)) {
    stats_box.add(uitem(UColor::black + line));
  }
  stats_dialog.setMessage(uscrollpane(  usize(250, 350)
                                      + UBackground::none
                                      + stats_box
                                     )
                         );
  stats_dialog.show();
}

/** Dialog box for messages */
void Widgets::messDialog()
{
  UBox& mess_box = uvbox(UBackground::none);
#if 0 //dax TODO
  char line[128];

  for (std::vector<UStr>::iterator it = Message::mess_history.begin(); it != Message::mess_history.end(); ++it) {
    strcpy(line, (*it).c_str());
    echo("mess: %s", line);
    mess_box.add(uitem(UColor::black + line));
  }
#endif
  mess_dialog.setMessage(uscrollpane(  usize(300, 200)
                                     + UBackground::none
                                     + mess_box
                                    )
                        );
  mess_dialog.show();
}

/** Dialog box to display worlds list */
void Widgets::gotoDialog()
{
  char worlds_url[URL_LEN];

  sprintf(worlds_url, "http://%s/%s%s", ::g.server, ::g.urlpfx, DEF_URL_WORLDS);

  UBox& goto_box = uvbox(g.theme.scrollpaneStyle);
  if (Http::httpOpen(worlds_url, gotoHttpReader, &goto_box, 0) < 0) {
    delete &goto_box;
    return;
  }

  worlds_dialog.setMessage(uscrollpane(usize(120, 400) + goto_box));
  worlds_dialog.show();
}

/** Dialog box for worlds list and their urls */
void Widgets::worldsDialog()
{
  char worlds_url[URL_LEN];

  sprintf(worlds_url, "http://%s/%s%s", ::g.server, ::g.urlpfx, DEF_URL_WORLDS);

  UBox* worlds_box = new UTextarea;
  if (Http::httpOpen(worlds_url, worldsHttpReader, worlds_box, 0) < 0) {
    delete worlds_box;
    return;
  }
  worlds_dialog.setMessage(uscrollpane(usize(250, 350) + worlds_box));
  worlds_dialog.show();
}

/** Sets buttons for Tools */
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
   uhbox(" Audio live        : " + UFont::plain
         + ucheckbox("Rat" + sel_audio_live
                     + UOn::select / ucall(this, RAT_TOOL, &Widgets::prefCB))
                     .setSelected()
         + ucheckbox("Vat" + sel_audio_live
                     + UOn::select / ucall(this, VAT_TOOL, &Widgets::prefCB))
         + ucheckbox("Fphone" + sel_audio_live
                     + UOn::select / ucall(this, FPHONE_TOOL, &Widgets::prefCB))
        )
   + uhbox(UBorder::shadowOut)
   + uhbox(" Video live      : " + UFont::plain
           + ucheckbox("Vic" + sel_video_live
                       + UOn::select / ucall( this, VIC_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Vlc" + sel_video_live
                       + UOn::select / ucall(this, VLCMC_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox(" Whiteboard      : " + UFont::plain
           + ucheckbox("Wb" + sel_wb
                       + UOn::select / ucall(this, WB_TOOL, &Widgets::prefCB))
                       .setSelected()
           + ucheckbox("Wbd" + sel_wb
                       + UOn::select / ucall(this, WBD_TOOL, &Widgets::prefCB))
           + ucheckbox("Nte" + sel_wb
                       + UOn::select / ucall(this, NTE_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox(" Web browser     : " + UFont::plain
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
                       + UOn::select / ucall(this, EDGE_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox(" Audio streaming : " + UFont::plain
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
   + uhbox(" Video streaming : " + UFont::plain
           + ucheckbox("Vlc" + sel_video_streaming
                       + UOn::select / ucall( this, VLC_TOOL, &Widgets::prefCB)
                      ).setSelected()
           + ucheckbox("Mpegplay" + sel_video_streaming
                       + UOn::select / ucall(this, MPEGPLAY_TOOL, &Widgets::prefCB))
           + ucheckbox("Quicktime" + sel_video_streaming
                       + UOn::select / ucall(this, QUICKTIME_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox(" Modeler         : " + UFont::plain
           + ucheckbox("Vred" + sel_modeler
                       + UOn::select / ucall(this, VRED_TOOL, &Widgets::prefCB)
                      ).setSelected()
           + ucheckbox("Vrem" + sel_modeler
                       + UOn::select / ucall(this, VREM_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox(" PsPdf           : " + UFont::plain
           + ucheckbox("Evince" + sel_pspdf
                       + UOn::select / ucall(this, EVINCE_TOOL, &Widgets::prefCB)
                      ).setSelected()
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
   + uhbox(" Office          : " + UFont::plain
           + ucheckbox("LibreOffice" + sel_office
                       + UOn::select / ucall(this, LIBROFFICE_TOOL, &Widgets::prefCB)
                      ).setSelected()
           + ucheckbox("MsOffice" + sel_office
                       + UOn::select / ucall(this, MSOFFICE_TOOL, &Widgets::prefCB))
           + ucheckbox("OpenOffice" + sel_office
                       + UOn::select / ucall(this, OPENOFFICE_TOOL, &Widgets::prefCB))
           + ucheckbox("StarOffice" + sel_office
                       + UOn::select / ucall(this, STAROFFICE_TOOL, &Widgets::prefCB))
          )
   + uhbox(UBorder::shadowOut)
   + uhbox(" Session         : " + UFont::plain
           + ucheckbox("Ssh" + sel_session
                       + UOn::select / ucall(this, SSH_TOOL, &Widgets::prefCB)
                      ).setSelected()
           + ucheckbox("Telnet" + sel_session
                       + UOn::select / ucall(this, TELNET_TOOL, &Widgets::prefCB)
                      )
          )
   + uhbox(UBorder::shadowOut)
   + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
  );
}

/** Dialog box for tools */
UDialog& Widgets::toolDialog()
{
  return udialog(  ubutton("Audio"      + ucall(this, true, &Widgets::audioCB))
                 + ubutton("Video"      + ucall(this, true, &Widgets::videoCB))
                 + ubutton("Whiteboard" + ucall(this, true, &Widgets::whiteboardCB))
                 + ubutton("Modeler"    + ucall(this, true, &Widgets::modelerCB))
                 + uhbox(UBorder::shadowIn)
                 + ubutton(UFont::bold + uhcenter() + " Close " + ucloseWin())
                );
}

/** Dialog box for grid marks */
UDialog& Widgets::gridDialog()
{
  return udialog(Grid::grid()->gridBox());
}


VncDialog* VncDialog::vnc_dialog = null;

/** Dialog box for VNC */
void VncDialog::vncDialog(Widgets* gw, Vnc* vnc)
{
  if (! gw)  return;

  if (! vnc_dialog) {
    vnc_dialog = new VncDialog(gw, vnc);
    gw->add(vnc_dialog);
    vnc_dialog->centerOnScreen();
  }
  vnc_dialog->show(true);
}

/** Dialog box for VNC */
VncDialog::VncDialog(Widgets* _gw, Vnc* _vnc) : vnc(_vnc)
{
  vnc_server = DEF_VNC_SERVER;
  vnc_port = "5900";
  setTitle("VNC Server");
  setMessage(uvbox(uhflex() + uvflex()
                   + uhbox(ulabel(14, UFont::bold + "Server name:") + utextfield(25, vnc_server))
                   + uhbox(ulabel(14, UFont::bold + "Port number:") + utextfield(vnc_port))
                   + uhbox(ulabel(14, UFont::bold + "Password:")    + utextfield(vnc_passwd))
                  )
            );
  setButtons(ubutton("Connect" + ucloseWin() + ucall(vnc_dialog, &VncDialog::vncConvert))
             + " "
             + ubutton("Cancel" + ucloseWin())
            );
}

void VncDialog::vncConvert()
{
  if (vnc) {
    vnc->convert(vnc_server.c_str(), vnc_port.c_str(), vnc_passwd.c_str());
  }
}


//---------------------------------------------------------------------------
//
// Addobj
//
// Dynamicaly objects creation
//

/** items */
enum {
  NONE,
  WALL, THING, MIRAGE, BALL, STEP, GROUND, GATE,	// objects
  BOX, SPHERE, CONE, CYLINDER, DISK, TORUS,		// shapes
  BLACK, RED, GREEN, BLUE, PURPLE, YELLOW, CYAN, WHITE,	// colors
  TINY, SMALL, MEDIUM, BIG, BIGEST,			// sizes
  OPAQUE, OPAQUE8, OPAQUE6, OPAQUE4, OPAQUE2, OPAQUE0,	// transparencies
  WOOD, MARBLE, BRICK, STUC, GRASS, PAPER, WATER, CLOUD,// textures
  MODEL, MAN, CAR, SHRUB, TREE, PENGUIN,		// models
  CHAIR_WOOD, TABLE_WOOD, TABLE_METAL, TABLE_GLASS,	// compound objects
  ENDOBJ
};

// Local
static uint8_t objtype = THING;	// thing (for multiple solids)
static char shape[16] = "box";	// box
static char texture[128] = {0};	// empty
static V3 color;		// white
static float alpha = 1;		// opaque
static float siz = .5;		// medium

static const char chair_wood[] = "\
<solid dim=\".25 .25 .01\" dif=\".5 .3 .1\" tx=\"/gif/wood.gif\" />\n\
<solid dim=\".02 .25 .25\" rel=\".12 0 .12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.12 .12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".12 .12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\"-.12 -.12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
<solid dim=\".01 .01 .25\" rel=\".12 -.12 -.12 0 0\" dif=\".5 .3 .1\" />\n\
";
static const char table_wood[] = "\
<solid dim=\".40 .80 .02\" dif=\".5 .3 .1\" tx=\"/gif/blondwood.gif\" />\n\
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

/** default values */
static void defaultAddobj()
{
  objtype = THING;	// thing (default for multiple solids)
  strcpy(shape, "box");	// box
  *texture = '\0';	// no textures
  alpha = 1;		// opaque
  siz = .5;		// 50 cm
  color = setV3(1,1,1);	// white
}

/** set values */
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

    // alpha
    case OPAQUE  : alpha = 1.; break;
    case OPAQUE8 : alpha = .8; break;
    case OPAQUE6 : alpha = .6; break;
    case OPAQUE4 : alpha = .4; break;
    case OPAQUE2 : alpha = .2; break;
    case OPAQUE0 : alpha = .0; break;

    // sizes
    case TINY :   siz = .12; break;
    case SMALL :  siz = .25; break;
    case MEDIUM : siz = .50; break;
    case BIG :    siz = 1.0; break;
    case BIGEST : siz = 2.0; break;

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

/** callback witch builds <solid ... \> */
void Widgets::newObjectCB()
{
  char solid[BUFSIZ], url[128];
  float r = .2 * siz;
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

  if (*solid == '\0') {
    printf("solid is empty\n");
    defaultAddobj();	// reset to default values
    return;
  }

  // Calls constructor of the selected object
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

/** Dialog box for addobj */
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

  UBox* addobj_box = new UBox
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
                      + ucall(this, &Widgets::newObjectCB) + ucloseWin())
            + ubutton(UFont::bold + uhcenter()
                      + " Cancel " + ucloseWin())
           )
    );	// end addobj_box

  defaultAddobj();
  return udialog(addobj_box);
}


//---------------------------------------------------------------------------

UMenu& Widgets::fileMenu()
{
  // Open a vreng Url
  UStr& url = ustr();

  UDialog* openvre_dialog =  // args are: title, content, icon, buttons
  new UOptionDialog("Open Vreng Url",
                     ulabel(UFont::bold + "Url or Name: ")
                     + uhflex()
                     + utextfield(80, url),
                     UArgs::none,
                     uhbox(  ubutton("  Open  "
                           + ucall(&gui, static_cast<const UStr&> (url), &Gui::openWorld))
                           + ubutton("  Cancel  " + ucloseWin())
                          )
                   );

  // Put & Publish Url
  UBox& puturl_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(20, UFont::bold + "Url" + UFont::plain + " (required)")
                + uhflex()
                + utextfield(65, putinfo.url)
               )
        + uhbox(ulabel(20, UFont::bold + "Alias" + UFont::plain + " (short name)")
                + uhflex()
                + utextfield(65,
                putinfo.name)
               )
        + uhbox(ulabel(20, UFont::bold + "Icon" + UFont::plain + " (optional)")
                + uhflex()
                + utextfield(65, putinfo.icon)
               )
       );

  UDialog* puturl_dialog =
  new UOptionDialog("Put new Url document",	//title
                    puturl_box,			// message
                    UArgs::none,		// no icon
                    ubutton("  Put & Publish Url  "
                             + ucall(&putinfo,&Message2::putIconCB))); //buttons

  // Put & Publish File
  UBox& putfile_box =
  uvbox(uvspacing(5)
        + uhbox(ulabel(25, UFont::bold + "Input File" + UFont::plain + " (required)")
                + uhflex()
                + utextfield(65, putinfo.file)
               )
        + uhbox(ulabel(25, UFont::bold +"Output File" + UFont::plain + " (public location)")
                + uhflex()
                + utextfield(65, putinfo.ofile)
               )
        + uhbox(ulabel(25, UFont::bold + "Alias" + UFont::plain + " (short name)")
                + uhflex()
                + utextfield(65, putinfo.name)
               )
       );

  UDialog* putfile_dialog =
  new UOptionDialog("Put new File document",  	// title
                    putfile_box,		// message
                    UArgs::none,		// no icon
                    ubutton("  Put & Publish File  "
                             + ucall(&putinfo, &Message2::putIconCB)));

  // Create File menu
  return umenu(  ubutton(g.theme.World  + " Open Vreng Url..." + openvre_dialog)
               + ubutton(g.theme.Save   + " Save Vreng File"   + ucall(this, &Widgets::saveCB))
               + ubutton(g.theme.UpDown + " Clean from Cache"  + ucall(this, &Widgets::cleanCB))
               + usepar()
               + ubutton(g.theme.Doc    + " Put & Publish Url..."  + puturl_dialog)
               + ubutton(g.theme.Book   + " Put & Publish File..." + putfile_dialog)
               + usepar()
               + ubutton(g.theme.Exit   + " Quit" + ucall(0/*status*/, Vreng::quit))
              );
}


//---------------------------------------------------------------------------

void Widgets::putMessage(UMessageEvent &e)
{
  const UStr* arg = e.getMessage();

  if (! arg || arg->empty())  return;

  UStr fname = arg->basename();
  UStr val;

  if (fname.empty()) val = "<url=\"" & *arg & "\">";
  else               val = "<url=\"" & *arg & "\">&<name=\"" & fname & "\">";
  if (! Cache::check(arg->c_str()))
    return;    // bad url
  putinfo.putIcon(val);
}

void Widgets::openMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();

  if (! msg || msg->empty())  return;

  gui.openWorld(*msg);
}

void Widgets::moveMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();

  if (! msg || msg->empty())  return;

  const UStr& arg = *msg;

  if      (arg == "left 1")       setKey(KEY_ML, true);
  else if (arg == "left 0")       setKey(KEY_ML, false);
  else if (arg == "right 1")      setKey(KEY_MR, true);
  else if (arg == "right 0")      setKey(KEY_MR, false);
  else if (arg == "forward 1")    setKey(KEY_FW, true);
  else if (arg == "forward 0")    setKey(KEY_FW, false);
  else if (arg == "backward 1")   setKey(KEY_BW, true);
  else if (arg == "backward 0")   setKey(KEY_BW, false);
  else if (arg == "up 1")         setKey(KEY_UP, true);
  else if (arg == "up 0")         setKey(KEY_UP, false);
  else if (arg == "down 1")       setKey(KEY_DO, true);
  else if (arg == "down 0")       setKey(KEY_DO, false);
  else if (arg == "turn_left 1")  setKey(KEY_LE, true);
  else if (arg == "turn_left 0")  setKey(KEY_LE, false);
  else if (arg == "turn_right 1") setKey(KEY_RI, true);
  else if (arg == "turn_right 0") setKey(KEY_RI, false);
}

void Widgets::getMessage(UMessageEvent &e)
{
  const UStr* msg = e.getMessage();

  if (! msg || msg->empty())  return;

  // a completer
  //cerr << "get: " << *selected_object_url << endl;
}
