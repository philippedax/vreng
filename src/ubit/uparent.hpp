/* ==================================================== ======== ======= *
 *
 *  uparent.hpp [internal implementation]
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

#ifndef _uparent_hpp_
#define	_uparent_hpp_ 1
namespace ubit {

/** [impl] Internal implementation of a parent node.
*/
class UParent {
public:
  UParent(UChildIter i) : child_iter(i) {}
  
  UElem* operator*() {return child_iter->parent;}
  const UElem* operator*() const {return child_iter->parent;}
  
  UChild& getChild() {return child_iter.child();}
  
private:
  UChildIter child_iter;
};

/* ==================================================== ======== ======= */

template <class _I>
struct _UParentIter : public _I {
  _UParentIter() : _I(null) {}       // !ATT: peut poser probleme si pas g++ !
  _UParentIter(const _I& i) : _I(i) {}
  _UParentIter(const _UParentIter& i) : _I(i) {}
  
  UElem* operator*() {return static_cast<UElem*>(*_I::operator*());}
  UParent&  parent() {return _I::operator*();}
};

/** forward iterator in a parent list.
* @see: UNode::pbegin(), UParents.
*/
typedef _UParentIter<std::list<UParent>::iterator> UParentIter;

/* ==================================================== ===== ======= */
/** Parent list.
* see also: UParentIter and UElem::parents().
*/
class UParents : public std::list<UParent> {
public:
  void removeFirst(UChild*);
  /// removes the first element that is pointing to this child.

  void updateAutoParents(const UUpdate&);
  ///< update parents that are in autoUpdate mode.

  void fireParents(const UCond& c, UNode* n);
  ///< fire parents callbacks that match this event.
  
  void setParentsViewsModes(int vmodes, bool on_off);
  ///< changes the modes of parents views.
};

}
#endif
