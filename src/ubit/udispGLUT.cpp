/************************************************************************
 *
 *  udispGLUT.cpp: GLUT implementation
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
#if UBIT_WITH_GLUT

#include <iostream>
#include <errno.h>
#include <ubit/ugraph.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/uconf.hpp>
#include <ubit/uwin.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/nat/udispGLUT.hpp>
#include <ubit/nat/urendercontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UCursor UCursor::pointer(GLUT_CURSOR_LEFT_ARROW, UCONST);
UCursor UCursor::crosshair(GLUT_CURSOR_CROSSHAIR, UCONST);
UCursor UCursor::text(GLUT_CURSOR_TEXT, UCONST);
UCursor UCursor::hand(GLUT_CURSOR_INFO, UCONST);
UCursor UCursor::pencil(GLUT_CURSOR_SPRAY, UCONST);  // ???GLUT_CURSOR_LEFT_ARROW
UCursor UCursor::wait(GLUT_CURSOR_WAIT, UCONST);
UCursor UCursor::question(GLUT_CURSOR_HELP, UCONST);
UCursor UCursor::hresize(GLUT_CURSOR_LEFT_RIGHT, UCONST);
UCursor UCursor::vresize(GLUT_CURSOR_UP_DOWN, UCONST);
UCursor UCursor::move(GLUT_CURSOR_INFO, UCONST);  // same sa hand
UCursor UCursor::dnd(GLUT_CURSOR_CYCLE, UCONST);  // ???
//UCursor UCursor::plus(GDK_PLUS, UCONST);
  
// ==================================================== [Ubit Toolkit] =========

UDispGLUT::UDispGLUT(const UStr& _dname) : UDisp(_dname)
{
  const char* dname = getDisplayName().empty() ? "''" : getDisplayName().c_str();

  // glutInitWindowSize(int width, int height); default: 300x300
  //NB: bpp is not used ?
  
  int display_modes = GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA;
  if (depth_size > 0) display_modes |= GLUT_DEPTH;
  if (stencil_size > 0) display_modes |= GLUT_STENCIL;
  //if (accum_size > 0) display_modes |= GLUT_ACCUM;
 
  glutInitDisplayMode(display_modes);
  
  // ATT: un seul DISP peut etre ouvert!
  int argc = 1;
  char *argv[] = {"Ubit"};
  // ? plutot dans initAppli() ?
  glutInit(&argc, argv);   // cf: -gldebug -sync -display -direct -indirect -geom
  
  screen_width = glutGet(GLUT_SCREEN_WIDTH);
  screen_height = glutGet(GLUT_SCREEN_HEIGHT);
  screen_width_mm = glutGet(GLUT_SCREEN_WIDTH_MM);
  screen_height_mm = glutGet(GLUT_SCREEN_HEIGHT_MM);
  
  if (screen_width_mm == 0 || screen_height_mm == 0) {
    UAppli::warning("UDispGLUT","the resolution of the diplay is unknow (display %s)",dname);
    setPixelPerInch(72);   // default
  }
  else setPixelPerMM(double(screen_width)/screen_width_mm);
   
  //if (!(default_rc = UGraph::createRC(this, null/*sharelists*/))) {
  //  UAppli::error("UDispGLUT","could not create Rendering Context on display '%s'",dname);
  //  return;
  //}
  
  UModifier::mapKeys(this);
  UKey::mapKeys(this);
  
  //default_rc->setDest(mainframe, 0, 0);  n'exsite pas sous GLUT!
  is_opened = true;
}
  
UDispGLUT::~UDispGLUT() {
  cerr << "UDispGLUT::~UDispGLUT() " <<endl;
  // if (sys_disp) ???CloseDisplay(sys_disp);
}

// ==================================================== [Ubit Toolkit] =========
  
unsigned long UDispGLUT::createColorPixel(const URgba& rgba) {
  return 0;
}

