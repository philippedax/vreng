/************************************************************************
 *
 *  ueventflow.cpp: Event Flow
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
#include <algorithm>
#include <ubit/uboxgeom.hpp>
#include <ubit/uwin.hpp>
#include <ubit/umenu.hpp>
#include <ubit/umenuImpl.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/uevent.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/uselection.hpp>
#include <ubit/upix.hpp>
#include <ubit/uon.hpp>
#include <ubit/utimer.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/u3d.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

int UEventFlow::TIP_X_SHIFT = 20;

void UEventFlow::LastPressed::reset() {
  view = null;
  box = null;
  canvas_view = null;
  refpos_in_canvas = null;
  behavior = UBehavior::MOUSE;
}

void UEventFlow::LastPressed::set(UView* source_view, 
                                  const UPoint& win_pos, const UPoint& screen_pos, 
                                  const UViewFind& vf) {
  view = source_view;
  box = view ? view->getBox() : null;
  // pos of the window in the screen (for drag and release events)
  win_in_screen.x = screen_pos.x - win_pos.x;
  win_in_screen.y = screen_pos.y - win_pos.y;
  canvas_view = vf.canvas_view;           // != null if the source is in a 3Dwidget
  refpos_in_canvas = vf.refpos_in_canvas; // 3Dpos of 3Dwidget in canvas_view (if it is != null)
  behavior = vf.bp;
}
  
UEventFlow::UEventFlow(UDisp& _d, int _channel) :
id(-1),     // not init!
channel(_channel),
disp(_d),
lastEntered(null),
currentFocus(null),
beingClicked(null),
auto_repeat_timer(new UTimer(UAppli::conf.auto_repeat_delay, -1, false)),
tip_timer(new UTimer(false)),
tip_win(*new UMenu()),
menu_man(*new UMenuManager(this)),
selection(*new USelection(disp.getConf().selection_color,
                          disp.getConf().selection_bgcolor,
                          disp.getConf().selection_font)),
user_data(null) {
  lastArmed     = null;
  dragSource    = dropTarget = null;
  lastCursor    = null;
  click_time    = 0;
  click_count   = 0;
  click_pos.x = click_pos.y = -5;
  auto_repeat_timer->onAction(ucall(this, &UEventFlow::autoRepeatCB));
  tip_timer->onAction(ucall(this, &UEventFlow::openTipCB));
  tip_win.addAttr(UBackground::wheat);
  tip_win.addAttr(ualpha(0.8));
  tip_win.addAttr(upadding(2,2));
  disp.add(tip_win);
  id = UAppli::impl.flowlist.size();
  UAppli::impl.flowlist.push_back(this);
}

UEventFlow::~UEventFlow() {
   // ici il faudrait notifier le UDisp !!!
  delete auto_repeat_timer;
  delete tip_timer;
  delete &tip_win;
  delete &menu_man;
  delete &selection;
  for (int k=0; k < (int)tele_pointers.size(); k++) {
    if (tele_pointers[k]) delete tele_pointers[k];
  }
}

/* ==================================================== ===== ======= */

void UEventFlow::deleteNotify(UView* del_view) {
  //cerr << "DEL_VIEW " << del_view << endl;
  if (del_view == lastEntered) lastEntered = null;
  if (del_view == lastPressed.view) lastPressed.reset();
  if (del_view == currentFocus) currentFocus = null;
}

void UEventFlow::deleteNotify(UElem* del_box) {
  //cerr << "DEL_BOX " << del_box << endl;
  if (del_box == lastArmed)    lastArmed  = null;
  if (del_box == beingClicked) beingClicked  = null;
  if (del_box == dragSource)   dragSource = null;
  if (del_box == dropTarget)   dropTarget = null;
  if (del_box == selection.getObj()) selection.setObj(null);
}

void UEventFlow::closeAllMenus() {
  getMenuManager().closeAllMenus(true);
}  
  
/* ==================================================== ===== ======= */

void UEventFlow::setFocus(UView* v) {
  currentFocus = v;
}

