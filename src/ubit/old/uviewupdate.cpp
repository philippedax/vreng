/************************************************************************
 *
 *  uviewupdate.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/uon.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/uedit.hpp>
#include <ubit/uima.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/utable.hpp>
#include <ubit/uscrollpane.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uborder.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

// ==================================================== [Ubit Toolkit] =========

inline void drawWallPaperLine(UGraph& g, UUpdateContext& ctx, float y, 
                              bool is_htiled, const UIma* wallp, float ima_w,
                              const URect&r, const URect&clip) {
  URect wr = r;
  wr.y = y;
  
  if (!is_htiled) {
    wr.x = (r.width - ima_w) / 2 + r.x;
    wallp->paint(g, ctx, wr);
  }
  else {
    for (wr.x = r.x; wr.x < r.x + r.width; wr.x += ima_w)
      if (wr.x + ima_w >= clip.x && wr.x < clip.x + clip.width)
        wallp->paint(g, ctx, wr);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void drawWallPaper(UGraph& g, UUpdateContext& ctx, 
                          const UIma* wallp, const UDimension& ima_dim,
                          const URect& r, const URect& clip) {
  float y;
  bool is_htiled = ctx.local.background ? ctx.local.background->isTiled() : true;
  bool is_vtiled = ctx.local.background ? ctx.local.background->isTiled() : true;
  
  if (!is_vtiled) {
    y = (r.height - ima_dim.height) / 2 + r.y;
    drawWallPaperLine(g, ctx, y, is_htiled, wallp, ima_dim.width, r, clip);
  }
  else {
    for (y = r.y; y < r.y + r.height; y += ima_dim.height)
      if (y + ima_dim.height >= clip.y && y < clip.y + clip.height) {
        drawWallPaperLine(g, ctx, y, is_htiled, wallp, ima_dim.width, r, clip);
      }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UViewUpdateImpl::updateBackground(UGraph& g, UUpdateContext& ctx, 
                                       const URect& r, const URect& clip) 
{
  if (!can_paint) return;

  // wallpaper a afficher (s'il existe)
  const UIma* wallp = null;
  UDimension ima_dim(0,0);

  // pixmap a afficher en wallpaper (si on le trouve!)
  if (ctx.local.background) {
    if (ctx.local.background->getColor()) {
      ctx.bgcolor = ctx.local.background->getColor();
    }

    wallp = ctx.local.background->getIma();
    if (wallp) {
      // NB: getSize charge l'image si pas deja fait
      wallp->getSize(ctx, ima_dim);

      // n'afficher que si on a vraiment reussi a charger l'image
      // !att: ne pas afficher UnknownPixmap en background en cas d'echec!
       if (!wallp->isRealized() || ima_dim.width <= 0 || ima_dim.height <= 0)
         wallp = null;  // inutilisable
    }
  }

  if (ctx.local.alpha == 0.
      // pas de wallpaper (ou pas trouve) et pas de bgcolor
      || (!wallp && ctx.bgcolor->equals(UColor::none))) {
    //completement transparent: on n'affiche rien
  }
  
  else {
    if (wallp) { // il y a un wallpaper
      //DAX bool blend_paint = (ctx.local.alpha == 1.) ? false : true;
      
      // nb: reafficher clip et non r qui va deborder!!!
      // if (blend_paint) g.beginBlend(clip, ctx.local.alpha);     &&&!!!!! A REVOIR !!!!&&&&
      
      drawWallPaper(g, ctx, wallp, ima_dim, r, clip);
      
      // terminer mode BLEND (=> merging des 2 plans)
      // if (blend_paint) g.endBlend();                              &&&!!!!! A REVOIR !!!!&&&&
    }
    
    else {  // uniform bgcolor
      //if (ctx.bgcolor) g.fillBackground(clip, *ctx.bgcolor, ctx.local.alpha);
      if (ctx.bgcolor) {
        if (ctx.local.alpha == 1.) g.setColor(*ctx.bgcolor);
        else g.setColor(*ctx.bgcolor, ctx.local.alpha);
        
        const URoundBorder* rb = null;
        if (ctx.local.border && (rb = dynamic_cast<const URoundBorder*>(ctx.local.border)))
          g.fillRoundRect(clip.x, clip.y, clip.width, clip.height, 
                          rb->getArcWidth(), rb->getArcHeight());
        else
          g.fillRect(clip.x, clip.y, clip.width, clip.height);
      }
    }
  }
}

// ==================================================== [Ubit Toolkit] =========

void UViewUpdateImpl::setPadding(UGraph& g, const UUpdateContext& ctx,
                                 const URect& r, bool add_frame_and_padding) {
  // padding doit il etre conserve dans vd?
  pad.left = pad.right = pad.top = pad.bottom = 0;
   
  if (ctx.local.border) {
    ctx.local.border->getSize(ctx, pad);    // add border decoration 

    // add border frame datas
    UViewBorderProp* vb = null;
    if (add_frame_and_padding && view->getProp(vb) && !ctx.local.border->isOverlaid())
      pad.add(*vb);
  }
 
  // add padding between border and content
  if (add_frame_and_padding) pad.add(ctx.local.padding);
  
  // !vd.pad ne sert plus apres ds la nvlle version => le virer de vd
  
  // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

  x = r.x + pad.left.val;
  y = r.y + pad.top.val;
  width  = r.width - pad.left.val - pad.right.val;
  height = r.height - pad.top.val - pad.bottom.val;
}

// ==================================================== [Ubit Toolkit] =========
  
void UViewUpdateImpl::callPaintCallbacks(UElem& grp, UUpdateContext& cur_ctx) {
  // the view has been destructed in the meanwhile: may happen if 
  // paint or resize callbacks destroys objects or views
  if (view->hasVMode(UView::DESTRUCTED)) return;
  
  if (grp.hasCallback(UOn::VIEW_PAINT_CB)) {
    UPaintEvent e(UOn::paint, view, &chclip);
    e.setContext(cur_ctx);
    grp.fire(e);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UViewUpdateImpl::callMoveResizeCallbacks(UElem& grp, UUpdateContext& cur_ctx) {
  if (view->hasVMode(UView::DESTRUCTED)) return;   // see above
  
  if (grp.hasCallback(UOn::VIEW_CHANGE_CB)) {
    UViewChangeCallbackProp* ps = null;
    view->obtainProp(ps);
    
    if (ps->pos.x != view->x || ps->pos.y != view->y) {
      UViewEvent e(UOn::motion, view);
      ps->pos.x = view->x;
      ps->pos.y = view->y;
      // fire() doit etre APRES modif x/y sinon boucle infinie
      // si le callback change lui meme la taille (car rappel de cette
      // meme fonction et old jamais modifie...)
      grp.fire(e);
    }
    
    if (ps->size.width != view->width || ps->size.height != view->height) {
      UResizeEvent e(UOn::resize, view);
      ps->size.width  = view->width;
      ps->size.height = view->height;
      // fire() doit etre APRES modif width/height sinon boucle infinie
      // si le callback change lui meme la taille (car rappel de cette
      // meme fonction et old jamais modifie...)
      grp.fire(e);
    }
  }
}
  
// ==================================================== [Ubit Toolkit] =========

UViewUpdateImpl::UViewUpdateImpl(UView *v, const URect &r, UViewUpdate &_upmode) :
pad(0,0),                 // !! ??? pas est-il vraiment utile ??? !!!
upmode(_upmode)
{
  view = v;
  hflex_space = vflex_space = 0;
  can_paint = false;
  edit = null;
  tabview = dynamic_cast<UTableView*>(v);   // si c'est une table:
  if (tabview) {tabview->ccur = tabview->lcur = 0;}

  // necessaire pour viewport qui calcule le max des tailles
  chr.width = chr.height = 0;
  
  if (!view->hasVMode(UView::INITIALIZED)) {
    if (!view->hasVMode(UView::INITIALIZING))
      view->addVModes(UView::INITIALIZING);
    else {
      view->removeVModes(UView::INITIALIZING);
      view->addVModes(UView::INITIALIZED);
    }
  }

  //if (vup.getMode() == UViewUpdate::PAINT_CHANGED) {
  //  //att aux floatings: pas encore deplaces ?
  //  if (v->x == r.x && v->y == r.y
  //	&& v->width == r.width && v->height == r.height)
  //    return;  //nothing changed
  //  else can_paint = true;
  // }
  // if (v->width != r.width || v->height != r.height) size_changed = true;
  //  else size_changed = false;

   // ATT: on peut pas init les coords. ici a cause des floatings 
   // dont les coords. dependent de ctx.

  switch (upmode.mode) {
  case UViewUpdate::PAINT_ALL:
    can_paint = true;
    // ca ne sert a rien de gerer les damaged et de detecter l'etat
    // "after painting all damaged" puisque de toute facon on repaint
    // tous les layers
    break;

  case UViewUpdate::PAINT_DAMAGED:
    if (view->hasVMode(UView::DAMAGED)) {
      can_paint = true;
      // on incremente le nombre de vues damaged traversees
      upmode.damaged_level++;
    }

    else if (upmode.damaged_level > 0)
      // faut afficher si on est dans un descendant d'un damaged
      can_paint = true;

   else if (upmode.after_damaged) 
     // ou bien si on est "after painting all damaged"
     // seuls les Floatings doivent etre reaffiches dans ce cas 
     // can_paint = true;  (code ah hoc dans update2() pour Floatings)
    break;

  case UViewUpdate::UPDATE_DATA:
    if (view->hasVMode(UView::DAMAGED)) {
      // on incremente le nombre de vues damaged traversees
      upmode.damaged_level++;
    }
    break;

  default:
    // NB: can_paint reste a false pour les DATA_OPS
    break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UViewUpdateImpl::~UViewUpdateImpl() {
  switch (upmode.mode) {
    case UViewUpdate::PAINT_ALL:
      view->removeVModes(UView::DAMAGED);
      // ca ne sert a rien de gerer les damaged (voir constructeur)
      break;
      
    case UViewUpdate::PAINT_DAMAGED:
    case UViewUpdate::UPDATE_DATA:
      
      if (view->hasVMode(UView::DAMAGED)) {
        view->removeVModes(UView::DAMAGED);
        // on decremente le nombre de vues damaged traversees
        upmode.damaged_level--;
        
        if (upmode.damaged_level == 0) {
          // on passe dans l'etat "after painting all damaged": ca signifie
          // qu'il faudra repaindre toutes les vues (en partculier les floatings)
          // a afficher dans la region de clipping
          upmode.after_damaged = true; 
        }
        else if (upmode.damaged_level < 0)
          UAppli::internalError("~UViewUpdateImpl","unbalanced damaged_level");
      }
      break;
      
    default:
      break;
  }
}

// ==================================================== [Ubit Toolkit] =========

static void nextH(UViewUpdateImpl&, const UUpdateContext&);
static void nextV(UViewUpdateImpl&, const UUpdateContext&);
static void nextViewport(UViewUpdateImpl&, const UUpdateContext&);
static void layoutBorder(UViewUpdateImpl& vd, const UUpdateContext&, UChildIter,
                         const UDimension&, UView* view);
static void nextBorder(UViewUpdateImpl&, const UUpdateContext&);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::doUpdate(UUpdateContext& parctx, URect r, URect clip, UViewUpdate& vup) {
  UBox* box = getBox();
  if (!box) {UAppli::internalError("UView::doUpdate", "Null box!"); return ;}

  //if (!hasVMode(UView::FORCE_POS)) {
  if (!box->isFloating()) {
    // toujours initialiser les coords meme si objet cache par autre chose,
    // sinon des objets theoriquement non visibles vont se retrouver dans 
    // une zone visible car leurs coords x,y vaudront 0,0 faute d'avoir
    // ete initialisees (en part. s'il y a du clipping avec des scrollpane)
    this->setRect(r);

    // data is not visible because of scrolling, etc... 
    // l'intersection de clip et de r est affectee dans clip
    if (clip.doIntersection(r) == 0) return;
  }

  UViewUpdateImpl vd(this, r, vup);
  UUpdateContext ctx(parctx, box, this,  &vd);
  doUpdate2(vd, *box, ctx, r, clip, vup);

  // NB: finalisation par destructeur de UViewUpdateImpl
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 
bool UView::updatePos(UViewUpdateImpl& vd, UElem& grp, UUpdateContext& ctx,
                      URect& r, URect& clip, UViewUpdate& vup) 
{
  UPos* pos = ctx.pos;
    
  // U3Dpos is a special case: just check we are in 3D mode and return
  if (pos && pos->is3Dpos()) {
    UGraph* g = ctx.getGraph();   // g can be null!
    if (g && !g->in_3d_mode)
      UAppli::error("UView::updatePos","widgets having a U3Dpos must be in a U3Dcanvas");
    return true;
  }
    
  // rajouter d'eventuelles coords relatives ajoutees par UPos
  // (cas UPos, mais PAS en mode search!)

  if (vup.mode < UViewUpdate::FIND_DATA_POS) {
 
    if (!ctx.parent_ctx || !ctx.parent_ctx->view_impl) {
      UAppli::error("UView::updatePos","UPos ignored because this widget has no parent");
      return false;
    }
    
    //float par_scale = ctx.parent_ctx ? ctx.parent_ctx->xyscale : 1.;

    r.x += //par_scale * 
    pos->x.toPixels(getDisp(), ctx.fontdesc, vd.view->width, 
                    ctx.parent_ctx->view_impl->width);
    r.y += //par_scale * 
    pos->y.toPixels(getDisp(), ctx.fontdesc, vd.view->height, 
                    ctx.parent_ctx->view_impl->height);
    /*
    cerr
    << " view " << this << " box " << getBox() << " cname " << getBox()->getClassName()
    << endl
    << " x "<< r.x 
    << " pos.x "<< pos->x.val
    << " par_scale " << par_scale
    << endl
    << " view->width "<< vd.view->width
    << " parview->width "<< ctx.parent_ctx->view->width
    << " parviewImpl->width "<< ctx.parent_ctx->view_impl->width
    << endl
    << endl;    
    */
    
    x = r.x;
    y = r.y;
    ctx.pos = null; // eviter possible recursion infinie (a cause de content)
    
    // specifie que les coords ont ete mises a jour
    removeVModes(UView::POS_HAS_CHANGED);
  }

  if (clip.doIntersection(r) == 0) return false;

  // on est dans l'etat "after painting all damaged": ca signifie qu'il
  // faut traiter a part les floatings car ils peuvent recouvrir les
  // damaged precedemment affiches
  // NB: ne rien faire de special si damaged_level > 0 (on est deja
  // a l'interieur d'un damaged, donc on repaint de tt facon)

  if (vup.damaged_level == 0 && vup.after_damaged) {
    switch (vup.mode) {
      case UViewUpdate::PAINT_DAMAGED:
        vd.can_paint = true;
        vup.above_damaged_count++;
        break;

      case UViewUpdate::UPDATE_DATA:
        vup.above_damaged_count++;
        break;

      default:	// rien a faire dans les autres cas
        break;
    }
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - mode SearchData: juste recuperer l'data et sa position sans redessiner
//   !ATT il faut IMPERATIVEMENT data_props != null dans le mode SearchData !
// - clip est passe en valeur, pas r
// - noter que coords de r et views sont != dans le cas des UWIn INBOX

void UView::doUpdate2(UViewUpdateImpl& vd, UElem& grp, UUpdateContext& ctx,
                      URect& r, URect& clip, UViewUpdate& vup) {
  UMultiList mlist(ctx, grp);
  
  bool is_border = grp.getDisplayType() == UElem::BORDER;
  UGraph* g = ctx.getGraph();   // g can be null !
  bool is_pane = false;

  if (ctx.xyscale != 1.) ctx.rescale();
  scale = ctx.xyscale;

  if (grp.isFloating()) {
    if (!updatePos(vd, grp, ctx, r, clip, vup)) return;
  }
  
  const USizeSpec& size = ctx.local.size;
  
  if (size.width.unit==UPERCENT || size.width.unit==UPERCENT_CTR)
    width = vd.width = 
    size.width.toPixels(ctx.getDisp(), ctx.fontdesc, vd.view->width, 
                        ctx.parent_ctx->view_impl->width);
  
  if (size.height.unit==UPERCENT || size.height.unit==UPERCENT_CTR)
    height = vd.height = 
    size.height.toPixels(ctx.getDisp(), ctx.fontdesc, vd.view->height, 
                         ctx.parent_ctx->view_impl->height);
  
  if (g && g->in_3d_mode) beginUpdate3d(vd, grp, ctx);
  
  if (grp.toBox()) {      // only for Boxes
    vd.edit = ctx.edit;
    is_pane = dynamic_cast<UPaneView*>(vd.view);
    
    if (g && grp.isSubWin() && vup.mode < UViewUpdate::UPDATE_DATA) {
      // subwindows must NOT be painted recursively:
      // - if the paint request is on a 1st level window, dont repaint its subwindows
      // - if the paint request is on a subwindow, it is repainted as hw==g->hardwin
      UHardwinImpl* hw = ((UWin*)&grp)->getHardwin(g->disp);
      if (!hw || hw != g->hardwin) vd.can_paint = false;
    }
    //else
    if (vd.can_paint) {
      // setClip() can only be called if can_paint is true (could seg fault otherwise)
      g->setHardwinClip(clip);
    }
    
    // orientation should not be taken into account in UElems
    vd.orient = grp.isVertical() ? UOrient::VERTICAL : UOrient::HORIZONTAL;

    vd.updateBackground(*g, ctx, r, clip);

    // padding normal qui prend en compte le border frame
    // initializes vd.x, vd.y, vd.width, vd.height
    vd.setPadding(*g, ctx, r, true);
    
    // clipping limits (NB: les bords sont exclus !)
    vd.chclip.setRect(vd.x, vd.y, vd.width, vd.height);
  
    // (mars06) paint & resize callbacks must be called after the painting of
    // the background and before the painting of the children
    // - appele que pour les boxes
    // - chclip doit etre initialise car les callbacks en dependent
    // - faut-il appeler si inter(clip) == 0 ?
    
    // notice: resizeCB callbacks must be called when vd.can_paint = false
    if (grp.hasCallback(UOn::VIEW_CHANGE_CB)) 
      vd.callMoveResizeCallbacks(grp, ctx);

    if (vd.can_paint && grp.hasCallback(UOn::VIEW_PAINT_CB)) 
      vd.callPaintCallbacks(grp, ctx);
    
    // dont attempt to display children if they are outside of the clipping area
    if (vd.chclip.doIntersection(clip) == 0) goto END;

    if (is_pane) 
      initLayoutViewport(vd, ctx, r);
    else if (vd.orient == UOrient::HORIZONTAL) 
      initLayoutH(vd, ctx, r);
    else 
      initLayoutV(vd, ctx, r);
  } // endif (grp.boxCast())

  // if 'content' != null the objects it contains are added to children
  // (this is for instance used for adding list-item markers and checkboxes)

  if (ctx.local.content) {
    UElem* content = ctx.local.content; // same ctx, same vd
    ctx.local.content = null;	// avoid infinite recursion
    doUpdate2(vd, *content, ctx, r, clip, vup);
  }

  for (UChildIter ch = mlist.begin(); ch != mlist.end(); mlist.next(ch))
    if (!ch.getCond() || ch.getCond()->verifies(ctx, grp)) {

      UNode* b = *ch;
      UData* data = null; 
      UElem* chgrp = null;
      UBox* chbox = null;
      UView* chview = null;
      UDimension dim(0,0);
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // attribute

      if (b->toAttr()) {
        b->toAttr()->putProp(&ctx, grp);
      }
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // data object

      else if ((data = b->toData())) {
        data->getSize(ctx, dim);
        if (is_border)
          layoutBorder(vd, ctx, ch, dim, null);
        else if (is_pane)
          layoutViewport(vd, ctx, ch, dim, null);
        else if (vd.orient == UOrient::HORIZONTAL) 
          layoutH(vd, ctx, ch, dim, null, null);
        else 
          layoutV(vd, ctx, ch, dim, null, null);
        
        if (vd.chr.width > 0 && vd.chr.height > 0) {
          
          //NB: ITEM_OPS => vd.can_paint == false mais pas l'inverse !
          if (vd.can_paint) {
            if ((vd.orient == UOrient::HORIZONTAL
                 && vd.chr.x + vd.chr.width > vd.chclip.x
                 && vd.chr.x < vd.chclip.x + vd.chclip.width)
                ||
                (vd.orient == UOrient::VERTICAL
                 && vd.chr.y + vd.chr.height > vd.chclip.y
                 && vd.chr.y < vd.chclip.y + vd.chclip.height)
                ) {
              g->setHardwinClip(vd.chclip);
              data->paint(*g, ctx, vd.chr);
              
              if (vd.edit && vd.edit->getCaretStr() == data) {
                // nb: danger: ne pas ecraser le curseur en reaffichant
                // l'data suivant par dessus
                vd.edit->paint(*g, ctx, vd.chr);
              }
            }
          } //endif(can_paint)
          
          // cas UPDATE_DATA: calculer position du cursor pour shifter
          // correctement les children au PAINT suivant
          else if (vup.mode == UViewUpdate::UPDATE_DATA) {
            if (vd.edit && vd.edit->getCaretStr() == data) {
              float xpos = vd.edit->getXpos(ctx, vd.chr);  // ex int
              
              if (xpos > vd.x + vd.width -1) {
                edit_shift += (vd.x + vd.width) - (xpos + 1);
              }
              else if (xpos < vd.x) {
                edit_shift += vd.x - xpos + 10;
                if (edit_shift > 0) edit_shift = 0;
              }
              else if (edit_shift < 0) {
                int caret_pos = -1;
                if (!vd.edit->getCaretStr(caret_pos)
                    ||(caret_pos == 0 
                       && !vd.edit->getPreviousStr(vd.edit->getCaretStr(), grp))
                    )
                  edit_shift = 0;
              }
            }
          }
          
          else if (vup.mode >= UViewUpdate::FIND_DATA_POS) {
            // do not draw, just find
            
            // !att: il faut que e->data_props soit =!null
            // on pourrait s'arreter de cherche qunad true est renvoye'
            // mais attention, il faut tout depiler dans ce cas (il peut
            // y avoir des UElem emboites suivis d'autres dataents
            
            if (vup.mode == UViewUpdate::FIND_DATA_POS) {
              if (vd.orient == UOrient::HORIZONTAL) {
                if (vd.view->findDataH(ctx, ch, mlist.end(), vd.chr, vup));
                // goto END; FAUX: car on saute l'increment qui
                // est necessaire si ubox( UElem(data1) + data2 ) 
              }
              else {
                if (vd.view->findDataV(ctx, ch, mlist.end(), vd.chr, vup));
              }
            }
            else if (vup.mode == UViewUpdate::FIND_DATA_PTR) {
              if (vd.view->findDataPtr(ctx, ch, mlist.end(), vd.chr, vup));
            }
          } 
          
          // increment 
          if (is_border) nextBorder(vd, ctx);
          else if (is_pane) nextViewport(vd, ctx);
          else if (vd.orient == UOrient::HORIZONTAL) nextH(vd, ctx);
          else nextV(vd, ctx);
            
       	} // endif (vd.chr.width > 0 && vd.chr.height > 0)	 
      } // endif dataCast()
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // UElem
      
      else if ((chgrp = b->toElem()) && !chgrp->toBox()) {  // element NOT box
        if (chgrp->isShowable()) {
          UUpdateContext chctx(ctx, chgrp, vd.view, &vd);   // own ctx, same vd
          
          if (vd.tabview  && dynamic_cast<UTrow*>(chgrp)) {  // tables
            // normalement lines[].d contient la valeur effective
            vd.height = vd.tabview->lines[vd.tabview->lcur].d;
            
            vd.tabview->ccur = 0;
            doUpdate2(vd, *chgrp, chctx, r, clip, vup);
            vd.chr.x = vd.x;
            vd.y += vd.height + ctx.vspacing;
            (vd.tabview->lcur)++;
          }
      	  else doUpdate2(vd, *chgrp, chctx, r, clip, vup);
        }
      }
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // UBox

      else if 
        (chgrp && (chbox = chgrp->toBox()) && chbox->isShowable()
         && 
         ((chbox->getDisplayType() == UElem::BLOCK   // boxes and subwins
          && (chview = (chbox)->getViewInImpl(vd.view, /*&ch.child(),*/ dim)))
          ||
           (mlist.in_softwin_list && chgrp->getDisplayType() == UElem::SOFTWIN
           && (chview = (chbox)->getViewInImpl(vd.view, /*null,*/ dim)))
          )
         ) {
          
        // 1) floating boxes (boxes that have an UPos)
        //if (chview->hasVMode(UView::FORCE_POS)) {
        if (chbox->isFloating()) {
          // fl_chr pour ne pas abimer vd.chr qui est cumulatif en x ou en y
          URect fl_chr;
          fl_chr.width  = dim.width;
          fl_chr.height = dim.height;
          // !att: on rajoute la MARGE: les coords sont locales 
          // % a l'INTERIEUR du CADRE et non % a l'origine de la Box.
          fl_chr.x = vd.x;
          fl_chr.y = vd.y;
          // coords relatives rajoutees ensuite via updatePos()
          chview->doUpdate(ctx, fl_chr, vd.chclip, vup);
        }

        // 2) all other boxes
        else {

          if (vd.tabview) {  // cas des tables
                             // tenir compte des rowspan des lignes precedentes
            if (vd.tabview->cols[vd.tabview->ccur].rowspan > 1) {
              // normalement cols[].d contient la valeur effective
              vd.chr.x += vd.tabview->cols[vd.tabview->ccur].d;
              
              (vd.tabview->cols[vd.tabview->ccur].rowspan)--;
              vd.tabview->ccur += vd.tabview->cols[vd.tabview->ccur].colspan;
            }
            
            // si UTcell tenir compte de colspan, rowspan
            UTcell* cell = dynamic_cast<UTcell*>(chbox);
            if (cell) {
              vd.tabview->cols[vd.tabview->ccur].colspan = cell->getColspan();
              vd.tabview->cols[vd.tabview->ccur].rowspan = cell->getRowspan();
              vd.tabview->ccur += cell->getColspan();
            }
            else vd.tabview->ccur++;
          }
          
          if (is_border)
            layoutBorder(vd, ctx, ch, dim, chview);
          else if (is_pane)
            layoutViewport(vd, ctx, ch, dim, chview);
          else if (vd.orient == UOrient::HORIZONTAL)
            layoutH(vd, ctx, ch, dim, chgrp, chview);
          else 
            layoutV(vd, ctx, ch, dim, chgrp, chview);
          
          if (vd.chr.width > 0 && vd.chr.height > 0) {
            chview->doUpdate(ctx, vd.chr, vd.chclip, vup);            
            if (is_border)
              nextBorder(vd, ctx);
            else if (is_pane) 
              nextViewport(vd, ctx);
            else if (vd.orient == UOrient::HORIZONTAL) 
              nextH(vd, ctx);
            else 
              nextV(vd, ctx);
          } 
        }
      }
    }

  // mis apres l'affichage du background et avant celui des enfants pour compat avec java     
  // if (vd.can_paint && grp->hasCallback(UMode::VIEW_PAINT_CB|UMode::VIEW_CHANGE_CB))
  //  vd.callbacks(grp, ctx);

 END:
  if (ctx.local.border) {
    // paint border decoration
    if (vd.can_paint) {
      g->setHardwinClip(clip);  // necessaire apres clip children

      // un hack pour appeler border->paint en coords locales
      float xx, yy;
      g->getWinOffset(xx,yy);
      g->setWinOffset(xx + r.x, yy + r.y);
      ctx.local.border->paint(*g, ctx, URect(0,0,r.width,r.height));
      g->setWinOffset(xx,yy);
    }
    
    // !! A GENERALISER
    
    // paint border frame datas
    // en fait faudrait dessiner les bords APRES (surtout si superposes!)
    UViewBorderProp* vb = null;
    UElem* chgrp = null;

    if ((chgrp = ctx.local.border->getSubGroup()) && vd.view->getProp(vb)) {
      // pad special qui ne prend en compte ni le border frame ni le padding interne
      vd.setPadding(*g, ctx, r, false);
      
      // NB: att au CLIP: doit etre celui de la view tt entiere !
      vd.chclip.setRect(vd.x, vd.y, vd.width, vd.height);
      if (vd.chclip.doIntersection(clip) != 0) {
        UUpdateContext chctx(ctx, chgrp, vd.view, &vd);
        doUpdate2(vd, *chgrp, chctx, r, clip, vup);
      }
    }
  }
      
  if (g && g->in_3d_mode) endUpdate3d(vd, grp, ctx);
}

// ==================================================== [Ubit Toolkit] =========

void UView::initLayoutH(UViewUpdateImpl& vd, const UUpdateContext& ctx, const URect& r) 
{
   // !!TOUTES CES OPTIONS DEVRAIENT ETRE RE-TESTEES POUR **CHAQUE** DATAENT!!
  vd.hflex_space = 0;

  // there are flexible objects => use all space
  if (vd.view->hflex_count > 0) {
    vd.chr.x = vd.x;
    // compute remaining space
    vd.hflex_space = (vd.width - vd.view->chwidth) / vd.view->hflex_count;
    vd.view->chwidth = vd.width;
  }
  
  else switch (ctx.halign) {
  case UHalign::LEFT:
  default:   // cas d'erreur: comme LEFT
    // NB: default se produit en particulier qd il y a une deconnade entre
    //  doLayout et doUpdate et que flexCount==0 avec UView::HFLEX
    // (ca vient du fait que toutes les options qui precedent devraient
    // etre retestees pour chaque element)
    vd.chr.x = vd.x + vd.view->edit_shift;  // edit_shift scrolle le texte edite
    break;
    
  case UHalign::CENTER:
    // ajouter espace restant a vd.x
    vd.chr.x = vd.x + (vd.width-vd.view->chwidth)/2 + vd.view->edit_shift;

    // si trop grand alors mettre en UHalign::LEFT pour que le debut
    // apparaisse et ne soit pas clipe (en part quand il y a du texte)
    if (vd.chr.x < vd.x && vd.view->edit_shift == 0) vd.chr.x = vd.x;
    break;

  case UHalign::RIGHT:     
    // ajouter espace restant a vd.x
    // reste centre (et clipe) si trop grand (peut deborder!)
    // a revoir: cf CENTER !!!
    vd.chr.x = vd.x + (vd.width-vd.view->chwidth) + vd.view->edit_shift;
    break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::initLayoutV(UViewUpdateImpl& vd, const UUpdateContext& ctx, const URect& r) 
{
  // !!TOUTES CES OPTIONS DEVRAIENT ETRE RE-TESTEES POUR **CHAQUE** DATAENT!!
  vd.vflex_space = 0;

  // there are flexible objects => use all space
  if (vd.view->vflex_count > 0)  {
    vd.chr.y = vd.y;
    // calcul espace restant
    vd.vflex_space = (vd.height - vd.view->chheight) / vd.view->vflex_count;
    vd.view->chheight = vd.height;
  }
  
  else switch (ctx.valign) {
  case UValign::TOP:
  default:  // cas d'erreur: comme TOP
    vd.chr.y = vd.y;
    break;
	
  case UValign::CENTER:
    // ajouter espace restant a vd.y
    vd.chr.y = vd.y + (vd.height-vd.view->chheight)/2;
    
    // si trop grand alors mettre en UHalign::TOP pour que le haut
    // apparaisse et ne soit pas clipe (en part quand il y a du texte)
    if (vd.chr.y < vd.y) vd.chr.y = vd.y;
    break;

  case UValign::BOTTOM:
    // ajouter espace restant a vd.y
    // reste centre (et clipe) si trop grand (peut deborder!)
    // a revoir: cf CENTER !!!
    vd.chr.y = vd.y + (vd.height-vd.view->chheight);
    break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::initLayoutViewport(UViewUpdateImpl& vd, const UUpdateContext& ctx, const URect& r) {
  UPaneView* paneview = dynamic_cast<UPaneView*>(vd.view);
  if (!paneview) {
    UAppli::internalError("initLayoutViewport","null UPaneView"); //fatal
    return;
  }
  paneview->setPadding(vd.pad);
}

// ==================================================== [Ubit Toolkit] =========

void UView::layoutH(UViewUpdateImpl& vd, const UUpdateContext& ctx, UChildIter link,
                    const UDimension& dim, UElem* chgrp, UView* chview) {
  char valign = ctx.valign;
  if (valign == UValign::FLEX) {
    // pour les Datas FLEX est identique a CENTER
    if (!chview) valign = UValign::CENTER;
    // pour les CANT_RESIZE_HEIGHT, FLEX est identique a TOP
    else if (!chgrp->isHeightResizable()) valign = UValign::TOP;
  }
  
  switch (valign) {
  case UValign::TOP:
    vd.chr.y = vd.y;
    vd.chr.height = dim.height;
    break;

  case UValign::FLEX:
    vd.chr.y = vd.y;
    vd.chr.height = vd.height;
    break;
    
  case UValign::CENTER:
    vd.chr.y = vd.y + (vd.height-dim.height)/2;    // ajouter moitie espace restant a vd.y
    vd.chr.height = dim.height;
    // si trop grand alors faire comme UHalign::TOP pour que le haut
    // apparaisse et ne soit pas clipe (en part quand il y a du texte)
    if (vd.chr.y < vd.y) vd.chr.y = vd.y;
    break;

  case UValign::BOTTOM:
    vd.chr.y = vd.y + (vd.height-dim.height);    // ajouter espace restant a vd.y
    vd.chr.height = dim.height;
    break;
  }
  
  // flexible horizontal object  ==>  add flexible width space
  // !!==> sauf pour les Datas qui sont jamais flexibles
  if (chview && ctx.halign == UHalign::FLEX && chgrp->isWidthResizable())
    vd.chr.width = dim.width + vd.hflex_space;
  else vd.chr.width = dim.width;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::layoutV(UViewUpdateImpl& vd, const UUpdateContext& ctx, UChildIter link,
                    const UDimension& dim, UElem* chgrp, UView* chview) {
  char halign = ctx.halign;
  if (halign == UHalign::FLEX) {
    // pour les Datas, FLEX est identique a CENTER
    if (!chview) halign = UHalign::CENTER;
    // pour CANT_RESIZE_WIDTH, FLEX est identique a LEFT (pas CENTER, incohrent ds qq cas)
    else if (!chgrp->isWidthResizable()) halign = UHalign::LEFT;
  }

  switch (halign) {
  case UHalign::LEFT:
    vd.chr.x = vd.x + vd.view->edit_shift;
    vd.chr.width = dim.width;
    break;
    
  case UHalign::FLEX:
    vd.chr.x = vd.x;
    vd.chr.width = vd.width;
    break;
    
  case UHalign::CENTER:
    vd.chr.x = vd.x + (vd.width-dim.width)/2;    // ajouter moitie espace restant a vd.x
    vd.chr.width = dim.width;
    // si trop grand alors faire comme UHalign::LEFT pour que le debut
    // apparaisse et ne soit pas clipe (en part quand il y a du texte)
    if (vd.chr.x < vd.x && vd.view->edit_shift == 0) vd.chr.x = vd.x;
    break;
    
  case UHalign::RIGHT:
    vd.chr.x = vd.x + (vd.width-dim.width);    // ajouter espace restant a vd.x
    vd.chr.width = dim.width;
    break; 
  }
  
  // flexible vertical object  ==>  add flexible height space
  // !!==> sauf pour les Datas qui sont jamais flexibles
  if (chview && ctx.valign == UValign::FLEX && chgrp->isHeightResizable()) 
    vd.chr.height = dim.height + vd.vflex_space;
  else vd.chr.height = dim.height;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UView::layoutViewport(UViewUpdateImpl& vd, const UUpdateContext& ctx,
                           UChildIter link, const UDimension& dim, UView* chview) {
  if (!chview) {
    UAppli::warning("UView::doUpdate","a UPane can only contain UBox(es) in its central area");
    return;
  }

  UPaneView* paneview = dynamic_cast<UPaneView*>(vd.view);
  if (!paneview) {
    UAppli::internalError("UView::doUpdate","null UPaneView"); //fatal
    return;
  }
  UScrollpane* pane = null;
  UCardbox* card = null;
  if (!(pane = dynamic_cast<UScrollpane*>(ctx.obj))
      && !(card =  dynamic_cast<UCardbox*>(ctx.obj))) {
    UAppli::internalError("UView::doUpdate","null UPane"); //fatal
    return;
  }
  
  vd.chr.x = vd.x - paneview->xscroll;
  vd.chr.y = vd.y - paneview->yscroll;

  // si scrollbar, child prend sa taille naturelle et ce scrollbar
  // permettra de faire defiler child. sinon child doit s'adapter
  // a la taille du pane comme si c'etait une box. de plus dans ce
  // denier cas, ca permet de superposer plusieurs children qui
  // seront superpose et de meme taille comme dans un cardbox
  
  if (pane && pane->getHScrollbar()) {
    float chw = dim.width > vd.width ? dim.width : vd.width;  // ex int
    vd.chr.width  = chw > vd.chr.width ? chw : vd.chr.width;
  }
  else {
    vd.chr.width = vd.width + paneview->xscroll;
  }

  if (pane && pane->getVScrollbar()) {
    float chh = dim.height > vd.height ? dim.height : vd.height;    // ex int
    vd.chr.height = chh > vd.chr.height ? chh : vd.chr.height;
  }
  else {
    vd.chr.height = vd.height + paneview->yscroll;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void layoutBorder(UViewUpdateImpl& vd, const UUpdateContext& ctx,
                         UChildIter link, const UDimension& dim, UView*) {
  UViewBorderProp* vb = null;
  if (!vd.view->getProp(vb)) {
    UAppli::internalError("UView::doUpdate","null frame view"); //fatal
    return;
  }
  UPaddingSpec& frame = *vb;

  // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

  switch (ctx.valign) {
  case UValign::TOP:
    vd.chr.y = vd.y;
    vd.chr.height = frame.top.val;
    break;
	    
  case UValign::BOTTOM:
    vd.chr.y = vd.y + vd.height - frame.bottom.val;
    vd.chr.height = frame.bottom.val;
    break;
	    
  case UValign::CENTER: // ne s'adapte PAS a la taille du Pane
  case UValign::FLEX: // Middle adaptatif: s'adapte a la taille du Pane
    vd.chr.y = vd.y + frame.top.val;
	    
#if EXX
    // !NOTE: pour les Datas, VFLEX est identique a MIDDLE
    if (!it && ctx.local.valign == VFLEX)
      vd.chr.height = r.height - frame.top.val - frame.bottom.val - pad.top - pad.bottom;
    else vd.chr.height = chview->h;
#endif

    //dans TOUS les cas (flex ou center/middle, Data ou Box) :
    // => TOUJOURS justifie
    vd.chr.height = vd.height - frame.top.val - frame.bottom.val;
    break;
  }
	  
  switch(ctx.halign) {
  case UHalign::LEFT:
    vd.chr.x = vd.x;
    vd.chr.width = frame.left.val;
    break;
    
  case UHalign::RIGHT:
    vd.chr.x = vd.x + vd.width - frame.right.val;
    vd.chr.width = frame.right.val;
    break;
	    
  case UHalign::CENTER: // ne s'adapte PAS a la taille du Pane
  case UHalign::FLEX: // Center adaptatif: s'adapte a la taille du Pane
    vd.chr.x = vd.x + frame.left.val;

#if EXX
    // !NOTE: pour les Datas, FLEX est identique a CENTER
    if (!it && ctx.local.halign == HFLEX)
      chr.width = r.width - frame.left - frame.right - pad.left - pad.right;	 
    else chr.width = chview->w;
#endif

    //dans TOUS les cas (flex ou center/middle, Data ou Box) :
    // => TOUJOURS justifie
    vd.chr.width = vd.width - frame.left.val - frame.right.val;
    break;
  }
}

// ==================================================== [Ubit Toolkit] =========

static void nextH(UViewUpdateImpl& vd, const UUpdateContext& ctx) {
  // increment vd.chr.x in all cases
  vd.chr.x += vd.chr.width + ctx.hspacing;
}

static void nextV(UViewUpdateImpl& vd, const UUpdateContext& ctx) {
  // increment vd.chr.y in all cases
  vd.chr.y += vd.chr.height + ctx.vspacing;
}

static void nextViewport(UViewUpdateImpl& vd, const UUpdateContext& ctx) {
  //NB: les children s'affiche les uns sur les autres (ce qui a un sens
  // s'ils sont transparents ou si on veut faire un cardbox
}

static void nextBorder(UViewUpdateImpl& vd, const UUpdateContext& ctx) {
  // nop
}

}

