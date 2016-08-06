/************************************************************************
 *
 *  ucss.cpp: HTML Style Sheets
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ubit.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ucss.hpp>
#include <ubit/udom.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct UCssMaker: public UStyleParser::StyleMaker {
  UXmlDocument* doc;
  UAttrList* attributes;

  UCssMaker(UXmlDocument* d, UAttrList* alist) : doc(d), attributes(alist) {}

  virtual void begin() {
    attributes = null;
  }

  virtual void end(bool ok) {
    attributes = null;
    // delete ? ....;
  }

  virtual void create() {
    for (unsigned int k = 0; k < count; k++) {
      const UClass* c = doc->getStyleSheet().obtainClass(*selectors[k]);
      //cerr << " *** " << *selectors[k] << endl;
      if (k == 0) {
        attributes = c->getAttributes();      //attributes partagee !!!
        if (!attributes) attributes = new UAttrList;
      }
      c->setAttributes(attributes);
    }
  }
   
  virtual void addProp(const UStr& name, const UStr& val) {
    static UCssProps css_props;   // !!!
    
    if (!attributes) 
      UAppli::error("UCssMaker","this UCssMaker object (%p) has no prop list", this);

    UCssProps::AddPropFunc func = css_props.findAddPropFunc(name);
    if (func) func(doc, attributes, val);
    /*
    UAttribute* p = c ? c(doc, val) : null;
    if (p) {
      //cerr << name << " " << val << endl;
      props->addAttr(*p);
    }
     */
 }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UCssParser::parse(const UStr& buf, UXmlDocument* doc) {
  UCssMaker* smaker = new UCssMaker(doc, null);
  parseImpl(*smaker, buf);
  delete smaker;
  return stat;
}

int UCssParser::parseAttr(const UStr& buf, UXmlDocument* doc, UAttrList* props) {
  p = text_buffer = buf.c_str();
  if (!p || !*p) {
    p = text_buffer = null;
    return 0;
  }

  UCssMaker* smaker = new UCssMaker(doc, props);
  readStyleProps(*smaker);

  delete smaker;
  return stat;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UStyleProps::defProp(const char* propname, UStyleProps::AddPropFunc func) {
  prop_map[new UStr(propname)] = func;
}

void UStyleProps::defProp(const UStr& propname, UStyleProps::AddPropFunc func) {
  prop_map[new UStr(propname)] = func;
}

UStyleProps::AddPropFunc UStyleProps::findAddPropFunc(const UStr& propname)  {
   PropMap::const_iterator k = prop_map.find(&propname);
   if (k == prop_map.end()) return 0;
   else return (k->second);
 }

UStyleProps::~UStyleProps() {
  for (PropMap::iterator k = prop_map.begin(); k != prop_map.end(); k++)
    delete k->first;
}

/*
 //static bool isEq(const UStr& s1, const char* s2);
 //static bool isEq(const UStr* s1, const char* s2);
 
bool UStyleProps::isEq(const UStr& s1, const char* s2) {
  return s1.equals(s2, true);      
}

bool UStyleProps::isEq(const UStr* s1, const char* s2) {
  return s1 ? s1->equals(s2, true) : false;
}

// QUE POSITIF !!!
bool UStyleProps::parseNum(const UStr& s, float& val, UStr& unit) {

bool UStyleProps::parseNum(const UStr* s, float& val, UStr& unit) {
  return s ? parseNum(*s, val, unit) : false;
}
*/
/* ==================================================== ======== ======= */

bool UStyleProps::parseUrl(const UStr& s, UStr& url, UStr& remain) {
  url.clear(); remain.clear();
  int begin = s.find("url(");

  if (begin >= 0) {
    int end = s.find(")");
    if (end > begin) {
      url.insert(0, s, begin+4, end-(begin+4));
      url.trim();
      if (url[0] == '"' || url[0] == '\'') url.remove(0,1);
      if (url[-1] == '"' || url[-1] == '\'') url.remove(-1,1);

      remain.insert(0, s, end+1);
      remain.trim();
      //cerr << "getURL '" << url  <<"' remain: " << remain << endl;
      return true;
    }
  }
  remain = s;
  return false;
}

/* ==================================================== ======== ======= */

UCssProps::UCssProps() {  
  defProp("font-family",     create_font_family);
  defProp("font-size",       create_font_size);
  defProp("font-weight",     create_font_weight);
  defProp("font-style",      create_font_style);
  //defProp("font",          create_font);  etc...

  defProp("color",           create_color);
  defProp("background-color",create_background_color);
  defProp("background-image",create_background_image);
  defProp("background",      create_background);

  defProp("width",           create_width);
  defProp("height",          create_height);
  defProp("text-align",      create_text_align);       // ~~ align
  defProp("vertical-align",  create_vertical_align);   // ~~ valign

  // text-indent
  //'text-decoration'
  // direction
  //  'position'static | relative | absolute | fixed | inherit
  //  'top'  'right'  'bottom' 'left'

  /*
   defProp("margin-top",     create_margin_top);   A REVOIR
   defProp("margin-bottom",  create_margin_bottom);
   defProp("margin-left",    create_margin_left);
   defProp("margin-right",   create_margin_right);
   //defProp("margin",       create_margin);
   */

  defProp("padding-top",     create_padding_top);
  defProp("padding-bottom",  create_padding_bottom);
  defProp("padding-left",    create_padding_left);
  defProp("padding-right",   create_padding_right);
  //defProp("padding",       create_padding);

  defProp("border",          create_border);
  // border-*....
  // table_* ...
}

/* ==================================================== ======== ======= */

void UCssProps::create_font_family(UXmlDocument*, UAttrList* props, const UStr& v) {
  //UFont* p = new UFont;
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) props->obtainAttr<UFont>().setFamily(v);  //  trim ??
}

