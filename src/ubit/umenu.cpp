/************************************************************************
 *
 *  umenu.cpp: menu bars, pulldown and popup menus
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
#include <iostream>
#include <ubit/umenu.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/ucall.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uview.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uevent.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/umenuImpl.hpp>
#include <ubit/uconf.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/utimer.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UStyle* UPopmenu::createStyle() {
  return UMenu::createStyle();
}

UPopmenu::UPopmenu(const UArgs& a) : UMenu(a) {
  wmodes.IS_AUTO_OPENED = false;
}

/* ==================================================== ======== ======= */

UStyle* UMenubar::createStyle() {
  UStyle* style = new UStyle();
  style->textSeparator = new UStr("\t");
  style->orient = UOrient::HORIZONTAL;
  style->halign = UHalign::LEFT;
  style->valign = UValign::FLEX;
  style->hspacing = 4;
  style->vspacing = 5;
  style->font = &UFont::bold;
  style->local.border = &UBorder::shadowOut;
  return style;
}

UMenubar::UMenubar(const UArgs& a): UBar(a) {
  emodes.IS_BROWSABLE = true;
  
  static UMultiCond* cond = null;
  if (!cond) {
    cond = new UMultiCond();
    *cond += UOn::enter;
    *cond += UOn::leave;
    *cond += UOn::mrelease;
    *cond += UOn::arm;  // only by menubar
  }
  // recupere evenements dans les enfants
  observeChildrenEvents(*cond / ucall(this, &UMenubar::menuChildCB));
}

void UMenubar::menuChildCB(UInputEvent& e) {
  UMenuManager& mc = e.getFlow()->getMenuManager();
  mc.menuChildCB(e, this);
}

/* ==================================================== [(c)Elc] ======= */

UStyle* UMenu::createStyle() {
  UStyle& s = *new UStyle();
  s.textSeparator = &ustr("\n");
  s.orient = UOrient::VERTICAL;
  s.halign = UHalign::FLEX;
  s.valign = UValign::TOP;
  s.hspacing = 1;
  s.vspacing = 1;
  s.setPadding(0, 0);
  s.local.border = &UBorder::shadowOut;
  s.local.background = &UBackground::velin;
  return &s;
}

/* ==================================================== ======== ======= */

UMenu::UMenu(const UArgs& a) : 
UWin(a),
menu_opener_cb(null),
placement(null)
{
  wmodes.IS_MENU = true;
  emodes.IS_BROWSABLE = true;
  if (UAppli::conf.soft_menus) wmodes.IS_HARDWIN = false;
  
  static UMultiCond* cond = null;
  if (!cond) {
    cond = new UMultiCond();
    *cond += UOn::enter;
    *cond += UOn::leave;
    *cond += UOn::mrelease;
  }
  // recupere evenements dans les enfants
  observeChildrenEvents(*cond / ucall(this, &UMenu::menuChildCB));
}

// necessaire a cause de removingFrom (car les fct ne sont pas virtuelles
// dans les desctructeurs!)
UMenu::~UMenu() {
  // att: reset du MenuCtrl si on detruit le menu
  UEventFlow* fl = UAppli::getFlow(0);   // DEFAULT IFLOW : A REVOIR
  if (fl) fl->getMenuManager().closeAllMenus(true);
  if (placement) delete(placement); placement = null;
  destructs();
}

bool UMenu::realize() {
  if (wmodes.IS_HARDWIN) return realizeHardwin(UWinImpl::MENU);
  else {
    UAppli::internalError("UMenu::realize",
                          "can't realize the soft window of this UMenu (%p)",this);
    return false;
  }
}

void UMenu::closeAllMenus(UInputEvent& e) {
  e.getFlow()->getMenuManager().closeAllMenus(true);
}

void UMenu::setPlacement(const UWinPlacement& pl) {
  if (placement) delete placement;
  placement = new UWinPlacement(pl);
}

/* ==================================================== ======== ======= */

