/************************************************************************
 *
 *  uslider.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/uslider.hpp>
#include <ubit/uscrollbar.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/upix.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uboxgeom.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


class USliderRail : public UBox {
public:
  UCLASS(USliderRail)  
  USliderRail(const UArgs& a = UArgs::none) : UBox(a) {}
  static UStyle* createStyle();
};
  
class USliderKnob : public UBox {
public:
  UCLASS(USliderKnob)  
  USliderKnob(const UArgs& a = UArgs::none) : UBox(a) {}
  static UStyle* createStyle();
};

/* ==================================================== ======== ======= */

USliderStyle::USliderStyle() {
  // fix size and avoid useless updates 
  //vstyle.local.size.set(16,UPX, 150,UPX);
  vstyle.local.size.width  = 16|UPX;
  vstyle.local.size.height = 150|UPX;
  vstyle.hspacing = 0;
  vstyle.vspacing = 0;
  vstyle.orient = UOrient::VERTICAL;
  vstyle.halign = UHalign::CENTER;
  vstyle.valign = UValign::FLEX;
  //vstyle.local.padding.left = 1; //sinon impression rail decentre
  
  //local.size.set(150,UPX, 16,UPX);
  local.size.width  = 150|UPX;
  local.size.height = 16|UPX;
  hspacing = 0;
  vspacing = 0;
  orient = UOrient::HORIZONTAL;
  halign = UHalign::FLEX;
  valign = UValign::CENTER;
  //local.padding.top = 1;
}

const UStyle& USliderStyle::getStyle(UUpdateContext* ctx) const {
  //att: par defaut, si pas de ctx, renvoyer HORIZ
  if (ctx && ctx->obj && ctx->obj->isVertical()) return vstyle; 
  else return *this;
}

/* ==================================================== ======== ======= */

USlider& uslider(const UArgs& a) {return *new USlider(a);}

USlider& uslider(UFloat& v, const UArgs& a) {return *new USlider(v, a);}

USlider& uhslider(const UArgs& a) {return *new USlider(a);}

USlider& uvslider(const UArgs& a) {
  return *new USlider(UOrient::vertical + a);
}

USlider::USlider(const UArgs& a) : UBox(a), pvalue(*new UFloat) {
  // UBox(a) avant constructs() pour que isVertical() soit correct
  constructs();
}

USlider::USlider(UFloat& v, const UArgs& a) : UBox(a), pvalue(v) {
  constructs();
}

void USlider::constructs() {
  // action doit etre appele a la fin sur le release
  pvalue->onChange(ucall(this, &USlider::actionCB));

  // the position is a percent of the parent size
  pknob_pos = new UPos(0,0);
  //pknob_pos->setPercentMode(true, true);  // adjusted
  pknob_pos->onChange(ucall(this, &USlider::changeCB));

  pknob_ctrl = (new UPosControl)->setModel(pknob_pos);
  // sinon appel incorrect de changeCB() a cause des setX(), setY() ci-dessous !
  pknob_pos->ignoreChangeCallbacks(true);
  if (isVertical()) {
    pknob_pos->setX(50.|UPERCENT_CTR);
    pknob_ctrl->freezeX(true);
  }
  else {
    pknob_pos->setY(50.|UPERCENT_CTR);
    pknob_ctrl->freezeY(true);
  }
  pknob_pos->ignoreChangeCallbacks(false); // retablir
  
  add(*(prail = createRail()));
  prail->ignoreEvents();

  add(*(pknob = createKnob()));
  pknob->addAttr(*pknob_pos);
  pknob->addAttr(*pknob_ctrl);
  
  addAttr(UOn::mpress / ucall(this, &USlider::gotoPosCB));
}


UBox* USlider::createKnob() {return new USliderKnob();}
UBox* USlider::createRail() {return new USliderRail();}

/* ==================================================== ===== ======= */

void USlider::gotoPosCB(UMouseEvent& e) {
  UView* v = e.getView();
  if (!v) return;

  if (isVertical()) {
    float y = e.getY() * 100.0 / v->getHeight();
    pknob_pos->setY(y | UPERCENT_CTR);
  }
  else {
    float x = e.getX() * 100.0 / v->getWidth();
    pknob_pos->setX(x | UPERCENT_CTR);
 }
}

