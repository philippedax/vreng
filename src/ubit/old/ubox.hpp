/************************************************************************
 *
 *  ubox.hpp: Box Elements (elements that manage 1 or N screen UView(s))
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

#ifndef _ubox_hpp_
#define	_ubox_hpp_ 1
#include <ubit/uelem.hpp>
namespace ubit {
  
  /** Box container.
   * UBox is the base class for widgets:
   * - interactors: UButton, UItem, UTextfield, UTextarea...
   * - containers: UHbox, UVbox, UMenubar...
   * - windows: UWin, UDialog, UMenu...
   * Most widgets' methods are inherited from UBox and UElem.
   *
   *
   * IMPORTANT NOTE ON RECURSIVE DESTRUCTION AND SMART POINTERS:
   * When a UBox is destroyed, its direct and indirect children are ALSO destroyed
   * EXCEPT if they have other parents OR if they are pointed by a uptr<> Smart Pointer.
   * @see class UNode for details.
   *
   * Multiple parents and multiple views:
   * - UBox objects can have several parents (that can also have multiple parents).
   *   They are then replicated on the screen(s), each replication corresponding to a UView.
   *   These views are synchronized when the UBox content is modified.
   *   A UBox controls as many UViews as there are paths from the root of the instance 
   *   graph to this UBox. @see: UView.
   *
   * Transparency:
   *  - boxes (and their subclasses) can be made fully transparent by adding
   *    the UBackground::none property, or partially transparent by adding a 
   *    UAlpha property to them.
   *
   *  Layout:
   *  - UBox, UHbox, UBar : horizontal layout by default (add UOrient::vertical
   *    for vertical layout). Children are adjusted in the vertical direction
   *    (@see: uvflex() for details)
   *  - UVbox : vertical layout by default. Children are adjusted in horizontal
   *    direction adjusted (@see: uhflex() for details)
   *  - UFlowbox : "flow" layout (similar to the layout of an HTML page)
   *    Typically used from creating text areas or hypermedia widgets.
   *
   *  AutoResize:
   *  - UBox, UHbox, UVbox, UBar are resized when children are added, removed or
   *    resized
   *  - UTextbox, UTextfield, etc. are not resized when their children are modified.
   *    For this reason, they are well adapted for displaying text
   *  - UFlowbox, UTextarea implement a specific behavior: their height changes 
   *    but their width remains constant
   */
  class UBox: public UElem {
  public:
    UCLASS(UBox)
    
    UBox(UArgs args = UArgs::none);  
    /**< creates a new UBox; 
     * The argument can either be a single object pointer or reference, or a list 
     * of object pointers or references separated by + operators, example:
     * <pre>
     *   UButton* b = new UButton(UPix::disquette + "Save" + ucall(obj, saveFunc));
     * </pre>
     *
     * @see also the ubox() shortcut, that returns: *new UBox(arglist)
     */
    
    virtual ~UBox() {destructs();}
    /**< recursive destructor.
     * children are automatically destroyed if not pointed elsewhere
     * (in the instance graph or by a uptr<>). see class UNode for details.
     */
    
    virtual UBox* toBox() {return this;}
    virtual const UBox* toBox() const {return this;}
    
    static UStyle* createStyle();
    /**< static function that returns the style of this class (see details!).
     * this function MUST be redefined by subclasses if they use another UStyle.
     */
    
    virtual int getDisplayType() const {return BLOCK;} // redefined
    
    void repaint() {update(UUpdate::paint);}           // redefined    
    void update() {update(UUpdate::layoutAndPaint);}   // redefined
    virtual void update(const UUpdate& update_options, UDisp* = null);  // redefined
    virtual void doUpdate(const UUpdate&, UDisp* = null);  // redefined
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    virtual int getViewCount() const;
    ///< returns the number of views of this box.
    
    virtual int getViews(std::vector<UView*>&) const;
    /**< returns the views of this box (see DETAILS).
     * the vector is emptied then filled with the views that are controlled by this box.
     * The view count is returned
     *
     * REMARK: the views can be obtained in a more efficient way as follows:
     * <pre>
     *   for (UView* v = obj->getView(); v!=null; v = v->getNext())
     *      ....;
     * </pre>
     */
    
    virtual UView* getView(int n = 0) const;
    /**< returns the Nth view of this box (see DETAILS).
     * returns the first view if 'n' = 0 and the last view if 'n' = -1.
     *
     * REMARK: getView() can also be used to access all views:
     * <pre>
     *   for (UView* v = obj->getView(); v!=null; v = v->getNext())
     *      ....;
     * </pre>
     */
    
    virtual UView* getView(const UInputEvent&) const;
    ///< returns the view of this box that contains the event location.
      
    virtual UView* getView(UWin&, const UPoint& pos_in_win, UDisp* = null) const;
    ///< returns the view of this box that contains this point in this window.

    virtual UView* getViewContaining(const UView& child_view) const;
    /**< returns the view of this box that contains 'child_view'.
     * return null if 'child_view' is not a (direct or indirect) child of a view
     * of this box ('child_view' can also be one of the views of this box)
     */
    
    virtual UView* getFirstViewInside(const UView& parent_view) const;
    /**< returns the first box view that is contained in 'parent_view'.
     * return null if 'parent_view' is not a (direct or indirect) parent of a view
     * of this box
     */
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
#ifndef NO_DOC
    
    virtual int retrieveRelatedViews(std::vector<UView*>& collating_vector) const;
    /* returns the views that are related to this box.
     * retrieves all the views that are currently controlled by this UBox.
     *
     * - this function does the same as getViews() except that 'collating_vector' 
     *   is not emptied: the views are added to the end of what it already contains.
     * - the function returns the number of views that were added (which may thus differ
     *   from the total size of 'collating_vector')
     *
     * Note that elements and boxes can have several parents, so that multiple wiews
     * can be associated to the same widget (see UElem and UBox classes for details)
     */
    
    virtual UView* getViewInImpl(UView* parent_view);
    
    virtual UView* getViewInImpl(UView* parent_view, UDimension&);
    /* returns the object's view that is contained in this DIRECT parent view.
     * 'parent_view' will contain several views of this object if it has been added
     * several times to this parent.
     */
    
    void setViewModes(int view_modes, bool);
    
  protected:
    friend class UElem;
    friend class UWin;
    friend class UView;
    friend class UAppliImpl;
    UView* views;

    virtual void addViewImpl(UView*);
    virtual void initView(UView* parent_view);
    virtual void initChildViews(UElem*);
    virtual void deleteViewsInside(const std::vector<UView*>& parent_views);
 #endif
  };
  
  inline UBox& ubox(const UArgs& args = UArgs::none) {return *new UBox(args);}
  /**< shortcut that returns: *new UBox().
   * Note: watch the case: UBox is a class while ubox() is a function!
   */
  
  UBox& uvbox(UArgs args = UArgs::none);
  ///< shortcut that creates a vertical UBox.
  
  UBox& uhbox(UArgs args = UArgs::none);
  ///< shortcut that creates a horizontal UBox.

}
#endif

