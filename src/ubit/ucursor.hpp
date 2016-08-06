/************************************************************************
 *
 *  ucursor.hpp: Cursor Attribute
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

#ifndef _ucursor_hpp_
#define	_ucursor_hpp_ 1
#include <ubit/uattr.hpp>
namespace ubit {
  
  class UCursorImpl;
  
  /** Mouse cursor property.
   * this property specifies the shape of the cursor in a UBox. It is inherited
   * by children, except if they have their own UCursor, or if their UStyle
   * specifies another UCursor.
   * As other UAttr(s), a UCursor instance must be added as an attribute 
   * (or as a child) of the UBox. @see: UAttr.
   */
  class UCursor : public UAttr {
  public:
    UCLASS(UCursor)
    
    static UCursor none, pointer, crosshair, text, hand, pencil, wait, question, 
    hresize, vresize, move, dnd;
    ///< predefined cursors.
    
    UCursor(const UCursor& = UCursor::none);
    ///< creates a new Cursor from a UCursor (or a default UCursor).
    
    UCursor(int cursor_type);
    ///< creates a new Cursor from a cursor_type (impl dependent).
    
    UCursor(int cursor_type, UConst);
    ///< creates a new CONSTANT Cursor from a cursor_type (impl dependent).
    
    virtual ~UCursor();
    
    UCursor& operator=(const UCursor& c) {return set(c);}
    ///< copies another UCursor.
    
    virtual UCursor& set(const UCursor&);
    ///< copies another UCursor.
    
    bool operator==(const UCursor& c) const {return equals(c);}
    bool operator!=(const UCursor& c) const {return !equals(c);}
    virtual bool equals(const UCursor&) const;
    
    int getCursorType() const {return cursor_type;}
    
    // - - - impl  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void update() {} 
    
    UCursorImpl* getCursorImpl(UDisp*) const;
    ///< [impl] returns internal data for this display.
    
#ifndef NO_DOC
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    virtual void putProp(UUpdateContext*, UElem&);
    ///< NOTE that removingFrom() requires a destructor to be defined.
    
  private:
    int cursor_type;
    mutable std::vector<UCursorImpl*> cimpl;
#endif
  };
  
}
#endif
