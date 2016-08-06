/* ==================================================== ======== ======= *
 *
 *  uxmlgrammar.hpp
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

#ifndef _uxmlgrammar_hpp_
#define _uxmlgrammar_hpp_ 1
#include <map>
#include <vector>
#include <ubit/udom.hpp>
namespace ubit {
  
  
  /** XML Grammar.
   */
  class UXmlGrammar {
  public:
    UXmlGrammar();
    virtual ~UXmlGrammar();
    
    virtual void addAttrClass(const UClass&);
    virtual void addElementClass(const UClass&);
    virtual void addCharEntityRef(const UStr& entity_name, unsigned short entity_value);
    
    virtual const UClass* getAttrClass(const UStr& classname) const;
    virtual const UClass* getElementClass(const UStr& classname) const;
    virtual unsigned short getCharEntityRef(const UStr& entity_name) const;
    
    static UXmlGrammar& getSharedUndefGrammar();  ///< TO BE CHANGED !!!
    static UClass* addUndefAttrClass(const UStr& classname); ///< TO BE CHANGED !!!
    static UClass* addUndefElementClass(const UStr& classname); ///< TO BE CHANGED !!!
    virtual void createDefaultCharEntityRefs(); ///< TO BE CHANGED !!!
    
  protected:
    struct Comp {
      bool operator()(const UStr*a,const UStr*b) const {return a->compare(*b)<0;}
    };
    typedef std::map<const UStr*, unsigned short, Comp> CharEntityRefMap;
    
    CharEntityRefMap char_entity_refs;
    UElemClassMap element_classes;
    UAttrClassMap attr_classes;
  };
  
  /* ==================================================== ===== ======= */
  /** List of XML Grammars.
   */
  class UXmlGrammars {
  public:  
    void addGrammar(const UXmlGrammar&);
    void addGrammars(const UXmlGrammars&);
    
    const UClass* getAttrClass(const UStr& classname) const;
    const UClass* getElementClass(const UStr& classname) const;
    unsigned short getCharEntityRef(const UStr& entityname) const;
    
  protected:
    typedef std::vector<const UXmlGrammar*> GrammarList;
    GrammarList glist;
  };
  
  
}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

