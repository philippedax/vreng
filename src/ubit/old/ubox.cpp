/************************************************************************
 *
 *  ubox.cpp: Box Elements (elements that manage 1 or N screen UView(s) 
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
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uappliImpl.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UBox& uvbox(UArgs args) {
  //return *new UVbox(args);
  UBox* box = new UBox(args);
  box->setAttr(UOrient::vertical);
  return *box;
}

UBox& uhbox(UArgs args) {
  //return *new UHbox(args);
  UBox* box = new UBox(args);
  box->setAttr(UOrient::horizontal);
  return *box;
}

UBox::UBox(UArgs a) : views(null) {
  // faire add() APRES sinon la box est consideree comme un new UElem
  // (car on sera dans le constructeur de new UElem) ce qui ferait merder
  // l'addition de children qui necessitent une UBox comme parent
  add(a);
}

UStyle* UBox::createStyle() {
  UStyle* style = new UStyle();
  style->textSeparator = new UStr(" ");
  style->orient = UOrient::INHERIT;
  // abndonne: marche pas: a la fois hflex et vflex
  style->halign = UHalign::FLEX;
  style->valign = UValign::FLEX;
  style->hspacing = 1;
  style->vspacing = 1;
  style->local.padding.set(0,0);
  return style;
}

/* ==================================================== [Elc] ======= */
/*
 virtual UBox* getChildAt(const UPoint&, UView* box_view = null) const;
 * return the direct child at this location in this box view.
 * a box can control several views. Returns the child at this position
 * in the first view if 'box_view' = null
 
UBox* UBox::getChildAt(const UPoint& p, UView* view) const {
  if (!view) view = getView(0);
  if (!view) return null;
  return getChildAtImpl(UPoint(p.x + view->x, p.y + view->y), view);
}

virtual UBox* getChildAtImpl(const UPoint&, UView* box_view) const;

 UBox* UBox::getChildAtImpl(const UPoint& p, UView* view) const {
  //if (!view) return null;
  for (UChildReverseIter ch = crbegin(); ch != crend(); ++ch) {
    // pas possible: context inconnu
    // if (!ch.getCond() || ch.getCond()->verifies(curp, grp))     
    UBox* chbox = (*ch)->toBox();
    
    // elimine les UWin
    if (chbox && (chbox->getDisplayType() == BLOCK) && chbox->isShowable()) {
      for (UView* v = chbox->views; v != null; v = v->getNext()) {
        // !!! faudrait tester v->isShown()                                  !!!
        if (//v->getBoxHolder() == chnode &&     // 9nov08: obsolete
            v->getParentView() == view
            && p.x >= v->x && p.x < v->x + v->width
            && p.y >= v->y && p.y < v->y + v->height)
          return v->getBox();
      }
    }
  }
  return null;
}

virtual UDimension getViewSize(UView* box_view = null) const; 
 * returns the size of this box view.
 * a box can control several views. Returns the size of the first view if 
 * 'box_view' = null
 *
virtual UPoint getViewPos(UView* box_view = null) const;  
 * returns the position of the upper left corner of this box view.
 * a box can control several views. Returns the upper left corner of the
 *  first view if 'box_view' = null
 *
UPoint UBox::getViewPos(UView* view) const {
  if (!view) view = getView(0);
  if (view) return view->getPos();
  else return UPoint(0,0);
}

UDimension UBox::getViewSize(UView* view) const {
  if (!view) view = getView(0);
  if (view) return view->getSize(); 
  else return UDimension(0,0);
}
 */
/* ==================================================== [Elc] ======= */

// IL FAUDRAIT ORDONNER LES VIEWS!
void UBox::addViewImpl(UView* v) {
  v->setNext(views);
  views = v;
}

int UBox::getViewCount() const {
  int n = 0;
  for (UView* v = views; v != null; v = v->getNext()) n++;
  return n;
}

int UBox::getViews(std::vector<UView*>& tab) const {
  // NB: fait tab.clear() contrairement a retrieveViews()
  tab.clear();
  int count = 0;
  for (UView* v = views; v != null; v = v->getNext(), count++) {
    tab.push_back(v);
  }
  return count;
}