void UEventFlow::setCursor(UEvent& e, const UCursor* c) {
  if (channel != 0) return;            // Cursor pas gere pour flowID > 0
  
  if (c != lastCursor) {
    lastCursor = c;
    UHardwinImpl* win = null;
    UView* v = null;
    UInputEvent* ie = e.toInputEvent();

    if (ie && (v = ie->getView()) && (win = v->getHardwin())) win->setCursor(c);
    // hum, un peu problematique, le curseur devrait etre un atribut du display
    // et now des window comme c'est le cas avec x11
  }
}
    
void UEventFlow::startAutoRepeat(UEvent& e) {
  // !!! ici il faudrait faire en sorte que e.source_view soit correct !!!
  auto_repeat_timer->start();
}
  
void UEventFlow::stopAutoRepeat(UEvent& e) {
  auto_repeat_timer->stop();
}
  
void UEventFlow::autoRepeatCB(UTimerEvent& e) {
  // !!! ici il faudrait faire en sorte que e.source_view soit correct !!!
  if (lastArmed) {
    e.setCond(UOn::arm);
    lastArmed->fire(e);
  }
  else auto_repeat_timer->stop(); // securite
}

// ==================================================== ===== =======
  
bool UEventFlow::mustCloseMenus(UView* source_view) {
  // NB: bug avec scrollpanes car les scrollbars sont dans border ce qui fait
  // echouer isChildOf => isMenuClosingDisabled() compense ca
  return 
  (menu_man.top_menu != null
   && (source_view == null        // mouse outside GUI
       || (!source_view->getBox()->isMenuClosingDisabled()
           && !source_view->getBox()->isChildOf(*menu_man.top_menu)) // mouse outside menu
       ));
}
  
void UEventFlow::redirectMousePress(UMouseEvent& e, UView* winview) {
  //e.setBrowsingGroup(getBrowsingGroup());
  //cerr << "redirect " << winview << " " << winview->getBox()->getClassName() << endl;
  mousePress(winview,  // should be the view of the menu
             e.when, e.state, 
             UPoint(e.getScreenPos().x - winview->getScreenPos().x,
                    e.getScreenPos().y - winview->getScreenPos().y),
             e.abs_pos, e.button);
}
  
// ==================================================== ===== =======
  
void UEventFlow::mousePress(UView* winview, unsigned long time, int state, 
                            const UPoint& win_pos, const UPoint& screen_pos, int btn) 
{
  UWin* modalwin = static_cast<UWin*>(UAppli::impl.modalwins->getChild(0)); //@@@??? why 0 ??
  if (modalwin && modalwin != winview->getBox()) {  // box outside modalwin
    modalwin->highlight(true);
    return;
  }
  UPoint source_pos;
  UViewFind vf(winview, win_pos, UBehavior::MOUSE, UElem::Modes::CATCH_MOUSE_MASK);
  UView* source_view = winview->findSource(vf, source_pos);
    
  if (mustCloseMenus(source_view)) {
    menu_man.closeAllMenus(true);
    return;
  }
  
  if (!source_view) return;   // must be AFTER test with closeAllMenus() !
  
  UMouseEvent e(UOn::mpress, source_view, this, time, state, source_pos, screen_pos, btn);  
  e.event_observer = vf.bp.event_observer;
  e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
  e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
  
  if (UAppli::hasTelePointers()) showTelePointers(e, +1);
  
AGAIN:
  // security if the crossing was not detected by boxMotion()
  //boxCross(source_view, time, state, vf.bp);
  boxCross(source_view, time, state, vf.bp.cursor, false);
  
  UBox* box = source_view->getBox();
  if (box->emodes.IS_TEXT_SELECTABLE 
      || (box->callback_mask & UOn::KEY_CB)
      || (box->emodes.CATCH_EVENTS & UElem::Modes::CATCH_KEY_MASK)) {
    currentFocus = source_view;   // FOCUS!
    //cerr << "Focus " << box << " " << box->getClassName() << endl;
  }
  
  // ne pas changer lastPressed si on presse sur un 2eme bouton de 
  // la souris sans avoir relache le premier
  if (e.getButton() == e.getButtons()) {
    lastPressed.set(source_view, win_pos, screen_pos, vf);
  }
  
  // UOn::press AVANT UOn::armBehavior comme Java etc. 
  // fire() sur 'box' et non lastPressed car le press peut se faire sur une autre
  // source que celle de depart si on presse ensuite un autre bouton de la souris
  box->fire(e); 
  
  if (vf.catched && e.modes.PROPAGATE) {
    source_view = source_view->findSourceNext(vf, source_pos);
    if (!source_view) { // cas anormal!
      UAppli::internalError("UEventFlow::mousePress","Null source view");
      return;
    }
    
    e.source_view = source_view;
    e.pos = source_pos;
    e.event_observer = vf.bp.event_observer;
    e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
    e.modes.PROPAGATE = false;
    goto AGAIN;
    // sinon cas standard : on execute la suite
  }
  
  //box->armBehavior(e, vf.bp);  // MOUSE, not browsing
  box->armBehavior(e, false);
  
  if (box->emodes.IS_TEXT_SELECTABLE && e.getButton() == UAppli::conf.mouse_select_button)
    selection.start(e);
}

