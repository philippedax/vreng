/************************************************************************
 *
 *  unode.hpp: base class for objects that can be added to the scene graph
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

#ifndef _unode_hpp_
#define	_unode_hpp_ 1
#include <ubit/udefs.hpp>
#include <ubit/uobject.hpp>
#include <ubit/uchild.hpp>
#include <ubit/uparent.hpp>
namespace ubit {

/** Base class of objects that can be added to the UBIT scene graph (SEE DETAILS!).
 *
 * A Ubit scene graph can be seen as a XML document:
 * - UNode corresponds to "Node", the base class of XML nodes in the DOM model
 * - subclasses UAttr and UElem respectively correspond to "Atrribute" and "Element"
 * - UBox, which derives from UElem, is the base class for widgets, and UWin for windows
 *
 * UNode derives from UObject, the base class for (almost) all Ubit objects. 
 * See the UNode documentation for important info about:
 * - subclassing and the UCLASS macro
 * - recursive deletion
 * - UPtr smart pointers
 */
class UNode : public UObject {
  UNode(const UNode&); 
  ///< assigment is forbidden (but is redefined by certain subclasses).
  
  UNode& operator=(const UNode&); 
  ///< assigment is forbidden (but is redefined by certain subclasses).
    
public:
  /// redefines Class() and getClass() methods (@see UNode).
  //UCLASS("unode", UNode, null)
  UCLASS(UNode)

  /// XML object types.
  enum NodeType {
    ELEMENT_NODE                   = 1,
    ATTRIBUTE_NODE                 = 2,
    TEXT_NODE                      = 3,
    CDATA_SECTION_NODE             = 4,
    ENTITY_REFERENCE_NODE          = 5,
    ENTITY_NODE                    = 6,
    PROCESSING_INSTRUCTION_NODE    = 7,
    COMMENT_NODE                   = 8,
    DOCUMENT_NODE                  = 9,
    DOCUMENT_TYPE_NODE             = 10,
    DOCUMENT_FRAGMENT_NODE         = 11,
    NOTATION_NODE                  = 12
  };
  
  /// Ubit display types.
  enum DisplayType {
    WINLIST=-1, UNDISPLAYBLE=0, ATTRIBUTE, DATA, INLINE, BLOCK, BORDER, HARDWIN, SOFTWIN
  };

protected:
  explicit UNode();
  ///< constructor, can anly be called by subclasses because UNode is abstract.
  
  virtual ~UNode() {destructs();}
  ///< destructor, note that children are recursively destroyed except if pointed elsewhere (@see class UNode).
  
public:
  virtual int getNodeType() const {return 0;}
  ///< returns the XML node type.
  
  virtual const UStr& getNodeName() const;
  ///< return the XML node name.

  virtual UStr getNodeValue() const;
  ///< return the XML node value.

  virtual int getDisplayType() const {return UNDISPLAYBLE;}
  virtual int isDisplayable() const {return getDisplayType() > 0;}

  virtual UNode* toNode() {return this;}
  virtual const UNode* toNode() const {return this;}
 
   // - - - parents - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual bool isChildOf(const UElem& parent, bool indirect = true) const;
  /**< returns true if this object a child of 'parent'.
    *  'indirect' specifies if indirect childhood is taken into account. 
    */
  
  UParentIter pbegin() const {return parents().begin();}
  /**< returns an iterator to the beginning of the list of direct parents.
    * UParentIter is a parent iterator. It is compatible with standard STL algorithms. 
    * This example prints the class name of the direct parents of an object :
    * <pre>
    *    UNode* obj = ...;
    *    for (UParentIter p = obj->pbegin(); p != obj->pend(); ++p)
    *        cout << (*p)->getClassName() << endl;
    * </pre>
    * @see also: parents(), abegin(), aend().
    */
  
  UParentIter pend() const {return parents().end();}
  ///< returns an iterator to the end of the parent list (@see pbegin()).

  virtual UParents& parents() const {return _parents;}
  ///< returns the list of direct parents (@see pbegin()).
  
  virtual UElem* getParent(int pos = 0) const;
  /**< returns the Nth direct parent.
    * returns the first parent if 'pos' = 0 and the last parent if 'pos' = -1.
    * returns null and issues an error message if 'pos' is out of range.
    * Notes:
    * - objects can have several parents
    * - getParents() is more efficient for accessing multiple parents.
    */
    
  virtual bool hasSceneGraphParent() const;   // redifined from UObject
  /**< this object has at least one parent in the scene graph.
    * objects can have non scene graph parents in some cases. Such parents are not
    * taken into account for deciding whether objects can be automatically destructed. 
    */
  
