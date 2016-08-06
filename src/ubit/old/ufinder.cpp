/************************************************************************
 *
 *  ufinder.cpp: UFinder element
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

#include <ubit/ubit_features.h>
#include <cmath>
#include <unistd.h>
#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/uctlmenu.hpp>
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/udoc.hpp>
#include <ubit/upix.hpp>
#include <ubit/usymbol.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uicon.hpp>
#include <ubit/uappli.hpp>
#include <ubit/utimer.hpp>
#include <ubit/ufilebox.hpp>
#include <ubit/ufinder.hpp>
#include <ubit/ufinderImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


static const char *doc_xpm[] = {       // moins large que UPix::doc
  "13 16 4 1",
  "       c None s None",
  ".      c black",
  "X      c white",
  "o      c #808080",
  "             ",
  " .......     ",
  " .XXXXX..    ",
  " .XoooX.X.   ",
  " .XXXXX....  ",
  " .XooooXoo.o ",
  " .XXXXXXXX.o ",
  " .XooooooX.o ",
  " .XXXXXXXX.o ",
  " .XooooooX.o ",
  " .XXXXXXXX.o ",
  " .XooooooX.o ",
  " .XXXXXXXX.o ",
  " ..........o ",
  "  oooooooooo ",
  "             "
};
UPix& UFinder::doc_pix = *new UPix(doc_xpm, UObject::UCONST);

/* ==================================================== ======== ======= */

UFinder::Options::Options() :
  clone_frame_size(600,450),
  default_background(UColor::white),
  default_color(UColor::black),
  //controls_width(100),
  //controls_folder_height(125),
  show_icon_images(true) {
}

/* ==================================================== (c)[Elc] ======= */

void UFinder::setTracking(bool state) {
  setTracking(state, state);
}

void UFinder::setTracking(bool doc_track, bool icon_track) {
  is_tracking = icon_track;
  pdocbox->scrollpane().setTracking(doc_track);
  if (piconbox) piconbox->scrollpane().setTracking(icon_track);
}

//void UFinder::showDocGlass(bool state) {docglass->show(state);}

void UFinder::showAlert(const UStr& msg) {alertbox->showMsg(msg);}

void UFinder::hideAlert() {alertbox->show(false);}

void UFinder::setListener(UFinderListener* ll) {
  listener = ll;
}

UFinderListener* UFinder::getListener() {
  return listener;
}

/* ==================================================== (c)[Elc] ======= */

UFinder::~UFinder() {
delete local_ums;
}