UHardwinGLUT* UDispGLUT::createWinImpl(UWin* win) {
  return new UHardwinGLUT(this, win);
}

UCursorImpl* UDispGLUT::createCursorImpl(int cursor_type) {
  // cas cursor_type == 0 ???
  UCursorImpl* ci = new UCursorImpl();
  ci->cursor = cursor_type;
  return ci;
}

void UDispGLUT::deleteCursorImpl(UCursorImpl* ci) {
  delete ci;
}

// =============================================================================

void UDispGLUT::setPointerPos(const UPoint&) {
  UAppli::error("UDisp::setPointerPos","Not available with GLUT");
}

UPoint UDispGLUT::getPointerPos() const {
  UAppli::error("UDisp::getPointerPos","Not available with GLUT");
  return UPoint(0,0);
}

int UDispGLUT::getPointerState() const {
  UAppli::error("UDisp::getPointerState","Not available with GLUT");
  return 0;
}

bool UDispGLUT::grabPointer(const UCursor* c) {
  //UAppli::error("UDisp::grabPointer","Not available with GLUT");
  return false;
}

void UDispGLUT::ungrabPointer() {
  //UAppli::error("UDisp::ungrabPointer","Not available with GLUT");
}

bool UDispGLUT::pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                           UCursor* cursor, UCall* callback) {
  UAppli::error("UDisp::pickWindow","This function is not available when using GLUT");
  return false;
}

// ==================================================== [Ubit Toolkit] =========

void UDispGLUT::startAppli() {}
void UDispGLUT::quitAppli() {}

class UGlutQuit {
public:
  int err;
  UGlutQuit(int _err) : err(_err) {} 
};

void UDispGLUT::startLoop(bool main) {
  try {
    glutMainLoop();
  }
  catch(UGlutQuit& e) {
    cerr << "UGlutQuit was called on:" << e.err <<endl; 
  }
  catch(...) {
    cerr << "Whatever"<<endl; 
  }
}

void UDispGLUT::quitLoop(bool main) {
  //  if (main) mainloop_running = subloop_running = false;
  //  else subloop_running = false;
  //if (main) throw UGlutQuit(1); m:arche pas sous MacOSX. Why ????
  exit(0);
}

UWin* UDispGLUT::retrieveWin(int sys_win) {
  HardwinList::iterator c = hardwin_list.begin();  // NB: on pourrait utiliser un VECTOR?
  HardwinList::iterator c_end = hardwin_list.end();
  for ( ; c != c_end; ++c) {
    UHardwinGLUT* hw = static_cast<UHardwinGLUT*>(*c);
    if (hw && hw->sys_win == sys_win && hw->win) return hw->win;
  }
  return null;
}

namespace glut {
  
  static UDispGLUT* _disp = null;   // only one display !!
  static int _state = 0, _mouse_state = 0;
  static int _win_pos_x = 0, _win_pos_y = 0;
  
