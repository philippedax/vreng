/************************************************************************
 *
 *  uboxes.cpp : misc containers.
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
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/uboxes.hpp>
#include <ubit/ucolor.hpp>  
#include <ubit/upix.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uborder.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/ulistbox.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uupdatecontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

// ==================================================== [Ubit Toolkit] =========
/* UVbox = vertical Box.
 *  Geometry: see class UBox for details.
 *
class UVbox: public UBox {
public:
  UCLASS("uvbox", UVbox, new UVbox)
  
  UVbox(UArgs args = UArgs::none) : UBox(args) {}
  ///< creates a new vertical box; @see also shortcut: uvbox().
  
  static UStyle* createStyle();
};

 * UHbox = horizontal Box.
 *  Geometry: see class UBox for details.
 *
class UHbox: public UBox {
public:
  UCLASS("uhbox", UHbox, new UHbox)
  
  UHbox(UArgs args = UArgs::none) : UBox(args) {}
  ///< creates a new horizontal box; @see also shortcut: uhbox().
  
  static UStyle* createStyle();
};

UStyle* UHbox::createStyle() {
  UStyle* style = new UStyle();
  style->textSeparator  =  &ustr(" ");
  style->orient   = UOrient::HORIZONTAL;
  style->halign   = UHalign::LEFT;
  style->valign   = UValign::FLEX;
  style->hspacing = 1;
  style->vspacing = 1;
  style->local.padding.set(0,0);
  return style;
}

UStyle* UVbox::createStyle() {
  UStyle* style = new UStyle();
  style->textSeparator  = &ustr("\n");
  style->orient   = UOrient::VERTICAL;
  style->halign   = UHalign::FLEX;
  style->valign   = UValign::TOP;
  style->hspacing = 1;
  style->vspacing = 1;
  //style->local.vmargin  = 0;  style->local.hmargin  = 0;
  style->local.padding.set(0,0);
  return style;
}
*/
// ==================================================== [Ubit Toolkit] =========

UStyle* UBar::createStyle() {
  UStyle* s = new UStyle;
  s->textSeparator = &ustr("\t");
  s->orient = UOrient::HORIZONTAL;
  s->halign = UHalign::LEFT;
  s->valign  = UValign::FLEX;
  s->hspacing = 4;
  s->vspacing = 5;
  s->local.padding.set(5,2);
  s->local.border = &UBorder::shadowOut;
  return s;
}

UStyle* UStatusbar::createStyle() {
  UStyle* s = new UStyle();
  s->textSeparator  = &ustr("\t");
  s->orient = UOrient::HORIZONTAL;
  s->halign = UHalign::LEFT;
  s->valign  = UValign::FLEX;
  s->hspacing = 2;
  s->vspacing = 2;
  s->local.padding.set(6,2); //tb,lr
  s->local.border = &UBorder::shadowIn;
  return s;
}

// ==================================================== [Ubit Toolkit] =========

UStyle* UFlowbox::createStyle() {
  UStyle& s = *new UStyle();
  s.viewStyle = &UFlowView::style;
  s.textSeparator = new UStr("\n");
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::FLEX;
  s.valign = UValign::FLEX;
  s.hspacing = 1;
  s.vspacing = 1;
  s.local.padding.set(0,0);
  
  // width does not depend on content, but height does! otherwise the box could
  // not grow vertically when children are added or resized (for instance the
  // height of a text flow could not grow)
  s.local.size.width = -1; // -1 means "keep initial size"
  return &s;
}

UFlowbox::UFlowbox(UArgs a): UBox(a) {
  emodes.IS_TEXT_SELECTABLE = true;
}

// ==================================================== [Ubit Toolkit] =========

UStyle* UCardbox::createStyle() {
  UStyle& s = *new UStyle();
  s.viewStyle = &UPaneView::style;
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::FLEX;
  s.valign = UValign::FLEX;
  s.hspacing = 1;
  s.vspacing = 1;
  s.local.padding.set(0,0);
  //surtout ne pas faire cela: les cards n'apparaissent pas a pas celle qui
  //est selectionnee au debut (ou si on est en flex) 
  //s.local.size.width  = -1; // -1 means "keep initial size"
  //s.local.size.height = -1;
  return &s; 
}

UCardbox::UCardbox(UArgs args) : UBox(args), ptabs(new UListbox()) 
{
  ptabs->addAttr(ucall(this, &UCardbox::setSelectedImpl)
                 + UOrient::horizontal + uhflex() + uvflex()
                 + upadding(0,0) + UBackground::none + UBorder::none);
  setAttr(*new UCompositeBorder(utop() + uhcenter() + *ptabs));
}

UCardbox::~UCardbox() {}

UChoice& UCardbox::choice() {return ptabs->choice();}

void UCardbox::setSelectedImpl() {
  int index = ptabs->choice().getSelectedIndex();
  int ix = 0;
  for (UChildIter i = cbegin(); i != cend(); ++i) {
    UBox* card = dynamic_cast<UBox*>(*i);
    if (card) {
      if (ix == index) card->show(true);
      else card->show(false);
      ix++;
    }
  }
}

UCardbox& UCardbox::addCard(UBox& card) {
  addTab(UArgs::none, card);
  return *this;
}