void UCssProps::create_font_size(UXmlDocument*, UAttrList* props, const UStr& v) {
  //UFont* p = new UFont;
  if (!v.empty()) props->obtainAttr<UFont>().setSize(v);  //  trim ??
}

void UCssProps::create_font_weight(UXmlDocument*, UAttrList* props, const UStr& v) {
  //UFont* p = new UFont;
  if (v.empty()) return;
  UFont& f = props->obtainAttr<UFont>();

  if (v.equals("bold",true)) f.setBold(true);
  else /* if (isEq(v,"medium")) */ f.setBold(false);
}

void UCssProps::create_font_style(UXmlDocument*, UAttrList* props, const UStr& v) {
  //UFont* p = new UFont();
  if (v.empty()) return;
  UFont& f = props->obtainAttr<UFont>();

  if (v.equals("italic",true)) f.setItalic(true);
  else /* if (isEq(v,"normal")) */ f.setItalic(false);
}

//UAttribute* UCssProps::create_font(const UStr& v) {
//return p;
//}

// etc...

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UCssProps::create_color(UXmlDocument*, UAttrList* props, const UStr& v) {
  UColor* p = new UColor();
  //trim ??
  //p->set(v);
  p->setNamedColor(v);
  props->addAttr(*p);
}

void UCssProps::create_background_color(UXmlDocument*, UAttrList* props, 
                                        const UStr& v) {
  UBackground* p = new UBackground();
  //trim ??
  if (v.equals("transparent",true)) p->setColor(UColor::none);
  else if (!v.empty()) p->setNamedColor(v);
  props->addAttr(*p);
}

void UCssProps::create_background_image(UXmlDocument* doc, UAttrList* props, 
                                        const UStr& v) {
  UBackground* p = new UBackground();
  UStr url, remain;
  //trim
  if (!parseUrl(v, url, remain)) {
    props->addAttr(*p);
    return;
  }
  
  if (v.equals("none",true)) ;   // rien a faire ?...
  else {
    UStr path; doc->makePath(path, url);
    UIma* ima = new UIma(path);
    p->setIma(*ima);
    static UStr name = "background_image";
    doc->addAttachment(new UImgAttachment(url, name, ima));
  }
  props->addAttr(*p);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UCssProps::create_background(UXmlDocument* doc, UAttrList* al, const UStr& v) {
  UBackground* p = new UBackground();
  //trim
  UStr url, remain;
  
  if (!parseUrl(v, url, remain)) {
    if (!v.empty()) p->setNamedColor(v);
  }
  else  {
    UStr path; doc->makePath(path, url);
    UIma* ima = new UIma(path);
    // EX: if (!remain.empty()) p->set(*ima, *new UBgcolor(remain));   // !! %%
    //else 
    *p = *ima;
    static UStr name = "background";
    doc->addAttachment(new UImgAttachment(url, name, ima));
  }  
  al->addAttr(*p);
}

/* ==================================================== ======== ======= */

void UCssProps::create_width(UXmlDocument*, UAttrList* al, const UStr& v) {
  // EX: UWidth* p = new UWidth(-1);
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) al->obtainAttr<USize>().setWidth(v);
}

