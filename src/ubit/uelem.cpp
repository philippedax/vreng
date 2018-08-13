/************************************************************************
 *
 *  uelem.cpp: Element Nodes (lightweight containers that can have children)
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
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uwin.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

//const UStr* UElem::getNodeName() const {return &getClass().getName();}

const UStr* UElem::getTextSeparator() const {
  return getStyle(null).textSeparator;
}

const UStyle& UElem::getStyle(UUpdateContext* ctx) const {  
  const UClass& c = getClass();
  
  const UStyle& style =
  c.obtainStyle() ? c.obtainStyle()->getStyle(ctx) : UAppli::getDefaultStyle();
  
  // determiner l'orient si PAS definie explicitement par attr ou child
  //if (!hasBMode(UOn::HAS_ORIENT)) {
  if (!emodes.HAS_ORIENT) {
    if (style.orient != UOrient::INHERIT) {
      emodes.IS_VERTICAL = (style.orient==UOrient::VERTICAL);
    }
    else {
      if (ctx && ctx->parent_ctx && ctx->parent_ctx->obj)
        emodes.IS_VERTICAL = ctx->parent_ctx->obj->emodes.IS_VERTICAL;
    }
  }
  return style;
}

UStyle* UElem::createStyle() {
  UStyle* s = new UStyle();
  //valign, halign;  // rendu heritable
  s->halign = UHalign::INHERIT;
  s->valign = UValign::INHERIT;
  // vspacing, hspacing; // rendu heritable
  s->hspacing = UHspacing::INHERIT;
  s->vspacing = UVspacing::INHERIT;
  return s;
}

/* ==================================================== [Elc] ======= */

UElem::UElem(UArgs a) {
  memset(&emodes, 0, sizeof(emodes));
  emodes.IS_SHOWABLE = true;
  ostate = UOn::IDLE;
  
  // call add-initialization procedure of child (VOIR aussi: add())
  // NB: duplication des liens  
  for (UChildIter c = a.children->begin(); c!=a.children->end(); ++c) {
    // pour recuperer la cond, passer c.getNode()
    UChildIter self = children().insert(cend(), c.child());
    
    // initialization procedure of child:
    UChild& child = self.child();
    UNode* child_node = *self;
    
    child.parent = this;
    child_node->_parents.push_back(self);      //  NB: inutile pour les UCONST!!
    child_node->addingTo(child, *this);  
    if (child.cond) child.cond->setParentModes(*this);  // deplace ici
  }
}

/* ==================================================== ===== ======= */

UElem::~UElem() {
  destructs();
  //moved in destructs
  //UAppli::deleteNotify(this);  // tells the appli the object is being deleted
}

// ATTENTION:
// les appels aux fcts vituelles ne marchent pas normalement dans les
// destructeurs (l'appel est fait avec la classe du destructeur,
// pas avec la classe effective de this)
// => appeler destructs() DANS LE DESTRUCTEUR de la CLASSE de l'objet
//   pour que destructs() soit appele avec le type adequat
// => CECI IMPOSE DE DEFINIR DES DESTRUCTEURS DANS LES SOUS-CLASSES

void UElem::destructs() {
  if (UAppli::isExiting()) return;
  
  UAppli::deleteNotify(this);  // tells the appli the object is being deleted

  if (omodes.IS_DESTRUCTED) return;
  
  // if this obj is in the updatelist, remove it
  if (omodes.IS_UPDATING) UAppli::impl.removeUpdateRequests(toBox());
  omodes.IS_UPDATING = false;
  
  // this object is now being destructed
  omodes.IS_DESTRUCTING = true;
  
  // doit etre avant sinon ne fera rien car CBs enleves
  if (hasCallback(UOn::DESTRUCT_CB)) {
    UEvent e(UOn::destruct, this); //UElemEvent
    fire(e);
  }
  
  // removes and destroys all children (when applicable)
  bool auto_up = isAutoUpdate();
  setAutoUpdate(false);
  removeAll();
  removeAllAttrs();
  setAutoUpdate(auto_up);
  
  // detaches from parents (this is done after removing children
  // for improving view management)
  if (pbegin() != pend()) removeFromParents(false);
  
  // must be at the end (callbacks wont be called otherwise)
  // superclasses' destructs() methods wont be called
  omodes.IS_DESTRUCTED = true;
}

/* ==================================================== [Elc] ======= */

