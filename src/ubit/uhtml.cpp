/************************************************************************
 *
 *  uhtml.cpp: HTML parser and renderer
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ubit.hpp>
#include <ubit/udom.hpp>
#include <ubit/uhtml.hpp>
#include <ubit/ucss.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UHtmlParser::UHtmlParser() {
  static const UXmlGrammar& html_grammar = *new UHtmlGrammar();
  addGrammar(html_grammar);
  setPermissive(true);
  setCollapseSpaces(true);
}

void UHtmlAttribute::setValueImpl(uptr<UStr>& pvalue, const UStr& s) {
  if (pvalue) *pvalue = s; else pvalue = new UStr(s);
  pvalue->trim();  // !! attention: ce  n'est pas la norme XML !!
}

bool UHtmlAttribute::getValueImpl(const uptr<UStr>& pvalue, UStr& s) {
  if (pvalue) {s = *pvalue; return true;}
  else {s.clear(); return false;}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ELEMENTS

#define UBIT_CLASS_DEF(CLASSNAME, CLASS, NEWCLASS, NEWSTYLE, MODES) \
struct Subclass : public UClass { \
Subclass() : UClass(CLASSNAME) {} \
virtual int getParseModes() const {return MODES;} \
virtual bool isInstance(UObject& obj) const {return dynamic_cast<CLASS*>(&obj);} \
virtual CLASS* newInstance() const {return NEWCLASS;} \
virtual UStyle* newStyle() const {return NEWSTYLE;} \
};\
static  const UClass& Class() {static Subclass& c = *new Subclass; return c;} \
virtual const UClass& getClass() const {return Class();}


#define HEAD_ELEMENT_CLASS(CL, SUPER, MODES) \
struct UHtml_##CL : public SUPER { \
  UBIT_CLASS_DEF(#CL, UHtml_##CL, new UHtml_##CL, UHtml_##CL::createStyle(), MODES) \
  UHtml_##CL() {show(false);} \
  virtual void initNode(UDoc*); \
};

#define ELEMENT_CLASS(CL, SUPER, MODES) \
struct UHtml_##CL : public SUPER { \
  UBIT_CLASS_DEF(#CL, UHtml_##CL, new UHtml_##CL, UCssStyles::create_##CL##_style(), MODES) \
};

//NB: UHtml_html::createStyle, pas null sinon le layout sera incorrect
struct UHtml_html : public UElem {
  UCLASSDEF("html", UHtml_html, new UHtml_html)
};

//NB: styles desfinis dans UCssStyles
HEAD_ELEMENT_CLASS(head, UElem, 0)
HEAD_ELEMENT_CLASS(title, UElem, 0)
HEAD_ELEMENT_CLASS(meta, UElem, UClass::EMPTY_ELEMENT)
HEAD_ELEMENT_CLASS(link, UElem, UClass::EMPTY_ELEMENT)
HEAD_ELEMENT_CLASS(script, UElem, UClass::DONT_PARSE_CONTENT)
HEAD_ELEMENT_CLASS(style, UElem, UClass::DONT_PARSE_CONTENT)

ELEMENT_CLASS(body, UFlowbox, 0)
ELEMENT_CLASS(div, UFlowbox, 0)
ELEMENT_CLASS(p, UFlowbox, 0)
ELEMENT_CLASS(ul, UFlowbox, 0)
ELEMENT_CLASS(ol, UFlowbox, 0)
ELEMENT_CLASS(li, UFlowbox, 0)
ELEMENT_CLASS(pre, UFlowbox, UClass::PRESERVE_SPACES)
ELEMENT_CLASS(blockquote, UFlowbox, 0)
ELEMENT_CLASS(center, UFlowbox, 0)
ELEMENT_CLASS(h1,UFlowbox, 0)
ELEMENT_CLASS(h2,UFlowbox, 0)
ELEMENT_CLASS(h3,UFlowbox, 0)
ELEMENT_CLASS(h4,UFlowbox, 0)
ELEMENT_CLASS(h5,UFlowbox, 0)
ELEMENT_CLASS(h6,UFlowbox, 0)
ELEMENT_CLASS(table, UTable, 0)
ELEMENT_CLASS(tr, UTrow, 0)
ELEMENT_CLASS(td, UTcell, 0)
ELEMENT_CLASS(th, UTcell, 0)
ELEMENT_CLASS(span, UElem, 0)
ELEMENT_CLASS(font, UElem, 0)
ELEMENT_CLASS(b, UElem, 0)
ELEMENT_CLASS(i, UElem, 0)
ELEMENT_CLASS(em, UElem, 0)
ELEMENT_CLASS(u, UElem, 0)


#define ELEMENT_INIT_CLASS(CL, SUPER, MODES) \
struct UHtml_##CL : public SUPER { \
UBIT_CLASS_DEF(#CL, UHtml_##CL, new UHtml_##CL, UCssStyles::create_##CL##_style(), MODES) \
virtual void initNode(UDoc*); \
};
  
ELEMENT_INIT_CLASS(img, UBox, UClass::EMPTY_ELEMENT)
ELEMENT_INIT_CLASS(br, UElem, UClass::EMPTY_ELEMENT)

struct UHtml_a : public ULinkbutton {
  UBIT_CLASS_DEF("a", UHtml_a, new UHtml_a, UCssStyles::create_a_style(), 0) 
  virtual void initNode(UDoc*);
  //virtual const UStr* getHRef() const; ihnerited from ULinkbutton.
};
    
/* ==================================================== [Elc] ======= */

