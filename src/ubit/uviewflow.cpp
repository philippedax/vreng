/************************************************************************
 *
 *  uflowview.cpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <ubit/udefs.hpp>
#include <ubit/ucond.hpp>
#include <ubit/uedit.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uview.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/ufontmetrics.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uon.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

#define WIDTH_HINT   20  // A_REVOIR
#define LINE_QUANTUM 20
#define CELL_QUANTUM 25

class UFlowCell {
public:
  UChild* link;
  int line;
  int offset, len;
  float w, h;
};

class UFlowLine {
public:
  float w, h;
  short hflexChildCount;
  bool empty;
};

/* ==================================================== [Elc] ======= */

UViewStyle UFlowView::style(&UFlowView::createView, UObject::UCONST);

UFlowView::UFlowView(UBox* box, UView* par_view, UHardwinImpl* wgraph) 
: UView(box, par_view, wgraph) {
  lines = null;
  cells = null;
  line_count = cell_count = 0;
  alloc_line_count = alloc_cell_count = 0;
}

// "static" constructor used by UViewStyle to make a new view
UView* UFlowView::createView(UBox* box, UView* parview, UHardwinImpl* wgraph){
  return new UFlowView(box, parview, wgraph);
}

UFlowView::~UFlowView() {
  if (lines) {free(lines); lines = null;}
  if (cells) {free(cells); cells = null;}
  alloc_line_count = 0;
  alloc_cell_count = 0;
  line_count = 0;
  cell_count = 0;
}

float UFlowView::getMaxWidth() const {
  float ww = 0; 
  for (int k = 0; k < line_count; ++k)
    ww += lines[k].w;
  return ww;
}

bool UFlowView::caretPosToXY(long _pos, int& _x, int& _y) const {
  long p = 0;
  long begline_p = 0;
  _x = _y = 0;
  
  for (int c = 0; c < cell_count; c++) {
    UFlowCell& cell = cells[c];
    
    if (cell.line > _y) {
      _y = cell.line;
      begline_p = p;
    }
    if (_pos >= p && _pos < p + cell.len) {
      _x = _pos - begline_p;
      return true;
    }
    else p += cell.len;
  }

  // out of range
  _x = _pos - begline_p;
  return false;
}

bool UFlowView::xyToCaretPos(int _x, int _y, long& _pos) const {
  _pos = 0;
  long cell_x = 0;
  
  for (int c = 0; c < cell_count; ++c) {
    UFlowCell& cell = cells[c];

    if (cell.line < _y) {
      _pos += cell.len;
    }
    else if (cell.line == _y) {
      if (_x >= cell_x && _x < cell_x + cell.len) {
        _pos += _x;   // la position (x,y) a effectivement ete trouvee
        return true;
      }
      else cell_x += cell.len;
    }
    else {    // cell.line > y
      // la position x est trop grande, mettre la valeur de pos qui correspond
      // au plus grand x possible pour cet y
      _pos += cell_x - 1;
      return false;
    }
  }
  return false;
}

/* ==================================================== ===== ======= */

class UFlowLayoutImpl : public UViewLayoutImpl {
public:
  UFlowCell *cell;
  UFlowLine *line;
  int l, c;
  float wlimit, line_maxw;   // ex int
  UFlowView* flowview;

  UFlowLayoutImpl(UFlowView *v);
  void addLine(UUpdateContext* ctx);
  void addCell(UUpdateContext* ctx, UChild*, float w, float h, int offset, int len);
};

/* ==================================================== ======== ======= */

UFlowLayoutImpl::UFlowLayoutImpl(UFlowView *v) : UViewLayoutImpl(v) {
  flowview = v;
  flowview->lastline_strcell = -1; 
  wlimit = line_maxw = 0;
  c = -1; 
  l = -1;
  addLine(null); 
  // l vaut maintenant 0 et l[0] est initialise
  //!! faudra rajouter au addLine a la fin
}

/* ==================================================== ======== ======= */