void UCssProps::create_height(UXmlDocument*, UAttrList* al, const UStr& v) {
  // EX: UHeight* p = new UHeight(-1);
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) al->obtainAttr<USize>().setHeight(v);
}

// egalement: 'line-height'

/* ==================================================== ======== ======= */

void UCssProps::create_padding_top(UXmlDocument*, UAttrList* al, const UStr& v) {
  // NB: obtainAttr() add attribute to props
  if (!v.empty()) al->obtainAttr<UPadding>().setTop(v);
}

void UCssProps::create_padding_bottom(UXmlDocument*, UAttrList* al, const UStr& v) {
  if (!v.empty()) al->obtainAttr<UPadding>().setBottom(v);
 }

void UCssProps::create_padding_left(UXmlDocument*, UAttrList* al, const UStr& v) {
  if (!v.empty()) al->obtainAttr<UPadding>().setLeft(v);
}

void UCssProps::create_padding_right(UXmlDocument*, UAttrList* al, const UStr& v) {
  if (!v.empty()) al->obtainAttr<UPadding>().setRight(v);
}

//UCssProps::create_padding

/* ==================================================== ======== ======= */
/*  align n'existe pas, text-align joue son role ?
 void create_align(UXmlDocument*, const UStr& v) {
 ?? trim()
 if (isEq(UXmlAttr::value,"left")) set(UHalign::left);
 else if (isEq(UXmlAttr::value,"center")) set(UHalign::center);
 else if (isEq(UXmlAttr::value,"right"))  set(UHalign::right);
 props->addAttr(*p);
 }
 */

void UCssProps::create_text_align(UXmlDocument*, UAttrList* al, const UStr& v) {
  UHalign* p = new UHalign();
  //trim ??
  if (v.equals("left",true)) *p = UHalign::left;
  else if (v.equals("center",true)) *p = UHalign::center;
  else if (v.equals("right",true))  *p = UHalign::right;
  // A COMPLETR:   justify, etc...
  al->addAttr(*p);
}

// valign existe pas: vertical_align joue son role ?
//
void UCssProps::create_vertical_align(UXmlDocument*, UAttrList* al, const UStr& v) {
  UValign* p = new UValign();
  //trim ??
  if (v.equals("top",true)) *p = UValign::top;
  else if (v.equals("middle",true)) *p = UValign::center;
  else if (v.equals("bottom",true)) *p = UValign::bottom;
  // baseline et bien d'autres.....
  al->addAttr(*p);
}

/*
 'text-indent'
 'text-decoration'
 'position'static | relative | absolute | fixed | inherit
 'top'  'right'  'bottom' 'left'
 direction
 */

/* ==================================================== ======== ======= */

void UCssProps::create_border(UXmlDocument*, UAttrList* al, const UStr& v) {
  UBorder* p = new UBorder();
  UStr unit;
  float n = 0;
  if (!v.scanValue(n, unit)) {
    if (n > 1) p->setDecoration(UBorder::LINE);   // A COMPLETER
    else p->setDecoration(UBorder::NONE);
  }
  al->addAttr(*p);
}

/* ==================================================== ======== ======= */
/* TABLES:
'border-collapse' collapse | separate | inherit
'border-spacing' <length> <length>? | inherit
remplacent cellspacing et cellpadding

'border-style'
'caption-side' top | bottom | inherit
*/

/* ==================================================== ======== ======= */

UStyle* UCssStyles::create_body_style() {
  //UStyle* style = UVbox::createStyle();
  UStyle* style = UBox::createStyle();
  style->setVertPadding(8,8);
  UFont* f = new UFont(); f->setPointSize(12); style->setFont(f);
  return style;
}

UStyle* UCssStyles::create_div_style() {
  UStyle* style = UFlowbox::createStyle();
  style->local.padding.set(6,1);
  return style;
}

UStyle* UCssStyles::create_p_style() {
  UStyle* style = UFlowbox::createStyle();
  style->local.padding.set(6,1);
  return style;
}

/* ==================================================== ======== ======= */

UStyle* UCssStyles::create_ul_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setHorizPadding(30, 0);
  style->setVertPadding(1, 0);
  return style;
}

