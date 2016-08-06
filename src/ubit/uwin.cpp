/************************************************************************
 *
 *  uwin.cpp: ubit windows
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
#include <ubit/uon.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uboxes.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/uinteractors.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ucall.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

static const char* Unrealized_Window = 
  "UWin object %p is not realized; check if this window has a valid parent";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStyle* UWin::createStyle() {
  UStyle* style = new UStyle;
  style->textSeparator = new UStr("\n");
  style->orient = UOrient::VERTICAL;
  style->halign = UHalign::FLEX;
  //NB: dialogs a la fois hflex et vflex
  style->valign = UValign::FLEX;
  style->hspacing = 1;
  style->vspacing = 1;
  style->local.padding.set(0, 0);
  style->font = null;
  style->local.background = &UAppli::conf.default_background;
  return style;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UWin::UWin(UArgs a) : UBox(a) {  
  emodes.IS_SHOWABLE = false;
  memset(&wmodes, 0, sizeof(wmodes));    // init to 0 all bits
  wmodes.IS_HARDWIN = !UAppli::conf.soft_wins;
  wmodes.IS_AUTO_OPENED = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// !ATTENTION: les appels aux fcts virtuelles ne marchent pas normalement
// dans les constructeurs et les DESTRUCTEURS (l'appel est fait avec
// la classe du destructeur, pas la classe effective)
// ==> toute redefinition de 'removingFrom' necessite un DESTRUCTEUR !!

UWin::~UWin() {
  //NB: removeFromParents et removeAll fait par ~UBox ou subclass
  if (wmodes.IS_MODAL) UAppli::impl.removeModalWin(*this);

  if (views) {    
    // views have been destroyed in UBox::deleteRelatedViews
    // except in some cases
    UView* nextview = null;
    for (UView* v = views; v != null; v = nextview) {
      nextview = v->getNext();
      delete v;
    }
    views = null;
  }

  for (unsigned int k = 0; k < wimpl.size(); ++k) delete wimpl[k];
  wimpl.clear();  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

USoftwinImpl* UWin::softImpl() const {
  if (!wmodes.IS_HARDWIN && wimpl.size() == 1) {
    return static_cast<USoftwinImpl*>(wimpl[0]);
  }
  else {
    if (!wmodes.IS_HARDWIN && wimpl.size() > 1)
      UAppli::error("UWin::softImpl",
                    "Incoherency UWin %p has multiple USoftImpl implementations",this);
    else
      UAppli::error("UWin::softImpl",
                    "UWin %p does not have a USoftImpl implementation",this);
    return null;
  }  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UHardwinImpl* UWin::hardImpl() const {
  if (wmodes.IS_HARDWIN && wimpl.size() == 1) {
    return static_cast<UHardwinImpl*>(wimpl[0]);
  }
  else {
    if (wmodes.IS_HARDWIN && wimpl.size() > 1)
      UAppli::error("UWin::hardImpl",
                    "Incoherency UWin %p has multiple UHardwinImplImpl implementations",this);
    else
      UAppli::error("UWin::hardImpl",
                    "UWin %p does not have a UHardwinImpl implementation",this);
    return null;
  }  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this function MUST return the same hardwin as hardImpl() WHEN the UWin is a hardwin

UHardwinImpl* UWin::getHardwin(UDisp* disp) const {
  UView* winview = getWinView(disp);
  return winview ? winview->hardwin : null;
}


UView* UWin::getWinView(UDisp* disp) const {   // NB: disp can be null 
  // hardwins have ONLY ONE shared view in the current version !!
  if (wmodes.IS_HARDWIN) return views;
  
  // default is the first view
  if (disp == null) return views; // A REVOIR!!!
  
  for (UView* v = views; v!= null; v = v->getNext()) {
    if (v->getDisp() == disp) return v;
  }
  return null; // not found
}


void UWin::addWinView(UView* view) {
  // hardwins have ONLY ONE shared view in the current version !!
  if (wmodes.IS_HARDWIN && views) {
    UAppli::internalError("UWin::addWinView","UWin object %p should have only one view",this);
    return;
  }
  UView* v = getWinView(view->getDisp());
  if (v) UAppli::internalError("UWin::addWinView","UWin object %p has already only one view for UDisp %p",this,view->getDisp());
  view->setNext(views);
  views = view;
}

// ==================================================== [Ubit toolkit] =========

//ATT: cette fct doit etre appelee a la creation de la UWin
UWin& UWin::setSoftwin(bool state) {
  if (wimpl.size() > 0) {
    UAppli::error("UWin::setSoftwin",
                  "this method can only be called before the window are created; UWin object: %p", this);
    return *this;
  }
  wmodes.IS_HARDWIN = !state;
  return *this;
}

UView* USoftwinImpl::getActualView(UView* winviews) {
  // si actual_view est nul ou pas dans la liste c'est qu'elle n'est plus valide
  if (!actual_view) return null;
  for (UView* v = winviews; v != null; v = v->getNext()) {
    if (v == actual_view) return actual_view;
  }
  actual_view = null; // plus valide !  // &&&ajout 13aug08
  return null;
}

void USoftwinImpl::setActualView(UView* winview) {
  actual_view = winview;
}

USoftwinImpl::USoftwinImpl(UWin& win) {
  //NOTE: SOFTWIN => FLOATING (fait implicitement via l'ajout d'un UPos aux softwins)
  ppos = new UPos(0, 0);
  win.setAttr(*ppos);
  actual_view = null;
  
  if (win.wmodes.IS_DIALOG) {   // pas pour les menus ni les frames
    UBox& titlebar = 
    uhbox(uhflex()
          + ulabel("---------").ignoreEvents()  // !! ICI gerer le title !!!
          + uright()
          + ubutton("X" + ucloseWin())
          );
    titlebar.addAttr((new UPosControl)->setModel(ppos)
                     + UBackground::navy + ualpha(0.5)
                     + UColor::white
                     );

    win.setAttr(*new UCompositeBorder(UValign::top + UHalign::flex + titlebar
                                      //+ UValign::bottom + UHalign::right + ubutton("resize")
                                      ));
  }
}

USoftwinImpl::~USoftwinImpl() {}

// ==================================================== [Ubit toolkit] =========
// ATT: voir aussi + voir cas Displays separes!!
// !!POUR L'instant winview UNIQUE!!!
// NOTE: les Windows sont creees une seule fois mais ouvertes par tous 
// leurs parents

void UWin::initView(UView *parview) {
  initViewImpl(parview, parview->getDisp());
  if (isShowable()) show(true);
}

// optimization: les vues des enfants de la UWin seront crees ulterieurement,
// au premier show() de la UWin, via la fonction realizeChildren()
//att: parview n'est pas la view du parent mais du 1er parent de type UBox!

//UView* UWin::initViewImpl(UChild* selflink, UView* parview, UDisp* disp) {
UView* UWin::initViewImpl(UView* parview, UDisp* disp) {
  UView* winview = getWinView(disp);
  
  // optimization: les vues des enfants de la UWin sont crees plus tard
  // au premier show() de la UWin, via la fonction realizeChildren()
  if (winview) return winview;
  
  // Il n'y a qu'une seule winview par Window
  // ==> ne creer et n'initialiser les descendants que la premiere fois
  UHardwinImpl* hardwin = null;

  if (wmodes.IS_HARDWIN) {  // - - HARDWIN - - - - - - - - - - - - - - - - - - -
    // ATT: cree une seule fois => only one shared window    
    if (wimpl.size() > 0) hardwin = hardImpl();
    else {
      hardwin = disp->createWinImpl(this);
      wimpl.push_back(hardwin);
      disp->addHardwin(this);
    }
  }
  
  else {  //                 - - SOFTWIN - - - - - - - - - - - - - - - - - - - -
    // ATT: cree une seule fois => les winviews partagent le meme upos
    // et pas possible de changer de softwin mode apres creation
    if (wimpl.size() == 0) wimpl.push_back(new USoftwinImpl(*this));
      
    // dans le cas des softwin il faut un parview, et il faut de plus
    // reinitialser la vue partagee a chaque apparition !!!!
    if (!parview) {
      // parview necessaire dans ce cas pour: parview->getWinGraph(); 
      // et aussi optionnellement pour getStyle(closest_parent)
      return null;
    }
    hardwin = parview->getHardwin();
  } // end softwin
  
  
  if (!hardwin) {
    UAppli::internalError("UWin::initView","can't realize UWin object %p", this);
    return null;
  }

  const UViewStyle* render = null;
  if (emodes.HAS_LAYOUT) {
    // si renderer defini dynamiquement dans la childlist
    attributes().findClass(render);
    if (!render) children().findClass(render);
  }
  else {  // default: prendre le renderer defini par le style de la Win
    render = getStyle(null).viewStyle;
  }

  // !note: winview est une var. d'instance!
  if (render) winview = (render->createView)(this, null, hardwin);
  else {
    UAppli::internalError("UWin::initView","can't retreive object UStyle of UWin object %p",this);
    winview = new UView(this, null, hardwin);
  }
    
  //if (emodes.HAS_POS) winview->addVModes(UView::FORCE_POS);
  
  if (isSubWin() || (parview && parview->hasVMode(UView::NO_DOUBLE_BUFFER))) {
    winview->addVModes(UView::NO_DOUBLE_BUFFER);
  }
  
  // NB: ne cree pas la X Window: ce sera fait ulterieurement par UWin::realize() 
  // qunad la fenetre apparaitra la 1ere fois
    
  addWinView(winview);   // add winview to the list of the window views
  return winview;
}

// ==================================================== [Ubit toolkit] =========

bool UWin::realize() {
  if (wmodes.IS_HARDWIN) return realizeHardwin(UHardwinImpl::FRAME);  // FRAME???
  else {
    UAppli::error("UWing::realize","can't realize window object %p",this);
    return false;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UWin::realizeHardwin(char wintype) {
  UHardwinImpl* hardwin = hardImpl();
  if (!hardwin) return false;
  if (hardwin->isRealized()) return true;  // deja realize'
    
  hardwin->realize((UHardwinImpl::WinType)wintype, 10, 10);
  
  if (!hardwin->isRealized()) {
    UAppli::error("UWin::realize","can't create window: UWin=%p",this);
    return false;
  }
  else {
    UView* winview = views;    // ONLY ONE screen !!
    realizeChildren(winview);  // create the children views of the UWin
    return true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UView* UWin::realizeSoftwin(UWin* hardwin, UView* hardwin_view,
                            UDisp* disp, bool add_to_the_end) {
  UHardwinImpl* hardi = hardwin->hardImpl();
  if (!hardi) return null;
  
  UWinList* softwins = hardi->obtainSoftwins();
  // est-ce que this est deja dans la softlist ?
  UChildIter i = softwins->children().find(*this);
  UChild* child = null;
  if (i != softwins->cend()) child = &i.child();
  else {
    bool auto_up = softwins->isAutoUpdate();
    softwins->setAutoUpdate(false);
    softwins->add(this);
    softwins->setAutoUpdate(auto_up);
    child = &(softwins->children().back());
  }
  
  if (!disp) disp = hardwin_view->getDisp();
  UView* winview = getWinView(disp);

  // creer winview (att: l'autre version de initView() appelle show
  // ce qui provoque une boucle infinie!
  if (!winview) winview = initViewImpl(/*child,*/ hardwin_view, disp);

  // le parent de la vue doit etre mis a jour (pour les softwins only)
  // AINSI QUE LE CHAMP win !!
  winview->parview = hardwin_view;
  winview->hardwin = hardwin_view->hardwin;
  
  // n'aura d'effet que la premiere fois (afin que les child views
  // ne soient pas crees plusieurs fois)
  realizeChildren(winview);

  // initialiser disposition spatiale avant affichage
  /*surtout pas: boucle infinie
    UUpdate upd(UUpdate::ALL/LAYOUT);
    upd.evenIfHidden();
    updateSoftwin(upd);
  */
  return winview;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// !ATT: l'initialisation des descendant NE DOIT SE FAIRE qu'une seule