void USlider::actionCB(UEvent& notused) {  //UNodeEvent
  // cette verif doit etre dans setCB, pas dans set() car pvalue peut etre
  // modifiee directement si partagee par d'autres objets (via value() etc)
  if (*pvalue < 0.) *pvalue = 0.;
  else if (*pvalue > 100.) *pvalue = 100.;  // entre 0 et 100
  
  if (isVertical()) pknob_pos->setY(*pvalue | UPERCENT_CTR); 
  else pknob_pos->setX(*pvalue | UPERCENT_CTR);
  
  UEvent e2(UOn::action, this, pknob);    //UElemEvent
  fire(e2);
}

void USlider::changeCB(UEvent& notused) {  //UNodeEvent
  if (isVertical()) *pvalue = pknob_pos->getY().val; 
  else *pvalue = pknob_pos->getX().val;
  
  UEvent e2(UOn::change, this, pknob);  //UElemEvent
  fire(e2);
}

/* ==================================================== [(c)Elc] ======= */

struct USliderRailStyle : public UStyle {
  UStyle vstyle;
  USliderRailStyle();
  virtual const UStyle& getStyle(UUpdateContext* ctx) const {
    //att: par defaut, si pas de ctx, renvoyer HORIZ
    if (ctx && ctx->parent_ctx && ctx->parent_ctx->obj
        && ctx->parent_ctx->obj->isVertical())
      return vstyle; 
    else return *this;
  }
};

USliderRailStyle::USliderRailStyle() {
  vstyle.orient = UOrient::INHERIT;
  //vstyle.local.size.setWidth(5);
  vstyle.local.size.width = 5|UPX;
  vstyle.local.border = &UBorder::shadowIn;
  
  orient = UOrient::INHERIT;
  //local.size.setHeight(5);
  local.size.height = 5|UPX;
  local.border = &UBorder::shadowIn;
}

UStyle* USliderRail::createStyle() {
  return new USliderRailStyle;
}

/* ==================================================== [(c)Elc] ======= */

struct USliderKnobStyle : public UStyle {
  UStyle vstyle;
  USliderKnobStyle();
  virtual const UStyle& getStyle(UUpdateContext* ctx) const {
  //att: par defaut, si pas de ctx, renvoyer vertical
    if (ctx && ctx->parent_ctx && ctx->parent_ctx->obj
        && ctx->parent_ctx->obj->isVertical())
      return vstyle; 
    else return *this;
  }
};

USliderKnobStyle::USliderKnobStyle() {
  orient = UOrient::INHERIT;
  halign = UHalign::LEFT;
  valign = UValign::TOP;
  hspacing = 0;
  vspacing = 0;
  local.content = new UElem(UPix::hslider);

  vstyle.orient = UOrient::INHERIT;
  vstyle.halign = UHalign::LEFT;
  vstyle.valign = UValign::TOP;
  vstyle.hspacing = 0;
  vstyle.vspacing = 0;
  vstyle.local.content = new UElem(UPix::vslider);
}

UStyle* USliderKnob::createStyle() {
  return new USliderKnobStyle();
}

/* ==================================================== ===== ======= */
#if EXXXX

/* URangeSlider gadget.
*  UOn::change callbacks are activated when the scale value is changed.
*/
 UClass* URangeSlider::getClass() const {
   static UClass* c = UAppli::addElementClass("urangeslider", createStyle);
   return c;
 }

class URangeSlider: public USlider {
public:
  URangeSlider(const UArgs& = UArgs::none);
  URangeSlider(const UOrient&, const UArgs& = UArgs::none);
  URangeSlider(UFloat& value1, UFloat& value2, 
               const UOrient&, const UArgs& = UArgs::none);
  /**< constructors.
    * - default orientation is UOrient::Vertical
  * - 'orient' can be UOrient::horizontal or UOrient::vertical.
    */
  
  UFloat& value2() {return *pvalue2;}
  virtual float getValue2() const;
  ///< returns the 2nd value (a float between 0 and 100).
  
  virtual void setValue2(float percent);
  /**< changes the 2nd value (a float between 0 and 100).
    * Note: UOn::change callbacks are activated when the value is changed.
    */
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // implementation
  
  virtual UClass* getClass() const;
  static UStyle* createStyle();
  
  virtual UBox* createSlider(bool vertical);
  virtual UBox* createSlider2(bool vertical);
  
protected:
    virtual float getPercent(UEvent& e, 
                             UView *slider_view, UView *railbox_view,
                             int _delta_mouse);
  virtual void setValueImpl(float newval, bool upd);
  
  virtual void pressSlider2(UEvent&);
  virtual void releaseSlider2(UEvent&);
  virtual void dragSlider2(UEvent&);
  
private:
  uptr<UFloat> pvalue2; 
  uptr<UBox> pslider2;
  UPos slider_pos2;
  int delta_mouse2;
  void constructs2();
};
#endif

}
