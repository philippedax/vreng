/************************************************************************
*
*  uxmlgrammar.cpp
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
#include <iostream>
#include <ubit/uappli.hpp>
#include <ubit/udom.hpp>
#include <ubit/uxmlgrammar.hpp>
using namespace std;
namespace ubit {

void UXmlGrammars::addGrammar(const UXmlGrammar& g) {
  glist.push_back(&g);
}

void UXmlGrammars::addGrammars(const UXmlGrammars& grammars) {
  const GrammarList& glist2 = grammars.glist;

  for (int k = 0; k < int(glist2.size()); ++k) {
    if (glist2[k]) glist.push_back(glist2[k]);
  }
}

const UClass* UXmlGrammars::getAttrClass(const UStr& classname) const {
  for (int k = int(glist.size()-1); k >= 0; --k) {
    const UClass* c = glist[k]->getAttrClass(classname);
    if (c) return c;
  }
  return null;
}

const UClass* UXmlGrammars::getElementClass(const UStr& classname) const {
  for (int k = int(glist.size()-1); k >= 0; --k) {
    const UClass* c = glist[k]->getElementClass(classname);
    if (c) return c;
  }
  return null;
}

unsigned short UXmlGrammars::getCharEntityRef(const UStr& entity_name) const {
  for (int k = int(glist.size()-1); k >= 0; --k) {
    unsigned short r = glist[k]->getCharEntityRef(entity_name);
    if (r) return r;
  }
  return 0;
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */

UXmlGrammar::UXmlGrammar() {
  // create ISO Character entity references
  createDefaultCharEntityRefs();
}

// ne detruit PLUS les classes: elles ont leur autonomie propre
// et continuent a exister
UXmlGrammar::~UXmlGrammar() {}

/* ==================================================== ===== ======= */

const UClass* UXmlGrammar::getElementClass(const UStr& classname) const {
  return element_classes.findClass(classname);
}

const UClass* UXmlGrammar::getAttrClass(const UStr& classname) const {
  return attr_classes.findClass(classname);
}

void UXmlGrammar::addAttrClass(const UClass& c) {
  attr_classes.addClass(c);
}

void UXmlGrammar::addElementClass(const UClass& c) {
  element_classes.addClass(c);
}

/* ==================================================== ===== ======= */

UXmlGrammar& UXmlGrammar::getSharedUndefGrammar() { // !!! A REVOIR !!!
  static UXmlGrammar& undef_grammar = *new UXmlGrammar();
  return undef_grammar;
}

UClass* UXmlGrammar::addUndefElementClass(const UStr& cname) {
  UClass* c = new UDefaultInlineElement::MetaClass(cname);  // !!! A REVOIR !!!
  getSharedUndefGrammar().addElementClass(*c); 
  return c;
}

UClass* UXmlGrammar::addUndefAttrClass(const UStr& cname) {
  UClass* c = new UDefaultAttribute::MetaClass(cname);  // !!! A REVOIR !!!
  getSharedUndefGrammar().addAttrClass(*c); 
  return c;
}

/* ==================================================== ===== ======= */

unsigned short UXmlGrammar::getCharEntityRef(const UStr& name) const {
  CharEntityRefMap::const_iterator k = char_entity_refs.find(&name);
  if (k == char_entity_refs.end()) return 0;
  else return k->second;
}

void UXmlGrammar::addCharEntityRef(const UStr& _name, unsigned short _value) {
  char_entity_refs[&ustr(_name)] = _value;
}