void UHtml_head::initNode(UDoc* doc) {
  UElem::initNode(doc);
}

void UHtml_title::initNode(UDoc* doc) {   // set title....
  UElem::initNode(doc);
}

void UHtml_meta::initNode(UDoc* doc) {    // set meta....
  UElem::initNode(doc);
}

void UHtml_script::initNode(UDoc* doc) {    // load script....
  UElem::initNode(doc);
}

void UHtml_br::initNode(UDoc* doc) {
  UElem::initNode(doc);
  add(ustr("\n"));
}

/* ==================================================== ===== ======= */
// reload reste a faire (complique: il faut mettre a jour l'arbre)

void UHtml_link::initNode(UDoc* doc) {
  UElem::initNode(doc);
  UXmlDocument* xmldoc = dynamic_cast<UXmlDocument*>(doc);
  if (xmldoc) {
    UStr href, rel, type;
    if (getAttrValue(href,"href") && !href.empty()
        && ((getAttrValue(rel,"rel") && rel.equals("stylesheet",true))
            || (getAttrValue(type,"type") && type.equals("text/css",true)))
        ) {
      UStr path; doc->makePath(path, href);
      UCssAttachment* a = new UCssAttachment(href, getNodeName(), xmldoc);
      xmldoc->addAttachment(a);
      a->load(xmldoc);
    }
  }
}

/* ==================================================== ===== ======= */

void UHtml_style::initNode(UDoc* doc) {
  UElem::initNode(doc);
  UXmlDocument* xmldoc = dynamic_cast<UXmlDocument*>(doc);
  if (xmldoc) {
    UChildIter i = cbegin();
    if (i != cend()) {
      UCssParser css;
      UStr* text = dynamic_cast<UStr*>(*i);    // UXmlText
      const UStr* val = text ? &text->getData() : null;
      if (val) css.parse(*val, xmldoc);
    }
  }
}

/* ==================================================== ===== ======= */

void UHtml_a::initNode(UDoc* doc) {
  UElem::initNode(doc);
  
  /*
   const UStr* hname = getAttr("hname");
   if (hname && !hname->empty()) {
   // rajouter dans la table des links .......
   }
   */
  
  UStr href;
  if (getAttrValue(href,"href") && !href.empty()) {
    UCall& c = ucall(doc, const_cast<const UStr*>(new UStr(href)), &UDoc::linkEventCB);
    addAttr(UOn::enter / c);
    addAttr(UOn::leave / c);
    addAttr(UOn::mpress / c);
    addAttr(UOn::mrelease / c);
    addAttr(UOn::action / c);
  }
}

/* ==================================================== ===== ======= */