  static void winMotionCB(int x, int y) {
    UWin* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    UEventFlow* f = UAppli::impl.flowlist[0];
    unsigned long time = UAppli::getTime();  

    f->mouseMotion(win->getWinView(_disp), time, _state, UPoint(x,y), 
                   UPoint(x + _win_pos_x, y + _win_pos_y));
    //cerr << "winMotionCB " << (x+_win_pos_x) <<" "<< (y+_win_pos_y) <<endl;

    UAppli::impl.processPendingRequests();
  }
  
  
  static void winMouseCB(int btn, int up_down, int x, int y) {
    // NB: l'appel du callback positionne glutGetWindow correctement
    int cur_win = glutGetWindow();
    UWin* win = _disp->retrieveWin(cur_win);
    if (!win) return;

    UEventFlow* f = UAppli::impl.flowlist[0];
    UView* winview = win->getWinView(_disp);
    unsigned long time = UAppli::getTime();
    
    _win_pos_x = glutGet(GLUT_WINDOW_X);
    _win_pos_y = glutGet(GLUT_WINDOW_Y);
    UPoint scr_pos(x + _win_pos_x, y + _win_pos_y);
    UPoint win_pos(x, y);
    //cerr << "winMouseCB " << _win_pos_x<<" "<<_win_pos_y<<" / "<<scr_pos<<endl;
    
    if (up_down == GLUT_DOWN) {
      switch (btn) {
        case GLUT_LEFT_BUTTON: 
          //19jan08: the pressed button must be included!
          _mouse_state |= UMouseEvent::LeftButton;
          _state = _mouse_state | glutGetModifiers();
          f->mousePress(winview, time, _state, win_pos, scr_pos, UMouseEvent::LeftButton);
          break;
        case GLUT_MIDDLE_BUTTON:
          _mouse_state |= UMouseEvent::MidButton;
          _state = _mouse_state | glutGetModifiers();
          f->mousePress(winview, time, _state, win_pos, scr_pos, UMouseEvent::MidButton);
          break;
        case GLUT_RIGHT_BUTTON: 
          _mouse_state |= UMouseEvent::RightButton;
          _state = _mouse_state | glutGetModifiers();
          f->mousePress(winview, time, _state, win_pos, scr_pos, UMouseEvent::RightButton);
          break;
        default:
          UAppli::warning("glutMousePress", "unknown mouse button %d", btn);
          break;
      }
    }
    else if (up_down == GLUT_UP) {
      switch (btn) {
        case GLUT_LEFT_BUTTON: 
          //19jan08: the pressed button must be excluded!
          _mouse_state &= ~UMouseEvent::LeftButton;
          _state = _mouse_state | glutGetModifiers();
          f->mouseRelease(winview, time, _state, win_pos, scr_pos, UMouseEvent::LeftButton);
          break;
        case GLUT_MIDDLE_BUTTON:
          _mouse_state &= ~UMouseEvent::MidButton;
          _state = _mouse_state | glutGetModifiers();
          f->mouseRelease(winview, time, _state, win_pos, scr_pos, UMouseEvent::MidButton);
          break;
        case GLUT_RIGHT_BUTTON: 
          _mouse_state &= ~UMouseEvent::RightButton;
          _state = _mouse_state | glutGetModifiers();
          f->mouseRelease(winview, time, _state, win_pos, scr_pos, UMouseEvent::RightButton);
          break;
        default:
          UAppli::warning("glutMouseRelease", "unknown mouse button %d", btn);
          break;
      }
    }
    UAppli::impl.processPendingRequests();
  }
  
  // -----------------------------------------------------------------------------
  
  static void winSpecialCB(int keycode, int x, int y) {  // modifiers, etc.
    UWin* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    UEventFlow* f = UAppli::impl.flowlist[0];
    UView* winview = win->getWinView(_disp);
    unsigned long time = UAppli::getTime();

    //cerr << "glutSpecialCB: code: " << int(keycode) << endl;
    
    //19jan08: the pressed MODIFIERS SHOULD be included!!!@@@
    _state = _mouse_state | glutGetModifiers();
    f->keyPress(winview, time, _state, keycode, 0/*keychar*/);
    
    //19jan08: the pressed MODIFIERS SHOULD be exluded!!!@@@    
    // !!!??? pas sur que le MODIFIER ait été enlevé !!!???
    _state = _mouse_state | glutGetModifiers();
    f->keyRelease(winview, time, _state, keycode, 0/*keychar*/);

    UAppli::impl.processPendingRequests();
  }
  