void UXmlGrammar::createDefaultCharEntityRefs() {
  struct CharEntityRef {UStr name; unsigned short value;};

  // ISO 8859-1 characters
  static CharEntityRef tab[] = {
  {"nbsp", 160},      // no-break space = non-breaking space,
  {"iexcl", 161},     // inverted exclamation mark
  {"cent", 162},      // cent sign, U+00A2 ISOnum
  {"pound", 163},     // pound sign, U+00A3 ISOnum
  {"curren", 164},    // currency sign, U+00A4 ISOnum
  {"yen", 165},       // yen sign = yuan sign, U+00A5 ISOnum
  {"brvbar", 166},    // broken bar = broken vertical bar
  {"sect", 167},      // section sign, U+00A7 ISOnum
  {"uml", 168},       // diaeresis = spacing diaeresis
  {"copy", 169},      // copyright sign, U+00A9 ISOnum
  {"ordf", 170},      // feminine ordinal indicator
  {"laquo", 171},     // left-pointing double angle quotation mark
  {"not", 172},       // not sign, U+00AC ISOnum
  {"shy", 173},       // soft hyphen = discretionary hyphen
  {"reg", 174},       // registered sign = registered trade mark sign
  {"macr", 175},      // macron = spacing macron = overline
  {"deg", 176},       // degree sign, U+00B0 ISOnum
  {"plusmn ", 177},   // plus-minus sign = plus-or-minus sign
  {"sup2", 178},      // superscript two = superscript digit two
  {"sup3", 179},      // superscript three = superscript digit three
  {"acute", 180},     // acute accent = spacing acute
  {"micro", 181},     // micro sign, U+00B5 ISOnum
  {"para", 182},      // pilcrow sign = paragraph sign
  {"middot", 183},    // middle dot = Georgian comma = Greek middle dot
  {"cedil", 184},     // cedilla = spacing cedilla, U+00B8 ISOdia
  {"sup1", 185},      // superscript one = superscript digit one
  {"ordm", 186},      // masculine ordinal indicator
  {"raquo", 187},     // right-pointing double angle quotation mark = right pointing guillemet
  {"frac14", 188},    // vulgar fraction one quarter = fraction one quarter
  {"frac12", 189},    // vulgar fraction one half = fraction one half
  {"frac34", 190},    // vulgar fraction three quarters = fraction three quarters
  {"iquest", 191},    // inverted question mark = turned question mark
  {"Agrave", 192},    // latin capital letter A with grave = latin capital letter A grave
  {"Aacute", 193},    // latin capital letter A with acute,
  {"Acirc", 194},     // latin capital letter A with circumflex,
  {"Atilde", 195},    // latin capital letter A with tilde,
  {"Auml", 196},      // latin capital letter A with diaeresis,
  {"Aring", 197},     // latin capital letter A with ring above = latin capital letter A ring,
  {"AElig", 198},     // latin capital letter AE = latin capital ligature AE,
  {"Ccedil", 199},    // latin capital letter C with cedilla,
  {"Egrave", 200},    // latin capital letter E with grave,
  {"Eacute", 201},    // latin capital letter E with acute,
  {"Ecirc", 202},     // latin capital letter E with circumflex,
  {"Euml", 203},      // latin capital letter E with diaeresis,
  {"Igrave", 204},    // latin capital letter I with grave,
  {"Iacute", 205},    // latin capital letter I with acute,
  {"Icirc", 206},     // latin capital letter I with circumflex,
  {"Iuml", 207},      // latin capital letter I with diaeresis,
  {"ETH", 208},       // latin capital letter ETH, U+00D0 ISOlat1
  {"Ntilde", 209},    // latin capital letter N with tilde,
  {"Ograve", 210},    // latin capital letter O with grave,
  {"Oacute", 211},    // latin capital letter O with acute,
  {"Ocirc", 212},     // latin capital letter O with circumflex,
  {"Otilde", 213},    // latin capital letter O with tilde,
  {"Ouml", 214},    // latin capital letter O with diaeresis,
  {"times", 215},   // multiplication sign, U+00D7 ISOnum
  {"Oslash", 216},  // latin capital letter O with stroke = latin capital letter O slash,
  {"Ugrave", 217},  // latin capital letter U with grave,
  {"Uacute", 218},  // latin capital letter U with acute,
  {"Ucirc", 219},   // latin capital letter U with circumflex,
  {"Uuml", 220},    // latin capital letter U with diaeresis,
  {"Yacute", 221},  // latin capital letter Y with acute,
  {"THORN", 222},   // latin capital letter THORN,
  {"szlig", 223},   // latin small letter sharp s = ess-zed,
  {"agrave", 224},  // latin small letter a with grave = latin small letter a grave
  {"aacute", 225},  // latin small letter a with acute,
  {"acirc", 226},   // latin small letter a with circumflex,
  {"atilde", 227},  // latin small letter a with tilde,
  {"auml", 228},    // latin small letter a with diaeresis,
  {"aring", 229},   // latin small letter a with ring above= latin small letter a ring
  {"aelig", 230},   // latin small letter ae = latin small ligature ae
  {"ccedil", 231},  // latin small letter c with cedilla,
  {"egrave", 232},  // latin small letter e with grave,
  {"eacute", 233},  // latin small letter e with acute,
  {"ecirc", 234},   // latin small letter e with circumflex,
  {"euml", 235},    // latin small letter e with diaeresis,
  {"igrave", 236},  // latin small letter i with grave,
  {"iacute", 237},  // latin small letter i with acute,
  {"icirc", 238},   // latin small letter i with circumflex,
  {"iuml", 239},    // latin small letter i with diaeresis,
  {"eth", 240},     // latin small letter eth, U+00F0 ISOlat1
  {"ntilde", 241},  // latin small letter n with tilde,
  {"ograve", 242},  // latin small letter o with grave,
  {"oacute", 243},  // latin small letter o with acute,
  {"ocirc", 244},   // latin small letter o with circumflex,
  {"otilde", 245},  // latin small letter o with tilde,
  {"ouml", 246},    // latin small letter o with diaeresis,
  {"divide", 247},  // division sign, U+00F7 ISOnum
  {"oslash", 248},  // latin small letter o with stroke = latin small letter o slash
  {"ugrave", 249},  // latin small letter u with grave,
  {"uacute", 250},  // latin small letter u with acute,
  {"ucirc", 251},   // latin small letter u with circumflex,
  {"uuml", 252},    // latin small letter u with diaeresis,
  {"yacute", 253},  // latin small letter y with acute,
  {"thorn", 254},   // latin small letter thorn,
  {"yuml", 255},    // latin small letter y with diaeresis,

    // !!!  mathematical is missing

    // html: markup-significant and internationalization characters
  {"gamma",947},    // greek small letter gamma,
  {"larr",8592},    // leftwards arrow
  {"quot",34},      // quotation mark = APL quote,
  {"amp",38},       // ampersand
  {"lt",60},        // less-than sign
  {"gt",62},        // greater-than sign
  {"OElig",338},    // latin capital ligature OE,
  {"oelig",339},    // latin small ligature oe, U+0153 ISOlat2
  {"Scaron",352},   // latin capital letter S with caron,
  {"scaron",353},   // latin small letter s with caron,
  {"Yuml",376},     // latin capital letter Y with diaeresis
  {"circ",710},     // modifier letter circumflex accent
  {"tilde",732},    // small tilde
  {"ensp",8194},    // en space
  {"emsp",8195},    // em space
  {"thinsp",8201},  // thin space
  {"zwnj",8204},    // zero width non-joiner
  {"zwj",8205},     // zero width joiner
  {"lrm",8206},     // left-to-right mark
  {"rlm",8207},     // right-to-left mark
  {"ndash",8211},   // en dash
  {"mdash",8212},   // em dash
  {"lsquo",8216},   // left single quotation mark,
  {"rsquo",8217},   // right single quotation mark
  {"sbquo",8218},   // single low-9 quotation mark
  {"ldquo",8220},   // left double quotation mark
  {"rdquo",8221},   // right double quotation mark
  {"bdquo",8222},   // double low-9 quotation mark
  {"dagger",8224},  // dagger, U+2020 ISOpub
  {"Dagger",8225},  // double dagger, U+2021 ISOpub
  {"permil",8240},  // per mille sign, U+2030 ISOtech
  {"lsaquo",8249},  // single left-pointing angle quotation mark
  {"rsaquo",8250},  // single right-pointing angle quotation mark
  {"euro",8364},    // euro sign, U+20AC
    
  {"", 0},          // null terminated
  };

  for (int k = 0; !tab[k].name.empty(); k++)
    char_entity_refs[&tab[k].name] = tab[k].value;
}

}


