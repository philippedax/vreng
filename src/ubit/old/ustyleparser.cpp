/************************************************************************
 *
 *  ustyleparser.cpp
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
#include <clocale>
#include <ctype.h>
#include <string.h>
#include <ubit/ustyleparser.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
using namespace std;
namespace ubit {


void UStyleParser::skipSpaces() {                // skip comments
  while (true) {
    const UChar* begin = p;

    while(*p && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t')) p++;
  
    if (*p == '/' && *(p+1) == '*') {    // comment
      p += 2;
      while (*p) {
        if (*p == '*' && *(p+1) == '/') {p = p+2; break;}
        else p++;
      }
    }

    if (p == begin) return;
  } 
}

/* ==================================================== ===== ======= */

bool UStyleParser::readName(UStr& name) {
  if (!isalpha(*p) && *p!='_') return false; // 1st char must be in (alpha _ :)
  const UChar* begin = p;
  p++;

  while (isalnum(*p) || *p == '-' || *p == '_' || *p == '.') p++;
  if (!*p) return false;

  name.append(begin, p-begin);
  if (permissive) name.lower();
  return true;
}


bool UStyleParser::readValue(UStr& value) {
  if (iscntrl(*p)) return false;
  const UChar* begin = p;

  // !iscntrl() fait planter s'il y a un newline ou un tab
  while (*p && /*!iscntrl(*p) &&*/ *p != ';' && *p != '}') {
    if (*p == '/' && *(p+1) == '*') {    // comment
      if (p > begin) value.append(begin, p-begin);
      skipSpaces();
      begin = p;
    }
    else p++;
  }
  //if (*p!= 0 && iscntrl(*p)) return false;
  if (p > begin) value.append(begin, p-begin);
  return true;
}

/* ==================================================== ===== ======= */
// starts on the 1st char of the name

bool UStyleParser::readNameValuePair(UStr& name, UStr& value) {
  const UChar* begin = p;

  if (!readName(name)) {
    error("invalid CSS attribute name", begin);
    return false;
  }

  skipSpaces();

  if (*p == ':') p++;
  else {
    if (permissive) {
      value.append("");
      return true;
    }
    else {
      error("invalid CSS attribute '", name,
            "': no : sign after attribute name",begin);
      return false;
    }
  }

  skipSpaces();

  if (readValue(value)) return true;
  else {
    error("invalid CSS attribute '", name,"': incorrect value", begin);
    return false;
  }
}

/* ==================================================== [Elc] ======= */
// reads the properties of the style definition, ie. the {} part after the selector

bool UStyleParser::readStyleProps(StyleMaker& xxx) {
  const UChar* begin = p;
  if (*p == '{') p++;

  while (true) {
    skipSpaces();

    if (*p == ';') {
      p++;
      skipSpaces();
    }

    else if (!*p || *p == '}') {         // end of style def
      return true;
    }

    else if (isalpha(*p)) {
      UStr name, value;
      if (readNameValuePair(name, value)) 
        xxx.addProp(name, value);
    }

    else {
      unexpected("in CSS style def", begin-1);
      return false;
    }
  }
}

/* ==================================================== [Elc] ======= */

static void readBracketSelector(const char* n, UStr& aux) {
  const char* pn = n;
  char* out = (char*)malloc(strlen(n));
  char* po = out;
  
  while (*pn) {
    if (*pn == '=') {
      if (pn > n && (*(pn-1) == '~' || *(pn-1) == '|')) {  // not taken into account
        *(po-1) = *pn++;     // enlever;
      }
      else *po++ = *pn++;
    }
    else if (*pn == '"') pn++;     // enlever (ne pas copier);
    else *po++ = *pn++;
  }
  *po = 0;
  aux &= out;
  delete out;
}

/* ==================================================== ===== ======= */

static bool readSingleSelector(UStr& name) {
  const char* n = name.c_str();
  if (!n) return false;

  // virer le wilcard puisqu'il est equivalent a rien
  if (*n == '*') {name.remove(0,1); n = name.c_str();}
    
  UStr aux;
  const char* pn = n;
  while (*pn) {
    if (*pn == '.') {
      // convertir aaa.bbb en aaa[class=bbb] 
      aux.append(n, pn-n);
      aux.append("[class=");
      aux.append(pn+1);
      aux.append(']');
      name = aux;
      break;
    }
    else if (*pn == '#') {
      // convertir aaa#bbb en aaa[id=bbb]
      aux.append(n, pn-n);
      aux.append("[id=");
      aux.append(pn+1);
      aux.append(']');
      name = aux;
      break;
    }
    else if (*pn == '[') {
      aux.append(n, pn-n);
      readBracketSelector(pn, aux);      
      name = aux;
      break;
    }
    else pn++;
  }

  return true;
}

