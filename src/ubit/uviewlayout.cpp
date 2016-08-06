/************************************************************************
 *
 *  uviewlayout.cpp
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | ENST Paris | http://www.enst.fr/~elc/ubit
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
#include <ubit/uupdatecontext.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uview.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uscrollpane.hpp>
using namespace std;
namespace ubit {


UViewLayoutImpl::UViewLayoutImpl(UView *v) {
  view = v;
  view->hflex_count = view->vflex_count = 0; 
  visibleElemCount = 0;
  chwidth  = pos_chwidth = 0;
  chheight = pos_chheight = 0;
  orient = 0; // orient initialise plus tard en fct de curp
  mustLayoutAgain = false;
}

/* ==================================================== ======== ======= */

void UViewLayoutImpl::computeWidth(const UUpdateContext& ctx,
                                   const UPaddingSpec& padding,
                                   UViewLayout& vl, bool minmax_defined) {
  const USizeSpec& size = ctx.local.size;
  
  // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

  float border_width = padding.left.val + padding.right.val 
    + ctx.local.padding.left.val + ctx.local.padding.right.val;

  // auto width of this view (= children + padding/borders)
  float auto_width = max(chwidth, pos_chwidth) + border_width;
  
  enum {ADJUST, KEEP_SIZE, FIXED_SIZE} mode;
  float spec_width = 0.;
 
  // keep into account vl.strategy for embedded uflowboxes
  /*
   // 28sept08: sert a quoi? pose pbm avec les tables
  if (vl.strategy == UViewLayout::IMPOSE_WIDTH) {
    spec_width = vl.spec_w;       // imposer la taille donnee par parent
    if (spec_width > 0.) mode = FIXED_SIZE; else mode = KEEP_SIZE;
  }
  else */ {
    // NB: PERCENT is meaningless at this stage (must be taken into account later)
    //if (size.width.unit==UAUTO || size.width.unit==UPERCENT || size.width.unit==UPERCENT_CTR)
      if (size.width==UAUTO || size.width.unit==UPERCENT || size.width.unit==UPERCENT_CTR)
      mode = ADJUST;
    else if (size.width == USize::INITIAL)   // -1 means "keep initial size"
      mode = KEEP_SIZE;
    else {
      mode = FIXED_SIZE;
      spec_width = size.width.toPixels(ctx.getDisp(), ctx.fontdesc, -1, -1);
    }
  }
    
  // DIFFERS from computeHeight!
  if (vl.strategy == UViewLayout::NESTED) {
    // cas nested flowboxes -> les retailler (sauf si taille specifiee)
    if (mode == KEEP_SIZE) mode = ADJUST;
  }
  
  // size is computed the 1st time then does not change (except when zoomed)
  if (mode == KEEP_SIZE) {
    // INITIALIZING is set at the 1st update, INITIALIZED at the 2nd update
    // (a combination of layout+update+layout is necessary for correct init)
    UViewKeepSizeProp* ks = null;
    
    if (view->width<=0 || !view->hasVMode(UView::INITIALIZED) || !view->getProp(ks)) {
      view->obtainProp(ks);
      ks->width = auto_width / ctx.xyscale;
      // ce qui suit permet de calculer la taille (max) d'une liste d'objets
      // qui partagent le meme uwidth (ce qui permettra de les aligner)
      //  float ww = auto_width / ctx.xyscale;
      // if (ctx.pwidth) {       !!! pbm si partage sur les defauts
      //  if (ww > ctx.pwidth->actual_value) ctx.pwidth->actual_value = ww;
      //  else ww = ctx.pwidth->actual_value;
      // ks->width = ww;
      // }
    }
    spec_width = ks->width;      // set width to the initial (kept) value 
  }

  // rescale according to ctx.xyscale if not null and spec_width specified
  if (ctx.xyscale != 1. && spec_width > 0.) {
    spec_width = spec_width * ctx.xyscale;
  }

  if (mode == ADJUST) {
    view->removeVModes(UView::FIXED_WIDTH);
    view->width = auto_width;
  }
  else {			// keepSize ou fixedSize
    view->addVModes(UView::FIXED_WIDTH);
    // NB: the fixedSize can be changed by client or zoom => always reset width
    view->width = spec_width;
  }

  // width of children. floating children are not taken into account
  // because view->chwidth will be used by uhflex() in viewupdate()
  // and floating object are maeningless for this computation
  view->chwidth = chwidth;
  vl.spec_w = spec_width;	// <= keepSize if undefined
  
  if (minmax_defined) {
    vl.dim.width = view->width;
    vl.min_w += border_width;
    vl.max_w += border_width;
  }
  else vl.dim.width = vl.min_w = vl.max_w = view->width; 
}

