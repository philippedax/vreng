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

#include "pref.hpp"	// width3Dm height3D
#include "env.hpp"	// menu
#include "universe.hpp" // Universe
#include "world.hpp"	// World
#include "user.hpp"	// localuser
#include "cart.hpp"	// cart::DROP
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


GuiWidgets::GuiWidgets(Gui* _gui) :    // !BEWARE: order matters!
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
  UAppli::onMessage("file",   ucall(this, &GuiWidgets::openMessage));
  UAppli::onMessage("open",   ucall(this, &GuiWidgets::openMessage));
  UAppli::onMessage("put",    ucall(this, &GuiWidgets::putMessage));
  UAppli::onMessage("get",    ucall(this, &GuiWidgets::getMessage));
  UAppli::onMessage("move",   ucall(this, &GuiWidgets::moveMessage));
  UAppli::onMessage("back",   ucall(this, &GuiWidgets::backCB));
  UAppli::onMessage("forward",ucall(this, &GuiWidgets::forwardCB));
  UAppli::onMessage("home",   ucall(this, &GuiWidgets::homeCB));
}


UBox& GuiWidgets::createInfobar()
{
  UBox& navigbox =
  uhbox(upadding(8,0) + uhspacing(8) + uleft() + UFont::bold
        + uitem(USymbol::left + UBackground::green  + utip("Prev world") + ucall(this, &GuiWidgets::backCB))
        + uitem(USymbol::up   + UBackground::green  + utip("Home world") + ucall(this, &GuiWidgets::homeCB))
        + uitem(USymbol::right + UBackground::green + utip("Next world") + ucall(this, &GuiWidgets::forwardCB))
        + usepar()
        );
  // infos initially contains the welcome message, its changed dynamically when objects are selected
  infos.addAttr(UOrient::horizontal + uleft());
  infos.add(uleft() + UPix::ray + UColor::red + UFont::bold + " Welcome to VREng " + UPix::ray);
  return uhbox(uvspacing(2) + navigbox + infos);
}

void GuiWidgets::setInfobar(UBox* content)
{
  infos.removeAll();
  if (content) infos.add(content);
}


UBox& GuiWidgets::createMenubar()
{
  UMenu& file_menu = createFileMenu();
  file_menu.addAttr(g.theme.menuStyle);

  UMenu& view_menu =
  umenu(g.theme.menuStyle
        + ubutton(g.theme.Edit  + "Source"      + ucall(this, &GuiWidgets::openSourceDialog))
        + ubutton(g.theme.List  + "Worlds"      + ucall(this, &GuiWidgets::openWorldsDialog))
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
        + ubutton("Previous World"   + ucall(this, &GuiWidgets::backCB))
        + ubutton("Next World"       + ucall(this, &GuiWidgets::forwardCB))
        + ubutton("Home"             + ucall(this, &GuiWidgets::homeCB))
        + ubutton("Visited Worlds >" + umenu(g.theme.menuStyle + worlds))
        );

  UMenu& help_menu =
  umenu(g.theme.menuStyle
        + ubutton("Home Page" + ucall(this, &GuiWidgets::helpCB))
        + ubutton("README"    + ucall("README", README, &GuiWidgets::showInfoDialog))
        + ubutton("ChangeLog" + ucall("ChangeLog", ChangeLog, &GuiWidgets::showInfoDialog))
        + ubutton("config.h"  + ucall("config.h", CONFIG_H, &GuiWidgets::showInfoDialog))
        + ubutton("DTD"       + ucall("DTD", DTD, &GuiWidgets::showInfoDialog))
        + ubutton("TODO"      + ucall("TODO", TODO, &GuiWidgets::showInfoDialog))
        + ubutton("COPYRIGHT" + ucall("COPYRIGHT", COPYRIGHT, &GuiWidgets::showInfoDialog))
        + ubutton("LICENSE"   + ucall("LICENSE", COPYING, &GuiWidgets::showInfoDialog))
        );

  // ===== Menubar ======
  UMenubar& menu_bar =
  umenubar(ubutton("File"    + file_menu)
         + ubutton("View"    + view_menu)
         + ubutton("Go"      + ucall(this, &GuiWidgets::destinationsDialog))
         + ubutton("Tools"   + tool_menu)
         + ubutton("History" + history_menu)
          );

  menu_bar.add(ubutton("Marks" + createMarkMenu()));
  addDynamicMenus(menu_bar, ::g.env.menu());
  menu_bar.add(ubutton("Help" + help_menu));
  return menu_bar;
}


