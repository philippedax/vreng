/************************************************************************
 *
 *  uflag.hpp
 *  (see also UAttrs.hpp for UAttr subclasses)
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

#ifndef _uflag_hpp_
#define	_uflag_hpp_ 1
#include <ubit/uattr.hpp>
#include <ubit/ucond.hpp>
namespace ubit {
  
  /** Flagging conditions.
   * for specifying conditional objets that are only active when this condition
   * is verified. Works in conbination with UFlagdef.
   * Example:
   * <pre>
   * const UFlag showStores, showMuseums;
   * 
   *   UBox& stores = ...;  // whatever
   *   UBox& museums = ...; // whatever
   *   UBox& scene = ubox( showStores/stores + showMuseums/museums );
   *   
   *   // the stores appear in this box (the museums are ignored)
   *   UBox& scene_with_stores  = ubox( uflagdef(showStores) + scene);
   *
   *   // the museums appear in this box (the stores are ignored)
   *   UBox& scene_with_museums = ubox( uflagdef(showMuseums) + scene);
   * </pre>
   *
   * !Warning: UFlag objects must not be deleted!
   * See also: UFlagdef, UPropdef.
   */
  class UFlag : public UCond {
  public:
    static const UFlag none;
    
    UFlag() {}
    virtual bool verifies(const UUpdateContext&, const UElem&) const;
  };
  

  // ==================================================== Ubit Toolkit =========
  /** [impl] negation of a UFlag condition.
   * see also: UCond& operator!(const UCond&)
   */
  class UNotFlag : public UCond {
  public:
    const UFlag& cond;
    UNotFlag(const UFlag& c) : cond(c) {}
    virtual bool verifies(const UUpdateContext&, const UElem&) const;
  };
  
  /** negation of a UFlag condition.
   */
  inline UNotFlag& operator!(const UFlag& c) {return *new UNotFlag(c);}
  
  
  // ==================================================== Ubit Toolkit =========
  /* [impl] Defines a flag that is inherited in the scene graph.
   *  See: UFlag.
   */
  class UFlagdef : public UAttr {
  public:
    UCLASS(UFlagdef)
    
    UFlagdef();
    UFlagdef(const UFlag&);
    ///< create a new flagdef (see UFlag.).
    
    UFlagdef& operator=(const UFlag& f) {return set(f);} 
    virtual UFlagdef& set(const UFlag&);
    virtual UFlagdef& clear();
    
    const UFlag* getFlag() const {return flag;}
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
    
  protected:
    const UFlag* flag;
  };
  
  inline UFlagdef& uflagdef() {return *new UFlagdef();}
  inline UFlagdef& uflagdef(const UFlag& f) {return *new UFlagdef(f);}
  
  
  // ==================================================== Ubit Toolkit =========
  /* [impl] Defines a property that is inherited in the scene graph.
   * use UAttrval to retreive the property in a (direct or indirect) child.
   */
  class UPropdef : public UFlagdef {
  public:
    UCLASS(UPropdef)
    UPropdef();
    UPropdef(const UFlag&);
    UPropdef(const UFlag&, UAttr&);
    
    virtual UPropdef& set(const UFlag&);
    virtual UPropdef& set(UAttr&);
    virtual UPropdef& set(const UFlag&, UAttr&);
    virtual UPropdef& clear();
    
    UAttr* getProp() const {return prop;}
    virtual void putProp(UUpdateContext*, UElem&);
    
  private:
    uptr<UAttr> prop;
  };
  
  /* ==================================================== ===== ======= */
  /* [impl] gets the value of an inherited property.
   * use UPropdef to define a property in a (direct or indirect) parent.
   */
  class UPropval : public UAttr {
  public:
    UCLASS(UPropval)
    UPropval();
    UPropval(const UFlag&);
    
    const UFlag* getFlag() const {return flag;}
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
    
  private:
    const UFlag* flag;
  };
  
}
#endif