void UFlowLayoutImpl::addLine(UUpdateContext*ctx) {
  // NB: pas forcement null : peut avoir ete alloue lors d'un precedent 
  // appel de Layout()
  if (!flowview->lines) {
    flowview->alloc_line_count = LINE_QUANTUM;
    flowview->lines = (UFlowLine*) malloc(sizeof(UFlowLine)
                                          * flowview->alloc_line_count);
  }
  else if (l+1 >= flowview->alloc_line_count) {
    flowview->alloc_line_count += LINE_QUANTUM;
    flowview->lines = (UFlowLine*) realloc(flowview->lines, sizeof(UFlowLine)
                                           * (flowview->alloc_line_count));
  }
  line = flowview->lines;

  // (sauf pour init)
  if (l >= 0) {
    // calculer la MAX taille de la ligne precedente a chaque chgt de ligne
    if (line[l].w > line_maxw) line_maxw = line[l].w;
    // add height of the last line (and add interline)
    //obs: flowview->favoriteHeight += ctx->local.vspacing + line[l].h;
    flowview->chheight += ctx->vspacing + line[l].h;
  }

  l++;
  line[l].empty = true;
  line[l].w = 0;
  line[l].h = 0;
  line[l].hflexChildCount = 0;
}

/* ==================================================== ======== ======= */

void UFlowLayoutImpl::addCell(UUpdateContext*ctx, UChild* _link, 
                              float _w, float _h, int _offset, int _len) {
  if (!flowview->cells) {
    flowview->alloc_cell_count = CELL_QUANTUM;
    flowview->cells = (UFlowCell*) malloc(sizeof(UFlowCell)
                                          * flowview->alloc_cell_count);
  }
  else if (c+1 >= flowview->alloc_cell_count) {
    flowview->alloc_cell_count += CELL_QUANTUM;
    flowview->cells = (UFlowCell*) realloc(flowview->cells, sizeof(UFlowCell)
                                           * (flowview->alloc_cell_count));
  }
  cell = flowview->cells;

  c++;
  cell[c].link = _link;
  cell[c].offset = _offset;
  cell[c].len = _len;
  // les cell de largeurs 0 (typiquement les lignes vides)
  // font merder la detection et l'affichage du caret
  cell[c].w = _w > 0 ? _w : 1; 
  cell[c].h = _h;

  // cell points to current line
  cell[c].line = l;  

  // number of horizontally "flex"ible child objects
  if (ctx->halign == UHalign::FLEX) line[l].hflexChildCount++;

  // line height is the max of all cell heights
  if (cell[c].h > line[l].h)  line[l].h = cell[c].h;

  // update line width
  line[l].w += cell[c].w;

  // add horizontal spacing to separe this cell from previous cell 
  if (line[l].empty) line[l].empty = false;
  else line[l].w += ctx->hspacing;
}

/* ==================================================== [Elc] ======= */
// att: arg = parctx = PARENT context !

