/************************************************************************
 *
 *  ustyle.cpp: Element and Box styles
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

#include <iostream>
#include <ubit/ubit_features.h>
#include <ubit/ucall.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uboxgeom.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/uview.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/uon.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


ULocalProps::ULocalProps() :
size(UAUTO, UAUTO),
padding(0, 0),
border(null), 
background(null), 
alpha(1.0),
content(null) {
}

UStyle::UStyle() {
  viewStyle = &UView::style;
  textSeparator = null;    // text separator between two children when printing
  orient = UOrient::INHERIT;
  halign = UHalign::LEFT;
  valign = UValign::TOP;
  hspacing = UHspacing::INHERIT;
  vspacing = UVspacing::INHERIT;
  font = null;
  cursor = null;
  //edit = null;
  //color = null;
  //bgcolor = null;
  //attrs = null;
  setColors(UColor::inherit);
  setColor(UOn::DISABLED, UColor::disabled);
  setBgcolors(UColor::none);
  setBgcolor(UOn::DROP_ENTERED, UColor::grey);
}

UStyle::~UStyle() {
  //on ne peut pas detruire colors & bgcolors car partagees
  //delete colors;
  //delete bgcolors;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UStyle::setSize(ULength w, ULength h) {
  if (w != UIGNORE) local.size.width = w;
  if (h != UIGNORE) local.size.height = h;
}

void UStyle::setPadding(ULength horiz, ULength vert) {
  if (horiz != UIGNORE) local.padding.left = local.padding.right = horiz;
  if (vert != UIGNORE) local.padding.top = local.padding.bottom = vert;
}

void UStyle::setHorizPadding(ULength left, ULength right) {
  if (left != UIGNORE) local.padding.left = left;
  if (right != UIGNORE) local.padding.right = right;
}

void UStyle::setVertPadding(ULength top, ULength bottom) {
  if (top != UIGNORE) local.padding.top = top;
  if (bottom != UIGNORE) local.padding.bottom = bottom;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const UColor* UStyle::getColor(const UElem& e) const {
  return colors[e.isSelected() * UOn::ACTION_COUNT + (int)e.getInterState()];
}

const UColor* UStyle::getBgcolor(const UElem& e) const {
  return bgcolors[e.isSelected()*UOn::ACTION_COUNT + (int)e.getInterState()];
}

void UStyle::setColors(UColor& c) {setColors(c, c);}

void UStyle::setColors(UColor& c_unselect, UColor& c_select) {
  for (int k = 0; k < UOn::ACTION_COUNT; ++k) {
    colors[k] = &c_unselect;
    colors[UOn::ACTION_COUNT + k] = &c_select;
  }
}

void UStyle::setBgcolors(UColor& c) {setBgcolors(c, c);}

void UStyle::setBgcolors(UColor& c_unselect, UColor& c_select) {
  for (int k = 0; k < UOn::ACTION_COUNT; ++k) {
    bgcolors[k] = &c_unselect;
    bgcolors[UOn::ACTION_COUNT + k] = &c_select;
  }
}

void UStyle::setColor(int action, UColor& c) {
  colors[action] = &c;
  colors[UOn::ACTION_COUNT + action] = &c;
}

void UStyle::setBgcolor(int action, UColor& c) {
  bgcolors[action] = &c;
  bgcolors[UOn::ACTION_COUNT + action] = &c;
}

}
