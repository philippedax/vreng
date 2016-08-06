/************************************************************************
 *
 *  ucall.hpp: Callback objects
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

#ifndef _ucall_hpp_
#define	_ucall_hpp_ 1
#include <ubit/unode.hpp>
#include <ubit/uevent.hpp>
#include <typeinfo>
namespace ubit {
  
  /** base class of callback objects for firing functions or methods.
   *
   * UClass is the base class for a family of template subclasses that make it possible
   * to fire functions or methods when a certain condition occurs. These conditions
   * are specified by UOn objects as shown in the examples below:
   *
   * <pre>
   *   void foo() {...}
   *
   *   class Test {
   *   public:
   *     virtual void doit() {...}
   *     ...
   *   };
   *
   *   Test* t = new Test();
   *
   *   UButton& btn = 
   *   ubutton("Press Me"
   *            + UOn::action / ucall(foo)             // callback function
   *            + UOn::mpress / ucall(t, &Test::doit)  // callback method of Test
   *            );
   * </pre>
   * UOn::action and UOn::mpress are callback conditions while ucall(...) are callback
   * objects (repectively encapsulating a function and an instance method) so that
   * foo() and a->doit() will be called when the corresponding condition is matched for
   * this button (see UOn::action and UOn::mpress for the meaning of these conditions). 
   * Note that:
   * - the / operator must be between the condition and the callback object
   * - the condition can be omitted, in which case UOn::action is taken as a default
   * - as 'doit' is an instance method, 't' an instance of Test must be given as the
   *   first argument of ucall()
   *
   *
   * <b>Passing ARGUMENTS to functions and methods:</b>
   * - up to 2 arguments can be passed to functions and methods
   * - arguments are always passed by VALUE (i.e. copied). Hence, objects should
   *   generally be passed as pointers (they will be copied otherwise)
   * - the types of the arguments passed to ucall() must be EXACTLY THE SAME as the
   *   the types of the formal parameters of the corresponding function or method
   *   (there is no implicit type conversion because templates are used)
   *
   * <pre>
   *   void foo(double value) {...}
   *
   *   class Test {
   *   public:
   *     virtual void doit(int x, int y) {...}
   *     ...
   *   };
   *
   *   Test* t = new Test();
   *
   *   UButton& btn = 
   *   ubutton("Press Me"
   *            + UOn::action / ucall(0.5, foo)
   *            + UOn::mpress / ucall(t, 30, 20, &Test::doit)
   *            );
   * </pre>
   *
   *
   * <b>Optional EVENT parameter</b>
   * - callback functions and methods can have an optional UEvent& parameter which
   *   refers to the event that provoked the call
   * - the actual event type that can be used as a parameter depends on the callback
   *   condition (see the corresponding UOn condition for details). For instance,
   *   doit() can have UMouseEvent as a parameter because it is fired by UOn::mdrag.
   * - event parameters can be superclasses of the event class specified by the UOn
   *   condition, but they cannot be sublasses (this will generate an execution error).
   *   For instance, UEvent& would be a valid parameter for doit() but UMouseEvent&
   *   would not be a valid parameter for foo().
   * <pre>
   *   void foo(UEvent& e, double value) {...}
   *
   *   class Test {
   *   public:
   *     virtual void doit(UMouseEvent& e, int x, int y) {...}
   *     ...
   *   };
   * </pre>
   *
   *
   * <b>Conditionnal attributes</b>
   * - attributes can be made 'conditional', in which case they will only be applied
   *   if the callback condition is verified for that object:
   * <pre>
   *   ucheckbox("Red or White"
   *             + UOn::select / UColor::red 
   *             + UOn::deselect / UColor::white
   *             )
   * </pre>
   * In this case, the color will be red if the checkbox is seledcted and white otherwise.
   * 
   *
   * <b>Adding and removing callbacks after widget creation</b>
   * callback objects can be added and removed by using the add(), addAttr(),
   * remove() and removeAttr() methods of the UElem class.
   *
   * <b>Predefined callback objects</b>
   * - uset(a,b)         performs:  a.set(b)  // a,b are passed by reference
   * - uassign(a,b)      performs:  a = b     // a,b are passed by reference
   * - ucopy(a,b)        performs:  a = b     // b is passed by value (copied)
   * - ushow(obj, state) performs:  obj.show(state)
   * - ucloseWin()      closes the first parent window that contains this expression
   * 
   * <pre>
   *   uptr<UDialog> dial = 
   *     udialog(...whatever... 
   *             + ubutton( "Close" + ucloseWin() )
   *             );
   *  </pre>
   * the "Close" button will close this dialog when activated (e.g. clicked)
   *
   *
   * <b>ucall() templates</b>
   * - ucall() expressions are templates that creates appropriate UCall subclasses 
   *   depending on the arguments that are passed to them
   *
   * @see also: UOn callback conditions.
   */
  class UCall: public UNode {
  public:
    //UCLASS("#call", UCall, null)
    UABSTRACT_CLASS(UCall)
    
    virtual ~UCall() {destructs();}

    virtual void operator()(UEvent&) = 0;
    ///< the method that fires callbacks; must be redefined by subclasses.
    
  protected:
    virtual UCall* toCall() {return this;}
    virtual const UCall* toCall() const {return this;}
    virtual void addingTo(UChild&, UElem& parent);
    
    template <class E> 
    E* checkType(UEvent& e) {
      E* actual_e = dynamic_cast<E*>(&e);
      if (actual_e) return actual_e;
      else {
        wrongEventType(typeid(E).name(), typeid(e).name());
        return null;
        //return (E*)&e; !!!
      }
    }
    
    virtual void wrongEventType(const char* event_name, const char* call_name);
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  /// Callback object that closes the first window (UDialog, UMenu...) that contains this element.
  UCall& ucloseWin(int stat = 0);
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // uopen()
  
  template <class CC>
  class UCall_open : public UCall {
    CC obj;
  public:
    UCall_open(CC _o) : obj(_o) {}
    void operator()(UEvent& e) {
      UMouseEvent*_e = checkType<UMouseEvent>(e); if (_e) obj.open(*_e);
    }
  };
  
  
  /// Callback object that perfoms obj.open() when fired.
  template <class CC> 
  UCall& uopen(CC& obj) {return *new UCall_open<CC&>(obj);}
  
  /// Callback object that perfoms obj->open() when fired.
  template <class CC> 
  UCall& uopen(CC* obj) {return *new UCall_open<CC&>(*obj);}
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // ushow()
  
  template <class CC, class VAL>
  class UCall_show : public UCall {
    CC obj; VAL val;
  public:
    UCall_show(CC _o, VAL _v) : obj(_o), val(_v) {}
    void operator()(UEvent&) {obj.show(val);}
  };
  
  
  /// Callback object that perfoms obj.show() when fired.
  template <class CC> 
  UCall& ushow(CC& obj) {return *new UCall_show<CC&,bool>(obj,true);}
  
  /// Callback object that perfoms obj->show() when fired.
  template <class CC> 
  UCall& ushow(CC* obj) {return *new UCall_show<CC&,bool>(*obj,true);}

  
  /// Callback object that perfoms obj.show(state) when fired.
  // NB: not available with g++2.*.* doit etre en premier avec CC car valeur par defaut 
  template <class CC, class VAL> 
  UCall& ushow(CC& obj, VAL state) {return *new UCall_show<CC&,VAL>(obj,state);}
    
  /// Callback object that perfoms obj.show(state) when fired.
  template <class CC, class VAL> 
  UCall& ushow(CC* obj, VAL state) {return *new UCall_show<CC&,VAL>(*obj,state);}
   
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // uset(x, y)
  
  template <class CC, class VAL>
  class UCall_set : public UCall {
    CC obj; VAL val;
  public:
    UCall_set(CC _o, VAL _v) : obj(_o), val(_v) {}
    void operator()(UEvent&) {obj.set(val);}
  };
  
  /** Callback object that calls the set() method when fired.
   * uset(a, b) performs a.set(b) when fired.
   * - a and b are passed by reference and are not duplicated
   * -'a' must have a set() method that can take 'b' as an argument
   */
  template <class CC, class VAL> 
  UCall& uset(CC& _o, VAL& _v) {return *new UCall_set<CC&,VAL&>(_o,_v);}
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // uassign(x, y)
  
  template <class CC, class VAL>
  class UCall_assign : public UCall {
    CC obj; VAL val;
  public:
    UCall_assign(CC _o, VAL _v) : obj(_o), val(_v) {}
    void operator()(UEvent&) {obj = val;}
  };
  
  /** Callback object that calls the = operator when fired.
   * uassign(a, b) and ucopy(a, b) perform a = b when fired.
   * - when using uassign(a,b), b is passed by reference
   * - when using ucopy(a,b), b is passed by value (ie. copied)
   * - a is always passed by reference.
   */
  template <class CC, class VAL> 
  UCall& uassign(CC& _o, VAL& _v) {return *new UCall_assign<CC&,VAL&>(_o,_v);}
  
  template <class CC, class VAL> 
  UCall& ucopy(CC& _o, VAL _v) {return *new UCall_assign<CC&,VAL>(_o,_v);}
  
  
  // ============================================================== [Elc] ======== 
  // Callback objects for firing objects' methods. 
  
  // ucall(obj, &Obj::method)
  
  template <class O, class R, class M> 
  class UCall_M0 : public UCall {
    O obj;
    R (M::*fun)();
  public:
    UCall_M0(O o, R(M::*m)()) : obj(o), fun(m) {}
    void operator()(UEvent&) {(obj.*fun)();}
  };
  
  template <class O, class R,  class M, class E> 
  class UCall_M0E : public UCall {
    O obj;
    R (M::*fun)(E&);
  public:
    UCall_M0E(O o, R(M::*m)(E&)) : obj(o), fun(m) {}
    void operator()(UEvent& e) {E*_e = checkType<E>(e); if(_e) (obj.*fun)(*_e);}
  };
  
  
  template <class O, class R, class M> 
  UCall& ucall(O& o, R(M::*m)()) {return *new UCall_M0<O&, R,M>(o,m);}
  ///< callback object that calls obj.foo(); syntax: ucall(obj, &Obj::foo).
  
  template <class O, class R, class M, class E> 
  UCall& ucall(O& o, R(M::*m)(E&)) {return *new UCall_M0E<O&, R,M,E>(o,m);}
  ///< callback object that calls obj.foo(UEvent&); syntax: ucall(obj, &Obj::foo).
  
  template <class O, class R, class M> 
  UCall& ucall(O* o, R(M::*m)()) {return *new UCall_M0<O&, R,M>(*o,m);}
  ///< callback object that calls obj->foo(); syntax: ucall(obj, &Obj::foo).
   
  template <class O, class R, class M, class E> 
  UCall& ucall(O* o, R(M::*m)(E&)) {return *new UCall_M0E<O&, R,M,E>(*o,m);}
  ///< callback object that calls obj->foo(UEvent&); syntax: ucall(obj, &Obj::foo).

  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ucall(obj, arg1, &Obj::method)
  
  template <class O, class A, class R, class M> 
  class UCall_M1 : public UCall {
    O obj;
    R (M::*fun)(A);
    A arg;
  public:
    UCall_M1(O o, R(M::*m)(A), A a) : obj(o), fun(m), arg(a) {}
    void operator()(UEvent&) {(obj.*fun)(arg);}
  };
  
  template <class O, class A, class R, class M, class E> 
  class UCall_M1E : public UCall {
    O obj;
    R (M::*fun)(E&, A);
    A arg1;
  public:
    UCall_M1E(O o, R(M::*m)(E&,A), A a) : obj(o), fun(m), arg1(a) {}
    void operator()(UEvent& e) {E*_e = checkType<E>(e); if(_e) (obj.*fun)(*_e,arg1);}
  };
  
  
  template <class O, class A, class R, class M>
  UCall& ucall(O& o, A a, R(M::*m)(A)) {return *new UCall_M1<O&, A,R,M>(o,m,a);}
  ///< callback object that calls obj.foo(arg); syntax: ucall(obj, arg, &Obj::foo).
  
  template <class O, class A, class R, class M>
  UCall& ucall(O& o, A& a, R(M::*m)(A&)) {return *new UCall_M1<O&, A&,R,M>(o,m,a);}
  ///< callback object that calls obj.foo(arg&); syntax: ucall(obj, arg, &Obj::foo).
  
  template <class O, class A, class R, class M, class E> 
  UCall& ucall(O& o, A a, R(M::*m)(E&,A)) {return *new UCall_M1E<O&, A,R,M,E>(o,m,a);}
  ///< callback object that calls obj.foo(UEvent&, arg); syntax: ucall(obj, arg, &Obj::foo).
  
  template <class O, class A, class R, class M, class E> 
  UCall& ucall(O& o, A& a, R(M::*m)(E&,A&)) {return *new UCall_M1E<O&, A&,R,M,E>(o,m,a);}
  ///< callback object that calls obj.foo(UEvent&, arg&); syntax: ucall(obj, arg, &Obj::foo).
  
  
  template <class O, class A, class R, class M>
  UCall& ucall(O* o, A a, R(M::*m)(A)) {return *new UCall_M1<O&, A,R,M>(*o,m,a);}
  ///< callback object that calls obj->foo(Arg); syntax: ucall(obj, arg, &Obj::foo).
  
  template <class O, class A, class R, class M>
  UCall& ucall(O* o, A& a, R(M::*m)(A&)) {return *new UCall_M1<O&, A&,R,M>(*o,m,a);}
  ///< callback object that calls obj->foo(Arg&); syntax: ucall(obj, arg, &Obj::foo).
  
  template <class O, class A, class R, class M, class E> 
  UCall& ucall(O* o, A a, R(M::*m)(E&,A)) {return *new UCall_M1E<O&, A,R,M,E>(*o,m,a);}
  ///< callback object that calls obj->foo(UEvent&, Arg); syntax: ucall(obj, arg, &Obj::foo).
  
  template <class O, class A, class R, class M, class E> 
  UCall& ucall(O* o, A& a, R(M::*m)(E&,A&)) {return *new UCall_M1E<O&, A&,R,M,E>(*o,m,a);}
  ///< callback object that calls obj->foo(UEvent&, Arg&); syntax: ucall(obj, arg, &Obj::foo).
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ucall(obj, arg1, args2, &Obj::method)
  
  template <class O, class A1, class A2, class R, class M> 
  class UCall_M2 : public UCall {
    O obj;
    R (M::*fun)(A1,A2);
    A1 arg1; 
    A2 arg2;
  public:
    UCall_M2(O o, R(M::*m)(A1,A2), A1 a1, A2 a2) : obj(o), fun(m), arg1(a1), arg2(a2) {} 
    void operator()(UEvent&) {(obj.*fun)(arg1, arg2);}
  };
  
  template <class O, class A1, class A2, class R, class M, class E> 
  class UCall_M2E : public UCall {
    O obj;
    R (M::*fun)(E&,A1,A2);
    A1 arg1; 
    A2 arg2;
  public:
    UCall_M2E(O o, R(M::*m)(E&,A1,A2), A1 a1, A2 a2): obj(o),fun(m),arg1(a1),arg2(a2) {} 
    void operator()(UEvent& e) {E*_e = checkType<E>(e); if(_e) (obj.*fun)(*_e,arg1,arg2);}
  };
  
  template <class O, class A1, class A2, class R, class M> 
  UCall& ucall(O& o, A1 a1, A2 a2, R(M::*m)(A1,A2)) 
  {return *new UCall_M2<O&,A1,A2,R,M>(o, m, a1, a2);}
  ///< callback object that calls obj.foo(arg1, arg2); syntax: ucall(obj, arg1, arg2, &Obj::foo).
  
  template <class O, class A1, class A2, class R, class M, class E> 
  UCall& ucall(O& o, A1 a1, A2 a2, R(M::*m)(E&,A1,A2)) 
  {return *new UCall_M2E<O&,A1,A2,R,M,E>(o, m, a1, a2);}
  ///< callback object that calls obj.foo(UEvent&, arg1, arg2); syntax: ucall(obj, arg1, arg2, &Obj::foo).
  
  template <class O, class A1, class A2, class R, class M> 
  UCall& ucall(O* o, A1 a1, A2 a2, R(M::*m)(A1,A2)) 
  {return *new UCall_M2<O&,A1,A2,R,M>(*o, m, a1, a2);}
  ///< callback object that calls obj->foo(arg1, arg2); syntax: ucall(obj, arg1, arg2, &Obj::foo).
  
  template <class O, class A1, class A2, class R, class M, class E> 
  UCall& ucall(O* o, A1 a1, A2 a2, R(M::*m)(E&,A1,A2)) 
  {return *new UCall_M2E<O&,A1,A2,R,M,E>(*o, m, a1, a2);}
  ///< callback object that calls obj->foo(UEvent&, arg1, arg2); syntax: ucall(obj, arg1, arg2, &Obj::foo).
  
  
  // ============================================================ [Elc] ======== 
  // Callback objects for firing functions. 
  
  // ucall(func)
  
  template <class R> 
  class UCall_F0 : public UCall {
    R (*fun)();
  public:
    UCall_F0( R(*f)()) : fun(f) {}
    void operator()(UEvent&) {(*fun)();}
  };
  
  template <class R, class E> 
  class UCall_F0E : public UCall {
    R (*fun)(E&);
  public:
    UCall_F0E( R(*f)(E&)) : fun(f) {}
    void operator()(UEvent& e) {E*_e = checkType<E>(e); if(_e) (*fun)(*_e);}
  };
  
  
  template <class R> 
  UCall& ucall(R(*f)()) {return *new UCall_F0<R>(f);}
  ///< creates a callback object that calls foo(); syntax: ucall(foo).
    
  template <class R, class E> 
  UCall& ucall(R(*f)(E&)) {return *new UCall_F0E<R,E>(f);}
  ///< creates a callback object that calls foo(UEvent&); syntax: ucall(foo).
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ucall(arg, function)
  
  template <class A, class R> 
  class UCall_F1 : public UCall {
    R (*fun)(A);
    A arg;
  public:
    UCall_F1( R(*f)(A), A a) : fun(f), arg(a) {}
    void operator()(UEvent&) {(*fun)(arg);}
  };
  
  template <class A, class R, class E> 
  class UCall_F1E : public UCall {
    R (*fun)(E&, A);
    A arg1;
  public:
    UCall_F1E( R(*f)(E&,A), A a) : fun(f), arg1(a) {}
    void operator()(UEvent& e) {E*_e = checkType<E>(e); if(_e) (*fun)(*_e,arg1);}
  };
  
  
  template <class A, class R> 
  UCall& ucall(A a, R(*f)(A)) {return *new UCall_F1<A,R>(f,a);}
  ///< callback object that calls foo(arg); syntax: ucall(arg, foo).
  
  template <class A, class R> 
  UCall& ucall(A& a, R(*f)(A&)) {return *new UCall_F1<A&,R>(f,a);}
  ///< creates a callback object that calls foo(arg); syntax: ucall(arg, foo).
  
  template <class A, class R, class E> 
  UCall& ucall(A a, R(*f)(E&,A)) {return *new UCall_F1E<A,R,E>(f,a);}
  ///< creates a callback object that calls foo(UEvent&, arg); syntax: ucall(arg, foo).
  
  template <class A, class R, class E> 
  UCall& ucall(A& a, R(*f)(E&,A&)) {return *new UCall_F1E<A&,R,E>(f,a);}
  ///< creates a callback object that calls foo(UEvent&, arg); syntax: ucall(arg, foo).
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // ucall(arg1, arg2, function)
  
  template <class A1, class A2, class R> 
  class UCall_F2 : public UCall {
    R (*fun)(A1,A2);
    A1 arg1; A2 arg2;
  public:
    UCall_F2( R(*f)(A1,A2), A1 a1, A2 a2) : fun(f), arg1(a1), arg2(a2) {}
    void operator()(UEvent&) {(*fun)(arg1,arg2);}
  };
  
  template <class A1, class A2, class R, class E> 
  class UCall_F2E : public UCall {
    R (*fun)(E&,A1,A2);
    A1 arg1; 
    A2 arg2;
  public:
    UCall_F2E( R(*f)(E&,A1,A2), A1 a1, A2 a2) : fun(f), arg1(a1), arg2(a2) {}
    void operator()(UEvent& e) {E*_e = checkType<E>(e); if(_e) (*fun)(*_e,arg1,arg2);}
  };
  
  template <class A1, class A2, class R> 
  UCall& ucall(A1 a1, A2 a2, R(*f)(A1,A2)) {return *new UCall_F2<A1,A2,R>(f,a1,a2);}
  ///< creates a callback object that calls foo(args1, arg2); syntax: ucall(arg1, arg2, foo).
  
  template <class A1, class A2, class R, class E> 
  UCall& ucall(A1 a1, A2 a2, R(*f)(E&,A1,A2)) {return *new UCall_F2E<A1,A2,R,E>(f,a1,a2);}
  ///< creates a callback object that calls foo(UEvent&, args1, arg2); syntax: ucall(arg1, arg2, foo).
  
}
#endif
