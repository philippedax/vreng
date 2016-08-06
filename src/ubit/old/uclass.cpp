/************************************************************************
 *
 *  uclass.cpp: Ubit MetaClasses
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
#include <ubit/ustr.hpp>
#include <ubit/uclass.hpp>
#include <ubit/uclassImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


unsigned int UClass::count = 0;
  
UClass::UClass(const char* n) : 
no(++count), name(new UStr(n)), style(null), attributes(null) {
}

UClass::UClass(const UStr& n) : 
no(++count), name(new UStr(n)), style(null), attributes(null) {
}

UClass::~UClass() {
  delete name;
  delete style;
}

/*
//#define UBIT_CLASS(CC) \
//static  const UClass& Class() {static UClass& c = *new USubclass<CC>(#CC); return c;} \
//virtual const UClass& getClass() const {return Class();}    

template <class CC>
struct USubclass : public UClass {
  USubclass(const char* name): UClass(name) {}
  virtual bool isInstance(UObject& obj) const {return dynamic_cast<CC*>(&obj);}
  virtual UStyle* newStyle() const {return CC::createStyle();}
};

template <class CC>
inline const UClass& MetaClass(const char*cc_name) {static UClass& c = *new USubclass<CC>(cc_name); return c;}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStyleSheet::~UStyleSheet() {
  for (Map::iterator k = map.begin(); k != map.end(); k++) {
    delete k->second; // deletes the nodes
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UElemClassMap::~UElemClassMap() {}

const UClass* UElemClassMap::findClass(const UStr& classname) const {
  Map::const_iterator k = map.find(&classname);
  if (k == map.end()) return null;
  else return k->second;
}

// add or replace
void UElemClassMap::addClass(const UClass& c) {
  Map::iterator k = map.find(&c.getName());
  if (k != map.end()) map.erase(k);
  map[&c.getName()] = &c;
}

const UClass* UElemClassMap::obtainClass(const UStr& classname) {
  const UClass* c = findClass(classname);
  if (c) return c;
  else {
    c = new UDefaultInlineElement::MetaClass(classname);   // !!! A REVOIR !!!
    map[&c->getName()] = c;
    return c;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UAttrClassMap::~UAttrClassMap() {
  //for (Map::iterator k = map.begin(); k != map.end(); k++) {
  //  delete k->second; // deletes the UClass
  //}
}

const UClass* UAttrClassMap::findClass(const UStr& classname) const {
  Map::const_iterator k = map.find(&classname);
  if (k == map.end()) return null;
  else return k->second;
}

// add or replace
void UAttrClassMap::addClass(const UClass& c) {
  Map::iterator k = map.find(&c.getName());
  if (k != map.end()) map.erase(k);
  map[&c.getName()] = &c;
}

const UClass* UAttrClassMap::obtainClass(const UStr& classname) {
  const UClass* c = findClass(classname);
  if (c) return c;
  else {
    c = new UDefaultAttribute::MetaClass(classname);   // !!! A REVOIR !!!
    map[&c->getName()] = c;
    return c;
  }
}

}
