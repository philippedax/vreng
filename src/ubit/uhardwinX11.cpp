/************************************************************************
 *
 *  UHardwinX11.cpp: X11 windows
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
#if UBIT_WITH_X11

#include <iostream>
#include <cstdio>
#include <ubit/uappli.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uon.hpp>
#include <ubit/ustr.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/uwin.hpp>
//#include <ubit/umsproto.hpp>
#include <ubit/udispX11.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

#define SYS_DISP   ((UDispX11*)disp)->sys_disp
#define SYS_SCREEN ((UDispX11*)disp)->sys_screen
  
Display* UHardwinX11::getSysDisp() const {return SYS_DISP;}
Screen* UHardwinX11::getSysScreen() const {return SYS_SCREEN;}
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
void UHardwinX11::makeCurrent() {
#if UBIT_WITH_GL
  if (!UAppli::isUsingGL()) {
    UAppli::error("UHardwinX11::makeCurrent","this application was not launched in OpenGL mode");
    return;
  } 
  if (glcontext) {
    cerr  << "--- makeCurrent: syswin " << sys_win<< " OWN glcontext: " << glcontext << endl;
    glXMakeCurrent(SYS_DISP, sys_win, glcontext);
  }
  else {
    cerr  << "--- makeCurrent: syswin " << sys_win<< " SHARED glcontext: " << ((UDispX11*)disp)->glcontext << endl;
    glXMakeCurrent(SYS_DISP, sys_win, ((UDispX11*)disp)->glcontext);
  }
#else
  UAppli::error("UHardwinX11::makeCurrent","this application was not compiled with OpenGL");
#endif
}
  
void UHardwinX11::swapBuffers() {
#if UBIT_WITH_GL
  if (!UAppli::isUsingGL()) {
    UAppli::error("UHardwinX11::swapBuffers","this application was not launched in OpenGL mode");
    return;
  } 
  
  cerr << "--- swapBuffers: syswin " <<sys_win<< endl;

  makeCurrent();  // glXSwapBuffers demande un positionnement adequat du context
  // ici on pourrait verifier !!!&&&
  
  glXSwapBuffers(SYS_DISP, sys_win);
  
#else
  UAppli::error("UHardwinX11::swapBuffers","this application was not compiled with OpenGL");
#endif
}
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
//NB: move fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())
void UHardwinX11::setPos(const UPoint& p) {
  //cerr <<"setpos: "<< this << " pos " << p.x << " " << p.y << endl; 
  if (sys_win != None) XMoveWindow(SYS_DISP, sys_win, int(p.x), int(p.y));
}

UPoint UHardwinX11::getPos() const {
  if (sys_win == None) return UPoint(0,0);
  else if (wintype == SUBWIN) {
    // cette fct retourne les coords relativement a parent window
    Window root;
    int x, y;
    unsigned int w, h;
    unsigned int border_w, depth;
    XGetGeometry(SYS_DISP, sys_win, &root, &x, &y, &w, &h, &border_w, &depth);
    return UPoint(x, y);
  }
  else {
    // XGetGeometry ne marche pas dans le cas des Shells car ceux-ci peuvent
    // etre inclus dans une fenetre intermediaire cree par le WM
    Window childw;
    int rootx, rooty;
    XTranslateCoordinates(SYS_DISP, sys_win, XRootWindowOfScreen(SYS_SCREEN),
                          0, 0, &rootx, &rooty, &childw);
    //cerr <<"getpos (1stwin): "<< this << " pos " << rootx << " " << rooty << endl; 
    return UPoint(rootx, rooty);
  }
}

UPoint UHardwinX11::getScreenPos() const {
  if (sys_win == None) return UPoint(0,0);
  else {
    // dans tous les cas, que la window soit inclue dans un Shell ou une autre window
    // retourne toujors la position sur l'ecran
    Window childw;
    int rootx, rooty;
    XTranslateCoordinates(SYS_DISP, sys_win, XRootWindowOfScreen(SYS_SCREEN),
                          0, 0, &rootx, &rooty, &childw);
    //cerr <<"getScreenPos: "<< this << " pos " << rootx << " " << rooty << endl; 
    return UPoint(rootx, rooty);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//NB: resize fait par programme (PAS par l'utilisateur: cf UDisp::on_configure())
void UHardwinX11::setSize(const UDimension& size) {
  if (sys_win != None) {
    XResizeWindow(SYS_DISP, sys_win, int(size.width), int(size.height));
    //if (UAppli::isUsingGL()) disp->setGLViewportOrtho(size);  fait dans UGraph
  }
}

UDimension UHardwinX11::getSize() const {
  if (sys_win == None) return UDimension(0,0);
  else {
    Window root;
    int x, y;
    unsigned int w, h;
    unsigned int border_w, depth;
    XGetGeometry(SYS_DISP, sys_win, &root, &x, &y, &w, &h, &border_w, &depth);
    return UDimension(w, h);
  }
}

void UHardwinX11::show(bool state) {
  if (sys_win != None) {
    //natdisp->showNotify(sys_win, state);
    if (state) XMapRaised(SYS_DISP, sys_win); else XUnmapWindow(SYS_DISP, sys_win);
  }
}

void UHardwinX11::toFront() {
  if (sys_win != None) XRaiseWindow(SYS_DISP, sys_win);
}

void UHardwinX11::toBack() {
  if (sys_win != None) XLowerWindow(SYS_DISP, sys_win);
}

void UHardwinX11::setCursor(const UCursor* curs) {
  Cursor xc = (!curs) ? None : curs->getCursorImpl(disp)->cursor;
  XDefineCursor(SYS_DISP, sys_win, xc);
}

void UHardwinX11::setTitle(const UStr& s) {
  if (sys_win != None && s.c_str()) XStoreName(SYS_DISP, sys_win, s.c_str());
  //cf. XSetWMName(SYS_DISP, w, text_prop);
}

UStr UHardwinX11::getTitle() const {
  if (sys_win == None) return "";

  char* t = null;
  XFetchName(SYS_DISP, sys_win, &t);
  //cf. Status XGetWMName(SYS_DISP, w, text_prop_return)
  UStr s = t;
  XFree(t);
  return s;
}

void UHardwinX11::setIconTitle(const UStr& s) {
  if (sys_win != None && s.c_str()) XSetIconName(SYS_DISP, sys_win, s.c_str());
  //cf. XSetWMIconName(SYS_DISP, w, text_prop);
}

UStr UHardwinX11::getIconTitle() const {
  if (sys_win == None) return "";
  else {
    char* t = null;
    XGetIconName(SYS_DISP, sys_win, &t);
    //cf. Status XSetWMIconName(SYS_DISP, w, text_prop_return)
    UStr s = t;
    XFree(t);
    return s;
  }
}

void UHardwinX11::setClassProperty(const UStr& res_name, const UStr& res_class) { 
  if (sys_win != None) {
    XClassHint* class_hints = XAllocClassHint();
    class_hints->res_class = const_cast<char*>(res_class.c_str());
    class_hints->res_name  = const_cast<char*>(res_name.c_str());
    XSetClassHint(SYS_DISP, sys_win, class_hints);
    XFree(class_hints);  // ???
  }
}


/*
void UHardwinX11::setNames(const UStr& res_class, const UStr& res_name, 
                          const UStr& win_name, const UStr& icon_name) { 
  XClassHint* class_hints = XAllocClassHint();
  class_hints->res_class = const_cast<char*>(res_class.c_str());
  class_hints->res_name  = const_cast<char*>(res_name.c_str());
  
  XmbSetWMProperties(SYS_DISP, sys_win,
                     win_name.c_str(), icon_name.c_str(),
                     null, 0,    //argv, argc,
                     null,       // normal_hints,
                     null,       // wm_hints, 
                     class_hints);
}
*/
// ==================================================== [Ubit Toolkit] =========