UCardbox& UCardbox::addTab(UArgs title, UBox& card) {
  if (&title != &UArgs::none) ptabs->add(utabbutton(title));
  else ptabs->add(uitem());  
  add(card);
  setSelectedIndex(-1);
  return *this;
}

UBox* UCardbox::getCard(int index) const {
  int ix = 0;
  for (UChildIter i = cbegin(); i != cend(); ++i) {
    UBox* c = dynamic_cast<UBox*>(*i);
    if (c && ix == index) return c;
    ix++;
  }
  return null;
}

int UCardbox::getCardIndex(UBox& card) const {
  int ix = 0;
  for (UChildIter i = cbegin(); i != cend(); ++i) {
    UBox* c = dynamic_cast<UBox*>(*i);
    if (c == &card) return ix;
    ix++;
  }
  return -1;
}

UBox* UCardbox::getSelectedCard() const {
  int index = ptabs->choice().getSelectedIndex();
  if (index >= 0) return getCard(index);
  return null;
}

void UCardbox::setSelectedCard(UBox& card) {
  int ix = getCardIndex(card);
  if (ix >= 0) setSelectedIndex(ix);
}

int UCardbox::getSelectedIndex() const {
  return ptabs->choice().getSelectedIndex();
}

void UCardbox::setSelectedIndex(int index) {
  ptabs->choice().setSelectedIndex(index);
}

//UBox* UCardbox::getTab(int index) const {
//  return ptabs->choice().getSelectableItem(index);
//}
//UBox* UCardbox::getSelectedTab() const {
//  return ptabs->choice().getSelectedItem();
//}

//void UCardbox::setSelectedTab(UBox* tab) {
//  ptabs->choice().setSelectedItem(tab);
//}
/*
 UCardbox& UCardbox::setComboMode(bool cm) {
 UBorder* border = getBorder(true); // true => create border if needed
 //cerr <<"border "<<border<<" " <<border->getSubGroup()<<endl;
 
 if (border->getSubGroup()) border->getSubGroup()->removeAll();
 
 if (cm) {
 ptabs->removeAllAttrs();
 ptabs->addAttr(ptabs->choice());
 border->add(utop() + uhcenter() + ucombobox(*ptabs));
 }
 else {
 ptabs->addAttr(UOrient::horizontal);
 ptabs->addAttr(uhspacing(0));
 ptabs->addAttr(uhmargin(1));
 ptabs->addAttr(uvmargin(1));
 ptabs->addAttr(UBorder::none);
 ptabs->addAttr(UBgcolor::none);
 ptabs->addAttr(uhflex());
 ptabs->addAttr(uvflex());
 border->add(utop() + uhcenter() + *ptabs);
 }
 return *this;
 }
 */
// ==================================================== [Ubit Toolkit] =========
// NB: ex: class UDocbox : public UVbox 

UDocbox::UDocbox(UArgs args) {
  zoom_quantum = 1.166;
  
  ptitlebar = uhbox();
  pscale = new UScale;
  pcontent = uvbox(args);
  pcontent->addAttr(*pscale);
  
  pspane = uscrollpane(true, false);
  pspane->add(*pcontent);
  
  addAttr(UOrient::vertical + utop());
  add(uhflex() + utop() + *ptitlebar + uvflex() + *pspane);
}

UDocbox::~UDocbox() {}

void UDocbox::zoom(float z) {
  *pscale = z;
}

void UDocbox::zoomIn() {
  pscale->mult(zoom_quantum);
}

void UDocbox::zoomOut() {
  pscale->div(zoom_quantum);
}

void UDocbox::setZoomQuantum(float zq) {
  zoom_quantum = zq;
}

bool UDocbox::isIconified() const {
  return pspane->isShown();
}

void UDocbox::iconify(bool state) {
  pspane->show(!state);
  //piconify_btn->select(state);
}

// ==================================================== [Ubit Toolkit] =========

UAlertbox::UAlertbox(UArgs args) {
  add(UOrient::vertical + UBackground::white + ualpha(0.8)
      + uhcenter() + uvcenter()
      + UFont::bold + UFont::large
      + args
      + UOn::action / ushow(*this, false)
      
      + uhbox(uscale(1.5) + UFont::xx_large
              + UPix::ray + UColor::red + " Alert! ")
      + " "
      + " "
      + " "
      + uhflex()  // necessaire sinon affiche un mot par ligne!
      + uflowbox(UFont::x_large + UColor::navy + uhcenter() + message)
      + " "
      + " "
      + " "
      + uhcenter()
      + ubutton(uscale(1.25) + UFont::x_large + ualpha(0.6) 
                + "   OK   " + ushow(*this, false))
      );
}

UStyle* UAlertbox::createStyle() {
  UStyle*s = UBox::createStyle();
  s->orient = UOrient::VERTICAL;
  return s;
}

void UAlertbox::showMsg(const UStr& msg) {
  message.removeAll();
  message.add(ustr(msg));
  update();
  UBox::show();
}

//void UAlertbox::showMsg(const UStr* msg) {if (msg) showMsg(*msg);}

void UAlertbox::showMsg(const char* msg) {
  message.removeAll();
  message.add(ustr(msg));
  update();
  UBox::show();
}

}