/* ==================================================== ======== ======= */

void UViewLayoutImpl::computeHeight(const UUpdateContext& ctx,
                                    const UPaddingSpec& padding,
                                    UViewLayout& vl, bool minmax_defined) {
  const USizeSpec& size = ctx.local.size;

  // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

  float border_height = padding.top.val + padding.bottom.val
  + ctx.local.padding.top.val + ctx.local.padding.bottom.val;

  // auto width of this view (= children + padding/borders)
  float auto_height = max(chheight, pos_chheight) + border_height;

  enum {ADJUST, KEEP_SIZE, FIXED_SIZE} mode;
  float spec_height = 0;
  
  //if (size.height.unit==UAUTO || size.height.unit==UPERCENT || size.height.unit==UPERCENT_CTR)
  if (size.height==UAUTO || size.height.unit==UPERCENT || size.height.unit==UPERCENT_CTR)
    mode = ADJUST;
  else if (size.height == USize::INITIAL)   // -1 means "keep initial size"
    mode = KEEP_SIZE;
  else {
    mode = FIXED_SIZE;
    spec_height = ctx.local.size.height.toPixels(ctx.getDisp(), ctx.fontdesc, -1, -1);
  }
  
  if (mode == KEEP_SIZE) {
    UViewKeepSizeProp* ks = null;
    
    if (view->height<=0 || !view->hasVMode(UView::INITIALIZED) || !view->getProp(ks)) {
      view->obtainProp(ks);
      ks->height = auto_height / ctx.xyscale;
      //float hh = auto_height / ctx.xyscale
      // if (ctx.pheight) {        !pbm si partage sur les defauts
      //   if (hh > ctx.pheight->actual_value) ctx.pheight->actual_value = hh;
      //  else hh = ctx.pheight->actual_value;
      //ks->height = hh;
      //}
    }
    spec_height = ks->height;
  }

  if (ctx.xyscale != 1. && spec_height > 0.) {
    spec_height = (spec_height * ctx.xyscale); 
  }
  
  if (mode == ADJUST) {
    view->removeVModes(UView::FIXED_HEIGHT);
    view->height = auto_height;
  }
  else {			// KEEP_SIZE ou FIXED_SIZE
    view->addVModes(UView::FIXED_HEIGHT);
    view->height = spec_height;
  }

  //view->chheight = chheight - chheight_f;  cf plus haut.
  view->chheight = chheight;
  vl.spec_h = spec_height;
  
  if (minmax_defined) {
    vl.dim.height = view->height;
    vl.min_h += border_height;
    vl.max_h += border_height;
  }
  else vl.dim.height = vl.min_h = vl.max_h = view->height;
}

/* ==================================================== [Elc] ======= */

bool UView::doLayout(UUpdateContext& parp, UViewLayout& vl) {
  UViewLayoutImpl vd(this); 
  UBox* box = getBox();
  if (!box) {UAppli::internalError("UView::doLayout", "Null box!"); return false;}

  UUpdateContext curp(parp, box, this, null);
  
  if (vl.strategy == UViewLayout::IMPOSE_WIDTH) {
    //imposer la taille donnee par parent
    width = vl.spec_w;  // curp.local.width modified in computeWidth()
  }

  doLayout2(vd, *box, curp, vl);

  // IMPORTANT: Height depends on Width for UFlowview
  // ceci impose de refaire une seconde fois le layout du parent
  // (sauf dans le cas ou Width est fixe a priori auquel cas Height
  // (peut directement etre determine des la premier passe)
  return vd.mustLayoutAgain;
};


static void hintElemVert(UViewLayoutImpl& vd, const UUpdateContext& curp,
                         const UViewLayout& chvl, UElem* box);
static void hintElemHoriz(UViewLayoutImpl& vd, const UUpdateContext& curp,
                          const UViewLayout& chvl, UElem* box);
static void hintElemViewport(UViewLayoutImpl& vd, const UUpdateContext& curp,
                             const UViewLayout& chvl, UElem* box);
static void hintElemBorder(UViewLayoutImpl& vd, const UUpdateContext& curp,
                           const UViewLayout& chvl, UElem* box);

/* ==================================================== ======== ======= */

