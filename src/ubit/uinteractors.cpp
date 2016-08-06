/************************************************************************
 *
 *  uinteractors.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/ubit.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

struct USeparStyle : public UStyle {
  USeparStyle();
  virtual const UStyle& getStyle(UUpdateContext*) const;
private:
  UStyle vstyle;
};

const UStyle& USeparStyle::getStyle(UUpdateContext* ctx) const {
  UBox* parent = null;
  if (ctx && (parent = ctx->getBoxParent()) && parent->isVertical())
    return vstyle; 
  else return *this;
}

USeparStyle::USeparStyle() {
  orient = UOrient::HORIZONTAL;
  halign = UHalign::FLEX;
  valign = UValign::CENTER;
  local.padding.set(0, 0);
  local.border = &UBorder::shadowIn;
  // len = -1 means "keep initial size"
  // UNRESIZABLE prevents a uhflex/uvflex to resize the widget
  local.size.width = ULength(-1, UPX, USize::UNRESIZABLE);

  vstyle.orient = UOrient::VERTICAL;
  vstyle.halign = UHalign::CENTER;
  vstyle.valign = UValign::FLEX;
  vstyle.local.padding.set(0, 0);
  vstyle.local.border = &UBorder::shadowIn;
  // len = -1 means "keep initial size"
  // UNRESIZABLE prevents a uhflex/uvflex to resize the widget
  vstyle.local.size.height = ULength(-1, UPX, USize::UNRESIZABLE);
}

UStyle* USepar::createStyle() {
  return new USeparStyle();
}

USepar::USepar() : UBox() {
  //setCloseMenuMode(ElementModes::CANNOT_CLOSE_MENU);
}

/* ==================================================== [Elc] ======= */

UStyle* ULabel::createStyle() {
  UStyle* style = new UStyle();
  style->orient = UOrient::HORIZONTAL;
  style->halign = UHalign::LEFT;
  style->valign = UValign::CENTER;
  style->hspacing = 1;
  style->vspacing = 1;
  style->setPadding(2, 2);
  return style;
}

ULabel::ULabel(int nbchars, UArgs a) : UBox(a) {
  obtainAttr<USize>().setWidth(nbchars|UEX);
}

/* ==================================================== [Elc] ======= */

UStyle* UTextfield::createStyle() {
  UStyle& s = *new UStyle();
  s.setColors(UColor::inherit, UColor::white);
  //s.setBgcolors(UColor::none, UColor::blue);
  s.setBgcolors(UColor::white, UColor::blue);
  s.setColor(UOn::DISABLED, UColor::disabled);
  s.setBgcolor(UOn::DROP_ENTERED,UColor::blue);
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::LEFT;
  s.valign = UValign::CENTER;
  s.hspacing = 0;
  s.vspacing = 3;
  s.setPadding(3,2); 
  static UBorder* border = 
  new URoundBorder(UBorder::LINE, UColor::darkgrey, UColor::white,1,1,15,15);
  s.setBorder(border);
  //s.local.border = &UBorder::shadowIn;
  
  //style->cursor = &UCursor::text;  // done by UEdit
  s.setSize(USize::INITIAL, USize::INITIAL);
  return &s;
}

UTextfield::UTextfield(UArgs a) : UBox(a) {
  obtainAttr<UEdit>();
  emodes.IS_TEXT_SELECTABLE = true;     // also added by uedit
  emodes.HAS_CURSOR = true;
  disableMenuClosing();
}

UTextfield::UTextfield(int nbchars, UArgs a) : UBox(a) {
  obtainAttr<USize>().setWidth(nbchars|UEX);
  obtainAttr<UEdit>();
  emodes.IS_TEXT_SELECTABLE = true;     // also added by uedit
  emodes.HAS_CURSOR = true;
  disableMenuClosing();
}

UEdit& UTextfield::edit() {
  return *getAttr<UEdit>();
}

UTextfield& UTextfield::setEditable(bool state) {
  edit().setEditable(state); return *this;
}

bool UTextfield::isEditable() const {
  return getAttr<UEdit>()->isEditable();
}

