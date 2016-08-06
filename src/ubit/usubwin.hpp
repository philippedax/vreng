/************************************************************************
 *
 *  usubwin.hpp
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

#ifndef _usubwin_hpp_
#define	_usubwin_hpp_ 1
#include <ubit/uwin.hpp>
namespace ubit {
  
  /** Subwindow: a hard window that is embedded inside another window.
   * This class should not be used in most applications. It creates a non-toplevel 
   * system window (typically a X11 "child window") that is embbeded into another window.
   * It can be useful in some specific cases (for instance for drawing GL graphics:
   * see UGlcanvas) but should be avoided in the general case because it breaks
   * certain graphical properties of the toolkit, such as the possibility to have
   * transparent windows and menus (they must then be created as "hard windows",
   * see UWin, otherwise they won't appear if they are on top of a Subwindow)
   *
   * @see also subclass: UGlcanvas.
   */
  class USubwin : public UWin {
  public:
    UCLASS(USubwin)
    
    USubwin(UArgs = UArgs::none);
    
    virtual bool isSubWin() const {return true;}
    
    static UStyle* createStyle(); // redefined
    virtual int getDisplayType() const {return BLOCK;} // redefined
    
    // - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual bool realize();
    
  protected:
    virtual void initView(UView* parent_view);
    // must be redefined to init parent view properly.
    
    virtual void motionImpl(UViewEvent&);
    
    virtual void resizeImpl(UResizeEvent&);
    // redefined by UGlcanvas.
  };
  
}
#endif
