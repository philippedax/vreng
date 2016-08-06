/* ==================================================== ======== ======= *
 *
 *  ufontImpl.hpp
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

#ifndef _ufontImpl_hpp_
#define	_ufontImpl_hpp_ 1
namespace ubit {
  
  /** [impl] Internal representation for fonts.
   * should not be used directly
   */
  class UFontDesc {
  public:
    UFontDesc() {}
    UFontDesc(const UFont&);
    
    void set(const UFont&);
    void merge(const UFont&); // merge only: set NON default values and combine styles.
    void setScale(float xyscale); // MUST be called to init the FontDesc !
    
    const UFontFamily* family;
    short styles, def_size, actual_size;
    mutable short findex;
    float scaled_size;
  };
  
}
#endif
