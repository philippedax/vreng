/* ==================================================== ======== ======= *
 *
 *  uclassImpl.hpp
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

#ifndef _uclassImpl_hpp_
#define	_uclassImpl_hpp_ 1
#include <map>
#include <ubit/uattr.hpp>
#include <ubit/uelem.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uclass.hpp>
namespace ubit {
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UDefaultAttribute : public UAttr {
  public:
    struct MetaClass : public UClass {
      MetaClass(const UStr& classname) : UClass(classname) {}
      
      virtual bool isInstance(UObject& obj) const {
        return dynamic_cast<UDefaultAttribute*>(&obj);  // && test name ?
      }
      virtual UDefaultAttribute* newInstance() const {
        return new UDefaultAttribute(*this);
      }
    };
    
    UDefaultAttribute(const UDefaultAttribute::MetaClass& c) : cid(c) {}
    
    static const UClass& Class() {
      static MetaClass* c = new MetaClass("#attribute"); return *c;
    }
    virtual const UClass& getClass() const {return cid;} 
    
  private:
    const UClass& cid;  // !!ATT aux destructions, un uptr<> serait preferable !!
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UDefaultInlineElement : public UElem {
  public:
    struct MetaClass : public UClass {
      MetaClass(const UStr& classname) : UClass(classname) {}
      
      virtual bool isInstance(UObject& obj) const {
        return dynamic_cast<UDefaultInlineElement*>(&obj);  // && test name ?
      }
      virtual UDefaultInlineElement* newInstance() const {
        return new UDefaultInlineElement(*this);
      }
      virtual UStyle* newStyle() const {return UDefaultInlineElement::createStyle();}
    };
    
    UDefaultInlineElement(const UDefaultInlineElement::MetaClass& c) : cid(c) {}  
    
    static  const UClass& Class() {
      static MetaClass* c = new MetaClass("#element"); return *c;
    }
    virtual const UClass& getClass() const {return cid;} 
    
  private:
    const UClass& cid;  // !!ATT aux destructions, un uptr<> serait preferable !!
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UDefaultBlockElement : public UElem {
  public:
    struct MetaClass : public UClass {
      MetaClass(const UStr& classname) : UClass(classname) {}
      
      virtual bool isInstance(UObject& obj) const {
        return dynamic_cast<UDefaultBlockElement*>(&obj);  // && test name ?
      }
      virtual UDefaultBlockElement* newInstance() const {
        return new UDefaultBlockElement(*this);
      }
      virtual UStyle* newStyle() const {return UDefaultBlockElement::createStyle();}
    };
    
    UDefaultBlockElement(const UDefaultBlockElement::MetaClass& c) : cid(c) {}  
    
    static const UClass& Class() {
      static MetaClass* c = new MetaClass("#element"); return *c;
    }
    virtual const UClass& getClass() const {return cid;}
    
  private:
    const UClass& cid;  // !!ATT aux destructions, un uptr<> serait preferable !!
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UAttrClassMap {
  public:
    ~UAttrClassMap();
    
    void addClass(const UClass&);
    ///< adds this class (if a class already exists with the same name it is removed).
    
    const UClass* findClass(const UStr& name) const;
    ///< returns class (null if not found).
    
    const UClass* obtainClass(const UStr& name);
    ///< finds class; creates a generic UAttrClass if not found.
    
    struct Comp {
      /// Attribute name comparator, case is ignored.
      bool operator()(const UStr*a,const UStr*b) const {return a->compare(*b,true) < 0;}
    };
    
    typedef std::map<const UStr*, const UClass*, Comp> Map;
    Map map;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UElemClassMap {
  public:
    ~UElemClassMap();
    
    void addClass(const UClass&);
    ///< adds this class (if a class already exists with the same name it is removed).
    
    const UClass* findClass(const UStr& name) const;
    ///< returns class (null if not found).
    
    const UClass* obtainClass(const UStr& name);
    ///< finds class; creates a generic UElemntClass if not found.
    
    struct Comp {
      /// Element name comparator, case is ignored.
      bool operator()(const UStr*a,const UStr*b) const {return a->compare(*b,true)<0;}
    };
    typedef std::map<const UStr*, const UClass*, Comp> Map;
    Map map;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UStyleSheet : public UElemClassMap {
  public:
    ~UStyleSheet();
  };
  
}
#endif

