/************************************************************************
 *
 *  uelem.hpp: Element Nodes (lightweight containers that can have children)
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

#ifndef _uelem_hpp_
#define	_uelem_hpp_ 1
#include <string.h>		// memset
#include <ubit/uattr.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uargs.hpp>
#include <ubit/ustr.hpp>
namespace ubit {
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** lightweight general purpose container.
   *
   * This class is a very lightweight and general container. In contrast with UBox 
   * (and its subclasses) UElem objects do not control UViews and
   * do not manage screen rendering directly (they are just genuine containers)
   *
   * IMPORTANT NOTE ON RECURSIVE DESTRUCTION AND SMART POINTERS:
   * When a UElem is destroyed, its direct and indirect children are ALSO destroyed
   * EXCEPT if they have other parents OR if they are pointed by a uptr<> Smart Pointer.
   * @see class UNode for details.
   *
   * Notes:
   * - see UBox for examples showing how to add and manage children and attributes
   * - objects can have several parents and they can be added several times
   *   to the same parent.
   * - many methods are inherited from superclasses.
   */
  class UElem: public UNode {
    UElem& operator=(const UElem&);
    ///< assigment is forbidden (and meaningless for elements and subclasses).
    
    explicit UElem(const UElem&);
    
  public:
    //UCLASS("#element", UElem, new UElem)
    UCLASS(UElem)
    
    UElem(UArgs node_arglist = UArgs::none);
    /**< creates a new UElem; 
     * The argument can either be a single object pointer or reference, or a list 
     * of object pointers or references separated by + operators, example:
     * <pre>
     *   UButton* b = new UButton(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     *
     * @see also the UElem() shortcut, that returns: *new UElem(arglist)
     */
    
    virtual ~UElem();
    ///< destructor, note that children are recursively destroyed except if pointed elsewhere (@see class UNode).
    
    virtual void destructs();
    
    virtual UElem* toElem() {return this;}
    virtual const UElem* toElem() const {return this;}
    
    static UStyle* createStyle();
    /**< static function that returns the style of this class.
     * this function MUST be redefined by subclasses if they use another UStyle.
     */  
    
    const UStyle& getStyle(UUpdateContext*) const;
    /**< virtual function that returns the style of this object.
     * this function calls createStyle() the first time it is called, then it always
     * return the same UStyle object.
     * In contrast with createStyle(), which that must be redefined for each subclass,
     * there is no need to redefine getStyle().
     */
    
    virtual int getNodeType() const {return ELEMENT_NODE;}
    ///< returns the XML node type.
    
    virtual const UStr& getNodeName() const {return getClass().getName();}
    ///< return the XML node name (which is the class name).
    
    virtual int getDisplayType() const {return INLINE;}
    
    virtual void initNode(UDoc* context) {}
    ///< initialises the XML context of this node.
    
    
    // - - - - - - - - - - - - - - - - - - - - - - - - 
    // attributes.
    
    UElem& setAttr(UNode& attribute);
    /**< adds or replaces this attribute in the ATTRIBUTE list.
     * NOTE that the previous attribute object is destroyed if it was created by 'new'
     * and is not referenced elsewhere by a widget or a UPtr smart pointer (@see UNode)
     */
    
    UElem& addAttr(const UArgs& attributes);
    /**< adds one or several attributes (deriving from UAttr) to the ATTRIBUTE list.
     * @see add(const UArgs&) for details (both functions work in the same way).
     */
    
    UElem& removeAttr(UNode& attribute, bool auto_delete = true);
    /**< removes/deletes an attribute (and its descendants) from the ATTRIBUTE list.
     * @see: remove(UNode&, bool) for details (both functions work in the same way).
     * NOTE that the removed attribute object is destroyed if it was created by 'new'
     * and is not referenced elsewhere by a widget or a UPtr smart pointer (@see UNode)
     */  
    
    UElem& removeAllAttrs(bool auto_delete = true);
    /**< removes/deletes all attributes (and their descendants) in the ATTRIBUTE list. 
     * @see: remove(UNode&, bool) for details (both functions work in the same way).
     * NOTE that the removed attributes are destroyed if they were created by 'new'
     * and are not referenced elsewhere by a widget or a UPtr smart pointer (@see UNode)
     */
    
    bool getAttrValue(UStr& value, const UStr& attr_name) const;
    /**< retrieves the value of the attribute which class name is 'attr_name'.
     * false is returned if there is no such attribute in the  element's ATTRIBUTE list.
     * true is returned otherwise and the attribute value is stored in the 'value' argument.
     */
    
    UAttr* getAttr(const UStr& attr_name) const;
    /**< returns the attribute node which class name is 'attr_name'.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     * example: UColor* col = (UColor*) element.getAttr("UColor");
     */
    
    template <class ATTR> 
    ATTR* getAttr() const {ATTR* a; _attributes.findClass(a); return a;}
    /**< retrieves the attribute node that derives from the "ATTR" C++ class.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     * example: UColor* col = element.getAttr<UColor>();
     */
    
    template <class ATTR> 
    ATTR& obtainAttr()  
    {ATTR* a; _attributes.findClass(a); if (!a) addAttr(a = new ATTR); return *a;}
    /**< retrieves or creates the element's attribute that derives from the "ATTR" C++ class.
     * if there is no such attribute in the element's ATTRIBUTE list, it is created
     * and added to the ATTRIBUTE list.
     * example: UColor& col = element.getAttr<UColor>();
     */
        
    // - - - - - - - - - - - - - - - - - - - - - - - - - - 
    // children.
    
    virtual bool isParentOf(const UNode& possible_child) const;
    ///< returns true if this object a direct or indirect parent of 'child'.
    
    UElem& add(const UArgs& children);
    /**< adds one or several objects to the end of the CHILD list of this element.
     * The argument can either be a single object pointer or reference, or a list 
     * of object pointers or references separated by + operators:
     * <pre>
     *   UButton* b = new UButton();    // creates a UButton (that derives from UElem)
     *   b->add(UPix::disquette);       // adds a pixmap image to the button
     *   b->add("Save");                // adds a string that serves as a label
     *   b->add(ucall(obj, saveFunc));  // adds a callback function (@see UCall)
     * </pre>
     * is equivalent to:
     * <pre>
     *   UButton* b = new UButton();
     *   b->add(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     * and also to:
     * <pre>
     *   UButton* b = new UButton(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     *
     * Objects added to the CHILD list should derive from UElem (typically, widgets
     * deriving from UBox, UWin), UData (viewable objects such as UStr, UIma) or UCall
     * (callback objects). Certain attributes such as UColor, UFont, UHalign, UValign
     * can also be added to the child list.
     *
     * Character strings (e.g. "abcd") are implicitely converted to UStr objects, so 
     * that add("abcd") is equivalent to: add( ustr("abcd") ). 
     *
     * For syntactical reasons, an argument list cannot solely contain pointers or strings
     * separated by + operators: one argument must then be dereferenced or appropriately
     * converted (exple: add(ustr("abcd") + "xyz") is correct, add("abcd" + "xyz") is not)
     *
     * The addAttr() method works as add() excepts that it adds arguments to the 
     * ATTRBITUTE list. Arguments should then derive from UAttr (attributes) or 
     * UCall (callback aobjects).
     *
     * Subclasses that require a specific behavior should not redefine this function
     * but addImpl(), that is called internally by all add functions
     *
     * @see also: add(const UArgs&, int pos), add(const UArgs&, UChildIter pos), addAttr(const UArgs&)
     */
    
    UElem& add(const UArgs& children, int position);
    /**< adds one or several objects in the CHILD list before this position.
     * @see add(const UArgs& objects) for details: both functions work in the same way 
     * except that this function makes it possible to specify the position instead of
     * adding to the end of the child list.
     * 0 refers to the position before the first argument in the child list, special
     * value -1 refers to the position after the last child.
     */
    
    UElem& add(const UArgs& children, UChildIter iterator);
    /**< adds one or several objects in the CHILD list before this iterator.
     * @see add(const UArgs& objects) for details: both functions work in the same way 
     * except that this function makes it possible to specify the position (through
     * an iterator, @see UChildIter) instead of adding to the end of the child list.
     */
    
    virtual UElem& addImpl(const UArgs& nodes, UChildIter pos, UChildren& in_list);
    virtual bool addImpl1(const UChild& node, UChildIter pos, UChildren& in_list);
    
    // - - - - 
    
    UElem& remove(UNode& child, bool auto_delete = true);
    /**< removes/deletes a child (and its descendants) from the CHILD list.
     * This function removes the first occurence of 'child' from the child list.   
     * Moreover, 'child' and its children are DESTROYED if 'auto_delete' is true, 
     * they were created by 'new' and they are not referenced elsewhere by a widget 
     * or a UPtr smart pointer (@see UNode).
     * 
     * The same child can appear SEVERAL times in the child list, in which case it
     * is never deleted (the first occurence is just removed from the list)
     *
     * Subclasses that require a specific behavior should not redefine this function
     * but removeImpl(), that is called internally by all remove functions
     */
    
    UElem& remove(int pos, bool auto_delete = true);
    /**< removes/deletes a child (and its descendants) at this position in the CHILD list.
     * 'pos' must be a valid position (positions start from 0, -1 indicates the last child)
     * @see: remove(UNode&, bool) for important details on removal vs. deletion.
     */
    
    UElem& remove(UChildIter pos, bool auto_delete = true);
    /**< removes/deletes a child (and its descendants) at this position in the CHILD list.
     * 'pos' must be an iterator pointing to the child that must be removed or deleted.
     * @see: remove(UNode&, bool).
     */
    
    UElem& removeAll(bool auto_delete = true);
    /**< removes/deletes all children (and their descendants) in the CHILD list. 
     * @see: remove(UNode&, bool).
     */
    
    virtual UElem& removeImpl(UChildIter begin, int N, bool autodel, UChildren& in_list);
    virtual bool removeImpl1(UChildIter pos, int auto_delete, UChildren& in_list);  
    
    // - - - - 
    
    UChildren& getChildren() const {return children();}
    /* return the child list.
     * @see: child(), getChild(), cbegin(), cend(), crbegin(), crend(), attributes().
     */
    
    int getChildCount() const {return children().size();}
    /* return the number of children.
     * @see: child(), getChild(), cbegin(), cend(), crbegin(), crend(), attributes().
     */
    
    UChildIter child(int pos) const {return children().at(pos);}
    /** returns an iterator pointing to the child at this position.
     * returns the last child if 'pos' = -1 / returns cend() if 'pos' is out of bounds 
     * @see also: UChildren find() functions
     */
    
    UNode* getChild(int pos) const;  
    /** returns the child at this position.
     * returns the last child if 'pos' = -1 / returns null if 'pos' is out of bounds 
     * @see also: UChildren find() functions
     */
    
    UChildIter cbegin() const {return children().begin();}
    /**< returns a forward iterator pointing to the beginning of the CHILD list.
     * UChildIter is a child iterator. It is compatible with standard STL algorithms. 
     * This example prints the children class names:
     * <pre>
     *    UElem* obj = ...;
     *    for (UChildIter i = obj->cbegin(); i != obj->cend(); ++i)
     *        cout << (*i)->getClassName() << endl;
     * </pre>
     * @see also: children(), cend(), crbegin(), crend(), abegin(), aend(), pbegin(), pend().
     */
    
    UChildIter cend() const {return children().end();}
    ///< returns a forward iterator pointing to the end of the child list (@see cbegin()).
    
    UChildReverseIter crbegin() const {return children().rbegin();}
    ///< returns a reverse iterator pointing to the beginning of the child list (@see cbegin()).
    
    UChildReverseIter crend() const {return children().rend();}
    ///< returns a reverse iterator pionting to the end of the child list (@see cbegin()).
    
    // - - - text - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual UStr retrieveText(bool recursive = true) const;
    /**< collates and returns the text that is enclosed in this object.
     *  'recursive' means that the text included in descendants is also collated.
     */
    
    virtual void retrieveText(UStr& string, bool recursive = true) const;
    /**< collates and returns the text that is enclosed in this object.
     *  'recursive' means that the text included in descendants is also collated.
     */
    
    // - - - graphics and layout - - - - - - - - - - - - - - - - - -
    
    virtual bool isShown() const;
    /**< returns true if this object is shown.
     * isShown() returns true if this object is "showable" (see isShowable())
     * and all its parents are also "showable".
     * Note that "shown" objects are not always visible on the screen as they
     * may be hidden or clipped by other objects such as scroll panes.
     * @see: isShowable(), UElem::show()
     */
    
    bool isShowable() const {return emodes.IS_SHOWABLE;}
    /**< returns true if this object can be shown.
     * isShowable() == true means that this object will be shown
     * if its parents can also be shown.
     * Note: this function has the same meaning as isVisible() in Java.
     * @eee: isShown(), UElem::show()
     */
    
    void setShowable(bool s) const {emodes.IS_SHOWABLE = s;}
    ///< synonym for show(bool).
    
    virtual void show(bool = true);
    /**< shows/hides this object.
     * @see also: isShown(), isShowable(), update()
     */
    
    static void closeWin(UInputEvent&, int status);
    /**< closes the first window (UDialog, UMenu...) that contains this element.
     * @see: UWin::close() and ucloseWin().
     */
    
    void repaint() {update(UUpdate::paint);}
    ///< indicates that this object will be repainted (when the main loop becomes idle).
    
    void update() {update(UUpdate::layoutAndPaint);}
    ///< indicates that this object will be layed out and repainted (when the main loop becomes idle).
    
    virtual void update(const UUpdate& update_options, UDisp* = null);
    /**< indicates that the layout and/or the paint of this object will be updated.
     * this will be done when the main loop becomes idle.
     * - the first argument specify what must be updated and misc options (see UUpdate)
     * - the second argument is only taken into account by redefinitions of this function
     * @see also: repaint(), update(), show().
     */
    
    void doUpdate() {doUpdate(UUpdate::layoutAndPaint, null);}
    
    virtual void doUpdate(const UUpdate&, UDisp* = null);
    /**< updates the layout and/or the paint of this object right now.
     * this function is called when the main loop becomes idle for repainting the object.
     * Usually, a client program should NOT call doUpdate() directly. Instead, it
     * should call update() or update(const UUpdate&, UDisp*) that delay layout and
     * repaint actions until the main loop becomes idle. This improves performance
     * and makes double buffering possible.
     */
    
    // - - - events - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    UElem& ignoreEvents(bool state = true);
    /**< if arg is true, this element and (its children) ignore events.
     * events are then received by the parent(s) of this element
     */
    
    UElem& catchEvents(const UChild& condition_callback_expr);
    /**< catches certain events before they reach the object's children.
     * Events are not delivered to their normal targets, instead, they are catched
     * by this element if it contains the target. The specified callback is then fired 
     * with this element as a "source" (@see UEvent::getSource()). If this callback
     * calls the event's propagate() method, this event will then be sent to the
     * normal target. This makes it possible to filter events and decide which of them
     * can reach targets. Also, some some useful functions can be triggered before
     * events reach their final targets and the events can be modified by settings UFlags
     * or by modifiying the event coordinates. 
     *
     * 3 categories of events can be currently catched:
     * - UOn::mpress, UOn::mrelease & UOn::mdrag mouse events. As, these events are 
     *   closely related, catching one of them will also catch the other 2 kinds.
     * - UOn::mmove mouse events.
     * - UOn::wheel events.
     *
     * 'condition_callback_expr' is a construct such as: 
     * <pre>   UOn::mpress / ucall(obj, &Obj::foo)   // (obj type is Obj*)  
     * </pre>
     * The callback method (eg. 'foo') will be fired BEFORE this condition (eg. 'UOn::mpress')
     * occurs in a direct and indirect child of this element. 
     *
     * To learn more about conditions @see classes UOn and UCond. To learn more about
     * callback objects @see class UCall. 
     */  
    
    UElem& observeChildrenEvents(const UChild& condition_callback_expr);
    /**< observe events that occur in children.
     * 'condition_callback_expr' is a construct such as: 
     * <pre>   UOn::action / ucall(obj, &Obj::foo)   // (obj type is Obj*)  
     * </pre>
     * The callback method (eg. 'foo') will be fired AFTER this condition (eg. 'UOn::action')
     * occurs in a direct and indirect child of this element. 
     *
     * To learn more about conditions @see classes UOn and UCond. To learn more about
     * callback objects @see class UCall. 
     */
    
    bool isIgnoringEvents() const {return emodes.IGNORE_EVENTS;}
    ///< returns true if events are ignored by this object; 
    
    bool hasCallback(long callback_id) const;
    ///< true if thie object has such a callback.
    
    virtual bool fire(UEvent&) const;
    ///< fires callback functions that match this event.  
    
    // - - - states - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    UElem& enable(bool state = true) {return setEnabled(state);} 
    ///< shortcut for setEnabled(bool state = true).
    
    virtual UElem& setEnabled(bool state = true, bool call_callbacks = true);
    /**< specifies whether this object is enabled.
     * - enabled means: sensible to high-level events such as UOn::action, UOn::arm, UOn::disarm...
     * - calls related callbacks if 'call_callbacks' is true
     *  @see: isEnabled(), enable(), disable().
     */
    
    bool isEnabled() const;
    ///< returns true if this object is currently enabled; @see setEnabled().
    
    // - - - - -
    
    virtual UElem& setSelected(bool state = true, bool call_callbacks = true);
    /**< specifies whether this object is selected.
     * fires related callbacks if 'call_callbacks' is true
     */
    
    bool isSelected() const {return emodes.IS_SELECTED;}  
    
    bool isSelectable() const {return emodes.IS_SELECTABLE;}
    UElem& setSelectable(bool state = true);
    
    // - - - - -
    
    bool isArmed() const;
    
    bool isArmable() const {return emodes.IS_ARMABLE;}
    /**< specifies whether this object can be armed.
     * an armable widget behaves as a button (but its appearance depends on its UStyle).
     * A widget can be made armable by calling its setArmable() methode
     */
    
    UElem& setArmable(bool state = true);
    
    // - - - - -
    
    UElem& setAutoRepeat(bool state = true);
    bool isAutoRepeat() const {return emodes.IS_AUTO_REPEAT;}
    
    bool isCrossable() const {return emodes.IS_CROSSABLE;}
    UElem& setCrossable(bool state = true);
    
    UElem& setBrowsable(bool = true);
    bool isBrowsable() const {return emodes.IS_BROWSABLE;}
    
    bool isDragged() const;
    bool isDraggable() const {return emodes.IS_DRAGGABLE;}
    UElem& setDraggable(bool = true);
    
    bool isDroppable() const {return emodes.IS_DROPPABLE;}
    UElem& setDroppable(bool = true);
    
    // - - - - -
    
    bool isTextEditable() const {return emodes.IS_TEXT_EDITABLE;}
    bool isFloating() const {return emodes.IS_FLOATING;}
    bool isVertical() const {return emodes.IS_VERTICAL;}    // && HAS_ORIENT ?  !!!!
    bool isWidthResizable()  const {return !emodes.IS_WIDTH_UNRESIZABLE;}
    bool isHeightResizable() const {return !emodes.IS_HEIGHT_UNRESIZABLE;}
    virtual bool isSubWin() const {return false;}  // redefined by USunWin => MUST be virtual!
    
    void disableMenuClosing(bool s = true) {emodes.DONT_CLOSE_MENUS = s;}
    ///< this object and its children wont close menus when clicked if argument is true.
    
    bool isMenuClosingDisabled() const {return emodes.DONT_CLOSE_MENUS;}
    ///< true if this object and its children do not close menus when clicked.
    
    // - - - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    
    struct Modes {
      unsigned IS_SHOWABLE:1;      ///< can be shown if parents are shown (see isShowable() and isShown())
      unsigned IS_BROWSABLE:1;     ///< can browse (as for lists and menus)
      unsigned IS_CROSSABLE:1;     ///< detects (and shows) UOn::enter and UOn::leave conditions
      unsigned IS_ARMABLE:1;       ///< can be armed 
      unsigned IS_AUTO_REPEAT:1;   ///< auto-repeats UOn::arm while armed
      unsigned IS_DRAGGABLE:1;     ///< can be dragged (with button 1 or button2)
      unsigned IS_DROPPABLE:1;     ///< can be dropped
      unsigned IS_SELECTABLE:1;    ///< can be selected (as for a checkbox)
      unsigned IS_SELECTED:1;      ///< this object is currently selected.
      unsigned IS_TEXT_SELECTABLE:1;///< can select included text
      unsigned IS_TEXT_EDITABLE:1; ///< can edit included text (ie. the group contains a UEdit brick)
      unsigned IS_WIDTH_UNRESIZABLE:1;
      unsigned IS_HEIGHT_UNRESIZABLE:1;
      unsigned IS_FLOATING:1;       ///< has a UPos attr in floating mode.
      unsigned IS_VERTICAL:1;       ///< has a vertical orientation (see HAS_ORIENT).
      unsigned HAS_ORIENT:1;        // A REVOIR @@@@ !!!!.
      unsigned HAS_CURSOR:1;        ///< has a cursor attr.
      unsigned HAS_LAYOUT:1;        ///< has a layout attr.
      unsigned HAS_TIP:1;           ///< has a tool tip.
      unsigned DONT_CLOSE_MENUS:1;
      unsigned OBSERVE_EVENTS:1;
      unsigned IGNORE_EVENTS:1;    ///< this object ignores events.
      enum {
        CATCH_MOUSE_MASK = 1<<0, CATCH_MOUSE_MOVE_MASK = 1<<1, 
        CATCH_WHEEL_MASK = 1<<2, CATCH_KEY_MASK = 1<<3
      };
      unsigned CATCH_EVENTS:4;  ///< this object can catch mouse events.
    };
    
    enum {REMOVE_FROM_PARENTS = -1};
    
    bool isBrowsingGroup() {return emodes.IS_BROWSABLE;}       // !!!
    //virtual UElem* getBrowsingGroup() {return emodes.IS_BROWSABLE ? this : null;} 
    
    //UElem& setConst() {UObject::setConst(); return *this;} // redefined from UObject.
    
    virtual UChildren& children() const {return _children;}
    /*[impl] return the child list.
     * @see: child(), getChild(), cbegin(), cend(), crbegin(), crend(), attributes().
     */
    
    virtual UChildren& attributes() const {return _attributes;}
    /*[impl] returns the attribute list.
     * @see: abegin(), aend(), children().
     */
    
    UChildIter abegin() const {return attributes().begin();}
    /*[impl] returns an iterator to the beginning the ATTRIBUTE list.
     * works in the same was as cbegin() cend() for the CHILD list.
     * @see: aend(), attributes()
     */
    
    UChildIter aend() const {return attributes().end();}
    //[impl] returns an iterator to the end of the attribute list (@see abegin()).
    
    UObject::State getInterState() const {return ostate;}
    //[impl] returns the internal interactive state.
    
    void setInterState(UObject::State);
    //[impl] changes the internal interactive state.
    
    virtual const UStr* getTextSeparator() const;
    ///< returns the text separator used by retrieveText() for separating enclosed children
    
    virtual void deleteViewsInside(const std::vector<UView*>& parent_views);
    
    virtual void initView(UView* parent_view);
    virtual void initChildViews(UElem*);
    virtual void highlight(bool state) {};
    
    virtual int retrieveRelatedViews(std::vector<UView*>& collating_vector) const;
    /* returns the views that are related to this element.
     * retrieves all the views where this element currently appears. This list is 
     * obtained  by collating the views of all the UBox parents of this element
     * (this may be indirect parents as this element may be a child of another element
     * that is a child of a UBox, and so on).
     *
     * - 'collating_vector' is not emptied: the views are added to the end of what
     *   it already contains.
     * - the function returns the number of views that were added (which may thus differ
     *   from the total size of 'collating_vector')
     *
     * Note that elements and boxes can have several parents, so that multiple wiews
     * can be associated to the same widget (see UElem and UBox classes for details)
     */
    
    UElem& _setArmableAndSelectable(bool state);
    
    /* [impl] behavioral functions.
     * these functions are called when the object receives the corresponding events.
     * Usually, these functions should not be redefined by clients. Instead, clients
     * should add UOn::xxx conditions that are called by these functions
     * @see class UOn.
     */
    
    virtual void keyPressBehavior(UKeyEvent&);
    virtual void keyReleaseBehavior(UKeyEvent&);
    virtual void enterBehavior(UInputEvent&, bool is_browing);
    virtual void leaveBehavior(UInputEvent&, bool is_browing);
    virtual void armBehavior(UInputEvent&, bool is_browing);
    virtual void disarmBehavior(UInputEvent&, bool is_browing);
    virtual void actionBehavior(UInputEvent&);
    
  protected:
    friend class UNode;
    friend class UEdit;
    friend class UChildEventFlag;  
    friend class UPos;
    friend class USize;
    friend class UOrient;
    friend class UCursor;
    friend class UTip;
    friend class UStr;
    friend class UWin;
    friend class UOn;
    friend class UEventFlow;
    friend class UViewStyle;
    friend class UUpdateContext;
    friend class UWinUpdateContext;
    //short istate; inherited from UObject
    mutable Modes emodes;
    long callback_mask;
    mutable UChildren _children;
    virtual int   _getTextLength(bool recursive) const;
    virtual char* _getTextData(char *ptr, bool recursive) const;
#endif
  };
  
  //typedef UElem UGroup; 
  //[Ubit5 compatibility] UGroup is an obsolete synonym for UElem.
  
  inline UElem& uelem(UArgs args = UArgs::none) {return *new UElem(args);}
  ///< shortcut function that returns *new UElem().
  
  inline UElem& ugroup(UArgs args = UArgs::none) {return *new UElem(args);}
  ///< [Ubit5 compatibility] shortcut function that returns *new UElem(args).
  
}
#endif