bool UFlowView::doLayout(UUpdateContext& parctx, UViewLayout& vl) {
  UFlowLayoutImpl vd(this);
  UBox* box = getBox();
  if (!box) {UAppli::internalError("UFlowView::doLayout","null box!");return false;}
  
  UUpdateContext ctx(parctx, box, this, null);
  UMultiList mlist(ctx, *box);
  if (ctx.xyscale != 1.) ctx.rescale();
  scale = ctx.xyscale;

  UPaddingSpec pad(0, 0);   // Border and Box size

  if (ctx.local.border) {                     // !! A COMPLETER !!
    ctx.local.border->getSize(ctx, pad);
  }

  // value >=0 means: "fixed size hint" / should be >= 0 in this case
  /*
   // 28sept08: sert a quoi? pose pbm avec les tables
  if (vl.strategy == UViewLayout::IMPOSE_WIDTH) {
    //imposer la taille donnee par parent;
    width = vl.spec_w;  // ctx.local.width modifie dans ComputeWidth()
  }
   */
  
  // le pbm est qu'un flow n'a pas de taille intrinseque. on prend
  // donc une base de WIDTH_HINT mais en pratique il faudra
  // rendre le flow 'boxIsHFlex' pour qu'il s'adapte toujours
  // a la taille de son parent

  // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

  vd.wlimit = (width > 0) ? width : WIDTH_HINT;
  vd.wlimit -= pad.left.val + pad.right.val
  + ctx.local.padding.left.val + ctx.local.padding.right.val;
  if (vd.wlimit < 0) vd.wlimit = 0; // securite 

  //calculer la MAX taille des la ligne : init a 0
  vd.line_maxw = 0;

  // IMPORTANT:
  // Height depend de Width pour les UFlowView
  // ceci impose de refaire une seconde fois le layout du parent
  // (sauf dans le cas ou Width est fixe a priori auquel cas Height
  // (peut directement etre determine des la premier passe)
  
  if (ctx.boxIsHFlex) vd.mustLayoutAgain = true;  // necessaire ??

  chwidth = vd.wlimit;
  chheight = 0;
   
  flowDoLayout(vd, *box, ctx, mlist);

  // se termine par newline: rajouter une cell
  if (vd.line[vd.l].empty && vd.c >= 0 
      && vd.cell[vd.c].link && (**vd.cell[vd.c].link)->toStr()) {

    UStr*s = (**vd.cell[vd.c].link)->toStr();
    int len = s ? s->length() : 0;
    vd.addCell(&ctx, vd.cell[vd.c].link, 
               2, // width: assez pour afficher le caret 
               vd.cell[vd.c].h,  // height: comme preced
               len, 0);
    // noter l'existence de cette cell pour affichage du caret
    // sur derniere line vide
    lastline_strcell = vd.c; 
  }

  // +1 car commence a -1
  cell_count = vd.c + 1;

  // il en faut un de plus pour calculer favoriteHeight/Width
  vd.addLine(&ctx);
  // enlever le dernier qui ne sert qu'aux calculs
  line_count = vd.l + 1 - 1;

  // !! BORDER A GENERALISER ICI !!

  vl.min_w = vl.max_w = 0;
  vl.min_h = vl.max_h = 0;

  for (int c= 0; c < cell_count; ++c) {
    vl.min_w = std::max(vl.min_w, cells[c].w);
    vl.max_w += cells[c].w;
    vl.min_h = std::max(vl.min_h, cells[c].h);
    vl.max_h += cells[c].h;      
  }
  
  vd.chwidth = vd.line_maxw;
  vd.computeWidth(ctx, pad, vl, true);
  
  vd.chheight = chheight;    // chheight deja initialise
  vd.computeHeight(ctx, pad, vl, true);
  return vd.mustLayoutAgain;  // true if must lay out again
}

/* ==================================================== ===== ======= */
//!! ctx pas parctx!

