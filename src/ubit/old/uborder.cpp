/************************************************************************
 *
 *  uborder.cpp: Border Attribute for UBox containers
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
#include <ubit/ucall.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uview.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uon.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

#define DARK  UColor::darkgrey
#define LIGHT UColor::lightgrey

UBorder UBorder::none(NONE, UColor::inherit, UColor::inherit, 0, 0, UCONST);
UBorder UBorder::empty(NONE, UColor::inherit, UColor::inherit, 1, 1, UCONST);
UBorder UBorder::line(LINE, DARK, LIGHT, 1, 1, UCONST);
UBorder UBorder::shadowOut(+SHADOW, DARK, LIGHT,1,1,UCONST);
UBorder UBorder::shadowIn(-SHADOW, DARK, LIGHT, 1,1,UCONST);
UBorder UBorder::etchedOut(+ETCHED, DARK, LIGHT, 2,2,UCONST);
UBorder UBorder::etchedIn(-ETCHED, DARK, LIGHT, 2,2,UCONST);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBorder::UBorder(int d) : padding(1,1) {
  constructs(d, UColor::white, UColor::black);
}

UBorder::UBorder(int d, const UColor& c, const UColor& bgc, float horiz, float vert) 
: padding(horiz,vert) {
  constructs(d, c, bgc);
}

UBorder::UBorder(int d, const UColor& c, const UColor& bgc, float horiz, float vert, UConst m)
: UAttr(m), padding(horiz,vert) {
  constructs(d, c, bgc);
}

UBorder::UBorder(const UBorder& b) : padding(b.padding) {
  constructs(b.decoration, *b.pcolor, *b.pbgcolor);
}

UBorder::~UBorder() {}

void UBorder::constructs(int d, const UColor& c, const UColor& bgc) {
  decoration = d;
  is_overlaid = is_rounded = false;
  pcolor = c;
  pbgcolor = bgc;
}

UBorder& UBorder::operator=(const UBorder& b) {
  if (checkConst()) return *this;
  // !! tester egalite !!
  constructs(b.decoration, *b.pcolor, *b.pbgcolor);
  padding = b.padding;
  changed(true);
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

URoundBorder::URoundBorder(int d) : UBorder(d), arc_w(5), arc_h(5) {
  is_rounded = true;
}

URoundBorder::URoundBorder(int d, const UColor& c, const UColor& bgc,
             float w, float h, float aw, float ah)
: UBorder(d, c, bgc, w, h), arc_w(aw), arc_h(ah) {
  is_rounded = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class UBorderContent : public UElem {
public:
  UCLASS(UBorderContent)
  UBorderContent(const UArgs& a = UArgs::none) : UElem(a) {}
  virtual int getDisplayType() const {return BORDER;}   // redefined
};

UCompositeBorder::UCompositeBorder() 
: UBorder(+SHADOW, UColor::none, UColor::none, 1,1) {
}

UCompositeBorder::UCompositeBorder(const UArgs& a) 
: UBorder(+SHADOW, UColor::none, UColor::none, 1,1) {
  pchildren = new UBorderContent(a);
}

/*
UCompositeBorder& UCompositeBorder::addChildren(const UArgs& a) {
  if (!pchildren) pchildren = new UBorderContent(a);
  else pchildren->add(a);
  return *this;
}

UCompositeBorder& UCompositeBorder::addChildren(bool superimposed, const UArgs& a) {
  is_overlaid = superimposed;
  if (!pchildren) pchildren = new UBorderContent(a);
  else pchildren->add(a);
  return *this;
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UBorder& UBorder::setDecoration(int decor) {
  if (checkConst()) return *this;
  decoration = decor;
  switch(decor) {
    case NONE: padding.set(0,0); break;
    case ETCHED: padding.set(2,2); break;
    default: padding.set(1,1); break;
  }
  return *this;
}

UBorder& UBorder::setColor(const UColor& c) {
  if (checkConst() || pcolor==&c || *pcolor==c) return *this;
  pcolor = c;
  return *this;
}

UBorder& UBorder::setBgcolor(const UColor& c) {
  if (checkConst() || pbgcolor==&c || *pbgcolor==c) return *this;
  pbgcolor = c;
  return *this;
}

/*
 UBorder& UBorder::setThickness(float t) {
 thickness = t;
 return *this;
 }
 */

UBorder& UBorder::setPadding(const UPaddingSpec& p) {
  if (checkConst()) return *this;
  padding = p;
  return *this;
}

UBorder& UBorder::setOverlaid(bool state) {
  if (checkConst()) return *this;
  is_overlaid = state; 
  return *this;
}

void UBorder::update() {
  updateAutoParents(UUpdate::LAYOUT_PAINT);
}

