/************************************************************************
 *
 *  ucond.hpp: Base class for the attributes of the Scene Graph
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
#ifndef _ucond_hpp_
#define	_ucond_hpp_ 1
#include <ubit/udefs.hpp>
namespace ubit {
  
  /** Base class for Ubit conditions.
   * main subclasses: UOn, UFlag, UInscale.
   * Example:
   * <pre>
   *    ubutton("Click Me" + UOn::action / ucall(arg, myfunc))
   * </pre>
   * !Warning: UCond (and derived) objects must not be deleted.
   */
  class UCond {
  public:
    virtual ~UCond() {}
    
    virtual bool operator==(const UCond& c) const {return this == &c;}
    virtual bool operator!=(const UCond& c) const {return this != &c;}
    virtual const UCond* matches(const UCond& c) const {return this == &c ? this : 0;}
    virtual bool verifies(const UUpdateContext&, const UElem&) const = 0;
    
    virtual UOn* toOn() {return null;}
    virtual const UOn* toOn() const {return null;}
    
    // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    virtual void setParentModes(UElem& parent) const {};
  };
  
  /* ==================================================== ===== ======= */
  // pose le pbm de la desctruction auto
  
  class UMultiCond : public UCond {
  public:
    UMultiCond& operator+=(const UCond& c) {return add(c);}
    virtual UMultiCond& add(const UCond&);
    virtual void remove(const UCond&);
    
    virtual const UCond* matches(const UCond& c) const;
    virtual bool verifies(const UUpdateContext&, const UElem&) const;
    virtual void setParentModes(UElem& parent) const;
    
  private:
    typedef std::list<const UCond*> CondList;
    CondList condlist;
  };
  
}
#endif

