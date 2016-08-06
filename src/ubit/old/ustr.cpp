/************************************************************************
 *
 *  ustr.cpp: active strings (can have callbacks)
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
#include <cstring>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ubit/udefs.hpp>
#include <ubit/uon.hpp>
#include <ubit/ufile.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/uview.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uedit.hpp>
#include <ubit/uselection.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uappli.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

//UStr UStr::none((char*)null, UMode::UCONST);
//UStr UStr::newline("\n", UMode::UCONST);
//UStr UStr::goBOL("\15", UMode::UCONST); // retour a la ligne si necessaire

const UStr& UStr::getNodeName() const {static UStr nn("#text"); return nn;}

UStr& UStr::onChange(UCall &c) {
  addChangeCall(c);
  return *this;
}

UStr::UStr(const char* chs, UConst m) : UData(m) {
  s = null; //!dont forget: checked by _set() for freeing memory!
  initImpl(chs, (chs ? strlen(chs) : 0));
}

UStr::UStr() {
  s = null; // !dont forget: checked by _set() for freeing memory!
  initImpl(null, 0);
}

UStr::UStr(const char* chs) {
  s = null; // !dont forget: checked by _set() for freeing memory!
  initImpl(chs, (chs ? strlen(chs) : 0));
}

UStr::UStr(const string& str) {
  s = null;
  initImpl(str.c_str(), str.length());  //pas optimal mais tant pis!
}

UStr::UStr(const UStr& str) {
  s = null;
  initImpl(str.s, str.len);
}

/*
UStr::UStr(const class UInteger& _n) {
  s = null;
  char _s[50] = "";
  sprintf(_s, "%d", _n.intValue()); // int!
  initImpl(_s, strlen(_s));
}

UStr::UStr(const class UFloat& _n) {
  s = null;
  char _s[50] = ""; 
  sprintf(_s, "%f", _n.doubleValue());  // double!
  initImpl(_s, strlen(_s));
}

UStr::UStr(const class UDouble& _n) {
  s = null;
  char _s[50] = "";
  sprintf(_s, "%f", _n.doubleValue());  // double!
  initImpl(_s, strlen(_s));
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr::~UStr() {
  destructs();     // necessaire car removingFrom specifique
  if (s) free(s);
  s = null; len = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UStr::addingTo(UChild& c, UElem& parent) {
  UData::addingTo(c, parent);

  // dans ce mode les Str de n'importe quel object peuvent etre selectees
  if (UAppli::conf.generic_textsel) {
    parent.emodes.IS_TEXT_SELECTABLE = true;
  }
  
  // il y a un caret et il ne pointe sur rien -> le faire pointer sur cette UStr
  if (parent.emodes.IS_TEXT_EDITABLE) {
    UEdit* ed = null;
    parent.attributes().findClass(ed);
    if (!ed) parent.children().findClass(ed);
    if (ed && !ed->getCaretStr()) ed->setCaretStr(this, 0);
  }
}

void UStr::removingFrom(UChild& c, UElem& parent) {
  // la Str est enlevee et un caret pointait dessus -> le faire pointer sur null
  if (parent.emodes.IS_TEXT_EDITABLE) {
    UEdit* ed = null;
    parent.attributes().findClass(ed);
    if (!ed) parent.children().findClass(ed);
    if (ed && ed->getCaretStr() == this) ed->setCaretStr(null, 0);
  }
  UData::removingFrom(c, parent);
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// NB: duplicates the char string (!ATT: length MUST be the EXACT length)

void UStr::initImpl(const char *_s, int _len) {
  if (!_s) syncVals(null, 0);
  else {
    s = (char*)malloc((_len + 1) * sizeof(char));
    if (s) {strncpy(s, _s, _len); s[_len] = 0; syncVals(s, _len);}
    else {
      syncVals(null, 0);
      UAppli::fatalError("UStr::initImpl","No more memory; UStr object %p",this);
    }
  }
}

void UStr::setImpl(const char *_s, int _len) {
  if (checkConst()) return;
  if (s) free(s);
  
  if (!_s) syncVals(null, 0);
  else {
    s = (char*)malloc((_len + 1) * sizeof(char));
    if (s) {strncpy(s, _s, _len); s[_len] = 0; syncVals(s, _len);}
    else {
      syncVals(null, 0);
      UAppli::fatalError("UStr::setImpl","No more memory; UStr object %p",this);
    }
  }
  changed(true);
}

void UStr::setImplNoCopy(char *_s, int _len) {
  if (checkConst()) return;
  if (s) free(s);
  
  if (!_s) syncVals(null, 0);
  else {
    s = _s;
    s[_len] = 0; 
    syncVals(s, _len);
  }
  changed(true);
}

/* ==================================================== ======== ======= */

UStr& UStr::clear() {
  if (!s) return *this;
  setImpl((char*)null, 0);
  return *this;
}