void UHtml_img::initNode(UDoc* doc) {
  UElem::initNode(doc);
  
  // doit ignorer les events sinon les liens contenant des images
  // marcheront pas
  ignoreEvents();
  
  UXmlDocument* xmldoc = dynamic_cast<UXmlDocument*>(doc);
  if (xmldoc) {
    UStr src;
    if (getAttrValue(src,"src") && !src.empty()) {
      UStr path; xmldoc->makePath(path, src);
      UIma* ima = new UIma(path);   // att pas compat avec reload ima redef!
      add(ima);
      //                      CF AUSSI: tailles predefinies et rescale !!!
      
      xmldoc->addAttachment(new UImgAttachment(src, getNodeName(), ima));
    }
  }
}

/* ==================================================== ===== ======= */
// ATTRIBUTES

void UHtmlStyle::initNode(UDoc* doc, UElem*) {
  UXmlDocument* xmldoc = dynamic_cast<UXmlDocument*>(doc);
  if (xmldoc && pvalue && !pvalue->empty()) {
    UCssParser css;
    css.parseAttr(*pvalue, xmldoc, this);
  }
}

void UHtmlClass::initNode(UDoc* doc, UElem* parent) {
  //pourrait tester s'il y a au moins un attribut de ce nom dans la base
  //pourrait etre generalise a tous les attributs
  UXmlDocument* xmldoc = dynamic_cast<UXmlDocument*>(doc);
  UStr val;
  if (xmldoc && getValue(val)) xmldoc->setClassStyle(parent, getName(), val);
}

void UHtmlId::initNode(UDoc* doc, UElem* parent) {
  UXmlDocument* xmldoc = dynamic_cast<UXmlDocument*>(doc);
  UStr val;
  if (xmldoc && getValue(val)) xmldoc->setIdStyle(parent, getName(), val);
}

void UHtmlColor::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  setNamedColor(*pvalue);
}

void UHtmlBgcolor::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);  // fait trim() !
  setNamedColor(*pvalue);
}

void UHtmlBorder::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  UStr unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  if (n > 1) setDecoration(UBorder::LINE);   // A COMPLETER
  else setDecoration(UBorder::NONE);
}

void UHtmlFontFace::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  if (pvalue) setFamily(*pvalue);
}

void UHtmlFontSize::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  UStr unit; 
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  
  //cerr << "SIZE: " << n << " [" << unit << "]" << endl;
  // faudrait gerer taille relatives !!!
  switch(int(n)) {
    case 1 : setPixelSize(8); break;    // devrait etre parametrable
    case 2 : setPixelSize(10); break;
    case 3 : setPixelSize(12); break;
    case 4 : setPixelSize(14); break;
    case 5 : setPixelSize(16); break;
    case 6 : setPixelSize(20); break;
    case 7 : setPixelSize(24); break;
    case 8 : setPixelSize(28); break;
    case 9 : setPixelSize(34); break;
    default: setPixelSize(48); break;
  }
}

/*
void UHtmlUWidth::setValue(const UStr& s) {
  if (!pvalue) pvalue = new UStr();
  *pvalue = s;
  UWidth::set(s);
}
void UHtmlHeight::setValue(const UStr& s) {
  if (!pvalue) pvalue = new UStr();
  *pvalue = s;
  UHeight::set(s);
}
*/

void UHtmlUWidth::initNode(UDoc*, UElem* parent) {
  // NB: obtainAttr() add attribute to props
  if (pvalue && !pvalue->empty()) parent->obtainAttr<USize>().setWidth(*pvalue);
}

void UHtmlHeight::initNode(UDoc*, UElem* parent) {
  // NB: obtainAttr() add attribute to props
  if (pvalue && !pvalue->empty()) parent->obtainAttr<USize>().setHeight(*pvalue);
}

void UHtmlAlign::setValue(const UStr& v) {
    UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
    if (pvalue->equals("left",true)) *(UHalign*)this = UHalign::left;
    else if (pvalue->equals("center",true)) *(UHalign*)this = UHalign::center;
    else if (pvalue->equals("right",true)) *(UHalign*)this = UHalign::right;
  }

void UHtmlValign::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  if (pvalue->equals("top",true)) *(UValign*)this = UValign::top;
  else if (pvalue->equals("middle",true)) *(UValign*)this = UValign::center;
  else if (pvalue->equals("bottom",true)) *(UValign*)this = UValign::bottom;
}