// fois, c'est a dire pour la seule WinView effectivement creee
// (sinon on irait creer des Views inutiles dans les descendants)

void UWin::realizeChildren(UView* winview) {
  //UChild *winlink = null;

  if (!winview)  // || !(winlink = winview->getBoxHolder()))
    UAppli::error("UWin::realizeChildren",Unrealized_Window,this);

  // propager si pas deja fait
  else if (!winview->hasVMode(UView::REALIZED_CHILDREN)) {
    winview->addVModes(UView::REALIZED_CHILDREN);
    UElem::initView(winview);   // !att: winview pas parview!
  }
}

// ==================================================== [Ubit toolkit] =========

UWin& UWin::setTitle(const UStr& title) {
  if (isHardwin()) {
    UHardwinImpl* nw = getHardwin(null/*disp*/);
    if (nw) nw->setTitle(title);
  }
  else /*nop*/;     // not implemented
  return *this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr UWin::getTitle() const {
  if (!isHardwin()) return "";
  else {
    UHardwinImpl* hw = getHardwin(null/*disp*/);
    return hw ? hw->getTitle() : "";
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::toFront(UDisp* disp) {
  if (wmodes.IS_HARDWIN) {
    UHardwinImpl* hw = hardImpl();
    if (hw) hw->toFront();
  }
  else {
    UView* winview;
    UWin* hardwin;
    UHardwinImpl* hardi;
    if (!(winview = getWinView(disp)) 
        || !(hardwin = winview->getWin())
        || !(hardi = hardwin->hardImpl())
        )
      return;
    
    // mettre ou remettre 'this' en LAST position dans la softwin list
    // (addSoftwin fait un removeSoftwin(win) implicite)
    hardi->addSoftwin(this, hardwin, disp, true/*at the end*/);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::toBack(UDisp* disp) {
  if (wmodes.IS_HARDWIN) {
    UHardwinImpl* hardi = hardImpl();
    //UNatWin* nw = hardi ? hardi->getNatWin() : null;
    //if (nw) nw->toBack();
    if (hardi) hardi->toBack();
  }
  //else ... a faire...
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::highlight(bool state) {
  UBox::highlight(state);
  UWin::toFront();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::setFullScreen(bool state, UDisp* disp) {
  //is_fullscreen = state;
  if (wmodes.IS_HARDWIN) {
    if (!disp) disp = UAppli::getDisp();
    if (state) {
      setSize(UDimension(disp->getScreenWidth(), disp->getScreenHeight()));
      setScreenPos(UPoint(0, 0), disp);
    }
    else { 
      setSize(UDimension(-1,-1));
      setScreenPos(UPoint((disp->getScreenWidth() - getWidth(disp)) /2,
                          (disp->getScreenHeight()- getHeight(disp)) /2),
                   disp);
    }
  }
  //else ... a faire...
}

// ==================================================== [Ubit toolkit] =========

bool UWin::isShown() const {
  return isShown(null);
}

bool UWin::isShown(UDisp* disp) const {
  if (!isShowable()) return false;

  if (wmodes.IS_HARDWIN) {
    UHardwinImpl* hardw = hardImpl();
    return hardw ? hardw->isRealized() : false;
  }
  else if (disp == null) {
    // verifier si actual_view toujours dans la liste
    USoftwinImpl* softw = softImpl();
    UView* v = softw ? softw->getActualView(views) : null;
    return v && v->isShown();
  }
  else {
    for (UView* v = views; v != null; v = v->getNext()) {
      if (v->getDisp() == disp && v->isShown()) return true;
    }
  }
  
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::show(bool state, UDisp* disp) {
  if (UAppli::isExiting()) return;
  
  if (wmodes.IS_HARDWIN) 
    /*nop*/;     // hardwins have only one shared window
  else if (disp == null) {
    // verifier si actual_view toujours dans la liste
    
    // FAUX! peut-etre pas encore realisee: USoftwinImpl* softw = softImpl();
    if (wimpl.size() == 1) {
      USoftwinImpl* softw = static_cast<USoftwinImpl*>(wimpl[0]);
      UView* v = softw ? softw->getActualView(views) : null;
      if (v) disp = v->getDisp();
    }
  }

  if (state) {
    emodes.IS_SHOWABLE = true;
    if (wmodes.IS_MODAL) UAppli::impl.addModalWin(*this);
    doUpdate(UUpdate::SHOW, disp); 
    UEvent e(UOn::show, this);  //UElemEvent
    fire(e);
  }
  else {
    emodes.IS_SHOWABLE = false;
    doUpdate(UUpdate::HIDE, disp);
    UEvent e(UOn::hide, this);  //UElemEvent
    fire(e);
    if (wmodes.IS_MODAL) UAppli::impl.removeModalWin(*this);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UWin::showModal(UDisp* disp) {
  setModal(true);
  show(true, disp);
  return UAppli::impl.startModalWinLoop(*this);
}

void UWin::setModal(bool state) {
  wmodes.IS_MODAL = state;
}

bool UWin::isModal() const {
  return wmodes.IS_MODAL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::close(int status) {
  UAppli::impl.setModalStatus(status);
  UEvent e(UOn::close, this);  //UElemEvent
  bool callback_fired = fire(e);
  if (!callback_fired) {
    if (wmodes.IS_MAINFRAME) UAppli::quit(status); else show(false);
  }
}

// ============================================================== ====== =======

void UWin::adjustSize() {
  update(UUpdate::ADJUST_WIN_SIZE, null);
}

void UWin::update(const UUpdate& upd, UDisp* disp) {
  if (UAppli::isExiting()) return;
  UAppli::impl.addUpdateRequest(this, upd);
/*
  if (UAppli::impl.isProcessingUpdateRequests()) {
    // do not update now, add to the update list
    UAppli::impl.addUpdateRequest(this, upd);
  }
  else if ((upd.modes & UUpdate::NO_DELAY) || UAppli::conf.usync) {
    // update objets already in the list, then proceed
    UAppli::impl.processUpdateRequests();
    doUpdate(upd, disp);
  }
  else {
    // do not update now, add to the update list
    UAppli::impl.addUpdateRequest(this, upd);   // !!! disp should be given &&&!!!
  }
 */
}
      
// ==================================================== [Ubit toolkit] =========
  
void UWin::doUpdate(const UUpdate& upd, UDisp* disp) {
  if (UAppli::isExiting()) return;
 
  // update objets already in the list, then proceed
  if (!UAppli::impl.isProcessingUpdateRequests()) 
    UAppli::impl.processUpdateRequests();
   
  if (wimpl.size() == 0) {
    // if this win was not explicitely added to a widget or a display, add it to UAppli
    if (!hasSceneGraphParent()) {
      if (wmodes.IS_HARDWIN) UAppli::add(this);
      else {
        // !!!NB: il faudrait le mainframe associé au disp si != null !!!
        UFrame* mainframe = UAppli::getMainFrame();
        if (mainframe) mainframe->add(this);
        else warning("UWin::doUpdate","this soft window (%p) can't be displayed because it has no parent and there is no available mainframe window", this);
      }
    }
    
    UView* parview = null;
    UChild* ch = null;

    for (UParentIter p = pbegin(); p != pend(); ++p) {
      // do not take into account WINLIST parents (either modalwins or softwin_list)
      if ((*p)->getDisplayType() != UElem::WINLIST) {
        std::vector<UView*> parviews;
        (*p)->retrieveRelatedViews(parviews);
        
        for (unsigned int k = 0; k < parviews.size(); ++k) {
          if (disp == null || parviews[k]->getDisp() == disp) {
            parview = parviews[k];
            ch = &p.parent().getChild();
            break;
          }
        }
      }
    }
    
    if (parview) initViewImpl(/*ch,*/ parview, parview->getDisp());
    if (wimpl.size() == 0) return;
  }  //endif (!impl.initialized) {
    
  // hardwins have only one shared window
  if (wmodes.IS_HARDWIN) {
    UHardwinImpl* hardi = hardImpl();
    if (!hardi) return;
    hardi->doUpdate(upd, this, views);
  }
  else {
    UView* view = null;
    USoftwinImpl* softw = softImpl();

    // NB: getWinView returns a default view if disp not found
    if (disp) view = getWinView(disp);
    // verifie si current_winview toujours dans la liste
    else view = softw ? softw->getActualView(views) : null;

    if (!view) view = views;
    if (softw) softw->doUpdate(upd, this, view);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void USoftwinImpl::doUpdate(const UUpdate& upd, UWin* win, UView* winview) {
  if (UAppli::isExiting()) return;
  UWin* hardwin = null;
  long upd_modes = upd.getModes();
  if (!winview || !(hardwin = winview->getWin())) return;

  UHardwinImpl* hardi = hardwin->hardImpl();
  if (!hardi) return;
  UDisp* disp = winview->getDisp();
  
  if (upd_modes & UUpdate::SHOW) {
    // mettre ou remettre 'this' en LAST position dans la softwin list
    // (addSoftwin fait un removeSoftwin(win) implicite)
    hardi->addSoftwin(win, hardwin, disp, true/*at the end*/);
    win->emodes.IS_SHOWABLE = true;
  }

  else if (upd_modes & UUpdate::HIDE) {
    //EX: hardi->removeSoftwin(win); incorrect car getActualView() 
    // renverrait null et on ne saurait pas ou reafficher au show() suivant
    win->emodes.IS_SHOWABLE = false;
  }
  
  else if (upd_modes & UUpdate::LAYOUT) {
    if (win->isShowable() || upd.isHiddenObjectsMode()) {
      if (!win->realizeSoftwin(hardwin, hardwin->getWinView(disp), disp, true)) 
        return;
    }
  }
  
  win->UBox::doUpdate(upd, null);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinImpl::doUpdate(const UUpdate& upd, UWin* ww, UView* winview) {
  if (UAppli::isExiting()) return;
  if (!winview) return;
  long upd_modes = upd.getModes();

  if (upd_modes & UUpdate::HIDE) {
    ww->emodes.IS_SHOWABLE = false;
    //fire(&e, UOn::hide); supprime car pas recursif: faire autrement
    show(false);	
  }
 
  else if (upd_modes & UUpdate::SHOW) {
    bool initialized = winview->hasVMode(UView::INITIALIZED);
    ww->emodes.IS_SHOWABLE = true;

    //fire(&e, UOn::show);   supprime car pas recursif: faire autrement
    if (!isRealized()) {
      if (!ww->realize()) return;  // creates the X-Window and child views
    }
    doUpdateImpl(upd, ww, winview, null);

    // si la view n'est pas encore initialisee, c'est qu'elle n'a jamais
    // ete affichee et il faut donc initialiser la X Window correspondante
    if (!initialized) {            // set title when applicable
      UTitle* title = null;
      ww->attributes().findClass(title);
      if (!title) ww->children().findClass(title);
      if (title) setTitle(title->value());

      // il faut changer la taille de la X Window la 1e fois qu'elle apparait
      // (avant son apparition par show pour eviter effets visuels indesirables)
      //setSize(int(w->getWidth(d)), int(w->getHeight(d)));
      setSize(ww->getSize());
    }

    show(true);  // dans tous les cas pop up and raise
  }
 
  else if ((upd_modes & UUpdate::PAINT) && !(upd_modes & UUpdate::LAYOUT)) {
    if (ww->isShowable() || upd.isHiddenObjectsMode()) 
      ww->UBox::doUpdate(upd, null);
  }

  else if (upd_modes & UUpdate::ADJUST_WIN_SIZE) {    // resize the window
    if (!isRealized()) {if (!ww->realize()) return;}
    winview->setSize(UDimension(-1, -1));
    doUpdateImpl(upd, ww, winview, null);
    setSize(winview->getSize());
  }
    
  else {   // LAYOUT, etc.
    if (ww->isShowable() || upd.isHiddenObjectsMode()) {
      if (!isRealized()) {if (!ww->realize()) return;}
      // preserver la taille courante
      // NOTE:  bug dans le cas des menus ou on add/remove des objs:  !!!
      // ne changent pas de taille!                 !! A REVOIR !!
      UDimension size = winview->getSize();
      if (size.width > 0 && size.height > 0)
        doUpdateImpl(upd, ww, winview, &size);
      else
        doUpdateImpl(upd, ww, winview, null);
      // 3feb08: the size of the physical window msut be updated
      setSize(winview->getSize());
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//NB:impose une taille donnee en mode 'resize=true' (si les valeurs sont >0)

void UHardwinImpl::doUpdateImpl(const UUpdate& upd, UWin* _win, UView* winview,
                                const UDimension* size) {
  long upd_modes = upd.getModes();
  if (!size || size->width <= 0 || size->height <= 0) size = null;   // securite

  if (_win->isShowable()) {
    if ((upd_modes & UUpdate::LAYOUT) && !(upd_modes & UUpdate::PAINT)) //LAYOUT_ONLY
      winview->updateLayout(size);
    else {  //LAYOUT_AND_PAINT
      winview->updateLayout(size, false);
      winview->updatePaint();
    }
  }
  
  // en mode updateWin, retailler meme si la win
  // n'est pas visisble (sinon c'est a peu pres jamis effectif)
  else if (upd.isHiddenObjectsMode() || (upd_modes & UUpdate::ADJUST_WIN_SIZE)) {
    //toujours faire layout (mais paint inutile) si always
    bool can_show = _win->emodes.IS_SHOWABLE;
    _win->emodes.IS_SHOWABLE = true; 
    winview->updateLayout(size);
    _win->emodes.IS_SHOWABLE = can_show; 
  }
  
  // securite: une taille <= 0 fait planter X11!
  if (winview->width <= 0)  winview->width = 1;
  if (winview->height <= 0) winview->height = 1;
}

// ==================================================== [Ubit toolkit] =========
//ATT: contrairement a update(), resize() impose une taille donnee
// (si les valeurs sont >0)

void UWin::setSize(const UDimension& s, UDisp* d) {
  UView* winview = getWinView(d);
  if (!winview) {
    UAppli::error("UWin::resize",Unrealized_Window,this);
    return;
  }
  winview->setSize(s);

  if (isShowable()) {
    UUpdate upd(UUpdate::PAINT | UUpdate::LAYOUT);

    // ici il manque la possibilite du doublebuf (trait de upd) !
    if (wmodes.IS_HARDWIN) {
      UHardwinImpl* hardi = hardImpl();
      if (!hardi) return;

      // mettre a jour l'affichage
      hardi->doUpdateImpl(upd, this, winview, &s);

      // changer la taille physique de la X window
      // pas w et h : valeurs controlees par updateImpl
      hardi->setSize(winview->getSize());
    }
    else UBox::doUpdate(upd, null);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UDimension UWin::getSize(UDisp* disp) const {
  UView* winview = null;
  
  if (!(winview = getWinView(disp))) {
    // ajouter par defaut a UAppli (ou disp si defini)
    UDisp* d = disp ? const_cast<UDisp*>(disp) : UAppli::getDisp();
    d->add((UWin&)*this);
  }
  
  if (!(winview = getWinView(disp))) {
    UAppli::error("UWin::getSize",Unrealized_Window,this);
    return UDimension(0,0);
  }
  else {
    if (!winview->hasVMode(UView::INITIALIZED)) {
      //UUpdate upd(UUpdate::LAYOUT | UUpdate::HIDDEN_OBJECTS | UUpdate::NO_DELAY);
      //((UWin*)(this))->update(upd);
      ((UWin*)(this))->doUpdate(UUpdate::LAYOUT | UUpdate::HIDDEN_OBJECTS);
    }
    return winview->getSize();
  }
}

float UWin::getWidth(UDisp* disp) const {
  return getSize(disp).width;
}

float UWin::getHeight(UDisp* disp) const {
  return getSize(disp).height;
}

// ==================================================== [Ubit toolkit] =========

UPoint UWin::getPos(const UView& view) const {
  UPoint p = getScreenPos(view.getDisp());
  UPoint p2 = view.getScreenPos();
  p.x -= p2.x; p.y -= p2.y;
  return p;
}

UPoint UWin::getPos(const UWin& win, UDisp* d) const {
  UPoint p = getScreenPos(d);
  UPoint p2 = win.getScreenPos(d);
  p.x -= p2.x; p.y -= p2.y;
  return p;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::setPos(UMouseEvent& e, float x, float y) {
  UPoint p = e.getScreenPos();
  p.x += x; p.y += y;
  setScreenPos(p, e.getDisp());
}

void UWin::setPos(const UView& view, const UPoint& p) {
  UWin* hardw = view.getWin();
  if (!hardw) UAppli::error("UWin::setPos", Unrealized_Window, this);
  else {
    UPoint p2(p.x + view.x, p.y + view.y);
    setPos(*hardw, p2, view.getDisp());
  }
}

void UWin::setPos(const UWin& win, const UPoint& p, UDisp* disp) {
  if (wmodes.IS_HARDWIN) {
    // deplacer par rapport aux coords du Frame ou du Dialog et non de la SUBWIN
    const UWin* w = null;
    if (!win.isSubWin()) w = &win;
    else w = win.getWinView(disp)->getParentView()->getWin();
    if (w) {
      UPoint p2 = w->getScreenPos(disp);
      p2.x += p.x; p2.y += p.y;
      setScreenPos(p2, disp);
    }
  }

  else {   // SOFTWIN
    UView* win_winview = win.getWinView(disp);
    UWin* win_hardwin = null;
    USoftwinImpl* softw = softImpl();
    
    if (!win_winview || !softw || !(win_hardwin = win_winview->getWin())) {
      UAppli::error("UWin::move", Unrealized_Window, this);
      return;
    }

    // verifie si current_winview toujours dans la liste
    UView* v = softw ? softw->getActualView(views) : null;
    UWin* current_hardwin = null;
    UView* current_hardwin_view = null;
    if (v) {
      current_hardwin = v->getWin();
      current_hardwin_view = v->getWinView();
    }

    if (current_hardwin == win_hardwin) {
      //p2.x = p.x - win_winview->x; p2.y = p.y - win_winview->y;
      //softi->getPos().set(p2);
    }
    else {
      UHardwinImpl* win_hardi = win_hardwin->hardImpl();
      if (!win_hardi) return;
      win_hardi->addSoftwin(this, win_hardwin, disp, true/*at the end*/);
    }
    UPoint p2(p.x - win_winview->x, p.y - win_winview->y);
    softw->pos() = p2;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UPoint UWin::getScreenPos(UDisp* disp) const {
  UView* winview = getWinView(disp);
  UHardwinImpl* hardw = null;
  UPoint p;
  if (!winview || !(hardw = winview->getHardwin())) {
    UAppli::error("UWin::getScreenPos", Unrealized_Window, this);
    return p;  // p set to 0,0 when created
  }
  
  p = hardw->getScreenPos();
  
  if (!wmodes.IS_HARDWIN) {
    USoftwinImpl* softw = softImpl();
    if (!softw) return p;
    p.x += softw->pos().getX().val;   // ne peut pas etre en percent!!!&&&&
    p.y += softw->pos().getY().val;   // unites pas prises en compte
  }
  return p;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UWin::setScreenPos(const UPoint& p, UDisp* d) {
  if (wmodes.IS_HARDWIN) {
    UHardwinImpl* hwin = getHardwin(d);
    // si pas deja realisee, le faire maintenant
    if (hwin == null || !hwin->isRealized()) {
      realize();
      hwin = getHardwin(d);
      if (!hwin || !hwin->isRealized()) {
        UAppli::error("UWin::setScreenPos","unrealized hard window %p",this);
        return;
      }
    }
    // moves window relatively to the screen if it is a firts-level window
    hwin->setPos(p);
  }
  
  else { // SOFTWIN
    UHardwinImpl* containing_win = getHardwin(d);
    if (!containing_win) {
      UAppli::error("UWin::setScreenPos","unrealized soft window %p",this);
      return;
    }
    UPoint containing_win_pos;
    containing_win_pos = containing_win->getScreenPos();
    UPoint softwin_pos;
    softwin_pos.x = p.x - containing_win_pos.x;
    softwin_pos.y = p.y - containing_win_pos.y;
    softImpl()->pos() = softwin_pos;
  }
}

void UWin::centerOnScreen(UDisp* disp) {
  if (!wmodes.IS_HARDWIN) return;
  if (!disp) disp = UAppli::getDisp();

  UDimension dim = getSize(disp);
  if (dim.width > 0 && dim.height > 0) {
    float scr_width  = disp->getScreenWidth();
    float scr_height = disp->getScreenHeight();
    setScreenPos(UPoint((scr_width - dim.width) / 2, (scr_height - dim.height) / 2),
                   disp);
  }
}

// ==================================================== [Ubit toolkit] =========
// updates object layout to get correct size when necessay

static void checkUpdate(UWin* win, UDisp* disp) {
  UDimension dim = win->getSize(disp);

  // if <= 0 must update layout to get correct sizes
  if (dim.getWidth() <= 0 || dim.getHeight() <= 0) {
    //UUpdate upd(UUpdate::PAINT | UUpdate::LAYOUT      // 27sep06
    //            | UUpdate::HIDDEN_OBJECTS | UUpdate::UUpdate::NO_DELAY);
    //win->update(upd);
    win->doUpdate(UUpdate::PAINT | UUpdate::LAYOUT | UUpdate::HIDDEN_OBJECTS);
  }
}

void UWin::setPos(const UView& view, const UWinPlacement& pl) {
  UDisp* disp = view.getDisp();
  float _x = 0, _y = 0;  // ext int
  
  if (pl.halign) {
    if (*pl.halign == UHalign::left) {
      if (pl.hoppositeBorder) {	  // a gauche de view
        // updates layout to get correct size when necessay
        checkUpdate(this, disp);
        _x -= this->getWidth(disp) + pl.hdist;
      }
      else _x += pl.hdist;  //alignes a dist pres
    }
    
    else if (*pl.halign == UHalign::right) {
      if (pl.hoppositeBorder)	  // a droite de view
        _x += view.getWidth() + pl.hdist;
      else {
        checkUpdate(this, disp);
        _x += view.getWidth() - this->getWidth(disp) - pl.hdist;
      }
    }
    
    //else nop: aligne' avec view (a gauche)
    // flex devrait s'adapter en largeur (qunad plus petit que view)
  }
  
  if (pl.valign) {
    if (*pl.valign == UValign::top) {
      if (pl.voppositeBorder) {	      // au dessus de view
        checkUpdate(this, disp);
        _y -= this->getHeight(disp) + pl.vdist;
      }
      else _y += pl.vdist;  //alignes a dist pres
    }
    
    else if (*pl.valign == UValign::bottom) {
      if (pl.voppositeBorder)	          // en dessous de view
        _y += view.getHeight() + pl.vdist;
      else {
        checkUpdate(this, disp);
        _y += view.getHeight() - this->getHeight(disp) - pl.vdist;
      }
    }
    
    //else nop: aligne' avec view (en haut)
    // flex devrait s'dapter en hauteur(qunad plus petit que view)
  }
  
  setPos(view, UPoint(_x, _y));
}

UWinPlacement::UWinPlacement() {
  halign = null;
  valign = null;
  hoppositeBorder = voppositeBorder = false;
  hdist = vdist = 0;
}

}
