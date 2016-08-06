/************************************************************************
 *
 *  uscrollbar.hpp
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

#ifndef _uscrollbar_hpp_
#define	_uscrollbar_hpp_ 1
#include <ubit/ubox.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uview.hpp>
namespace ubit {
  
  /** Scrollbar widget.
   * the value is a percentage in the range [0., 100.]
   *
   * Geometry:
   *  - horizontal orientation by default 
   *  - addAttr(UOrient::vertical) makes the scrollbar vertical
   *  - shortcut functions uhscrollbar() and uvscrollbar() specify the orientation
   *
   * Callbacks
   *  - UOn::action (default callback) is fired when the value is changed
   *  - UOn::change is fired continuously while the knob is dragged
   *  both callbacks are equivalent when isTracking() is true
   */
  class UScrollbar: public UBox {
  public:
    UCLASS(UScrollbar)

    UScrollbar(const UArgs& = UArgs::none);
    /**< creates a new scrollbar (@see also shortcut uscrollbar()).
     * Default orientation is vertical. new UScrollbar(UOrient::horizontal) creates
     * a horizontal slider. The scrollbar value is in the range [0., 100.]
     */
    
    UScrollbar(UFloat& value, const UArgs& = UArgs::none);
    /**< creates a new scrollbar with a model value.
     * the scrollbar is synchronized with this value. Several widgets 
     * (eg.scrollbar) that share the same value are synchronized.
     * Default orientation is vertical. new USlider(value, UOrient::horizontal) 
     * creates a horizontal scrollbar. The value is in the range [0., 100.]
     */
    
    virtual ~UScrollbar();
        
    static UStyle* createStyle();

    virtual float getValue() const;
    ///< returns the current value (a float between 0 and 100).
    
    virtual UScrollbar& setValue(float percent);
    /**< changes the current value (a float between 0 and 100).
     * Note: UOn::change callbacks are activated when the value is changed.
     */
    
    virtual float getUnitIncrement() const;
    ///< returns the unit increment.

    virtual UScrollbar& setUnitIncrement(float);
    ///< change the unit increment.
    
    virtual float getBlockIncrement() const;
    ///< returns the block increment.

    virtual UScrollbar& setBlockIncrement(float);
    ///< change the block increment.
    
    virtual UScrollbar& setTracking(bool);
    ///< tracking mode updates the value while the scrollbar is being dragged (default is true).

    virtual bool isTracking() const;
    
    virtual UScrollbar& setTransparent(bool state = true);
    ///< transparent scrollbars are displayed above scrollpane viewports (default is false).

    virtual bool isTransparent() const;
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // implementation
    
    UFloat& value() {return *pvalue;}

    virtual UBox* getKnob() {return pknob;}
    virtual UBox* getRail() {return prail;}
    virtual UBox* getLessButton() {return pless_btn;}
    virtual UBox* getMoreButton() {return pmore_btn;}
    ///< return the knob, the rail and the 2 scroll buttons (if any, null otherwise).
    
    virtual UBox* createKnob(bool vertical);
    virtual UBox* createRail(bool vertical);
    virtual UBox* createLessButton(bool vertical);
    virtual UBox* createMoreButton(bool vertical);
    /* fonctions that create the knob, the rail and the 2 scroll buttons.
     * can be redefined by subclasses to use other widgets
     */
    
    virtual void setPane(UScrollpane*);
    UScrollpane* getPane() {return ppane;}
    ///< [impl] attaches the scrollbar to a scrollpane.
    
    virtual void setValueImpl(float percent, bool update_pane);
    ///< [impl] does not update the associated pane.
    
    virtual void pressScrollButton(UMouseEvent&, int dir);
    ///< [impl].
    
  private:
    friend class UScrollbarView;
    friend class UScrollpane;
    uptr<UFloat> pvalue;	        // scrollbar value (percentage)
    uptr<UScrollpane> ppane;	    // the pane controlled by this scrollbar
    uptr<UBox> pless_btn, pmore_btn, pknob, prail;
    uptr<UPos> pknob_pos;		      // current position of the knob
    float unit_increment, block_increment;
    float delta_mouse;
    bool press_rail_goto_pos;     // dont scroll but goes to pos is trus
    bool is_transparent, show_rail, tracking_mode; //enter_feedback, drag_feedback
    
    void constructs();
    //nb: inutile que contructs soit virtuel car appele par constructeur!
    
    virtual void pressKnob(UMouseEvent&);
    virtual void releaseKnob(UMouseEvent&);
    virtual void dragKnob(UMouseEvent&);
    virtual void pressRail(UMouseEvent&);
    virtual void releaseRail(UMouseEvent&);
    virtual float getPercent(UMouseEvent&, UView* knob_view, UView* rail_view, float delta_mouse);
    virtual float getIncrementPercent(UMouseEvent&, float increment);
  };
  
  inline UScrollbar& uscrollbar(const UArgs& args = UArgs::none) {return *new UScrollbar(args);}
  ///< shortcut that creates a new scrollbar (that is vertical by default).
  
  UScrollbar& uhscrollbar(const UArgs& args = UArgs::none);
  ///< shortcut that creates a new horizontal scrollbar
  
  inline UScrollbar& uvscrollbar(const UArgs& args = UArgs::none) {return *new UScrollbar(args);}
  ///< shortcut that creates a new vertical scrollbar
  

  /* ==================================================== ===== ======= */
  /** Scrollbar View Renderer.
   *  used by: UScrollbar
   *  - Implementation: clients usually don't need to use this class
   */
  class UScrollbarView : public UView {
    friend class UScrollbar;
  public:
    static  UViewStyle style;  // renderer
    virtual UViewStyle* getViewStyle() {return &style;}
    
    UScrollbarView(UBox*, UView* parview, UHardwinImpl*);
    static UView* createView(UBox*, UView* parview, UHardwinImpl*);
    
    virtual void doUpdate(UUpdateContext&, URect r, URect clip, UViewUpdate&);
  };
}
#endif

