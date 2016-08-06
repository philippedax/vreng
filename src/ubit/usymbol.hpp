/*************************************************************************
 *
 *  usymbol.hpp : graphical symbols that can be embedded in elements
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

#ifndef _usymbol_hpp_
#define	_usymbol_hpp_ 1
#include <ubit/udata.hpp>
namespace ubit {

/** Graphical Symbols.
 */
class USymbol: public UData {
public:
  UCLASS(USymbol)

  /// predefined symbols:
  static USymbol left, right, up, down, check, radio, square, circle;

  USymbol(const USymbol& = USymbol::left);
  ///< creates a new USymbol; @see also shortcut usymbol().
  
  virtual void set(const USymbol&);
  virtual void update();

  void setColor(const UColor&);
  void setFrontShadowColor(const UColor&);
  void setBackShadowColor(const UColor&);

  // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

protected:
  friend class UBox;
  enum  {
    S_LEFT, S_RIGHT, S_UP, S_DOWN, S_CHECK, S_RADIO, S_SQUARE, S_CIRCLE
  };

  // private constructor (internal implementation):
  USymbol(int index, UConst);
  int ix;  // internal index
  const class UColor *color, *frontShadowColor, *backShadowColor;
  virtual void getSize(UUpdateContext&, UDimension&) const;
  virtual void paint(UGraph&, UUpdateContext&, const URect&) const;
};

inline USymbol& usymbol(const USymbol& s) {return *new USymbol(s);}
///< shortcut function that creates a new USymbol.

}
#endif
