//---------------------------------------------------------------------------
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
// widgets.hpp 
//
// Ubit Widgets for the VREng GUI
//
// VREng Project
// Author: Eric Lecolinet - ENST Paris
//---------------------------------------------------------------------------
#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include "gui.hpp"
#include "ubit/ubit.hpp"	// UBox, UMenu, UElem, UDialog

using namespace ubit;


/**
 * GuiItem class
 */
struct GuiItem : public UButton {
  GuiItem(UArgs args = UArgs::none);
};

/** struct for updating the ObjectBar and the ObjectMenu in the GUI */
struct ObjInfo {
  char *name;			///< class, instance or action name
  void (*fun)(int);		///< function to call
  int  farg;			///< arg of the function
};

/** KR structure for postponing Key Releases when AutoRepeat is ON */
struct KRKey {
  static const int KRBUF_MAX=10;///< max number of postponed Key Release(s)
  int vrkey;			///< code of the corresponding VREng change key
  struct timeval time;		///< time when this key was released
};


/**
 * Widgets class
 */
class Widgets : public ubit::UFrame {
 friend class Message;

 public:
  static const int MESSAGES_BOX_WIDTH = 320;
  static const int MESSAGES_BOX_HEIGHT = 150;
  static const int GW_DEFAULT_SCROLL = 50;
  static const int GW_ZOOM_SCROLL = 35;

  Widgets(class Gui*);	///< constructor.
  ~Widgets() {};	///< destructor.

  class Navig* getNavig() { return &navig; }

  void alertDialog(const char* message);
  ///< Opens alert box if arg is not null, closes it if arg is null.

  void setInfobar(UBox* content);

  static void showInfoDialog(const char* title, const char* message);

  ////////
  // Users
  ////////

  GuiItem * addAvatar(class User *username);
  ///< Adds an avatar name.

  void updateAvatar(GuiItem *gp, class User *username);
  ///< Updates an avatar.

  void removeAvatar(GuiItem *gp);
  ///< Removes an avatar name.

  /////////
  // Worlds
  /////////

  GuiItem * addWorld(class World *worldname, bool isCurrentWorld);
  ///< Adds a world name.

  void updateWorld(class World *, bool isCurrentWorld);
  ///< Updates a world name.

  void removeWorld(class World *);
  ///< Removes a world name.

  /////////////////////
  // Callback functions
  /////////////////////

  class Object* pointedObject(int x, int y, ObjInfo* obji, int depthsel);
  /**< Returns info about the pointed object but do NOT select it */
  
  static void action(int numaction);
  /**< Applies action */

  static void setKey(int key, int is_pressed);  
  void processKey(const int keysym, int keychar, bool is_pressed);
  /**< Processes key */

  void flushPostponedKRs();
  bool pendingPostponedKRs() {return (postponedKRmask || postponedKRcount);}

 private:
  friend class Gui;
  friend class Scene;
  friend class Navig;
  friend class Panels;
  
  // Private members
  // ! BEWARE: order matters !
  class Gui &gui;      		///< reference to the GUI class
  UBox info_box;		///< info box horizontal under menubar
  UElem worlds;			///< optional worlds palette
  UElem basket;			///< optional basket palette
  UElem avatars;		///< optional avatars palette
  class Scene &scene;		///< OpenGL Drawing Zone
  class Navig &navig;		///< navigation

  UOptionDialog &source_dialog;
  UOptionDialog &objects_dialog;
  UOptionDialog &worlds_dialog;
  UOptionDialog &stats_dialog;
  UOptionDialog &mess_dialog;
  UDialog &prefs_dialog;
  UDialog &settings_dialog;
  UDialog &grid_dialog;
  UDialog &tool_dialog;
  UDialog &addobj_dialog;

  class Message &message;
  class Message2 &putinfo;
  class Panels &panels;
  UBox &infobar, &menubar;
  class Capture &capture;
  long postponedKRmask;		///< mask of all postponed Key Release events
  long postponedKRcount;	///< count in the KRbuf
  KRKey postponedKRbuf[KRKey::KRBUF_MAX]; ///< kbd bufs

  // Private methods

  // Dialogs
  void sourceDialog();
  void objectsDialog();
  void gotoDialog();
  void worldsDialog();
  void statsDialog();
  void messDialog();
  UDialog& prefsDialog();
  UDialog& settingsDialog();
  UDialog& gridDialog();
  UDialog& toolDialog();
  UDialog& addobjDialog();

  // Menus
  UMenu& fileMenu();
  UMenu& markMenu();
  UBox& createInfobar();
  UBox& createMenubar();
  void dynamicMenus(UMenubar&, const char* filename);

  // Messages
  void openMessage(UMessageEvent&);
  void putMessage(UMessageEvent&);
  void getMessage(UMessageEvent&);
  void moveMessage(UMessageEvent&);

  // Widgets Callbacks
  void prevCB();
  void nextCB();
  void homeCB();
  void cleanCB();
  void saveCB();
  void siteCB();
  void prefCB(int tool);
  void audioCB(bool on);
  void videoCB(bool on);
  void whiteboardCB(bool on);
  void modelerCB(bool on);
  void markCB();
  void newObjectCB();
  void rayDir(int x, int y);
};


/**
 * VncDialog class
 */
class VncDialog : public UOptionDialog {
 private:
  static VncDialog* vnc_dialog;
  class Vnc* vnc;	///< vnc pointer.

  UStr vnc_server;	///< vnc server string.
  UStr vnc_port;	///< vnc port string.
  UStr vnc_passwd;	///< vnc passwd string.
  
  VncDialog(class Widgets*, class Vnc*);

  void vncConvert();

 public:
  static void vncDialog(Widgets*, class Vnc*);
};

#endif