void UBorder::putProp(UUpdateContext* props, UElem&) {
  // a defaut de mieux : ne pas ecraser une definition de border !!
  // s'il contient des elements (typiquement un scrollpane)
  if (!props->local.border || !props->local.border->getSubGroup())
    props->local.border = this;
}

void UCompositeBorder::putProp(UUpdateContext* props, UElem&) {
  props->local.border = this;  // ecrase si existe
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UBorder::getSize(const UUpdateContext&, UPaddingSpec& _m) const {
  _m = padding;
}

void UBorder::paint(UGraph& g, const UUpdateContext& parp, const URect& r) const {
  // new: fait avant:
  // vd.margin = margin;
  // if (!vd.can_paint) return;   // si rien a peindre on s'arrete la
  
  // !ATT: il peut y avoir des scrollbars meme si border == none
  if (decoration == NONE) return;
  
  // invert is_active when the btn is pressed
  bool is_active;
  if (parp.obj->isSelected())
    is_active = (parp.obj->getInterState() != UOn::ARMED);
  else 
    is_active = (parp.obj->getInterState() == UOn::ARMED);
  
  const UColor *_color;
  if (pcolor->equals(UColor::inherit)) _color = parp.color;
  else _color = pcolor;
  
  const UColor *_bgcolor;
  if (pbgcolor->equals(UColor::inherit)) _bgcolor = parp.bgcolor;
  else _bgcolor = pbgcolor;
  
  const UColor *bg = null, *fg = null;
  
  if (decoration > NONE) {  // OUT (normal case)
    if (is_active) {bg = _color; fg = _bgcolor;}
    else {bg = _bgcolor; fg = _color;}   // cas normal
  }
  else {                  // IN (inverted shadows)
    if (is_active) {bg = _bgcolor; fg = _color;}
    else {bg = _color; fg = _bgcolor;}
  }

  paintDecoration(g, r, *parp.obj, *fg, *bg);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UBorder::paintDecoration(UGraph& g, const URect& r, UElem& obj, 
                              const UColor& fg, const UColor& bg) const {
  float x1 = r.x;
  float y1 = r.y;
  float x2 = r.x + r.width - 1;
  float y2 = r.y + r.height -1;
  
  switch (abs(decoration)) {
    case SHADOW:
      // out: fg = bottom+right and bg = top+left
      // in:  invert
      g.setColor(fg);
      g.drawLine(x1+1, y2, x2, y2); // bottom
      g.drawLine(x2, y1+1, x2, y2); // right
      g.setColor(bg);
      g.drawLine(x1, y1, x2-1, y1); // top
      g.drawLine(x1, y1, x1, y2-1); // left
      break;
      
    case ETCHED:
      g.setColor(fg);
      g.drawRect(x1+1, y1+1, r.width-2, r.height-2);
      g.setColor(bg);
      g.drawRect(x1, y1, r.width-2,  r.height-2);
      break;
      
    case LINE:
      g.setColor(fg);
      g.drawRect(x1, y1, r.width-1, r.height-1);
      break;
      
      /*
    case BIGSHADOW:{
      float prev_thickness = g.getWidth();
      g.setColor(*fg);
      g.setWidth(thickness);
      g.drawLine(x1 + 10+4, y2, x2, y2); // bottom
      g.drawLine(x2, y1 + 10+4, x2, y2 + 4); // right
      g.setWidth(prev_thickness);
    } break;
       */
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void URoundBorder::paintDecoration(UGraph& g, const URect& r, UElem& obj, 
                                   const UColor& fg, const UColor& bg) const {
  float x1 = r.x;
  float y1 = r.y;
  float x2 = r.x + r.width - 1;
  float y2 = r.y + r.height -1;
  
  switch (abs(decoration)) {
    case SHADOW:
      // out: fg = bottom+right and bg = top+left
      // in:  invert
      g.setColor(fg);
      g.drawLine(x1+1, y2, x2, y2); // bottom
      g.drawLine(x2, y1+1, x2, y2); // right
      g.setColor(bg);
      g.drawLine(x1, y1, x2-1, y1); // top
      g.drawLine(x1, y1, x1, y2-1); // left
      break;
      
    case ETCHED:
      g.setColor(fg);
      g.drawRoundRect(x1+1, y1+1, r.width-2, r.height-2, arc_w, arc_h);
      g.setColor(bg);
      g.drawRoundRect(x1, y1, r.width-2,  r.height-2, arc_w, arc_h);
      break;
      
    case LINE:
      g.setColor(fg);
      g.drawRoundRect(x1, y1, r.width-1, r.height-1, arc_w, arc_h);
      break;
  }
}

}

