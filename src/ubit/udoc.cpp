/************************************************************************
 *
 *  udoc.cpp: Document Element
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
#include <ubit/ufile.hpp>
#include <ubit/udoc.hpp>
#include <ubit/udom.hpp>
using namespace std;
namespace ubit {


UDocFactory& UDocFactory::getDefaultFactory() {
  static UDocFactory* fact = new UDocFactory();
  return *fact;
}

UDocFactory& UDoc::getDefaultFactory() {
  return UDocFactory::getDefaultFactory();
}

UDoc::UDoc() : ppath(new UStr("")) {}
UDoc::UDoc(const UStr& _path) : ppath(new UStr(_path)) {}
UDoc::~UDoc() {}

/* ==================================================== ===== ======= */

bool UDoc::isEditable() const {return false;}
void UDoc::setEditable(bool state) {}

void UDoc::setPath(const UStr& _path) {
  if (!ppath) ppath = ustr();
  *ppath = _path;
}

void UDoc::makePath(UStr& path, const UStr& name) {
  path = name;
  path.trim();
  int pos;

  // http url:  not a local pathname
  if (path.find("http:") == 0) return;

  pos = path.find("https:");
  if (pos == 0) return;

  pos = path.find("ftp:");
  if (pos == 0) return;
  
  pos = path.find("ssh:");
  if (pos == 0) return;

  pos = path.find("file:");
  if (pos == 0) path.remove(0, 5); // remove "file:"

  if (path.at(0) != '/')  {
    UStr dirname = getPath().dirname();
    path.insert(0, dirname);
  }
}

/* ==================================================== ======== ======= */

void UDoc::addLinkCallback(const UChild& cond_slash_callback) {
  if (!call_list) call_list = new UElem();
  call_list->add(cond_slash_callback);
}

void UDoc::linkEventCB(UInputEvent& e, const UStr* path) {
  if (call_list) {
    //const UOn* on = dynamic_cast<const UOn*>(&e.getCond());  // ???
    //if (on) {
    //e.setTarget((UNode*)path);   ??? sert a qq chose ??? @@@!!!
    //e.setCond(*on);
    call_list->fire(e);
    //}
  }
}

  /* ==================================================== ===== ======= */

UFlatDoc::UFlatDoc(const UStr& _pathname) 
: UDoc(_pathname), edit(null) {
}

bool UFlatDoc::isEditable() const {
  return edit ? edit->isEditable() : false;
}

void UFlatDoc::setEditable(bool state) {
  if (edit) edit->setEditable(state);
}

/* ==================================================== ======== ======= */

class TextDocCreator : public UDocCreator {
public:
  virtual UDoc* create(UDocSource&);
};


UDoc* TextDocCreator::create(UDocSource& so) {
  UStr* str = null;

  if (so.from == UDocSource::PATHNAME) {
    str = new UStr();
    if ((so.stat = str->read(*so.fullpath)) <= 0) {
      delete str;
      return null;
    }
  }
  else if (so.from == UDocSource::BUFFER) {
    str = new UStr(so.buffer ? *so.buffer : "");
    so.stat = 1;
  }
  else if (so.from == UDocSource::SCRATCH) {
    str = new UStr();
    so.stat = 1;
  }
  else {
    so.stat = 0;
    return null;
  }
  
  UBox& fbox = uflowbox(upadding(4,6) + *str);
  UEdit& ed = uedit();
  fbox.addAttr(ed);      // editable

  UFlatDoc* doc = new UFlatDoc(*so.path);
  doc->add(fbox);
  doc->edit = &ed;
  return doc;
}

/* ==================================================== [Elc] ======= */

class ImaDocCreator : public UDocCreator {
public:
  virtual UDoc* create(UDocSource&);
};


UDoc* ImaDocCreator::create(UDocSource& so) {
  UIma* ima = null;

  if (so.from == UDocSource::PATHNAME) {
    ima = new UIma();
    if ((so.stat = ima->read(*so.fullpath)) <= 0) {
      delete ima; 
      return null;
    }
  }
  else if (so.from == UDocSource::BUFFER) {
    so.stat = 0;
    return null;  // not implemented!
  }
  else if (so.from == UDocSource::BUFFER) {
    ima = new UIma();
    so.stat = 1;
  }
  else {
    so.stat = 0;
    return null;
  }
  
  UDoc* doc = new UFlatDoc(*so.path);
  doc->addAttr(uleft());  // do not center images!
  doc->add(ima);
  return doc;
}

/* ==================================================== [Elc] ======= */

UPluginDoc::UPluginDoc(const UStr& _pathname) 
: UDoc(_pathname) {
}

