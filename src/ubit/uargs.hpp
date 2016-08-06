/************************************************************************
 *
 *  uargs.hpp : argument lists.
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

#ifndef _uargs_hpp_
#define	_uargs_hpp_  1
#include <ubit/uargsImpl.hpp>
namespace ubit {
  
  class UArgsImpl;
    
  /**< Argument list (for passing arguments to constructor or add functions).
   * UArgs makes it possible to create "additive" argument lists by using the + operator.
   *
   * A UArg object may either hold a single object pointer or reference, or a list 
   * of object pointers or references separated by + operators, example:
   * <pre>
   *   class UButton : public UBox {
   *     UButton(UArgs arglist);
   *     ....
   *   };
   *
   *   void foo() {
   *     UButton* b = new UButton(UPix::disquette + "Save" + ucall(obj, saveFunc));
   *     ...
   *   }
   * </pre>
   *
   * UArgs::none is the empty arglist.
   */
  class UArgs {
  public:
    static const UArgs none;
    ///< the empty arglist.
    
    ~UArgs();
    UArgs();
    UArgs(UNode*);
    UArgs(UNode&);
    UArgs(const char*);
    UArgs(const UChild&);
    UArgs(const UArgs&);
    UArgs(const UArgsImpl&);
      
    bool operator !() const;
    ///< returns true is this arglist is empty.
    
    bool empty() const;
    ///< returns true is this arglist is empty.
    
    UArgs& operator+=(const UArgs& arglist2);
    ///< adds the children contained in 'arglist2' to this arglist.
    
    friend const UArgsImpl& operator+(const UArgsImpl&, const char* c_string);
    ///< adds a C String to this arglist (note that the string content is duplicated).
    
    friend const UArgsImpl& operator+(const UArgsImpl&, UNode*);
    ///< adds a UNode to this arglist (UNode is the base class for objects that can be part of the scene graph).
    
    friend const UArgsImpl& operator+(const UArgsImpl&, UNode&);
    ///< adds a UNode to this arglist (UNode is the base class for objects that can be part of the scene graph).
    
    friend const UArgsImpl& operator+(const UArgsImpl&, const UChild&);
    /**< adds a UChild to this arglist.
     * a UChild holds a UNode (UNode is the base class for objects that can be part
     * of the scene graph) and, possibly, a UCond (a condition that specifies if
     * this node is active). Example:
     * <pre>
     *    UOn::action / ucall(obj, &MyObj::foo)
     * </pre>
     * This statement is stored in a UChild where UOn::action corresponds to the
     * condition and ucall(...) to the corresponding UNode (which is, in this example
     * a callback object that will fire: obj->foo()).
     * Note however that the condition part is generally null in which case the
     * corresponding object is always active.
     */
    
    friend const UArgsImpl& operator+(const UArgsImpl&, const UArgs&);
    ///< adds the children contained in 'arglist2' to this arglist.
    
  private:  
    UArgs& operator=(const UArgs&); 
    ///< assigment is forbidden.
    
    friend class UElem;
    friend class UArgsImpl;
    class UArgsChildren* children;
  };
  
}
#endif