UView* UBox::getView(int pos) const {
  if (pos == 0) return views;
  else {
    int n = 0;
    UView* last_view = null;
    for (UView* v = views; v != null; v = v->next) {
      if (n == pos) return v;
      last_view = v;
      n++;    // no de la vue (pas du parent!)
    }
    
    if (pos == -1) return last_view;
    else return null;	// not found
  }
}

UView* UBox::getView(const UInputEvent& e) const {
  UView* eview = e.getView();
  if (!eview) return null;
  
  // this first test won't work if the event source is on a superimposed component
  // such as a menu displayed over this box
  UView* found_view = getViewContaining(*eview);
  if (found_view) return found_view;

  UMouseEvent* me = ((UInputEvent&)e).toMouseEvent();
  UView* winview = e.getWinView();
  if (!me || !winview) return null;
    
  UPoint pos_in_win = UView::convertPosTo(*winview, *eview, me->getPos());

  for (UView* v = views; v != null; v = v->next) {
    if (v->containsWC(pos_in_win)) return v;
  }
  
  return null;
}

UView* UBox::getView(UWin& win, const UPoint& pos_in_win, UDisp* d) const {
  UView* winview = win.getWinView(d);
  if (!winview) return null;
  
  for (UView* v = views; v != null; v = v->next) {
    // CF SHAPE!!!@@@ and IS_SHOWN !!!@@@
    if (v->getWinView() == winview && v->containsWC(pos_in_win)) return v;
  }
  return null;
}

/*
UView* UBox::getSubView(UView& view, const UPoint& pos_in_view) const {
  UView* winview = view.getWinView();
  if (!winview) return null;
  UPoint pos_in_win = UView::convertPosTo(*winview, view, pos_in_view);

  for (UView* v = views; v != null; v = v->next) {
    UViewFind vf(winview, pos_in_win, UBehavior::MOUSE, 0);
    UPoint source_pos;
    UView* found_view = v->findSource(vf, source_pos);
    if (found_view) return found_view;
  }
  return null;
}

UView* UBox::getSubView(UWin& win, const UPoint& win_pos, UDisp* d) const {
  UView* winview = win.getWinView(d);
  return winview ? getViewAt(*winview, win_pos) : null;
}

UView* UBox::getSubView(const UPoint& screen_pos, UDisp* d = null) const {
  UView* winview = UAppli::getWinView(screen_pos, d);
  if (!winview) return null;
  return getViewAt(winview, conversion);
}
*/
/* ==================================================== ===== ======= */

UView* UBox::getViewContaining(const UView& _view) const {
  for (UView* v = const_cast<UView*>(&_view); v!=null; v = v->getParentView()) {
    if (v->getBox() == this) return v;
  }
  return null;  // not found
}

UView* UBox::getFirstViewInside(const UView& parent_view) const {
  for (UView* v = views; v != null; v = v->next) {
    UView* pv = v->getParentView();
    if (!pv) ;
    else if (pv == &parent_view) return v;
    else if (pv->getBox()->getFirstViewInside(parent_view)) return v;
  }
  return null;	// not found
}

UView* UBox::getViewInImpl(UView* parview /*,UChild* cnode*/) {  
  for (UView* v = views; v != null; v = v->next) {
    if (v->getParentView() == parview) return v;
  }
  return null;	// not found
}


UView* UBox::getViewInImpl(UView* parview, /*UChild* cnode,*/ UDimension& dim) {
  UView* v = getViewInImpl(parview /*, cnode*/);
  if (v) {dim.width = v->width; dim.height = v->height;} 
  else dim.width = dim.height = 0;
  return v;
}

// ==================================================== [Ubit Toolkit] =========
/* debug
 void foo(UView* v) {
 for (; v ; v = v->getParentView()) {
 cerr << "--pview: " << v <<" box " << v->getBox() 
 << " "  << v->getBox() ->getTypeName() << endl;
 }
 }
 */

