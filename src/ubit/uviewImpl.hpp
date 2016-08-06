/************************************************************************
 *
 *  uviewImpl.hpp : internal implementation of UView
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

#ifndef _uviewImpl_hpp_
#define	_uviewImpl_hpp_ 1
#include <ubit/uevent.hpp>
#include <ubit/uborder.hpp>
#include <ubit/uupdatecontext.hpp>
namespace ubit {
  
  class U3DcanvasView;
  class U3Dpos;
  
  // ==================================================== ===== =======
  // [Impl] Object behaviors.
  
  class UBehavior {
  public:
    enum InputType {MOUSE=0, BROWSE=1, KEY=2};
    
    UBehavior(InputType = MOUSE);
    
    InputType intype;
    bool SOURCE_IN_MENU;   ///< is the source (indirectly) included in a menu?.
    bool DONT_CLOSE_MENU;     ///< the source does not close menus.
    const UCursor* cursor;
    UElem* browsing_group;
    UBox* event_observer;
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewProp: view property.
  
  class UViewProp {
  public:
    virtual ~UViewProp() {}
  };
  
  // used by motion/resize callbacks to check if pos/size has changed.
  struct UViewChangeCallbackProp : public UViewProp {
    UPoint pos;
    UDimension size;
  };
  
  // used by the keep size mode of USize
  struct UViewKeepSizeProp : public UViewProp {
    UViewKeepSizeProp() : width(-1), height(-1) {}
    float width, height;
  };
  
  // border padding of this view.
  struct UViewBorderProp : public UViewProp, public UPaddingSpec {
    UViewBorderProp() : UPaddingSpec(0,0) {}
  };
  
  // used by to force position.
  //struct UViewForcePosProp : public UViewProp {
  //  UViewForcePosProp() : x(0), y(0) {}
  //  float x, y;
  //};
  
  // ==================================================== ===== =======
  // [Impl] UViewContext: struct used to retrieve the view context.

  class UViewContext {
  public:
    UViewContext();
    ~UViewContext();    
    UView::FindMode find_mode;
    bool is_clip_set;    // is clip valid ?
    URect clip;
    UUpdateContext* upd_context;  // set if get_upd_context is true
    UView* layout_view;  // layout should take place from this view.
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewFind: struct used to find event sources.
  
  class UViewFind {
  public:
    UViewFind(UView* win_view, const UPoint pos_in_win,
              UBehavior::InputType intype, unsigned char catch_mask);
    void updateProps(UView*, UElem*, const UUpdateContext& cur_ctx);
    
    UView* ref_view;             // window or 3Dwidget view (if canvas_view != null)
    UPoint ref_pos;              // pos in ref_view (whatever it is)
    U3DcanvasView* canvas_view;  // != null if the source is in a 3Dwidget
    U3Dpos* refpos_in_canvas;    // 3Dpos of the 3Dwidget in canvas_view (if it is != null)
    
    UWinUpdateContext win_ctx;
    UUpdateContext found_ctx;
    unsigned char CATCH_MASK;
    UBox* catched;
    UBox* uncatchable;
    UBehavior bp;
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewLayout: struct used to layout widget views.
  
  class UViewLayout {
  public:
    enum Strategy {BOXVIEW, GET_HINTS, IMPOSE_WIDTH, NESTED} strategy;
    UDimension dim;
    float spec_w, min_w, max_w;
    float spec_h, min_h, max_h;
    UViewLayout() : strategy(BOXVIEW) {}
  };
  
  class UViewLayoutImpl {
  public:
    UViewLayoutImpl(UView*);
    void computeWidth(const UUpdateContext& curp, const UPaddingSpec&,
                      UViewLayout&, bool minmax_defined);
    void computeHeight(const UUpdateContext& curp, const UPaddingSpec&,
                       UViewLayout&, bool minmax_defined);
    
    UView* view;
    int visibleElemCount;  // UBox or UData (not UElem, not a PosBox)
    float chwidth, pos_chwidth, chheight, pos_chheight;  // ex int
    unsigned char orient;
    bool mustLayoutAgain;
  };
  
  // ==================================================== ===== =======
  // [Impl] UViewUpdate: struct used to update widget views.
  
  class UViewUpdate {
  public:
    // !BEWARE: order is MEANINGFUL: all ITEM operations must be > ITEM_OPS
    enum Mode {
      PAINT_ALL,       // Paint all objects in clipping area
      PAINT_DAMAGED,   // Just paint damaged objects and their children,
      //MAIS AUSSI toutes les vues soeurs qui suivent (a cause des vues
      //evntuellement superposees, voir note dans UViewUpdateData ci-dessous)
      //PAINT_CHANGED, // Just paint objects whose size or pos has changed
      UPDATE_DATA,     // Just update data, do not paint
      FIND_DATA_POS,   // Locate an UData from its pos - do NOT paint it
      FIND_DATA_PTR    // Locate an UData from a link or a pointer
    };
    
    Mode mode;
    UDataContext* datactx;  // optional, used when searching UDatas
    // when applicable (mode PAINT_DAMAGED) : 0 before drawing any damaged,
    // > 0 No (depth) of the damaged view, < 0 after painting the first damaged
    int damaged_level;
    // true when one or several damaged views have been processed
    bool after_damaged;
    // number of views to repaint above damaged objects
    int above_damaged_count;
    
    UViewUpdate(Mode _mode) {
      mode = _mode;
      datactx = null;
      damaged_level = 0;
      after_damaged = false;
      above_damaged_count = 0;
    }
    
    void setMode(Mode _mode) {mode = _mode;}
    Mode getMode() {return mode;}    
  };
  
  
  class UViewUpdateImpl : public URect {
  public:
    UViewUpdateImpl(UView*, const URect& r, UViewUpdate&);
    virtual ~UViewUpdateImpl();
    
    virtual void updateBackground(UGraph&, UUpdateContext&, 
                                  const URect &r, const URect& clip);
    virtual void setPadding(UGraph&, const UUpdateContext&, 
                            const URect &r, bool add_frame_and_paddind);
    virtual void callPaintCallbacks(UElem&, UUpdateContext&);
    virtual void callMoveResizeCallbacks(UElem&, UUpdateContext&);
    
    UView* view;
    UPaddingSpec pad;
    URect chr, chclip;
    float vflex_space, hflex_space;
    bool can_paint;
    unsigned char orient;
    UViewUpdate& upmode;
    class UTableView* tabview;
    class UEdit* edit;  
  };
  
  // ==================================================== ===== =======
  
  class UMultiList {
  public:
    static const int MAXCOUNT = 2;
    
    // parses the child list of grp, finds the 1st visible element (UData or UElem)
    // and adds it (and its followers) as a sublist
    // also checks HARDWIN and SOFTWIN_LIST
    UMultiList(UUpdateContext&, UElem&);
    
    UChildIter begin() {return sublists[current = 0].begin;}
    UChildIter end() {return sublists[current].end;}
    // att: end() doit etre celui de current car cette valeur est utilisee ds le code.
    
    void next(UChildIter& c);
    void addChildList(UChildIter begin, UChildIter end);
    
    //private:
    struct SubList {UChildIter begin, end;};
    SubList sublists[MAXCOUNT];
    int card, current;
    bool in_softwin_list;
  };
  
}
#endif

