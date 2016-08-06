/* ==================================================== ======== ======= *
 *
 *  uselection.hpp
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

#ifndef _uselection_hpp_
#define	_uselection_hpp_
#include <ubit/udefs.hpp>
namespace ubit {


/** text selection.
*/
class USelection {
public:
  USelection(const UColor* color, const UColor* bgcolor, const UFont*);
  ~USelection();
  
  void copyText(UStr&);
  void cutText(UStr&);
  void deleteText();
  void clear();

  UColor& color()   {return *pcolor;}
  UColor& bgcolor() {return *pbgcolor;}
  UFont&  font()    {return *pfont;}

  const UElem* getObj() {return in_obj;}

protected:
  void setObj(UElem* obj);
  void start(UMouseEvent&);
  void extend(UMouseEvent&);
  bool complete(UMouseEvent&);
  void keyPress(UKeyEvent&);
  bool beingSelected() {return being_selected;}
  void perform(UStr* copy, bool del);

private:
  friend class UEdit;
  friend class UStr;
  friend class UEventFlow;
  
  bool being_selected;  
  bool rec_sel;  //< recursive selection (the default).
  UElem* in_obj;
  
  static const UChildIter null_link;
  // reordered in the left to right order
  UChildIter fromLink, toLink, pressLink, endPressLink, oldLink;
  long  fromPos, toPos, pressPos, oldPos;
  uptr <UColor> pcolor;
  uptr <UColor> pbgcolor;
  uptr <UFont>  pfont;

  void update(UDataContext*);
  void paint(long refreshFromPos, long refreshToPos);
  void paintImpl(UElem* obj, bool state, long refreshFromPos, long refreshToPos);
};
}
#endif