void UBox::setViewModes(int vmodes, bool state) {
  for (UView* v =getView(0); v != null; v = v->getNext()) {
    if (state) v->addVModes(vmodes); else v->removeVModes(vmodes);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// att: parview n'est pas la view du parent mais du 1er box!

void UBox::initView(UView* parview) {
  if (!parview) {
    error("UBox::initView","[internal error] object has no parent view");
    return;
  }
  
  const UViewStyle* render = null;
  if (emodes.HAS_LAYOUT) {
    // si renderer defini dynamiquement dans la childlist
    attributes().findClass(render);
    if (!render) children().findClass(render);
  }
  else {  // default: rendre le renderer defini par le style de la Win
    render = getStyle(null).viewStyle;
  }

  UView *view = null;
  if (render)
    view = (render->createView)(this, parview, parview->getHardwin());
  else {
    error("UBox::initView","the UStyle of this object could not be retreived");
    view = new UView(this, parview, parview->getHardwin());
  }
  
  if ((parview && parview->hasVMode(UView::NO_DOUBLE_BUFFER)))
    view->addVModes(UView::NO_DOUBLE_BUFFER);

  addViewImpl(view);
  
  // propager aux children
  UElem::initView(view);  // view, pas parview !
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//void UBox::initChildViews(UElem* child, UChild *childlink) {
void UBox::initChildViews(UElem* child) {
  // retrouver toutes les vues pour tous les parents de box
  for (UView* v = views; v != null; v = v->next) {
    if (v && v->isRealized())   // initialiser b et ses descendants
      // ? QUESTION: init pour tous ou que UElem/ubox ?
      //child->initView(childlink, v);
      child->initView(v);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// does the same as getViews() except that tab is not emptied

int UBox::retrieveRelatedViews(std::vector<UView*>& tab) const {
  int count = 0;
  for (UView* v = views; v != null; v = v->next, count++) {
    tab.push_back(v);
  }
  return count;
}

// parent_views may belong to several widgets, which are not necessarily
// the direct parents of this box

//void UBox::deleteRelatedViews(UView* parview, UChild* cnode) {
void UBox::deleteViewsInside(const vector<UView*>& parent_views) {
  vector<UView*> tab;  
  for (UView* v = views; v != null; v = v->next) {
    if (v->isChildOf(parent_views)) tab.push_back(v);
  }
  
  UElem::deleteViewsInside(tab); 
  
  UView *nextv = null, *prevv = null;
  for (UView* v = views; v!=null; v = nextv) {
    nextv = v->next;
    if (v->isChildOf(parent_views)) {
      if (prevv) prevv->next = nextv; else views = nextv;
      delete v;  // NB: delete est redefini
      v = null;
    }
    if (v) prevv = v;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NOTE: a la difference des UBox, les Views des UWin sont partagees sauf
// dans le cas ou elles appartiennent a des UDisp diffrents
// La regle de destruction est donc differente: on ne detruit les vues
// que si cette UWin est en cours de destruction (UMode::DESTRUCTING)
// ou si c'est une softwin qui depend d'une hardwin en cours de destr.

//void UWin::deleteRelatedViews(UView* parview, UChild* cnode) {
void UWin::deleteViewsInside(const std::vector<UView*>& parent_views) {
  vector<UView*> tab;
  UWin* hw = null;

  for (UView* v = views; v != null; v = v->next) {
    if (isHardwin()? omodes.IS_DESTRUCTING: ((hw=v->getWin()) && hw->omodes.IS_DESTRUCTING))
      tab.push_back(v);
  }
  
  UElem::deleteViewsInside(tab); 
  
  UView *nextv = null, *prevv = null;
  for (UView* v = views; v!=null; v = nextv) {
    nextv = v->next;
    if (isHardwin()? omodes.IS_DESTRUCTING: ((hw=v->getWin()) && hw->omodes.IS_DESTRUCTING)){
      if (prevv) prevv->next = nextv; else views = nextv;
      delete v;  // NB: delete est redefini
      v = null;
    }
    if (v) prevv = v;
  }
}

// ==================================================== [Ubit Toolkit] =========
// performs the actual update when the main loop is idle

void UBox::update(const UUpdate& upd, UDisp*) {
  if (UAppli::isExiting()) return;
  
  // do not update now, add to the update list
  UAppli::impl.addUpdateRequest(this, upd);

  /*
  if (UAppli::impl.isProcessingUpdateRequests()) {
    // do not update now, add to the update list
    UAppli::impl.addUpdateRequest(this, upd);
  }
  else if ((upd.modes & UUpdate::NO_DELAY) || UAppli::conf.usync) {
    // update objets already in the list, then proceed
    UAppli::impl.processUpdateRequests();
    doUpdate(upd, null);
  }
  else {
    // do not update now, add to the update list
    UAppli::impl.addUpdateRequest(this, upd);
  }
 */
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// performs the actual update now

void UBox::doUpdate(const UUpdate& upd, UDisp*) {
  if (UAppli::isExiting()) return;

  // update objets already in the list, then proceed
  if (!UAppli::impl.isProcessingUpdateRequests()) 
    UAppli::impl.processUpdateRequests();

  // REMARQUE IMPORTANTE: les hide/show ne sont pas traites de la meme facon
  // si l'objet est floating ou non:
  // - dans le 1er cas, pas besoin de recalculer le layout, un repaint suffit
  // - dans le 2nd cas, il faut recalculer le Layout - et actuellement -
  //   c'est meme celui DE LA FENETRE qu'on recalcule ce qui est A REVOir

  UWin* updated_win = null;
  for (UView* view = views; view != null; view = view->getNext()) {
    UWin*  hardwin = null;
    UView* hardwin_view = null;
    
    if (view && view->isRealized()   // check views!=null (some may have been deleted)
        && (hardwin = view->getWin())
        && (hardwin_view = hardwin->getWinView(view->getDisp()))
        // ne rien faire si la window n'est pas visible (sauf si always)
        // ce qui implique de ne pas appliquer directement cette version
        // de update() sur une window                           !!POSE PBM AVEC SOFTWINS!!
        && (hardwin->isShowable() || upd.isHiddenObjectsMode())
        ) {
      
      UPaintEvent e(UOn::paint, hardwin_view, null/*flow*/);
    
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // MOVE
      
      if (upd.modes & UUpdate::MOVE) {
        // on veut reafficher la zone minimale du parent de view
        // (= la reunion de l'ancienne et de la nouvelle position de view)
        e.setSourceAndProps(view);
        view->updatePaintData(&e.redraw_clip);
      }
      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // PAINT_ONLY (NO LAYOUT)
      
      else if ((upd.modes & UUpdate::PAINT) && !(upd.modes & UUpdate::LAYOUT)) {
        if (!isShowable() && !upd.isHiddenObjectsMode()) break;
        e.setSourceAndProps(view);
        
        // si e.getView() == null cette view n'est pas visible
        // a l'ecran actuellement car cachee par un ScrollPane, etc.
        if (!e.getView()) return;

        // si data est specifie, essayer de ne reafficher QUE la region
        // contenant elem (au lieu de la box entiere)
        // NB: data n'est pas necessairement situe sous la souris 
        // (ie. ce n'est pas toujours e->getData())

        if (!(upd.modes & UUpdate::STR_DATA)) {
          // NB: updateViewImpl utilise e.redrawClip et redrawStatus pour ne
          // reafficher que le clip concerne (ou juste upd.region si definie)
          if (!isShowable()) view->updatePaintData(&e.redraw_clip);
          else view->updatePaint(&e.redraw_clip);
        }
        else {
          UDataContext dc;
          const UUpdate::StrData& sd = upd.props.sd;

          if (sd.data && e.getView()) {
            // !ATT: ca marche pas vraiment car e devrait etre un MouseEvent
            // avec (xmouse,ymouse) correctement intialises. Mais c'est pas
            // completement inutile dans le cas des textarea avec des plusieurs
            // lignes: on ne reaffichera que cette ligne (mais en entier)
            UData* d = e.getView()->findData(dc, UPoint(1,1), sd.data, sd.pos1, sd.pos2);
            
            // NB: on modifie sd->region alors que upd est cense etre const
            // ceci dit dans ce cas c'est pas un pbm car il est jamais const
            if (d && d == sd.data) sd.region = &dc.win_clip;
          }
          if (!isShowable()) view->updatePaintData(sd.region);
          else if (upd.props.sd.region) view->updatePaint(sd.region);
        }
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // SHOW/HIDE
      
      else if (upd.modes & (UUpdate::SHOW | UUpdate::HIDE)) {
        if (upd.modes & UUpdate::SHOW) emodes.IS_SHOWABLE = true;
        else emodes.IS_SHOWABLE = false;
        
        // Cas Floating
        if (isFloating() || getDisplayType() == SOFTWIN) {
          UView* layout_view = view->getParentView();
          if (!layout_view) layout_view = hardwin_view;
          // keep the same size (un show/hide ne doit pas retailler le parent!)
          UDimension size = layout_view->getSize();
          layout_view->updateLayout(&size);
          hardwin_view->updatePaint(view);
        }
        else {
          // devrait etre comme LAYOUT mais le pbm c'est que e.firstLayoutView
          // est indetermine
          if (hardwin != updated_win) {
            hardwin->doUpdate(UUpdate::layoutAndPaint, null);
            updated_win = hardwin;
          }
        }
      }

      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // LAYOUT_ONLY or LAYOUT_AND_PAINT (et SCROLL mais passe jamais par la)

      else {
        // ici il y a un pbm si la region est ponctuelle sans epaisseur
        // (par ex une ubox vide) car locateSource renverra null
        // (du fait d'une intersection vide) => on met a 1,1
        if (view->width == 0)  view->width = 1;
        if (view->height == 0) view->height = 1;

        e.setSourceAndProps(view);
        
        // fromview = from which view we must perform the lay out
        // (de telle sorte que les parents se reajustent correctement
        // quand c'est necessaire)
        // typiquement fromview est window vie ou bien la vue d'un
        // floating ou d'une box qui a une width ET height imposee
        UView* layout_view = e.layout_view;
        if (!layout_view) layout_view = hardwin_view;

        // showview : la vue a partir de laquelle on paint
        // generalement : meme view que layoutview (cf ci-apres)
        UView* showview = null;
        UDimension size(0,0);
        // est-ce qu'on impose la meme taille? normalement oui
        bool has_size = true;

        // cas ou la box a une width OU height imposee mais elle vient
        // de changer => il faut donc la recalculer (getSize() renverrait
        // l'ancienne taille). De plus, comme cette taille a peut etre diminue
        // il faut reafficher le parent
        if (layout_view->hasVMode(UView::SIZE_HAS_CHANGED)) {
          size.width = size.height = -1; 
          has_size = false;
          if (layout_view != hardwin_view) showview = layout_view->getParentView();
        }
        else {
          // !!! cet algo est completement FAUX: on ne peut pas savoir
          // a priori a partir d'ou il faut refaire le layout ni le paint
          // - AVANT on prenait tjrs showview = fromview; ce qui a pour effet
          //   de reafficher trop de choses dans la plupart des cas (eg.
          //   les scrollbars qui reaffichent le viewport!) 
          // - MAINTENANT: on a rajoute la "rustine" UUpdate::ADD_REMOVE
          //   pour ne prendre showview = fromview que dans ce cas
          // mais de toute facon, il faudrait s'y prendre autrement, avec
          // des damaged et de la factorisation
          
          size = layout_view->getSize();     // keep the same size
          if (upd.modes & (UUpdate::ADD_REMOVE)) showview = layout_view;
          else showview = view;
        }

        if (isShowable() && e.getView()) {
          if ((upd.modes & UUpdate::LAYOUT) && !(upd.modes & UUpdate::PAINT))
            // que layout, pas paint => showiew = null
            layout_view->updateLayout((has_size? &size :null)); 
          else {
            layout_view->updateLayout((has_size? &size :null), layout_view!=showview);
            hardwin_view->updatePaint(showview);
          }
        }
        
        else if (upd.isHiddenObjectsMode()) {
          // toujours faire layout (mais paint inutile) si always
          bool can_show = emodes.IS_SHOWABLE;
          emodes.IS_SHOWABLE = true;

          // hiddenObjects() est typiquement appele qunad l'objet n'a jamais
          // ete affiche => taille pas encore calculee => size = null
          layout_view->updateLayout(null/*size*/);
          emodes.IS_SHOWABLE = can_show;
        }
      }
    }
  }
}


}