void UMenu::addingTo(UChild& c, UElem& parent) {
  UWin::addingTo(c, parent);
  
  static UMultiCond* cond = null;
  if (!cond) {
    cond = new UMultiCond();
    *cond += UOn::enter;
    *cond += UOn::leave;
    *cond += UOn::arm;
    *cond += UOn::disarm;
  }
  if (wmodes.IS_AUTO_OPENED) {
    if (!menu_opener_cb) menu_opener_cb = &ucall(this, &UMenu::menuOpenerCB);
    parent.addAttr(*cond / *menu_opener_cb);
  }
}

//NB: removingFrom() requires a destructor to be defined
void UMenu::removingFrom(UChild& c, UElem& parent) {
  // don't delete the ucalls as they are shared
  if (menu_opener_cb) parent.removeAttr(*menu_opener_cb);
  
  UWin::removingFrom(c, parent);
}

/* ==================================================== [(c)Elc] ======= */
// Cette fonction est appelee par les boutons qui ouvrent un submenu.

void UMenu::menuOpenerCB(UInputEvent& e) {
  UMenuManager& mc = e.getFlow()->getMenuManager();
  UView* opener_view = e.getView();
  
  if (e.getCond() == UOn::arm) {
    //cerr<< endl << "UMenu::armOpener"<< endl;
    openImpl(mc, opener_view, true, null);
    
    // noter que top_menu est esnuite ecrase par menuChildCB() si le bouton 
    // est dans un menubar
    if (mc.top_menu == null) mc.top_menu = this;
  }
  
  else if (e.getCond() == UOn::disarm) {
    //cerr<< endl<< "UMenu::disarmOpener"<< endl;
    mc.active_menu = this;
    mc.active_opener = e.getView(); //verif
  }
  
  // !!! normalement ca devrait sur TOUS les boutosn conetnus dans un menu,
  // !!! pas seulement ceux qui ouvrent des sous menus   
  else if (e.getCond() == UOn::enter) {
    //cerr << "menuOpenerCB: enter1" << mc.top_menu<< endl;
    if (mc.top_menu != null) {
      openImpl(mc, opener_view, true, null);
    }
  }
  
  else if (e.getCond() == UOn::leave) {
    //cerr << "menuOpenerCB: leave1" << mc.top_menu<< endl;
    if (mc.top_menu != null) {
      mc.closeSubMenus(this, false);
    }
  }
}

void UMenu::menuChildCB(UInputEvent& e) {
  UMenuManager& mc = e.getFlow()->getMenuManager();
  mc.menuChildCB(e, this);
}

void UMenuManager::menuChildCB(UInputEvent& e, UBox* menu_or_menubar) {
  if (e.getCond() == UOn::mrelease) {
    if (active_opener != e.getView() && !e.dontCloseMenu()) {
      //cerr << "menuChildCB: release" << endl;
      closeAllMenus(true);
    }
  }
  else if (e.getCond() == UOn::arm) {
    //cerr << "menuChildCB: arm" << endl;
    // forces value set by setTopMenu() when a menubar button is armed
    // (to set the menubar instead of the open menu)
    top_menu = menu_or_menubar;
  }
  else if (e.getCond() == UOn::enter) {
    // !!!!@@@@ BUG: enter and leave events pas observes !!!!!!@@@
    cerr << "menuChildCB: enter" << endl;
  }
  else if (e.getCond() == UOn::leave) {
    cerr << "menuChildCB: leave" << endl;
  }
}  

void UMenuManager::openMenuAfterDelay() {
  if (possible_opener && possible_opener_menu) {
    possible_opener_menu->openImpl(*this, possible_opener,
                                   true, //autoplace
                                   null);
    possible_opener = null;
    possible_opener_menu = null;
  }
}

void UMenuManager::closeMenuAfterDelay() {
  if (possible_closer && possible_closer_menu) {
    closeSubMenus(possible_closer_menu, false);
    possible_closer = null;
    possible_closer_menu = null;
  }
}

/* ==================================================== ======== ======= */

void UPopmenu::open(UMouseEvent& e) {
  // the menu will behave as a "spring menu" if x>=0 and y>=0 because it will
  // catch the mouse release event. x or y should be set to a negative value
  // to avoid this behavior.
  setPos(e, 2, -2);
  UMenu::show(true, e.getDisp());
}