void UFlowView::flowDoLayout(UFlowLayoutImpl&vd, UElem& grp, 
                             UUpdateContext& ctx, UMultiList& mlist) {
  // if this group is not null (which generally is the case) the object
  // it contains are added to children for normal display
  // (can for instance be used for adding list-item markers, checkboxes...

  if (ctx.local.content) {
    UElem* content = ctx.local.content;     // pas de ctx, meme vd
    ctx.local.content = null;	// avoid infinite recursion
    UMultiList mlist2(ctx, *content);
    //nb: dont rescale: deja a la bonne taille !!
    flowDoLayout(vd, *content, ctx, mlist2);
  }

  bool no_str_found = true;

  for (UChildIter ch = mlist.begin(); ch != mlist.end(); mlist.next(ch))
    if (!ch.getCond() || ch.getCond()->verifies(ctx, grp)) {
      UNode* b = (*ch);
      UData* data = null;
      UElem* chgrp = null;
      UBox* chbox = null; 
      UView* chview = null;
      UViewLayout chvl; //att: reinit par constr.
      
      if (b->toAttr()) {
        b->toAttr()->putProp(&ctx, grp);
      }
      
      else if ((chgrp = b->toElem()) && !chgrp->toBox()) {
        if (chgrp->isShowable()) {
          UUpdateContext chcurp(ctx, chgrp, vd.view, null);   // own ctx, same vd
          UMultiList chmlist(chcurp, *chgrp);
          chcurp.rescale();
          flowDoLayout(vd, *chgrp, chcurp, chmlist);
        }
      }
      
      // UDatas + UBoxes
      else if ((data = b->toData()) 
               ||
               (chgrp && (chbox = chgrp->toBox()) && chgrp->isShowable() 
                && chbox->getDisplayType() == UElem::BLOCK
                && (chview = chbox->getViewInImpl(vd.view /*,&ch.child()*/)))
               ) {

        UStr* str = (data ? data->toStr() : null);
        if (str) {
          int offset = 0;
          do {
            //int subw, subh;
            UDimension subdim(0,0);
            int sublen = 0;
            int change_line = 0;

            str->getSize(ctx, subdim, 
                         vd.wlimit - vd.line[vd.l].w, // taille max dispo
                         offset, sublen, change_line);

            // substr contient NL ne tient pas: passer a la ligne suivante
            // (sauf si la ligne courante est vide, ce qui signifie que
            // substr est de tt facon trop grande et sera donc clippee)

            // ajouter cette cellule a la ligne si non vide
            // (sauf si c'est la premiere pour cas pas encore init)
            if (sublen > 0 || no_str_found) {
              no_str_found = false;
              vd.addCell(&ctx, &ch.child(), subdim.width, subdim.height, 
                         offset, sublen);
              offset += sublen;
            }
            
            if (change_line >= 2       // contient \n
                || (change_line > 0 && vd.line[vd.l].w > 0)) {
              vd.addLine(&ctx);
            }
            
            // sortir qunad tous les chars de la str ont ete pris en compte
          } while (offset < str->length());
        } // endif(str)
	
	
        // cas des enfants autre que UStr : sous-cas a)
        // -a- cas des floating
	
        else if (chbox && chbox->isFloating()) {
          vd.mustLayoutAgain |= chview->doLayout(ctx, chvl);
          // floatings pas additionnes pour calcul taille parent,  mais parent
          // doit etre au moins aussi large que le floating le plus grand
          if (chvl.dim.width > vd.chwidth) vd.chwidth = chvl.dim.width;
          if (chvl.dim.height > vd.chheight) vd.chheight = chvl.dim.height;  
        }
	
        // cas des enfants autre que UStr: sous-cas b)
        // -b- tout le reste excepte les floating
	
        else {
          //int ww=0, hh=0;
          UDimension dim(0,0);
          
          if (data) data->getSize(ctx, dim);
          else {
            chview->doLayout(ctx, chvl);
            dim.width = chview->getWidth();
            dim.height = chview->getHeight();

            // pour les UFlowView, ils doivent prendre toute la place disponible 
            // pour le parent, donc passage a la ligne avant si lines_maxw 
            // et taille = maxwidth !
	    
            if (chview->getViewStyle() == &UFlowView::style) {
              // fait passer a la ligne *AVANT* un UFlow imbrique
              if (vd.line[vd.l].w >0) vd.addLine(&ctx);
              
              chview->width = vd.wlimit;     // chview->w modifie!
              // pour que UView::computeWidth() fonctionne correctement
              chvl.strategy = UViewLayout::NESTED;
            }
            
            // recommencer
            chview->doLayout(ctx, chvl);
            
            if (chview->getViewStyle() == &UFlowView::style) {
              dim.width = chview->getWidth();
            }
            dim.height = chview->getHeight();  // a include dans accolade?
          } // end else(it)
          
          // si cet dataent ne tient pas sur la ligne courante:
          if (vd.line[vd.l].w + dim.width > vd.wlimit)  {
            // s'il tiendrait sur une ligne vide, alors passer a la ligne
            if (dim.width < vd.wlimit) vd.addLine(&ctx);

            // heurististique: s'il ne tient pas sur une ligne vide
            // mais que la ligne courante est pleine au 1/3, alors
            // passer a la ligne pour reduire les debordements
            else if (vd.line[vd.l].w > vd.wlimit / 3) vd.addLine(&ctx);
          }
          vd.addCell(&ctx, &ch.child(), dim.width, dim.height, 0, 0);
	  
          // fait passer a la ligne *APRES* un UFlow imbrique
          if (chview && chview->getViewStyle() == &UFlowView::style) vd.addLine(&ctx);
        }
      }
    } // endfor ( ; ch...)
}

