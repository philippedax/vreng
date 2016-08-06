/************************************************************************
 *
 *  uview.hpp : UBox views
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

#ifndef _uview_hpp_
#define	_uview_hpp_ 1
#include <list>
#include <ubit/uelem.hpp>
#include <ubit/uevent.hpp>
#include <ubit/ugeom.hpp>
namespace ubit {
  
  class UFlowView;
  class UTableView;
  class UPaneView;
  class UViewFind;
  class UViewLayout;
  class UViewUpdate;
  class UViewLayoutImpl;
  class UViewUpdateImpl;
  class UFlowLayoutImpl;
  class UFlowUpdateImpl;
  class UMultiList;
  class UViewProp;
  //struct UViewContext;
  
  /* ==================================================== ===== ======= */
  /** Specifies the View Style of an UBox.
   */
  class UViewStyle : public UAttr {
  public:
    UCLASSDEF("UViewStyle", UViewStyle, null)

    UViewStyle(UView* (*)(UBox*, UView*, UHardwinImpl*), UConst);
    virtual ~UViewStyle() {destructs();}
    
    UView* (*createView)(UBox*, UView* parview, UHardwinImpl*);
    ///< pointer to the corresponding UView::createView static constructor.
    
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    ///< NOTE that this function require a specific destructor.
  };
  
  /* ==================================================== ===== ======= */
  /** Box View.
   *  UBox objects (and subclasses except UWin) can have several views.
   *  these views can be retrieved by methods UBox::getView(), UBox::getViews(), etc.
   *  
   *  Note: UWin objects (and subclasses) have only onse (shared) view that
   *  is called the "window view".
   */
  class UView : public URect {
  public:
    UCLASSDEF("UView", UView, null)
    
    static UViewStyle style;
    
    UView(UBox*, UView* parview, UHardwinImpl*);
    virtual ~UView();
    
    virtual UFlowView*  toFlowView()  {return null;}
    virtual UTableView* toTableView() {return null;}
    virtual UPaneView*  toPaneView()  {return null;}
    
    bool isShown() const;
    ///< returns true if this view is currently shown.
    
    bool isRealized() const;
    ///< returns true if system resources have been allocated.
    
    float getWidth()  const {return width;}
    ///< returns the width of the view.
    
    float getHeight() const {return height;}
    ///< returns the height of the view.
    
    UDimension getSize() const;
    ///< returns the size of the view.
    
    void setSize(const UDimension&);
    /**< changes the size of the view (see details).
     * Note that most box views are layed out automatically. Calling setSize()
     * on such views won't have the expected effect because their size is 
     * controlled by the layout manager.
     */
    
    static UPoint convertPosTo(const UView& to, const UView& from, const UPoint& pos_in_from);
    /**< converts position in 'from' view to position in 'to' view.
     * both views must be on the same UDisp. returns false if the conversion failed.
     */
    
    float getX() const;
    ///< returns the x position of the view relatively to its parent origin. 
    
    float getY() const;
    ///< returns the y position of the view relatively to its parent origin. 
    
    UPoint getPos() const;
    ///< returns the position of the view relatively to its parent origin. 
    
    UPoint getPosIn(UView& ref_view) const;
    ///< returns the position of the view relatively to the view given as an argument. 

    UPoint getScreenPos() const;
    ///< returns the position of the view relatively to the screen origin. 
    
    UPoint getGLPos() const;
    /**< returns the position of the view relatively to its hard window in GL coordinates.
     * y is relative to the BOTTOM left corner.
     * @see getHardwinPos() to obtain x,y in 'normal' coordinates
     */
    
    UPoint getHardwinPos() const;
    /**< returns the position of the view relatively to its hard window. 
     * y is relative to the TOP left corner
     * - @see getGLPos() to obtain x,y in OpenGL coordinates
     * - NB: coordinates are always relative to the topmost hard window (USubwin
     *   hard windows are not taken into account)
     */
    
    float getScale() const {return scale;}
    ///< returns the scaling factor applied to this view.
    
    
    virtual bool caretPosToXY(long caret_pos, int& xcol, int& yline) const;
    /**< convert a caret position to XY coordinates.
     * returns false if the conversion did not succeeded.
     */
    
    virtual bool xyToCaretPos(int line, int col, long& caret_pos) const;
    /**< convert XY coordinates to a caret position.
     * returns false if the conversion did not succeeded.
     */
    
    UBox* getBox() const {return box;}
    ///< returns the widget that controls this view.
    
    UBox* getBoxParent() const;
    ///< returns the closest UBox parent of getBox() (which is not necessarily a direct parent).
    
    UView* getParentView() const {return parview;}
    ///< returns the parent view (if any, null for window views).
    
    UDisp* getDisp() const;
    ///< returns the Display where this view is displayed.
    
    UWin* getWin() const;
    ///< returns the Window where this view is displayed.
    
    UView* getWinView() const;
    ///< returns the view of the Window where this view is displayed.
    
    UHardwinImpl* getHardwin() const {return hardwin;}
    ///< returns the hard window that contains this view.
        
    bool isChildOf(const std::vector<UView*>& parent_views);
    
    // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  

    virtual bool containsWC(const UPoint& pos_in_win);
    ///< returns true if this point (in window coordinates) in contained in this view.
    
    virtual void updateLayout(const UDimension* force_size, bool upd_paint_data = true);
    ///< [Impl] updates layout coordinates.

    virtual void updatePaintData(const URect* region = null);
    /**< [Impl] updates paint data (but does not paint).
     * certain coordinates (related to scrolling etc.) are updated while painting.
     * this function updates them without acutally painting
     */
    
    virtual void updatePaint(const URect* region = null);
    /**< [Impl] paints this view or a part of this view.
     * note that 'region', must be in window coordinates if provided.
     */         

    virtual UView* findSource(UViewFind&, UPoint& pos_in_src);
    ///< [Impl].
    
    virtual UView* findSourceNext(UViewFind&, UPoint& pos_in_src);
    ///< [Impl].
    
    enum FindMode {FIND_CLIP, FIND_VIEW_CONTEXT, FIND_PARENT_CONTEXT};

    virtual bool findContext(UViewContext&, FindMode);
    ///< [Impl].
    
    virtual UData* findData(UDataContext&, const UPoint& pos,
                            const UData* searched_data, int strpos1, int strpos2);
    ///< [Impl].

#ifndef NO_DOC
    enum VMODES {
      // INITIALIZING is set at the 1st update, INITIALIZED at the 2nd update
      // (a combination of layout+update+layout is necessary for correct init)
      INITIALIZING = 1<<0,
      INITIALIZED  = 1<<1,
      DAMAGED      = 1<<2,      // this view has been damaged and must be repaint
      // this viewobject has been fully destructed (by all its destructors)
      // but the memory has not yet been freed (operator delete being redefined)
      DESTRUCTED   = 1<<3,
      FIXED_WIDTH  = 1<<4,      // this view has a fixed width.
      FIXED_HEIGHT = 1<<5,      // this view has a fixed height.
      REALIZED_CHILDREN = 1<<6, // the children of this view have been realized (for win views only)
      POS_HAS_CHANGED  = 1<<9,  // position has changed => geometry must be updated
      SIZE_HAS_CHANGED = 1<<10, // size has changed => geometry must be updated
      NO_DOUBLE_BUFFER = 1<< 11 
      // !BEWARE: no comma after last item!
    };
    
    typedef std::list<UViewProp*> UViewProps;
    
    static UView* createView(UBox*, UView* parview, UHardwinImpl*);
    // createView() is a static constructor used by UViewStyle to make a new view.
    
    void operator delete(void*);
    // requests view deletion.
    
    int getVModes() const {return vmodes;}
    // returns the current V-Modes bitmask.
    
    bool hasVMode(int modes) const {return (vmodes & modes) != 0;}
    // true if ONE of these V-Modes is verified.
    
    void addVModes(long modes) {vmodes |= modes;}
    // add these modes to the V-Modes bitmask.
    
    void removeVModes(long modes) {vmodes &= ~modes;}
    // remove these modes from the V-Modes bitmask.
    
    virtual UViewStyle* getViewStyle() {return &style;}
    // returns the default view style.
    
    UView* getNext() {return next;}
    // returns the next view in the list (if any, null otherwise).
    
    template <typename T> T* getProp(T*& p) { 
      p = null; 
      for (UViewProps::iterator i = props.begin(); i != props.end(); ++i)
      { if (dynamic_cast<T*>(*i)) {p = (T*)*i; return p;} }
      return null;
    }
    // returns the corresponding View Property; null if this property does not belong to the view.
    
    template <typename T> T& obtainProp(T*& p) {
      getProp(p); if (!p) {p = new T(); props.push_back(p);} return *p;
    }
    // returns or creates the corresponding View Property: creates and adds this property if it does not belong to the view.
    
    void incrHFlexCount() {++hflex_count;}
    void incrVFlexCount() {++vflex_count;}
    void setScale(float s) {scale = s;}
    
    virtual bool doLayout(UUpdateContext&, UViewLayout&);
    virtual void doUpdate(UUpdateContext&, URect r, URect clip, UViewUpdate&);

  protected:
    int vmodes;              // modes of this view
    float scale;
    float chwidth, chheight; // size occupied by children
    float edit_shift;        // for UEdit
    unsigned short hflex_count, vflex_count; // number of horiz and vert flexible children
    UView* parview;          // parent view
    UBox* box;               // corresponding UBox
    UHardwinImpl* hardwin;   // hard window 
    UView* next;	           // next view
    UViewProps props;
    
    void setParentView(UView* parent_view);
    void setNext(UView* v) {next = v;}
            
    virtual void doLayout2(UViewLayoutImpl&, UElem&, UUpdateContext&, UViewLayout&);
    
    virtual void doUpdate2(UViewUpdateImpl&, UElem&, UUpdateContext&,
                           URect& r, URect& clip, UViewUpdate&);
    
    virtual bool updatePos(UViewUpdateImpl&, UElem&, UUpdateContext& curctx,
                           URect& r, URect& clip, UViewUpdate&);
    
    virtual void beginUpdate3d(UViewUpdateImpl&, UElem&, UUpdateContext& curctx);
    virtual void endUpdate3d(UViewUpdateImpl&, UElem&, UUpdateContext& curctx);
    
    virtual UView* findInBox(UBox*, const UPoint& winpos, const UUpdateContext&, UViewFind&);
    virtual UView* findInGroup(UElem*, const UPoint& winpos, const UUpdateContext&, UViewFind&);
    virtual UView* findInChildren(UElem*, const UPoint& winpos, const UUpdateContext&, UViewFind&);
    
    virtual bool findDataV(UUpdateContext&, UChildIter data_iter, UChildIter end_iter, 
                           const URect&, UViewUpdate&);
    virtual bool findDataH(UUpdateContext&, UChildIter data_iter, UChildIter end_iter, 
                           const URect&, UViewUpdate&);
    virtual bool findDataPtr(UUpdateContext&, UChildIter data_iter, UChildIter end_iter, 
                             const URect&, UViewUpdate&);
    
    static void initLayoutH(UViewUpdateImpl& vd, const UUpdateContext& curp, const URect& r);
    static void initLayoutV(UViewUpdateImpl& vd, const UUpdateContext& curp, const URect& r);
    static void initLayoutViewport(UViewUpdateImpl& vd, const UUpdateContext& curp, const URect& r);
    static void layoutH(UViewUpdateImpl& vd, const UUpdateContext& curp, UChildIter link,
                        const UDimension& dim, UElem* chgrp, UView* chview);
    static void layoutV(UViewUpdateImpl& vd, const UUpdateContext& curp, UChildIter link,
                        const UDimension& dim, UElem* chgrp, UView* chview);
    static void layoutViewport(UViewUpdateImpl& vd, const UUpdateContext& curp,
                               UChildIter link, const UDimension& dim, UView* chview);
#endif
  private:
    friend class UBox;
    friend class UWin;
    friend class UHardwinImpl;
    friend class USubwin;
    friend class UChild;
    friend class UViewLayoutImpl;
    friend class UViewUpdateImpl;
    friend class UAppliImpl;
    friend class UInputEvent;
    friend class UEventFlow;
    friend class U3DcanvasView;
    UView(const UView&);
    UView& operator=(const UView&);  // assigment is forbidden
  };
  
  // ==================================================== ===== ======= //
  
  class UFlowView: public UView {
  public:
    UCLASSDEF("UFlowView", UFlowView, null)

    static UViewStyle style;  // renderer
    virtual UViewStyle* getViewStyle() {return &style;}
    
    UFlowView(UBox*, UView* parview, UHardwinImpl*);
    virtual ~UFlowView();
    
    static UView* createView(UBox*, UView* parview, UHardwinImpl*);
    
    virtual UFlowView* toFlowView() {return this;}
    ///< pseudo dynamic cast: returns this object if it a a UFlowView.
    
    virtual bool caretPosToXY(long caret_pos, int& xcol, int& yline) const; // ex int
    virtual bool xyToCaretPos(int xcol, int yline, long& caret_pos) const;
    
    virtual bool doLayout(UUpdateContext& parent_context, UViewLayout&);
    virtual void doUpdate(UUpdateContext& parent_context, URect r, URect clip, UViewUpdate&);
    
    float getMaxWidth() const;
    
    // - - - Impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  private:
    friend class UFlowLayoutImpl;
    friend class UFlowUpdateImpl;
    class UFlowLine* lines;
    class UFlowCell* cells;
    int line_count, cell_count, lastline_strcell;
    int alloc_line_count, alloc_cell_count;
    
    //NB: pour des raiason historiquesflowview utilise des fonctions differentes
    // de sa superclasse UView
    void flowDoLayout(UFlowLayoutImpl&, UElem&, UUpdateContext&, UMultiList&);
    void flowDoUpdate(UFlowUpdateImpl&, UUpdateContext&, UElem&, UMultiList&,
                      const URect& r, URect& clip, UViewUpdate&);
    
    //NB: NOT virtual: these functions are used instead of findDataV, findDataH, findDataPtr
    bool flowFindDataPos(UUpdateContext&, UChildIter data_it, UChildIter end_it, 
                         UFlowCell*, const URect&, UViewUpdate&);
    
    bool flowFindDataPtr(UUpdateContext&, UChildIter data_it, UChildIter end_it,
                         UFlowCell*, const URect&, UViewUpdate&);
  };
  
  
}
#endif