void UMenu::show(bool state, UDisp* disp) {
  if (UAppli::isExiting()) return;
  
  if (wmodes.IS_HARDWIN) 
  /*nop*/;     // hardwins have only one shared window
  else if (disp == null) {
    // verifier si actual_view toujours dans la liste
    USoftwinImpl* softw = softImpl();
    UView* v = softw ? softw->getActualView(views) : null;
    if (v) disp = v->getDisp();
  }
  
  if (state) {
    if (!disp) disp = UAppli::getDisp();
    UEventFlow* ef = const_cast<UDisp*>(disp)->obtainChannelFlow(0);
    if (ef) openImpl(ef->getMenuManager(), null, /*null,*/ false, disp);
    else UAppli::internalError("UMenu::show","null event flow; menu: %p",this);
  }
  
  else {  // !state
    UEventFlow* f = null;
    if (disp) f = disp->getChannelFlow(0);  // DEFAULT IFLOW : A REVOIR
    else f = UAppli::getFlow(0);
    
    // fermer les sous-menus de CE menu
    if (f) f->getMenuManager().closeSubMenus(this, true/*including*/);    
    UWin::show(false, disp);
  }
}

/* ==================================================== ======== ======= */

// a generaliser avec UElem::closeWinCB
static UMenu* getMenuParent(UView* opener) {
  for (UView *v = opener; v != null; v = v->getParentView()) {
    UBox* box = v->getBox();
    if (box) {
      if (dynamic_cast<UMenubar*>(box)) return null;
      UMenu* menu = dynamic_cast<UMenu*>(box);
      if (menu) return menu;
    }
  }
  return null;
}

void UMenu::openImpl(UMenuManager& mc, UView* opener, 
                     bool auto_place, UDisp* disp) {
  UView* winview = getWinView(opener ? opener->getDisp() : null);
  if (!winview)  {
    UAppli::error("UMenu::openImpl","the window of this UMenu (%p) is not realized; check if this menu has a valid parent",this);
    return;
  };
  
  UMenu* parent_menu = null;
  bool is_cascaded = false;   // sert a positionner le menu en dessous ou a droite
  
  // s'il y a deja un menu ouvert et qu'un parent (indirect) de l'opener
  // est aussi un menu alors this est un menu cascade
  // -> dans ce cas il ne faut fermer que les sous-menus deja eventuellement
  //    ouverts (et sinon il faut fermer tous les menus)
  if (//!mc.empty()
      mc.top_menu != null
      && opener && ((parent_menu = getMenuParent(opener)))
      ) {
    is_cascaded = true;
    // si ce menu est deja affiche fermer ses menus cascades
    // sinon fermer ceux du menu parent
    if (mc.contains(this)) mc.closeSubMenus(this, false);
    else mc.closeSubMenus(parent_menu, false);
  }
  
  else {
    is_cascaded = false;
    // 1aug06: si show(true) est appele, il n'y a pas d'opener, et pourtant il
    // n'y a pas de raison de fermer les menus parents
    mc.closeSubMenus(this, false);
  }
  
  // cas d'erreur: le menu ne peut etre realize
  if (wmodes.IS_HARDWIN) {
    if (!this->realize()) return;
  }
  
  // affecter le browsingGroup de UAppli:
  // -- c'est celui herite du graphe d'instanciation
  //    (en particulier le cas des menubars ou des menus cascades)
  // -- sauf s'il est nul auquel cas le browsingGroup sera l'opener
  //    qui a provoque l'ouverture du menu (cad: opener_view->getBox())
  // -- et si l'opener est nul ce sera le menu lui-meme
  
  //menuBrowsingGroup = menugroup;
  //if (!menuBrowsingGroup) menuBrowsingGroup = this; // le menu lui-meme
  
  //positionner le menu
  if (opener) {
    if (placement) setPos(*opener, *placement);
    else if (auto_place) {	 // default rules
      UWinPlacement pl;
      if (is_cascaded) {
        pl.halign = &UHalign::right;
        pl.hoppositeBorder = true;
        pl.hdist = 1;
      }  
      else {
        pl.valign = &UValign::bottom;
        pl.voppositeBorder = true;
        pl.vdist = 1;
      }
      setPos(*opener, pl);
    }
  }
  
  mc.openMenu(opener, this, /*menuBrowsingGroup,*/ disp);
  
  // le grab physique fait en sorte que le menu se ferme automatiquement
  // si on clique la souris (ou si ou tape une touche du clavier)
  // qunad la souris est en dehors de l'application
  // (ie. si le grab physique etait supprime les menus resteraient ouverts
  //  en permanence qunad la souris est dans une autre appli, ce qui n'est
  //  pas completement delirant mais n'est pas conforme a l'usage)
  
  // NB: ne pas faire de grab si plusieurs display: comme il est toujours
  // fait sur les 1er UDips il bloquerait les autres
  
  if (wmodes.IS_HARDWIN && UAppli::getDispList().size() == 1
      && UAppli::conf.menu_grab) {
    
    // EX-BUG: ne rien faire quand disp est null sinon ca bloque
    //if (disp) disp->grabPointer();+
    // mod:5fev6: il faut grabber sinon les menus ne marchent plus correctement
    // le pbm venait d'ailleurs: ungrab n'etait pas correctement appele
    
    if (!disp) disp = UAppli::getDisp();
    const_cast<UDisp*>(disp)->grabPointer(); 
  }
}

