/************************************************************************
 *
 *  uscrollpane.hpp
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

#ifndef _uscrollpane_hpp_
#define	_uscrollpane_hpp_ 1
#include <ubit/uborder.hpp>
#include <ubit/uview.hpp>
namespace ubit {
  
  /** A box with (optionnal) scrollbars that manages a viewport.
   * There are two ways to use a scroll pane:
   *
   * - a) to scroll a viewport: in this case the scrollpane must have one
   *   (and only one) child that derives from UBox 
   *
   * - b) to superpose several layers: a scrollpane can have several UBox children,
   *   in which case they are superimposed (but not scrolled, except for the last one).
   *   Transparent boxes can be superimposed in this way (see UAlpha). UCardbox can
   *   also be used for this purpose.
   *
   * Note: a scrollpane should only have children that derive from UBox, UAttr, UCall
   * (it should not have UElem or UData/UStr children)
   */ 
  class UScrollpane: public UBox {
  public:
    UCLASS(UScrollpane)

    UScrollpane(UArgs = UArgs::none);
    ///< create a scroll pane with 2 scrollbars; see also shortcut functions uscrollpane().
    
    UScrollpane(int vert_scrollbar, int horiz_scrollbar, UArgs = UArgs::none);
    /**< create a scroll pane with 0, 1 or 2 scrollbars; see also shortcut functions uscrollpane().
     * - the vertical scrollbar is created if 'vert_scrollbar' is true 
     * - the horizontal scrollbar is created if 'horiz_scrollbar' is true 
     */
         
    virtual ~UScrollpane();
    
    static UStyle* createStyle();

    void setTracking(bool);
    ///< tracking mode updates the value while the scrollbar is being dragged (default is true).
    
    float getXScroll() const {return xscroll;}
    ///< returns the current X scroll value (float value in [0,100]).
    
    float getYScroll() const {return yscroll;}
    ///< returns the current Y scroll value (float value in [0,100]).
  
    UScrollpane& setXScroll(float _xscroll) {return setScroll(_xscroll, yscroll);}
    ///< changes the X scroll value (float value in [0,100]).
    
    UScrollpane& setYScroll(float _yscroll) {return setScroll(xscroll, _yscroll);}
    ///< changes the Y scroll value (float value in [0,100]).
    
    virtual UScrollpane& setScroll(float xscroll, float yscroll);
    ///< changes the X and Y scroll values (float values in [0,100]).

    virtual void makeVisible(UBox& child);
    /**< scrolls the pane to make this child visible.
     * 'child' must be a direct or indirect child of the scrollpane
     */
      
    UScrollpane& showHScrollbar(bool state);
    UScrollpane& showVScrollbar(bool state);
    
    UScrollpane& showHScrollButtons(bool state);
    UScrollpane& showVScrollButtons(bool state);

    UScrollbar* getHScrollbar() {return hscrollbar;}
    ///< returns the horizontal scrollbar (if any; null otherwise)
    
    UScrollbar* getVScrollbar() {return vscrollbar;}
    ///< returns the vertical scrollbar (if any; null otherwise)
    
    UBox* getScrolledBox();
    ///< returns the box that is scrolled by this pane.
    
    UView* getScrolledView(UView* pane_view);
    /**< returns the appropriate view of the box that is scrolled by this pane.
     * A Pane can have multiple parents and thus, can appear several times
     * on the GUI. This functions makes return the corresponding view
     * of the scrolled box.
     */
  
    // - - - Impl.  - - - - - - - - - - - - - - - - - - - - - - - - - - -    
#ifndef NO_DOC
    virtual void setScrollImpl(float xscroll, float yscroll);    
  protected:
    friend class UScrollbar;
    UScrollbar *hscrollbar, *vscrollbar;
    float xscroll, yscroll;
    
    virtual void constructs(int vs_mode, int hs_mode, const UArgs&);
    virtual void unsetHScrollbar();
    virtual void unsetVScrollbar();
    virtual void resizeCB(UResizeEvent&);
    virtual void wheelCB(UWheelEvent&);
#endif
  };
  
  inline UScrollpane& uscrollpane(const UArgs& args = UArgs::none) 
  {return *new UScrollpane(args);}
  ///< shortcut function that returns *new UScrollpane(args).
  
  inline UScrollpane& uscrollpane(int vscrollbar, int hscrollbar, const UArgs& args = UArgs::none)
  {return *new UScrollpane(vscrollbar, hscrollbar, args);}
  ///< shortcut function that returns *new UScrollpane(vert_scrollbar, horiz_scrollbar, args).
  
  
  /* ==================================================== ======== ======= */
  /** [Impl] UPane View.
   */
  class UPaneView: public UView {
  public:
    static  UViewStyle style;  // renderer
    virtual UViewStyle* getViewStyle() {return &style;}
    
    UPaneView(UBox*, UView* parview, UHardwinImpl*);
    virtual ~UPaneView() {}
    static UView* createView(UBox*, UView* parview, UHardwinImpl*);
    
    virtual UPaneView* toPaneView() {return this;}
    
    float getXScroll() {return xscroll;}
    float getYScroll() {return yscroll;}
    
    virtual void setXScroll(float scr) {xscroll = scr;}
    virtual void setYScroll(float scr) {yscroll = scr;}
    ///< note that using these functions is not a good idea if this pane is already controlled by scrollbars.
    
    void setPadding(const UPaddingSpec& p) {padding = p;} // for UView::initLayoutViewport
    
#ifndef NO_DOC
    friend class UScrollpane;
    UPaddingSpec padding;
    float xscroll, yscroll;
#endif
  };
}
#endif