/* ==================================================== [Elc] ======= */

class UFlowUpdateImpl : public UViewUpdateImpl {
public:
  UFlowLine *line;
  UFlowCell *cell;
  int l, c;
  float line_y;  // ex int
  bool newline;
  UFlowView* flowview;

  UFlowUpdateImpl(UFlowView* v, const URect& r, UViewUpdate& vup, 
                  UFlowLine* _lines, UFlowCell* _cells)
    : UViewUpdateImpl(v, r, vup) {
    flowview = v;
    line = _lines;
    cell = _cells;
    l = c = 0;
    line_y = 0; 
    newline = true;
  }
};

/* ==================================================== ======== ======= */
//NB: mode SearchData: juste recuperer l'data et sa position sans redessiner
//!ATT il faut IMPERATIVEMENT datactx != null dans le mode SearchData !
//NB: clip est passe en valeur, pas r
// parctx = parent context

void UFlowView::doUpdate(UUpdateContext& parctx, URect r, URect clip, UViewUpdate& vup) {
  if (!getBox()) {UAppli::internalError("UFlowView::doUpdate","null box!"); return;}
  UBox& box = *getBox();

  // test pas valable pour les FLOATING car leurs coords dependent de ctx
  // et sont donc changees apres creation et parsing de ce dernier

  //if (!hasVMode(UView::FORCE_POS)) {  // cf. UView::doUpdate
  if (!box.isFloating()) {
    this->setRect(r);
    if (clip.doIntersection(r) == 0) return;
  }

  // ex: faux: il faut poursuivre le calcul meme quand le contenu est vide
  // afin de pouvoir tenir compte des padding et borders
  // if (!lines || !cells || line_count <= 0 || cell_count <=0) 
  //  return;

  {
    UFlowUpdateImpl vd(this, r, vup, lines, cells);
    UUpdateContext ctx(parctx, &box, this, &vd);
    UMultiList mlist(ctx, box);
    UGraph* g = ctx.getGraph();  // g can be null !
    //bool g_end = false;
    vd.edit = ctx.edit;

    if (ctx.xyscale != 1.) ctx.rescale();
    scale = ctx.xyscale;

    //if (hasVMode(UView::FORCE_POS)) {
    if (box.isFloating()) {
      if (!updatePos(vd, box, ctx, r, clip, vup)) return;
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
    
    if (g && g->in_3d_mode) beginUpdate3d(vd, box, ctx);

    if (g && box.isSubWin() && vup.mode < UViewUpdate::UPDATE_DATA) {      
      // subwindows must NOT be painted recursively:
      // - if the paint request is on a 1st level window, dont repaint its subwindows
      // - if the paint request is on a subwindow, it is repainted as hw==g->hardwin
      UHardwinImpl* hw = ((UWin*)&box)->getHardwin(g->getDisp());
      if (!hw || hw != g->getHardwin()) vd.can_paint = false;
    }
    //else
    if (vd.can_paint) {
      // setClip() can only be called if can_paint is true (could seg fault otherwise)
      g->setHardwinClip(clip);
    }
    
    vd.chclip = clip;
    vd.updateBackground(*g, ctx, r, clip);

    // initializes vd.x, vd.y, vd.width, vd.height
    vd.setPadding(*g, ctx, r, true);

    // !! A COMPLETER !!!!!!!!!!!!!!!!

    //vd.line_y = r.y + vd.pad.top; // !!!
    vd.line_y = vd.y;
    
    // clipping limits (NB: les bords sont exclus !)
    vd.chclip.setRect(vd.x, vd.y, vd.width, vd.height);
    //ex: nb bugge on avait oublie 1 marge pour h, w
    //vd.chclip.set(r.x + vd.pad.left, r.y + vd.pad.top,
    //		  r.width - vd.pad.right, r.height - vd.pad.bottom);

    // (mars06) paint & resize callbacks must be called after the painting of
    // the background and before the painting of the children (ATT: que pour les UBox!)

    // 02aug06: att: les resizeCB doivent etre appeles quand vd.can_paint = false
    if (box.hasCallback(UOn::VIEW_CHANGE_CB)) 
      vd.callMoveResizeCallbacks(box, ctx);
    
    if (vd.can_paint && box.hasCallback(UOn::VIEW_PAINT_CB)) 
      vd.callPaintCallbacks(box, ctx);
    
    // pas la peine de chercher a afficher les enfants s'ils sont hors zone de clip
    if (vd.chclip.doIntersection(clip) != 0) {
      flowDoUpdate(vd, ctx, box, mlist, r, clip, vup);
    }

    //if (g_end) g->end();   // ne pas oublier!!
    if (g && g->in_3d_mode) endUpdate3d(vd, box, ctx);

  } //NB: finalisation par destructeur de UFlowUpdateImpl
}

/* ==================================================== [Elc] ======= */

void UFlowView::flowDoUpdate(UFlowUpdateImpl& vd, UUpdateContext& ctx, 
                             UElem& grp, UMultiList& mlist, 
                             const URect& r, URect& clip, UViewUpdate&vup) {
  UGraph& g = *ctx.getGraph();
  vd.hflex_space = 0;
  vd.vflex_space = 0;
  
  // if this group is not null (which generally is the case) the object
  // it contains are added to children for normal display
  // (can for instance be used for adding list-item markers, checkboxes...
  
  if (ctx.local.content) {
    UElem* content = ctx.local.content;     // same ctx, same vd
    ctx.local.content = null;	// avoid infinite recursion
    UMultiList mlist2(ctx, *content);
    //nb: dont rescale() : deja fait !!
    flowDoUpdate(vd, ctx, *content, mlist2, r, clip, vup);
  }
  
  for (UChildIter ch = mlist.begin(); ch != mlist.end(); mlist.next(ch))
    if (!ch.getCond() || ch.getCond()->verifies(ctx, grp)) {

      UNode* b = (*ch);
      UData* data = null; 
      UElem* chgrp = null;
      UBox* chbox = null;
      UView* chview = null;

      if (b->toAttr()) {
        b->toAttr()->putProp(&ctx, grp);
      }
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      
      else if ((chgrp = b->toElem()) && !chgrp->toBox()) {
        if (chgrp->isShowable()) {
          UUpdateContext chctx(ctx, chgrp, vd.view, &vd);    // own ctx, same vd
          UMultiList chmlist(chctx, *chgrp);
          chctx.rescale();
          flowDoUpdate(vd, chctx, *chgrp, chmlist, r, clip, vup);
        }
      }
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // UDatas + UBoxes, UWins
      
      else if ((data = b->toData())
               ||
               (chgrp && (chbox = chgrp->toBox()) && chgrp->isShowable() 
                && chbox->getDisplayType() == UElem::BLOCK
                && (chview = chbox->getViewInImpl(vd.view /*,&ch.child()*/)))
               ) {
        
        // 1::cas des Floating
        //if (chbox && chview->hasVMode(UView::FORCE_POS)) {
        if (chbox && chbox->isFloating()) {
          UDimension size = chview->getSize();
                    
          // !!!!!!  A COMPLETER !!! prendre en compte les Units du padding !!!!!!!

          //!att: on rajoute la MARGE: les coords sont locales 
          // % a l'INTERIEUR du CADRE et non % a l'origine de la Box.
          URect fl_chr(r.x + vd.pad.left.val, r.y + vd.pad.top.val, 
                       size.width, size.height);
                         
          // coords relatives rajoutees ensuite dans updateFloating
          chview->doUpdate(ctx, fl_chr, vd.chclip, vup);
        }
        
        // 2::tous les autres objets
        else {
          
          if (vd.c >= vd.flowview->cell_count
              // ce lien n'a pas de cell car il est vide
              || vd.cell[vd.c].link != &ch.child())
            continue;
          
          while (vd.c < vd.flowview->cell_count) {
            
            if (vd.newline) {
              vd.newline = false;
              vd.hflex_space = 0; // HALIGN
              
              switch (ctx.halign) {
                case UHalign::LEFT:
                  vd.chr.x = r.x + vd.pad.left.val;
                  break;
                  
                case UHalign::CENTER:
                  vd.chr.x = (r.width - vd.line[vd.l].w) / 2;
                  // box smaller than child ==> impose left pad
                  if (vd.chr.x < vd.pad.left.val)  vd.chr.x = vd.pad.left.val;
                    vd.chr.x += r.x;
                  break;
                  
                case UHalign::RIGHT: 
                  vd.chr.x = r.width - vd.line[vd.l].w - vd.pad.right.val;
                  // box smaller than child ==> impose left pad
                  if (vd.chr.x < vd.pad.left.val)  vd.chr.x = vd.pad.left.val;
                    vd.chr.x += r.x;
                  break;
                  
                default:
                  // cas d'erreur divers et aussi les HFLEX que l'on a avire
                  vd.chr.x = r.x + vd.pad.left.val;
                  break;
              }
            } // endif(newline)
            
            // VALIGN
            /*
             switch (ctx.local.valign) {
               case V_BOTTOM:
               case V_CENTER:
               case V_TOP:
               case V_FLEX:
                 */
            
            // !NOTE: pour les Datas FLEX == CENTER
            // Cas BOX et FLEX
            if (!data && ctx.valign == UValign::FLEX) {
              vd.chr.height = vd.line[vd.l].h;
              vd.chr.y = vd.line_y;
            }
            else {
              vd.chr.height = vd.cell[vd.c].h;
              vd.chr.y = vd.line_y;
            }
            
            // flexible horizontal object => add flexible width space
            if (ctx.halign == UHalign::FLEX)
              vd.chr.width = vd.cell[vd.c].w + vd.hflex_space;
            else vd.chr.width = vd.cell[vd.c].w;
            
            // completement faux: ne triuve pas les lignes vides
            // if (vd.chr.width > 0 && vd.chr.height > 0) {
            
            // pas un data => c'est forcement un Box
            if (!data) chview->doUpdate(ctx, vd.chr, vd.chclip, vup);
            else {
              // NB: DATA_OPS => vd.can_paint==false mais pas l'inverse!
              if (vd.can_paint) {
                
                if (vd.chr.y + vd.chr.height > vd.chclip.y
                    && vd.chr.y < vd.chclip.y + vd.chclip.height) {
                  
                  g.setHardwinClip(vd.chclip);
                  UStr* str = (data ? data->toStr() : null);
                  
                  if (!str) data->paint(g, ctx, vd.chr);
                  else {
                    str->paint(g, ctx, vd.chr,
                               vd.cell[vd.c].offset ,vd.cell[vd.c].len);
                    
                    //pbm subtil: 2 cas differents
                    //-1- newline (explicite par \n ou implicite par taille)
                    //    au milieu d'une UStr --> test pos_in_string <strict
                    //    sinon le caret s'affiche 2 fois (debut+fin de ligne)
                    //-2- newline implicite juste a la fin physique de la UStr
                    //    --> test >= sinon le caret va disparaitre
                    
                    int strpos = 0;
                    UFlowCell& cell = vd.cell[vd.c];
                    if (vd.edit
                        && vd.edit->getCaretStr(strpos) == (const UStr*)str
                        && strpos >= cell.offset
                        && (strpos < cell.offset + cell.len
                            || (vd.flowview->lastline_strcell > 0
                                && vd.flowview->lastline_strcell == vd.c)
                            || (strpos == cell.offset + cell.len
                                // en fin de str
                                && strpos == str->length()
                                // et str nulle/vide ou pas terminee par \n
                                && (str->length() < 1
                                    || str->c_str()[str->length()-1] != '\n')
                                )
                            )
                        ) {
                      // nb: danger: ne pas ecraser le curseur en reaffichant
                      // l'data suivant par dessus
                      vd.edit->paint(g, ctx, vd.chr, cell.offset, cell.len);
                    }
                  }
                }
              }
              
              else if (vup.mode >= UViewUpdate::FIND_DATA_POS) {
                // do not draw, just find Data
                if (vup.mode == UViewUpdate::FIND_DATA_POS) {
                  if (vd.flowview->flowFindDataPos(ctx, ch, mlist.end(),
						   &vd.cell[vd.c],vd.chr,vup))
                    return;
                }
                else if (vup.mode == UViewUpdate::FIND_DATA_PTR) {
                  if (vd.flowview->flowFindDataPtr(ctx, ch, mlist.end(),
						   &vd.cell[vd.c],vd.chr,vup))
                    return;
                }
              }
            }
            
            // increment vd.chr.x in all cases
            vd.chr.x += vd.chr.width + ctx.hspacing;
            
            // next cell
            vd.c++;
            
            // fin de liste
            if (vd.c >= vd.flowview->cell_count) break;
            
            // on a change de ligne
            if (vd.cell[vd.c].line > vd.cell[vd.c-1].line) {
              vd.newline = true;
              vd.line_y += vd.line[vd.l].h + ctx.vspacing;
              vd.l++;
            }
            
            // on est passe a l'data suivant dans la liste
            if (vd.cell[vd.c].link != &ch.child()) break;
            
            } // end while (vd.c < vd.flowview->cell_count) 
             }	  
          } // endif
        } // endfor
      
      //deplace (mars06) : voir uview.cpp
      //if(vd.can_paint &&grp->hasBMode(UMode::VIEW_PAINT_CB|UMode::VIEW_CHANGE_CB)) 
      // vd.callbacks(grp, ctx);
}

/* ==================================================== [Elc] ======= */
// retrieves data and info from x,y position
//!!ATT: 
// - suppose que datactx!= null !
// - return==true signifie: ne pas chercher davantage car:
//                trouve' OU pas trouve' mais plus la peine de chercher
// - return==false signifie: continuer a chercher car:
//                pas trouve' mais on peut encore trouver 
//

bool UFlowView::flowFindDataPos(UUpdateContext& ctx, UChildIter data_iter, UChildIter end_iter, 
				UFlowCell* cell, const URect& r, UViewUpdate& vup) 
{
  if (!vup.datactx) {
    UAppli::internalError("UFlowView::flowFindDataPos","null event or wrong type");
    return false;
  }
  const UPoint& evpos = vup.datactx->win_eventpos;
  
  if (r.y > evpos.y) return true;  // plus rien a chercher (not found)
  
  UData *data;
  UStr *str;
  
  if (evpos.y >= r.y && evpos.y <= r.y + r.height
      && ((data = (*data_iter)->toData()))) {
    
    if (r.x <= evpos.x) {
      int strpos = -1;
      
      if ((str = data->toStr())
          && &data_iter.child() == cell->link // petite verif
          && str->c_str()) {
        // search the strpos
        strpos = cell->offset 
        + UFontMetrics(ctx).getCharPos(str->c_str() + cell->offset, cell->len,
                                       evpos.x - r.x);
      }
      
      if (evpos.x <= r.x + r.width) { 
        //exact match: data exactly found -> stop searching
        vup.datactx->set(ctx, data, data_iter, end_iter, r, strpos, true);
        return true;
      }
      else { // data approximatively found -> continue seraching
        vup.datactx->set(ctx, data, data_iter, end_iter, r, strpos, false);
      }
    }
  }
  return false;	// continuer a chercher (plusieurs cas)
}


bool UFlowView::flowFindDataPtr(UUpdateContext& ctx, 
                                UChildIter data_iter, UChildIter end_iter, 
                                UFlowCell*cell, const URect&r, UViewUpdate&vup) {  
  UData *data;
  if ((vup.datactx->it == data_iter || vup.datactx->data == *data_iter)
      && ((data = (*data_iter)->toData()))
      ) {
    
    if (!data->toStr()) {
      // not an UStr: positions don't matter
      vup.datactx->set(ctx, data, data_iter, end_iter, r, vup.datactx->strpos, true);
      return true;
    }
    else {
      // inutile de continuer a chercher
      if (cell->offset > vup.datactx->strpos2) return true;
      
      if (cell->offset + cell->len >= vup.datactx->strpos) {
        vup.datactx->merge(ctx, data, data_iter, end_iter, r, true);
      }
      // pas de return true: continuer a chercher et a ajouter les regions:
      // il peut y avoir plusieurs cells concernes et on renverra la region globale
    }
  }
  
  //tous les cas pas trouve (PAS de else!)
  return false;
}

}