/* ==================================================== [(c)Elc] ======= */

UMenuManager::~UMenuManager() {}

UMenuManager::UMenuManager(UEventFlow* _eflow) : eflow(*_eflow) {
  disp = null;
  // grabbed menus
  menu_count = 0;
  active_menu = null;
  active_opener = null;
  possible_opener = null;
  possible_opener_menu = null;
  possible_closer = null;
  possible_closer_menu = null;
  //open_timer = new UTimer();
  //open_timer->onAction(ucall(this, &UMenuManager::openMenuAfterDelay));
  //close_timer = new UTimer();
  //close_timer->onAction(ucall(this, &UMenuManager::closeMenuAfterDelay));
  top_menu = null;
}

//bool UMenuManager::empty() const {return (menu_count == 0);}

bool UMenuManager::contains(UMenu* menu) const {
  if (!menu) return false;
  for (int k = 0; k < menu_count; k++) {
    if (menustack[k] == menu) return true;
  }
  return false;   // not found
}

bool UMenuManager::contains(UView* v) const {
  while (v && !v->getBox()->toMenu()) v = v->getParentView();
  if (!v) return false;
  return contains(v->getBox()->toMenu());
}

void UMenuManager::openMenu(UView* opener, UMenu* menu, UDisp*_disp) {
  active_menu = menu;
  active_opener = opener;
  disp = _disp;
  
  // ajouter ce menu a la liste des grabbedMenus sauf s'il y est deja
  if (!contains(menu)) {
    if (menu_count < int(menustack.size())) menustack[menu_count] = menu;
    else menustack.push_back(menu);
    menu_count++;
  }
  menu->UWin::show(true, disp);
}

void UMenuManager::closeAllMenus(bool clear_top_menu) {
  if (UAppli::isExiting()) return;
  
  for (int k = 0; k < menu_count; k++) {
    // !ATT: uses UWin::show(), not the specific UMenu::show() method.
    menustack[k]->UWin::show(false, null);
  }
  menu_count = 0;
  
  active_menu = null;
  active_opener = null;
  if (disp) const_cast<UDisp*>(disp)->ungrabPointer(); 
  else UAppli::getDisp()->ungrabPointer(); 
  
  if (clear_top_menu) top_menu = null;
}

// closes menus that are after 'menu' in the menustack (ie. does not close 'menu'
// nor the menus that opend 'menu'
//
void UMenuManager::closeSubMenus(UMenu* menu, bool including_this_menu) {
  int k;  // must be unsigned because k becomes<0 in: for (..; k >=0 ; k--) {
  int found = -1;
  
  for (k = 0; k < menu_count; k++) {
    if (menustack[k] == menu) {
      found = including_this_menu ? k-1 : k;
      break;
    }
  }
  
  // cas ou menu n'est pas encore dans la pile (1jt06)
  // fermer ceux qui sont pas ses parents
  if (found < 0 && menu && menu_count > 0) {
    for (k = menu_count-1; k >=0 ; k--) {
      if (menu->isChildOf(*menustack[k])) {
        found = k;
        break;
      }
    }
  }
  
  if (found < 0) {  // not found : depile tout et ungrab
    closeAllMenus(false);
    return;
  }
  
  // if !included skip this menu
  k++; found++;
  
  for ( ; k < menu_count; k++) {
    // !ATT: uses UWin::show(), not the specific UMenu::show() method.
    menustack[k]->UWin::show(false, null);
  }
  menu_count = found;
}


