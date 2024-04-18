/************************************************************************
 *
 *  ucond.cpp: Callback Conditions (see also class UOn)
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
#include <ubit/ucall.hpp>
#include <ubit/ucond.hpp>
#include <ubit/uelem.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/uevent.hpp>
#include <ubit/ustr.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UMultiCond& UMultiCond::add(const UCond& c) {
  condlist.push_back(&c);
  return *this;
}

void UMultiCond::remove(const UCond& c) {
  for (CondList::iterator p = condlist.begin(); p != condlist.end(); ++p) {
    if (*p == &c) {
      condlist.erase(p);
      return;
    }
  }
}

// ATT: renvoie *p et non c !
const UCond* UMultiCond::matches(const UCond& c) const {
  for (CondList::const_iterator p = condlist.begin(); p != condlist.end(); ++p) {
    if ((*p)->matches(c)) return *p;
  }
  return null;
}

bool UMultiCond::verifies(const UUpdateContext& ctx, const UElem& par) const {
  for (CondList::const_iterator p = condlist.begin(); p != condlist.end(); ++p) {
    if ((*p)->verifies(ctx, par)) return true;
  }
  return false;
}

void UMultiCond::setParentModes(UElem& parent) const {
  for (CondList::const_iterator p = condlist.begin(); p != condlist.end(); ++p) {
    (*p)->setParentModes(parent);
  }
}

}