UFinder::UFinder(const UStr& _pathname) :
mode(NoMode),
is_tracking(false), 
open_in_fullwin(false),
//iconbox_panel(new UVbox()),
listener(new UFinderListener()),
alertbox(new UAlertbox()),
ctlmenu(&createContextMenu()),
local_ums(null),
remote_ums(null),
pdocument(null),
pdocbox(new UDocbox()),
piconbox(null),
//docglass(new UBox()),
last_direntry(null),
last_preview_request(null),
last_preview(null),
preview_timer(new UTimer(false)) 
{
  preview_timer->onAction(ucall(this, &UFinder::showIconPreviews));
  UAppli::onMessage("next",    ucall(this, &UFinder::nextEntry));
  UAppli::onMessage("previous",ucall(this, &UFinder::previousEntry));
  UAppli::onMessage("parent",  ucall(this, &UFinder::openParent));
  UAppli::onMessage("file",    ucall(this, &UFinder::openMsg));
  UAppli::onMessage("open",    ucall(this, &UFinder::openMsg));
  UAppli::onMessage("clone",   ucall(this, &UFinder::createCloneRequest));
  //UAppli::onMessage("put",     ucall(this, &UFinder::sendFileMessage)); 
  /*
  docglass->addAttr(UBackground::none);
  docglass->addAttr(upos(0,0));
  docglass->addAttr(usize(50000,100000)); // a revoir
  docpane->content().add(*docglass);
  docglass->show(false);
   */
  openContextMenuIn(*pdocbox);
  
  alertbox->addAttr(upos(0,0) + usize(100|UPERCENT,100|UPERCENT));
  alertbox->show(false);

  ask_dialog_msg = new UStr(); 
  ask_dialog = new UDialog
  (uhcenter() + uvflex()
   + upadding(15,20) + uvspacing(20)
   + UFont::bold + UFont::large
   + uhbox(UColor::orange + "Remote Request")
   + uhbox(UColor::navy + ask_dialog_msg)
   + uhcenter()
   + uhbox(ubutton("  OK " + ucall(1, &UElem::closeWin))
           + ubutton(" Cancel " + ucall(0, &UElem::closeWin)))
   );

  // doit toujours rester une hardwin, meme en mode -group et
  // etre afficher sur le display initial (= celui du maitre)
  ask_dialog->setSoftwin(false);

  //===========================================

  pdocbox->titlebar().show(false);

  initOptbox();
  optbox.show(false);

  toolbar.addAttr(UBackground::metal + UFont::xx_large + UFont::bold);
  toolbar
  .add(*optbox_btn
       + ubutton(uhcenter() + UPix::bigUp + ucall(this, &UFinder::openParent))
       + ubutton(uhcenter() + UPix::bigLeft + ucall(this, &UFinder::previousEntry))
       + ubutton(UPix::bigRight + ucall(this, &UFinder::nextEntry))
       + " " 
       + ubutton(UFont::bold + "  Z  " + ucall(this, &UFinder::zoomIn))
       + ubutton(UFont::bold + "  z  " + ucall(this, &UFinder::zoomOut))
       + ubutton(UFont::bold + "  =  " + ucall(this, 1.0f, &UFinder::zoom))
       );
  
  mainbox.addAttr(UOrient::vertical + uvflex() + uvflex());
  
  addAttr(UBackground::white); 
  add(UOrient::vertical 
      + utop() + toolbar
      + uvflex()
      + uhbox(uvflex() + uleft() + optbox + uhflex() + mainbox)
      + ctlmenu
      + ask_dialog
      + alertbox
     );

  // rajouter HOME dans l'historique
  const char* pwd = ::getenv("HOME");  // pas de free() sur un getenv!
  if (pwd) open(pwd);

  // ouvrir le dir donne en arg (celui ou on est par defaut)
  open(_pathname);
  
  addHost("localhost");
}

/* ==================================================== [(c)Elc] ======= */

void UFinder::showSideBar(bool state) {
  optbox_btn->setSelected(state);
}

void UFinder::initOptbox() {
  optbox_btn = 
  ubutton(USymbol::square 
          + UOn::select   / ushow(optbox, true)
          + UOn::deselect / ushow(optbox, false)
          );
  optbox.addAttr(UOrient::vertical + UBackground::velin
                 + usize(120,UAUTO) + upadding(6,9).setRight(2) + utop());

  // - - - HISTORY - - - -
  
  folderlist.addAttr(UOrient::vertical + utop() + uchoice());
  UBox& folders = uscrollpane(true, false, folderlist);
  folders.addAttr(usize(UAUTO,100) + upadding().setLeft(15)); 
  folders.show(false);

  // opens/closes history
  optbox.add(uhbox(UOn::select / ushow(folders, true)
                   + UOn::deselect / ushow(folders, false)
                   + UOn::select / USymbol::down
                   + UOn::deselect / USymbol::right
                   + UFont::bold + "History")
             + folders
             + ubox(usize(UAUTO,6))   // separator
             );
  
  // - - - HOSTS - - - -
 
  UStr* host_str = new UStr;
  UMenu& new_host_menu = 
  umenu(UOrient::horizontal
        + "Host Name: "
        + utextfield(usize(150) + host_str
                     + ucall(this, (const UStr*)host_str, &UFinder::addHostCB)
                     + ucall(0, &UElem::closeWin))
        + ubutton(" Add "
                  + ucall(this, (const UStr*)host_str, &UFinder::addHostCB)
                  + ucall(0, &UElem::closeWin))
        );
  
  hostlist.addAttr(UOrient::vertical + utop());
  hostlist.add(uitem(UFont::italic + " Add Host" + new_host_menu));

  UBox& hosts = uscrollpane(true, false, hostlist);
  hosts.addAttr(usize(UAUTO,100) + upadding().setLeft(15));
  hosts.show(false);

  // opens/closes hosts
  optbox.add(uhbox(UOn::select / ushow(hosts, true)
                   + UOn::deselect / ushow(hosts, false)
                   + UOn::select / USymbol::down
                   + UOn::deselect / USymbol::right
                   + UFont::bold + "Remote Hosts")
             + hosts
             + ubox(usize(UAUTO,6))   // separator
             );
  
  // - - - FILES - - - -
  
  filelist.addAttr(uhcenter());
  filelist.addAttr(uvflex());
  filelist_btn = uhbox(USymbol::down + UFont::bold + "Folder Files");
  optbox.add(*filelist_btn + uvflex() + filelist);
}

