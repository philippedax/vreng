/************************************************************************
 *
 *  uargsImpl.cpp : class for creating argument lists ("additive notation").
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

#ifndef _uargsImpl_hpp_
#define	_uargsImpl_hpp_  1
#include <ubit/unode.hpp>
namespace ubit {
  
  /* ==================================================== ======== ======= */
  // implementation of UArgs (should not be used directly by clients).

  class UArgs;
  class UArgsImpl;
  class _UAttrArgs;
  
  const UArgsImpl& operator+(const UArgsImpl&, const char*);
  const UArgsImpl& operator+(const UArgsImpl&, UNode*);
  const UArgsImpl& operator+(const UArgsImpl&, UNode&);
  const UArgsImpl& operator+(const UArgsImpl&, const UChild&);
  const UArgsImpl& operator+(const UArgsImpl&, const UArgs&); 
  
  const _UAttrArgs& operator,(const _UAttrArgs&, UAttr*);
  const _UAttrArgs& operator,(const _UAttrArgs&, UAttr&);
  const _UAttrArgs& operator,(const _UAttrArgs&, UCall*);
  const _UAttrArgs& operator,(const _UAttrArgs&, UCall&);
  //const _UAttrArgs& operator,(const _UAttrArgs&, const UAttrChild&);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  class UArgsImpl {
  public:
    UArgsImpl();
    UArgsImpl(UNode*);
    UArgsImpl(UNode&);
    UArgsImpl(const char*);
    UArgsImpl(const UChild&);  
    UArgsImpl(const UArgs&);
    UArgsImpl(const UArgsImpl&);
    ~UArgsImpl();

    friend const UArgsImpl& operator+(const UArgsImpl&, const char*);
    friend const UArgsImpl& operator+(const UArgsImpl&, UNode*);
    friend const UArgsImpl& operator+(const UArgsImpl&, UNode&);
    friend const UArgsImpl& operator+(const UArgsImpl&, const UChild&);
    friend const UArgsImpl& operator+(const UArgsImpl&, const UArgs&);
    
    friend const UArgsImpl& operator+(const _UAttrArgs&, const UArgsImpl&);  // !!!!!

  private:
    UArgsImpl& operator=(const UArgsImpl&);  // assigment is forbidden
    friend class UElem;
    friend class UArgs;
    class UArgsChildren* children;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class _UAttrArgs {
  public:
    _UAttrArgs();
    _UAttrArgs(UAttr*);
    _UAttrArgs(UAttr&);
    _UAttrArgs(UCall*);
    _UAttrArgs(UCall&);
    _UAttrArgs(const UChild&);  
    //_UAttrArgs(const _UAttrArgs&);
    ~_UAttrArgs();

    friend const _UAttrArgs& operator,(const _UAttrArgs&, UAttr*);
    friend const _UAttrArgs& operator,(const _UAttrArgs&, UAttr&);
    friend const _UAttrArgs& operator,(const _UAttrArgs&, UCall*);
    friend const _UAttrArgs& operator,(const _UAttrArgs&, UCall&);
    //friend const _UAttrArgs& operator,(const _UAttrArgs&, const UAttrChild&);
    
    friend const UArgsImpl& operator+(const _UAttrArgs&, const UArgsImpl&);  // !!!!!

  private:
    _UAttrArgs& operator=(const _UAttrArgs&);  // assigment is forbidden
    friend class UElem;
    friend class UArgs;
    class UArgsChildren* children;
  };
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UArgsChildren : public UChildren {
    friend class UArgs;
    friend class UArgsImpl;
    friend class _UAttrArgs;
    int refcount;
    //UArgsChildren(); test for counting creation/deletion balance
    //~UArgsChildren();
  };
  
}
#endif
