/************************************************************************
 *
 *  upalette.hpp: movable palette box (internal frame)
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

#ifndef _upalette_hpp_
#define	_upalette_hpp_ 1
#include <ubit/ubox.hpp>
namespace ubit {
  
  /** movable palette box (internal frame).
   */
  class UPalette : public UBox {
  public:
    UCLASS(UPalette)
    static UStyle* createStyle();

    UPalette(UArgs content_args = UArgs::none);

    UBox& content()  {return *pcontent;}
    ///< returns the content (the working area of the palette).

    UElem& title()    {return *ptitle;}
    ///< returns the title (located in the left hand part of the titlebar()).

    UElem& controls() {return *pcontrols;}
    ///< returns the controls area (located in the right hand part of the titlebar()).
        
    UBox& titleBar()  {return *ptitle_bar;}
    ///< returns the title bar (which includes a title() and a controls() area).
    
    UBox& resizeBtn() {return *presize_btn;}
    ///<returns the resize button (not show by default, resizeBtn().show() to make it appear).

    UPos&   pos()   {return *ppos;}
    USize&  size()  {return *psize;}
    UScale& contentScale() {return *pcontent_scale;}  
  
    void setPosModel(UPos*);
    void setPosControlModel(UPosControl*);

  protected:
    uptr<UPos> ppos;
    uptr<UPosControl> ppos_ctrl;
    uptr<USize> psize;
    uptr<USizeControl> psize_ctrl;
    uptr<UScale> pcontent_scale;
    uptr<UBox> pcontent;
    uptr<UElem> ptitle;
    uptr<UElem> pcontrols;
    uptr<UBox> ptitle_bar;
    uptr<UBox> presize_btn;
  };
  
  inline UPalette& upalette(UArgs content = UArgs::none) {return *new UPalette(content);}
  ///< shortcut function that returns *new UPalette(content).

}
#endif