/* ==================================================== [Elc] ======= */

UDocbox* UFinder::getCurrentBox() {
  if (mode == DirMode) return getIconbox();
  else if (mode == DocMode) return getDocbox();
  else return null;
};

void UFinder::zoom(float z) {
  if (getCurrentBox()) getCurrentBox()->zoom(z);
}

void UFinder::zoomIn() {
  if (getCurrentBox()) getCurrentBox()->zoomIn();
}

void UFinder::zoomOut() {
  if (getCurrentBox()) getCurrentBox()->zoomOut();
}

void UFinder::setZoomQuantum(float z) {
  if (getCurrentBox()) getCurrentBox()->setZoomQuantum(z);
}

void UFinder::showSelectedIcon() {
  UIconbox* ibox = getIconbox();
  if (ibox) {
    UIcon* icon = ibox->getSelectedIcon();
    if (icon) ibox->scrollpane().makeVisible(*icon);
  }
}

void UFinder::showDocBox() {
  UIconbox* ibox = getIconbox();
  if (ibox) {
    ibox->title() = ibox->pathname().basename();
    filelist.add(*ibox);
  }
  filelist.show(true);
  filelist_btn->show(true);
  showSelectedIcon();
}


void UFinder::showDirBox() {
  filelist.removeAll();
  UIconbox* ibox = getIconbox();
  if (ibox) ibox->title() = ibox->pathname();
  filelist.show(false);
  filelist_btn->show(false);
}

/* ==================================================== [Elc] ======= */

void UFinder::open(const UStr& path) {
  if (path.empty()) return;

  int path_type = UFileInfo::parsePrefix(path);

  if (path_type == UFileInfo::LOCAL)
    openImpl(path, 0/*mode unknow at this stage*/, UFileInfo::LOCAL);
  else
    new UFinderCom(this, path, path_type);
}


void UFinder::openImpl(const UStr& path_name, int path_mode, int path_type) {
  if (path_name.empty()) return;
  UStr path = path_name;

  // local file or dir  
  if (path[0] == '~' && (path[1] == '/' || path[1] == 0)) {
    //NB: ne PAS faire de free() sur un getenv!
    static char* home = ::getenv("HOME");
    if (home) path.replace(0, 1, home);   // virer ~ mais pas /
  }

  // cas xxxx/. virer le . (mais pas /..)
  if (path.length() > 1
      && path[path.length()-1] == '.'
      && path[path.length()-2] == '/'
      ) {
    path.remove(-1, 1);  // remove last char
  }

  if (path_type == UFileInfo::LOCAL) {
    UFileMode fmode(path);
    if (!fmode.isValid()) {
      showAlert("Can't load: " & path);
      return;
    }
    if (fmode.isDir()) openDir(path, path_type);
    else if (fmode.isFile()) openFile(path, path_type);
    else showAlert("Invalid file: " & path);
  }

  else {
    if (path_mode == UFileInfo::DIR) openDir(path, path_type);
    else openFile(path, path_type);
  }
}

/* ==================================================== [Elc] ======= */

void UFinder::openParent() {
  UStr name;
  if (piconbox) name = piconbox->pathname();
  else name = ".";

  if (mode == DocMode) {
    // c'est necessaire pour les URL pour indiquer un directory
    if (name[-1] != '/') name.append('/');
    open(name);
  }
  else {
    //c'est pas bon: si basename est protege en lecture on peut plus remonter!
    //name &= "/.."; name = name.dirname(); openDir(name);
    open(name.dirname(true));
  }
}

void UFinder::openFBox(UFilebox& openbox) {
  UStr path = openbox.getPath();
  open(path);
}

