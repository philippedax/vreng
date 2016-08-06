/************************************************************************
 *
 *  winImpl.cpp: window implementation
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
#include <ubit/uwinImpl.hpp>
#include <ubit/uviewImpl.hpp>
#include <ubit/udisp.hpp>
#include <ubit/uglcontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UHardwinImpl::UHardwinImpl(UDisp* d, UWin* w) :
wintype(NONE), must_update(false), 
disp(d), win(w), softwin_list(null), glcontext(null) {
}
  
UHardwinImpl::~UHardwinImpl() {
  if (disp) {
    // enlever cette window de la liste des win managees par le disp
    disp->removeHardwin(win);
    // disp.getNatDisp()->showNotify(getSysWin(), false);    // warn the UMS server
    
    if (disp->default_context == glcontext) disp->default_context = null;
    if (disp->current_glcontext == glcontext) disp->current_glcontext = null;
  }
  delete glcontext;  // if any
}


UChildren* UHardwinImpl::getSoftwinList() {return softwin_list;}
  
UWinList* UHardwinImpl::getSoftwins() {
  if (softwin_list && softwin_list->begin() != softwin_list->end()) 
    return static_cast<UWinList*>(**softwin_list->begin());
  // other cases
  return null;
}

UWinList* UHardwinImpl::obtainSoftwins() {
  UWinList* softwins = getSoftwins();
  // recuperer ou creer la softlist de la hardwin
  if (!softwins) {
    if (!softwin_list) softwin_list = new UChildren();
    // UWinList elements are not counted has parents when auto deleting children
    // they are considered as genuine lists, not viewable elements when managing views
    softwins = new UWinList();
    softwin_list->push_back(softwins);
    softwins->setShowable(true);
  }
  return softwins;
}

void UHardwinImpl::addSoftwin(UWin* softwin, UWin* hardwin, UDisp* d, bool add_to_the_end) {
  if (!softwin || !hardwin) return;
  
  // s'il ya  une vue courante l'enlever (ca devrait etre fait uniquement si meme disp !!!)
  //UView* softwin_view = softwin->getActualSoftwinView();
  USoftwinImpl* softw = softwin->softImpl();
  UView* softwin_view = softw ? softw->getActualView(softwin->views) : null;
  
  if (softwin_view) {
    UHardwinImpl* old_hardwin_impl = softwin_view->getHardwin();
    if (old_hardwin_impl) old_hardwin_impl->removeSoftwin(softwin);
  }
  
  softwin_view = softwin->realizeSoftwin(hardwin, hardwin->getWinView(d), 
                                         d, add_to_the_end);
  if (softw) softw->setActualView(softwin_view);
}

// seul addSoftWin() doit appeller cette fonction, de maniere a enlever de la liste cournate
void UHardwinImpl::removeSoftwin(UWin* softwin) {
  if (!softwin) return;
  USoftwinImpl* swi = softwin->softImpl();
  if (swi) swi->setActualView(null);
  
  UWinList* softwins = getSoftwins();
  if (softwins) {
    softwins->setAutoUpdate(false);
    softwins->remove(*softwin, false);
    softwins->setAutoUpdate(true);
  }
}


}