UElem& UElem::addImpl(const UArgs& a, UChildIter pos, UChildren& list) {
  bool update = false;
  
  for (UChildIter c = a.children->begin(); c != a.children->end(); ++c) {
    if (!omodes.DONT_AUTO_UPDATE && (*c)->isDisplayable()) update = true;
    addImpl1(c.child(), pos, list);
  }
  
  if (update) {
    UBox* box = this->toBox();
    if (box && box->views)
      box->update(UUpdate::LAYOUT | UUpdate::PAINT | UUpdate::ADD_REMOVE);
    // et pour les groups .....
  }
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UElem::addImpl1(const UChild& c, UChildIter pos, UChildren& list) { 
  if (!*c) {
    error("UElem::add","attempt to add null argument"); 
    return false;
  }
  
  //!!!!!!! verifier pas deja dans la liste !!!
  
  UChildIter self = list.insert(pos, c);  // exception if incorrect pos
  UChild& child = self.child();
  UNode* child_node = *self;
  
  // initialization procedure of child:
  child.parent = this;
  child_node->_parents.push_back(self);  //  NB: inutile pour les UCONST!!
  child_node->addingTo(child, *this);  
  if (child.cond) child.cond->setParentModes(*this);  // deplace apres
  
  UElem* cel = child_node->toElem();
  if (cel) initChildViews(cel /*, &child*/);
  
  if (hasCallback(UOn::CONTAINER_CB)) {    // a la fin
    UEvent e(UOn::addChild, this, child_node);  //UElemEvent
    //e.setTarget(child_node);
    fire(e);
  }
  
  return true;
}

/* ==================================================== [Elc] ======= */

UElem& UElem::removeImpl(UChildIter pos, int N, bool autodel, UChildren& list) {
  bool update = false;
  
  if (N == -1) {
    while (true) {
      if (list.begin() == list.end()) break;
      if (!omodes.DONT_AUTO_UPDATE && (**list.begin())->isDisplayable()) update = true;
      removeImpl1(list.begin(), autodel, list);
    }
  }    
  else {
    UChildIter c = pos;
    for (int k = 0; k < N; ++k) {
      if (c == list.end()) break;
      if (!omodes.DONT_AUTO_UPDATE && (*c)->isDisplayable()) update = true;
      UChildIter c2 = c; ++c2;
      removeImpl1(c, autodel, list);
      c = c2;
    }
  }
  
  if (update) {
    UBox* box = this->toBox();
    if (box && box->views) box->update(UUpdate::LAYOUT | UUpdate::PAINT | UUpdate::ADD_REMOVE);
    // et pour les groups .....
  }
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UElem::removeImpl1(UChildIter pos, int auto_delete, UChildren& list) {
  if (pos == list.end() || list.begin() == list.end()) {
    error("UElem::remove","iterator is out of range");
    return false;
  }
  
  UNode* chnode = *pos;
  if (!chnode) {
    error("UElem::remove","attempt to remove null argument");
    return false;
  }
  
  UChild& chlink = pos.child();  
  chnode->removingFrom(chlink, *this);
  chnode->_parents.removeFirst(&chlink);
  
  // delete views if the node is an Element or can contain an Element
  //if (chelem) chelem->deleteRelatedViews(null, &chlink);
  if (chnode->toElem() || chnode->getSubGroup()) {
    vector<UView*> objviews;
    retrieveRelatedViews(objviews);
      
    if (chnode->toElem()) 
      chnode->toElem()->deleteViewsInside(objviews);
    // att au else: ne pas detruire ici le subgroup d'un element
    else if (chnode->getSubGroup())
      chnode->getSubGroup()->deleteViewsInside(objviews);
  }
  
  //UChildIter next = list.erase(pos);
  UNode* res = null;
  
  // si plus que des pseudo-parents, il faut les detacher 
  // (sauf si appel depuis removeFromParents sinon bouclage)
  if (auto_delete != REMOVE_FROM_PARENTS) {
    if (pbegin() != pend() && ! chnode->hasSceneGraphParent()) 
      chnode->removeFromParents(false);
  }
  
  // child n'est pas detruit dans les cas suivants:
  // 1) auto_del est faux
  // 2) il a au moins un "full" parent pas detruit
  // 3) il est reference
  // 4) il n'a pas ete cree par new (ou a ete rendu indeletable)
  
  if (auto_delete != true     // ATT: auto_delete can be = -1 !!!
      || chnode->hasSceneGraphParent()
      || chnode->getPtrCount() > 0
      || !chnode->omodes.IS_DYNAMIC) {
    res = chnode;    // child pas detruit => le retourner
  }
  else {
    delete chnode;  // NB: delete est redefini
    res = null;
  }
  
  if (hasCallback(UOn::CONTAINER_CB)) {  //deplace apres
    UEvent e(UOn::removeChild, this, chnode);   //UElemEvent
    //e.setTarget(child_node);   // !ATT child may have been deleted
    fire(e);
  }
  
  return true;
}

/* ==================================================== [Elc] ======= */

bool UElem::isParentOf(const UNode& b) const {
  return b.isChildOf(*this, true);
}

UNode* UElem::getChild(int pos) const {
  UChildIter i = children().at(pos);
  return (i == cend()) ? null : *i;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UElem& UElem::add(const UArgs& nodes) {
  return addImpl(nodes, cend(), children());
}

UElem& UElem::add(const UArgs& nodes, int pos) {
  return addImpl(nodes, children().at(pos), children());
}

UElem& UElem::add(const UArgs& nodes, UChildIter pos) {
  return addImpl(nodes, pos, children());
}

UElem& UElem::remove(UNode& c, bool autodel) {
  UChildIter it = children().find(c);
  if (it == cend()) return *this;   // no error if not found
  return removeImpl(it, 1, autodel, children());
}

UElem& UElem::remove(int pos, bool autodel) {
  UChildIter it = children().at(pos);   // error if invalid index
  return removeImpl(it, 1, autodel, children());
}

UElem& UElem::remove(UChildIter it, bool autodel) {
  // error if invalid index
  return removeImpl(it, 1, autodel, children());
}

UElem& UElem::removeAll(bool autodel) {
  return removeImpl(cbegin(), -1, autodel, children());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ce serait intereassnt de recupere l'iterateur (pour setAttr)

//EX: UAttr* UElem::getAttributeNode(const UStr& name, bool ignore_case) 
UAttr* UElem::getAttr(const UStr& name) const {
  for (UChildIter i = abegin(); i != aend(); ++i) {    // DEVRAIT ETRE UNE MAP!!!!
    UAttr* a = dynamic_cast<UAttr*>(*i);
    const UStr* n = a ? &a->getName() : null;
    if (n && n->equals(name /*,ignore_case*/)) return a;
  }
  return null;
}

bool UElem::getAttrValue(UStr& value, const UStr& name) const {
  UAttr* a = getAttr(name);
  if (!a) return false; 
  else {
    a->getValue(value);  // !devrait renvoyer default selon API officielle
    return true;
  }
}

//EX: UAttr* UElem::setAttrNode(UAttr* attr)
//if (!attr || !attr->getName()) return null;

// il faudrait egalement distinguer les UCond !!!

UElem& UElem::setAttr(UNode& attr) {   // DEVRAIT UTILISER UNE MAP!!!!
  const UStr& aname = attr.getNodeName();
  if (aname.empty()) return *this;
 
  UAttr* a = getAttr(aname);  // check if attr already in the list
  if (a == &attr) return *this;  // same attr : do nothing
  if (a) {
    bool auto_up = isAutoUpdate();
    setAutoUpdate(false);
    removeAttr(*a);  // remove the old attribute
    setAutoUpdate(auto_up);
  }
  addAttr(attr);  // add the new attribute
  return *this;
 }

UElem& UElem::addAttr(const UArgs& nodes) {  // A REMPLACER PAR setAttr !!!
  return addImpl(nodes, aend(), attributes());
}

UElem& UElem::removeAttr(UNode& c, bool autodel) {
  UChildIter it = attributes().find(c);
  if (it == aend()) return *this;   // no error if not found
  return removeImpl(it, 1, autodel, attributes());
}

UElem& UElem::removeAllAttrs(bool autodel) {
  return removeImpl(abegin(), -1, autodel, attributes());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 UIter UElem::getAttributeNodes() {return UIter(attributes());}

 // faux: devrait renvoyer true si dans API
 bool UElem::hasAttribute(const UStr& name) const {
   const UAttr* a = getAttributeNode(name);
   return (a != null);
 }
 
 //template <class CC>
 //UAttr* getAttr(CC*& attribute);
 //virtual UNode* appendChild(UNode*);
 //virtual UNode* removeChild(UNode* x);
 //UChildIter UElem::findChild(int pos) const {
 //  return children().find(pos);
 //}
 //UChildIter UElem::findChild(UNode& x) const {
 //  return children().find(x);
 //}
 //UChildren& getChildNodes() {return children();}
 // XML compat: returns children().  
 // bool hasChildNodes() const {return cbegin() != cend();}
 // XML compat: returns true if children() is not empty.
 //UChildren& getAttributeNodes() {return attributes();}
 //virtual bool hasAttributes() const;
 
 void UElem::closeWin(int status) {
   for (UParentIter p = pbegin(); p != pend(); ++p) {
    UElem* par = *p;
    if (par) {    // par == null if no other parent (UFrame or similar)
      UWin* win = par->toWin();
      if (!win) par->closeWin(status);
      //pas question de fermer le mainframe de cette maniere
      else if (win != UAppli::getMainFrame())
        win->close(status);
    }
  }
 }
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::closeWin(UInputEvent& e, int status) {
  for (UView* v = e.getView(); v != null; v = v->getParentView()) {
    UBox* box = v->getBox();
    if (box) {
      UWin *win = box->toWin();
      if (win) {
        //pas question de fermer le mainframe de cette maniere
        if (win != UAppli::getMainFrame()) win->close(status);
        break;
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::update(const UUpdate& upd, UDisp* d) {
  for (UParentIter p = pbegin(); p != pend(); ++p)
    if (*p) (*p)->update(upd, d);
}

void UElem::doUpdate(const UUpdate& upd, UDisp* d) {
  for (UParentIter p = pbegin(); p != pend(); ++p)
    if (*p) (*p)->doUpdate(upd, d);
}

// Note: isShown() ne veut pas dire grand chose (sauf pour les Window)
// a cause des scrollpanes et autres clipping: une box peut-etre "SHOWN"
// sans apparaitre reellement a l'ecran (car hors zone de clipping du viewport)

void UElem::show(bool state) {
  // EX: faux if (isShown() == state) return;
  // FAUX: isShown est egalement relatif aux parents!
  if (isShowable() == state) return;
  
  if (state) {             // il faut maj immediate pour show()
    emodes.IS_SHOWABLE = true;
    doUpdate(UUpdate::SHOW, null);
    UEvent e(UOn::show, this);  // devrait etre fait par doUpdate sur view au bon moment
    fire(e);
  }
  else {
    emodes.IS_SHOWABLE = false;
    doUpdate(UUpdate::HIDE, null);
    UEvent e(UOn::hide, this);  //UElemEvent
    fire(e);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UElem::isShown() const {
  if (!isShowable()) return false;
  
  for (UParentIter p = pbegin(); p != pend(); ++p) {
    UElem* parent = *p;
    if (!parent || parent == this) {  // cas toplevel (UFrame or similar)
      if (isShowable()) return true;
    }
    else {
      // continuer recursivement (on retourne direct si OK
      // on continue en sequence grace au for() sinon
      if (parent->isShown()) return true;
    }
  }
  
  // aucune racine valide parmi tous les chemins des parents
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// att: parview n'est pas la view du parent mais du 1er box!
void UElem::initView(UView* parview) {
  //if (!hasBMode(UOn::INITIALIZED)) init1();
  //init(parview);
  
  // Propager aux children
  for (UChildIter it = cbegin(); it != cend(); ++it) {
    // ? QUESTION: init pour tous ou que UElem/ubox ?
    UElem* cel = (*it)->toElem();
    //if (cel) cel->initView(&ch.child(), parview);
    if (cel) cel->initView(parview);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//void UElem::initChildViews(UElem* child, UChild *childlink) {
void UElem::initChildViews(UElem* child) {
  // il faut remonter dans l'arborescence pour retrouver les Views
  // (c'est fait par appel recursif de cette fct)
  for (UParentIter p = pbegin(); p != pend(); p++) {
    UElem* grp = *p;
    //if (grp) grp->initChildViews(child, childlink);
    if (grp) grp->initChildViews(child);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UElem::retrieveRelatedViews(vector<UView*>& tab) const {
  int count = 0;
  // tab is not emptied: the views are added to the end of what it already contains
  for (UParentIter p = pbegin(); p != pend(); p++) 
    count += (*p)->retrieveRelatedViews(tab);
  
  // returns the number of views that were added, which may thus differ from tab size
  return count;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// supprimer les descendants de cette vue dans les enfants et (descendants)
// ATTENTION: 
// - il faut passer au travers des UElem pour detruire les vues
//   de ses descendants de type UBox
// NOTE:
// - ne pas oublier que les UElem peuvent avoir des enfants de type UBox
//   et qu'il faut donc aussi detruire ces vues
// - les UBox peuvent avoir des Active UBorders (typiquement les Scrollpane)
//   et il faut egalement detruire les vues des elements contenus dedans
//   (cas du getSubGroup()). De plus ces UBorders peuvent etre dans l'ATTR_LIST

void UElem::deleteViewsInside(const vector<UView*>& parent_views) {
  for (UChildIter a = abegin(); a != aend(); ++a) {
    UNode* child = *a;
    if (child->getSubGroup())
      child->getSubGroup()->deleteViewsInside(parent_views);
  }
  
  for (UChildIter c = cbegin(); c != cend(); ++c) {
    UNode* child = *c;
    if (child->toElem())
      child->toElem()->deleteViewsInside(parent_views);
    // att au else: ne pas detruire ici le subgroup d'un element
    else if (child->getSubGroup())
      child->getSubGroup()->deleteViewsInside(parent_views);
  }
  
  if (getSubGroup())
    getSubGroup()->deleteViewsInside(parent_views);
}

/* ==================================================== [Elc] ======= */

int UElem::_getTextLength(bool recursive) const {
  int len = 0;
  UStr *str = null;
  UElem *grp = null;
  
  const UStr* sep = getTextSeparator();
  if (sep && sep->length() <= 0) sep = null;
  
  for (UChildIter ch = cbegin(); ch != cend(); ch++) {
    UNode *b = *ch;
    if ((str = b->toStr())) {
      len += str->length();
    }
    // ne pas entrer dans les UWin
    else if (recursive && (grp = b->toElem()) && !grp->toWin()) {
      if (sep /*&& grp_found*/)	len += sep->length();
      //grp_found = true;
      len += grp->_getTextLength(recursive);
    }
  }
  return len;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char* UElem::_getTextData(char* ptrpos, bool recursive) const {
  UStr *str = null;
  UElem *grp = null;
  
  const UStr* sep = getTextSeparator();
  if (sep && sep->length() <= 0) sep = null;
  
  for (UChildIter ch = cbegin(); ch != cend(); ch++) {
    UNode* b = *ch;
    if ((str = b->toStr()) && str->c_str()) {
      strcpy(ptrpos, str->c_str());
      ptrpos += str->length();
    }
    // ne pas entrer dans les UWin
    else if (recursive && (grp = b->toElem()) && !grp->toWin()) {
      if (sep /*&& grp_found*/) {
        strcpy(ptrpos, sep->c_str());
        ptrpos += sep->length();
      }
      //grp_found = true;
      ptrpos = grp->_getTextData(ptrpos, recursive);
    }
  }
  
  return ptrpos;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::retrieveText(UStr& res, bool recursive) const {
  int len = _getTextLength(recursive);
  if (len <= 0) {res.clear(); return;}
  
  char *data = (char*)malloc((len+1)*sizeof(char));
  if (!data) {
    error("UElem::retrieveText","No more memory");
    return;
  }
  
  char *ptrpos = _getTextData(data, recursive);
  if (ptrpos != data + len) {
    error("UElem::retrieveText","[internal error] wrong length");
    res.clear(); return;
  }
  *ptrpos = '\0';  // null terminated!
  
  res.setImpl(data, len);
}

UStr UElem::retrieveText(bool recursive) const {
  UStr res;
  retrieveText(res, recursive);
  return res;
}

/* ==================================================== [Elc] ======= */

bool UElem::isEnabled() const  {return ostate != UOn::DISABLED;}
bool UElem::isArmed()   const  {return ostate == UOn::ARMED;}
bool UElem::isDragged() const  {return ostate == UOn::DRAGGED;}

//bool UElem::isEntered() const  {return ostate == UOn::ENTERED;}
//bool UElem::isActionPerformed() const  {return ostate == UOn::ACTIONED;}

void UElem::setInterState(UObject::State s) {
  ostate = s;
}

UElem& UElem::setArmable(bool state) {
  emodes.IS_ARMABLE = state;
  return *this;
}

UElem& UElem::setAutoRepeat(bool state) {
  emodes.IS_AUTO_REPEAT = state;
  return *this;
}

UElem& UElem::setSelectable(bool state) {
  emodes.IS_SELECTABLE = state;
  return *this;
}

UElem& UElem::_setArmableAndSelectable(bool state) {
  emodes.IS_SELECTABLE = state;
  emodes.IS_ARMABLE = state;
  return *this;
}

UElem& UElem::setDraggable(bool state) {
  emodes.IS_DRAGGABLE = state;
  return *this;
}

UElem& UElem::setDroppable(bool state) {
  emodes.IS_DROPPABLE = state;
  return *this;
}

UElem& UElem::setBrowsable(bool state) {
  emodes.IS_BROWSABLE = state;
  return *this;
}

UElem& UElem::setCrossable(bool state) {
  emodes.IS_CROSSABLE = state;
  return *this;
}

UElem& UElem::ignoreEvents(bool state)  {
  emodes.IGNORE_EVENTS = state;
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UElem& UElem::catchEvents(const UChild& condition_callback_expr)  {
  const UCond* cond = condition_callback_expr.getCond();
  if (!cond) return *this;
  
  if (*cond == UOn::mpress || *cond == UOn::mrelease || *cond == UOn::mdrag)
    emodes.CATCH_EVENTS |= Modes::CATCH_MOUSE_MASK;
  else if (*cond == UOn::mmove)
    emodes.CATCH_EVENTS |= Modes::CATCH_MOUSE_MOVE_MASK;
  else if (*cond == UOn::wheel)
    emodes.CATCH_EVENTS |= Modes::CATCH_WHEEL_MASK;
  else if (*cond == UOn::kpress || *cond == UOn::krelease || *cond == UOn::ktype)
    emodes.CATCH_EVENTS |= Modes::CATCH_KEY_MASK;
  
  addAttr(condition_callback_expr);
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct _UObserverCallbacks : public UAttr {
  void addAttr(const UChild& callback) {_addAttr(callback);}
};

/* To invalidate a 'condition_callback_expr' you must remove this callback object
 * (eg. ucall(...)) from the object's child list. To do so, keep a pointer to this
 * callback object then remove it by calling one of the removeAttr() method.
 */

UElem& UElem::observeChildrenEvents(const UChild& callback) {  
  emodes.OBSERVE_EVENTS = true;
  
  // NB : une autre solution serait d'appeler les callback 'normaux' de l'observer
  // mais avec un flag ad hoc
  
  _UObserverCallbacks* oc = null;
  attributes().findClass(oc);
  if (!oc) addAttr(*(oc = new _UObserverCallbacks));
  oc->addAttr(callback);
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace impl {
  
  struct CALLCELL {
    UCall* call;
    const UCond* cond;
  };
  
  // by default, allocates in the stack to improve perf
  // then allocates in the heap if the size is too small
  struct CALLTAB {
    static const int STACK_SIZE = 100;
    CALLCELL table[STACK_SIZE+1];   // +1 necessaire!
    CALLCELL *begin, *end, *max;
    int size;
    
    CALLTAB() {
      begin = end = table;
      max = table + STACK_SIZE;
      size = STACK_SIZE;
    }
    
    ~CALLTAB() {
      // if the list is not in the stack, then delete it
      if (begin != table) delete begin;      
    }
    
    CALLCELL* next() {
      if (end < max) {
        return ++end;
      }
      else {
        CALLCELL* p = new CALLCELL[size*2];
        ::memcpy(p, begin, size * sizeof(CALLCELL));
        if (begin != table) delete begin;      
        begin = p;
        end = begin + size;
        max = begin + size*2;
        size *= 2;
        return end;
      }
    }
  };
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UElem::hasCallback(long c) const {
  return (c & callback_mask) != 0; 
}

bool UElem::fire(UEvent& e) const {
  const UOn* on = null;
  // dont call callback if the object has already been destructed
  if (omodes.IS_DESTRUCTED || !(on = e.getCond().toOn()))
    return false;
  
  bool fired = false;
  
  // each UOn must have an appropriate callback_mask field to be fired
  if ((callback_mask & on->callback_mask) != 0) {
    
    // we use an intermediate list to fire callbacks because some of them may remove
    // children (or even themselves) in the list: iterators would thus be undefined
    impl::CALLTAB ctab;
    impl::CALLCELL* pc = ctab.begin;
    
    // find matching callbacks in the attribute list 
    for (UChildIter i = abegin(); i != aend(); ++i) {
      UChild& ch = i.child();
      // ch.cond can be a multicond, actual_cond is the one which is actually fired
      if (ch.cond && (pc->cond = ch.cond->matches(*on)) && (pc->call = ch.obj->toCall()))
        pc = ctab.next();
    }
    
    // find matching callbacks in the child list
    for (UChildIter i = cbegin(); i != cend(); ++i) {
      UChild& ch = i.child();
      if (ch.cond && (pc->cond = ch.cond->matches(*on)) && (pc->call = ch.obj->toCall()))
        pc = ctab.next();
    }
    
    // fire callbacks (and check deletions!)
    for (pc = ctab.begin; pc != ctab.end; ++pc) {
      if (!pc->call->omodes.IS_DESTRUCTING && !pc->call->omodes.IS_DESTRUCTED) {
        //if (on != UOn::filterEvent) e.setCond(*pc->cond);
        (*pc->call)(e);
        fired = true;
        // stop if the object has been destructed while firing a callback
        if (omodes.IS_DESTRUCTED) {
          //return fired;
          break;
        }
      }
    }
  }  // endif((callback_mask & on->callback_mask) != 0)
  
  // fire the callbacks of the observer if there is one
  UMouseEvent* me = e.toMouseEvent();
  
  //cerr << "FIRE src " << this << " " << (this ? this->getClassName() : "none") 
  //<<" / ev " << me  << " / on " << on->ID;
  //if (me) cerr<< " / obs " << me->event_observer 
  //<< " " << (me->event_observer ? me->event_observer->getClassName() : "none") ;
  //cerr << endl;
  
  if (me && me->event_observer) {
    _UObserverCallbacks* oc = null;
    me->event_observer->attributes().findClass(oc);
    if (oc) {
      // serait interessant d'avoir 'fired' et 'this' dans e (et indicateur childCB)
      // et aussi d'eviter appel si (oc->callback_mask & on->callback_mask) == 0)
      e.setCond(*on);  // utile ?
      oc->fire(e);
    }
  }
  
  return fired;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UElem& UElem::setEnabled(bool state, bool call_cbs) {
  if (state == isEnabled()) return *this;  // avoid infinite loop!  
  if (state) setInterState(UOn::IDLE);
  else setInterState(UOn::DISABLED);
  
  update(UUpdate::paint);
  return *this;
}

UElem& UElem::setSelected(bool state, bool call_cbs) {
  if (state == isSelected()) return *this;  // avoid infinite loop!
  emodes.IS_SELECTED = state;
  
  if (call_cbs) {    
    if (state) {
      UEvent e(UOn::select, this);  //UElemEvent
      fire(e);
    }
    else { 
      UEvent e(UOn::deselect, this);  //UElemEvent
      fire(e);
    }
    
    // fire(e, UOn::change); supprime car ambigu
    
    // 07jun03: UOn::action deja appele par ailleurs
    // d'autre part, action est reserve aux cas ou il y a une action directe
    // sur l'objet. les actions indirectes (eg. deselection parce que l'on a 
    // selectionne un autre bouton) ne doivent pas etre prises en compte
    // pour eviter incoherences et diverses bizarreries
    // fire(e, UOn::action);
  }
  update(UUpdate::paint);
  return *this;
}

// =============================================================================

UBehavior::UBehavior(InputType it) {
  intype = it;
  SOURCE_IN_MENU = DONT_CLOSE_MENU = false;
  cursor = null;
  browsing_group = null;
  event_observer = null;
}

static UEventFlow& getEventFlow(UEvent& e) {
  UInputEvent* ie = e.toInputEvent();
  UEventFlow* f = ie ? ie->getFlow() : null;
  return f ? *f : *UAppli::getFlow(0);
};

// =============================================================================
// comportement standard: le redraw et le change mouse ne sont faits 
// que si l'on rentre dans l'objet avec la souris non enfoncee

void UElem::enterBehavior(UInputEvent& e, bool is_browsing) {
  UEventFlow& f = *e.getFlow();
    
  //if (bp.intype == UBehavior::MOUSE) {
  if (!is_browsing) {
    //f.setCursor(e, bp.cursor); deplace dans ueventflow.cpp
    
    // ne changer l'ACTION et ne reafficher QUE si necessaire (sinon on aurait un 
    // changement de couleur des reafficahges partiel (par exple dans le Text)
    if (ostate != UOn::DISABLED && emodes.IS_CROSSABLE) {
      setInterState(UOn::ENTERED);
      update(UUpdate::paint);
    }
  }
  
  // appeler qu'un button soit pressed ou non (meme si DND ???)
  e.setCond(UOn::enter);
  fire(e);
  
  // NB: doit lancer le timer meme si not enabled
  if (emodes.HAS_TIP) f.openTipRequest(e);
  
  // cas du Browsing ou la souris a ete enfoncee ailleurs
  //if (ostate != UOn::DISABLED && bp.intype == UBehavior::BROWSE) {
  if (ostate != UOn::DISABLED && is_browsing) {
    if (!f.dragSource) {		// just browsing
      armBehavior(e, is_browsing);
    }
    else if (emodes.IS_DROPPABLE) { 			// DND being performed
      f.dropTarget = this;
      setInterState(UOn::DROP_ENTERED);
      update(UUpdate::paint);
      e.setCond(UOn::dropEnter);
      fire(e);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::leaveBehavior(UInputEvent& e, bool is_browsing) {
  UEventFlow& f = *e.getFlow();
  
  // jamais execute SAUF CALLBACK si disabled
  if (ostate != UOn::DISABLED) {
    
    if (f.dragSource /*&& bstyle == BROWSING*/) {  // DND being performed
      if (emodes.IS_DROPPABLE && ostate == UOn::DROP_ENTERED) {
        f.dropTarget = null;
        setInterState(UOn::IDLE);
        update(UUpdate::paint);
        e.setCond(UOn::dropLeave);
        fire(e);
      }
    }
    
    else {
      //bug: disarm() est appele tout le temps !!!
      //if (ostate != UOn::DISABLED && bstyle == BROWSING) {
      
      //if (bp.intype == UBehavior::BROWSE) {
      if (is_browsing) disarmBehavior(e, true);
      
      // si ostate vaut IDLE c'est que l'on etait en mode Browsing et 
      // que disarmB() a fait le travail. sinon disarm() a remit en mode
      // ENTER et il faut donc reafficher a la sortie de soursi
      if (ostate != UOn::IDLE) {
        setInterState(UOn::IDLE);
        // ne reafficher QUE si necessaire
        if (!isDestructed() && emodes.IS_CROSSABLE) update(UUpdate::paint);
      }
    }
  }
  
  // appeler que btn soit pressed ou non (et meme DND)
  e.setCond(UOn::leave);
  fire(e);
  
  if (emodes.HAS_TIP) f.closeTipRequest(e);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::armBehavior(UInputEvent& e, bool is_browsing) {
  UEventFlow& f = getEventFlow(e);

  UMouseEvent* me = e.toMouseEvent();
  int btns = me ? me->getButtons() : 0;
  
  if (ostate == UOn::DISABLED) return;    // jamais execute si disabled
  if (emodes.HAS_TIP) f.closeTipRequest(e);
  
  if (emodes.IS_DRAGGABLE) {	// drag and drop
    // ICI IL Y A UN BUG: mouse_alt_btn sert a la fois au paste et au DND
    // cela veut dire qu'il ne faut pas faire paste s'il y a une selection
    // et si on attend avant de bouger la souris (et meme chose pour le cas
    // UOn::CAN_DRAG avec mouse_select_btn)
    
    //if (bp.intype == UBehavior::MOUSE
    if ((me && !is_browsing)
        && (btns & (UAppli::conf.mouse_select_button|UAppli::conf.mouse_alt_button))
        ) {
      // !ATT: ne commence JAMAIS en mode Browsing
      // (sinon, entrerait en mode DND des qu'on survolerait un objet 
      // CAN_DRAG meme si on n'est pas parti de celui-ci)
      f.dragSource = this;
      f.dropTarget = null;
      e.setCond(UOn::dragStart);
      fire(e);
      // changer curseur!     // NB: pourrait avoir cursor propre 
      f.setCursor(e, &UCursor::dnd);
    }
  }
  
  if (f.beingClicked != this 
      || (me && me->getWhen() >= f.click_time + UAppli::conf.multi_click_delay)) {
    f.click_count = 0;
  }
  
  // garder derniere pos de mouse pour les multiclics
  if (!me) f.beingClicked = null;
  else {
    f.beingClicked = this;
    f.click_pos = me->getScreenPos();
  }
    
  // ATT: pas de else: un objet peut etre a la fois canArm ET canDrag !
  if (emodes.IS_ARMABLE) {
    
    int btn_arm_mask = UAppli::conf.mouse_select_button;
    if (e.modes.SOURCE_IN_MENU) {
      btn_arm_mask |= UAppli::conf.mouse_menu_button;
    }
    
    //if (bp.intype == UBehavior::MOUSE) {
    if (me && !is_browsing) {
      if (btns & btn_arm_mask) {
        f.lastArmed = this;
        setInterState(UOn::ARMED);
        if (!isDestructed()) {
          update(UUpdate::paint);
          e.setCond(UOn::arm);
          fire(e);
        }
      }
    } 
    
    //else if (bp.intype == UBehavior::BROWSE) {
    else if (me && is_browsing) {
      // n'importe quel button dans un menu system, que btn1 sinon
      if ((e.modes.SOURCE_IN_MENU || (btns & UAppli::conf.mouse_select_button))
          && (              // coming back to the button that was initially pressed
              (f.lastPressed.box == this && emodes.IS_ARMABLE)
              //||    // OR: this object is in the current Browsing group
              //(f.browsing_group && f.browsing_group == bp.browsing_group)
              )
          ) { 
        f.lastArmed = this;
        setInterState(UOn::ARMED);          
        if (!isDestructed()) {
          update(UUpdate::paint);
          me->modes.IS_BROWSING = true;
          e.setCond(UOn::arm);
          fire(e);
        }
      } 
    }
    
    //else if (bp.intype == UBehavior::KEY) {
    else { // this action was provoked by a keyPress
      f.lastArmed = this;
      setInterState(UOn::ARMED);
      //if (e.source_view) {
      if (!isDestructed()) {
        update(UUpdate::paint);
        e.setCond(UOn::arm);
        fire(e);
      }
    }
    
    if (emodes.IS_AUTO_REPEAT) f.startAutoRepeat(e);
  }  //endif(armable)
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::disarmBehavior(UInputEvent& e, bool is_browsing) {
  UEventFlow& f = getEventFlow(e);

  // jamais execute si disabled
  if (ostate == UOn::DISABLED) return;
  
  short prev_ostate = ostate;
  
  if (f.dragSource) {   // DND being performed
    if (f.dropTarget) { // si on est toujours au dessus de dropTarget
      e.setCond(UOn::dropDone);
      f.dropTarget->fire(e);
    }
    f.dragSource->setInterState(UOn::IDLE);
    f.dragSource->update(UUpdate::paint);  //NB on pourrait sauver dragSourceView
    e.setCond(UOn::dragDone);
    f.dragSource->fire(e);
    update(UUpdate::paint);
    
    //f.setCursor(e, bp.cursor);  // set previous cursor
    f.setCursor(e, null);  // set previous cursor
    
    f.dragSource = null;
    f.dropTarget = null;
  }
  
  // si !armed ne rien faire si activations incorrectes
  else if (ostate == UOn::IDLE) {
    f.lastArmed = null;
  }
  
  else {
    // action() doit avoir lieu avant click() et dans tous les cas, meme si multiclick
    // faudrait aussi annuler le dernier actionBehavior()
    if (emodes.IS_AUTO_REPEAT) f.stopAutoRepeat(e);
        
    //if (armable && bp.intype != UBehavior::BROWSE) actionBehavior(e, bp);
    if (emodes.IS_ARMABLE && !is_browsing) actionBehavior(e);
    
    // deplace et relatif a beingClicked et non lastArmed (sinon pas effectue si objet par armable)
    bool multiclick = false;
    
    //if (bp.intype != UBehavior::BROWSE && f.beingClicked == this) {
    if (!is_browsing && f.beingClicked == this) {
      UMouseEvent *me = e.toMouseEvent();
      if (me
          && me->getScreenPos().x > f.click_pos.x - UAppli::conf.click_radius
          && me->getScreenPos().x < f.click_pos.x + UAppli::conf.click_radius
          && me->getScreenPos().y > f.click_pos.y - UAppli::conf.click_radius
          && me->getScreenPos().y < f.click_pos.y + UAppli::conf.click_radius) {
        
        if (f.click_count == 0) {    // single click
          me->click_count = f.click_count = 1;
          f.click_time = me->getWhen();
          e.setCond(UOn::click);
          fire(e);
        }
        
        // cas multi-click :
        // verifier delai et position entre clics successifs
        // NB: attention a ne pas mettre une valeur mouse_click_delay
        // trop grande sinon les click/action vont devenir des multiclick
        else if (f.click_count > 0 
                 && me->getWhen() < f.click_time + UAppli::conf.multi_click_delay) {
          me->click_count = ++f.click_count;
          f.click_time = me->getWhen();
          multiclick = true;
          e.setCond(UOn::click);
          fire(e); // always fired
          if (f.click_count == 2) {
            e.setCond(UOn::doubleClick);
            fire(e);
          }
        }
      }
    }
    
    //if (bp.intype == UBehavior::BROWSE)
    if (is_browsing)
      setInterState(UOn::IDLE);
    else if (prev_ostate == UOn::ENTERED) 
      setInterState(UOn::ENTERED);
    else 
      setInterState(UOn::IDLE); 
    
    //if (e.source_view) {
    if (!isDestructed()) {
      update(UUpdate::paint);
      if (emodes.IS_ARMABLE) {
        e.setCond(UOn::disarm);
        fire(e);
      }
    }
    
    f.lastArmed = null;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::actionBehavior(UInputEvent& e) {
  //f.inAction = this;
  
  // mis en premier sinon fire(action) est appele avec une valeur 
  // de isSelected() qui est obsolete
  
  if (emodes.IS_SELECTABLE) {
    // il faut selectionner au DISARM et non au ARM pour avoir un
    // comportement conforme a l'usage (sauf dans les listes et les trees)    
    emodes.IS_SELECTED = ! emodes.IS_SELECTED;
    
    if (isSelected()) {
      e.setCond(UOn::select);
      fire(e);
    }
    else {
      e.setCond(UOn::deselect);
      fire(e);
    }
    // fire(e, UOn::change); supprime car ambigu
  }
  
  setInterState(UOn::ACTIONED);
  
  //if (e.source_view) {
  if (!isDestructed()) {
    e.setCond(UOn::action);
    fire(e);
  }
    
  // IDLE doit etre la car actionB() n'est pas seulement appele de armB() 
  // mais aussi de typeB() et aussi pour le update des selectables
  
  setInterState(UOn::IDLE);
  
  if (emodes.IS_SELECTABLE) {
    //remet a jour toutes les "views" en mode IDLE
    //NB: le redraw simple ne suffit pas car objet modal
    update(UUpdate::paint);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UElem::keyPressBehavior(UKeyEvent& e) {
  if (ostate == UOn::DISABLED) return;    // jamais execute si disabled
    
  // moved to unatevent.cpp: int count = ke->decodeKey();
  e.setCond(UOn::kpress);
  fire(e);
  
  if (e.keychar) {
    e.setCond(UOn::ktype);
    fire(e);
    
    if (emodes.IS_TEXT_EDITABLE) {
      if (e.keycode == UKey::Enter) actionBehavior(e);
    }
    else {
      if (e.keychar == ' ') armBehavior(e, false);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NB: certaines implementation pourraient etre nettement plus compliquees,
// le UOn::type pouvant eventuellement etre appele apres le keyReleaseBehavior

void UElem::keyReleaseBehavior(UKeyEvent& e) {
  if (ostate == UOn::DISABLED) return;
    
  e.setCond(UOn::krelease);
  fire(e);
  disarmBehavior(e, false);
}

}



