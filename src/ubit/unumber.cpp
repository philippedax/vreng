/************************************************************************
 *
 *  unumber.cpp: ACtive Numbers (can have callbacks)
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

#include <ubit/ubit_features.h>
#include <cstdlib>
#include <cstdio>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uevent.hpp>
#include <ubit/unumber.hpp>
#include <ubit/uappli.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

  
UNumber& UNumber::onChange(UCall &c) {   // @@@ devrait heriter de UObject !!!
  addChangeCall(c);
  return *this;
}

void UNumber::changed(bool update_now) {
  /// if (update_now && (bmodes & UMode::NO_AUTO_UPDATE) == 0) update();

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

UInt::UInt(const UStr& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

UInt::UInt(const string& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

/* ==================================================== ======== ======= */

UInt& UInt::operator=(const UStr& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

UInt& UInt::operator=(const std::string& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

istream& operator>>(istream& s, UInt& v) {
  int n;
  s >> n;
  v.setImpl(n, true);  // call callbacks
  return s;
}

ostream& operator<<(ostream& s, const UInt& v) {
  return (s << v.value);
}

/* ==================================================== ======== ======= */

UInt UInt::operator++(int) {
  UInt clone = *this;
  setImpl(value+1);
  return clone;
}

UInt  UInt::operator--(int) {
  UInt clone = *this;
  setImpl(value+1);
  return clone;
}

/* ==================================================== ======== ======= */

UInt& UInt::setImpl(int v, bool call_cb) {
  if (checkConst()) return *this;
  if (value == v) return *this;
  value = v;
  if (call_cb) changed(true);
  return *this;
}

UInt& UInt::setImpl(const char* s, bool call_cb) {
  if (checkConst()) return *this;
  if (!s) {
    value = 0;
    UAppli::warning("UInt::set","null char* argument");
  }
  else return setImpl(strtol(s, null, 0), call_cb);
  return *this;
}

UStr UInt::toString() const {
  char _s[50] = "";
  sprintf(_s, "%d", value);
  return _s;
}

/* ==================================================== [Elc] ======= */

UFloat::UFloat(const UStr& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

UFloat::UFloat(const string& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

/* ==================================================== ===== ======= */

UFloat& UFloat::operator=(const UStr& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

UFloat& UFloat::operator=(const std::string& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

istream& operator>>(istream& s, UFloat& v) {
  float n;
  s >> n;
  v.setImpl(n, true);  // call callbacks
  return s;
}

/* ==================================================== ===== ======= */

ostream& operator<<(ostream& s, const UFloat& v) {
  return (s << v.value);
}

UFloat UFloat::operator++(int) {
  UFloat clone = *this;
  ++value;
  return clone;
}

UFloat UFloat::operator--(int) {
  UFloat clone = *this;
  ++value;
  return clone;
}

/* ==================================================== ===== ======= */

UFloat& UFloat::setImpl(float v, bool call_cb) {
  if (checkConst()) return *this;
  if (value == v) return *this;
  value = v;
  if (call_cb) changed(true);
  return *this;
}

UFloat& UFloat::setImpl(const char* s, bool call_cb) {
  if (checkConst()) return *this;
  if (!s) {
    value = 0;
    UAppli::warning("UDouble::set","null char* argument");
  }
  else return setImpl(strtod(s, null), call_cb);
  return *this;
}

UStr UFloat::toString() const {
  char _s[50] = "";
  sprintf(_s, "%f", value);
  return _s;
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */

//UDouble::UDouble(const char* s) {
//  setImpl(s, false);  // dont call callbacks
//}

UDouble::UDouble(const UStr& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

UDouble::UDouble(const string& s) {
  setImpl(s.c_str(), false);  // dont call callbacks
}

/* ==================================================== ===== ======= */

UDouble& UDouble::operator=(const UStr& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

UDouble& UDouble::operator=(const std::string& s) {
  setImpl(s.c_str(), true);  // call callbacks
  return *this;
}

istream& operator>>(istream& s, UDouble& v) {
  double n;
  s >> n;
  v.setImpl(n, true);  // call callbacks
  return s;
}

/* ==================================================== ===== ======= */

ostream& operator<<(ostream& s, const UDouble& v) {
  return (s << v.value);
}

UDouble UDouble::operator++(int) {
  UDouble clone = *this;
  (*this)++;
  return clone;
}

UDouble UDouble::operator--(int) {
  UDouble clone = *this;
  (*this)++;
  return clone;
}

/* ==================================================== ===== ======= */

UDouble& UDouble::setImpl(double v, bool call_cb) {
  if (checkConst()) return *this;
  if (value == v) return *this;
  value = v;
  if (call_cb) changed(true);
  return *this;
}

UDouble& UDouble::setImpl(const char* s, bool call_cb) {
  if (checkConst()) return *this;
  if (!s) {
    value = 0;
    UAppli::warning("Double::set","Null char* argument");
  }
  else return setImpl(strtod(s, null), call_cb);
  return *this;
}

UStr UDouble::toString() const {
  char _s[50] = "";
  sprintf(_s, "%f", value);
  return _s;
}

}