/* ==================================================== [Elc] ======= */

UStyle* UTextarea::createStyle() {
  UStyle& s = *new UStyle();
  s.viewStyle = &UFlowView::style;
  s.textSeparator = new UStr("\n");
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::FLEX;
  s.valign = UValign::CENTER;
  
  s.setColors(UColor::inherit, UColor::white);
  s.setBgcolors(UColor::white, UColor::blue);
  s.setColor(UOn::DISABLED, UColor::disabled);
  s.setBgcolor(UOn::DROP_ENTERED,UColor::blue);
  s.hspacing = 0;
  s.vspacing = 3;
  s.local.padding.set(3,2);
  s.local.border = &UBorder::shadowIn;
  //style->cursor = &UCursor::text;  // fait par UEdit
  
  // width does not depend on content, but height does! otherwise the box
  // could not grow vertically when children are added or resized (for instance
  // the height of a text flow could not grow)
  //s.local.size.width = -1;   // len = -1 means "keep initial size"
  s.setSize(USize::INITIAL, UAUTO);
  return &s;
}

/* ==================================================== ===== ======= */

class UButtonStyle : public UStyle {
public:
  UStyle menu, bar, tab;
  UButtonStyle();
  virtual const UStyle& getStyle(UUpdateContext*) const;
};

UButtonStyle::UButtonStyle() {
  orient = UOrient::HORIZONTAL;
  halign = UHalign::LEFT;
  valign = UValign::CENTER;
  hspacing = 2;
  vspacing = 2;
  setPadding(2, 2);
  local.border = &UBorder::shadowOut;
  /*
  setAttributes(UOn::select/UColor::grey
                  + UOn::enter/UColor::lightgrey
                  + UOn::arm/UColor::grey);
   */
  setBgcolors(UColor::none, UColor::grey);
  setBgcolor(UOn::ENTERED, UColor::lightgrey);
  setBgcolor(UOn::ARMED, UColor::grey);
  setBgcolor(UOn::DROP_ENTERED, UColor::grey);

  tab = *this;
  tab.halign = UHalign::CENTER;
  tab.valign = UValign::CENTER;
  tab.setPadding(2, 4);
  tab.setColors(UColor::black, UColor::white);
  tab.setBgcolors(UColor::none, UColor::blue);
  
  menu = *this;
  menu.setPadding(10,4);
  menu.local.border = null;
  menu.setBgcolors(UColor::none, UColor::grey);
  menu.setColor(UOn::ENTERED, UColor::white);
  menu.setBgcolor(UOn::ENTERED, UColor::blue);
  menu.setColor(UOn::ARMED, UColor::white);  
  menu.setBgcolor(UOn::ARMED, UColor::blue);
  menu.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  
  bar = *this;
  bar.orient = UOrient::VERTICAL;
  bar.halign = UHalign::CENTER;
  bar.valign = UValign::CENTER;
  bar.setPadding(5,3);
  bar.local.border = null;
  bar.setBgcolors(UColor::none, UColor::grey);
  bar.setBgcolor(UOn::ENTERED, UColor::lightgrey);
  bar.setBgcolor(UOn::ARMED, UColor::grey);
  bar.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
};

