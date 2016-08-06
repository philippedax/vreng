/************************************************************************
 *
 *  uview.cpp
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <ubit/ucond.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uview.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/uon.hpp>
#include <ubit/uevent.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


UViewStyle::UViewStyle(UView*(*make_view)(UBox*, UView*, UHardwinImpl*), UConst c) 
: UAttr(c), createView(make_view) {}

void UViewStyle::addingTo(UChild& c, UElem& parent) {
  UNode::addingTo(c, parent);
  if (parent.emodes.HAS_LAYOUT) {
    UAppli::warning("UViewStyle::addingTo","This UViewStyle object (%p) has a parent (%s %p) that contains another UViewStyle object", this, &parent.getClassName(), &parent);
  }
  parent.emodes.HAS_LAYOUT = true;
}

void UViewStyle::removingFrom(UChild& c, UElem& parent) {
  parent.emodes.HAS_LAYOUT = false;
  UNode::removingFrom(c, parent);
}

// ==================================================== [ELC] ==================

UViewStyle UView::style(&UView::createView, UViewStyle::UCONST);

UView* UView::createView(UBox* box, UView* parview, UHardwinImpl* w) {
  return new UView(box, parview, w);
}

UView::UView(UBox*_box, UView*_parview, UHardwinImpl* w) :
vmodes(0),
scale(1.),
chwidth(0.), chheight(0.),
edit_shift(0.),
hflex_count(0), vflex_count(0), 
parview(_parview), box(_box), hardwin(w),
next(null) {
}

UView::~UView() {
  addVModes(DESTRUCTED);  // this view has been destructed
  for (UViewProps::iterator i = props.begin(); i != props.end(); ++i) delete (*i);  
  next = null;
  UAppli::deleteNotify(this); // notifies the Appli that this view has been destructed
}

void UView::operator delete(void* p) {
  if (!p) return;
  if (UAppli::impl.isTerminated()) ::operator delete(p);
  else UAppli::impl.addDeleteRequest(static_cast<UView*>(p));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool   UView::isRealized() const {return hardwin && hardwin->isRealized();}
UDisp* UView::getDisp()    const {return hardwin ? hardwin->disp : null;}
UWin*  UView::getWin()     const {return hardwin ? hardwin->win : null;}

UView* UView::getWinView() const {
  if (!hardwin || !hardwin->win) return null; 
  else return hardwin->win->views;      // OPTIMISATION: suppose une seule Hardwin par disp !!
}

void UView::setParentView(UView* pv) {
  parview = pv;
  hardwin = pv->hardwin;
}

UBox* UView::getBoxParent() const {
  return parview ? parview->getBox() : null;
}

bool UView::isChildOf(const vector<UView*>& parent_views) {
  for (unsigned int k = 0; k < parent_views.size(); k++) {
    if (parview == parent_views[k]) return true;
  }
  return false;
}

bool UView::isShown() const {                 // devrait etre optimise !!!
  // ex: 4nov06: plantage dans destructeurs: getBox() peut alors etre nul
  // if (!getBox()->isShowable()) return false;
  UBox* b = getBox();
  if (!b || !b->isShowable()) return false;
  if (!parview)              // cas hardwin, parent_view is null !!
    return b->isShown();
  else
    return parview->isShown();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 //void setPos(const UPoint&);
 * changes the position of the view (see details).
 * Note that most box views are layed out automatically. Calling setPos()
 * on such views won't have the expected effect because their position is
 * controlled by the layout manager.
 
 //void setAutoPositioning(bool);
 // speficies update policy when the object is modified.
 
 UBox* UView::getBox() const {
 UBox* b = null;
 if (!childnode || !(b = (**childnode)->toBox())) {
 //ne pas faire car peut arriver dans etape de destruction
 //UAppli::internalError("UView::getBox","this UView has no corresponding UBox");
 return null;
 }
 else return b;
 }

void UView::setPos(const UPoint& p) {
  if (hasVMode(FORCE_POS)) {
    UViewForcePosProp* sp = null;
    obtainProp(sp);
    sp->x = p.x; sp->y = p.y;
  }
  else { // AUTO_POS
    if (parview) {x = p.x + parview->x; y = p.y + parview->y;}
    else {x = p.x; y = p.y;}
  }
}

void UView::setAutoPositioning(bool autopos) {
  if (autopos) removeVModes(FORCE_POS);
  else {
    addVModes(FORCE_POS);
    UViewForcePosProp* sp = null;
    obtainProp(sp);
  }
}
*/
/* EX:
UPoint UView::getTopWinPos() const {
  return UPoint(x, y);
}

UPoint UView::getWinPos() const {
  UView* winview = getWinView();
  // must return (0,0) for a subwindow's view
  if (!winview || this == winview) return UPoint(0,0);
  else return UPoint(x, y);
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UDimension UView::getSize() const {return UDimension(width, height);}
void UView::setSize(const UDimension& s) {width = s.width; height = s.height;}

float UView::getX() const {if (parview) return x - parview->x; else return x;}
float UView::getY() const {if (parview) return y - parview->y; else return y;}

UPoint UView::getPos() const {
  if (parview) return UPoint(x - parview->x, y - parview->y);
  else return UPoint(x, y);
}

UPoint UView::getHardwinPos() const {
  return UPoint(x, y);
}

UPoint UView::getGLPos() const {
  UView* winview = getWinView();
  // must return (0,0) for a subwindow's view
  if (!winview || this == winview) return UPoint(0,0);
  else return UPoint(x, winview->height - height - y);
}

UPoint UView::getScreenPos() const {
  UWin* w = getWin();
  if (!w) return UPoint(0,0);
  else {
    UPoint p = w->getScreenPos(getDisp());
    // dans le cas des hardwin et softwin il n'y a rien a ajouter
    // (et dans le cas des softwins on ajouterait 2 fois x et y)
    UBox* b = getBox();
    if (!b || !b->toWin()) {
      p.x += x;
      p.y += y;
    }
    return p;
  }
}

UPoint UView::getPosIn(UView& ref_view) const {
  return convertPosTo(ref_view, *this, UPoint(0,0));
}

UPoint UView::convertPosTo(const UView& to, const UView& from, const UPoint& from_pos) {
  // marche que si both view in same win
  if (to.hardwin == from.hardwin) {
    return UPoint(from_pos.x + from.x - to.x, from_pos.y + from.y - to.y);
  }

  // on devrait plutot comparer les Screens !!!
  if (to.getDisp() == from.getDisp()) {
    UPoint frompos = from.getScreenPos();
    UPoint topos = to.getScreenPos();
    return UPoint(from_pos.x + frompos.x - topos.x, from_pos.y + frompos.y - topos.y);
  }
  
  return UPoint(0,0);
}

bool UView::caretPosToXY(long _pos, int& _x, int& _y) const {
  _y = 0;
  _x = _pos;
  return true;
}

bool UView::xyToCaretPos(int _x, int _y, long& _pos) const {
  _pos = _x;
  return (_y == 0);
}

// =========================================================== [Elc] ===========

void UView::updatePaintData(const URect* winrect) {     // window coordinates
  UView* winview = getWinView();
  if (!winview) return;
  if (!winrect) winrect = this;
  
  UWinUpdateContext winctx(winview, null);
  UViewUpdate vup(UViewUpdate::UPDATE_DATA);
  addVModes(UView::DAMAGED);
  winview->doUpdate(winctx, *winview, *winrect, vup);
  //inutile: setVmodes(UView::DAMAGED, false);
  //return vup.above_damaged_count;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::updatePaint(const URect* winrect) {     // window coordinates
  UView* winview = getWinView();
  if (!winview) return;
  if (!winrect) winrect = this;

  // !ATT: paint impossible sur winrect vide: peut poser probleme pour maj des donnees
  if (winrect->width == 0 || winrect->height == 0) return;
  
  //cerr << endl <<">>> UView::paintImpl: PAINT win: " << winview->getWin() << endl; 
  
  if (UAppli::impl.isProcessingLayoutUpdateRequests()) {
    // UAppliImpl::processUpdateRequests() is processing the layout requests
    // so that nothing should be drawn at this stage (normally the next line
    // should never be executed except in exotic cases)
    updatePaintData(winrect);
  }
  else {
    UGraph g(winview);
    UViewUpdate vup(UViewUpdate::PAINT_ALL);
    UWinUpdateContext winctx(winview, &g);
    
    // ctlmeneu laisse des bords for some reason
    //winview->doUpdate(winctx, *winview, *winrect, vup);    

    winview->doUpdate(winctx, *winview, *winview, vup);
  }
  
  //cerr << "<<< UView::paintImpl: PAINT win: " << winview->getWin() << endl; 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::updateLayout(const UDimension* size, bool upd_paint_data) {
  UView* winview = getWinView();
  if (!winview) return;
  
  UViewLayout vl;
  UWinUpdateContext winctx1(winview, null);
  winctx1.xyscale = parview ? parview->scale : 1.;
  //cerr << this << " " << getClassName() << " " << parview->scale << endl; 
  
  bool mustLayoutAgain = this->doLayout(winctx1, vl);
  
AGAIN:
  // forcer valeurs de w et h, indep de ce que veut layout()
  if (size) this->setSize(*size);
  
  // dans ce cas (typiquement pour show() d'un floating) il faut recalculer
  // le layout a partir de layoutview (= par ex. le parent du floating)
  if (upd_paint_data) {
    updatePaintData();
    if (size) this->setSize(*size);
  }
  
  if (mustLayoutAgain) {
    updatePaintData();
    if (size) this->setSize(*size);
    
    UWinUpdateContext winctx2(winview, null);     // nb: not the same WinContext!
    winctx2.xyscale = parview ? parview->scale : 1.;
    this->doLayout(winctx2, vl);
    
    mustLayoutAgain = false;
    goto AGAIN;
  }
}

// ==================================================== [ELC] ==================

UView* UView::findInChildren(UElem* grp, const UPoint& winpos,
                             const UUpdateContext& ctx, UViewFind& vf) 
{
  if (!grp->isShowable() || grp->isIgnoringEvents()) return null;
  bool in_softwin_list = grp->getDisplayType() == UElem::WINLIST;

  for (UChildReverseIter ch = grp->crbegin(); ch != grp->crend(); ++ch) {
    if (!ch.getCond() || ch.getCond()->verifies(ctx, *grp)) {
      
      UElem* chgrp = (*ch)->toElem();
      UView* chview = null;
        
      if (!chgrp || !chgrp->isShowable() || chgrp->isIgnoringEvents())
        continue;
      
      if (!chgrp->toBox()) {   // group but not box
        UView* v = findInGroup(chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
      
      else if (chgrp->getDisplayType() == UElem::BLOCK   // elimine les UWin
               && (chview = ((UBox*)chgrp)->getViewInImpl(this /*,&ch.child()*/))) {
        // !!! faudrait tester chview->isShown() !!!
        UView* v = chview->findInBox((UBox*)chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
      
      else if (in_softwin_list && chgrp->getDisplayType() == UElem::SOFTWIN
               && (chview = ((UBox*)chgrp)->getViewInImpl(this /*, null*/))) {//pas de ch
        // !!! faudrait tester chview->isShown() !!!
        UView* v = chview->findInBox((UBox*)chgrp, winpos, ctx, vf);
        if (v) return v; 
      }
    }
  }
  
  return null;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UView* UView::findInGroup(UElem* grp, const UPoint& winpos, 
                          const UUpdateContext& par_ctx, UViewFind& vf)
{
  UUpdateContext ctx(par_ctx, grp, this, null);
  UMultiList mlist(ctx, *grp);   // necessaire pour parser ctx
  return findInChildren(grp, winpos, ctx, vf);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// wpos = pos in hard window or in 3D window

bool UView::containsWC(const UPoint& pos_in_win) {     // CF SHAPE & HIDE   !!!@@@!!!!
  return (pos_in_win.x >= x && pos_in_win.y >= y 
          && pos_in_win.x < x + width && pos_in_win.y < y + height);
}

UView* UView::findInBox(UBox* box, const UPoint& wpos, 
                        const UUpdateContext& par_ctx, UViewFind& vf) 
{
  if (!box->isShowable() || box->isIgnoringEvents() || !containsWC(wpos))
    return null;

  UUpdateContext ctx(par_ctx, box, this, null);
  UMultiList mlist(ctx, *box);   // necessaire pour parser ctx
  vf.updateProps(this, box, ctx);  // !ATT evprops n'est plus recopie
  
  // si c'est une hardwin, chercher si l'event est dans ses softwins
  //BUG: if (box->getDisplayType() == UElem::HARDWIN) {
  UWin* w = box->toWin();
  if (w && w->isHardwin()) {   // all hardwins, including subwins
    UHardwinImpl* hw = w->hardImpl(/*ctx.getDisp()*/);
    UWinList* softwins = null;
    if (hw && (softwins = hw->getSoftwins())) {
      UView* v = findInGroup(softwins, wpos, ctx, vf);
      if (v) return v;
    }
  }
  
  // chercher si l'element catches ce type d'event 
  if (box != vf.uncatchable && (box->emodes.CATCH_EVENTS & vf.CATCH_MASK)) {
    vf.catched = box;
    goto FOUND;
  }

  if (box->emodes.OBSERVE_EVENTS) {
    vf.bp.event_observer = box;
  }
  
  { // chercher si l'event est dans les borders
    UElem* chgrp = null;
    UViewBorderProp* vb = null;
    if (getProp(vb) && ctx.local.border && (chgrp = ctx.local.border->getSubGroup())) {
      UView* v = findInGroup(chgrp, wpos, ctx, vf);
      if (v) return v;
      else {
        UPaddingSpec padding(0, 0);
        ctx.local.border->getSize(ctx, padding);
        padding.add(*vb);
        //clip.set(clip.x + padding.left, clip.y + padding.top,
        //         clip.width - padding.left - padding.right,
        //         clip.height - padding.top - padding.bottom);
      }
    }
  }
  
  {// sinon chercher si l'event est dans les enfants
    UView* v = findInChildren(box, wpos, ctx, vf);
    if (v) return v;
  }
  
  // else: si l'objet n'a pas d'enfant ou si la souris n'est contenue dans aucun 
  // de ses enfants alors retourner l'objet lui-meme
FOUND:
  vf.found_ctx = par_ctx;     // !ATT: par_ctx PAS ctx !
  return this;
} 

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UView* UView::findSource(UViewFind& vf, UPoint& source_pos) {
  // vf.ref_pos = pos in refview
  UView* source_view = findInBox(getBox(), vf.ref_pos, vf.win_ctx, vf);
  if (source_view) {
    // refview is either the window view or the 3Dwidget view
    // pos in source = pos in refview - pos of sourceview in refview
    source_pos.x = vf.ref_pos.x - source_view->x;
    source_pos.y = vf.ref_pos.y - source_view->y;
  }
  return source_view;
}

UView* UView::findSourceNext(UViewFind& vf, UPoint& source_pos) {
  UBox* box = getBox();
  vf.catched = null;
  vf.uncatchable = box;  // this box cannot be catched by findInBox()
  
  UView* source_view = findInBox(box, vf.ref_pos, vf.found_ctx, vf);
  if (source_view) {
    source_pos.x = vf.ref_pos.x - source_view->x;
    source_pos.y = vf.ref_pos.y - source_view->y;
  }
  return source_view;
}
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
UViewFind::UViewFind(UView* win_view, const UPoint win_pos, 
                     UBehavior::InputType intype, unsigned char catch_mask) :
  ref_view(win_view),   // window or 3Dwidget view (may be changed later)
  ref_pos(win_pos),     // pos in refview
  canvas_view(null),
  refpos_in_canvas(null),
  win_ctx(win_view, null),
  CATCH_MASK(catch_mask),
  catched(null),
  uncatchable(null),
  bp(intype) {
}

void UViewFind::updateProps(UView* v, UElem* grp, const UUpdateContext& ctx) {  
  // cursor herite mais ecrase si defini au niveau local
  if (ctx.cursor) bp.cursor = ctx.cursor;
  
  if (grp->toMenu()) bp.SOURCE_IN_MENU = true;
  else if (grp->toWin()) bp.SOURCE_IN_MENU = false;
  // EX: louche, SUBWIN oublies
  //else if (grp->getDisplayType()==UElem::HARDWIN || grp->getDisplayType()==UElem::SOFTWIN) bp.SOURCE_IN_MENU = false;
    
  if (grp->isMenuClosingDisabled()) bp.DONT_CLOSE_MENU = true;
  
  // browsing_group herité mais dominant : ecrase le niveau local
  if (!bp.browsing_group && grp->isBrowsingGroup())  bp.browsing_group = grp;
}  

// ==================================================== [ELC] ==================

UViewContext::UViewContext() {
  upd_context = null;
}

UViewContext::~UViewContext() {
  delete upd_context;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace impl {

struct UViewLink {
  UView* view;
  UViewLink* prev;
  UViewLink(UView* v, UViewLink* l) : view(v), prev(l) {}
};


static bool setCtx(UViewLink* l, const UUpdateContext& parctx, URect clip, UViewContext& vc) 
{
  if (!clip.doIntersection(*l->view)) return false;

  UBox* box = l->view->getBox();
  if (!box) {UAppli::internalError("uview::setCtx", "Null box!"); return false;}
  
  UUpdateContext ctx(parctx, box, l->view, null);
  UMultiList mlist(ctx, *box);   // necessaire pour parser ctx
  
  //if (!box.isEnabled()) vc.IS_SOURCE_ENABLED = false;
  if (box->isFloating()
      || (l->view->hasVMode(UView::FIXED_WIDTH) && l->view->hasVMode(UView::FIXED_HEIGHT)))
    vc.layout_view = l->view;
  
  UElem* chgrp = null;
  UViewBorderProp* vb = null;

  if (l->view->getProp(vb) && ctx.local.border 
      && (chgrp = ctx.local.border->getSubGroup())) 
  {
    //UView* v = findInGroup(chgrp, ctx, clip, e, searchedView, ep);
    //if (v) return v;
    //else {
      UPaddingSpec padding(0, 0);
      ctx.local.border->getSize(ctx, padding);
      padding.add(*vb);
    
    // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!
    
      clip.setRect(clip.x + padding.left.val, clip.y + padding.top.val,
                   clip.width - padding.left.val - padding.right.val,
                   clip.height - padding.top.val - padding.bottom.val);
    //}
  }
  
  if (l->prev) return setCtx(l->prev, ctx, clip, vc);
  else {
    vc.is_clip_set = true;
    vc.clip = clip;
    if (vc.find_mode == UView::FIND_VIEW_CONTEXT) 
      vc.upd_context = new UUpdateContext(ctx);
    else if (vc.find_mode == UView::FIND_PARENT_CONTEXT) 
      vc.upd_context = new UUpdateContext(parctx);
    return true;
  }
}


static bool findView(UViewLink* l, UViewContext& vc) {
  UView* parv = l->view->getParentView();
  if (parv) {
    UViewLink parl(parv, l);
    return findView(&parl, vc);
  }
  else {
    UWinUpdateContext winctx(l->view, null);
    vc.clip = *(l->view);
    return setCtx(l, winctx, vc.clip, vc);
  }
}

}  //endof namespace impl
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UView::findContext(UViewContext& vc, FindMode fmode) {
  delete vc.upd_context; vc.upd_context = null; // faudrait un uptr<> !!!
  vc.find_mode = fmode;
  vc.is_clip_set = false;
  vc.layout_view = null;
  
  impl::UViewLink l(this, null);
  return impl::findView(&l, vc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UData* UView::findData(UDataContext& dc, const UPoint& pos,
                       const UData* searched_data, int strpos1, int strpos2) {
  UViewContext vc;
  bool stat = findContext(vc, FIND_PARENT_CONTEXT);
  
  if (!stat || !vc.upd_context) return null;

  UViewUpdate vup(searched_data? UViewUpdate::FIND_DATA_PTR: UViewUpdate::FIND_DATA_POS);  
  vup.datactx = &dc;
  dc.it = dc.it2 = UChildIter();
  dc.data = (UData*)searched_data;
  dc.strpos = strpos1;
  dc.strpos2 = strpos2;
  dc.exact_match = false;  // !! 
  dc.source_view = this;
  dc.win_eventpos.set(pos.x + this->x, pos.y + this->y);
  //delete dp.dataContext; dp.dataContext = null;
  
  URect clip = *this;
  doUpdate(*vc.upd_context, clip, clip, vup);
  
  if (dc.it != dc.it2) return dc.data; else return null;
}

// ==================================================== [ELC] ==================

static void saveDataProps(UUpdateContext& ctx, UChildIter it, UChildIter end,
                          const URect& r, UViewUpdate& vup, bool exact_match) {
  int strpos = -1;
  UData* data = (*it)->toData();
  UStr* str;
  if (data && (str = data->toStr())) {   // search the strpos
    strpos = UFontMetrics(ctx).
    getCharPos(str->c_str(), str->length(), vup.datactx->win_eventpos.x - r.x);
  }
  vup.datactx->set(ctx, data, it, end, r, strpos, exact_match);
}

// Horizontal Search. 
// retrieves data and info from x,y position
// !!ATT: 
// - suppose que data_props!= null !
// - return==true signifie: ne pas chercher davantage car:
//                trouve' OU pas trouve' mais plus la peine de chercher
// - return==false signifie: continuer a chercher car:
//                pas trouve' mais on peut encore trouver 

bool UView::findDataH(UUpdateContext& ctx, UChildIter data_iter, UChildIter end_iter, 
                      const URect& r, UViewUpdate& vup) {
  if (!vup.datactx) {
    UAppli::internalError("UView::findDataH","null event or wrong type");
    return false;
  }
  const UPoint& evpos = vup.datactx->win_eventpos;
  
  if (r.x > evpos.x) return true;  // plus rien a chercher (not found)

  if (r.y <= evpos.y && evpos.y < r.y + r.height) {
    if (evpos.x < r.x + r.width) {
      // data exactly found
      saveDataProps(ctx, data_iter, end_iter, r, vup, true);
      return true;  // trouve' => finir la recherche
    }
    // data approximatively found
    else saveDataProps(ctx, data_iter, end_iter, r, vup, false);
  }

  return false;	// continuer a chercher (dans les 2cas)
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UView::findDataV(UUpdateContext& ctx, UChildIter data_iter, UChildIter end_iter, 
                      const URect& r, UViewUpdate& vup) {
  if (!vup.datactx) {
    UAppli::internalError("UView::findDataV","null event or wrong type");
    return false;
  }
  const UPoint& evpos = vup.datactx->win_eventpos;

  if (r.y > evpos.y) return true;  // plus rien a chercher (not found)

  if (r.x <= evpos.x && evpos.x < r.x + r.width) {
    if (evpos.y < r.y + r.height) {
      saveDataProps(ctx, data_iter, end_iter, r, vup, true);
      return true;
    }
    else saveDataProps(ctx, data_iter, end_iter, r, vup, false);
    
  }
  return false;	// continuer a chercher (dans les 2cas)
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UView::findDataPtr(UUpdateContext& ctx, UChildIter data_iter, UChildIter end_iter, 
                        const URect& r, UViewUpdate& vup) {
  if (!vup.datactx) {
    UAppli::internalError("UView::findDataPtr","null event or wrong type");
    return false;
  }
  
  UData* data = null;
  // retrieves info from Data link or Data ptr
  if ((vup.datactx->it == data_iter || vup.datactx->data==(*data_iter))
      && ((data = (*data_iter)->toData()))) {
    vup.datactx->set(ctx, data, data_iter, end_iter, r, vup.datactx->strpos, true);
    return true;    //trouve, inutile de continuer a chercher
  }
  
  return false;   //tous les cas pas trouve (PAS de else!)
}

// ==================================================== [ELC] ==================
// parses the AttrList and the ElemList, put properties and add
// elements (and groups) as a sublist
// also takes into account the softwin list when applicable

UMultiList::UMultiList(UUpdateContext& curp, UElem& grp) 
: card(0), current(0) {
  //for (int k = 0; k < MAXCOUNT; k++) clists[k] = null;
  sublists[0].begin = sublists[0].end = UChildIter(); //securite
  
  // put ALL the properties of the AttrList
  // does not take into account other elements
  UAttr* prop = null;
  for (UChildIter c = grp.abegin(); c != grp.aend(); ++c) {
    if ((prop = (*c)->toAttr())) {
      // NB: null cond means always
      if (!c.getCond() || c.getCond()->verifies(curp, grp))  
        prop->putProp(&curp, grp);
    }
  }
  
  for (UChildIter c = grp.cbegin(); c != grp.cend(); ++c) {
    // put the properties of the childlist that are BEFORE the first elem
    if ((prop = (*c)->toAttr())) {
      // NB: null cond means always
      if (!c.getCond() || c.getCond()->verifies(curp, grp))  
        prop->putProp(&curp, grp);
    }
    // adds the first elem (or group) and its followers as a sublist
    else if ((*c)->toData() || (*c)->toElem()) {
      addChildList(c, grp.cend());
      break;  		// passer a la suite
    }
  }
  
  //BUG: if (grp.getDisplayType() == UElem::HARDWIN) {
  UWin* w = grp.toWin();
  if (w && w->isHardwin()) {   // all hardwins, including subwins
    UHardwinImpl* hw = w->hardImpl(/*curp.getDisp()*/);
    UChildren* softwin_list = null;    
    if (hw && (softwin_list = hw->getSoftwinList())) {
      addChildList(softwin_list->begin(), softwin_list->end());
    }
    //cerr << "****** softwin " <<w <<" " << hw << " " <<  softwin_list << endl;
  }
  
  in_softwin_list = grp.getDisplayType() == UElem::WINLIST;
}

void UMultiList::next(UChildIter& c) {
  ++c;
  if (c == sublists[current].end) {
    if (current+1 < card) c = sublists[++current].begin;
  }
}

void UMultiList::addChildList(UChildIter begin, UChildIter end) {
  if (begin == end) return;  // nop (nb: test important: sauter listes nulles)
  else if (card < MAXCOUNT) {
    sublists[card].begin = begin;
    sublists[card].end = end;
    card++;
  }
  else UAppli::internalError("MultiList::addChildList","Too many elements: %d", card);
}

}
