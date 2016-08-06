/************************************************************************
 *
 *  uobject.cpp: Base Class for all Ubit Objects
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
#include <typeinfo>
#include <cstdarg>
#include <ubit/unode.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UError::UError(int _errnum, const UObject* obj, const char* funcname) :
errnum(_errnum),
object(obj),
function_name(funcname) {
  message[0] = 0;
}

UError::~UError() throw () {
  //cerr << "UError destroyed " << (function_name? function_name:"")
  //<< " / " <<  (message?*message:"") << endl << endl;
}

const char* UError::what() const throw() {
  return message;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UObject::UConst UObject::UCONST;

const char* UObject::getVersion() {
  return UBIT_VERSION;
}

const UStr& UObject::getClassName() const {
  if (this) return getClass().getName();
  else {
    UAppli::error("UObject::getClassName","null pointer");
    static UStr& np = *new UStr("<Null Pointer>");
    return np;
  }
}

void UObject::error(const char* funcname, const char* format, ...) const throw(UError) {
  va_list ap;
  va_start(ap, format);
  UAppli::raiseError(UError::ERROR, this, funcname, format, ap);
  va_end(ap);
}

void UObject::warning(const char* funcname, const char* format, ...) const throw(UError) {
  va_list ap;
  va_start(ap, format);
  UAppli::raiseError(UError::WARNING, this, funcname, format, ap);
  va_end(ap);
}

void uerror(const char* funcname, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  UAppli::raiseError(UError::ERROR, null/*object*/, funcname, format, ap);
  va_end(ap);
}

void uwarning(const char* funcname, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  UAppli::raiseError(UError::WARNING, null/*object*/, funcname, format, ap);
  va_end(ap);
}

UObject& UObject::setAutoUpdate(bool state) {
  omodes.DONT_AUTO_UPDATE = !state;
  return *this;
}

UObject& UObject::ignoreChangeCallbacks(bool state) {
  omodes.IGNORE_CHANGE_CALLBACKS = state;
  return *this;
}

/* ==================================================== ===== ======= */

void* UObject::operator new(size_t sz) {
  UObject* obj = (UObject*) ::operator new(sz);
  // trick to detect whether this object is created by 'new' (see constructor)
  obj->ptr_count = (PtrCount)(long((obj)));
  return obj;
}

void UObject::operator delete(void* p) {
  if (!p) return;
  UObject* obj = static_cast<UObject*>(p);
  
  if (!obj->omodes.IS_DYNAMIC) {
    UAppli::error("delete UObject",
                  "%p: attempt to delete an object that was not created by new", p);
    return;
  }
  
  if (UAppli::impl.isTerminated()) {
    ::operator delete(p);
    return;
  }
  
  // NB: ptr_count<0 means that the object has already been destructed
  if (obj->ptr_count > 0) {
    UAppli::error("delete UObject",
                  "%p: attempt to delete an object that is pointed by a 'uptr' (Ubit smart pointer)", p);
    // peut etre rellement detruit plus tard
    obj->omodes.IS_DESTRUCTING = false;
    obj->omodes.IS_DESTRUCTED = false;
    return;
  }
  
  // this object will be deleted when this is safe to do so
  UAppli::impl.addDeleteRequest(obj);
}

/* ==================================================== ===== ======= */

UObject::UObject() {
  memset(&omodes, 0, sizeof(omodes));
  // object created by 'new' that can be destroyed by 'delete'
  if (ptr_count == (PtrCount)(long((this)))) omodes.IS_DYNAMIC = true; 
  ptr_count = 0;
  ostate = 0;
}

UObject::UObject(const UObject& obj) {
  memset(&omodes, 0, sizeof(omodes));
  // object created by 'new' that can be destroyed by 'delete'
  if (ptr_count == (PtrCount)(long((this)))) omodes.IS_DYNAMIC = true; 
  ptr_count = 0;
}

UObject& UObject::operator=(const UObject& obj) {
  // ptr_count et omodes ne doivent pas changer!
  return *this;
} 

UObject::~UObject() {
  ptr_count = -1;  // ptr_count<0 means that this object has been destructed
} 

/* ==================================================== ===== ======= */

UObject& UObject::setConst() {
  omodes.IS_CONST = true;
  return *this;
}

bool UObject::checkConst() const {
  if (omodes.IS_CONST) {
    error("UNode::checkConst","attempt to change the value of this constant object");
    return true;
  }
  else return false;
}

// PBM: addPtr() faux dans le cas des constantes types UColor::white, etc.
// car l'ordre d'init des vars globales est aleatoire. ceci dit ca n'a aucune 
// importance car ces vars ne sont jamais detruitre en cours d'exec (le ptr_count 
// sera faux mais on n'en a cure)

void UObject::addPtr() const {
  ++ptr_count;                           // @@@ !!!! MUST CHECK LIMIT !!!
} 

void UObject::removePtr() const {
  --ptr_count;
  
  // ptr_count < 0 means that the object has already been destructed
  // CAN_DELETE means that teh object has been created by 'new'
  
  if (ptr_count == 0 && omodes.IS_DYNAMIC && !hasSceneGraphParent())
    delete this;     // detacher du graphe, detruire l'objet et ses enfants
}

void UPtr::deferenceError() const {
  UAppli::fatalError("uptr::operator * or ->",
                     "can't derefence a uptr that points to null; uptr address= %p",
                     this);
}


}

