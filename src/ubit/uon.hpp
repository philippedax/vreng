/************************************************************************
 *
 *  uon.hpp: Callback conditions
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

#ifndef _uon_hpp_
#define	_uon_hpp_ 1
#include <ubit/ucond.hpp>
namespace ubit {
  
  /** Callback conditions.
   * specify when callback functions are fired or when 'conditional' attributes are activated. 
   *
   * @see:
   * - class UCall for learning how to define callback functions and methods
   * - the respective documentation of UOn callback conditions (which are UOn class
   *   variables such as UOn::action, UOn::mpress...) 
   *
   * Note that:
   * - UOn conditions MUST NOT be deleted (this is done automatically by the toolkit)
   * - most UOn conditions only have an effect on widgets that derive from UBox 
   *   (some on them only on UWin subclasses) as explained in their documentation
   */
  class UOn : public UCond {
  public:    
    static UOn
    idle,
    /**< no event condition.
     * this condition is useful for UStyle specs.
     */
    
    action,
    /**< detects that a high-level "action" was performed on a widget.
     * the semantics of this "action" depends on the widget (e.g. a mouse click or
     * a SPACE key press for a button, an ENTER key press for a textfield).
     *
     * Callback functions triggered by UOn::action can have an optional UEvent parameter.
     * @see also: UOn::arm, UOn::mpress.
     */
    
    &arm, &disarm,
    /**< detects that a widget was "armed" or "disarmed".
     * The semantics of 'arm' and 'disarm' depends on the widget. An individual button
     * is armed when it is pressed then disarmed when it is released. It is also
     * disarmed/armed if the mouse moves outside/inside the button while being pressed.
     * 
     * This behavior is slightly different in lists and menus: an item is armed when
     * the mouse enters even if the initial mouse press was performed on another item.
     *
     * Any widget deriving from UBox can be made "armable" by UElem::setArmable(). 
     * Most interactors (e.g. UButton, UCheckbox...) are armable by default.
     *
     * Callback functions triggered by UOn::arm and UOn::disarm can have an optional 
     * UEvent parameter. @see also UOn::action, UOn::mpress.
     */
    
    &select, &deselect,
    /**< detects that a widget was "selected" or "deselected".
     * Certain widgets, such as UCheckbox or UListbox items can be selected. Besides,   
     * any widget deriving from UBox can be made "selectable" by UElem::setSelectable().
     *
     * Callback functions triggered by UOn::select and UOn::deselect can have an optional
     * UEvent parameter. @see also UOn::action.
     */
    
    enter, leave,
    /**< detects that the mouse entered or left a widget.
     * These conditions are applicable to any widget that derives from UBox.
     *
     * Callback functions triggered by UOn::enter and UOn::leave can have an optional
     * UMouseEvent parameter.
     */
    
    click, doubleClick,
    /**< detects that the mouse was clicked or double-clicked on a widget.
     * These conditions are applicable to any widget that derives from UBox.
     *
     * Callback functions triggered by UOn::click and UOn::doubleClick can have an
     * optional UMouseEvent parameter which has a getClickCount() method.
     *   
     * The radius and the delay for detecting mouse clicks depend on 
     * UConf::click_radius and UConf::click_delay.
     *
     * @see also UOn::action, UOn::mpress.
     */
    
    mpress, mrelease, mdrag, mmove, 
    /**< detects that the mouse was pressed, released, dragged or moved.
     * These conditions are applicable to any widget that derives from UBox.
     *
     * Callback functions triggered by these conditions can have an optional UMouseEvent
     * parameter which have several useful methods such as getSource(), getX(), getY()
     * that return the corresponding widget and the position of the mouse relatively
     * from its origin.
     *
     * It is worth noticing that drag and release events are always related to the 
     * widget where the mouse press occured, even if the mouse is now located on
     * another widget (this is not the case for move events as no widget was pressed).
     *
     * Notes:
     * - press and release events are repeated if the widget is in auto repeat mode
     *   see: UElem::setAutoRepeat()
     * - event are produced in the following order (when applicable): 
     *   mpress / arm / action / click / disarm / mrelease.
     */
    
    wheel,
    /**< detects that the wheel was moved while the mouse was over a widget.
     * This condition is applicable to any widget that derives from UBox. The widget
     * that receive the wheel events is the one where the mouse is currenly located. 
     *
     * Callback functions triggered by UOn::wheel can have an optional UWheelEvent 
     * parameter which has getWheelDelta() method that returns the amount of wheel movement
     */
    
    kpress, krelease,
    /**< fire callbacks when a key is pressed or released.
     * these are low-level conditions that do not compose characters. For instance,
     * typing SHIFT 'A' produces 2 kpress and 2 krelease events. UOn::ktype, that 
     * composes characters, should be used in most cases. Callbacks can have an
     * optional UKeyEvent parameter. Their getKeyCode() method returns a code that 
     * identifies the key.
     *
     * The box that gets these events is the one that has the input focus (which is
     * the box where the mouse is currenly located in the current version of the toolkit)
     * Note that mouse and key these conditions only apply to boxes (ie. objects 
     * that derive from UBox)
     * @see: UOn::ktype
     */
    
    ktype,
    /**< fire callbacks when a text is entered by pressing a key.
     * in constrast with kpress, ktype composes characters and it is only fired
     * when printable characters are typed. For instance, typing SHIFT 'A'
     * produce only one ktype event. Callbacks can have an optional UKeyEvent
     * parameter. Their getChar() method returns the composed character (for instance,
     * if SHIFT was pressed, it will return 'uppercase A' rather than 'lowercase A')
     *
     * The box that gets these events is the one that has the input focus (which is
     * the box where the mouse is currenly located in the current version of the toolkit)
     * Note that mouse and key these conditions only apply to boxes (ie. objects 
     * that derive from UBox)
     * @see: UOn::kpress, UOn::krelease
     */
    
    &dragStart, &dragDone, &dropEnter, &dropLeave, &dropDone,
    /**< detects drag and drop events occuring on a widget.
     */
    
    show, hide,
    /**< detect that a widget was shown or hidden.
     * these callbacks are *ONLY* fired when show(true) or show(false) is applied 
     * to a widget (currently, they are not fired if a parent was shown or hidden)
     */
    
    close,
    ///< fires callbacks when a window (or a menu) is closed by the window manager (@see UWin::close()).
    
    paint, 
    /**< detect that a widget was repainted.
     * This condition is applicable to any widget that derives from UBox.
     * 
     * Callback functions triggered by UOn::paint can have an optional UPaintEvent
     * parameter which have several useful methods such as getSource(), getView(),
     * getClip(), etc. @see UPaintEvent for details.
     *
     * A UBox can have multiple views. Callback functions are then fired for each view
     * that has been repainted.   
     */
    
    resize, motion,
    /**< detect that a widget was resized or moved.
     * These conditions are applicable to any widget that derives from UBox.
     * 
     * Callback functions triggered by these conditions can have an optional UEvent
     * parameter 
     *
     * A UBox can have multiple views. Callback functions are then fired for each view
     * that has been resized or moved.
     */
    
    addChild, removeChild,
    /**< fire callbacks when a child is added to / removed from an object.
     * objects must derive from UElem. Callbacks can have an optional UEvent parameter. 
     * Its getTarget() method returns the child that was added or removed
     * (beware: this child may have been destroyed in the latter case!)
     */
    
    destruct, 
    /**< detects when an object is being destructed.
     */
    
    winState,
    /**< fires callbacks when a window state is changed.
     * this condition only applies for windows. It is fired by map, unmap and window state events.
     */
    
    sysWM,
    /**< fires callbacks when a platform-dependent window manager event is received.
     * this condition only apply for windows. When X11 is used, it will be fired when X11
     * properties are changed and, non-Ubit client messages are received (see UOn::message).
     */
    
    change,
    /**< fires callbacks when the value of an object is changed.
     * this condition detects when:
     * - the value of UScrollbar, USlider objects is changed
     * - the selection is changed in UChoice, UListbox, UCombobox objects
     *   (= when a item, that was previously unselected, becomes selected)
     *
     * Notes for UChoice, UListbox, UCombobox:
     *  - UEvent::getTarget() returns the item that was selected (while
     *    UEvent::getSource() returns the UChoice, UListbox... container)
     *  - UOn::change is not triggered if the user selects an item that was
     *    already selected (in contrast with UOn::action which would be
     *    fired in this case)
     */
    
    propChange, dataChange, strChange, //caretChange,
    /**< child change conditions.
     * these conditions detects when a child of a box is changed:
     * - propChange detects if a UAttr child was changed
     * - dataChange ... if a UData ..
     * - strChange ... if a UStr ..
     * Note that UOn::change is triggered on the child before propChange,
     * dataChange or strChange are triggered on the parents.
     * 'dataChange' then 'strChange' are triggered when UStr(ings) 
     * are changed.
     */
    
    userEvent;
    ///< user event.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    enum Callback {  /// callbacks this object is sensitive to
      DESTRUCT_CB      = 1<<12,
      CONTAINER_CB     = 1<<13,
      CHANGE_CB        = 1<<14,
      CHILD_CHANGE_CB  = 1<<15,
      VIEW_PAINT_CB    = 1<<16,
      VIEW_CHANGE_CB   = 1<<17,
      SHOW_HIDE_CB     = 1<<18,
      MOUSE_CB         = 1<<19,
      MOUSE_CROSS_CB   = 1<<20,
      MOUSE_DRAG_CB    = 1<<21,
      MOUSE_MOVE_CB    = 1<<22,
      KEY_CB           = 1<<23,
      DND_CB           = 1<<24,
      MESSAGE_CB       = 1<<25,
      WIN_STATE_CB     = 1<<27,
      SYSWM_CB         = 1<<28,
      USER_EVENT_CB    = 1<<29,
      
      CALLBACKS =  DESTRUCT_CB | CONTAINER_CB | CHANGE_CB | CHILD_CHANGE_CB
      | VIEW_PAINT_CB | VIEW_CHANGE_CB | SHOW_HIDE_CB
      | MOUSE_CB | MOUSE_CROSS_CB | MOUSE_DRAG_CB | MOUSE_MOVE_CB | KEY_CB | DND_CB 
      | MESSAGE_CB | WIN_STATE_CB | SYSWM_CB 
      /*| FILTER_EVENT_CB*/ | USER_EVENT_CB //!no comma after last item!
    };
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // interaction state
    
    enum IState {
      ANY = -1,
      IDLE = 0,			// must be 0 (index in style arrays)
      DISABLED, ENTERED, ARMED, ACTIONED, DRAGGED, DROP_ENTERED,
      ACTION_COUNT
    };
  
    const int ID;
    
    UOn(int id, long callback_mask, int istate = -1);  // -1 == UMode::ANY
    /**< creates a callback condition.
     * callback conditions have a unique ID. IDs in the range [0-100] are reserved for
     * standard event conditions. IDs > 100 can be used for custom client conditions.
     * Notes:
     * - UOn instances MUST NOT be deleted (this is done automatically by the toolkit)
     * - in the current version of the toolkit, mouse and key conditions are ignored
     *   by objects that do not derive from UBox
     */
    
    virtual int getID() const {return ID;}
    
    // see superclass for details.
    virtual bool operator==(const UCond&) const;
    virtual bool operator!=(const UCond&) const;
    virtual const UCond* matches(const UCond&) const;
    virtual bool verifies(const UUpdateContext&, const UElem&) const;
    
    virtual UOn* toOn() {return this;}
    virtual const UOn* toOn() const {return this;}
    
  protected:
    friend class UElem;
    long callback_mask;
    int istate;
    virtual void setParentModes(UElem& parent) const;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  class UOn2 : public UOn {
  public:
    typedef UElem& (UElem::*SetModeFunc)(bool);
    
    UOn2(int id, long callback_mask, int state, SetModeFunc);
    
  protected:
    virtual void setParentModes(UElem& parent) const;
    SetModeFunc set_mode_func;
  };
  
}
#endif