// ==================================================== ===== =======

void UEventFlow::mouseRelease(UView* winview, unsigned long time, int state, 
                              const UPoint& win_pos, const UPoint& screen_pos, int btn) 
{  
  //if (UAppli::hasTelePointers()) showTelePointers(e, -1);
  // security if the crossing was not detected by boxMotion()
  //boxCross(e.source_view, e.when, e.state, UElem::NORMAL);
  
  if (lastPressed.view) {
    // if lastPressed is set there is no reason to check the modal dialog condition

    // win_pos cannot be used because it is relative to the window that the mouse
    // is over when using X11: this won't be the lastPressed win if another window
    // was opened on the top of it in the meantime (typically a popup menu)
    UPoint where(screen_pos.x - lastPressed.win_in_screen.x,
                 screen_pos.y - lastPressed.win_in_screen.y);
    
    // if lastPressed was in a 3D widget, 'where' must be relative to this widget
    if (lastPressed.canvas_view) {
      lastPressed.canvas_view->unproject(lastPressed.refpos_in_canvas, win_pos, where);
    }
    
    // pos of source = pos in ref (win or 3Dwidget) - pos of source in ref
    where.x -= lastPressed.view->x;
    where.y -= lastPressed.view->y;

    /*
    cerr << endl <<"* mouseRelease: srcview: "<< lastPressed.view << " " 
    << (lastPressed.view ? lastPressed.view->getBox()->getClassName() : "")
    << " / pos-in-screen " << screen_pos.x << " " << screen_pos.y
    << " / pos-in-source " << where.x << " " << where.y
    << " / source-in-win " << lastPressed.view->x << " " << lastPressed.view->y
    << endl << endl;
    */
    
    UMouseEvent e(UOn::mrelease, lastPressed.view, this, time, state, where, 
                  screen_pos, btn);
    e.event_observer = lastPressed.behavior.event_observer;
    e.modes.DONT_CLOSE_MENU = lastPressed.behavior.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = lastPressed.behavior.SOURCE_IN_MENU;
    lastPressed.box->fire(e);
      
    if (selection.beingSelected()) {
      if (selection.complete(e)) {
        // ! Alternative: Ctrl-C Pomme-C au lieu d'etre fait implicitement !!@@@
        disp.copySelection(e, selection);
      }
    }
  
    if (lastPressed.view) {  // verify that lastPressed has not been destroyed by fire()
      //lastPressed.box->disarmBehavior(e, lastPressed.behavior);
      lastPressed.box->disarmBehavior(e, false);
    }
    
    if (e.getButtons() == 0)  // plus aucun button pressed
      lastPressed.reset();
  }
}

// ==================================================== ===== =======