  template <class CC>
    CC* findParent() const {
      for (UParentIter p = pbegin(); p != pend(); ++p) {
        if (dynamic_cast<CC*>(*p)) {return (CC*)*p;}
        else return ((UNode*)(*p))->findParent<CC>();
      }
      return null;
    }
  ///< returns first the direct or indirect parent with that class.

  virtual int getBoxParents(std::vector<UBox*>& parvect) const;
  ///< retrieves direct and indirect parents that are boxes.

  virtual UBox* getParent(const UView*) const;
  /**< returns the direct or indirect parent that contains this view.
    * intermediate UElem parents are ignored (ie. traversed) by this function.
    */
  
  virtual UView* getParentView(const UView*) const;
  /**< returns the direct or indirect parent's view that contains this view.
    * intermediate UElem parents are ignored (ie. traversed) by this function.
    */
  
  virtual UBox*  getParent(const UInputEvent&) const;
  
  
  virtual UView* getParentView(const UInputEvent&) const;
  /* returns the direct or *indirect* UBox parent (resp. parent's view) that contains this event.
    * intermediate UElem parents are ignored (ie. traversed) by this function.
    */
     
  virtual void removeFromParents(bool update_parents = true);
  /**< [impl] removes this object from all direct parents.
    * notes:
    * - objects can have several parents!
    * - this function is potentially dangerous: UElem::remove() methods
    *   should be prefered when possible.
    */
   
  // - - - misc.  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  friend UChild operator/(const UCond&, UNode&);  
  /**< conditional expression for callback functions and conditionnal objects.
    * example:
    * <pre>
    *    struct Demo {
    *       void saveFile();
    *       void showMsg(const char*);
    *       ....
    *    };
    *    Demo* d = new Demo;
    *    // NB: ubutton(...) returns *new UButton(...)
    *    UBox& b = ubutton(" Save..." + UOn::action / ucall(d, &Demo::saveFile));
    *    ....
    *    b.addAttr(UOn::enter / ucall(d, "Saves the file", &Demo::showMsg)
    * </pre>
    * @see: addAttr(), UCall, UOn, UFlag for more details.
    */
  
  friend UChild operator/(const UCond&, UNode*);
  ///< see operator/(const UCond&, UNode&).
  
  virtual bool fire(UEvent&) const;
  ///< fires callback functions that match this event.

  virtual UNode& removeAllAttrs(bool autodel = true);
  ///< removes all attributes (if any, redefined by subclasses).
      
  virtual void update() {}
  ///< updates object graphics.

  bool isAutoUpdate() const {return !omodes.DONT_AUTO_UPDATE;}
  ///< return current update policy: see setAutoUpdate().
  
  UNode& setAutoUpdate(bool state = true);
  /**< changes the update policy of this object when its is modified.
    * - if 'state' is true, the object if automatically updated when modified
    *   (either by adding/removing children for UElem and subclasses,
    *   or by setting new values for UData, UStr, UAttr ...)
    * - if 'state' is false, the update() function must be called
    *   explicitely to update the graphics. This can be useful in certain cases
    *   to avoid intermediate updates in order to to speed up rendition
    */

protected:
  virtual void addingTo(UChild&, UElem& parent) {}
  ///< called when this object is added to a parent.
  
  virtual void removingFrom(UChild&, UElem& parent) {}
  /**< called when this object is removed from a parent (SEE DETAILS IF YOU REDEFINE THIS METHOD).
   * !BEWARE: subclasses that redefine removingFrom() MUST HAVE A DESTRUCTOR that
   * calls destructs().
   * <br>DETAILS: this is because of a C++ subtletly: as polymorphism is deactivated
   * in destructors, a redefined removingFrom() method won't be called except if 
   * destructs() - that calls removingFrom() - is called in the destructor of the
   * class that redefines removingFrom()). 
   */
  
  virtual void destructs();
  ///< unlinks the object from its parents and destroys its children.
    
  // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC 
public:
  void fireParents(const UCond& c, UNode* n) const {_parents.fireParents(c,n);}
  
  void updateAutoParents(const UUpdate& m) {_parents.updateAutoParents(m);}    
  virtual void addChangeCall(UCall&);
  virtual UElem* getSubGroup() const {return null;}
  
  void _addAttr(const UChild& attribute);
  ///< [impl] to be removed @@@.

  UChildIter _abegin() const {return _attributes.begin();}
  ///< [impl] to be removed @@@.

  UChildIter _aend() const {return _attributes.end();}
  ///< [impl] to be removed @@@.
  
protected:
  friend class UChild;
  friend class UArgs;
  friend class UParents;
  friend class UElem;
  mutable UParents _parents;
  mutable UChildren _attributes;  // attributes (including callbacks)
#endif
};

}
#endif 