const UStyle& UButtonStyle::getStyle(UUpdateContext* ctx) const {
  if (!ctx) return *this;
  
  UButton* btn = dynamic_cast<UButton*>(ctx->getObj());
  if (btn->button_type != UButton::AUTO) {
    switch (btn->button_type) {
      case UButton::TAB:  return tab; break;
      case UButton::MENU: return menu; break;
      case UButton::BAR:  return bar; break;
      default: return *this; break;
    }
  }
  else {
    UBox* parent = null;
    if ((parent = ctx->getBoxParent())) {
      // faudrait separer les 2 cas et rajouter listbox
      if (dynamic_cast<const UMenu*>(parent) || dynamic_cast<const UMenubar*>(parent))
        return menu;
      else if (dynamic_cast<const UBar*>(parent))
        return bar;
    }
  }
  return *this;   // tous les autres cas
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStyle* UButton::createStyle() {return new UButtonStyle;}

UButton::UButton(UArgs a) {
  button_type = AUTO;
  emodes.IS_ARMABLE = true;
  emodes.IS_CROSSABLE = true;
  // this object auto opens dialog children because it is ARMable
  // !att: pour que ca marche faut ajouter a APRES emodes.IS_ARMABLE = true !
  add(a);
}

UButton::UButton(int btype, UArgs a) {
  button_type = btype;
  emodes.IS_ARMABLE = true;
  emodes.IS_CROSSABLE = true;
  // this object auto opens dialog children because it is ARMable
  // !att: pour que ca marche faut ajouter a APRES emodes.IS_ARMABLE = true !
  add(a);
}

UButton& uflatbutton(UArgs a) {
  UButton* b = new UButton;
  b->setAttr(UBorder::line);    // !!! A REVOIR avec styles 
  b->add(a);
  return *b;
}

UButton& utabbutton(UArgs a) {
  return *new UButton(UButton::TAB, a);
}

/* Tab Button widget (for tabbed panes, @see UCardbox).
class UTabbutton: public UButton {
public:
  UCLASS("utabbutton", UTabbutton, new UTabbutton)
  UTabbutton(const UArgs& a = UArgs::none);
  static UStyle* createStyle();
};

 * Flat Button gadget.
 * has a "flat" square border
 * See UButton.
 *
class UFlatbutton: public UButton {
public:
  UCLASS("uflatbutton", UFlatbutton, new UFlatbutton)
  UFlatbutton(const UArgs& a = UArgs::none);
    static UStyle* createStyle();
};
*/

/* ==================================================== [Elc] ======= */

UStyle* UItem::createStyle() {
  UStyle& s = *new UStyle();
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::LEFT;
  s.valign = UValign::FLEX;
  s.hspacing = 2;
  s.hspacing = 2;
  s.setPadding(1, 1);
  /*
   s.setAttributes(UOn::select/UColor::white
   + UOn::select/UBackground::blue
   + UOn::arm/UColor::white
   + UOn::arm/UBackground::blue);
   + UOn::drop/UColor::white
   + UOn::drop/UBackground::blue);
   */
  s.setColors(UColor::inherit, UColor::white);
  s.setBgcolors(UColor::none, UColor::blue);
  s.setColor(UOn::ARMED, UColor::white);
  s.setBgcolor(UOn::ARMED, UColor::blue);
  s.setColor(UOn::DROP_ENTERED, UColor::white);
  s.setBgcolor(UOn::DROP_ENTERED, UColor::blue);
  return &s;
}

UItem::UItem(UArgs a): UBox() {
  emodes.IS_ARMABLE = true;
  // this object auto opens dialog children because it is ARMable
  // !att: pour que ca marche faut ajouter a APRES!
  add(a);
}

/* ==================================================== ===== ======= */

UStyle* ULinkbutton::createStyle() {
  UStyle& s = *new UStyle;
  s.orient = UOrient::HORIZONTAL;
  s.halign = UHalign::LEFT;
  s.valign = UValign::CENTER;
  s.hspacing = 1;
  s.vspacing = 1;
  s.local.padding.set(1, 1);
  s.local.border = null;
  s.cursor = &UCursor::hand;
  s.font = &UFont::underline;
  s.setColors(UColor::navy, UColor::red);
  s.setBgcolors(UColor::none, UColor::none);  
  s.setColor(UOn::DISABLED, UColor::disabled);
  s.setBgcolor(UOn::ENTERED, UColor::grey);
  s.setColor(UOn::ARMED, UColor::red);
  s.setColor(UOn::ACTIONED, UColor::red);
  s.setColor(UOn::DROP_ENTERED, UColor::blue);
  return &s;
}

ULinkbutton::ULinkbutton(UArgs a) : UButton(a) {
  emodes.HAS_CURSOR = true; // le style suffit pas !
}

UStr ULinkbutton::getHRef() const {        // pas tres efficace!
  UStr val;
  getAttrValue(val,"href"); 
  return val;
}

/* ==================================================== [Elc] ======= */

UStyle* UCheckbox::createStyle() {
  UButtonStyle& s = *new UButtonStyle;
  s.orient = s.menu.orient = s.bar.orient = UOrient::HORIZONTAL;
  s.local.border = s.menu.local.border = s.bar.local.border = null;
  s.local.content = s.menu.local.content = s.bar.local.content = new UElem(USymbol::check);
  s.setBgcolors(UColor::none, UColor::none);
  s.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  s.bar.setBgcolors(UColor::none, UColor::none);
  s.bar.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  s.menu.setBgcolors(UColor::none, UColor::none);
  s.menu.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  return &s;
}

UStyle* URadiobutton::createStyle() {
  UButtonStyle& s = *new UButtonStyle;
  s.orient = s.menu.orient = s.bar.orient = UOrient::HORIZONTAL;
  s.local.border = s.menu.local.border = s.bar.local.border = null;
  s.local.content = s.menu.local.content = s.bar.local.content = new UElem(USymbol::radio);
  s.setBgcolors(UColor::none, UColor::none);
  s.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  s.bar.setBgcolors(UColor::none, UColor::none);
  s.bar.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  s.menu.setBgcolors(UColor::none, UColor::none);
  s.menu.setBgcolor(UOn::DROP_ENTERED, UColor::grey);
  return &s;
}

UCheckbox::UCheckbox(UArgs a) : UButton(a) {
  emodes.IS_SELECTABLE = true;
  emodes.IS_CROSSABLE = false;
}

URadiobutton::URadiobutton(UArgs a) : UButton(a) {
  emodes.IS_SELECTABLE = true;
  emodes.IS_CROSSABLE = false;
}

/* ==================================================== [Elc] ======= */

USpinbox::USpinbox(UArgs a) : 
pvalue(*new UInt(0)),
pstr(*new UStr),
increment(1) {
  constructs(a);
}

USpinbox::USpinbox(UInt& _value, UArgs a) : 
pvalue(_value),
pstr(*new UStr),
increment(1) {
  constructs(a);
}

UStyle* USpinbox::createStyle() {
  return UBox::createStyle();
}

void USpinbox::constructs(const UArgs& a) {
  ptextfield = new UTextfield;
  ptextfield->addAttr(UOn::action / ucall(this, 0, &USpinbox::updateValue)
                      + UOn::leave  / ucall(this, 0, &USpinbox::updateValue));
  ptextfield->add(a); // ??
  ptextfield->add(*pstr);

  pstr->setInt(*pvalue);
  pstr->append("        ");

  // value change -> mise a jour de la string
  pvalue->onChange(ucall(this, &USpinbox::changed));

  UBox& up_btn = ubox
    (USymbol::up + UOn::arm/ucall(this,+1,&USpinbox::updateValue));

  UBox& down_btn = ubox
    (USymbol::down + UOn::arm/ucall(this,-1,&USpinbox::updateValue));

  //up_btn.setAutoRepeat(true);
  //down_btn.setAutoRepeat(true);

  add(UOrient::horizontal 
      + uhflex() + ptextfield
      + uright() + uvbox(uscale(0.95) + uvcenter() + uhcenter() + up_btn + down_btn)
      );
}

/* ==================================================== ======== ======= */

void USpinbox::setValue(int v) {*pvalue = v;}
void USpinbox::setIncrement(int v) {increment = v;}

void USpinbox::updateValue(int dir) {
  if (dir == 0) *pvalue = *pstr;  // transform string to num value
  // ajouter ou retrancher increment 
  else if (dir > 0) *pvalue += increment;
  else if (dir < 0) *pvalue -= increment;
}

void USpinbox::changed() {
  pstr->setInt(*pvalue);  // reset string format
  UEvent e1(UOn::change, this, pvalue);  //UElemEvent
  fire(e1);

  UEvent e2(UOn::action, this, pvalue);  //UElemEvent
  fire(e2);
}

}
