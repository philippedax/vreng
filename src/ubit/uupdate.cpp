/************************************************************************
 *
 *  update.cpp: Paint and Layout Updates
 *  Ubit GUI Toolkit - Version 6
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
#include <cstdio>
#include <iostream>
#include <ubit/ustr.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uview.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uupdate.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

const UUpdate UUpdate::paint(PAINT);
const UUpdate UUpdate::layoutAndPaint(LAYOUT | PAINT);

/*
 NO_DELAY = 1<<2,
 * updates immediately.
 * by default, objects are updated asynchronously when the program returns
 * to the event loop
 
 SOFT_DBF = 1<<4,
 * enables soft double buffering for X11.
 * by default double buffering depends on UConf.x_double_buffer
 
 NO_SOFT_DBF = 1<<5,
 *< disables soft double buffering for X11.
 * by default double buffering depends on UConf.x_double_buffer
 
 //void hiddenObjects() {modes |= HIDDEN_OBJECTS;}

 // int  getDbfMode() const;
 
 //void softDbf() {modes |= SOFT_DBF;}
 // will use soft double bufferiong (for X11 rendering only).
 
 //void setScroll(float delta_x, float delta_y);
 // [impl] used to scroll objects.
 //void setScroll(float delta_x, float delta_y, UView* viewport_view);
 //void noDelay() {modes |= NO_DELAY;}
 //will update immediately (update operations are normally buffered).   
 */

UUpdate::~UUpdate() {
  //if (modes & STR_DATA) delete props.sd;
  //else if (modes & (/*SCROLL|*/MOVE)) delete props.sm;
}

UUpdate::UUpdate(unsigned int m) {
  modes = m;
  //props.sd = null;
}

UUpdate::UUpdate(const UUpdate& upd) {
  modes = upd.modes;
  if (modes & STR_DATA) setPaintData(null);
  else if (modes & (/*SCROLL|*/MOVE)) setMove(0,0,false,false);
  //if (!upd.props.sd) props.sd = null;
  //else if (modes & STR_DATA) props.sd = new StrData(*upd.props.sd);
  //else if (modes & (/*SCROLL|*/MOVE)) props.sm = new ScrollMove(*upd.props.sm);
}

UUpdate& UUpdate::operator=(const UUpdate& upd) {
  modes = upd.modes;
  //if (!upd.props.sd) props.sd = null;
  //else if (modes & STR_DATA) props.sd = new StrData(*upd.props.sd);
  //else if (modes & (/*SCROLL|*/MOVE)) props.sm = new ScrollMove(*upd.props.sm);
  return *this;
}

bool UUpdate::operator==(const UUpdate& upd) {
  //return (modes == upd.modes && props.sd  == upd.props.sd);
  if (modes != upd.modes) return false;
  else if (modes & STR_DATA)
    return props.sd.data == upd.props.sd.data 
    && props.sd.region == upd.props.sd.region
    && props.sd.pos1 == upd.props.sd.pos1 
    && props.sd.pos2 == upd.props.sd.pos2;
  else if (modes & (/*SCROLL|*/MOVE))
    return props.sm.delta_x == upd.props.sm.delta_x
    && props.sm.delta_y == upd.props.sm.delta_y
    && props.sm.xpercent == upd.props.sm.xpercent
    && props.sm.ypercent == upd.props.sm.ypercent;
  else 
    return true;
}

/*
 int UUpdate::getDbfMode() const {
 if (modes & NO_SOFT_DBF) return -1;    // disable
 else if (modes & SOFT_DBF) return +1;  // enable
 else return 0; // as default
 }
 */

//UUpdate::StrData::StrData(const UData* d, int p1, int p2)
//: data(d), region(null), pos1(p1), pos2(p2) {}

void UUpdate::setPaintData(const UData* d) {
  modes |= (PAINT | STR_DATA);
  props.sd.data = d;
  props.sd.region = null;
  props.sd.pos1 = 0;
  props.sd.pos2 = 0;
  //delete props.sd;
  //props.sd = new StrData(d, 0, 0);
}

// will only paint an enclosing subpart of this string
void UUpdate::setPaintStr(const UStr* s, int str_pos1, int str_pos2) {
  modes |= (PAINT | STR_DATA);
  props.sd.data = s;
  props.sd.region = null;
  props.sd.pos1 = str_pos1;
  props.sd.pos2 = str_pos2;
  //delete props.sd;
  //props.sd = new StrData(s, strp1, strp2);
}

//UUpdate::ScrollMove::ScrollMove(float dx, float dy, bool _xpercent, bool _ypercent) : 
//delta_x(dx), delta_y(dy), xpercent(_xpercent), ypercent(_ypercent) {
//}

void UUpdate::setMove(float dx, float dy, bool xpercent, bool ypercent) {
  modes |= MOVE;
  props.sm.delta_x = dx;
  props.sm.delta_y = dy;
  props.sm.xpercent = xpercent;
  props.sm.ypercent = ypercent;
  //delete props.sm;
  //props.sm = new ScrollMove(dx, dy, xpercent, ypercent);
}

/*
 void UUpdate::setScroll(float dx, float dy) {
 modes |= SCROLL;
 delete props.sm;
 props.sm = new ScrollMove(dx, dy, false, false);
 }
 */
}
