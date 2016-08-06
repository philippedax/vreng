/************************************************************************
 *
 *  uclass.hpp: Ubit MetaClasses
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

#ifndef _uclass_hpp_
#define	_uclass_hpp_ 1
#include <ubit/udefs.hpp>
namespace ubit {
  
  
#define UCLASSDEF(NAME,C,CREATOR) \
struct MetaClass : public UClass { \
MetaClass(): UClass(NAME) {} \
virtual bool isInstance(UObject& obj) const {return dynamic_cast<C*>(&obj);} \
virtual bool isInstance(UObject* obj) const {return dynamic_cast<C*>(obj);} \
virtual C* newInstance() const {return CREATOR;} \
virtual UStyle* newStyle() const {return C::createStyle();} \
}; \
static  const UClass& Class() {static MetaClass& c = *new MetaClass; return c;} \
virtual const UClass& getClass() const {return Class();}

  
  /** instanciable UObject (and thus, UNode) subclasses MUST invoke this macro in their public section
   * @see UClass and UABSTRACT_CLASS().
   */
#define UCLASS(C) UCLASSDEF(#C, C, new C)
  
  /** abstract UObject (and thus, UNode) subclasses MUST invoke this macro in their public section
   * @see UClass and UCLASS().
   */  
#define UABSTRACT_CLASS(C) UCLASSDEF(#C, C, null)
    
  
  /** Ubit class.
   *
   * All classes that derives from UObject can have an associated UClass.
   * UClass is a metaclass that defines some useful data for the corresponding
   * Ubit class, such as the class name, a unique ID, a static constructor, 
   * a style def, etc.
   *
   * UObject (and UNode) subclasses MUST invoke the UCLASS macro in the public
   * section of their declaration as follows:
   *
   * <pre>
   *   class UColor: public UAttr {
   *   public:
   *     UCLASS("ucolor", UColor, new UColor)
   *     .....
   *  };
   * </pre>
   *
   * The arguments of the UCLASS macro are:
   * - the ASCII name of the class (usually in lowercase letters)
   * - the C++ class name
   * - an instruction that would creates an instance of that class (such as: new UColor)
   *   or NULL if this class is abstract
   *
   * The UCLASS macro redefines the getClass() and Class() methods which are,
   * respectively, an instance ad a class method. Both methods return a metaclass
   * that describes the C++ class.
   */
  class UClass {  
  public:
    /** Parse Modes returned by UClass::getParseModes():
     * - closing tags can be omitted for EMPTY_ELEMENTs (eg. <img>)
     *   if the 'permissive' mode of the XMLParser is set.
     * - the textual content of DONT_PARSE_CONTENT elements is not parsed
     *   and their comments are stored as a text element (eg. <style> <script>)
     * - the XML Parser collapses white spaces (and tabs and new lines)
     *   if the 'collapse_spaces' mode is on except for PRESERVE_SPACES 
     *   elements (eg. the <pre> element) 
     */
    enum ParseModes {
      EMPTY_ELEMENT      = 1<<0, 
      DONT_PARSE_CONTENT = 1<<1,
      PRESERVE_SPACES    = 1<<2
    };
        
    UClass(const char* name);
    UClass(const UStr& name);
    virtual ~UClass();
    
    const UStr& getName() const {return *name;}
    ///< returns the class name.
    
    unsigned int getNo() const {return no;}
    /**< returns the class #.
     * the class No depends in which order classes are created (the first created class
     * has No 1 and so on)
     */
    
    virtual int getParseModes() const {return 0;}
    ///< returns an ORed combination of the parse modes (0 if none).
    
    virtual bool isInstance(UObject& object) const {return false;}
    ///< returns true if this 'object' derives from this class.
    
    virtual UObject* newInstance() const {return null;}
    /**< creates a new instance of the corresponding class (if applicable).
     * Ubit classes that derive from UObject should have an associated UClass that
     * redefines this method.
     */
    
    virtual UStyle* newStyle() const {return null;}
    /**< creates a new instance of the style of this class (if applicable).
     * Ubit classes that derive from UElem should have an associated UClass that
     * redefines this method.
     */
        
    UStyle* obtainStyle() const {return style ? style : (style = newStyle());}
    /**< returns the style prototype that is associated to this class.
     * Ubit classes that derive from UElem can have a style prototype.
     * This function calls newStyle() the 1st type it is called to create the
     * style prototype
     * WARNING: the returned style must NOT be deleted.
     */
    
    UAttrList* getAttributes() const {return attributes;}
    /**< returns current class properties.
     * WARNING: the returned properties must NOT be deleted.
     */
    
    void setAttributes(UAttrList* alist) const {attributes = alist;}
    /**< changes the class properties.
     * WARNING: the properties given as argument are not copied and will be destroyed
     * by the Class destr. They must NOT be deleted by another function
     */
    
  private:
    static unsigned int count;
    unsigned int no;
    UStr* name;
    mutable UStyle* style;
    mutable UAttrList* attributes;
    
    UClass(const UClass&);   // assignment is forbidden.
    UClass& operator=(const UClass&);
  };

}
#endif