void UHtmlColspan::initNode(UDoc* doc, UElem* parent) {
  UTcell* cell;
  if (parent && pvalue && (cell = dynamic_cast<UTcell*>(parent))) {
    int n = pvalue->toInt();
    if (n > 0) cell->setColspan(n);
  }
}
  
void UHtmlColspan::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  UStr unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty()) colspan = int(n);
}

void UHtmlRowspan::initNode(UDoc* doc, UElem* parent) {
  UTcell* cell;
  if (parent && pvalue && (cell = dynamic_cast<UTcell*>(parent))) {
    int n = pvalue->toInt();
    if (n > 0) cell->setRowspan(n);
  }
}
  
void UHtmlRowspan::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  UStr unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty()) rowspan = int(n);
}

void UHtmlCellspacing::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  UStr unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty() || UCSS::isEq(unit,"px")) set(i);   // A FAIRE
}

void UHtmlCellpadding::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  UStr unit;
  float n = 0;
  if (!pvalue->scanValue(n, unit)) return;
  //if (unit.empty() || UCSS::isEq(unit,"px")) set(i);   // A FAIRE
}

void UHtmlNowrap::setValue(const UStr& v) {
  UHtmlAttribute::setValueImpl(pvalue, v);     // ! fait trim() !
  // pas de valeur ..
  // A FAIRE
}

/* ==================================================== ===== ======= */

UHtmlGrammar::UHtmlGrammar() 
{  
  addElementClass(UHtml_html::Class());
  addElementClass(UHtml_head::Class());
  addElementClass(UHtml_title::Class());
  addElementClass(UHtml_meta::Class());
  addElementClass(UHtml_link::Class());
  addElementClass(UHtml_script::Class());
  addElementClass(UHtml_style::Class());

  addElementClass(UHtml_body::Class());
  addElementClass(UHtml_div::Class());
  addElementClass(UHtml_p::Class());
  addElementClass(UHtml_ul::Class());
  addElementClass(UHtml_ol::Class());
  addElementClass(UHtml_li::Class());
  addElementClass(UHtml_pre::Class());  
  addElementClass(UHtml_blockquote::Class());
  addElementClass(UHtml_center::Class());
  addElementClass(UHtml_h1::Class());
  addElementClass(UHtml_h2::Class());
  addElementClass(UHtml_h3::Class());
  addElementClass(UHtml_h4::Class());
  addElementClass(UHtml_h5::Class());
  addElementClass(UHtml_h6::Class());
  addElementClass(UHtml_table::Class());
  addElementClass(UHtml_tr::Class());
  addElementClass(UHtml_td::Class());
  addElementClass(UHtml_th::Class());
  addElementClass(UHtml_span::Class());
  addElementClass(UHtml_font::Class());
  addElementClass(UHtml_em::Class());
  addElementClass(UHtml_b::Class());
  addElementClass(UHtml_i::Class());
  addElementClass(UHtml_u::Class());
  addElementClass(UHtml_a::Class());
  addElementClass(UHtml_img::Class()); 
  addElementClass(UHtml_br::Class()); 
 
  //  - - - - - - - - - -- - - - - - - - - - - - - - -
    
  addAttrClass(UHtmlStyle::Class());
  addAttrClass(UHtmlClass::Class());
  addAttrClass(UHtmlId::Class());
  addAttrClass(UHtmlRel::Class());
  addAttrClass(UHtmlType::Class());
  addAttrClass(UHtmlAlt::Class());
  addAttrClass(UHtmlName::Class());
  addAttrClass(UHtmlSrc::Class());
  addAttrClass(UHtmlHref::Class());
  addAttrClass(UHtmlColor::Class());
  addAttrClass(UHtmlBgcolor::Class());
  addAttrClass(UHtmlFontFace::Class());
  addAttrClass(UHtmlFontSize::Class());
  addAttrClass(UHtmlUWidth::Class());
  addAttrClass(UHtmlHeight::Class());
  addAttrClass(UHtmlAlign::Class());
  addAttrClass(UHtmlValign::Class());
  addAttrClass(UHtmlBorder::Class());
  addAttrClass(UHtmlColspan::Class());
  addAttrClass(UHtmlRowspan::Class());
  addAttrClass(UHtmlCellspacing::Class());
  addAttrClass(UHtmlCellpadding::Class());
  addAttrClass(UHtmlNowrap::Class());
}

}
