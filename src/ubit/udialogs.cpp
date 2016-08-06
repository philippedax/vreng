/************************************************************************
 *
 *  udialogs.cpp: dialog boxes
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ucall.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uboxes.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/upix.hpp>
#include <ubit/ucolor.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UStyle* UFrame::createStyle() {
  return UWin::createStyle();
}

UFrame::UFrame(UArgs a) : UDialog(a) {
  wmodes.IS_HARDWIN = true;   // a frame is always a HARDWIN!
  wmodes.IS_FRAME  = true;
  wmodes.IS_DIALOG = false;
}

// Dialogs are centered but not Frames
void UFrame::show(bool state, UDisp* disp) {
  if (isShown(disp) == state) return;
  UWin::show(state, disp);
}

bool UFrame::realize() {
  if (! wmodes.IS_HARDWIN) {
    error("UFrame::realize","can't realize UFrame object %p",this);
    return false;
  }
  if (wmodes.IS_MAINFRAME) return realizeHardwin(UWinImpl::MAINFRAME);
  else return realizeHardwin(UWinImpl::FRAME);
}

// ------------------------------------------------------------ [ELC] ----------

UStyle* UDialog::createStyle() {
  UStyle* s = UWin::createStyle();
  static UBorder* b = new URoundBorder(UBorder::LINE,UColor::black,UColor::white,2,2,15,15);
  s->setBorder(b);
  s->setPadding(4, 6);
  return s;
}

UDialog::UDialog(UArgs a) : UWin(a), open_call(null) {
  wmodes.IS_DIALOG = true;
}

bool UDialog::realize() {
  if (wmodes.IS_HARDWIN) return realizeHardwin(UWinImpl::DIALOG);
  else {
    error("UDialog::realize","can't realize UDialog object %p",this);
    return false;
  }
}

void UDialog::addingTo(UChild& c, UElem& parent) {
  UWin::addingTo(c, parent);
  // ajouter handler pour ouvrir auto le Dialog si le parent est un UButton
  if (wmodes.IS_AUTO_OPENED && parent.isArmable()) {
    if (!open_call) open_call = &ucall(this, true, &UDialog::show);
    parent.addAttr(*open_call);
  }
}

//NB: removingFrom() requires a destructor to be defined
void UDialog::removingFrom(UChild& c, UElem& parent) {
  if (wmodes.IS_AUTO_OPENED && parent.isArmable()) {
    // don't delete the ucall as it is shared
    if (open_call) {
      bool auto_up = parent.isAutoUpdate();
      parent.setAutoUpdate(false);
      parent.removeAttr(*open_call, false);
      parent.setAutoUpdate(auto_up);
    }
  }
  UWin::removingFrom(c, parent);
}

void UDialog::show(bool state, UDisp* disp) {
  if (isShown(disp) == state) return;
  UView* v = getView(0);
  bool already_shown = (v && v->hasVMode(UView::INITIALIZED));
  UWin::show(state, disp);
  
  // deconne si avant show, ne recentrer qu'a la la apparition
  if (state && !already_shown) centerOnScreen(disp);
}

// ------------------------------------------------------------ [ELC] ----------
/* creates a new OptionDialog; see also creator shortcut uoptiondialog().
 * All parameters are optional:
 * - 'title' is the dialog title
 * - 'content' controls the main display area, typically for displaying a text message,
 *    but it can contain any combination of UNode objects (separated by a + operator)
 * - 'icon' controls an area on the left hand side, normaly used for displaying an image.
 *    As for 'content' it can in fact contain any combination of objects.
 *    No icon is displayed if this argument is 'none'.
 * - 'buttons' controls the button area, located on the bottom. By default, a "OK"
 *    button is displayed if this argument is 'none'. Otherwise, whatever passed
 *    to this argument will be displayed in the button area.
 * @see UElem::add(UArgs arguments) for more info on argument lists.
 */