UStr& UStr::operator=(const char* _s) {
  if (equals(_s)) return *this;
  setImpl(_s, (_s ? strlen(_s) : 0));
  return *this;
}

UStr& UStr::operator=(const UStr& _s) {
  if (equals(_s)) return *this;
  setImpl(_s.s, _s.len);
  return *this;
}

UStr& UStr::operator=(const string& _s) {
  if (equals(_s)) return *this;
  setImpl(_s.c_str(), _s.length());
  return *this;
}

/*
UStr& UStr::set(const UInteger& _n) {
  return setInt(_n.intValue());
}

UStr& UStr::set(const UFloat& _n) {
  return setFloat(_n.floatValue());
}

UStr& UStr::set(const UDouble& _n) {
  return setDouble(_n.doubleValue());
}
*/

UStr& UStr::setInt(int _n) {
  if (checkConst()) return *this;
  char _s[50] = ""; 
  sprintf(_s, "%d", _n);
  return operator=(_s); //cf autoupdate
}

UStr& UStr::setLong(long _n) {
  if (checkConst()) return *this;
  char _s[50] = ""; 
  sprintf(_s, "%ld", _n);
  return operator=(_s); //cf autoupdate
}

UStr& UStr::setFloat(float _n) {
  if (checkConst()) return *this;
  char _s[50] = ""; 
  sprintf(_s, "%f", _n);
  return operator=(_s); //cf autoupdate
}

UStr& UStr::setDouble(double _n) {
  if (checkConst()) return *this;
  char _s[50] = "";
  sprintf(_s, "%f", _n);
  return operator=(_s); //cf autoupdate
}

bool UStr::scanValue(float& val, UStr& unit) const {
  const char* u = null;
  bool valfound = scanValue(val, u);
  if (u && *u) unit = u; else unit.clear();
  return valfound;
}

