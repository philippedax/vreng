/************************************************************************
 *
 *  udom.cpp: XML DOM Nodes
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

#include <typeinfo>
#include <iostream>
#include <cstdio>
#include <ubit/ubit_features.h>
#include <ubit/ubit.hpp>
#include <ubit/udom.hpp>
#include <ubit/uxmlgrammar.hpp>
#include <ubit/uxmlparser.hpp>
#include <ubit/uhtml.hpp>
#include <ubit/ucss.hpp>   // pour UCssAttachment
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

const UStr& UComment::getNodeName() const 
{static UStr nn("#comment"); return nn;}

const UStr& UCDATASection::getNodeName() const 
{static UStr nn("#cdata-section"); return nn;}

const UStr& UXmlDocument::getNodeName() const 
{static UStr nn("#document"); return nn;}

// NB: theoriquement ca devrait pas etre la mais ce n'est pas tres grave
bool UImgAttachment::isLoaded() const {
  return ima->isLoaded();
}

int UImgAttachment::load(UDoc*) {
  return ima->loadNow();  // always reloads the file
}

int UCssAttachment::load(UDoc*) {
  if (!doc || url.empty()) return 0;
  UStr path; 
  doc->makePath(path, url);
  UStr style;
  stat = style.read(path);
  //cerr << "link stylesheet: " << stat << " pathname " << " " << path << endl;
  if (stat > 0) {
    UCssParser css;
    css.parse(style, doc);
  }
  return stat;
}

/* ==================================================== [(C)Elc] ======= */

//UXmlDocType::UXmlDocType()
//: name(null), public_id(null), system_id(null) {}

UXmlDocType::UXmlDocType(const UStr& _name, const UStr& _public_id,
                         const UStr& _system_id)
: name(ustr(_name)), public_id(ustr(_public_id)), system_id(ustr(_system_id)) {
}

UXmlDocType::~UXmlDocType() {}

/* ==================================================== ======== ======= */

UXmlDocument::UXmlDocument() : UDoc() {
  constructs();
}

UXmlDocument::UXmlDocument(const UStr& _pathname) : UDoc(_pathname) {
  constructs();
}

void UXmlDocument::constructs() {
  doc_type = null;
  grammars = new UXmlGrammars;
  // c'est la grammaire par defaut pour les elements inconnus
  // elle est partagee, ce qui peut etre genant si sa taille devient grande
  // mais ceci permet que les UXmlElements pointent toujours sur des
  // classes qui continuent d'exister meme si les XmlDox correspondants
  // on ete detruits
  // ce mecanisme sera peut-etre a revoir ulterieurement....
  addGrammar(UXmlGrammar::getSharedUndefGrammar());
  
  // doit etre fait apres addGrammar() sinon ca plantera
  doc_elem = createElement("DocumentElement");  
  add(doc_elem);
  xml_version = new UStr();
  xml_encoding = new UStr("1.0");
  xml_standalone = true;
}

UXmlDocument::~UXmlDocument() {
  delete doc_elem;  // faudrait un uptr
  
  // NB: detruit le handle, pas les grammaires (pour des raisons de perfs
  // et car sinon les XmlNodes qui pointent dessus pourrainet planter
  delete grammars;

  //link_listener: externe: ne pas detruire
}



void UXmlDocument::addGrammar(const UXmlGrammar& g) {
  grammars->addGrammar(g);
}

/* ==================================================== [(C)Elc] ======= */

UXmlCreator::UXmlCreator() {
  load_objects = true;
  keep_comments = false;
  permissive = false;
  collapse_spaces = false;
}

UHtmlCreator::UHtmlCreator() {
  permissive = true;
  collapse_spaces = true;
}