class UPluginDocCreator : public UDocCreator {
public:
  UPluginDocCreator(const UStr& doctype, const UStr& command);

  virtual UDoc* create(UDocSource&);
  virtual void exec(UInputEvent&, UDoc*);
  virtual void open(UInputEvent&, UDoc*);
  virtual void openWith(UInputEvent&, UDoc*);
  virtual void openAsText(UInputEvent&, UDoc*);
protected:
  UStr doctype, command, alt_command, status;
};

UPluginDocCreator::UPluginDocCreator(const UStr& _doctype, 
                                     const UStr& _command) :
  doctype(_doctype),
  command(_command) {
}


UDoc* UPluginDocCreator::create(UDocSource& so) {
  so.stat = 0;
  if (so.from == UDocSource::BUFFER) {
    return null;
  }

  UStr* fname = new UStr(so.path->basename());
  UDoc* doc = new UPluginDoc(*so.path);

  doc->addAttr(utop() + upadding(13,13) + uvspacing(8) + UFont::large);
  doc->add(uhbox(uelem(UFont::bold + "File: ") + fname));

  UFileMode fm(*so.path);
  if (fm.isExec()) {
    doc->add(uhbox(uleft() + USymbol::right + " "
                   + ulinkbutton(UFont::bold + "Execute"
                                 + ucall(this, doc, &UPluginDocCreator::exec))));
  }

  if (!command.empty()) {
    UStr& help_s = ustr();
    if (command !="open") help_s = " (using " & command & " )";
    
    doc->add(uhbox(uleft() + USymbol::right + " "
                   + ulinkbutton(UFont::bold + "Open"
                                 + ucall(this, doc, &UPluginDocCreator::open))
                   + help_s));
  }

  UCall& call = ucall(this, doc, &UPluginDocCreator::openWith);
  doc->add(uhbox(uleft() + USymbol::right + " "
                 + ulinkbutton(UFont::bold + "Open With" + call)
                 + "  "
                 + utextfield(usize(200) + alt_command + call)));

  doc->add(uhbox(uleft() + USymbol::right + " "
                 + ulinkbutton(UFont::bold + "Open As Text"
                               + ucall(this, doc, &UPluginDocCreator::openAsText))));
  status = "";
  doc->add(" " + uhbox(UColor::red + UFont::bold + status));
  return doc;
}

/* ==================================================== ======== ======= */

void UPluginDocCreator::open(UInputEvent& e, UDoc* doc) {
  status = "";
  if (command.empty()) return;
  if (e.getDisp() != UAppli::getDisp()) {
    status = "Operation not allowed on clones";
    return;
  }
  UStr s = command & " \"" & doc->getPath() & "\"&";
  // OK sur clones si le filename n'est pas trop bizarre
  //if (s.find('`') >= 0 || s.find('|') >= 0 || s.find('*') >= 0 || s.find('?') >= 0) {
  //  status = "Invalid file name";
  //  return;
  //}
  int stat = system(s.c_str());              // !! system = DANGEREUX
  if (stat != 0) status = "Status: ", status &= stat; 
}

/* ==================================================== ======== ======= */

void UPluginDocCreator::openWith(UInputEvent& e, UDoc* doc) {
  status = "";
  if (alt_command.empty()) return;
  if (e.getDisp() != UAppli::getDisp()) {
    status = "Operation not allowed on clones";
    return;
  }
  UStr s = alt_command & " \"" & doc->getPath() & "\"&";
  int stat = system(s.c_str());
  if (stat != 0) status = "Status: ", status &= stat;
}


void UPluginDocCreator::exec(UInputEvent& e, UDoc* doc) {
  status = "";
  if (e.getDisp() != UAppli::getDisp()) {
    status = "Operation not allowed on clones";
    return;
  }

  UStr dir  = doc->getPath().dirname();
  UStr name = doc->getPath().basename();

  // faire cd avant l'exec pour mettre dans le bon contexte si lecture
  // d'arguments
  UStr s = "(cd \"" & dir & "\"; " & " \"" & name & "\")&";  
  int stat = system(s.c_str());
  if (stat != 0) status = "Status: ", status &= stat;
}


void UPluginDocCreator::openAsText(UInputEvent& e, UDoc* doc) {
  status = "";
  doc->removeAll();

  TextDocCreator creator;
  UDocSource so(UDocSource::PATHNAME, &doc->getPath(), null);
  
  UDoc* doc2 = creator.create(so);
  if (doc2) {
    doc->removeAll();
    doc->add(doc2);
  }
}

/* ==================================================== [Elc] ======= */

