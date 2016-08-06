/************************************************************************
 *
 *  uattr.cpp: Attributes of the Scene Graph
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
#include <ubit/udefs.hpp>
#include <ubit/uattr.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/uappli.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

//bool UAttr::isSpecified() const {return getClass().isSpecified();}

// defini dans impl sinon pbms compil (a cause du uptr<>) avce gcc2
UAttr::UAttr() {}
UAttr::UAttr(UConst) {omodes.IS_CONST = true;}

// defini dans impl sinon pbms compil
UAttr::~UAttr() {destructs();}
  
UStr UAttr::getNodeValue() const {
  UStr val; getValue(val); return val;
}

bool UAttr::getValue(UStr& val) const {
  val.clear();
  return false;
}

UAttr& UAttr::onChange(UCall& c) {
  addChangeCall(c);
  return *this;
}

void UAttr::changed(bool _update) {
  if (_update && !omodes.DONT_AUTO_UPDATE) update();
  
  if (!omodes.IGNORE_CHANGE_CALLBACKS) {
    if (_abegin() != _aend()) {
      UEvent e(UOn::change, this);  //UNodeEvent
      fire(e);
    }
    // ensuite on lance les callbacks des parents
    fireParents(UOn::propChange, this);
  }
}

/* ==================================================== ===== ======= */

struct UNamedValue: public UAttr {
  UNamedValue(const UStr& _name, const UStr& _value) 
  : pname(new UStr(_name)), pvalue(new UStr(_value)) {}
    
  virtual const UStr& getName() const {return *pname;}   //attention herite!!!
  virtual bool getValue(UStr& val) const {val = *pvalue; return true;}  //attention herite!!!  
  
  uptr<UStr> pname, pvalue;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void UAttrList::addAttr(UAttr& c) {
  _addAttr(c);
}

void UAttrList::addAttr(const UStr& name, const UStr& value) {
  if (name.empty()) return;
  UAttrList::addAttr(*new UNamedValue(name, value));
}

void UAttrList::update() {
  updateAutoParents(UUpdate::LAYOUT_PAINT);   // !! simplification  qui fait eventuellement des choses en trop...
}

UStr* UAttrList::getAttr(const UStr& name, bool ignore_case) {
  if (_abegin() == _aend()) return null;
 
  UStr* res = null;         // renvoie le dernier trouve!
  UNamedValue* p;
   for (UChildIter a = _abegin(); a != _aend(); ++a) {
     if ((p = dynamic_cast<UNamedValue*>(*a)) && p->getName().equals(name,ignore_case)) {
       res = p->pvalue;
    }
  }
  return res;
}

void UAttrList::putProp(UUpdateContext *props, UElem& par) {
  if (_abegin() == _aend()) return;
   UAttr* p;
   for (UChildIter a = _abegin(); a != _aend(); ++a) {
    if ((p = dynamic_cast<UAttr*>(*a))) p->putProp(props, par);
  }
}

/* ==================================================== [Elc] ======= */

UTip::UTip(const char* label) {set(label);}
UTip::UTip(const UArgs& a) {set(a);}

UTip::~UTip() {destructs();}  // removingFrom() impose un destructeur

UTip& UTip::set(const char* label) {
  content = label ? new UStr(label) : null;
  return *this;
}

UTip& UTip::set(const UArgs& a) {
  content = new UElem(a);
  return *this;
}

void UTip::addingTo(UChild& c, UElem& parent) {
  UAttr::addingTo(c, parent);
  //if (parent.emodes.HAS_TIP) {
  //  warning("UTip::addingTo","This UTip has a parent (%s %p) that contains another UTip object", this, &parent.getClassName(), &parent);
  //}
  // rendre parent sensitif aux events ad hoc
  parent.emodes.HAS_TIP = true;
}

void UTip::removingFrom(UChild& c, UElem& parent) {
  parent.emodes.HAS_TIP = false;
  UAttr::removingFrom(c, parent);
}

// ==================================================== Ubit Toolkit =========

UTitle::UTitle(const char* _s) {
  pstring = new UStr(_s);     // EX: pvalue!!!
  pstring->onChange(ucall(this, true, &UAttr::changed));
}

UTitle::UTitle(const UStr& _s) {
  pstring = new UStr(_s);     // EX: pvalue!!!
  pstring->onChange(ucall(this, true, &UAttr::changed));
}

void UTitle::update() {
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    UElem* grp = *p;
    UWin* win = grp ? grp->toWin() : null;
    if (win) win->setTitle(*pstring);     // EX: pvalue!!!
  } 
}

/*
UComment::UComment(const char* _s) : pvalue(new UStr(_s)) {}
UComment::UComment(const UStr& _s) : pvalue(new UStr(_s)) {}
*/

}