UStyle* UCssStyles::create_ol_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setHorizPadding(30, 1);
  style->setVertPadding(1, 0);
  return style;
}

UStyle* UCssStyles::create_li_style() {
  UStyle* style = UFlowbox::createStyle();
  style->local.padding.set(1, 1);
  //style->local.content = new UElem(UPix::rball);
  style->local.content = new UElem(" - ");
  return style;
}

/* ==================================================== ======== ======= */

UStyle* UCssStyles::create_pre_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setHorizPadding(80,0);
  style->setVertPadding(5,5);
  style->setFont(&UFont::monospace);
  return style;
}

UStyle* UCssStyles::create_blockquote_style() {
  UStyle* style = UFlowbox::createStyle();
  style->local.padding.set(20, 8);
  return style;
}

UStyle* UCssStyles::create_center_style() {
  UStyle* style = UFlowbox::createStyle();
  style->local.padding.set(1, 1);
  style->halign = UValign::CENTER;
  return style;
}

/* ==================================================== [Elc] ======= */

UStyle* UCssStyles::create_h1_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setVertPadding(14,14);
  UFont* f = new UFont(UFont::bold); f->setPointSize(32); style->setFont(f);
  return style;
}

UStyle* UCssStyles::create_h2_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setVertPadding(12,12);
  UFont* f = new UFont(UFont::bold); f->setPointSize(18); style->setFont(f);
  return style;
}

UStyle* UCssStyles::create_h3_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setVertPadding(10,10);
  UFont* f = new UFont(UFont::bold); f->setPointSize(14); style->setFont(f);
  return style;
}

UStyle* UCssStyles::create_h4_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setVertPadding(8,8);
  UFont* f = new UFont(UFont::bold); f->setPointSize(12); style->setFont(f);
  return style;
}

UStyle* UCssStyles::create_h5_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setVertPadding(7,7);
  UFont* f = new UFont(UFont::bold); f->setPointSize(10); style->setFont(f);
  return style;
}

UStyle* UCssStyles::create_h6_style() {
  UStyle* style = UFlowbox::createStyle();
  style->setVertPadding(6,6);
  UFont* f = new UFont(UFont::bold); f->setPointSize(8); style->setFont(f);
  return style;
}

/* ==================================================== ======== ======= */

UStyle* UCssStyles::create_table_style() {
  UStyle* style = UTable::createStyle();
  style->valign = UValign::BOTTOM;
  style->halign = UHalign::LEFT;
  return style;
}

UStyle* UCssStyles::create_tr_style() {
  UStyle* style = UTrow::createStyle();
  style->hspacing = 0;
  style->valign = UValign::BOTTOM;
  style->halign = UHalign::LEFT;
  return style;
}

UStyle* UCssStyles::create_td_style() {
  UStyle* style = UTcell::createStyle();
  style->valign = UValign::BOTTOM;
  style->halign = UHalign::LEFT;
  return style;
}

UStyle* UCssStyles::create_th_style() {
  UStyle* style = UTcell::createStyle();
  return style;
}

/* ==================================================== ======== ======= */

UStyle* UCssStyles::create_span_style() {
  UStyle* style = UElem::createStyle();
  return style;
}

UStyle* UCssStyles::create_b_style() {
  UStyle* style = UElem::createStyle();
  style->setFont(&UFont::bold);
  return style;
}

UStyle* UCssStyles::create_i_style() {
  UStyle* style = UElem::createStyle();
  style->setFont(&UFont::italic);
  return style;
}

UStyle* UCssStyles::create_em_style() {
  UStyle* style = UElem::createStyle();
  //style->font = &UFont::fill;
  style->setColors(UColor::orange, UColor::white);
  return style;
}

UStyle* UCssStyles::create_u_style() {
  UStyle* style = UElem::createStyle();
  style->setFont(&UFont::underline);
  return style;
}

UStyle* UCssStyles::create_font_style() {
  UStyle* style = UElem::createStyle();
  return style;
}

UStyle* UCssStyles::create_img_style() {
  UStyle* style = UBox::createStyle();
  return style;
}

UStyle* UCssStyles::create_br_style() {
  UStyle* style = UElem::createStyle();
  return style;
}

UStyle* UCssStyles::create_a_style() {                  // A REVOIR  !!!@@@@
  UStyle* href_style = ULinkbutton::createStyle();
  href_style->setBgcolors(UColor::none);
  return href_style;
}

}