UHardwinX11::UHardwinX11(UDispX11* d, UWin* w) : UHardwinImpl(d, w), sys_win(None) {}

UHardwinX11::~UHardwinX11() {
  if (sys_win == None) return;
  else if (wintype == PIXMAP) XFreePixmap(SYS_DISP, sys_win);
  else XDestroyWindow(SYS_DISP, sys_win);    // detruit MAINFRAME ???
}

void UHardwinX11::realize(WinType wtype, float w, float h) {
  if (isRealized()) return;
  wintype = wtype;  // must be done first!
  UDispX11* d = getDispX11();

  if (isPixmap()) {  // first!
    sys_win = XCreatePixmap(d->sys_disp, d->getSysFrame(), int(w), int(h), d->getBpp());
    return;
  }
  
  if (!isHardwin()) {
    UAppli::error("UHardwinX11","wrong type for this function: %d", wtype);
    return;
  }
  
  XSetWindowAttributes attr;
  unsigned long attr_mask = 0;
  
  Window parent_win = XRootWindowOfScreen(d->sys_screen);

  // !attention: defaultPixmap doit deja avoir ete cree!
  //default Pixmap with same depth as UDisp
  //INDISPENSABLE, sinon XCreateWindow plante si depth != rootWindow's depth
  //MEME si border_width = 0 !
  attr.border_pixmap = d->default_pixmap;
  attr_mask |= CWBorderPixmap;
  
  // curseur standard
  attr.cursor =  UCursor::pointer.getCursorImpl(disp)->cursor;
  attr_mask |= CWCursor;
  
  //mettre colormap appropriee par rapport au Visual sinon plantage!
  if (d->getSysColormap() != None) {
    attr.colormap = d->getSysColormap();
    attr_mask |= CWColormap;
  }
  
  if (wtype == MENU) {
    //window pas geree par le WM (no borders...) car c'est un menu
    attr.override_redirect = True;
    attr_mask |= CWOverrideRedirect;
    
    //sauve ce qui est sous le le menu pour economiser les reaffichages
    attr.save_under = True;
    attr_mask |= CWSaveUnder;
  }
  
  else if (wtype == SUBWIN) {
    // il faut rendre cette window sensible aux ExposeEvent car sa
    // parent window ne les recevra pas
    attr.event_mask = ExposureMask;
    attr_mask |= CWEventMask;
    
    //attr.background_pixel = BlackPixelOfScreen(d->sys_screen);
    //attr_mask |= CWBackPixel;
    
    // !!! ATTENTION: il faut retrouver le bon parent ! (NB: revoir API de realize()) !!!@@@
    UView *win_v, *parent_v;
    UHardwinX11* parent_hw;
    if ((win_v = win->getWinView(/*disp*/))
        && (parent_v = win_v->getParentView())
        && (parent_hw = (UHardwinX11*)parent_v->getHardwin())
        ) {
      parent_win = parent_hw->sys_win;
      if (parent_win == None) 
        UAppli::error("UHardwinX11::realize","the parent of the USubwin is not realized, using default parent");
    }
  }
  
  //transparent background
  attr.background_pixmap = None;
  attr_mask |= CWBackPixmap;
  
  //wattr.background_pixel = WhitePixelOfScreen(nd->getXScreen());
  //wattr_mask |= CWBackPixel;
  //wattr.border_pixel = BlackPixelOfScreen(nd->getXScreen());
  //wattr_mask |= CWBorderPixel;
  
  // !ATT: dans le cas CHILD_WIN c'est la Window du Disp qui est prise
  // comme parent (cad le main frame). Theoriquement ca devrait etre celle
  // du widget contenant
  
  // FAUX: dans le cas SUBWIN d->getSysFrame() n'est pas forcement le bon parent
  // Window parent_win = (wtype == SUBWIN) ?
  //   d->getSysFrame() : XRootWindowOfScreen(d->sys_screen);  // un seul niveau de SUBWIN !!!

  sys_win = XCreateWindow(d->sys_disp, parent_win,
                          0,0, int(w), int(h), // !! width=0 or height=0 crashes!
                          0,	     // border_width
                          d->getBpp(),
                          InputOutput,  // Class
                          d->sys_visual->visual,
                          attr_mask, &attr);
  if (sys_win == None) {
    UAppli::error("UHardwinX11::realize","could not create X11 Window");
    return;
  }
  
  if (wtype == SUBWIN) {
    XMapWindow(d->sys_disp, sys_win);  // rendre visible
  }
  else {
    XSelectInput(d->sys_disp, sys_win,
                 PointerMotionMask
                 | ExposureMask
                 //NB: GrapicsExpose, NoExpose -> field graphics_expose of GC
                 | EnterWindowMask
                 | LeaveWindowMask
                 | ButtonPressMask
                 | ButtonReleaseMask
                 | KeyPressMask
                 | KeyReleaseMask
                 | FocusChangeMask
                 | KeymapNotify
                 | ColormapChangeMask
                 | PropertyChangeMask
                 //VisibilityChangeMask
                 //NB: ne pas utiliser ResizeRedirectMask: c'est pour le WM
                 //et ca fait deconner les Resise, Expose, etc.
                 // ResizeRedirectMask
                 | StructureNotifyMask
                 //SubstructureNotifyMask
                 //SubstructureRedirectMask
                 );
    
    const UAtomsX11& atoms = d->getAtoms();
    
    // Close/Delete Window Protocol
    XSetWMProtocols(d->sys_disp, sys_win, 
                    const_cast<Atom*>(&(atoms.WM_DELETE_WINDOW)), 1);
    
    // non-native pointer events are sent by the UMS on UBIT_WINDOW(s)
    // according to the UBIT_EVENT_FLOW Protocol
    // NB: inutile de copier le 0 final de la string: getProperty le rajoutera
    unsigned char winid[2] = {wtype, 0};
    XChangeProperty(d->sys_disp, sys_win, atoms.UMS_WINDOW,
                    XA_STRING, 8/*format*/, PropModeReplace, winid, 2);  
  }
  
  // l'iconification du MainFrame entraine l'iconification de ce Dialog
  if (wtype == DIALOG) {
    XSetTransientForHint(d->sys_disp, sys_win, d->getSysFrame());
  }  
}

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */
