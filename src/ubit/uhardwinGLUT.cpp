/************************************************************************
 *
 *  uHardwinGLUT.cpp: GLUT Windows
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

#if 0 //dax - notused in vreng

#include <ubit/ubit_features.h>
#if UBIT_WITH_GLUT

#include <iostream>
#include <cstdio>
#include <ubit/uappli.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uwin.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/udispGLUT.hpp>
#include <ubit/uglcontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

//void glutIconifyWindow(void);
//void glutFullScreen(void)
/*
void UHardwinGLUT::makeCurrent() {
  glutSetWindow(sys_win);
}
  
void UHardwinGLUT::swapBuffers() {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  //cerr << "*** swapBuffers win: " << sys_win << " *** " << endl;
  glutSwapBuffers();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//NB: move fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())
void UHardwinGLUT::setPos(const UPoint& p) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutPositionWindow(int(p.x), int(p.y));
  if (cur_win != sys_win) glutSetWindow(cur_win);
}


UPoint UHardwinGLUT::getPos() const {
  if (sys_win <= 0) return UPoint(0,0);
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  UPoint pos;
  
  if (wintype != SUBWIN) {  // toplevel window
    pos.set(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));
  }
  else {  // subwindow
    int glob_x = glutGet(GLUT_WINDOW_X);
    int glob_y = glutGet(GLUT_WINDOW_Y);    
    //int parwin = glutGet(GLUT_WINDOW_PARENT);
    //if (parwin == 0) glutPositionWindow(int(p.x), int(p.y));
    pos.set(glob_x, glob_y);                                    // FAUX &&&&&&!!!!!
  }
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
  return pos;
}


UPoint UHardwinGLUT::getScreenPos() const {
  if (sys_win <= 0) return UPoint(0,0);
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  
  UPoint pos(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y)); // OK with subwindows????%%%%
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
  return pos;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//NB: resize fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())

void UHardwinGLUT::setSize(const UDimension& size) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);

  glutReshapeWindow(int(size.width), int(size.height));
  //if (wintype != SUBWIN) UAppli::getDisp()->setGLViewportOrtho(size);  // fait dans UGraph
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UDimension UHardwinGLUT::getSize() const {
  if (sys_win <= 0) return UDimension(0,0);

  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  
  UDimension dim(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  
  if (cur_win != sys_win) glutSetWindow(cur_win);
  return dim;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLUT::show(bool state) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  if (state) glutShowWindow(); 
  else glutHideWindow();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLUT::toFront() {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutPopWindow();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLUT::toBack() {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutPushWindow();
  if (cur_win != sys_win) glutSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLUT::setCursor(const UCursor* curs) {
  if (sys_win <= 0) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  int xc = (!curs) ? GLUT_CURSOR_INHERIT : curs->getCursorImpl(disp)->cursor;
  glutSetCursor(xc);
  if (cur_win != sys_win) glutSetWindow(cur_win);  
 }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLUT::setTitle(const UStr& s) {
  if (sys_win <= 0 || !s.c_str()) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutSetWindowTitle(s.c_str());
  if (cur_win != sys_win) glutSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr UHardwinGLUT::getTitle() const {
  if (sys_win <= 0) return "";
  
  //int cur_win = glutGetWindow();
  //if (cur_win != sys_win) glutSetWindow(sys_win);
  //title = ???;                                                // existe ???  !!!&&&
  //if (cur_win != sys_win) glutSetWindow(cur_win);

  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UHardwinGLUT::setIconTitle(const UStr& s) {
  if (sys_win <= 0 || !s.c_str()) return;
  
  int cur_win = glutGetWindow();
  if (cur_win != sys_win) glutSetWindow(sys_win);
  glutSetIconTitle(s.c_str());
  if (cur_win != sys_win) glutSetWindow(cur_win);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr UHardwinGLUT::getIconTitle() const {
  if (sys_win <= 0) return "";
  
  //int cur_win = glutGetWindow();
  //if (cur_win != sys_win) glutSetWindow(sys_win);
  //title = ???;                                                // existe ???  !!!&&&
  //if (cur_win != sys_win) glutSetWindow(cur_win);

  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// no effect with GLUT

void UHardwinGLUT::setClassProperty(const UStr& instance_name, const UStr& class_name) {
}

// ==================================================== [Ubit Toolkit] =========

UHardwinGLUT::UHardwinGLUT(UDispGLUT* d, UWin* w)
: UHardwinImpl(d, w), sys_win(0) {
  glcontext = new UGlcontext(d, this);
}

UHardwinGLUT::~UHardwinGLUT() {
  if (sys_win < 1) return;
  //else if (wintype == PIXMAP) DestroyPixmap(SYS_DISP, sys_win);  //A_COMPLETER!!!
  
  glutDestroyWindow(sys_win);    // detruit MAINFRAME ???
  // fait par ~UHardwin: delete glcontext;
}

// ==================================================== [Ubit Toolkit] =========

void UHardwinGLUT::realize(WinType wtype, float w, float h) {
  if (isRealized()) return;  
  wintype = wtype;  // must be done first!

  if (wtype == PIXMAP) {  // first!
    UAppli::error("UHardwinGLUT","type PIXMAP not implemented for GLUT");
    return;
  }
  
  if (!isHardwin()) {
    UAppli::error("UHardwinGLUT","wrong type for this function: %d", wtype);
    return;
  }
  
  if (wtype != SUBWIN) {
    sys_win = glutCreateWindow(" ");
    glutReshapeWindow(int(w), int(h));
    getDispGLUT()->initWinCallbacks(sys_win);
  }
  
  else {       // !!! ATTENTION: il faut retrouver le bon parent!
    /*
    int parent_win = 0;
    UView *win_v, *parent_v;
    UHardwinGLUT* parent_hw;
    if ((win_v = win->getWinView(/disp/))
        && (parent_v = win_v->getParentView())
        && (parent_hw = (UHardwinGLUT*)parent_v->getHardwin())
        ) {
      parent_win = parent_hw->sys_win;
    }
    */
    int parent_win = 0;
    UElem* par = win->getParent(0);
    UWin* parw = dynamic_cast<UWin*>(par);
    if (parw) parent_win = ((UHardwinGLUT*)parw->getHardwin())->sys_win;
    
    if (parent_win == 0) {
      UAppli::error("UHardwinGLUT::realize","the parent of the USubwin is not realized, using the main frame as a parent");
      parent_win = 1;
    }

    sys_win = glutCreateSubWindow(parent_win, 0, 0, int(w), int(h));
    getDispGLUT()->initWinCallbacks(sys_win);   // ???
  }
}


}
#endif
#endif //notused