/* ==================================================== [(c)Elc] ======= */

/* $$$void UMenuManager::enterMenuOpener(UInputEvent& e, UMenu* menu_to_open) {
 // si autoOpenMode et meme browsing group, alors auto ouvrir
 //if (eflow.getBrowsingGroup() && e.getBrowsingGroup() == eflow.getBrowsingGroup()) { @@@@!!!!!
 possible_opener = e.getView();
 possible_opener_menu = menu_to_open;
 possible_closer = null;
 possible_closer_menu = null;
 open_timer->start(UAppli::conf.open_submenu_delay, 1, false);
 //}
 }
 
 void UMenuManager::enterMenubarChild(UInputEvent& e, bool inside_menubar) {
 //if (eflow.getBrowsingGroup() && e.getBrowsingGroup() == eflow.getBrowsingGroup()) 
 
 UElem* opener = e.getSource();
 cerr << "enterMenubarChild" <<  opener << endl;
 
 if (top_menu && opener && opener->isChildOf(*top_menu)) {
 
 UMenu* menu = null;
 opener->attributes().findClass(menu);
 
 cerr << " - enterMenubarChild" <<  " menu" << menu << endl;
 if (!menu) return;
 
 leaveMenubarChild(e, inside_menubar);
 
 possible_opener = e.getView();
 possible_opener_menu = menu;
 possible_closer = null;
 possible_closer_menu = null;
 //open_timer->start(UAppli::conf.open_submenu_delay, 1, false);
 
 // set by enterMenuOpener
 if (!inside_menubar && possible_opener && possible_opener_menu) {
 possible_opener_menu->openImpl(*this, possible_opener,
 true, null);
 }
 possible_opener = null;
 possible_opener_menu = null;
 }
 }
 
 void UMenuManager::leaveMenubarChild(UInputEvent& e, bool inside_menubar) {
 UElem* opener = e.getSource();
 cerr << "leaveMenubarChild" <<  opener << endl;
 
 if (active_menu) {
 if (inside_menubar) closeAllMenus();
 else closeSubMenus(active_menu, false);
 }
 possible_closer = null;
 possible_closer_menu = null;
 //}
 }
 
 void UMenuManager::enterMenuChild(UInputEvent& e, UMenu* containing_menu) {
 // on est toujours sur le meme bouton (enterMenuChild est apppele
 // apres enterMeuOpener)
 if (possible_opener == e.getView()) {
 possible_closer = null;
 }
 
 else {
 // on a change de bouton avant l'ouverture du menu
 // => interdire son ouverture
 possible_opener = null;
 
 // il y a deja un menu ouvert => le fermer (avec delai)
 if (active_menu
 // ni sur l'opener du menu (sinon fermerait le menu qu'il a ouvert)
 && active_opener != e.getView()
 // on est bien dans le meme menu group (dans les cas ou il y a
 // plusieurs menu groups, interdire les interactions indesirables
 // entre eux (seul un btn d'un meme menu group que le menu ouvert
 // doit pouvoir le fermer
 //&& eflow.getBrowsingGroup()
 // && e.getBrowsingGroup() == eflow.getBrowsingGroup()             @@@@!!!!!!!!!
 // en plus si c'est un enfant contenu dans le menu ouvert (ou un
 // sous menu) il ne faut bien sur pas le fermer
 && !e.getSource()->isChildOf(*active_menu, true)
 ) {
 possible_closer = e.getView();
 possible_closer_menu = containing_menu;
 //close_timer->start(UAppli::conf.open_submenu_delay, 1, false);
 }
 }
 }
 
 void UMenuManager::leaveMenuChild(UInputEvent& e) {
 if (possible_closer == e.getView()) possible_closer = null;
 if (possible_opener == e.getView()) possible_opener = null;
 }
 
 void UMenuManager::releaseMenuChild(UInputEvent& e) {
 if (active_opener != e.getView() && !e.isMenuClosingDisabled()) closeAllMenus();
 }
 
 */


}