void UFinder::openMsg(UMessageEvent& e) {
  const UStr* file = e.getMessage();
  if (!file || file->empty()) openEntry();
  else {
    // d'abord ouvrir le directory pour que next/prev fonctionne
    open(file->dirname());
    if (piconbox) piconbox->selectIcon(file->basename());
    open(*file);
  }
}

/* ==================================================== (c)[Elc] ======= */
namespace impl {

  struct CMenu : public UCtlmenu {
    UElem * mopener;
    CMenu() {}
  };
  

  struct ZoomAction : public UCall {
    CMenu& cmenu;
    UFinder& browser;
    double base;
    int incr, last_incr;
    uptr<UScale> scale;
    
    static const int QUANTUM;
    static const float ZOOM_QUANTUM;
    
    ZoomAction(CMenu& _c, UFinder& _b) :  cmenu(_c), browser(_b) {}
    
    virtual void operator()(UEvent& event) {
      UMouseEvent& e = (UMouseEvent&)event;
      
      if (e.isFirstDrag()) {
        UElem* source = cmenu.mopener;
        scale = null;
        
        if (source == browser.getIconbox())
          scale = &browser.getIconbox()->scale();
        else if (source == browser.getDocbox())
          scale = &browser.getDocbox()->scale();
        
        if (scale) {
          base = *scale;
          incr = 0;
          last_incr = 0;
        }
      }
      
      else {  // !isFirstDrag
        if (!scale) return;
        int val = int(cmenu.getMouseMovement().x / QUANTUM);
        if (val == incr) return;
        
        if (val > 10) val = 10;  // eviter les valeurs extermes
        else if (val < -10) val = -10;
        incr = val;
        
        if (scale && incr != last_incr) {
          last_incr = incr;
          *scale = base * pow(ZOOM_QUANTUM, incr);
        }
      }
    }
  };
  
  const int ZoomAction::QUANTUM = 15;
  // eventuellement (mais pas forcement) le meme ZOOM_QUANTUM que zZ
  const float ZoomAction::ZOOM_QUANTUM = 1.10;
  
  /* ==================================================== ======== ======= */
  
  struct ScrollAction : public UScrollAction {
    CMenu& cmenu;
    UFinder& f;
    
    ScrollAction(CMenu& _c, UFinder& _f, float _gain = 0.3)
      : UScrollAction(_gain), cmenu(_c), f(_f) {}
    
    virtual void mdrag(UMouseEvent& e, UCtlmenu&) {
      if (e.isFirstDrag()) {
        UElem* source = cmenu.mopener;
        if (source == f.getIconbox()) setPane(f.getIconbox()->scrollpane());
        else if (source == f.getDocbox()) setPane(f.getDocbox()->scrollpane());
      }
      UScrollAction::mdrag(e, cmenu);
    }  
  };
  
/* ==================================================== ======== ======= */

  void mpressCB(UMouseEvent& e, CMenu* m) {
    // open menu if RightButton ou CTRL-LeftButton
    if (e.getButton() == e.RightButton
        || (e.getButton() == e.LeftButton && e.isControlDown())) {
      m->mopener = e.getSource();
      m->open(e);
    }
    // propagate event to children otherwise
    else e.propagate();
  }
  
}  // endof nested namespace

  
void UFinder::openContextMenuIn(UBox& box) {  
  // mpressCB() opens menu if RightButton or CTRL-LeftButton and propagates events
  // to children otherwise
  box.catchEvents(UOn::mpress / ucall((impl::CMenu*)ctlmenu(), impl::mpressCB));
}

UCtlmenu& UFinder::createContextMenu() {
  impl::CMenu* cmenu = new impl::CMenu();
  
  impl::ZoomAction* za = new impl::ZoomAction(*cmenu, *this);
  cmenu->item(4).add(UFont::xx_large + UFont::bold + UColor::red + " Z- " 
                      + UOn::mdrag/za);
  cmenu->item(0).add(UFont::xx_large + UFont::bold + UColor::red + " Z+ "
                      + UOn::mdrag/za);
  
  impl::ScrollAction* sa = new impl::ScrollAction(*cmenu, *this);
  cmenu->item(2).add(UPix::bigUp + UOn::mdrag/sa);
  cmenu->item(6).add(UPix::bigDown + UOn::mdrag/sa);
  
  return *cmenu;
}

}

