/************************************************************************
 *
 *  ucall.cpp: Callback Object
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
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uevent.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uview.hpp>
#include <ubit/umenu.hpp>
#include <ubit/utimer.hpp>
#include <iostream>
using namespace std; 
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

void UCall::wrongEventType(const char* evname, const char* callname) {
  error("operator",
         " - this callback requires an event of type: %s \n"
         " - but the type of the actual event is: %s \n"
         "To solve this problem, put a breakpoint in ubit::UCall::wrongEventType()",         evname, callname);
}

void UCall::addingTo(UChild& child, UElem& parent) {
  // sert a specifier la condition par defaut
  if (!child.getCond()) child.setCond(UOn::action);
  UNode::addingTo(child, parent);
}

/*
 void UCall::removingFrom(UChild *selflink, UElem *parent) {
 // ne PAS faire " par->setCmodes(on, false) " car il peut y avoir
 // plusieurs callbacks avec la meme condition
 // (et de toute facon ce n'est pas une erreur, juste un peu plus long)
 UNode::removingFrom(selflink, parent);
 }
 */

UCall& ucloseWin(int stat) {return ucall(stat, &UElem::closeWin);}

/* TEST
 template <class A, class R> 
 class Truc_F1 {
 const UCond& cond;
 R (*fun)(A);
 public:
 Truc_F1(const UCond& c, R(*f)(A)) : cond(c), fun(f) {}
 UChild operator()(A a) {return UChild(new UCall_F1<A,R>(fun,a), cond);}
 };
 
 template <class A, class R, class E> 
 class Truc_F1E {
 const UCond& cond;
 R (*fun)(E&,A);
 public:
 Truc_F1E(const UCond& c, R(*f)(E&,A)) : cond(c), fun(f) {}
 UChild operator()(A a) {return UChild(new UCall_F1E<A,R,E>(fun,a), cond);}
 };
 
 - - - - - - 
 
 template <class A1, class A2, class R> 
 class Truc_F2 {
 const UCond& cond;
 R (*fun)(A1,A2);
 public:
 Truc_F2(const UCond& c, R(*f)(A1,A2)) : cond(c), fun(f) {}
 UChild operator()(A1 a1, A2 a2) 
 {return UChild(new UCall_F2<A1,A2,R>(fun,a1,a2), cond);}
 };
 
 template <class A1, class A2, class R, class E> 
 class Truc_F2E {
 const UCond& cond;
 R (*fun)(E&,A1,A2);
 public:
 Truc_F2E(const UCond& c, R(*f)(E&,A1,A2)) : cond(c), fun(f) {}
 UChild operator()(A1 a1, A2 a2) 
 {return UChild(new UCall_F2E<A1,A2,R,E>(fun,a1,a2), cond);}
 };
 */

/*
 //UCall& ucompactEvents(UCall& c) {return c;}
 
 UCompactEvents::UCompactEvents(UCall& c) :
 postponed_event(*new UEvent(UEvent::timer, null, null, null)),
 timer(*new UTimer()),
 pcall(c),
 delay(0) {
 timer.onAction(ucall(this, &UCompactEvents::timeout));
 }
 
 UCompactEvents::~UCompactEvents() {
 delete &timer;
 delete &postponed_event;
 }
 
 void UCompactEvents::operator()(UEvent& e) {
 //postponed_event.copy(e);
 postponed_event = e;
 timer.start(delay,1,false);
 }
 
 void UCompactEvents::timeout() {
 //postponed_event.setTime(e.getTime());
 (*pcall)(postponed_event);
 }
 */

}