/* ==================================================== ===== ======= */
// reads the selector(s) of the style definition.

bool UStyleParser::readStyleSelectors(StyleMaker& xxx) {
  const UChar* begin = p;
  bool multiple_sel = false;

  UStr sel;

  while (*p && *p != '{') {
    if (*p == '/' || *(p+1) == '*') {    // comment
      if (p > begin) sel.append(begin, p-begin);
      skipSpaces();
      begin = p;
    }
    else if (*p == ',') {  // multiple selectors
      multiple_sel = true;
      p++;
    }
    else p++;
  }

  if (!*p) return false;

  sel.append(begin, p-begin);
  sel.trim();
  if (permissive) sel.lower();

  if (!multiple_sel) {
    readSingleSelector(sel);
    xxx.count = 1;
    *(xxx.selectors[0]) = sel;
  }
  else {
    xxx.count = sel.tokenize(xxx.selectors, ",");
    for (unsigned int k = 0; k < xxx.count; k++)
      readSingleSelector(*(xxx.selectors[k]));
  }
 
  return true;
}

/* ==================================================== [Elc] ======= */
// reads one complete style definition (= the selector and the value part).

bool UStyleParser::readStyleDef(StyleMaker& xxx) {
  const UChar* begin = p;

  xxx.count = 0;
  //xxx.sel.clear();
  xxx.begin();

  if (!readStyleSelectors(xxx)) {
    error("invalid style selector", begin);
    return false;
  }
    
  skipSpaces();
  
  if (*p == '{') p++;
  else {
    error("invalid style declaration '", *xxx.selectors[0],
          "': no { sign after style name", begin);
    xxx.end(false);
    return false;
  }

  skipSpaces();

  xxx.create();

  if (readStyleProps(xxx)) {
    xxx.end(true);
    return true;
  }
  else {
    xxx.end(false);
    error("invalid style declaration '", *xxx.selectors[0],
          "': incorrect value", begin);
    return false;
  }
}

/* ==================================================== [Elc] ======= */
// reads all style definitions included in the buffer.

int UStyleParser::parseImpl(StyleMaker& xxx, const UStr& _buffer) {
  p = text_buffer = _buffer.c_str();
  if (!p || !*p) {
    p = text_buffer = null;
    return 0;
  }
  
  stat = 1;
  
  while (*p) {
    skipSpaces();
    if (!*p) break;
    
    if (readStyleDef(xxx)) p++;
    else {
      stat = 0;
      break;
    }
  }

  p = text_buffer = null;
  return stat;
}

/* ==================================================== ===== ======= */

UStyleParser::UStyleParser() :
  permissive(true),
  stat(0),
  text_buffer(null),
  p(null),
  perrhandler(UAppli::getErrorHandler()) {
}

// une fois pour toutes: 
// les destr DOIVENT etre definis dans les .ccp qunad il y a des uptr<XXX> 
// pour eviter d'etre oblige d'inclure la def de XXX dans le .hpp

UStyleParser::~UStyleParser() {}


UStyleParser::StyleMaker::StyleMaker() {
  // il faut TOUJOURS au moins un element sinon plantage !
  selectors.push_back(new UStr());
}

UStyleParser::StyleMaker::~StyleMaker() {
  for (unsigned int k = 0; k < selectors.size(); k++)
    delete selectors[k];
}

/* ==================================================== [Elc] ======= */

void UStyleParser::unexpected(const char* msg, const UChar* line) {
  if (!*p) {
    error("premature end of file ", "", msg, line);
  }
  else if (iscntrl(*p)) {
    char s[] = "invalid character ' '";
    s[sizeof(s)-3] = *p;
    // NB: + avec un char deconne avec g++
    error(s, "", msg, line);
    p++;
  }
  else  {
    char s[] = "unexpected character ' '";
    s[sizeof(s)-3] = *p;
    error(s, "", msg, line);
    p++;
  }
}

void UStyleParser::error(const char* msg, const UChar* line) {
  error(msg, "", 0, line);
}

void UStyleParser::error(const char* msg1, const UStr& name,
                      const char* msg2, const UChar* line) {
  perrhandler->parserError(UError::CSS_ERROR, text_buffer, msg1, name, msg2, line);
}


}