static void functionMenu(GuiWidgets*)
{
  trace(DBG_FORCE, "functionMenu OK");
}

void GuiWidgets::addDynamicMenus(UMenubar& menu_bar, const char* filename)
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
UMenu& GuiWidgets::createMarkMenu()
{
  UBox& mark_box = uvbox();
  mark_box.add(ubutton(UBackground::none + "Add Worldmark"
                       + ucall(this, &GuiWidgets::addMarkCB)));
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
      // Je ne comprends pas pourquoi le callback ci-dessous n'est jamais appelé
      mark_box.add(uitem(buf) + ucall(&gui, (const UStr&)buf, &Gui::gotoWorld));
    }
    File::closeFile(fp);
  }
  return mark_menu;
}

//UMenu* GuiWidgets::getOpenedMenu() {return navig.opened_menu;}
void GuiWidgets::showInfoDialog(const char* title, const char* message)
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

GuiItem *GuiWidgets::addUser(User *user)
{
  if (! user)  return null;
  GuiItem *gu = new GuiItem();
  setUser(gu, user);
  avatars.add(*gu);
  return gu;
}

void GuiWidgets::updateUser(GuiItem* gu, User *user)
{
  if (! user)  return;
  gu->removeAll(true);
  setUser(gu, user);
}

void GuiWidgets::removeUser(GuiItem* gu)
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

GuiItem *GuiWidgets::addWorld(World *world, bool isCurrent)
{
  if (! world)  return null;
  GuiItem *gw = new GuiItem();
  setWorld(gw, world, isCurrent);
  worlds.add(*gw);
  return gw;
}

void GuiWidgets::updateWorld(World *world, bool isCurrent)
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

void GuiWidgets::removeWorld(World *world)
{
  if (! world)  return;
  GuiItem *gw = world->getGui();
  if (! gw)  return;
  worlds.remove(*gw);
  world->resetGui();
}

// CALLBACKS

void GuiWidgets::homeCB()
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

void GuiWidgets::backCB()
{
  World::goBack();
}

void GuiWidgets::forwardCB()
{
  World::goForward();
}

void GuiWidgets::saveCB()
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

void GuiWidgets::helpCB()
{
  UStr cmd = "IFS=' '; firefox -remote 'openURL(http://"
              & UStr(Universe::current()->name)
              & "/)' &";
  system(cmd.c_str());
}

void GuiWidgets::setPrefCB(int tool)
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

void GuiWidgets::setAudioCB(bool on)
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

void GuiWidgets::setVideoCB(bool on)
{
  if (on) Video::start(World::current()->getChan());
  else    Video::quit();
}

void GuiWidgets::setWhiteboardCB(bool on)
{
  if (on) Wb::start(World::current()->getChan());
  else    Wb::quit();
}

void GuiWidgets::setModelerCB(bool on)
{
  if (on) Modeler::start();
  else    Modeler::quit();
}

void GuiWidgets::addMarkCB()
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

void GuiWidgets::setKey(int key, int ispressed)
{
  struct timeval t;
  gettimeofday(&t, NULL);       // get time
  changeKey(key, ispressed, t.tv_sec, t.tv_usec);
}

void GuiWidgets::callAction(int numaction)
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
WObject* GuiWidgets::getPointedObject(int x, int y, ObjInfo *objinfo, int z)
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

void GuiWidgets::setRayDirection(int x, int y)
{
  if (! localuser)  return;
  localuser->setRayDirection(x, y);
}
