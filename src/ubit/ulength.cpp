/************************************************************************
 *
 *  ugeom.cpp: Geometry
 *  Ubit GUI Toolkit - Version 6
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
#include <ubit/ubit_features.h>
#include <ubit/ulength.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ufontImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

const UUnit 
UPX(UUnit::PX),
UEM(UUnit::EM),
UEX(UUnit::EX),
UPERCENT(UUnit::PERCENT),
UPERCENT_CTR(UUnit::PERCENT_CTR),
UIN(UUnit::IN),
UCM(UUnit::CM),
UMM(UUnit::MM),
UPT(UUnit::PT),
UPC(UUnit::PC);

const ULength
UAUTO(0., UUnit(UUnit::AUTO)),
UIGNORE(0., UUnit(UUnit::IGNORE));

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ostream& operator<<(ostream& s, const ULength& l) {
  return (s << l.toString());
}

bool ULength::operator==(const ULength& l) const {
  return val == l.val && unit.type == l.unit.type && modes.val == l.modes.val;
}

bool ULength::operator!=(const ULength& l) const {
  return val != l.val || unit.type != l.unit.type || modes.val != l.modes.val;
} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ULength::ULength(const UStr& spec) : val(0), unit(UPX), modes(0) {
  set(spec);
}

ULength& ULength::set(const UStr& spec) {
  val = 0.;
  unit = UPX;
  modes.val = 0;
  
  if (spec.empty()) {
    UAppli::error("ULength:","empty textual specification");
    return *this;
  }
  
  const char* u = null;
  spec.scanValue(val, u);
  return setUnit(u);
}

ULength& ULength::setUnit(const char* spec) {
  if (!spec || !*spec) {
    unit.type = UUnit::PX;
    return *this;
  }
  
  char u0 = tolower(spec[0]);
  char u1 = tolower(spec[1]);
  
  switch (u0) {
    case 'a':
      if (u1 == 'u' || u1 == 0) unit.type = UUnit::AUTO;
      else goto ERROR;
      break;
    case 'p':
      if (u1 == 'x') unit.type = UUnit::PX;
      else if (u1 == 't') unit.type = UUnit::PT;
      else if (u1 == 'c') unit.type = UUnit::PC;
      else goto ERROR;
      break;
    case '%':
      if (u1 == 0) unit.type = UUnit::PERCENT;
      else goto ERROR;
      break;
    case 'e':
      if (u1 == 'x') unit.type = UUnit::EX;
      else if (u1 == 'm') unit.type = UUnit::EM;
      else goto ERROR;
      break;
    case 'i':
      if (u1 == 'n') unit.type = UUnit::IN;
      else goto ERROR;
      break;
    case 'm':
      if (u1 == 'm') unit.type = UUnit::MM;
      else goto ERROR;
      break;
    case 'c':
      if (u1 == 'm') unit.type = UUnit::CM;
      else goto ERROR;
      break;
    default:
      goto ERROR;
      break;
  }
  return *this;
  
ERROR: 
  UAppli::error("ULength::setUnit","invalid unit");
  return *this;
}


float ULength::toPixels(UDisp* disp, const UFontDesc& fd, 
                        float view_len, float parview_len) const {
  float pixlen = 0;
  switch (unit.type) {
    //case ULength::Unit::AUTO:
      //...;
      //break
    case UUnit::PX:
      pixlen = val;
      break;
    case UUnit::PERCENT:
      pixlen = val * parview_len / 100.; // percentages of the parent size 
      break;
    case UUnit::PERCENT_CTR:
      // the size of this view is removed from the parent size
      pixlen = val * (parview_len - view_len) / 100.;
      break;
    case UUnit::EX:
      pixlen = val * fd.actual_size * 0.5 * disp->PT_TO_PX;
      break;      
    case UUnit::EM:      
      //pixlen = val * UFontMetrics(fd, disp).getHeight();
      pixlen = val * fd.actual_size * disp->PT_TO_PX;
      break;
    case UUnit::IN:
      pixlen = val * disp->IN_TO_PX;
      break;
    case UUnit::MM:
      pixlen = val * disp->MM_TO_PX;
      break;      
    case UUnit::CM:
      pixlen = val * disp->CM_TO_PX;
      break;
    case UUnit::PT:
      pixlen = val * disp->PT_TO_PX;
      break;
    case UUnit::PC:
      pixlen = val * disp->PC_TO_PX;
      break;
    default:
      UAppli::error("ULength::toPixels","invalid unit");
      return 0;
  }
  
  if (modes.val) return parview_len - view_len - pixlen;
  else return pixlen;
} 


UStr ULength::toString() const {
  UStr s;
  s.setFloat(val);

  switch (unit.type) {
    case UUnit::AUTO:
      s = "auto";
      break;
     case UUnit::PX:
      s &= "px";
      break;
    case UUnit::PERCENT:
      s &= "%";
      break;
    case UUnit::PERCENT_CTR:
      s &= "%c";
      break;
    case UUnit::EX:
      s &= "ex";
      break;      
    case UUnit::EM:      
      s &= "em";
      break;
    case UUnit::IN:
      s &= "in";
      break;
    case UUnit::MM:
      s &= "mm";
      break;      
    case UUnit::CM:
      s &= "cm";
      break;
    case UUnit::PT:
      s &= "pt";
      break;
    case UUnit::PC:
      s &= "pc";
      break;
    default: //case UUnit::IGNORE:
      s = "none";
      break;
  }
  return s;
} 

// =========================================================== [Elc] ===========
/*
 void UPaddingSpec::setH(float l_margin, float r_margin) {
 left = l_margin;
 right = r_margin;
 }
 
 void UPaddingSpec::setV(float t_margin, float b_margin) {
 top = t_margin;
 bottom = b_margin;
 }
 */

void UPaddingSpec::add(const UPaddingSpec& p2) {
  // !!! A COMPLETER !!! prendre en compte les Units !!!!!!
  top.val    += p2.top.val; 
  bottom.val += p2.bottom.val;
  left.val   += p2.left.val; 
  right.val  += p2.right.val;
}

}