void UEventFlow::mouseMotion(UView* winview, unsigned long time, int state,
                             const UPoint& win_pos, const UPoint& screen_pos) {
  // MOUSE_DRAG CASE
  if (lastPressed.view) {
    // NB: if lastPressed is set there is no reason to check the modal dialog condition
    
    // win_pos cannot be used because it is relative to the window that the mouse
    // is over when using X11: this won't be the lastPressed win if another window
    // was opened on the top of it in the meantime (typically a popup menu)
    UPoint where(screen_pos.x - lastPressed.win_in_screen.x,
                 screen_pos.y - lastPressed.win_in_screen.y);
    
    // if lastPressed was in a 3D widget, 'where' must be relative to this widget
    if (lastPressed.canvas_view) {
      lastPressed.canvas_view->unproject(lastPressed.refpos_in_canvas, win_pos, where);
    }
    
    // pos of source = pos in ref (win or 3Dwidget) - pos of source in ref
    where.x -= lastPressed.view->x;
    where.y -= lastPressed.view->y;
    //cerr << where  << " / win " << win_pos << " / scr " << screen_pos << endl;

    // the button number is 0 for drag and movve events    
    UMouseEvent e(UOn::mdrag, lastPressed.view, this, time, state, where, screen_pos, 0);
    e.event_observer = lastPressed.behavior.event_observer;
    e.modes.DONT_CLOSE_MENU = lastPressed.behavior.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = lastPressed.behavior.SOURCE_IN_MENU;
    e.modes.IS_MDRAG = true;
 
    if (selection.beingSelected() /* && box->emodes.IS_TEXT_SELECTABLE*/)
      selection.extend(e);
    
    if (UAppli::hasTelePointers()) showTelePointers(e);

    // Drag callbacks called if MOUSE_DRAG_CB is ON if we are in the
    if (lastPressed.box->hasCallback(UOn::MOUSE_DRAG_CB) || e.event_observer)
      lastPressed.box->fire(e);
  }
  
  // MOUSE_MOVE CASE
  else {
    UViewFind vf(winview, win_pos, UBehavior::MOUSE, UElem::Modes::CATCH_MOUSE_MOVE_MASK);
    UPoint source_pos;
    UView* source_view = winview->findSource(vf, source_pos);
    if (!source_view) return;
  
    UMouseEvent e(UOn::mmove, source_view, this, time, state, source_pos, screen_pos, 0);
    e.event_observer = vf.bp.event_observer;
    e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;

    // bool mbtn_pressed = (e.getButtons() != 0);
    //vf.bp.intype = mbtn_pressed? UBehavior::BROWSE: UBehavior::MOUSE;

    if (UAppli::hasTelePointers()) showTelePointers(e);

AGAIN:
    //boxCross(source_view, time, state, vf.bp);
    boxCross(source_view, time, state, vf.bp.cursor, vf.bp.intype == UBehavior::BROWSE);

    UBox* box = source_view->getBox();
    if (box && (box->hasCallback(UOn::MOUSE_MOVE_CB) || e.event_observer))
      box->fire(e);
      
    if (vf.catched && e.modes.PROPAGATE) {
      source_view = source_view->findSourceNext(vf, source_pos);
      if (!source_view) { // cas anormal!
        UAppli::internalError("UEventFlow::mouseMotion","Null source view");
        return;
      }
      e.source_view = source_view;
      e.pos = source_pos;
      e.event_observer = vf.bp.event_observer;
      e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
      e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
      e.modes.PROPAGATE = false;
      goto AGAIN;
    }
  }
}

// ==================================================== ===== =======

void UEventFlow::wheelMotion(UView* winview, unsigned long time, int state, 
                             const UPoint& win_pos, const UPoint& screen_pos,
                             int type, int delta) {
  UWin* modalwin = static_cast<UWin*>(UAppli::impl.modalwins->getChild(0));
  if (modalwin && modalwin != winview->getBox()) {  // box outside modalwin
    modalwin->highlight(true);
    return;
  }
  UViewFind vf(winview, win_pos, UBehavior::MOUSE/* !!! */, UElem::Modes::CATCH_WHEEL_MASK);
  UPoint source_pos;
  UView* source_view = winview->findSource(vf, source_pos);
  if (!source_view) return;
  
  if (mustCloseMenus(source_view)) {
    //cerr <<"wheelMotion mustCloseMenus()"<<endl;
    return;
  }
  
  UWheelEvent e(UOn::wheel, source_view, this, time, state, source_pos, 
                screen_pos, type, delta);
  e.event_observer = vf.bp.event_observer;
  e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
  e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;

AGAIN:
  source_view->getBox()->fire(e);
  
  if (vf.catched && e.modes.PROPAGATE) {
    source_view = source_view->findSourceNext(vf, source_pos);
    if (!source_view) { // cas anormal!
      UAppli::internalError("UEventFlow::wheelMotion","Null source view");
      return;
    }
    e.source_view = source_view;
    e.pos = source_pos;
    e.event_observer = vf.bp.event_observer;
    e.modes.DONT_CLOSE_MENU = vf.bp.DONT_CLOSE_MENU;
    e.modes.SOURCE_IN_MENU = vf.bp.SOURCE_IN_MENU;
    e.modes.PROPAGATE = false;
    goto AGAIN;
  }
}