UDoc* UXmlCreator::create(UDocSource& so) {
  UXmlDocument* doc = null;

  //UXmlParser parser;   // ATT !!!
  UHtmlParser parser; 
  parser.setPermissive(permissive);
  parser.setCollapseSpaces(collapse_spaces);
  
  if (so.from == UDocSource::PATHNAME) {
    doc = parser.read(*so.fullpath/*, so.errors*/);
    so.stat = parser.getStatus();
  }
  else if (so.from == UDocSource::BUFFER) {
    doc = parser.parse(*so.path, so.buffer ? *so.buffer : ""/*, so.errors*/);
    so.stat = parser.getStatus();
  }
  else if (so.from == UDocSource::SCRATCH) {
    doc = new UXmlDocument(*so.path);
    so.stat = 1;
  }
  else {
    so.stat = 0;
    return null;
  }
  
  return doc;
}

/* ==================================================== [(C)Elc] ======= */

UStr* UXmlDocument::createTextNode(const UStr& _data) {
  return new UStr(_data);
}

UComment* UXmlDocument::createComment(const UStr& _data) {
  return new UComment(_data); 
}

UComment::UComment(const UStr& _data) 
: data(new UStr(_data)) {}


UCDATASection* UXmlDocument::createCDATASection(const UStr& _data) {
  return new UCDATASection(_data); 
}

UCDATASection::UCDATASection(const UStr& _data) 
: data(new UStr(_data)) {}

// ==================================================== ======== 

UProcessingInstruction*
UXmlDocument::createProcessingInstruction(const UStr& _target, const UStr& _data) {
  return new UProcessingInstruction(_target, _data); 
}

//UProcessingInstruction::UProcessingInstruction()
//: target(null), data(null) {}

UProcessingInstruction::UProcessingInstruction(const UStr& _target, 
                                               const UStr& _data)
: target(ustr(_target)), data(ustr(_data)) {}

// ==================================================== ======== 

UAttr* UXmlDocument::createAttribute(const UStr& name) {
  // search if UAttributeClass in grammar
  const UClass* c = grammars->getAttrClass(name);
  
  // create+add default AttrClass otherwise:
  if (!c) c = UXmlGrammar::addUndefAttrClass(name); 
  
  // @@@ cast necessaire a cause pbm de refs croisees dans uclass.hpp
  return c->newInstance()->toAttr();
}

// ==================================================== ======== 

UElem* UXmlDocument::createElement(const UStr& name) {
  // search if UElemClass in grammar
  const UClass* c = grammars->getElementClass(name);
  
  // create+add default ElemClass otherwise:
  if (!c) c = UXmlGrammar::addUndefElementClass(name);
  
  // @@@ cast necessaire a cause pbm de refs croisees dans uclass.hpp
  return c->newInstance()->toElem();
}

void UXmlDocument::initElement(UElem* e) {
  const UStr& nname = e->getNodeName();
  //if (nname) {
    const UClass* c = getStyleSheet().findClass(nname);
    //il faut le mettre en premier!!!
    // cette fonctionnalite n'est pas standard !!!
    if (c) e->addImpl1(c->getAttributes(), e->abegin(), e->attributes());  // reafficher ????
  //}
  e->initNode(this);
}

/* ==================================================== ======== ======= */
// probleme: faire en sorte que les proplists soient dans le bon ordre et
// qu'elles soient avant les proplist des styles attributes qui doivent 
// etre plus loin dans la liste pour l'emporter

// NB: ca ne serait pas mal de concatener pour accelerer le rendu

static void _addProp(UElem* e, UAttr* prop) {
  UChildIter i = e->abegin();
  for ( ; i != e->aend(); ++i) {
    // !! vraiment un hack !!
    //if (dynamic_cast<UProps*>(*i) && !dynamic_cast<UXmlAttr*>(*i)) k++;
    if (dynamic_cast<UAttrList*>(*i) && (*i)->getNodeName().compare("style")!=0)
      ;
    else break;
  }
  
  // cette fonctionalite n'est pas standard !!!
  //e->addImpl(e->attributes(), prop, i, true, null);
  e->addImpl1(prop, i, e->attributes());   // reafficher ????
}

// ==================================================== ======== 

void UXmlDocument::setClassStyle(UElem* e, const UStr& name, const UStr& value) {
  setClassIdStyle(e, name, value);
}

void UXmlDocument::setIdStyle(UElem* e, const UStr& name, const UStr& value) {
  setClassIdStyle(e, name, value);
}