/*
 int UOptionDialog::showDialog(const UStr& title, UArgs message, UArgs icon, UArgs ok) {
 setDialog(title, message, icon, ok);
 show(true);
 showModal( );  !!!!!&&&&
 }
 */

UOptionDialog::UOptionDialog(UArgs message) {
  constructs(message);
}

UOptionDialog::UOptionDialog(const UStr& title, UArgs message, UArgs icon, UArgs buttons) {
  setTitle(title);
  constructs(message);
  setIcon(icon);
  setButtons(buttons);
}

void UOptionDialog::constructs(UArgs message) {
  pmessage = uhbox(message);
  pmessage->addAttr(uhflex());

  pbuttons = uhbox(ubutton("    OK    " + ucloseWin()));
  pbuttons->addAttr(uhcenter() + UFont::bold + UFont::large);
    
  picon = new UBox;
  picon->addAttr(uhcenter() + uvcenter());

  addAttr(upadding(12,12) + uvspacing(12));
  
  add(uhflex() + uvflex() 
      + uhbox(*picon + uhflex() + *pmessage)
      + ubottom()
      //+ usepar()
      + *pbuttons
      );
}

UOptionDialog& UOptionDialog::setTitle(const UStr& title) {
  UWin::setTitle(title);
  return *this;
}

UOptionDialog& UOptionDialog::setMessage(UArgs nodelist) {
  pmessage->removeAll();
  pmessage->add(nodelist);
  adjustSize();
  return *this;
}

UOptionDialog& UOptionDialog::setMessage(const UStr& msg) {
  return setMessage(UArgs(ustr(msg)));
}

UOptionDialog& UOptionDialog::setIcon(UArgs icon) {
  picon->removeAll();
  if (!icon) picon->show(false);
  else {
    picon->add(icon);
    picon->show(true);
  }
  adjustSize();
  return *this;
}

UOptionDialog& UOptionDialog::setButtons(UArgs buttons) {
  pbuttons->removeAll();
  if (!buttons) pbuttons->add(ubutton(" OK " + ucloseWin()));
  else {
    pbuttons->add(buttons);
    for (UChildIter i = pbuttons->cbegin(); i != pbuttons->cend(); i++) {
      UButton* b = dynamic_cast<UButton*>(*i);
      if (b) b->addAttr(ucloseWin());
    }
  }
  adjustSize();
  return *this;
}
// ------------------------------------------------------------ [ELC] ----------
/*
 ERROR_MESSAGE
 INFORMATION_MESSAGE
 WARNING_MESSAGE
 QUESTION_MESSAGE
 PLAIN_MESSAGE
 */

UOptionDialog* UDialog::messdialog = null;

void UDialog::showMessageDialog(const UStr& title, UArgs message_nodes, UArgs icon) {
  if (!messdialog) messdialog = new UOptionDialog();
  messdialog->setMessage(message_nodes);
  messdialog->setIcon(icon);
  messdialog->show(true);
  messdialog->setTitle(title);
}

void UDialog::showMessageDialog(const UStr& title, const UStr& msg_string, UArgs icon) {
  showMessageDialog(title, UArgs(ustr(msg_string)), icon);
}

void UDialog::showAlertDialog(UArgs message_nodes) {
  showMessageDialog("Alert", 
                    UFont::large + upadding(5,5) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void UDialog::showAlertDialog(const UStr& message_string) {
  showAlertDialog(UArgs(ustr(message_string)));
}

void UDialog::showErrorDialog(UArgs message_nodes) {
  showMessageDialog("Error", 
                    UFont::large + upadding(5,10) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void UDialog::showErrorDialog(const UStr& message_string) {
  showErrorDialog(UArgs(ustr(message_string)));
}

void UDialog::showWarningDialog(UArgs message_nodes) {
  showMessageDialog("Warning", 
                    UFont::large + upadding(5,10) + message_nodes, 
                    uscale(1.5) + UPix::ray);      // !!! CHOIX ICON !!!!
}

void UDialog::showWarningDialog(const UStr& message_string) {
  showWarningDialog(UArgs(ustr(message_string)));
}

}