// ==================================================== ===== =======

void UEventFlow::keyPress(UView* winview, unsigned long time, int state,
                          int keycode, short keychar) {
  if (currentFocus == null) {
    //cerr <<  "winKeyPress: NO CURRENT FOCUS"<<endl;
    return;
  }
  
  UWin* modalwin = static_cast<UWin*>(UAppli::impl.modalwins->getChild(0));
  if (modalwin && modalwin != currentFocus->getWin()) {  // focus outside modalwin
    // !!! ICI on pourrait changer le focus !!!
    modalwin->highlight(true);
    return;
  }

  //if (mustCloseMenus(source_view)) return;
  
  UKeyEvent e(UOn::kpress, currentFocus, this, time, state, keycode, keychar);
  UBox* box = currentFocus->getBox();  
  //box->keyPressBehavior(e, UBehavior::KEY);
  box->keyPressBehavior(e);
  
  // efface ou detruit la selection suivant entered key
  if (box->emodes.IS_TEXT_SELECTABLE) selection.keyPress(e);
}  
  
// ==================================================== ===== =======

void UEventFlow::keyRelease(UView* winview, unsigned long time, int state,
                            int keycode, short keychar) {
  if (currentFocus == null) {
    //cerr <<  "winKeyRelease: NO CURRENT FOCUS"<<endl;
    return;
  } 
  
  UWin* modalwin = static_cast<UWin*>(UAppli::impl.modalwins->getChild(0));
  if (modalwin && modalwin != currentFocus->getWin()) {  // focus outside modalwin
    modalwin->highlight(true);
    return;
  }

  //if (mustCloseMenus(source_view)) return;

  UKeyEvent e(UOn::krelease, currentFocus, this, time, state, keycode, keychar);
  UBox* box = currentFocus->getBox();
  //box->keyReleaseBehavior(e, UBehavior::KEY);
  box->keyReleaseBehavior(e);
}
  
// ==================================================== ===== =======
/*
 void UEventFlow::winDestroy(UWin *win, UEvent& e) {
   win->emodes.IS_SHOWABLE = false;
   if (menu_man.contains(win->toMenu()))  menu_man.closeAllMenus(true);
   delete win;
 }
*/
  
void UEventFlow::winEnter(UView* winview, unsigned long time) {                         
                            // !!! A COMPLETER....
  //UBehavior bp(UBehavior::MOUSE);
  //boxCross(winview, time, 0, bp); 
  //!!! should be the entered box PAS WINVIEW !!!@@@@
  boxCross(winview, time, 0, null, false); 
}
  
void UEventFlow::winLeave(UView* winview, unsigned long time) {
  // ATTENTION: le XGrab genere des LeaveWindow qd on ouvre le menu associe a un 
  // bouton et qd on bouge la souris sur ce bouton une fois que le menu est ouvert.
  // Ne pas en tenir compte sinon le bouton ouvrant oscille entre Enter et Leave 
  // qd on deplace la souris sur le menubar
  
  //cerr << "winview  "<<winview << endl;
  //cerr << "--  "<<winview->getBox()<< endl;
  //cerr << "--  "<<winview->getBox()->getClassName() << endl;
  
  if (menu_man.contains(winview->getBox()->toMenu())) return;
  
  //UBehavior bp(UBehavior::MOUSE);
  //boxCross(null, time, 0, bp);
  boxCross(winview, time, 0, null, false); 
}
  