void UXmlDocument::setClassIdStyle(UElem* e, const UStr& att_name, const UStr& att_value) 
{
  if (att_name.empty() /*|| !att_value*/) return;
  char fullname[1000];  // ATT MAX LEN !!
  const UClass* id = null;
  
  // NB: generalisable a tous les attributs
  
  if (att_value.empty())
    sprintf(fullname, "[%s]", att_name.c_str());
  else
    sprintf(fullname, "[%s=%s]", att_name.c_str(), att_value.c_str());
  
  if ((id = getStyleSheet().findClass(fullname)))
    _addProp(e, id->getAttributes());
  //cerr <<"setClassIdStyle1 "<<fullname << " " << id
  //  <<" : "<< *att_name <<"="<< *att_value <<endl;
  
  if (att_value.empty())
    sprintf(fullname, "%s[%s]", e->getNodeName().c_str(), att_name.c_str());
  else
    sprintf(fullname, "%s[%s=%s]", e->getNodeName().c_str(), 
            att_name.c_str(), att_value.c_str());
  
  if ((id = getStyleSheet().findClass(fullname)))
    _addProp(e, id->getAttributes());
  
  //cerr <<"setClassIdStyle2 "<<fullname << " " << id
  //  <<" : "<< *att_name <<"="<< *att_value <<endl;
}

/* ==================================================== ======== ======= */

const UDocAttachments* UXmlDocument::getAttachments() const {
  return &attachments;
}

void UXmlDocument::addAttachment(UDocAttachment* a) {
  if (a) attachments.push_back(a);
}

bool UXmlDocument::loadAttachment(UDocAttachment* a, bool reload) {
  if (reload || !a->isLoaded()) return (a->load(this) > 0);
  else return true;
}

int UXmlDocument::loadAttachments(bool reload) {
  int count = 0;
  for (UDocAttachments::iterator k = attachments.begin(); 
       k != attachments.end();
       k++) {
    if (*k && (reload || !(*k)->isLoaded())) {
      if ((*k)->load(this) >= 0) count++;
    }
  }
  return count;
}

/* ==================================================== [(C)Elc] ======= */

static void _print(ostream* out, UStr* buf, UNode* node) {
  if (!node) return;
  UElem* e = null;
  UStr* text = null;

  if ((e = dynamic_cast<UElem*>(node))) {
    // print begin tag (and node name)
    if (out) *out << "<" << e->getNodeName();
    if (buf) *buf &= "<" & e->getNodeName();

    // print attributes (if any)
    for (UChildIter ia = e->abegin(); ia != e->aend(); ++ia) {
      UAttr* a = dynamic_cast<UAttr*>(*ia);
      if (a /* && a->getValue()*/) {    // !! hack a revoir !!
        UStr val;
        a->getValue(val);
        if (out) *out << " " << a->getName() << "=\"" << val << "\"";
        if (buf) *buf &= " " & a->getName() & "=\"" & val & "\"";
      }
    }
    if (out) *out << ">" << endl;
    if (buf) *buf &= ">\n";

    // print child nodes
    for (UChildIter ic = e->cbegin(); ic != e->cend(); ++ic)
      _print(out, buf, *ic);

    // print end tag
    if (out) *out << "</" << e->getNodeName() << ">" << endl;
    if (buf) *buf &= "</" & e->getNodeName() & ">\n";
  }

  // c'est faux pour les subclasses de UStr !!!!!!!!
  else if ((text = dynamic_cast<UStr*>(node)) /*&& text->getData()*/) {
    if (out) *out << text->getData();
    if (buf) *buf &= text->getData();
  }
}

/* ==================================================== ======== ======= */

void UXmlDocument::print(std::ostream& fout) {
  UElem* e = getDocumentElement();
  if (e) {
    _print(&fout, null, e);
    fout << endl << endl;
  }
}

void UXmlDocument::print(UStr& buf) {
  UElem* e = getDocumentElement();
  if (e) {
    _print(null, &buf, e);
    buf &= "\n";
  }
}

}