UDocFactory::UDocFactory() {
  stat = 0;
  errors = null;

#if (defined(__MACH__) && defined(__APPLE__))        //__POWERPC__
  default_creator = new UPluginDocCreator("","open");
#else
  default_creator = new UPluginDocCreator("","");  //empty = unknown command
#endif
  
  UDocCreator& image_creator = *new ImaDocCreator();
  addCreator("xpm", image_creator);
  addCreator("jpg", image_creator);
  addCreator("jpeg",image_creator);
  addCreator("gif", image_creator);

  UDocCreator& html_creator = *new UHtmlCreator();
  addCreator("html", html_creator);
  addCreator("htm",  html_creator);

  UDocCreator& xml_creator = *new UXmlCreator();
  addCreator("xhtml",xml_creator);
  addCreator("xml",  xml_creator);
  
  UDocCreator& text_creator = *new TextDocCreator();
  addCreator("txt", text_creator);
  addCreator("java",text_creator);
  addCreator("c",   text_creator);
  addCreator("h",   text_creator);
  addCreator("cpp", text_creator);
  addCreator("hpp", text_creator);
  addCreator("cc",  text_creator);
  addCreator("hh",  text_creator);
  addCreator("am", text_creator);
  addCreator("in", text_creator);

  UDocCreator& pdf_creator =
#if (defined(__MACH__) && defined(__APPLE__))        //__POWERPC__
    *new UPluginDocCreator("PDF Document", "open");
#else
    *new UPluginDocCreator("PDF Document", "acroread");
#endif
  addCreator("pdf", pdf_creator);

  UDocCreator& office_creator =
#if (defined(__MACH__) && defined(__APPLE__))        //__POWERPC__
    *new UPluginDocCreator("Office Document", "open");
#else
    *new UPluginDocCreator("Office Document", "soffice");
#endif
  addCreator("doc", office_creator);
  addCreator("ppt", office_creator);
  addCreator("xls", office_creator);
  addCreator("rtf", office_creator);
}

UDocFactory::~UDocFactory() {
  // detruire les Creators
}

int UDocFactory::getStatus() const {
  return stat;
}

UStr* UDocFactory::getErrors() const {
  return errors;
}

void UDocFactory::saveErrors(bool mode) {
  if (mode) errors = new UStr();
  else errors = null;
}

/* ==================================================== ======== ======= */

UDocSource::UDocSource(int _from, const UStr* _path, UStr* _errors,
                       const UStr* _buffer)
: from(_from), stat(0), path(_path), buffer(_buffer), errors(_errors)
{
  // path is not duplicated
  // fullpath may be shared with path
  fullpath = UFileCache::getOrCreateFullPath(*_path);
}

/* ==================================================== ======== ======= */

UDoc* UDocFactory::read(const UStr& pathname) {
  stat = 0;
  if (errors) errors->clear();

  UStr fext = pathname.suffix();
  UDocCreator* cr = getCreator(fext);
  if (!cr) cr = getDefaultCreator();
  if (!cr) return null;

  UDocSource so(UDocSource::PATHNAME, &pathname, errors);
  UDoc* doc = cr->create(so);
  stat = so.stat;
  return doc;
}

UDoc* UDocFactory::load(const UStr& name, const UStr& buffer) {
  stat = 0;
  if (errors) errors->clear();

  UStr fext = name.suffix();
  UDocCreator* cr = getCreator(fext);
  if (!cr) cr = getDefaultCreator();
  if (!cr) return null;

  UDocSource so(UDocSource::BUFFER, &name, errors, &buffer);
  UDoc* doc = cr->create(so);
  stat = so.stat;
  return doc;
}

UDoc* UDocFactory::create(const UStr& name) {
  stat = 0;
  if (errors) errors->clear();

  UStr fext = name.suffix();
  UDocCreator* cr = getCreator(fext);
  if (!cr) cr = getDefaultCreator();
  if (!cr) return null;

  UDocSource so(UDocSource::SCRATCH, &name, errors);
  UDoc* doc = cr->create(so);
  stat = so.stat;
  return doc;
}

/* ==================================================== ======== ======= */

bool UDocFactory::Comp::operator()(const UStr* s1, const UStr* s2) const {
  return s1->compare(*s2, true) < 0;   // ignore case
}

void UDocFactory::addCreator(const UStr& suffix, UDocCreator& creator) {
  // si existe deja ?
  rmap[&ustr(suffix)] = &creator;
}

void UDocFactory::setDefaultCreator(UDocCreator& creator) {
  default_creator = &creator;
}

UDocCreator* UDocFactory::getCreator(const UStr& fext) {
  if (fext.empty()) return null;

  Map::iterator k = rmap.find(&fext);
  if (k != rmap.end() && k->second) return k->second;

  return null;  // not found
}

UDocCreator* UDocFactory::getDefaultCreator() {
  return default_creator;
}

}

