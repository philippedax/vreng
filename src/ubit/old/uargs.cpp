/************************************************************************
 *
 *  uargs.cpp : class for creating argument lists ("additive notation").
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
#include <ubit/udefs.hpp>
#include <ubit/unode.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uargs.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

const UArgs UArgs::none;

/* test for counting creation/deletion balance
int Ncreate, Ndelete;
UArgsChildren::UArgsChildren() {Ncreate++;}
UArgsChildren::~UArgsChildren() {Ndelete--;}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UChild operator/(const UCond& cond, UNode& b) {
  return UChild(&b, cond);
}

UChild operator/(const UCond& cond, UNode* b) {
  if (!b) {
    UAppli::error("UArgs::operator/","null UNode argument in / specification");
    return UChild(null, cond);
  }
  else return UChild(b, cond);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UArgs::empty() const {
  return children->empty();
}

bool UArgs::operator!() const {
  return children->empty();
}

UArgs::~UArgs() {
  if (children->refcount > 1) children->refcount--;
  else if (children->refcount == 1) delete children;
}

UArgs::UArgs() {
  children = new UArgsChildren;
  children->refcount = 1;
}

UArgs::UArgs(UNode* b) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!b) UAppli::error("UArgs::UArgs","null UNode argument in arglist");
  else children->push_back(b);
}
  
UArgs::UArgs(UNode& b) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(b);
}

UArgs::UArgs(const char* s) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(new UStr(s));
}

UArgs::UArgs(const UChild& c) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!*c) UAppli::error("UArgs::UArgs","UChild argument pointing to null node");
  else children->push_back(c);
}

UArgs::UArgs(const UArgs& a) : children(a.children) {
  if (!children) {     // cas UArgs::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}

UArgs::UArgs(const UArgsImpl& a) : children(a.children) {
  if (!children) {     // cas UArgs::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}

UArgs& UArgs::operator+=(const UArgs& a2) {
  // marche pas car operator* est redefini
  // children->insert(children->end(), a2.children->begin(), a2.children->end());
  for (UChildIter i = a2.children->begin(); i != a2.children->end(); ++i)
    children->push_back(i.child());
  return *this;
}

// ========================================================= [Elc] ===========

UArgsImpl::~UArgsImpl() {
  if (children->refcount > 1) children->refcount--;
  else if (children->refcount == 1) delete children;
}

UArgsImpl::UArgsImpl() {
  children = new UArgsChildren;
  children->refcount = 1;
}

UArgsImpl::UArgsImpl(const UArgsImpl& a) : children(a.children) {
  if (!children) {  // cas UArgs::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}

// dans ce cas il ne faut pas augmenter a, sinon une expression comme :
//    ubox(args + string)
// aurait pour effet de rajouter string a args
UArgsImpl::UArgsImpl(const UArgs& a) {
  children = new UArgsChildren;
  *children = *a.children;   // !! la difference importante est ici !!
  children->refcount = 1;
}

UArgsImpl::UArgsImpl(const char* s) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(new UStr(s));
}

UArgsImpl::UArgsImpl(UNode* b) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!b) UAppli::error("UArgs","null argument in arglist");
  else children->push_back(b);
}

UArgsImpl::UArgsImpl(UNode& b) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(b);
}

UArgsImpl::UArgsImpl(const UChild& c) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!*c) UAppli::error("UArgs","null UChild argument in arglist");
  else children->push_back(c);
}

const UArgsImpl& operator+(const UArgsImpl& a, const char* s) {
  a.children->push_back(new UStr(s));
  return a;
}

const UArgsImpl& operator+(const UArgsImpl& a, UNode* b) {
  if (!b) UAppli::error("UArgs::operator+","null UNode in arglist");
  else a.children->push_back(b);
  return a;
}

const UArgsImpl& operator+(const UArgsImpl& a, UNode& b) {
  a.children->push_back(b);
  return a;
}

const UArgsImpl& operator+(const UArgsImpl& a, const UChild& c) {
  if (!*c) UAppli::error("UArgs::operator+","null UChild in arglist");
  else a.children->push_back(c);
  return a;
}

// UArgsImpl + UArgs => recopie children de UArgs a la suite de ceux de UArgsImpl
// SANS MODIFIER UArgs
//
const UArgsImpl& operator+(const UArgsImpl& a, const UArgs& a2) {
  // marche pas car operator* est redefini
  //a.children->insert(a.children->end(), a2.children->begin(), a2.children->end());
  for (UChildIter i = a2.children->begin(); i != a2.children->end(); ++i)
    a.children->push_back(i.child());
  return a;
}

const UArgsImpl& operator+(const _UAttrArgs& attrs, const UArgsImpl& a) {   // !!!!!!
  for (UChildReverseIter i = attrs.children->rbegin(); i != attrs.children->rend(); ++i)
    a.children->push_front(i.child());
  return a;
}

// ========================================================= [Elc] ===========

_UAttrArgs::~_UAttrArgs() {
  if (children->refcount > 1) children->refcount--;
  else if (children->refcount == 1) delete children;
}

_UAttrArgs::_UAttrArgs() {
  children = new UArgsChildren;
  children->refcount = 1;
}

/*
_UAttrArgs::_UAttrArgs(const UArgsImpl& a) : children(a.children) {
  if (!children) {  // cas UArgs::none pas encore initialise...
    children = new UArgsChildren;
    children->refcount = 0;
  }
  children->refcount++;
}
*/

_UAttrArgs::_UAttrArgs(UAttr* n) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!n) UAppli::error("UArgs","null UAttr argument in arglist");
  else children->push_back(n);
}

_UAttrArgs::_UAttrArgs(UAttr& n) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(n);
}

_UAttrArgs::_UAttrArgs(UCall* n) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!n) UAppli::error("UArgs","null UChild argument in arglist");
  else children->push_back(n);
}

_UAttrArgs::_UAttrArgs(UCall& n) {
  children = new UArgsChildren;
  children->refcount = 1;
  children->push_back(n);
}

/*
_UAttrArgs::_UAttrArgs(const UChild& c) {
  children = new UArgsChildren;
  children->refcount = 1;
  if (!*c) UAppli::error("UArgs::UArgs","null UChild argument in arglist");
  else children->push_back(c);
}
*/

const _UAttrArgs& operator,(const _UAttrArgs& a, UAttr* n) {
  if (!n) UAppli::error("UArgs::operator,","null UAttr argument in arglist");
  else a.children->push_back(n);
  return a;
}

const _UAttrArgs& operator,(const _UAttrArgs& a, UAttr& n) {
  a.children->push_back(n);
  return a;
}

const _UAttrArgs& operator,(const _UAttrArgs& a, UCall* n) {
  if (!n) UAppli::error("UArgs::operator,","null UCall argument in arglist");
  else a.children->push_back(n);
  return a;
}

const _UAttrArgs& operator,(const _UAttrArgs& a, UCall& n) {
  a.children->push_back(n);
  return a;
}

/*
const _UAttrArgs& operator+(const _UAttrArgs& a, const UChild& c) {
  if (!*c) UAppli::error("UArgs::operator,","null UChild argument in arglist");
  else a.children->push_back(c);
  return a;
}
*/

}
