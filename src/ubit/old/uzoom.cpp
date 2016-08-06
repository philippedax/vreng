/************************************************************************
 *
 *  uzoom.cpp: semantic zooming
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/uzoom.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/upix.hpp>

using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


bool UInscale::verifies(const UUpdateContext& ctx, const UElem&) const {
  return (ctx.xyscale >= smin && ctx.xyscale <= smax);
}

// ==================================================== [Ubit Toolkit] =========

UZoommenu::UZoommenu(UBox& zoomed_box, UBox& panned_box) :
pzoom_action(new UZoomAction(zoomed_box)),  // for centred zooming
ppan_action(new UPanAction(panned_box)),    // for panning
event_mask(UModifier::RightButton | UModifier::ControlDown)
{
  itemRadius() = 0;

  // items 0 and 4 zoom the zoomed_box
  item(0).setAttr(*pzoom_action);
  item(0).add(UFont::xx_large + UFont::bold + UColor::red + "  Z+ ");
  
  item(4).setAttr(*pzoom_action);
  item(4).add(UFont::xx_large + UFont::bold + UColor::red + "  Z-  ");
  
  // items 2 and 6 pan the zoomed_box
  item(2).setAttr(*ppan_action);
  item(2).add(UPix::bigUp);
  
  item(6).setAttr(*ppan_action);
  item(6).add(UPix::bigDown);
}

void UZoommenu::openMenuOn(int _event_mask) {
  event_mask = _event_mask;
}

// opens menu on event_mask on event_box and propagates events to children otherwise
void UZoommenu::openMenuCB(UMouseEvent& e) {
  if ((e.getButton() & event_mask) || (e.getModifiers() & event_mask))
    open(e);   // open the menu
  else
    e.propagate();  // propagate to children
}

// ==================================================== [Ubit Toolkit] =========

UZoompane::UZoompane(UArgs a) : 
pviewport(new UBox(a)),
pmenu(new UZoommenu(*pviewport, *pviewport)),  // creates pos and scale
ppos(pviewport->obtainAttr<UPos>()),
pscale(pviewport->obtainAttr<UScale>())
{
  add(*pviewport);
  catchEvents(UOn::mpress / ucall(pmenu(), &UZoommenu::openMenuCB));  
}

void UZoompane::openMenuOn(int event_mask) {
  pmenu->openMenuOn(event_mask);
}

}