void UEventFlow::boxCross(UView* new_view, unsigned long time, int state, 
                           const UCursor* cursor, bool is_browing) {   
  if (lastEntered == new_view) return;

  if (lastEntered) {
    UInputEvent e(UOn::leave, lastEntered, this, time, state);
    lastEntered->getBox()->leaveBehavior(e, is_browing);
    lastEntered = null;
  }
    
  lastEntered = new_view;

  if (new_view) {
    // ATT: e.source_view==null si on est hors de la window (auquel cas
    // il ne faut PAS faire boxEnter()
    UInputEvent e(UOn::enter, lastEntered, this, time, state);
    setCursor(e, cursor);
    lastEntered->getBox()->enterBehavior(e, is_browing);
  }
}

// ==================================================== ===== =======
 
void UEventFlow::openTipRequest(UEvent& e) {
#ifndef UBIT_WITH_GLUT
                                               // !!! bug avec GLUT a corriger !!!
  if (lastEntered) {
    tip_timer->start(UAppli::conf.open_tip_delay,1);
  }
#endif
}

void UEventFlow::closeTipRequest(UEvent& e) {
#ifndef UBIT_WITH_GLUT
                                                 // !!! bug avec GLUT a corriger !!!
  tip_timer->stop();
  // ne pas ouvrir/fermer tip_win comme un menu mais comme une window normale
  tip_win.UWin::show(false);
  tip_win.removeAll();
#endif
}

void UEventFlow::openTipCB(UTimerEvent& e) {
  if (lastEntered) {
    UBox* box = lastEntered->getBox();
    UTip* tip = null;
    box->attributes().findClass(tip);
    if (!tip) box->children().findClass(tip);
    
    if (tip && tip->content) {
      tip_win.add(*tip->content);
      tip_win.setPos(*lastEntered, UPoint(TIP_X_SHIFT, lastEntered->getHeight()));
      // ne pas ouvrir/fermer tip_win comme un menu mais comme une window normale
      tip_win.UWin::show(true, lastEntered->getDisp());
      tip_win.update(UUpdate::ADJUST_WIN_SIZE);
    }
  }
}

/* ==================================================== [Elc] ======= */

static const char* telepointer_xpm[] = {
  " 8 8 2 1",
  ". c white",
  "# c None s None",
  "........",
  ".#######",
  ".######.",
  ".#####..",
  ".#####..",
  ".######.",
  ".##..##.",
  ".#.....#",
};
static UPix telepointer_pix(telepointer_xpm, UPix::UCONST);


UWin* UEventFlow::retrieveTelePointer(UDisp* rem_disp) {
  unsigned int disp_id = rem_disp->getID();
    
  if (disp_id < tele_pointers.size() && tele_pointers[disp_id]) {
    return tele_pointers[disp_id];
  }
  else {
    for (unsigned int k = tele_pointers.size(); k <= disp_id; k++) 
      tele_pointers.push_back(null);
    
    UWin* ptr = new UMenu(UBorder::none
                          + UColor::red
                          + UBackground::red
                          + UOn::select/UBackground::green
                          + telepointer_pix
                          );
    ptr->ignoreEvents();
    tele_pointers[disp_id] = ptr;
    ptr->setSoftwin(false);
    rem_disp->add(*ptr);
    ptr->show();
    return ptr;
  }
}
  
// il y a un pbm du au fait que les vues des objects conditionnels
  // sont toujours generees. non seulement c'est inutilement couteux
  // (cf le zoom semantique etc) mais en plus ca fait deconner les
  // telepointeurs car ces vues conditionnelles n'ont pas de sens
  // (et ne sont d'ailleurs pas affichees)
  
void UEventFlow::showTelePointers(UMouseEvent& e, int mode)  {
  UBox* source = e.getSource() ? e.getSource()->toBox() : null;
  if (!source) return;
  
  for (UView* v = source->getView(0); v != null; v = v->getNext()) {
    if (v->getDisp() != e.getDisp()) {
      UWin* remptr = retrieveTelePointer(v->getDisp());
      if (remptr){
        if (mode) {
          if(mode> 0) remptr->setSelected(true);
          else if(mode< 0) remptr->setSelected(false);
          remptr->update();
        }
        remptr->toFront();
        UPoint pos = v->getScreenPos(); 
        remptr->setScreenPos(UPoint(pos.x + e.getX(), pos.y + e.getY()));
      }
    }
  }
}


}
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