  static void winKeyboardCB(unsigned char keychar, int x, int y) {
    UWin* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    UEventFlow* f = UAppli::impl.flowlist[0];
    UView* winview = win->getWinView(_disp);
    unsigned long time = UAppli::getTime();

    //cerr << "glutKeyboardCB: key: " << keychar << " int: "<< int(keychar) << endl;
    
    if (keychar == 127     // backspace
        || keychar == 13   // enter
        || keychar == 8    // delete
        || keychar == 9    // tab
        || keychar == 27   // escape
        ) {
      winSpecialCB(keychar, x, y);  // keychar serves as keycode
    }
    else {
      _state = _mouse_state | glutGetModifiers();
      f->keyPress(winview, time, _state, 0/*keycode*/, keychar);
      f->keyRelease(winview, time, _state, 0/*keycode*/, keychar);
      UAppli::impl.processPendingRequests();
    }
  }
  
  // -----------------------------------------------------------------------------
  
  static void winEntryCB(int in_out) {
    UWin* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    UEventFlow* f = UAppli::impl.flowlist[0];    
    UView* winview = win->getWinView(_disp);
    unsigned long time = UAppli::getTime();

    _win_pos_x = glutGet(GLUT_WINDOW_X);
    _win_pos_y = glutGet(GLUT_WINDOW_Y);
    
    if (in_out == GLUT_ENTERED) f->winEnter(winview, time);
    else if (in_out == GLUT_LEFT) f->winLeave(winview, time);
    UAppli::impl.processPendingRequests();
  }
  
  static void winDisplayCB() {    
    if (glutLayerGet(GLUT_NORMAL_DAMAGED)) { // glutDisplayCB() caused by window system
      UWin* win = _disp->retrieveWin(glutGetWindow());
      if (!win) return;
      
      UView* v = win->getWinView();
      //cerr << "winDisplayCB " << win << " size " <<v->width << " " <<  v->height << endl;
      _disp->onPaint(v, v->x, v->y, v->width, v->height);
    }
    // in all cases, even if caused by glutPostRedisplay()
    UAppli::impl.processPendingRequests();
  }
  
  static void winReshapeCB(int w, int h) {
    UWin* win = _disp->retrieveWin(glutGetWindow());
    //cerr << "winReshapeCB " << win << " size " <<w << " " << h << endl;
    if (!win) return;
    
    UView* v = win->getWinView();
    _disp->onResize(v, UDimension(w, h));
  }
  
  static void winVisibilityCB(int state) {
    UWin* win = _disp->retrieveWin(glutGetWindow());
    if (!win) return;
    //UView* v = win->getWinView();
    //cerr << "glutVisibilityCB " << endl;
    // state: GLUT_NOT_VISIBLE or GLUT_VISIBLE
  }
  
  /*
   void glutIdleFunc(void (*func)(void));
   void glutTabletMotionFunc(void (*func)(int x, int y));
   void glutTabletButtonFunc(void (*func)(int button, int state, int x, int y));
   void glutSpaceballMotionFunc(void (*func)(int x, int y, int z));
   void glutSpaceballRotateFunc(void (*func)(int x, int y, int z));
   void glutSpaceballButtonFunc(void (*func)(int button, int state));
   void glutDialsFunc(void (*func)(int dial, int value));
   void glutButtonBoxFunc(void (*func)(int button, int state));   
   */
} // end namespace glutImpl
  
void UDispGLUT::initWinCallbacks(int sys_win) {
  glut::_disp = this;  // static var used by callback functions

  if (sys_win <= 0) {
    UAppli::error("UDispGLUT::initWinCallbacks","Wrong window ID: %d", sys_win);
    return; 
  }

  glutSetWindow(sys_win);  
  glutMotionFunc(glut::winMotionCB);
  glutPassiveMotionFunc(glut::winMotionCB);
  glutMouseFunc(glut::winMouseCB);
  glutSpecialFunc(glut::winSpecialCB);
  glutKeyboardFunc(glut::winKeyboardCB);
  glutEntryFunc(glut::winEntryCB);
  glutDisplayFunc(glut::winDisplayCB);
  glutReshapeFunc(glut::winReshapeCB);
  glutVisibilityFunc(glut::winVisibilityCB);
}  

}
#endif  // UBIT_WITH_GLUT



