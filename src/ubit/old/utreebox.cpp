/************************************************************************
 *
 *  utreebox.cpp
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
#include <ubit/ubit.hpp>
using namespace std;
namespace ubit {


UTreebox& utreebox(const UArgs& a) {return *new UTreebox(a);}

UTreebox::~UTreebox() {}

// ATT: TYPES LIES !
static struct IsSelectableByTree : public UChoice::IsSelectable {
  virtual bool operator()(const UBox* box) const {
    return dynamic_cast<const UItem*>(box);   // !!!!!!!!
  }
} is_selectable_by_tree;

UTreebox::UTreebox(const UArgs& a) : UListbox(a) {
  choice().setSelectionRule(is_selectable_by_tree);
  addAttr(UBorder::none);
}

UTreenode* UTreebox::getSelectedNode() const {
  UElem* title = choice().getSelectedItem();
  UElem* node = title ? title->getParent(0) : null; // c'est title qui est selected !
  return node ? dynamic_cast<UTreenode*>(node) : null;
}

void UTreebox::setSelectedNode(UTreenode& n) {
  choice().setSelectedItem(*n.plabel);  // c'est title qui est selected !
}

int UTreebox::getSelectedIndex() const {
  return choice().getSelectedIndex();
}

UTreebox& UTreebox::setSelectedIndex(int i) {
  choice().setSelectedIndex(i);
  return *this;
}

/* ==================================================== ===== ======= */

UTreenode& utreenode(const UArgs& title) {
  return *new UTreenode(title);
}

UTreenode& utreenode(const UArgs& title, const UArgs& children) {
  return *new UTreenode(title, children);
}

UTreenode::UTreenode(const UArgs& title) {
  constructs(title, UArgs::none);
}

UTreenode::UTreenode(const UArgs& title, const UArgs& children) {
  constructs(title, children);
}

void UTreenode::constructs(const UArgs& _title, const UArgs& children) {
  static UPadding& offset = upadding(0, UIGNORE).setLeft(40);
  psubnodes = &uvbox(children);
  psubnodes->addAttr(offset);
  psubnodes->addAttr(UVspacing::none);
  addAttr(UVspacing::none);
  
  pexpander = new UBox
    (UOn::select   / USymbol::down
     + UOn::deselect / USymbol::right
     + UOn::select   / ushow(*psubnodes, true)
     + UOn::deselect / ushow(*psubnodes, false)
     );
  pexpander->setSelected(false);
  psubnodes->show(false);
  
  plabel = new UItem(*pexpander +  " " + _title);
  UBox::add(*plabel + *psubnodes);
}

void UTreenode::expand(bool s) {
  pexpander->setSelected(s);
}

/*
void UTreenode::setOffset(UHmargin& margin) {
  UHmargin* m = null;
  UChildIter i = psubnodes->attributes().findClass(m);
  if (m) remove(i);
  psubnodes->add(margin);
}
*/

void UTreenode::addNode(UTreenode& n) {
  psubnodes->add(n);
}
 
void UTreenode::removeNode(UTreenode& n) {
  psubnodes->remove(n);
}
 
}


