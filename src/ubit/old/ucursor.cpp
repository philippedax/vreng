/************************************************************************
 *
 *  ucursor.cpp: Cursor Attribute
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
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uview.hpp>
#include <ubit/uevent.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uappli.hpp>
namespace ubit {

UCursor UCursor::none(-1, UCONST);
  
  /* les autres definis dans udispX11.cpp :
  *     UCursor UCursor::pointer(XC_left_ptr, UCONST);
  *     UCursor UCursor::crosshair(XC_tcross, UCONST);
  * ...etc...
  */
  
/* ==================================================== ===== ======= */

UCursor::UCursor(const UCursor& c) : cursor_type(c.cursor_type) {}
UCursor::UCursor(int ctype) : cursor_type(ctype) {}
UCursor::UCursor(int ctype, UConst m): UAttr(m), cursor_type(ctype) {}

UCursor& UCursor::set(const UCursor& c) {
  if (checkConst()) return *this;
  if (cursor_type == c.cursor_type) return *this;
  cursor_type = c.cursor_type;
  changed(true);
  return *this;
}

bool UCursor::equals(const UCursor &c) const {
  return (cursor_type == c.cursor_type);
}

/* ==================================================== ===== ======= */

void UCursor::addingTo(UChild& c, UElem& parent) {
  UAttr::addingTo(c, parent);
  //if (parent->isDef(0, UMode::HAS_CURSOR)) 
  //  UAppli::warning("UCursor::addingTo","multiple UCursor bricks in object:", parent->cname());
  // rendre parent sensitif aux events ad hoc
  parent.emodes.HAS_CURSOR = true;
}

void UCursor::removingFrom(UChild& c, UElem& parent) {
  // tant pis s'il y a plusieurs Cursors: de tt facon c'est une erreur
  parent.emodes.HAS_CURSOR = false;
  UAttr::removingFrom(c, parent);
}

void UCursor::putProp(UUpdateContext* ctx, UElem&) {
  ctx->cursor = this;
}

UCursor::~UCursor() {
  if (UAppli::isExiting()) return;
  for (int k = 0; k < (int)cimpl.size(); ++k) {
    UDisp* d = UAppli::getDisp(k);
    if (d) d->deleteCursorImpl(cimpl[k]);
  }
  destructs();
}

UCursorImpl* UCursor::getCursorImpl(UDisp* d) const {
  int id = d->getID();
  if (id < (int)cimpl.size() && cimpl[id] != null) return cimpl[id];
  else {
    if (id >= (int)cimpl.size()) {    // agrandir la table;
      for (int k = cimpl.size(); k <= id; ++k) cimpl.push_back(null);
    }
    return cimpl[id] = d->createCursorImpl(cursor_type);
  }
}


}
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
