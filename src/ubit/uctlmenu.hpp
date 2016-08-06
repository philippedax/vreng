/************************************************************************
 *
 *  uctlmenu.hpp : Control menus (see upiemenu.hpp)
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

#ifndef _uctlmenu_hpp_
#define _uctlmenu_hpp_ 1
#include <ubit/upiemenu.hpp>
#include <ubit/ucall.hpp>
namespace ubit {
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Control menu.
   * a control menu makes it possible to select an operation then to control one
   * or two continous values in a single gesture. Control menus have to modes:
   *
   * - novice mode: the user presses the mouse and waits for getShowDelay() msec.
   *   The menu appears and the user selects one of the menu items by dragging the
   *   mouse. When the mouse reaches this item, it is selected and the menu 
   *   disappears. The user can then control one or two values (that correspond 
   *   to the horizontal and vertical directions) by dragging the mouse.
   *
   * - expert mode: the user performs the same gesture, but faster so that he
   *   does not wait for getShowDelay() msec. The same actions are performed
   *   except that the menu does not show up.
   *
   * @see: UPiemenu class for inherited methods.
   *
   * Event management:
   * - each menu item can have callbacks as explained in UPiemenu class
   * - a major difference with UPiemenus(s) is that items cannot be deselected
   *   one they have been selected by dragging the mouse inside their area
   * - callbacks are fired in the same way in the expert and novice mode. The only
   *   difference is that the menu does not appear in expert mode. In novice
   *   mode, the menu disappears when an item is selected.
   * - UCtlAction class and its subclasses can be used to make code simpler. 
   *   A UCtlAction object has a mdrag() method that is called when the mouse is 
   *   dragged as shown in the example below:
   *
   * <pre>
   *   class Demo {
   *   public:
   *       void scrollMore(UMouseEvent& e);
   *       ....
   *   };
   *  
   *   Demo* d = new Demo();
   *   UCtlmenu* cmenu = new UCtlmenu(); 
   *
   *   // scrollMore() is fired if item(0) has been selected and the mouse 
   *   // is being dragged (even if it is now outside of item(0))
   *
   *   cmenu->item(0)->add(" Scroll More " + UOn::mdrag / ucall(this, &Demo::scrollMore))
   *
   *   // here we use the predefined UScrollAction class (that derives from 
   *   // UCtlAction) to perform the same kind of behavior in a simpler way
   *
   *   UScrollAction* a = new UScrollAction(...);
   *   cmenu->item(4)->add(" Scroll Less " + *a)
   * </pre>
   */  
  class UCtlmenu : public UPiemenu {
  public:
    UCLASS(UCtlmenu)
    static UStyle* createStyle();

    UCtlmenu();
    ///< creates a new Control Menu; @see also shortcut function uctlmenu().
  };
  
  inline UCtlmenu& uctlmenu() {return *new UCtlmenu;}
  ///< shortcut function that creates a new control menu.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Callback object for UCtlmenu items (base class).
   * This class is intended to be subclassed by implementation classes that
   * redefine the mdrag() method.
   */
  class UCtlAction : public UCall {
  public:
    UCtlAction(float _gain) : xmag(_gain), ymag(_gain) {}
    virtual ~UCtlAction() {}
    
    void  setGain(float _gain) {xmag = ymag = _gain;}
    void  setGain(float _xgain, float _ygain) {xmag = _xgain, ymag = _ygain;}
    float getXGain() const {return xmag;}
    float getYGain() const {return ymag;}
    
  protected:
    virtual void mdrag(UMouseEvent& e, UCtlmenu&) = 0;
    /**< called when the mouse is dragged (must be redefined by subclasses).
     * This method is called repeatedly when one of the menu items has been
     * selected and the mouse is being dragged. Note that e.isFirstDrag() returns
     * true at the beginning of a drag sequence. @see: UCtlmenu::item() for details,
     */
    
    virtual void operator()(UEvent&);
    ///< [impl] calls mdrag() with appropriate arguments (no need to redefine this method).
    
    virtual void addingTo(UChild&, UElem& parent);
    ///< [impl] adds the UOn::mdrag event condition  (no need to redefine this method).
    
    float xmag, ymag;
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Centred zoom (Callback object for UCtlmenu items).
   */
  class UZoomAction : public UCtlAction {
  public:
    UZoomAction(UBox& zoomed_box, float gain = 0.5);
  protected:
    uptr<UBox> zbox;     // zoomed box
    uptr<UPos> posAttr; // box pos attribute (NB: its value is scale independent)
    uptr<UScale> scaleAttr; // box scale attribute
    float  zbox_scale0;    // initial scale of box 
    UPoint zbox_pos0;     // initial (scale indep) pos of box in its container
    UPoint mouse_in_zbox0;
    float mouse_delta;
    virtual void mdrag(UMouseEvent&, UCtlmenu&);
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Panning (Callback object for UCtlmenu items).
   */
  class UPanAction : public UCtlAction {
  public:
    UPanAction(UBox& panned_box, float gain = 1.);
  protected:
    uptr<UBox> box;     // panned box
    uptr<UPos> posAttr; // box pos attribute (NB: its value is scale independent)
    UPoint pos0;        // initial (scale indep) pos of box in its container
    //float container_scale;
    virtual void mdrag(UMouseEvent&, UCtlmenu&);
  };

  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Scrolling (Callback object for UCtlmenu items).
   */
  class UScrollAction : public UCtlAction {
  public:
    UScrollAction(UScrollpane&, float gain = 0.33);
    UScrollAction(float gain = 0.3);

    void setPane(UScrollpane&);
    UScrollpane* getPane() const {return pane;}
    
  protected:
    virtual void mdrag(UMouseEvent&, UCtlmenu&);
    UScrollpane* pane;
    float arm_xpos, arm_ypos, xpos, ypos;
  };
  
}
#endif
