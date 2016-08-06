/************************************************************************
 *
 *  ubackground.cpp: Widget background and transparency.
 *  (see also uattribute.hpp for UAttribute base class)
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
#include <ubit/ubackground.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/upix.hpp>
#include <ubit/uappli.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UAlpha::UAlpha(float v) : UAttr() {val = v;}

UAlpha& UAlpha::set(float v) {
  if (checkConst() || v == val) return *this;
  val = v;
  changed();
  return *this;
}

void UAlpha::update() {
  updateAutoParents(UUpdate::paint);
}

void UAlpha::putProp(UUpdateContext *props, UElem&) {
  props->local.alpha = val;
}

/* ==================================================== [Elc] ======= */

//inline UBackground& ubgcolor(int r, int g, int b);
//obsolete.

UBackground UBackground::none(UColor::none, UCONST);
UBackground UBackground::inherit(UColor::inherit, UCONST);

UBackground UBackground::velin(UPix::velin, UCONST);
UBackground UBackground::metal(UPix::metal, UCONST);

UBackground UBackground::black(UColor::black, UCONST);
UBackground UBackground::white(UColor::white, UCONST);
UBackground UBackground::lightgrey(UColor::lightgrey, UCONST);
UBackground UBackground::grey(UColor::grey, UCONST);
UBackground UBackground::darkgrey(UColor::darkgrey, UCONST);
UBackground UBackground::navy(UColor::navy, UCONST);
UBackground UBackground::lightblue(UColor::lightblue, UCONST);
UBackground UBackground::blue(UColor::blue, UCONST);
UBackground UBackground::red(UColor::red, UCONST);
UBackground UBackground::green(UColor::green, UCONST);
UBackground UBackground::yellow(UColor::yellow, UCONST);
UBackground UBackground::wheat(UColor::wheat, UCONST);
UBackground UBackground::teal(UColor::teal, UCONST);
UBackground UBackground::orange(UColor::orange, UCONST);

UBackground::UBackground() 
: alpha(1.), tiling(true) {}

UBackground::UBackground(const UBackground& b) : 
pcolor(b.pcolor ? new UColor(*b.pcolor) : null), 
//pima(new UIma(*b.pima)), 
pima(b.pima),                                // !!! A REVOIR (dupliquer)
alpha(b.alpha), tiling(b.tiling) {}

UBackground::UBackground(const UColor& c) 
: pcolor(new UColor(c)), alpha(1.), tiling(true) {}

UBackground::UBackground(UIma& i) 
: pima(i), alpha(1.), tiling(true) {} // !!! A REVOIR (dupliquer)

UBackground::UBackground(UColor& c, UConst m) 
: UAttr(m), pcolor(c), alpha(1.), tiling(true) {}    // color is not duplicated

UBackground::UBackground(UIma& i, UConst m) 
: UAttr(m), pima(i), alpha(1.), tiling(true) {}    // image is not duplicated

UBackground::~UBackground() {}  // pour pval

const UColor* UBackground::getColor() const {return pcolor;}
const UIma* UBackground::getIma() const {return pima;}
bool UBackground::isTiled() const {return tiling;}

bool UBackground::operator==(const UBackground& b) {
  return ((pcolor == b.pcolor || (pcolor && b.pcolor && *pcolor == *b.pcolor))
          && pima == b.pima
          && alpha == b.alpha && tiling == b.tiling);
}

UBackground& UBackground::operator=(const UBackground& b) {
  if (checkConst() || *this == b) return *this;
  if (!b.pcolor) pcolor = null;
  else {
    if (pcolor) *pcolor = *b.pcolor; 
    else pcolor = new UColor(*b.pcolor); 
  }
  //pima = new UColor(*b.pima);
  pima = b.pima;                           // !!! A REVOIR (dupliquer)
  alpha = b.alpha;
  tiling = b.tiling;
  changed();
  return *this;
}

UBackground& UBackground::setColor(const UColor& c) {
  if (checkConst() || (pcolor && *pcolor == c)) return *this;
  if (!pcolor) pcolor = new UColor(c); else *pcolor = c;
  changed();
  return *this;
}

UBackground& UBackground::setRgba(const URgba& comps) {
  if (checkConst() || (pcolor && *pcolor == comps)) return *this;
  if (!pcolor) pcolor = new UColor(comps); else pcolor->setRgba(comps);
  changed();
  return *this;
}

UBackground& UBackground::setRbgaF(float r, float g, float b, float a) {
  return setRgba(URgba(r,g,b,a));
}

UBackground& UBackground::setRbgaI(unsigned int r, unsigned int g, 
                                   unsigned int b, unsigned int a) {
  return setRgba(URgba(r,g,b,a));
}
                     
UBackground& UBackground::setNamedColor(const UStr& colname, float a) {
  if (checkConst()) return *this;
  URgba comps;
  bool found = UColor::parseColor(colname.c_str(), comps);
  comps.comps[3] = (unsigned char)a*255; 
  if (found) return setRgba(comps);
  else {
    warning("UBackground::setNamedColor","color name '%s' not found", 
            (colname.empty() ? "null" : colname.c_str()));
    return *this;
  }
}

UBackground& UBackground::setIma(UIma& i, bool t) {
  if (checkConst()) return *this;
  if (pima == &i) return *this;         // !!! A REVOIR (il faut dupliquer)
  pima = i;
  tiling = t;
  changed();
  return *this;
}

UBackground& UBackground::setTiling(bool t) {
  if (checkConst()) return *this;
  if (tiling == t) return *this;
  tiling = t;
  changed();
  return *this;
}

void UBackground::update() {
  updateAutoParents(UUpdate::paint);
}

void UBackground::putProp(UUpdateContext *props, UElem&){
  props->local.background = this;
}



}