bool UStr::scanValue(float& val, const char*& unit) const {
  val = 0.;
  if (!s || !*s) return false;
  const char* p = s; 
  // virer tout les caracteres speciaux jusqua ' ' inclus
  // attention cast necessaire sinon suppression des accentues
  while (*p && (unsigned char)*p <= ' ') p++;
  
  bool positive = true;
  if (*p == '-') {
    positive = false;
    ++p;
  }
  
  const char* pnum = p;
  double x = 0.;
  
  while (isdigit(*p)) {
    x = x * 10 + (*p - '0');
    p++;
  }
  
  if (*p=='.') {
    p++;
    double n = 10.;
    while (isdigit(*p)) {
      x += (*p - '0') / n;
      n *= 10;
      p++;
    }
  }
  
  val = positive ? x : -x;
  unit = p;
  return (p != pnum);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ostream& operator<<(ostream& os, const UStr& s) {
  if (s.s) return (os << s.s);
  else return os;
}

istream& operator>>(istream& is, UStr& s) {
  char* c_s = null;
  is >> c_s;
  s.operator=(c_s);
  return is;
}

/* ==================================================== [Elc] ======= */

bool UStr::equals(const char* _s, bool ignore_case) const {
  if (s == _s) return true;
  else if (!_s) return isEmpty();
  else if (!s) return (!_s || !*_s); 
  else return (UCstr::compare(s, _s, ignore_case) == 0);
}

bool UStr::equals(const UStr& _s, bool ignore_case) const {
  return equals(_s.s, ignore_case);
}

bool UStr::equals(const string& _s, bool ignore_case) const {
  return equals(_s.c_str(), ignore_case);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UStr::compare(const char* _s, bool ignore_case) const {
  if (s == _s) return 0;
  else if (!_s) return isEmpty() ? 0 : 1;
  else if (!s) return (!_s || !*_s) ? 0 : -1;
  else return UCstr::compare(s, _s ,ignore_case);
}

int UStr::compare(const UStr& _s, bool ignore_case) const {
  return compare(_s.s, ignore_case);
}

int UStr::compare(const string& _s, bool ignore_case) const {
  return compare(_s.c_str(), ignore_case);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UStr::compareN(const char* _s, unsigned int n, bool ignore_case) const {
  if (s == _s) return 0;
  else if (!_s) return isEmpty() ? 0 : 1;
  else if (!s) return (!_s || !*_s) ? 0 : -1;
  else return UCstr::compareN(s, _s, n, ignore_case);
}

int UStr::compareN(const UStr& _s, unsigned int n, bool ignore_case) const {
  return compareN(_s.s, n, ignore_case);
}

int UStr::compareN(const string& _s, unsigned int n, bool ignore_case) const {
  return compareN(_s.c_str(), n, ignore_case);
}

/* ==================================================== [Elc] ======= */

int UStr::toInt() const {
  return s ? atoi(s) : 0;
}

long UStr::toLong() const {
  //return s ? strtol(s, null, 0) : 0;
  return s ? atol(s) : 0;
}

float UStr::toFloat() const {
  return float(s ? atof(s) : 0);
}

double UStr::toDouble() const {
  return s ? atof(s) : 0;  // == strtod(s, null) 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr UStr::toUpper() const {
  UStr s2 = s;
  if (!s2.s) return s2;
  for (char* p = s2.s; *p; p++) *p = toupper(*p);
  return s2;
}

UStr UStr::toLower() const {
  UStr s2 = s;
  if (!s2.s) return s2;
  for (char* p = s2.s; *p; p++) *p = tolower(*p);
  return s2;
}

void UStr::upper() {
  if (!s) return;
  for (char* p = s; *p; p++) *p = toupper(*p);
}

void UStr::lower() {
  if (!s) return;
  for (char* p = s; *p; p++) *p = tolower(*p);
}

void UStr::capitalize() {
  if (!s) return;
  if (!*s) return; else *s = toupper(*s);
  for (char* p = s+1; *p; p++) *p = tolower(*p);
}

/* ==================================================== [Elc] ======= */

bool UStr::checkFormat(int pos, int _c) {
  if (_c == '\0') return false;      //don't insert the NUL char!
  //else if (model) return model->checkFormat(pos, _c);
  else return true;
}

bool UStr::checkFormat(int pos, const char *_s) {
  if (_s == null) return false;      //don't insert the NULL string!
  //else if (model) return model->checkFormat(pos, _s);
  else return true;
}

void UStr::syncVals(char *news, int newlen) {
  s = news;
  len = newlen;
  /*
  if (model) model->set(this);
  void UStr::setStrModel(class UEditable *m) {
  model = m;
  chainage et cas null et enlever existant;
  */
}

void UStr::update() {
  _parents.updateAutoParents(UUpdate::layoutAndPaint);
}

// calls UOn::change callbacks of this object when its value is changed
// then UOn::changeData AND UOn::changeStr callbacks of its parents
//
void UStr::changed(bool upd) {
  if (upd && !omodes.DONT_AUTO_UPDATE) update();

  if (!omodes.IGNORE_CHANGE_CALLBACKS) {
    if (_abegin() != _aend()) {
      UEvent e(UOn::change, this);  //UNodeEvent
      fire(e);
    }
    // ensuite on lance les callbacks des parents
    // container callbacks
    fireParents(UOn::dataChange, this);  // utile ?
    fireParents(UOn::strChange, this);
  }
}

/* ==================================================== ===== ======= */

char UStr::operator[](int pos) const {
  if (!s || pos >= len) return 0;
  if (pos < 0) pos = len-1; // last char
  return s[pos];
}

char UStr::at(int pos) const {
  if (!s || pos >= len) return 0;
  if (pos < 0) pos = len-1; // last char
  return s[pos];
}
 
char UStr::setCharAt(int pos, char newchar, bool upd) {
  if (checkConst()) return 0;
  if (!s || pos >= len) return 0;
  if (!checkFormat(pos, newchar)) return false; // !!

  if (pos < 0) pos = len-1; // last char
  s[pos] = newchar;

  syncVals(s, len);
  changed(upd);
  return true; 
}

/* ==================================================== [Elc] ======= */

UStr operator&(const UStr& s1, const UStr& s2) {
  UStr str = s1; str.append(s2); return str;
}

UStr operator&(const UStr& s1, const char* s2) {
  UStr str = s1; str.append(s2); return str;
}

UStr operator&(const char* s1, const UStr& s2) {
  UStr str = s1; str.append(s2); return str;
}

UStr operator&(const UStr& s1, const string& s2) {
  UStr str = s1; str.append(s2); return str;
}

UStr operator&(const string& s1, const UStr& s2) {
  UStr str = s1; str.append(s2); return str;
}

UStr operator&(const UStr& _s, int _v) {
  UStr str(_s); str.append(_v); return str;
}

UStr operator&(int _v, const UStr& _s) {
  UStr str; str.setInt(_v); str.append(_s); return str;
}

UStr operator&(const UStr& _s, double _v) {
  UStr str(_s); str.append(_v); return str;
}

UStr operator&(double _v, const UStr& _s) {
  UStr str; str.setDouble(_v); str.append(_s); return str;
}

/*
UStr operator&(const UStr& _s, const UInteger& _v) {
  UStr str(_s); str.append(_v); return str;
}
UStr operator&(const UInteger& _v, const UStr& _s) {
  UStr str; str = _v; str.append(_s); return str;
}
UStr operator&(const UStr& _s, const UDouble& _v) {
  UStr str(_s); str.append(_v); return str;
}
UStr operator&(const UDouble& _v, const UStr& _s) {
  UStr str; str = _v; str.append(_s); return str;
}
*/
/* ==================================================== [Elc] ======= */

void UStr::append(const char c) {
  insertImpl(-1, c, true);
}

void UStr::append(const char *s2) {
  insertImpl(-1, s2, 0, npos, true);
}

void UStr::append(const char *s2, unsigned int nbchars) {
  insertImpl(-1, s2, 0, nbchars, true);
}

void UStr::append(const UStr &s2) {
  insertImpl(-1, s2.s, 0, npos, true);
}

void UStr::append(const UStr &s2, unsigned int nbchars) {
  insertImpl(-1, s2.s, 0, nbchars, true);
}

void UStr::append(const string& s2) {
  insertImpl(-1, s2.c_str(), 0, npos, true);
}

void UStr::append(const string& s2, unsigned int nbchars) {
  insertImpl(-1, s2.c_str(), 0, nbchars, true);
}

void UStr::append(int v2) {
  char _s2[50] = {0}; 
  sprintf(_s2, "%d", v2);
  insertImpl(-1, _s2, 0, npos, true);
}

void UStr::append(double v2) {
  char _s2[50] = {0}; 
  sprintf(_s2, "%f", v2);
  insertImpl(-1, _s2, 0, npos, true);
}

/*
void UStr::append(const UInteger& v2) {
  append(v2.intValue());
}

void UStr::append(const UDouble& v2) {
  append(v2.doubleValue());
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UStr::insert(int to_pos, char c) {
  return insertImpl(to_pos, c, true);
}

bool UStr::insert(int to_pos, const UStr &s2, int from_pos,
                  unsigned int from_nbchars) {
  return insertImpl(to_pos, s2.s, from_pos, from_nbchars, true);
}

bool UStr::insert(int to_pos, const char *s2, int from_pos,
                  unsigned int from_nbchars) {
  return insertImpl(to_pos, s2, from_pos, from_nbchars, true);
}

bool UStr::insert(int to_pos, const std::string& s2, int from_pos,
                  unsigned int from_nbchars) {
  return insertImpl(to_pos, s2.c_str(), from_pos, from_nbchars, true);
}

/* ==================================================== ======== ======= */
// from_nbchars < 0 means whole string s2
// return : true if changed

bool UStr::insertImpl(int to_pos, const char *s2, int from_pos,
                      unsigned int from_nbchars, bool upd) {
  if (checkConst()) return false;
  if (!checkFormat(to_pos, s2) || to_pos > len) return false;

  if (to_pos < 0) to_pos = len; // append

  int len_s2 = strlen(s2);
  if (from_pos < 0) from_pos = len_s2 -1;
  
  int nbc = (from_nbchars == npos) ? len_s2 : from_nbchars;
  if (from_pos + nbc > len_s2) nbc = len_s2 - from_pos;
  if (nbc <= 0) return false;

  char* news = (char*) malloc(len + nbc + 1); // 0 final
  if (!news) {
    UAppli::fatalError("UStr::insertImpl","No more memory; UStr object %p",this);
    return false;		// str et strLen inchanges !
  }

  if (!s) strncpy(news, s2+from_pos, nbc);
  else {
    if (to_pos > 0) strncpy(news, s, to_pos);
    strncpy(news+to_pos, s2+from_pos, nbc);
    strcpy(news+to_pos+nbc, s+to_pos);
    free(s);
  }
  news[len+nbc] = 0;  // 0 final

  syncVals(news, len+nbc);
  changed(upd);
  return true;
}

/* ==================================================== ======== ======= */

bool UStr::insertImpl(int pos, char c, bool upd) {
  if (checkConst()) return false;
  if (!checkFormat(pos, c)) return false; //!!
  if (pos < 0 || pos > len) pos = len; // append

  char *news = (char*) malloc(len + 1 + 1); // 0 final
  if (!news) {
    UAppli::fatalError("UStr::insertImpl","No more memory; UStr object %p",this);
    return false;		// str et strLen inchanges !
  }

  if (!s) {
    news[0] = c;
    news[1] = '\0';
  }
  else {
    if (pos>0) strncpy(news, s, pos);
    news[pos] = c;
    strcpy(news+pos+1, s+pos);
    free(s);
  }

  syncVals(news, len+1);
  changed(upd);
  return true;
}

/* ==================================================== [Elc] ======= */

void UStr::remove(int pos, unsigned int nbchars) {
  replaceImpl(pos, nbchars, (char*)null, true);
}

void UStr::replace(int pos, unsigned int nbchars, const UStr& s2) {
  replaceImpl(pos, nbchars, s2.s, true);
}

void UStr::replace(int pos, unsigned int nbchars, const char *s2) {
  replaceImpl(pos, nbchars, s2, true);
}

void UStr::replace(int pos, unsigned int nbchars, const std::string& s2) {
  replaceImpl(pos, nbchars, s2.c_str(), true);
}

/* ==================================================== ======== ======= */

bool UStr::replaceImpl(int pos, unsigned int nbchars, const char *str, bool upd) {
  if (checkConst()) return false;
  if (pos < 0) pos = len-1;  // last char (new 26jt)

  int nbc = (nbchars == npos) ? len : nbchars;
  
  // revient a un insert() ou un append() dans ce cas
  if (pos >= len) return insertImpl(pos, str, 0, npos, upd);

  // ne pas depasser taille actuelle de str
  if (pos + nbc > len) nbc = len - pos;

  int nadd = str ? strlen(str) : 0;
  int newlen = len - nbc + nadd;

  if (newlen <= 0) {	  // theoriquement jamais < 0 mais == 0
    syncVals(null, 0);
  }
  
  else {
    char* news = (char*)malloc(newlen + 1); // 0 final
    if (!news) {
      UAppli::fatalError("UStr::replaceImpl","No more memory; UStr object %p",this);
      return false;		  // str et strLen inchanges !
    }

    // cas (!s ou !*s)  ==>  equivalent a remove()
    if (nadd==0) {	
      if (!s) return false;	  // securite: deja teste par: newlen <= 0
      if (pos>0) strncpy(news, s, pos);
      strcpy(news+pos, s+pos+nbc);
      free(s); s = null;
    }

    // il y a qq chose a ajouter
    else {
      if (!checkFormat(pos, str)) return false; //!!

      if (!s) strcpy(news, str);
      else {
	if (pos>0) strncpy(news, s, pos);
	strncpy(news+pos, str, nadd);
	strcpy(news+pos+nadd, s+pos+nbc);
	free(s); s = null;
      }
    }

    syncVals(news, newlen);
  }

  changed(upd);
  return true;
}

/* ==================================================== [Elc] ======= */

bool UStr::toString(string& str, int from_pos, unsigned int nbchars) const {
  if (!s || from_pos >= len) // hors bornes
    return false;
  else {
    if (from_pos < 0) from_pos = len-1;
    int nbc = (nbchars == npos) ? len : nbchars;
  
    // ne pas depasser taille actuelle de str
    if (from_pos + nbc > len) nbc = len - from_pos;
    if (nbc <= 0) return false;
    else {
      str.append(s + from_pos, nbc);
      return true;
    }
  }
}

std::string UStr::toString(int pos, unsigned int nbchars) const {
  std::string substr;
  toString(substr, pos, nbchars);
  return substr;
}

UStr UStr::substring(int pos, unsigned int nbchars) const {
  UStr substr;
  substr.insert(0, *this, pos, nbchars);
  return substr;
}

/* ==================================================== ======== ======= */

void UStr::trim(bool strip_beginning, bool strip_end) {
  if (checkConst()) return;
  if (!s || !*s) return;

  char* p1 = s;
  if (strip_beginning) {	// enlever les \n \t et spaces au debut
    // virer tout les caracteres speciaux jusuq'a ' ' inclus
    // attention cast necessaire sinon suppression des accentues
    while (*p1 && (unsigned char)*p1 <= ' ') p1++;
    if (!*p1) {
      free(s);
      syncVals(null, 0);
      return;
    }
  }

  char* p2 = s + len - 1;
  if (strip_end) {		// enlever les \n \t et spaces a la fin
    //while (p2 >= p1 && (*p2 == ' ' || *p2 == '\n' || *p2 == '\t')) p2--;
    while (p2 >= p1 && (unsigned char)*p2 <= ' ') p2--;
    *(p2+1) = 0;
  }

  char* s2 = UCstr::dup(p1);
  free(s);
  syncVals(s2, p2 - p1 + 1);
}

/* ==================================================== ======== ======= */

int UStr::tokenize(std::vector<UStr*>& tokens, const UStr& separators,
		   bool trim_beginning, bool trim_end) const
{
  if (empty()) return 0;
  char* p1 = s;
  unsigned int ix = 0;

  while (p1) {
    UStr* t;
    // on recupere les tokens existants si c'est le cas
    if (ix < tokens.size()) {
      t = tokens[ix];
      if (t) t->clear(); 
      else {t = new UStr(); tokens[ix] = t;}
    }
    else {
      t = new UStr(); tokens.push_back(t);
    }
    ix++;

    char* p2 = ::strpbrk(p1, separators.c_str());
    if (p2) t->append(p1, p2-p1);
    else t->append(p1);
    
    t->trim(trim_beginning, trim_end);
    if (t->empty()) ix--;
    
    if (p2) p1 = p2 + 1;
    else break;
  }
  return ix;
}

/* ==================================================== ======== ======= */

UStr UStr::split(int pos, bool delete_char_at_pos) {
  //if (checkConst()) return UStr::none;
  if (checkConst()) return "";
  UStr res;

  if (!s || pos<0 || pos>=len) return res;

  if (delete_char_at_pos) res = s+1+pos; else res = s+pos;

  if (pos == 0) {
    free(s);
    syncVals(null, 0);
  }
  else {
    len = pos;
    s = (char*)realloc(s, sizeof(char)*(pos+1));
    s[pos] = '\0';
    syncVals(s, len);
  }

  changed(true);
  return res;
}

/* ==================================================== [Elc] ======= */

int UStr::find(char c, int pos) const {
  if (empty() || pos >= len) return -1;
  char* p = ::strchr(s + pos, c);
  return (p ? p-s : -1);
}

int UStr::find(const char* s2, int pos) const {
  if (empty() || !s2 || pos >= len) return -1;
  char* p = ::strstr(s + pos, s2);
  return (p ? p-s : -1);
}

int UStr::find(const UStr& s2, int pos) const {
  return find(s2.c_str(), pos);
}

int UStr::find(const std::string& s2, int pos) const {
  return find(s2.c_str(), pos);
}

int UStr::rfind(char c) const {
  if (empty()) return -1;
  char* p = ::strrchr(s, c);
  return (p ? p-s : -1);
}

/* ==================================================== [Elc] ======= */

void UStr::getSize(UUpdateContext& ctx, UDimension& dim) const {
  // NB: conserver une hauteur stable etfaire en sorte que la surface ne soit 
  // jamais de taille nulle (sinon on ne pourra plus la selecionner en cliquant)  
  UFontMetrics fm(ctx);
  dim.width = (s && *s) ? fm.getWidth(s, len) : 1;   // 1 pour afficher le caret
  dim.height = fm.getHeight();
}

// prototype for warped text (UFlowview)
void UStr::getSize(UUpdateContext& ctx, UDimension& dim, float available_width,
                   int offset, int& str_sublen, int& change_line) const {
  UFontMetrics fm(ctx);
  if (s && *s) {
    fm.getSubTextSize(s + offset, len - offset, dim, available_width, 
                      str_sublen, change_line);
    // sinon il n'y aura pas de place pour afficher le caret en fin de ligne
    // *w += 1; ???
  }
  else { // voir NOTES ci-dessus
    dim.width = 1;
    dim.height = fm.getHeight();
    str_sublen = 0;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UStr::paint(UGraph& g, UUpdateContext& ctx, const URect& r) const {
  paint(g, ctx, r, 0, len);
}

void UStr::paint(UGraph& g, UUpdateContext& ctx, const URect& r,
                 int offset, int cellen) const {
  // !! don't confuse cellen and len!
  if (!s || !*s) return;
  
  USelection* ts = g.getDisp() ? g.getDisp()->getChannelSelection(0) : null;

  //ce qui serait cool c'est d'economiser les setColor et setFont!!!
  g.setFont(ctx.fontdesc);
  if (ctx.color) g.setColor(*ctx.color);

  // cas ou il n'y a pas de TextSel, ou l'objet pointe est null
  // ou ce n'est pas le parent de str 
  // -> affichage de base sans selection
  
  //if ((bmodes & UMode::IN_SELECTION) == 0
  if (!omodes.IS_IN_SELECTION
      || !ts || !ts->in_obj
      // permettre l'affichage des objets inclus dans ts.in_obj
      || ts->fromLink == USelection::null_link
      || ts->toLink == USelection::null_link
      || ts->fromLink == ts->endPressLink
      || ts->toLink == ts->endPressLink
      ) {
    g.drawString(s + offset, cellen, r.x, r.y);
  }

  // -> sinon affichage avec selection
  else {
    UFontMetrics fm(ctx);

    int deb = 0, fin = cellen;
    float ww = 0;    // ex int
    // IL FAUDRAIT tester LEs LIENs (pas les UStr)
    // sinon les strings partagees vont s'allumer a plusieurs endroits!!

    if (this == dynamic_cast<const UStr*>(*(ts->fromLink))) {
      deb = ts->fromPos - offset;
      if (deb < 0) deb = 0;  //inclus dans selection
      else if (deb >= cellen) deb = cellen;   //exclus (avant) selection
    }

    // pas de else: this peut etre a la fois fromLink et toLink
    // (c'est meme le cas le plus frequent!)

    if (this == dynamic_cast<const UStr*>(*(ts->toLink))) {
      fin = ts->toPos - offset;
      if (fin < 0) fin = 0;   //exclus (apres) selection
      else if (fin >= cellen) fin = cellen; //inclus dans selection
    }

    if (deb > 0) {
      g.drawString(s+offset, deb, r.x, r.y);
      ww = fm.getWidth(s+offset, deb);
    }
    
    if (fin > deb) {
      if (ts->pcolor) g.setColor(*ts->pcolor);
      if (ts->pbgcolor) g.setBackground(*ts->pbgcolor);
      if (ts->pfont) {
        UFontDesc fd = ctx.fontdesc;
        fd.merge(*ts->pfont);
        fd.setScale(ctx.xyscale);
        g.setFont(fd);
        g.drawString(s+deb+offset, fin-deb, r.x + ww, r.y);
        ww += UFontMetrics(fd,ctx.getDisp()).getWidth(s+deb+offset, fin-deb);
      }
      else {
        g.drawString(s+deb+offset, fin-deb, r.x + ww, r.y);
        ww += fm.getWidth(s+deb+offset, fin-deb);
      }
    }

    if (fin < cellen) {
      g.setFont(ctx.fontdesc);
      if (ctx.color) g.setColor(*ctx.color);
      g.drawString(s+fin+offset, cellen-fin, r.x + ww, r.y);
    }
  }
} 

/* ==================================================== [Elc] ======= */

UStr UStr::basename(bool with_suffix) const {
  UStr res;
  if (len == 0) return res;
  
  char* dir_name, *base_name, *suffix;
  int dir_len, base_len, suffix_len;
  UCstr::parsePath(s, len, dir_name, dir_len, base_name, base_len, suffix, suffix_len);
  
  if (with_suffix) res = base_name;
  else res.insert(0, base_name, 0, base_len);
  return res;
}


UStr UStr::dirname(bool with_slash) const {
  UStr res;
  if (len == 0) res = ".";
  else {
    char* dir_name, *base_name, *suffix; 
    int dir_len, base_len, suffix_len;
    UCstr::parsePath(s, len, dir_name, dir_len, base_name, base_len, suffix, suffix_len);
    
    if (dir_len == 0) res = ".";
    else res.insert(0, dir_name, 0, dir_len);
  }
  
  if (with_slash && res.at(-1) != '/') res.insert(-1, '/');
  return res;
}

UStr UStr::suffix() const {
  UStr res;
  if (len == 0) return res;
  
  char* dir_name, *base_name, *suffix;
  int dir_len, base_len, suffix_len;
  UCstr::parsePath(s, len, dir_name, dir_len, base_name, base_len, suffix, suffix_len);
  res.insert(0, suffix, 0, suffix_len);
  return res;
}

UStr UStr::expand() const {
  if (empty()) return "";
  UStr res = *this;
  
  if (s[0] == '~' && s[1] == '/') {
    //NB: ne PAS faire de free() sur un getenv!
    char* home = getenv("HOME");
    if (home) res.replace(0, 1, home); // virer ~mais pas /
  }
  // on pourrait aussi gerer les variables....
  return res;
}

/* ==================================================== [Elc] ======= */

int UStr::read(const UStr& filename) {
  if (checkConst()) return 0;
  UStr fname = filename.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  char* buffer = null;
  struct stat finfo;
  int fd = -1;
  int res = UFilestat::NotOpened;
  int ll = 0;
  
  if ((fd = ::open(fname.c_str(), O_RDONLY, 0)) == -1) {
    res = UFilestat::CannotOpen;
  }
  else if (::fstat(fd, &finfo) == -1
           || (finfo.st_mode & S_IFMT) != S_IFREG) {
    res = UFilestat::CannotOpen;
  }
  else if (!(buffer = (char*)::malloc((finfo.st_size + 1) * sizeof(char)))) {
    res = UFilestat::NoMemory;
  }
  else if (::read(fd, buffer, finfo.st_size) <= 0) {
    res = UFilestat::InvalidData;
    if (buffer) ::free(buffer);
    buffer = null;
  }
  else {
    // trouver le 1et \0 et s'arreter la pour eviter plantages avec
    // les executables, etc.
    for (ll = 0; ll < finfo.st_size; ll++)
      if (buffer[ll] == 0) break;
    
    // be sure it's NULL terminated (nb: on a fait malloc (size + 1))
    if (ll >= finfo.st_size) {
      ll = finfo.st_size;
      buffer[ll] = '\0';
    }
    
    res = UFilestat::Opened;
  }
  
  if (fd != -1) close(fd);
  setImplNoCopy(buffer, ll);
  return res;
}

/* ==================================================== ===== ======= */

int UStr::readLines(const UStr& filename, std::vector<UStr*>& lines,
                    bool strip_beginning, bool strip_end) {
  UStr fname = filename.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  ifstream in(fname.c_str());
  if (!in) return UFilestat::CannotOpen;
  
  string line;
  while ((std::getline(in, line))) {
    UStr* l = new UStr(line);
    l->trim(strip_beginning, strip_end);
    if (l->empty()) delete l; else lines.push_back(l);
  }
  
  return UFilestat::Opened;
}

/* ==================================================== ===== ======= */

int UStr::write(const UStr& filename) {
  UStr fname = filename.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  int fd = -1;
  int res = UFilestat::NotOpened;
  
  if ((fd = ::open(fname.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1)
    res = UFilestat::CannotOpen;
  else if (::write(fd, s, len) <= 0)
    res = UFilestat::InvalidData;
  else
    res = UFilestat::Opened;
  
  if (fd != -1) close(fd);
  return res;
}

/* ==================================================== [Elc] ======= */
// useful functions 

char *UCstr::dup(const char *s1) {
  if (!s1) return null;
  else {
    char *p = (char*)::malloc((strlen(s1) + 1) * sizeof(char));
    if (p) ::strcpy(p, s1);
    return p;
  }
}

char *UCstr::dup(const char *s1, const char *s2) {
  if (!s1 || !*s1) return UCstr::dup(s2);
  else if (!s2 || !*s2) return UCstr::dup(s1);
  else {
    int l1 = strlen(s1);
    char *p = (char*)::malloc((l1 + strlen(s2) + 1) * sizeof(char));
    if (p) {
      ::strcpy(p, s1);
      ::strcpy(p+l1, s2);
    }
    return p;
  }
}

char* UCstr::dup(const char *s1, char sep, const char *s2) {
  if (!s1 || !*s1) return UCstr::dup(s2);
  else if (!s2 || !*s2) return UCstr::dup(s1);
  else {
    int l1 = strlen(s1);
    char *p = (char*)::malloc((l1 + strlen(s2) + 2) * sizeof(char));
    if (p) {
      ::strcpy(p, s1);
      p[l1] = sep;
      ::strcpy(p + l1 + 1, s2);
    }
    return p;
  }
}

bool UCstr::equals(const char* s1, const char* s2, bool ignore_case) {
  if (s1 == s2) return true;
  else return compare(s1, s2, ignore_case);
};

int UCstr::compare(const char* s1, const char* s2, bool ignore_case) {
  if (s1 == s2) return 0;
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (ignore_case) {
    do {
      c1 = isupper(*p1) ? tolower(*p1) : (*p1);
      c2 = isupper(*p2) ? tolower(*p2) : (*p2);
      if (c1 == '\0') break;
      ++p1; ++p2;
    } while (c1 == c2);
  }
  else {
    do {
      c1 = *p1;
      c2 = *p2;
      if (c1 == '\0') break;
      ++p1; ++p2;
    } while (c1 == c2);    
  }
  
  return c1 - c2;
}

int UCstr::compareN(const char* s1, const char* s2, unsigned int n, bool ignore_case) {
  if (s1 == s2 || n == 0) return 0;  
  register const unsigned char *p1 = (const unsigned char *) s1;
  register const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;

  if (ignore_case) {
    do {
      c1 = isupper(*p1) ? tolower(*p1) : (*p1);
      c2 = isupper(*p2) ? tolower(*p2) : (*p2);
      if (--n == 0 || c1 == '\0') break;
      ++p1; ++p2;
    }
    while (c1 == c2);
  }
  else {
    do {
      c1 = *p1;
      c2 = *p2;
      if (--n == 0 || c1 == '\0') break;
      ++p1; ++p2;
    } while (c1 == c2);    
  }
  
  return c1 - c2;
}


//const char *UCstr::suffix(const char *path) {
//  return UCstr::suffix((char*) path);
//}

const char *UCstr::suffix(const char *path) {
  const char *p = path + ::strlen(path);
  //probleme: etre sur que le . est apres le dernier /
  while (p >= path) {
    if (*p == '.' || *p == '/') break;
    p--;
  }
  // be sure its a . and eliminate . and .xxxrc and ..
  if (p <= path || *p != '.') return null;
  else if (p == path+1 && *path == '.') return null;
  else return p+1;
}


void UCstr::parsePath(char* path, int len,
                      char*& dir_name, int& dir_len,
                      char*& base_name, int& base_len,
                      char*& suffix, int& suffix_len) {
  dir_name = null;
  dir_len = 0;
  base_name = null;
  base_len = 0;
  suffix = null;
  suffix_len = 0;
  
  if (len == 0 || !path || !*path) return;
  
  char *p = path + len;
  
  // chercher le /
  while (p > path) {
    if (*p == '/') break;
    p--;
  }
  
  if (*p == '/') {
    dir_name = path;
    if (p == path) dir_len = 1; // CP du "/"
    else dir_len = p - path;
    base_name = p+1;
    base_len = len - dir_len -1;
  }
  else {
    base_name = path;
    base_len = len;
  }
  
  if (*base_name == 0) {
    base_name = null;
    base_len = 0;
    return;
  }
  
  // chercher le . (be sure its a . and eliminate . and .xxxrc and ..)
  char *f = base_name;
  bool normal_char_found = false;
  while (*f != 0) {
    if (*f != '.') normal_char_found = true;
    else if (normal_char_found) break;
    f++;
  }
  
  if (*f == '.' && normal_char_found) {
    suffix = f+1;
    suffix_len = len - (f+1 - path);
    base_len -= (suffix_len + 1);
  }
}

}


