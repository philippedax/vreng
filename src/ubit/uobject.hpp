/************************************************************************
 *
 *  uobject.hpp: Base Class for all Ubit Objects
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

#ifndef _uobject_hpp_
#define	_uobject_hpp_ 1

#include <stdlib.h>    // for size_t, new(), delete()
#include <exception>
#include <ubit/udefs.hpp>
#include <ubit/uclass.hpp>

namespace ubit {
  
  /** Ubit exception.
   */
  class UError /*: public std::exception*/ {
  public:
    enum {
      FATAL_ERROR = -1, WARNING, ERROR, INTERNAL_ERROR, STYLE_ERROR, CSS_ERROR, XML_ERROR
    };
    
    UError(int errnum, const UObject*, const char* funcname);
    virtual ~UError() /*noexcept throw ()*/;
    
    virtual const char* what() const /*noexcept throw()*/;

    static const int message_capacity = 2000;
    int errnum;
    const UObject* object;
    const char* function_name;
    char message[message_capacity];
  };
  
  //notused void uerror(const char* function_name, const char* format, ...);
  /**< prints out an error message.
   * this method displays an error message (on std::cerr by default) :
   * - 'function_name' = the name of the function where the error occured
   * - 'format' = a printf-like format
   * - '...'    = a variadic list of arguments that correspond to 'format'.
   * @see also: uwarning() and UAppli::error().
   */
  
  //notused void uwarning(const char* function_name, const char* format, ...);
  //notused ///< prints out a warning message: @see uerror().
  

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Base class of most Ubit objects (SEE DETAILS!).
   *
   * UObject is the base class for (almost) all Ubit objects. Its main subclasses are
   * UNode, UAttr, UElem that play the same role as Node, Attribute and Element in a
   * XML/DOM model and UBox and UWin that are the base classes for widgets and windows.
   *
   *
   * 1. SUBCLASSING: UObject subclasses MUST invoke the UCLASS macro in the public section
   * of their declaration:
   * <pre>
   *   class UColor: public UAttr {
   *   public:
   *     UCLASS("UColor", UColor, new UColor)
   *     .....
   *  };
   * </pre>
   *
   * UCLASS redefines getClass() and Class() which return a metaclass that describes
   * the corresponding Ubit class (see UClass). getClass() is an instance method,
   * Class() a static method. 
   *
   *
   * 2. RECURSIVE DELETION: when a UObject instance is destroyed, its attributes and
   * chidren (when applicable, @see UNode and UElem) are recursively deleted if they:
   * - are NOT pointed by a UPtr SMART POINTER (see #3 below),
   * - are NOT referenced by another (undeleted) UObject as a child or an atttribute.
   *
   * Note that:
   * - UPtr(s) should be preferred to ordinary C++ pointers and references as they
   *   may reference data that has been implicitely deleted
   * - recursive destruction only apply to objects allocated in the heap (by using 'new'
   *   or a shortcut creator function such as ubox() or ubutton() that calls 'new')
   *
   *
   * 3. UPtr SMART POINTERS provide automatic memory management. UObject(s) are
   * automatically deleted when nothing reference them, i.e. when the last reference
   * from a UPtr or anoher UObject is removed (as said above, this only apply to 
   * objects created in the heap)  
   *
   *
   * Example:  
   *  <pre>
   *    class MyClass : public UObject {
   *        ....
   *    };
   *
   *    uptr<MyClass> p1 = new MyClass();   // p1 points to a new MyClass object
   *    uptr<MyClass> p2 = p1;              // p2 points to the same object
   *
   *    p1 = null;   // (null = 0) does nothing because p2 also points to the object
   *    p2 = null;   // destroys the object as no other uptr<> points on it
   *  </pre>
   * @see: UPtr, UObject, UClass
   */
  class UObject {
  public:
    struct MetaClass : public UClass {
      MetaClass() : UClass("UObject") {}
      //virtual bool isInstance(UObject& obj) const { return dynamic_cast<UObject*>(&obj); }
      //dax virtual bool isInstance(UObject* obj) const { return dynamic_cast<UObject*>(&obj); }
      virtual UObject* newInstance() const {return null;} 
      virtual UStyle* newStyle() const {return null;}
    };
    
    UObject();
    UObject(const UObject&);
    virtual ~UObject();
    UObject& operator=(const UObject&);
    
    //notused static const char* getVersion();
    //notused ///< returns the version of the Ubit package.

    static const UClass& Class() {static MetaClass& c = *new MetaClass; return c;}
    ///< class method that returns the metaclass of this class. @see getClass().

    virtual const UClass& getClass() const {return Class();}
    /**< instance method that returns the metaclass of this object.
     * Class() and getClass() return a metaclass (see UClass) that describes this
     * class. Class() is a class method, while getClass() is an instance method.
     *
     * WARNING: Class() and getClass() must be redefined by ALL Ubit classes.
     * This can be done by invoking the UCLASS macro (see UNode) in the public section
     */
    
    const UStr& getClassName() const;
    ///< returns getClass().getName().
         
    static UStyle* createStyle() {return null;}

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual void error(const char* function_name, const char* format, ...) const /*throw (UError)*/;
    /**< prints out an error message.
     * this method displays an error message (on std::cerr by default) :
     * - 'function_name' = the name of the function where the error occured
     * - 'format' = a printf-like format
     * - '...'    = a variadic list of arguments that correspond to 'format'.
     * @see also: warning() and UAppli::error().
     */
    
    virtual void warning(const char* function_name, const char* format, ...) const /*throw (UError)*/;
    ///< prints out a warning message: @see error().
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    bool isDeletable() const {return omodes.IS_DYNAMIC;}
    /**< checks if this object can be destroyed by 'delete'.
     * objects that were allocated in the heap or in static memory, or that are
     * plain fields of other objects cannot be destroyed by using 'delete'. 
     */
    
    bool isConst() const {return omodes.IS_CONST;}
    ///< checks whether the content of this object can be modified (@see setConst()).
    
    UObject& setConst();
    /**< specifies that the content of this object cannot be modified.
     * an attempt to modify the content of this object will then produce an error. 
     * Notes:
     * - constness can't be changed once set. 
     * - this feature makes it possible to specify that an object is constant even if 
     *   it is not declared 'const'. This makes it possible to have constant and non
     *   constant objects included in the same container. Constness is thus checked
     *   at run time instead of compilation time.
     * @see also: UCONST.
     */
     
    bool isAutoUpdate() const {return !omodes.DONT_AUTO_UPDATE;}
    ///< return current update policy: @see setAutoUpdate().

    UObject& setAutoUpdate(bool state = true);
    /**< changes the update policy of this object when its is modified.
     * if 'state' is true, the object if automatically updated when modified
     * @see UNode::setAutoUpdate() for more details.
     */
    
    bool isIgnoringChangeCallbacks() const {return omodes.IGNORE_CHANGE_CALLBACKS;}
    ///< return true if change callbacks are fired.
    
    UObject& ignoreChangeCallbacks(bool state);
    ///< change callbacks are not fired if argument is true.
    
    // - - - dynamic cast - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual UNode* toNode() {return null;}
    ///< dynamic cast: returns this object if it derives from UNode and null otherwise.
    
    virtual const UNode* toNode() const {return null;}
    ///< dynamic cast: returns this object if it derives from UNode and null otherwise.
    
    virtual UAttr* toAttr() {return null;}
    ///< dynamic cast: returns this object if it derives from UAttr and null otherwise.
    
    virtual const UAttr* toAttr() const {return null;}
    ///< dynamic cast: returns this object if it derives from UAttr and null otherwise.
    
    virtual UElem* toElem() {return null;}
    ///< dynamic cast: returns this object if it derives from UElem and null otherwise.
    
    virtual const UElem* toElem() const {return null;}
    ///< dynamic cast: returns this object if it derives from UElem and null otherwise.
        
    virtual UData* toData() {return null;}
    ///< dynamic cast: returns this object if it derives from UData and null otherwise.
    
    virtual const UData* toData() const {return null;}
    ///< dynamic cast: returns this object if it derives from UData and null otherwise.
    
    virtual UStr* toStr() {return null;}
    ///< dynamic cast: returns this object if it derives from UStr and null otherwise.
    
    virtual const UStr* toStr() const {return null;}
    ///< dynamic cast: returns this object if it derives from UStr and null otherwise.
    
    virtual UCall* toCall() {return null;}
    ///< dynamic cast: returns this object if it derives from UCall and null otherwise.
    
    virtual const UCall* toCall() const {return null;}
    ///< dynamic cast: returns this object if it derives from UCall and null otherwise.
        
    virtual UBox* toBox() {return null;}
    ///< dynamic cast: returns this object if it derives from UBox and null otherwise.
    
    virtual const UBox* toBox() const {return null;}
    ///< dynamic cast: returns this object if it derives from UBox and null otherwise.
    
    virtual UWin* toWin() {return null;}
    ///< dynamic cast: returns this object if it derives from UWin and null otherwise.
    
    virtual const UWin* toWin() const {return null;}
    ///< dynamic cast: returns this object if it derives from UWin and null otherwise.
    
    virtual UMenu* toMenu() {return null;}
    ///< dynamic cast: returns this object if it derives from UMenu and null otherwise.
    
    virtual const UMenu* toMenu() const {return null;}
    ///< dynamic cast: returns this object if it derives from UMenu and null otherwise.
      
    // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    typedef unsigned char State;
    typedef short PtrCount;
    
    State getState() const {return ostate;}
    ///< returns the current object state.
    
    void setState(State s) {ostate = s;}
    ///< changes the object state.
    
    PtrCount getPtrCount() const {return ptr_count;}  
    ///< [impl] returns the number of uptr<> smart pointers that are pointing to this object.
    
    /// [Impl] internal object modes.
    struct Modes {
      unsigned IS_DYNAMIC:1;         // this obj was allocated in dynamic memory
      unsigned IS_CONST:1;           // this obj is const, its value can't be changed
      unsigned IS_DESTRUCTED:1;      // this obj has been destructed
      unsigned IS_DESTRUCTING:1;     // this obj is being destructed
      unsigned DONT_AUTO_UPDATE:1;   // set(), add() and remove() do NOT call update().
      unsigned IS_UPDATING:1;        // is being updated (repainted or re-layed out).
      unsigned IS_IN_SELECTION:1;    // is part of a text selection
      unsigned IGNORE_CHANGE_CALLBACKS:1;
    };
    
    struct UConst {};
    
    static UConst UCONST;
    /**< UCONST can be given as an argument to some constructors to make the object constant.
     * an attempt to modify the value of this object will produce an error. 
     * Note that constness can't be changed once set. @see also: setConst().
     */
    
    bool checkConst() const;
    ///< [impl] produces an error if this object is logically constant (@see setConst()).
    
    bool isDestructed()  const {return omodes.IS_DESTRUCTED;}
    ///< [impl] this object has been destructed (but memory has not been freed yet).
    
    bool isDestructing() const {return omodes.IS_DESTRUCTING;}
    ///< [impl] this object is being destructed (one of its destructors has been called).
    
    virtual bool hasSceneGraphParent() const {return false;};    // !MUST be virtual!
    ///< [impl] returns true if there is at least one scene graph parent (redefined by UNode).
    
    void* operator new(size_t);
    ///< [impl] internal memory management.
    
    void operator delete(void*);
    ///< delete operator is forbidden on instances that derive from UObject.
    
    void addPtr() const;       ///< [Impl] a uptr is added to this object.
    void removePtr() const;    ///< [Impl] a uptr is removed from this object.

  protected:
    friend class UNode;
    friend class UUpdateContext;
    friend class UAppliImpl;
    friend class USelection;
    friend class UPtr;
    
    mutable PtrCount ptr_count;  // uptr<> reference count
    Modes omodes;
    State ostate;
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Smart Pointer for UObject instances.
   * All classes that derive from UObject support UPtr SMART POINTERS (SP), which 
   * provide automatic memory management (based on reference counting). UObject(s) are 
   * automatically deleted when nothing reference them, i.e. when the last reference 
   * from a UPtr or from anoher UObject is removed.
   *
   * UPtr is the base class for upr<> templates as shown in the examples below:
   * <pre>
   *    uptr<UBox> box = new UBox();             // 'box' is a Ubit Smart Pointer
   *    uptr<UButton> btn = ubutton("Do It!");   // ubutton() == *new UButton()
   *    box->add(*btn);                    // the UButton is a child of the UBox
   *
   *    box = null;  // destroys this UBox because it has no parent and no other
   *                 // uptr<> pointing to it (its UButton child is not destroyed 
   *                 // because 'btn' points to it)
   *
   *    btn = null;  // destroys this UBox because it has no parent and no other
   *                 // uptr<> pointing to it.
   * </pre>
   *
   * NOTES:
   * - an arbitrary number of SPs can reference a given UObject
   * - a UObject pointer by a UPtr cannot be destroyed by calling 'delete' (this would
   *   produce an error)
   * - SP should point to objects allocated in the heap (i.e. by using 'new' or a 
   *   shortcut creator function such as ubox() or ubutton() that calls 'new').
   *   SP have no effect on objects allocated in the stack or in global memory.
   *
   * @see: class UObject and the uptr<> template.
   */
  class UPtr {
  protected:
    //void addPtr(const UObject* o) {if (o) o->addPtr();}  
    //void removePtr(const UObject* o) {if (o) o->removePtr();}
    void deferenceError() const;
  };
  
  
  /** Smart Pointer for UObject instances (@see class UPtr).
   */
  template <class CC> class uptr : public UPtr {
  public:
    void addPtr(const CC* o) {if (o) o->addPtr();}  
    void removePtr(const CC* o) {if (o) o->removePtr();}
    
    uptr()         {obj = null;}
    uptr(CC& _obj) {obj = &_obj; addPtr(obj);}
    uptr(CC* _obj) {obj = _obj; addPtr(obj);}
    ///< constructors:  uptr<ObjClass> ptr = obj;
    
    uptr(const uptr<CC>& p2) {obj = p2.obj; addPtr(obj);}
    ///< constructor:  uptr<ObjClass> ptr = p2;
    
    ~uptr() {removePtr(obj);}
    ///< destructor: deletes pointed object (if not null and not pointed elsewhere).
    
    uptr<CC>& operator=(CC& _obj) {
      removePtr(obj); obj = &_obj; addPtr(obj); return *this;
    }
    uptr<CC>& operator=(CC* _obj) {
      removePtr(obj); obj = _obj; addPtr(obj); return *this;
    }
    ///< assignment: ptr = obj; (obj can be null).
    
    uptr<CC>& operator=(const uptr<CC>& p2) {
      removePtr(obj); obj = p2.obj; addPtr(obj); return *this;
    }
    ///< assignment: ptr = ptr2;
    
    operator CC*() const {return obj;}
    ///< type conversion: allows for:  ObjClass* obj = ptr.
    
    CC* operator&() const {return obj;}
    ///< returns pointed object: if 'ptr' points to 'obj' then &ptr returns obj.

    CC* operator()() const {return obj;}
    ///< returns pointed object: if 'ptr' points to 'obj' then ptr() returns obj.
    
    CC& operator*() const {
      if (!obj) deferenceError();
      return *obj;
    }
    ///< dereferencing: if 'ptr' points to 'obj' then *ptr return *obj.
    
    CC* operator->() const {
      if (!obj) deferenceError();
      return obj;
    }
    ///< dereferencing: if 'ptr' points to 'obj' then ptr->foo() calls obj->foo().
    
  private:
    CC* obj;
    CC& operator[](int);  // [] meaningless and forbidden.
  };
  
}
#endif
