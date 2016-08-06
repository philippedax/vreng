/* ***********************************************************************
 *
 *  uchild.hpp [internal implementation]
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
 * **********************************************************************/

#ifndef _uchild_hpp_
#define	_uchild_hpp_ 1
namespace ubit {

/** [impl] Internal implementation of a child node.
*/
class UChild {
public:
  UChild(UNode& o) : obj(&o), parent(0), cond(0) {}
  UChild(UNode* o) : obj(o),  parent(0), cond(0) {}
  UChild(UNode* o, const UCond& c) : obj(o), parent(0), cond(&c) {}
  UChild(const char* s);

  UNode* operator*() {return obj;}  
  const UNode* operator*() const {return obj;}  
  
  UElem* getParent() {return parent;}

  const UCond* getCond() const {return cond;}
  void setCond(const UCond& c)  {cond = &c;}
  void setCond(const UChild& c) {cond = c.cond;}
  
 private:
  friend class UParent;
  friend class UNode;
  friend class UElem;
  UNode* obj;
  UElem* parent;
  const UCond* cond;
};

/* ==================================================== ===== ======= */

template <class _Iter>
class _UChildIter : public _Iter {
public:
  //_UChildIter() : _Iter(null) {}       // !ATT: peut poser probleme si pas g++ !
  _UChildIter() : _Iter() {}
  _UChildIter(const _Iter& i) : _Iter(i) {}
  _UChildIter(const _UChildIter& i) : _Iter(i) {}
  
  UNode*  operator*()    {return static_cast<UNode*>(*_Iter::operator*());}
  UChild& child()        {return _Iter::operator*();}
  const UCond* getCond() {return _Iter::operator*().getCond();}
};

// ==================================================== [(c)Elc] ======= 

/** forward iterator in a child or attribute list.
* @see: UElem::cbegin(), UNode::abegin(), UChildren.
*/
typedef _UChildIter<std::list<UChild>::iterator> UChildIter;

/** reverse iterator in a child or attribute list.
* @see: UElem::crbegin(), UChildren.
*/
typedef _UChildIter<std::list<UChild>::reverse_iterator> UChildReverseIter;

/* ==================================================== ===== ======= */
/** Child (or attribute) list.
 * @see: UChildIter, UElem::children(), UElem::attributes(), UAttr::attributes().
 */
class UChildren : public std::list<UChild> {
public:
  UChildIter at(int pos);
  ///< returns an iterator pointing to the object at this position; returns the last child if 'pos' = -1 and end() if 'pos' is out of bounds.

  UChildIter find(const UNode& child);
  ///< returns an iterator pointing to 'child'; returns end() if 'child' does no belong to the child list.
  
  UChildIter findStr(const UStr& value, bool ignore_case = true);
  /**< searches a string (UStr) which is equal to 'value'; returns end() if there is no such child.
    * this function compares the content of strings (not their addresses).
    */
  
  UChildIter findBox(const UStr& value, bool ignore_case = true);
  /**< searches a box (UBox or subclass) which contains a string which is equal to 'value'; returns end() if there is no such child.
    * this function compares the content of strings (not their addresses).
    */
  
  /** returns an iterator to the first child that derives from this class.
    * "derives" means: this class or a direct or indirect subclass. Exemple:
    * <pre>
    *    UColor* c = null;
    *    UChildIter i = children.findClass(c);
    * </pre>
    */
  template <class CC>
    UChildIter findClass(CC*& c) {
      c = null;
      for (UChildIter i = begin(); i != end(); ++i) {
        if (dynamic_cast<CC*>(*i)) {c = (CC*)*i; return i;}
      }
      return end();
    }
};

}
#endif