void UView::doLayout2(UViewLayoutImpl& vd, UElem& grp, UUpdateContext& curp, UViewLayout& vl) {
  UMultiList mlist(curp, grp);
  if (curp.xyscale != 1.) curp.rescale();
  scale = curp.xyscale;
  
  bool is_pane  = dynamic_cast<UScrollpane*>(&grp);  // !!!@@@ A REVOIR !!!
  bool is_border = grp.getDisplayType() == UElem::BORDER; // !!!@@@ A REVOIR !!!
  UNode* b = null;
  UElem* chgrp = null;
  UView* chboxview = null;
  
  // interdiction de tenir compte de l'orient dans les UElems
  // if (grp.boxCast()) vd.orient = curp.orient;
  
  // les UElem sont normalement en UOrient::inherit    
  vd.orient = grp.isVertical() ? UOrient::VERTICAL : UOrient::HORIZONTAL;

  // if this group is not null (which generally is the case) the object
  // it contains are added to children for normal display
  // (can for instance be used for adding list-item markers, checkboxes...
  if (curp.local.content) {
    UElem* content = curp.local.content;
    curp.local.content = null;	// avoid infinite recursion
    doLayout2(vd, *content, curp, vl);    // pas de curp, meme vd
  }

  for (UChildIter ch = mlist.begin(); ch != mlist.end(); mlist.next(ch))
    // NB: null cond means always
    if (!ch.getCond() || ch.getCond()->verifies(curp, grp)) {

      b = (*ch);
      chboxview = null;
      UViewLayout chvl; // !att: reinit by constr.
      chgrp = null;

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      if (b->toAttr()) {
        b->toAttr()->putProp(&curp, grp);
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      else if (b->toData()) {
        b->toData()->getSize(curp, chvl.dim);
        if (is_border) 
          hintElemBorder(vd, curp, chvl, null);
        else if (is_pane) 
          hintElemViewport(vd, curp, chvl, null);
        else if (vd.orient == UOrient::VERTICAL)
          hintElemVert(vd, curp, chvl, null);
        else if (vd.orient == UOrient::HORIZONTAL)
          hintElemHoriz(vd, curp, chvl, null);
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // UElems, UBoxes, UWins

      else if ((chgrp = b->toElem()) && chgrp->isShowable()) {
        UBox* boxgrp = chgrp->toBox();
        if (boxgrp) {  // QUE les Box

          if ((chgrp->getDisplayType() == UElem::BLOCK   // UBox or UPluggin
               && (chboxview = boxgrp->getViewInImpl(vd.view /*,&ch.child()*/)))
              ||
              (mlist.in_softwin_list && chgrp->getDisplayType() == UElem::SOFTWIN
               && (chboxview = boxgrp->getViewInImpl(vd.view /*,null*/)))
              ) {
            
            //if (chboxview->hasVMode(UView::FORCE_POS)) {
            if (chgrp->isFloating()) {
              vd.mustLayoutAgain |= chboxview->doLayout(curp, chvl);
              // floatings pas additionnes pour calcul taille parent, mais parent
              //  doit etre au moins aussi large que le floating le plus grand              
              if (chvl.dim.width > vd.pos_chwidth)  vd.pos_chwidth = chvl.dim.width;
              if (chvl.dim.height > vd.pos_chheight) vd.pos_chheight= chvl.dim.height;
            }

            else {
              vd.mustLayoutAgain |= chboxview->doLayout(curp, chvl);
              if (is_border)
                hintElemBorder(vd, curp, chvl, chgrp);
              else if (is_pane)
                hintElemViewport(vd, curp, chvl, chgrp);
              else if (vd.orient == UOrient::VERTICAL)
                hintElemVert(vd, curp, chvl, chgrp);
              else
                hintElemHoriz(vd, curp, chvl, chgrp);
              
            } // end cas normal
          } // endif(isDef(Mode::BOX) ...)
        } // endif(boxCast)
        
        else {  // just an UElem
          UUpdateContext chcurp(curp, chgrp, vd.view, null);
          doLayout2(vd, *chgrp, chcurp, vl);   //own curp, same vd
        }
      }
    }
      
    // la suite ne concerne pas les UElem
    if (grp.toBox()) {

    // Border and Box size
    UPaddingSpec padding(0, 0);
      
    if (curp.local.border) {
       curp.local.border->getSize(curp, padding);

      if ((chgrp = curp.local.border->getSubGroup())) {
        UViewBorderProp* vb = null;
        if (!vd.view->getProp(vb)) {
          vd.view->obtainProp(vb);
          chgrp->initView(/*new UChild(chgrp),*/ vd.view);
        }
        
        UUpdateContext chcurp(curp, chgrp, vd.view, null);
        doLayout2(vd, *chgrp, chcurp, vl);   //own curp, same vd
        padding.add(*vb);
      }
    }

    vd.computeWidth(curp, padding, vl, false);
    vd.computeHeight(curp, padding, vl, false);
  }
}

/* ==================================================== [Elc] ======= */

void hintElemHoriz(UViewLayoutImpl& vd, const UUpdateContext& curp,
                   const UViewLayout& chvl, UElem* chbox) {
  if (chbox) {
    // number of horizontally flexible child objects
    // ATT: jamais flexible si CANT_RESIZE_WIDTH
    if (curp.halign==UHalign::FLEX && chbox->isWidthResizable())
      vd.view->incrHFlexCount(); 
    // fait avant: vd.mustLayoutAgain |= chboxview->doLayout(curp, chvl);
  }

  // add spacing if something before
  if (vd.visibleElemCount > 0) vd.chwidth += curp.hspacing;

  // NB: only takes into account Ubox or Uelem (not UElem nor floating)
  vd.visibleElemCount++;

  // add element size
  vd.chwidth += chvl.dim.width;

  // imposer meme height pour tous les children
  if (chvl.dim.height > vd.chheight) vd.chheight = chvl.dim.height;
}

/* ==================================================== ======== ======= */

void hintElemVert(UViewLayoutImpl& vd, const UUpdateContext& curp,
                  const UViewLayout& chvl, UElem* chbox) {
  if (chbox) {
    // number of vertically flexible child objects
    // ATT: jamais flexible si CANT_RESIZE_HEIGHT
    if (curp.valign==UValign::FLEX && chbox->isHeightResizable())
      vd.view->incrVFlexCount();
    // fait avant: vd.mustLayoutAgain |= chboxview->doLayout(curp, chvl);
  }

  // add spacing if something before
  if (vd.visibleElemCount > 0) vd.chheight += curp.vspacing;

  // NB: only takes into account Ubox or Uelem (not UElem nor floating)
  vd.visibleElemCount++;

  // add element size then take max
  vd.chheight += chvl.dim.height;

  // imposer meme width pour tous les children
  if (chvl.dim.width > vd.chwidth) vd.chwidth = chvl.dim.width;
}

/* ==================================================== ======== ======= */

void hintElemViewport(UViewLayoutImpl& vd, const UUpdateContext& curp,
                      const UViewLayout& chvl, UElem* chbox) {
  if (!chbox) {
    UAppli::error("UView::doLayout","a UPane can only contain UBoxes in its central area");
    return;
  }

  // fait avant: vd.mustLayoutAgain |= chboxview->doLayout(curp, chvl);
  // on prend le max de tous les children
  if (vd.chwidth < chvl.dim.width) vd.chwidth = chvl.dim.width;
  if (vd.chheight < chvl.dim.height) vd.chheight = chvl.dim.height;
}

/* ==================================================== ======== ======= */

void hintElemBorder(UViewLayoutImpl& vd, const UUpdateContext& curp,
                    const UViewLayout& chvl, UElem* chbox) {
  UViewBorderProp* vb = null;           
  if (!vd.view->getProp(vb)) {
    UAppli::internalError("UView::doLayout","Invalid border");
    return;
  }
  UPaddingSpec& frame = *vb;
  
  // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

  //NB: top et bottom s'adaptent a la zone centrale en largeur
  //ils ne controlent donc que la hauteur de leurs zones respectives
  switch (curp.valign) {
  case UValign::TOP:
    if (chvl.dim.height > frame.top.val) frame.top.val = chvl.dim.height;
    break;
  case UValign::BOTTOM:
    if (chvl.dim.height > frame.bottom.val) frame.bottom.val = chvl.dim.height;
    break;
  case UValign::CENTER:
  case UValign::FLEX:
    if (chvl.dim.height > vd.chheight) vd.chheight = chvl.dim.height;
    break;
  }

  //NB: left et right s'adaptent a la zone centrale en hauteur
  //ils ne controlent donc que la largeur de leurs zones respectives
  switch (curp.halign) {
  case UHalign::LEFT:
    if (chvl.dim.width > frame.left.val) frame.left.val = chvl.dim.width;
    break;
  case UHalign::RIGHT:
    if (chvl.dim.width > frame.right.val) frame.right.val = chvl.dim.width;
    break;
  case UHalign::CENTER:
  case UHalign::FLEX:
    if (chvl.dim.width > vd.chwidth) vd.chwidth = chvl.dim.width;
    break;
  }
}

}
