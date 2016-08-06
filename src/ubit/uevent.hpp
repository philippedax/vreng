/************************************************************************
 *
 *  uevent.hpp
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

#ifndef uevent_hpp
#define	uevent_hpp  1
#include <ubit/ubox.hpp>
#include <ubit/ugeom.hpp>
#include <ubit/ukey.hpp>
namespace ubit {
  
  class UDataContext;
  
  /** Ubit Event class.
   * note that this class inherits from class UModifier that defines modifier masks
   * such as LeftButton, RightButton, etc. 
   */
  class UEvent {
  public:
    UEvent(const UCond& c, UObject* source, UObject* aux = null);
    
    virtual ~UEvent();
    
    const UCond& getCond() const {return *cond;}
    /**< returns the event condition that detected this event.
     * e.g.: returns 'UOn::change' if the expression was: UOn::change / ucall(...)
     */
    
    virtual UObject* getSource() const {return source;}
    ///< returns the object that received this event.
    
    virtual UObject* getAux() const {return aux;}
    ///< returns auxilliary data.
    
    /// dynamic cast: returns 'this' converted to the requested class or null.
    virtual UInputEvent*   toInputEvent()  {return null;}
    virtual UMouseEvent*   toMouseEvent()  {return null;}
    virtual UKeyEvent*     toKeyEvent()    {return null;}
    virtual UWheelEvent*   toWheelEvent()  {return null;}
    virtual UViewEvent*    toViewEvent()   {return null;}
    virtual UPaintEvent*   toPaintEvent()  {return null;}
    virtual UResizeEvent*  toResizeEvent() {return null;}  
    virtual UTimerEvent*   toTimerEvent()  {return null;}
    virtual UWinEvent*     toWinEvent()    {return null;}
    virtual UMessageEvent* toMessageEvent(){return null;}  
    virtual USysWMEvent*   toSysWMEvent()  {return null;}  
    virtual UUserEvent*    toUserEvent()   {return null;}
    
    void setCond(const UCond&);
    
  protected:
    const UCond* cond;  // event condition
    UObject* source;    // the object that produced this event
    UObject* aux;       // auxilliary data
  };
  

  // =============================================================================
  /** Base class for UMouseEvent and UKeyEvent
   * Note that this class inherits from class UModifier that defines modifier masks 
   * such as LeftButton, RightButton... 
   */
  class UInputEvent : public UEvent, public UModifier {     
  public:
    UInputEvent(const UCond&, UView* source_view, UEventFlow*, 
                unsigned long when, int state);

     virtual UInputEvent* toInputEvent() {return this;}
    
    virtual UElem* getSource() const;  
    ///< returns the object that received this event.
    
    UView* getView() const {return source_view;}
    /**< returns the object's view that received this event.
     * if getSource() derives from UBox, getView() returns the box's view where
     * this event occured. null is returned otherwise.
     */
    
    UView* getViewOf(const UBox& box) const;
    /**< returns the view of 'box' that (indirectly) received this event.
     * returns the view of 'box' that contains getView() if any (null otherwise)
     */
    
    unsigned long getWhen() const {return when;}
    ///< returns the time when this event was produced.
    
    virtual int getModifiers() const;
    /**< returns an ORed mask of mouse buttons and modifiers that are pressed.
     * Modifier masks are defined in class UModifier. They are inherited by UEvent,
     * and thus, UInputEvent. Exemple:
     * <pre>
     * void callbackFunc(UEvent& e) {
     *    if (e.getModifiers() & (e.LeftButton | e.ControlDown))
     *        // done when the left mouse button AND the control key are pressed
     * }
     * </pre>
     * alternatively, UEvent::LeftButton or UModifier::LeftButton can be used 
     * instead of e.LeftButton
     */
    
    bool isShiftDown() const;
    bool isControlDown() const;
    bool isMetaDown() const;
    bool isAltDown() const;
    bool isAltGraphDown() const;
    
    UWin* getWin() const;
    ///< returns the hard window that received this event.
    
    UView* getWinView() const;
    ///< returns the view of the hard window that received this event.
    
    UEventFlow* getFlow() const {return eflow;}
    /**< returns the Event Flow that received this event (for two-handed interaction or groupware).
     * a Ubit application can manage 1 or several Event Flows. This is useful 
     * for two-handed interaction or groupware (each user controlling his own 
     * pointer on the screen(s)). @see UAppli and UAppli::getFlow().
     */
    
    UDisp* getDisp() const;
    /**< returns the display that received this event.
     * A Ubit application can open windows on multiple displays.
     * The default display is the UAppli.
     * @see UAppli and UAppli::getDisp().
     */
    
    bool dontCloseMenu() const {return modes.DONT_CLOSE_MENU;}
    ///< impl.
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    friend class UElem;
    friend class UEventFlow;
    friend class UView;
    friend class UFlowView;
    friend class UViewFind;
    
    struct Modes {
      unsigned PROPAGATE:1;
      unsigned IS_MDRAG:1;
      unsigned IS_FIRST_MDRAG:1;
      unsigned IS_BROWSING:1;
      unsigned DONT_CLOSE_MENU:1;
      unsigned SOURCE_IN_MENU:1;
    } modes;
        
    int state;
    unsigned long when;
    UView* source_view;    // the view that contains the mouse
    UEventFlow* eflow;     // the event flow that produced this event
    UElem* event_observer;
#endif
  };
  
  // =============================================================================
  /** mouse events
   * @see UOn::mpress, UOn::mrelease, UOn::mdrag, UOn::mmove for explanations.
   */
  class UMouseEvent : public UInputEvent {
  public:
    UMouseEvent(const UCond&, UView* source, UEventFlow*, unsigned long time, int state,
                const UPoint& pos, const UPoint& abs_pos, int btn);
    
    virtual UMouseEvent* toMouseEvent() {return this;}
    
    float getX() const {return pos.x;}
    ///< returns the X coordinate of the mouse in getView().
    
    float getY() const {return pos.y;}
    ///< returns the Y coordinate of the mouse in getView().
    
    const UPoint& getPos() const {return pos;}
    ///< returns the coordinates of the mouse in getView().
    
    const UPoint& getScreenPos() const {return abs_pos;}
    ///< returns the screen coordinates of the mouse.
    
    UPoint getPosIn(const UView& view) const;
    ///< returns the coordinates of this event in this 'view'.
    
    UPoint getPosIn(const UBox& parent_box) const;
    /**< returns the coordinates of this event in parent_box.
     * returns point (0,0) if the mouse is not located inside 'parent_box'.
     */
    
    UPoint getPosAndViewIn(const UBox& parent, UView*& parent_view) const;
    /**< returns the coordinates of this event in 'parent' and the corresponding 'parent's view.
     * returns point (0,0) and 'parent_view' == null if the mouse is not located inside 'parent'.
     */
    
    int getClickCount() const {return click_count;}
    ///< returns the number of mouse clicks.
    
    int getButton() const {return button;}
    /**< returns the button that was just pressed or released.
     * returns one of: UEvent::LeftButton, MidButton, RightButton for UOn::mpress 
     * and UOn::mrelease events. Returns 0 for UOn::mmove or UOn::mdrag events.
     * Example:
     *
     * <pre>
     * class MyClass {
     * public:
     *    Myclass();  // contructor
     * private:
     *    void pressCB(UMouseEvent&);
     * };
     *
     * MyClass::MyClass() {
     *    ....
     *    ubutton("Press Me" + ucall(this, &MyClass::pressCB))
     *    ....
     * }
     *
     * void MyClass::pressCB(UMouseEvent& e) {
     *    if (e.getButton() == e.RightButton && e.isControlDown()) 
     *         // done when the left mouse button AND the control key are pressed
     * }
     * </pre>
     */
    
    int getButtons() const;
    /**< returns an ORed mask of mouse buttons *after* the event occured.
     * button mask are UEvent::LeftButton, MidButton, RightButton. They are defined
     * in class UModifier and inherited by UEvent.
     * see UInputEvent::getModifiers() for examples.
     */
    
    void translatePos(float x, float y);
    ///< translates the mouse position.
    
    void propagate() {modes.PROPAGATE = true;}
    ///< propagates events in children: @see UElem::catchEvents().
    
    void setFirstDrag(bool s)  {modes.IS_FIRST_MDRAG = s;}
    bool isFirstDrag() const   {return modes.IS_FIRST_MDRAG;}
    bool isBrowsing()  const   {return modes.IS_BROWSING;}
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    UStr* getStr();
    ///< returns the UStr object that is under the mouse (@see getData(UDataContext&)).
    
    UStr* getStr(UDataContext&);
    ///< returns the UStr object that is under the mouse and related properties (@see getData(UDataContext&)).
    
    UData* getData();
    ///< returns the UData object that is under the mouse (@see getData(UDataContext&)).
    
    UData* getData(UDataContext&);
    /**< returns the UData object that is under the mouse and related properties.
     * - returns null if no UData object was under the mouse when this event was generated
     * - the UDataContext argument is filled with useful info on the retreived object.
     * - use getStr() if you are only interested in UData objects that are strings.
     * @see also: UDataContext
     */
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    friend class UEventFlow;
    friend class UElem;
    friend class UView;
    friend class UFlowView;
    int button, click_count;
    UPoint pos, abs_pos;
#endif
  };
  
  // =============================================================================
  /** wheel events
   * @see UOn::wheel;
   */
  class UWheelEvent : public UMouseEvent {
  public:
    enum {WHEEL_DELTA = 120};
    
    UWheelEvent(const UCond&, UView* source, UEventFlow*,
                unsigned long time, int state, 
                const UPoint& pos, const UPoint& abs_pos,
                int wheel_btn, int wheel_delta);
    
    virtual UWheelEvent* toWheelEvent() {return this;}
    
    bool isVertical() const {return (button & 1) == 0;}
    
    int getWheelDelta() const {return delta;}
    /**< returns the amount of wheel movement.
     * - a positive value indicates that the wheel was rotated forward,
     *    away from the user;
     * - a negative value indicates that the wheel was rotated backward, 
     *    toward the user.
     * - One wheel click is defined as WHEEL_DELTA, which is 120.
     */
    
  private:
    int delta;
  };
  
  // =============================================================================
  /** keyboard events
   * @see UOn::kpress, UOn::krelease, UOn::ktype for explanations.
   */
  class UKeyEvent : public UInputEvent {
  public:
    UKeyEvent(const UCond&, UView* source, UEventFlow*, 
              unsigned long time, int state, 
              int keycode, short keychar);
    
    virtual UKeyEvent* toKeyEvent() {return this;}
    
    short getKeyChar() const {return keychar;}
    ///< returns the typed character (in ISO Latin).
    
    int getKeyCode() const {return keycode;}
    ///< returns the key code (@see UKey constants).
    
    virtual void setKeyChar(short keychar);
    /**< changes the typed character (in ISO Latin).
     * Note: this can be useful to hide actual text when typing a password etc.
     */
    
    virtual void setKeyCode(int keycode);
    ///< changes the key code (@see UKey constants).
    
  private:
    friend class UElem;
    int keycode;
    short keychar;
  };
  
  // =============================================================================
  /** view event: base class for UPaintEvent and UResizeEvent.
   */
  class UViewEvent : public UEvent {
  public:
    UViewEvent(const UCond&, UView* source_view);
    virtual UViewEvent* toViewEvent() {return this;}
    
    UBox* getSource() const;
    ///< returns the widget that received this event.
    
    UView* getView() const {return source_view;}
    ///< returns the view of getSource() that received this event.
    
    UDisp* getDisp() const;
    /**< returns the display that received this event.
     * A Ubit application can open windows on multiple displays.
     * The default display is the UAppli.
     * @see UAppli and UAppli::getDisp().
     */
    
  protected:
    UView* source_view;
  };
  
  // =============================================================================
  /** Paint event.
   * @see:
   * - condition UOn::paint that triggers callback functions when the view is
   *   repainted. These functions can have an optional UPaintEvent argument
   * - UGraph for drawing Ubit graphics in these callback functions
   * - UGraph::Glpaint drawing OpenGL graphics in these callback functions
   */
  class UPaintEvent : public UViewEvent {
  public:
    UPaintEvent(const UCond&, UView* source_view, const URect* win_clip = null);
    
    virtual UPaintEvent* toPaintEvent() {return this;}  

    bool isClipSet() {return is_clip_set;}
    ///< true if the clipping zone is vaild.
    
    void getClip(URect& r) const;
    /**< returns the drawing zone relatively to the origin of getView().
     * returns (0, 0, getView()->width, getView()->height) if isClipSet() is false.
     * and and empty rectangle if getView() is null
     */
    
    const URect getClip() const {URect r; getClip(r); return r;}
    ///< shortcut for getClip(URect& r).
    
#ifndef NO_DOC
    virtual bool setSourceAndProps(UView*);
    void setContext(UUpdateContext& c) {current_context = &c;}
    
  protected:
    friend class UBox;
    friend class UDisp;
    friend class UGraph;
    
    bool is_clip_set;   // is redraw_clip valid ?
    URect  redraw_clip;       // in window coordinates!
    UUpdateContext* current_context;
    UView* layout_view;
    
    virtual void setProps(const class UViewContext&);
#endif
  };
  
  // =============================================================================
  /** Resize event.
   * @see:
   * - condition UOn::resize that triggers callback functions with this argument
   */
  class UResizeEvent : public UViewEvent {
  public:
    UResizeEvent(const UCond&, UView* source_view);
    virtual UResizeEvent* toResizeEvent() {return this;}  
  };
    
  // =============================================================================
  
  class UWinEvent : public UEvent {
  public:
    UWinEvent(const UCond&, UView* source_view, int type, int state);
    
    virtual UWinEvent* toWinEvent() {return this;}
  
    UView* getView() const {return source_view;}    
    int getType()  const {return type;}
    int getState() const {return state;}
    
  private:
    UView* source_view;
    int type, state;
  };
  
  // =============================================================================
  /** Platform-dependent window manager events.
   */
  class USysWMEvent : public UEvent {
  public:
    USysWMEvent(const UCond&, UView* source_view, void* system_event);
    
    virtual USysWMEvent* toSysWMEvent() {return this;}
    
    UView* getView() const {return source_view;}
    void* getSysEvent() {return sys_event;}
    
  private:
    UView* source_view;
    void* sys_event;
  };
  
  // =============================================================================
  /** Ubit Message events.
   * @see UAppli::onMessage()
   */
  class UMessageEvent : public UEvent {
  public:
    UMessageEvent(const UCond&, UMessagePort* port);
    
    virtual UMessageEvent* toMessageEvent() {return this;}
    
    //virtual UObject* getSource() const {return (UObject*)port;}
    
    const UStr* getMessage() const;
    ///< returns the message string.
    
    UMessagePort* getMessagePort() const {return (UMessagePort*)source;}
    ///< returns the corresponding messagge port.
    
  private:
    friend class UMessagePortMap;
    //UMessagePort* port;
  };
  
  // =============================================================================
  /** timer event.
   * @see UTimer;
   */
  class UTimerEvent : public UEvent {
  public:
    UTimerEvent(const UCond&, UTimer* timer, unsigned long when);
    
    virtual UTimerEvent* toTimerEvent() {return this;}
    
    //virtual UObject* getSource() const {return (UObject*)timer;}
    UTimer* getTimer() const {return (UTimer*)source;}
    unsigned long getWhen() const {return when;}
    
  protected:
    //UTimer* timer;
    unsigned long when;
  };
  
  // =============================================================================
  /** User events.
   * this class is intented to be subclassed.
   */
  class UUserEvent : public UEvent {
  public:
    UUserEvent(const UCond&, UObject* source, int type, void* data);
    
    virtual UUserEvent* toUserEvent() {return this;}
    
    //virtual UObject* getSource() const {return source;}
    int   getType() const {return type;}
    void* getData() const {return data;}
    
  private:
    //UObject* source;
    int type;
    void* data;  
  };
  
  /* ==================================================== [(c)Elc] ======= */
  /** information on UData objects that are being retrieved 
   *  by UEvent::getData() or UEvent::getStr().
   */
  class UDataContext {
  public:
    UDataContext();
    ~UDataContext();
    
    URect getBounds() const;
    ///< return the location and the size of the data object relatively to e.getView().
    
    // - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   
# ifndef NO_DOC
  //private:
    friend class UBox;
    friend class UView;
    friend class UFlowView;
    friend class UEdit;
    friend class USelection;
     
    URect win_clip;
    // in the case of multine UStr objects, the region does not contain the entire
    // string but just the line that is beneath the mouse
    
    bool exact_match;
    // true if this data object is exactly located under the Mouse.
    // false if this data is the last data object before the Mouse position
    
    long strpos, strpos2;
    // strpos = position of the mouse in the string if data is an UStr.
    
    UChildIter it, it2;
    // points to the data object that is located under the mouse when calling 
    // UMouseEvent::getData() or UMouseEvent::getStr().
    
    UData* data;
    UView* source_view;
    UPoint win_eventpos;

    void set(UUpdateContext&, UData*, UChildIter data_iter, UChildIter end_iter, 
             const URect&, int _strpos, bool exact_match);
    void merge(UUpdateContext&, UData*, UChildIter data_iter, UChildIter end_iter, 
               const URect&, bool exact_match);    
# endif
  };
  
}
#endif

