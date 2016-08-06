/************************************************************************
 *
 *  UDispX11.cpp: X11 implementation
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
#include <errno.h>
#include <ubit/udispX11.hpp>  // includes X11 headers
#include <X11/keysym.h> 
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <ubit/uconf.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/uselection.hpp>
#include <ubit/utimer.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uwin.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/urendercontext.hpp>
#include <ubit/ux11context.hpp>
#include <ubit/uglcontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

#define GetAtom(D,Name) XInternAtom(D,Name,False)
  
UCursor UCursor::pointer(XC_left_ptr, UCONST);
UCursor UCursor::crosshair(XC_tcross, UCONST);
UCursor UCursor::text(XC_xterm, UCONST);
UCursor UCursor::hand(XC_hand2, UCONST);
UCursor UCursor::pencil(XC_pencil, UCONST);
UCursor UCursor::wait(XC_watch, UCONST);
UCursor UCursor::question(XC_question_arrow, UCONST);
UCursor UCursor::hresize(XC_sb_h_double_arrow, UCONST);
UCursor UCursor::vresize(XC_sb_v_double_arrow, UCONST);
UCursor UCursor::move(XC_fleur, UCONST);
UCursor UCursor::dnd(XC_gumby, UCONST);
//UCursor UCursor::plus(XC_plus, UCONST);

// ==================================================== [Ubit Toolkit] =========

UDispX11::UDispX11(const UStr& _dname) : UDisp(_dname),
  sys_disp(null),
  sys_screen(null),
  sys_visual(null),
  sys_cmap(None),
  xconnection(-1),
  xsync(UAppli::getConf().xsync),
  default_pixmap(None)
# if UBIT_WITH_GL
  ,glvisual(null)   // null or equal to sys_visual
# endif
{
  sys_disp = XOpenDisplay(getDisplayName().c_str());
  const char* dname = getDisplayName().empty() ? "''" : getDisplayName().c_str();
  if (!sys_disp || !(sys_screen = DefaultScreenOfDisplay(sys_disp))) {
    UAppli::error("UDispX11","could not open display %s (is the X11 server running ?)",dname);
    return;
  }
  if (xsync) XSynchronize(sys_disp, True);
  xconnection = XConnectionNumber(sys_disp);
  
  // bpp is set by UDisp to UAppli::conf.bpp (usually 24, or 0 for a default value)
  if (bpp == 0) bpp = DefaultDepthOfScreen(sys_screen);

  screen_width = WidthOfScreen(sys_screen);
  screen_height = HeightOfScreen(sys_screen);
  screen_width_mm = XWidthMMOfScreen(sys_screen);
  screen_height_mm = XHeightMMOfScreen(sys_screen);

  // !PBM taille rendue par XWidthMMOfScreen generalement incorrecte!
  
  if (screen_width_mm == 0 || screen_height_mm == 0) {
    UAppli::warning("UDispX11","the resolution of the diplay is unknow (display %s)",dname);
    setPixelPerInch(72);   // default
  }
  else setPixelPerMM(double(screen_width)/screen_width_mm);
  
  // NB: delire X bordelien typique:
  // - d'une part, pour creer une Window de bpp != de celle de RootWindow,
  //   il faut obligatoirement fournir un BorderPixmap de meme bpp que cette
  //   Window MEME si son BorderDepth = 0
  // - ensuite, pour creer ce BorderPixmap (qui est utilise par initWinAttr)
  //   il faut un drawable (typiquement cette Window) en argument: bref le 
  //   cercle vicieux!
  // - mais comme l'API de X est decidemment tres bien faite, en realite 
  //   ce drawable ne sert qu'a indiquer le Screen (et non la bpp!)
  // - DONC, la manip qui consiste a prendre RootWindowOfScreen marche meme
  //   si bpp de RootWindowOfScreen != bpp du Pixmap.

  // default Pixmap with same bpp as UDisp: necessaire pour createXWindow!
  default_pixmap = XCreatePixmap(sys_disp, RootWindowOfScreen(sys_screen), 10,10,bpp);
  
  if (!setVisual(bpp, None)) {
    UAppli::error("UDispX11","could not find appropriate visual or colormap on display '%s' \n(the requested RGB configuration is not supported)", dname);
    return;
  }

#if (!UBIT_WITH_GL && !WITH_2D_GRAPHICS)
# error Wrong compilation options: no graphics engine is available 
#endif
#if UBIT_WITH_GL
  if (UAppli::isUsingGL()) default_context = new UGlcontext(this, null/*sharelist*/);
#endif
#if WITH_2D_GRAPHICS
  if (!default_context) default_context = new UX11context(this);
#endif
  
  if (!default_context) {
    UAppli::error("UDispX11","could not create a rendering context on display '%s'",dname);
    return;
  }
  
  black_pixel = BlackPixelOfScreen(sys_screen);
  white_pixel = WhitePixelOfScreen(sys_screen);
  
  // !attention: les masks varient suivant le type de Visual
  red_mask   = sys_visual->red_mask;
  green_mask = sys_visual->green_mask;
  blue_mask  = sys_visual->blue_mask;
  countBits(red_mask,   red_bits,   red_shift);
  countBits(green_mask, green_bits, green_shift);
  countBits(blue_mask,  blue_bits,  blue_shift);

  atoms.PRIMARY_SELECTION   = XA_PRIMARY;
  atoms.SECONDARY_SELECTION = XA_SECONDARY;
  atoms.WM_PROTOCOLS    = GetAtom(sys_disp, "WM_PROTOCOLS");
  atoms.WM_DELETE_WINDOW= GetAtom(sys_disp, "WM_DELETE_WINDOW");
  atoms.WM_TAKE_FOCUS   = GetAtom(sys_disp, "WM_TAKE_FOCUS");
  atoms.UMS_WINDOW      = GetAtom(sys_disp, UMS_WINDOW_PROPERTY);
  atoms.UMS_MESSAGE     = GetAtom(sys_disp, UMS_MESSAGE_PROPERTY);
  //atoms.USELECTION    = GetAtom(sys_disp, _USELECTION);
  
  // NB: requires: sys_visual, default_pixmap, atoms, and possibly, white/black pixels
  mainframe = createWinImpl(null);
  mainframe->realize(UWinImpl::MAINFRAME, 10, 10);

  if (getSysFrame() == None) {
    UAppli::error("UDispX11","could not create the main frame window for display '%s'", dname);
    return;
  }
  
  UModifier::mapKeys(this);
  UKey::mapKeys(this);
  
  // NB: en realite mainframe sert uniquement a la construction, ce n'est pas
  // la fenetre principale sous X11 (pas utilisee apres!)
  default_context->setDest(mainframe, 0, 0);   // necessaire pour init default_context
  is_opened = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UDispX11::~UDispX11() {
  if (sys_disp && default_pixmap != None) XFreePixmap(sys_disp, default_pixmap);
  if (sys_disp) XCloseDisplay(sys_disp);
  sys_disp = null;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if UBIT_WITH_GL

// GLXContext UDispX11::getCurrentGLContext() {return glXGetCurrentContext();}

GLXContext UDispX11::createGlcontext(UGlcontext* shareList) {
  // NB: cree glvisual mais sans initialiser sys_visual contrairement a setVisual()
  // lequel doit donc etre appele en premier em mode GL
  if (!glvisual) glvisual = createGlvisual(bpp/3, bpp/3, bpp/3, 0/*alpha*/, depth_size);
  
  if (!glvisual) {
    UAppli::error("UDispX11::createGLContext","Couldn't create OpenGL visual");
    return null;
  }
  GLXContext glc = glXCreateContext(sys_disp, glvisual, 
                                    // ressources shared with this context if not null
                                    (shareList ? shareList->glxcontext : null),
                                    True); // direct rendering allowed if available
  if (!glc) UAppli::error("UDispX11::createGLContext","Couldn't create OpenGL Context");
  return glc;
}

void UDispX11::destroyGlcontext(GLXContext glctx) {
  glXDestroyContext(sys_disp, glctx);
}

#endif
// ==================================================== [Ubit Toolkit] =========

static XVisualInfo* _chooseX11Visual(Display*, Screen*, int bpp);

bool UDispX11::setVisual(int bpp, Colormap c) {
#if UBIT_WITH_GL
  if (UAppli::isUsingGL()) {
    glvisual = createGlvisual(bpp/3, bpp/3, bpp/3, 0/*alpha*/, depth_size);
    if (glvisual) {
      // set sys_visual to glvisual, init variables and colormap
      setVisual(glvisual, None);  // a quoi la Colormap en mode GL ???
      return true;
    }
    else {
      UAppli::error("UDispX11::setVisual","could not get OpenGL Visual with BPP = %d", bpp);
      return false;
    }
  }
#endif
  XVisualInfo* v = null;
  if ((v = _chooseX11Visual(sys_disp, sys_screen, bpp))) return setVisual(v, c);
  else {
    UAppli::error("UDispX11::setVisual","could not get X11 visual with BPP = %d", bpp);
    return false;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool UDispX11::setVisual(XVisualInfo* vinfo, Colormap c) {
  if (!vinfo || vinfo->screen >= ScreenCount(sys_disp)) {
    UAppli::error("UDispX11::setVisual","Null or invalid visual argument");
    return false;
  }
  bpp = vinfo->depth;
  sys_visual = vinfo;
  sys_screen = ScreenOfDisplay(sys_disp, vinfo->screen);
  if (sys_cmap != 0) XFreeColormap(sys_disp, sys_cmap);
  if (c != 0) sys_cmap = c;
  else sys_cmap = XCreateColormap(sys_disp, RootWindow(sys_disp, vinfo->screen),
                                  vinfo->visual, AllocNone);  // A quoi la Colormap en mode GL ???
  if (sys_cmap == 0) {
    UAppli::error("UDispX11::setVisual","Can't create colormap");
    return false;
  }
  return true;
}

// ==================================================== [Ubit Toolkit] =========
#if UBIT_WITH_GL

static XVisualInfo* _chooseGLVisual(Display* sys_disp, Screen* sys_screen,
                                    int r, int g, int b, int a, int depth) {
  int gl_attr[] = {
    GLX_RGBA, 
    GLX_RED_SIZE,   r, 
    GLX_GREEN_SIZE, g,
    GLX_BLUE_SIZE,  b, 
    GLX_ALPHA_SIZE, a,
    GLX_DEPTH_SIZE, depth,
    GLX_DOUBLEBUFFER,
    None
  };
  int screen_no = XScreenNumberOfScreen(sys_screen);
  return glXChooseVisual(sys_disp, screen_no, gl_attr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

XVisualInfo* UDispX11::createGlvisual(int r, int g, int b, int a, int depth) {
  // cf aussi: GLXFBConfig *glconfig;
  XVisualInfo* vis = null;
  
  int dummy;
  if (!glXQueryExtension(sys_disp, &dummy, &dummy)) {
    UAppli::error("UDispX11::createGLVisual","X11 server has no OpenGL extension");
    return (XVisualInfo*) null;
  }
  
  int glxmajor = 0, glxminor = 0;
  glXQueryVersion(sys_disp, &glxmajor, &glxminor);
  //cerr << "OpenGL version: "<< glxmajor <<"."<<glxminor << endl;
  
  // find a Visual that match the requested bpp and OpenGL options
  if (!(vis = _chooseGLVisual(sys_disp, sys_screen, r,g,b,a, depth))) {
    UAppli::error("UDispX11::createGLVisual",
                  "could not get requested OpenGL visual with RGBA = %d %d %d %d / Depth = %d",
                  r,g,b,a,depth);
    
    r = g = b = a = 0;
    if ((vis = _chooseGLVisual(sys_disp, sys_screen, r,g,b,a,depth)))
      UAppli::warning("UDispX11::createGLVisual",
                      "using OpenGL visual with RGBA = %d %d %d %d / Depth = %d",
                      r,g,b,a,depth);
    else {
      UAppli::error("UDispX11::createGLVisual","could not get any OpenGL visual");
      return (XVisualInfo*) null;
    }
  }
  
  //if (vis->depth < bpp)
  //  UAppli::warning("NatDisp::createGLVisual",
  //                  "Visual depth (%d bpp) is smaller than requested size (%d pbb)", 
  //                  vis->depth, bpp);
  
  if (UAppli::conf.getVerbosity() != 0) {
    cout <<"OpenGL Visual: requested bpp: "<< bpp <<" / actual bpp: "<< vis->depth
    // <<" / depth buffer= "<< 
    <<" / visual id: "   <<vis->visualid <<" / visual class: "<<vis->c_class
    << (vis->c_class == PseudoColor ? 
        " (PseudoColor)" : vis->c_class==TrueColor ? " (TrueColor)" : " (Unknown)")
    << endl;
  }
  
  return vis;
}

#endif //UBIT_WITH_GL
// ==================================================== [Ubit Toolkit] =========

static bool _findX11Visual(Display* sys_disp, Screen* sys_screen, 
                           int bpp, XVisualInfo* vinfo) {
  int visual_class = TrueColor;
  bool linear_gamma  = false;
  vinfo->depth   = bpp;
  vinfo->c_class = visual_class;
  vinfo->screen  = XScreenNumberOfScreen(sys_screen);
  long vinfo_mask = VisualScreenMask|VisualDepthMask|VisualClassMask;;
  
  bool found = false;
  int count = 0;
  XVisualInfo* tab = XGetVisualInfo(sys_disp, vinfo_mask, vinfo, &count);
  
  for (int k = 0; k < count; k++) {
    double gamma = 0.;
    //ifdef HAVE_XSOLARISGETVISUALGAMMA
    //    XSolarisGetVisualGamma(sys_disp, vinfo.screen, tab[k].visual, &gamma);
    //else
    linear_gamma = false;
    //endif
    if (!found) { // on prend le premier
      found = true;
      *vinfo = tab[k];
      if (!linear_gamma) break;
    }
    
    // si linear, ecraser eventuel choix precedent
    if (linear_gamma && gamma > 0.9 && gamma < 1.1) {
      *vinfo = tab[k];
      break;
    }
  }
  
  if (tab) XFree(tab);
  return found;
}


static XVisualInfo* _chooseX11Visual(Display* sys_disp, Screen* sys_screen, int bpp) {
  XVisualInfo vinfo;
  
  int scr_no = XScreenNumberOfScreen(sys_screen);
  if (scr_no >= ScreenCount(sys_disp)) {
    UAppli::error("UDispX11::setVisual","Invalid screen number %n", scr_no);
    return 0;
  }
  
  if (_findX11Visual(sys_disp, sys_screen, bpp, &vinfo)) 
    goto OK;  // exact match
  
  { // find closest available bpp
    int depth_count, possible_d = 0;
    int* depths = XListDepths(sys_disp, scr_no, &depth_count);
    XVisualInfo vi;
    
    if (depths) {
      for (int k = 0; k < depth_count; k++) {
        if (_findX11Visual(sys_disp, sys_screen, depths[k], &vi)) {
          if (possible_d == 0) {
            possible_d = depths[k];
            vinfo = vi;
          }
          else if (abs(depths[k] - bpp) < abs(possible_d - bpp)) {
            possible_d = depths[k];
            vinfo = vi;
          }
        }
      }
      XFree(depths);
    } 
  }
  
OK: XVisualInfo* res = new XVisualInfo;
  memcpy(res, &vinfo, sizeof(vinfo));
  return res;
}

// ==================================================== [Ubit Toolkit] =========
#if WITH_2D_GRAPHICS

unsigned long UDispX11::createColorPixel(const URgba& rgba) {
  XColor xc;
  // !ATT XColor.red/green/blue est sur 16 bits et non 8 !
  xc.red   = rgba.components[0]<<8;
  xc.green = rgba.components[1]<<8; 
  xc.blue  = rgba.components[2]<<8;
  // color allocation & conversion to pixel value
  if (XAllocColor(sys_disp, sys_cmap, &xc)) {
    return xc.pixel;    // the color was allocated in the colormap
  }
  else { // the appropriate color could be allocated => warning + use black or white
    UAppli::warning("UDispX11::createColor","can't allocate color (colormap is full?)");
    // white if color > 65535, black otherwise
    if (xc.red + xc.blue + xc.green > 65535*2) return getWhitePixel();
    else return getBlackPixel();
  }
}

#else

unsigned long UDispX11::createColorPixel(const URgba& rgba) {return 0;}

#endif

Window UDispX11::getSysRoot() const {return RootWindowOfScreen(sys_screen);}
Window UDispX11::getSysFrame() const {return ((UHardwinX11*)mainframe)->sys_win;}

UHardwinX11* UDispX11::createWinImpl(UWin* win) {
  return new UHardwinX11(this, win); 
}

UCursorImpl* UDispX11::createCursorImpl(int cursor_type) {
  Cursor xc = None;
  if (cursor_type >= 0) xc = XCreateFontCursor(sys_disp, cursor_type);
  else {   //none cursor
    char bits[] = {0x01};
    XColor color; color.pixel = getWhitePixel();
    Pixmap pix = XCreateBitmapFromData(sys_disp, getSysRoot(), bits, 1, 1);
    xc = XCreatePixmapCursor(sys_disp, pix, pix, &color, &color, 0, 0);
  }
  UCursorImpl* ci = new UCursorImpl();
  ci->cursor = xc;
  return ci;
}

void UDispX11::deleteCursorImpl(UCursorImpl* ci) {
  XFreeCursor(sys_disp, ci->cursor);
  delete ci;
}

// =============================================================================

void UDispX11::setPointerPos(const UPoint& p) {
  XWarpPointer(sys_disp, getSysRoot(), getSysRoot(), 0, 0, 0, 0, int(p.x), int(p.y));
  XFlush(sys_disp);
}

UPoint UDispX11::getPointerPos() const {
  Window xroot = None, xchild = None;
  int win_x, win_y;
  unsigned int mask;
  int x = 0, y = 0;
  XQueryPointer(sys_disp, getSysRoot(), &xroot, &xchild, &x, &y, &win_x, &win_y, &mask);
  return UPoint(x, y);
}

int UDispX11::getPointerState() const {
  Window xroot = None, xchild = None;
  int screen_x, screen_y;
  int win_x, win_y;
  unsigned int mask;
  XQueryPointer(sys_disp, getSysRoot(),
                &xroot, &xchild, &screen_x, &screen_y, &win_x, &win_y, &mask);
  return mask;
}

// =============================================================================

bool UDispX11::grabPointer(const UCursor* c) {
  Cursor xc = (!c) ? None : c->getCursorImpl(this)->cursor;
  return XGrabPointer(sys_disp, 
                      getSysFrame(),
                      //owner_events: si True, les windows sous le pointeur
                      //recoivents les Events comme d'habitude (et les coords
                      //sont relatives a celles-ci). Si False, c'est la grab-window
                      //qui recoit tous les events (et les coords sont relatives
                      //a la grab-window)
                      true, (ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
                      GrabModeAsync, GrabModeAsync,
                      //(confine_to_window ? win->natwin->getSysWin() : USysNull),
                      None,  // confine to window
                      xc,    // cursor 
                      CurrentTime)
    == 0;   // GrabSuccess
}

void UDispX11::ungrabPointer() {
  XUngrabPointer(sys_disp, CurrentTime);
}

// =============================================================================

namespace udisp {
  struct PickTarget {
    Display* disp;
    Window root, win;
    int x, y;
  };
  
  static void getClientWindow(PickTarget& t, XEvent& xev) {
    Window win_child;
    // NB: ev.xbutton.(sub)window ne pointent pas sur la fenetre
    // que l'on veut (ni ne retournent les coords ad hoc)
    t.win = XmuClientWindow(t.disp, xev.xbutton.subwindow);
    XTranslateCoordinates(t.disp, /*src*/t.root, /*dst*/t.win,
                          xev.xbutton.x_root, xev.xbutton.y_root, 
                          &t.x, &t.y, &win_child);
  }
}

bool UDispX11::pickWindow(int& x, int& y, UHardwinImpl* hw, 
                          UCursor* curs, UCall* call) {
  Cursor xc = (!curs) ? None : curs->getCursorImpl(this)->cursor;
  UEventFlow* flow = obtainChannelFlow(0);
  udisp::PickTarget t;
  t.disp = getSysDisp();
  t.root = getSysRoot();
  t.win = 0;
  t.x = t.y = 0;
  
  if (XGrabPointer(t.disp, t.root, False,  //owner_events
                   (ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
                   GrabModeSync, GrabModeAsync, None, //confine_to
                   xc, CurrentTime) != GrabSuccess)
    return false;   // can't grab the mouse
  
  while (true) {
    XEvent xev;    // get next event
    XAllowEvents(t.disp, SyncPointer, CurrentTime);
    XNextEvent(t.disp, &xev);
  
    switch (xev.type) {
      case MotionNotify:
        if (call) {
          udisp::getClientWindow(t, xev);
          UMouseEvent e(UOn::mmove, 
                        null, flow, xev.xmotion.time, xev.xmotion.state, 
                        UPoint(xev.xmotion.x, xev.xmotion.y),
                        UPoint(xev.xmotion.x_root, xev.xmotion.y_root),
                        0);  // !! doit pas etre 0 si il y a eu un press !!
          (*call)(e);
        }
        break;
      
      case ButtonPress:
        udisp::getClientWindow(t, xev);
        if (call) {
          UMouseEvent e(UOn::mpress,
                        null, flow, xev.xbutton.time, xev.xbutton.state, 
                        UPoint(xev.xbutton.x, xev.xbutton.y),
                        UPoint(xev.xbutton.x_root, xev.xbutton.y_root),
                        xev.xbutton.button);
          (*call)(e);
        } 
        break;
      
      case ButtonRelease:
        udisp::getClientWindow(t, xev);
        if (call) {
          UMouseEvent e(UOn::mrelease,
                        null, flow, xev.xbutton.time, xev.xbutton.state, 
                        UPoint(xev.xbutton.x, xev.xbutton.y),
                        UPoint(xev.xbutton.x_root, xev.xbutton.y_root),
                        xev.xbutton.button);
          (*call)(e);
        }
        goto UNGRAB;
        break;
    }
  }

UNGRAB:
  XUngrabPointer(t.disp, CurrentTime);   // releases the Grab
  x = t.x;
  y = t.y;
  if (hw) {
    UHardwinX11* hwx = dynamic_cast<UHardwinX11*>(hw);
    if (!hwx)
      UAppli::error("UDispX11::pickWindow","invalid UHardwin argument");
    else {
      hwx->wintype = UWinImpl::EXTWIN;
      hwx->sys_win = t.win;
      hwx->disp = this;
      hwx->win = null;
      hwx->softwin_list = null;
    }
  }
  return true;
}

// ==================================================== [Ubit Toolkit] =========

// X11 errors are catched except if there is an error loop
static const int MAX_XERRORS = 1000;
static int xerror_count = 0, last_xerror = 0;

// X11 generates many stupid errors, even in cases when no error should occur
// => we catch them and do nothing except if many errors are produced sucessively
//    in which case ... there is really a nasty error!
extern "C" {
  static int _quietErrorHandler(Display* d, XErrorEvent* e) {
    
    char buf[500];
    XGetErrorText(d, e->error_code, buf, sizeof(buf));
    char msg[1000];
    sprintf(msg, "X11 Error %d request %d minor %d: %s",
            e->error_code, e->request_code, e->minor_code, buf);
    UAppli::warning("UDispX11", msg);
    
    last_xerror = e->error_code;
    xerror_count++;
    if (xerror_count > MAX_XERRORS) {
      UAppli::fatalError("eventLoop", "Max number of X Errors exceeded: aborting");
    }
    return 0;
  }
}

void UDispX11::startAppli() {
  XSetErrorHandler(_quietErrorHandler);     // catches (stupid) XErrors
  xerror_count = 0;
  last_xerror = Success;
}

void UDispX11::quitAppli() {}

// ==================================================== [Ubit Toolkit] =========

void UDispX11::quitLoop(bool main) {
  UAppliImpl& a = UAppli::impl;
  if (main) a.mainloop_running = a.subloop_running = false;
  else a.subloop_running = false;
}

void UDispX11::startLoop(bool main) {
  UAppliImpl& a = UAppli::impl;
  bool& running = main ? a.mainloop_running : a.subloop_running;
  running = true;
  UDispList& displist = a.displist;
  
  while (running) {
#ifdef UBIT_WITH_GL
    if (UAppli::isUsingGL()) glFlush(); // necessaire ici en mode GL
#endif

    while (running) {
      static XEvent e;  // !!static!!
      bool e_found = false;
      int e_count = 0;
      
      for (unsigned int k = 0; k < displist.size(); ++k) {
        UDispX11* nd = (UDispX11*)displist[k];
        
        if ((e_count = XPending(nd->sys_disp)) <= 0)    // fct non bloquante
          continue;
        
        e_found = true;
        XNextEvent(nd->sys_disp, &e);   // fct bloquante
        
        long t = UAppli::getTime();
        //cerr << "MainLoop: time " << t << " count " << e_count <<endl;
        
        if (e.type != MotionNotify || e_count <= 1
            || t - nd->app_motion_time < UAppli::impl.app_motion_lag
            || long(e.xmotion.time) - nd->nat_motion_time > UAppli::impl.nat_motion_lag) {
          
          nd->app_motion_time = t;
          if (e.type == MotionNotify) nd->nat_motion_time = e.xmotion.time;
          
          //cerr << "MainLoop: before dispatch " << e.type  << " "<< e_count << endl;
          nd->dispatchEvent(&e);
          // !!! ca devrait etre lie au disp !!!
          if (UAppli::impl.request_mask) UAppli::impl.processPendingRequests();
        }
        //else
        //  cerr << "trash event " << e.type  << " apptime " << t - nd->appli_time 
        //       << " xtime " << e.xmotion.time - nd->xmotion_time << endl;
      } //endfor(k)
      
      // pour TOUS les disps
      //if (UAppli::impl.request_mask) UAppli::impl.processPendingRequests();
      
      if (!e_found) break;  // continuer jusqu'au select
    } //endwhile
    
    if (!running) break;
    xerror_count = 0;  // voir note plus haut
    
#ifdef UBIT_WITH_GL
    if (UAppli::isUsingGL()) glFlush(); // necessaire apres dispatch
#endif

    // ** sources and timers
    fd_set read_set;
    FD_ZERO(&read_set);
    
    int maxfd = 0;
    for (unsigned int k = 0; k < displist.size(); ++k) {
      int xconnection = ((UDispX11*)displist[k])->xconnection;
      FD_SET(xconnection, &read_set);
      maxfd = std::max(maxfd, xconnection);
    }
    
    if (UAppli::impl.sources) a.resetSources(UAppli::impl.sources, read_set, maxfd);
    
    struct timeval delay;
    bool has_timeout = false;
    UTimerImpl::Timers& timers = UAppli::impl.timer_impl.timers;

    // NB: delay can be (0,0)
    if (timers.size() > 0) has_timeout = UAppli::impl.timer_impl.resetTimers(delay);
    
    // bloquer tant que: 
    // rien sur xconnection, rien sur sources, timeouts pas atteints
    int has_input = ::select(maxfd+1,
                             &read_set, //read
                             null,      //write
                             null,      //except
                             (has_timeout ? &delay : null));
    if (has_input < 0) {
      if (errno == EINTR || errno == EAGAIN) errno = 0;
      UAppli::warning("UDispX11::startLoop","error in select()");
      a.cleanSources(a.sources); // remove invalid sources
    }
    
    else {
      if (has_input > 0) {	// source event
        if (a.sources) a.fireSources(a.sources, read_set);
        if (a.request_mask) a.processPendingRequests();
      }
      
      if (has_timeout) {	// timeout event
        if (timers.size() > 0) UAppli::impl.timer_impl.fireTimers();
        if (a.request_mask) a.processPendingRequests();
      }
    }
  }
}

// =============================================================================
// ==================================================== [Ubit Toolkit] =========

#define FLOW(E,T) obtainFlow(E->T.state, E->T.subwindow)

static int getButton(XEvent* sev) {
  switch (sev->xbutton.button) {
    case 1: return UMouseEvent::LeftButton;
    case 2: return UMouseEvent::MidButton;
    case 3: return UMouseEvent::RightButton;
  }
  return 0;
}

static void getKey(XEvent* sev, unsigned int& keycode, unsigned short& keychar) {
  int count = 0;
  KeySym keysym = 0;
  char buf[1] = "";
  // Attention (X11 doc) :
  // The creation of XComposeStatus structures is implementation-dependent;
  // a portable program must pass NULL for this argument.
  // XComposeStatus compose;
  // int count = XLookupString(keyevent, buf, sizeof(buf), &keysym, &compose);
  
  // nb: XmbXLookupString() si on voulait du multibyte (wide chars)
  // ATT: XmbXLookupString() ne peut PAS etre appele sur Release !
  count = XLookupString(&(sev->xkey), buf, sizeof(buf), &keysym, NULL);
  
  switch (keysym) {
    case XK_Shift_R: keycode = XK_Shift_L; break;
    case XK_Control_R: keycode = XK_Control_L; break;
    case XK_Meta_R: keycode = XK_Meta_L; break;
    case XK_Alt_R : keycode = XK_Alt_L; break;
    default: keycode = keysym; break;
  }
  
  keychar = (count > 0) ? buf[0] : 0;
}

// -----------------------------------------------------------------------------

static void onSelection(UDisp* nd, UWin* win, UView* winview, XEvent* sev) {
  // NB: no time, no state!
  UEventFlow* f = nd->obtainFlow(0,0);   // ????
  switch (sev->type) {
    case SelectionClear: {      // effacer la selection
      if(f->getSelection()) f->getSelection()->clear();
    } break;
    case SelectionRequest: {
      // une autre application demande a obtenir la valeur de la
      // selection courante (detenue par 'this' appli)
      nd->copySelectionCB(sev);
    } break;
    case SelectionNotify:
      // ce NatDisp a demande le contenu de la selection
      // en appellant askServerSelection()
      nd->pasteSelectionCB(sev);
      break;
  }
  if (win->hasCallback(UOn::SYSWM_CB)) {
    USysWMEvent e(UOn::sysWM, winview, sev);
    win->fire(e);
  }  
}

// -----------------------------------------------------------------------------
// a revoir: le cas ou les XChangeProperty sont trop proches et s'accumulent
  
static void onMessage(UDispX11* nd, XEvent* xev) {
  UMessagePortMap* messmap = UAppli::getMessagePortMap();
  if(!messmap) return;
  char* buf = null;
  unsigned char* prop = null;
  
  if (xev->type == ClientMessage) {
    xev->xclient.data.b[sizeof(xev->xclient.data.b) - 1] = 0;
    buf = xev->xclient.data.b;
  }
  
  else if (xev->type == PropertyNotify) {
    Atom UMS_MESSAGE = nd->getAtoms().UMS_MESSAGE;
    long req_offset = 0;
    long req_length = XMaxRequestSize(xev->xproperty.display);
    Atom type;
    int format;
    unsigned long nitems = 0, bytes_after = 1;
    
    // NB: getProperty rajoute toujours le 0 final d'une string retourne
    // dans prop_return (meme si elle est nulle)
    Bool req_stat =
    XGetWindowProperty(xev->xproperty.display, xev->xproperty.window,
                       UMS_MESSAGE,
                       req_offset, req_length,
                       True, // delete property and generate PropertyNotify
                       // if bytes_after_return is zero
                       XA_STRING,    // req_type
                       &type, &format, &nitems, &bytes_after,
                       &prop);
    if (req_stat == Success && nitems > 0 && type == XA_STRING && format == 8)
      buf = (char*)prop;
  }
  
  messmap->fireMessagePort(buf);
  if (prop) XFree(prop);
}
  
// ==================================================== [Ubit Toolkit] =========

void UDispX11::dispatchEvent(XEvent* sev) {
  Window event_win = sev->xany.window;
  UWin* win = null;
  {   // retrieve the window
    HardwinList::iterator c = hardwin_list.begin();     // NB: on pourrait utiliser une MAP
    HardwinList::iterator c_end = hardwin_list.end();
    for ( ; c != c_end; ++c) {
      UHardwinX11* hw = static_cast<UHardwinX11*>(*c);
      if (hw && hw->sys_win == event_win && hw->win) {win = hw->win; break;}
    }
  }
  if (!win) return;
  UView* winview = win->getWinView(this);
  
  switch(sev->type) {
    
    case MotionNotify: {
      //cerr << "XMOTION evwin "<< (event_win) << " win " << win 
      //<< " xy " << sev->xmotion.x << " " << sev->xmotion.y << endl; 
      UEventFlow* f = FLOW(sev, xmotion);
      UPoint win_pos(sev->xmotion.x, sev->xmotion.y);
      UPoint scr_pos(sev->xmotion.x_root, sev->xmotion.y_root);
      f->mouseMotion(winview, sev->xmotion.time, sev->xmotion.state, win_pos, scr_pos);
    } break;
      
    case ButtonPress: {
      UEventFlow* f = FLOW(sev, xbutton);
      UPoint win_pos(sev->xbutton.x, sev->xbutton.y);
      UPoint scr_pos(sev->xbutton.x_root, sev->xbutton.y_root);
      
      if (sev->xbutton.button == 4)
        f->wheelMotion(winview, sev->xbutton.time, sev->xbutton.state, 
                    win_pos, scr_pos, 0/*type*/, +UWheelEvent::WHEEL_DELTA);
      else if (sev->xbutton.button == 5)
        f->wheelMotion(winview, sev->xbutton.time, sev->xbutton.state, 
                       win_pos, scr_pos, 0/*type*/, -UWheelEvent::WHEEL_DELTA);
      else
        f->mousePress(winview, sev->xbutton.time, 
                      // the pressed button must be included!
                      sev->xbutton.state | getButton(sev), 
                      win_pos, scr_pos, getButton(sev));
    } break;
      
    case ButtonRelease:
      // NB: dont send a release for mouse wheel: this would close opened menus
      // and other strange things
      if (sev->xbutton.button != 4 && sev->xbutton.button != 5) {
        UEventFlow* f = FLOW(sev, xbutton);
        UPoint win_pos(sev->xbutton.x, sev->xbutton.y);
        UPoint scr_pos(sev->xbutton.x_root, sev->xbutton.y_root);
        f->mouseRelease(winview, sev->xbutton.time, 
                        // the pressed button must be excluded!
                        sev->xbutton.state & ~getButton(sev), 
                        win_pos, scr_pos, getButton(sev));
      } break;
      
    case KeyPress: {
      unsigned short keychar;
      unsigned int keycode;
      UEventFlow* f = FLOW(sev, xkey);
      UPoint win_pos(sev->xkey.x, sev->xkey.y);
      getKey(sev, keycode, keychar);
      
      //19jan08: the pressed MODIFIERS SHOULD be included!!!@@@
      f->keyPress(winview, sev->xkey.time, sev->xkey.state, keycode, keychar);
    } break;
      
    case KeyRelease: {
      unsigned short keychar;
      unsigned int keycode;
      UEventFlow* f = FLOW(sev, xkey);
      UPoint win_pos(sev->xkey.x, sev->xkey.y);
      // ATT: XmbXLookupString() ne peut PAS etre appele sur Release !
      // (si decodeKey appelait XmbXLookupString)
      getKey(sev, keycode, keychar);
      
      //19jan08: the pressed MODIFIERS SHOULD be excluded!!!@@@
      f->keyRelease(winview, sev->xkey.time, sev->xkey.state, keycode, keychar);
    } break;
      
    case EnterNotify: {
      if (sev->xcrossing.window != None) {     // PBM MULTIFLOW ??
        // la fenetre dans laquelle on rentre prend le focus: c'est obligatoire
        // pour les menus sinon ils ne prennent pas les entrees clavier
        // (NB: les erreurs X doivent etre desactivees sinon ca plante !)
        
        // !!! NB: effets desagraeble sur les title des windows !
        // est-ce encore utile ?
        // XSetInputFocus(sev->xcrossing.display, sev->xcrossing.window,    @@@@@!!!!!
        //                RevertToPointerRoot/*ex: RevertToParent*/, CurrentTime);
      }
      
      // !!!! COMPLETER: EVENTS PERDUS !!!!
      UEventFlow* f = FLOW(sev, xcrossing);
      //UPoint win_pos(sev->xcrossing.x, sev->xcrossing.y);
      f->winEnter(winview, sev->xcrossing.time);
      // ATT: ne PAS faire: win->boxMotion(); incompatible avec gestion des grabbedMenu !
    } break;
      
    case LeaveNotify: {
      UEventFlow* f = FLOW(sev, xcrossing);
      //UPoint win_pos(sev->xcrossing.x, sev->xcrossing.y);
      f->winLeave(winview, sev->xcrossing.time);
    } break;
      
    case FocusIn:
    case FocusOut:
      //onFocus(winview, winview, sev);
      break;
      
    case Expose:
      onPaint(winview, sev->xexpose.x, sev->xexpose.y, sev->xexpose.width, sev->xexpose.height);
      break;
      
    case GraphicsExpose:
      // these events are generated when XCopyArea or XCopyPlanes is called
      // and a part of the view is obscured. They are useful for repainting
      // these parts when scrolling data.
      // see fct. copyArea() in ugraph.hpp for details
      onPaint(winview, sev->xgraphicsexpose.x, sev->xgraphicsexpose.y,
              sev->xgraphicsexpose.width, sev->xgraphicsexpose.height);
      break;
      
      //case NoExpose: onPaint(winview, v, sev); break;
    case ConfigureNotify:
      onResize(winview, UDimension(sev->xconfigure.width, sev->xconfigure.height));
      break;
      
    case MapNotify:
      // permet de ne pas prendre en compte les Configure Events
      // tant que window pas visible (voir onResize)
      win->setShowable(true);
      win->setMapped(true);
      if (win->hasCallback(UOn::WIN_STATE_CB)) {
        UWinEvent e(UOn::winState, winview, sev->type, 0/*state*/);
        win->fire(e);
      }
      break;

    case UnmapNotify:
      win->setShowable(false);
      win->setMapped(false);
      if (win->hasCallback(UOn::WIN_STATE_CB)) {
        UWinEvent e(UOn::winState, winview, sev->type, 0/*state*/);
        win->fire(e);
      }
      break;
      
    case SelectionClear:
    case SelectionRequest:
    case SelectionNotify:
      onSelection(this, win, winview, sev);
      break;
      
    case ClientMessage: {
      //UEventFlow* f = obtainFlow(0,0);      
      if (sev->xclient.message_type == atoms.UMS_MESSAGE) {
        //UMessageEvent e(UOn::message, v, sev);
        //messmap->fireMessagePort(e);
        onMessage(this, sev);
      }
      // on a clique' sur le bouton Close du WM
      else if (sev->xclient.message_type == atoms.WM_PROTOCOLS
               && ((Atom)sev->xclient.data.l[0]) == atoms.WM_DELETE_WINDOW) {
        win->close(0);   // hides or kills the UWin depending on actual subclass
      }        
      else if (win->hasCallback(UOn::SYSWM_CB)) {
        USysWMEvent e(UOn::sysWM, winview, sev);
        win->fire(e);
      }
    } break;
      
    case PropertyNotify: {
      //UEventFlow* f = obtainFlow(0,0);
      if (sev->xproperty.atom == atoms.UMS_MESSAGE) {
        //UMessageEvent e(UOn::message, v, sev);
        //messmap->fireMessagePort(e);
        onMessage(this, sev);
      }
      else if (win->hasCallback(UOn::SYSWM_CB)) {
        USysWMEvent e(UOn::sysWM, winview, sev);
        win->fire(e);
      }
    } break;
      
    case DestroyNotify:		// la fenetre est detruite
      //cerr<<"DestroyNotify"<< endl;
      break;

    case ColormapNotify:
      //fprintf(stderr, ">ColormapNotify\n");
      break;
      
    default:
      if (win->hasCallback(UOn::SYSWM_CB)) {
        //UEventFlow* f = obtainFlow(0,0);
        USysWMEvent e(UOn::sysWM, winview, sev);
        win->fire(e);
      }
      break;
  }
}

// ==================================================== [Ubit Toolkit] =========

//!ATT ces fcts doivent etre appellee sur des Button events

static long MaxSelRequestSize(Display *disp) {
  long l = XMaxRequestSize(disp);
  if (l > 65536) l = 65536;  // min
  return (l << 2) - 100;
}

/* ==================================================== ===== ======= */
// COPY SELECTION

// en fait ca copie pas la selection mais ca dit au serveur ou elle est
// (= quelle appli la controle)

bool UDispX11::setSelectionOwner(UMouseEvent& e) {
  /*
   XButtonEvent* xev = &(((USysEvent)e.sys_event)->xbutton);
   if (xev->type != ButtonPress && xev->type != ButtonRelease) {
     UAppli::warning("UDispX11::setSelectionOwner","Wrong event type: %d", xev->type);
     return false;
   }
   Display* disp = xev->display;
   Window owner = xev->window;
  */
  Display* disp = ((UDispX11*)e.getDisp())->getSysDisp();
  Window owner = ((UHardwinX11*)e.getWin()->getHardwin())->getSysWin();

  XSetSelectionOwner(disp, XA_PRIMARY, owner, e.getWhen() /*xev->time*/);
  return (XGetSelectionOwner(disp, XA_PRIMARY) == owner);
}

/* ==================================================== ======== ======= */
// copie reellement la selection (ou plus exactement envoie son contenu a
// l'appli qui la demande)
// cette fonction n'est pas directement consecutive a setSelection()
// mais au fait qu'ensuite une appli va effectivement demander le
// contenu de cette selection
// l'event doit etre un XSelectionRequestEvent

void UDispX11::copySelectionCB(void* xev) {
  // on verifie qu'il s'agit bien de la section PRIMARY et le type
  // demande par le requestor (STRING) puis:
  // -- on change la property adequate avec la nouvelle valeur
  //    de la selection
  // -- puis on envoie un XSelectionEvent de confirmation ou d'echec
  //    DANS TOUS LES CAS y compris si on n'a rien change!

  // NOTES:
  // -- il peut y avoir plusieurs echanges: certaines applis peuvent
  //    d'abord demander les donnes sous un type specifique (par exple
  //    propre a Qt, etc.), puis, en cas d'echec (si on a renvoye
  //    un SelectionEvent d'echec), redemander la selection sous une
  //    autre forme
  // -- selection != property
  //    selection = PRIMARY = 1 tandis que property depend de l'appli
  //    qui demande les donnees

  XSelectionRequestEvent *xsel = (XSelectionRequestEvent*)xev;
  bool property_changed = false;

  if (xsel->selection == XA_PRIMARY && xsel->target == XA_STRING) {
    // troncature si la taille de selval est trop grande
    int req_max_length = MaxSelRequestSize(xsel->display);
    int nitems = getCopyBuffer().length();
    if (nitems > req_max_length) nitems = req_max_length;

    // NOTE: pour bien faire il faudrait faire une boucle pour
    // pouvoir envoyer les messages de longue taille qui excedent
    // les limites du serveur X (ie. MaxSelRequestSize)

    // NB: inutile de copier le 0 final de la string: getProperty le rajoutera

    XChangeProperty(xsel->display, xsel->requestor,
                    xsel->property, xsel->target,
                    8/*format*/, PropModeReplace/*mode*/,
                    (unsigned char*)getCopyBuffer().c_str(), nitems);
    property_changed = true;
  }

  // On envoie un XSelectionEvent de CONFIRMATION ou d'ECHEC
  // DANS TOUS LES CAS y compris si on n'a rien change!

  XSelectionEvent event_send;
  event_send.type       = SelectionNotify;
  event_send.serial     = xsel->serial;
  event_send.time       = xsel->time;
  event_send.send_event = xsel->send_event;
  event_send.display    = xsel->display;
  event_send.requestor  = xsel->requestor;
  event_send.selection  = xsel->selection;
  event_send.target     = xsel->target;

  // send and event with property == None if nothing changed
  if (property_changed)
    event_send.property = xsel->property;  // cas OK
  else
    event_send.property = None;            // cas d'echec

  XSendEvent(xsel->display, xsel->requestor,
             0,  //propagate
             0,  //event_mask
             (XEvent*)&event_send);
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// PASTE SELECTION

// demande de recuperer la selection au serveur. comme elle n'est pas stockee
// par le serveur mais par une appli quelconque (event. en reseau) , cette
// appli va etre notifiee, va copier effectivement les donnees ce qui va
// *ensuite* entrainer l'appel de pasteSelectionCB
//
// --> pasteSelectionRequest ne recupere PAS tout de suite le contenu
//     de la selection !
//
// (the server will send a SelectionNotify event, that will call pasteSelectionCB)

void UDispX11::pasteSelectionRequest(UMouseEvent& e) {
  /*
   XButtonEvent* xev = &(((USysEvent)e.sys_event)->xbutton);
   if (xev->type != ButtonPress && xev->type != ButtonRelease) {
     UAppli::warning("UDispX11::pasteSelectionRequest","Wrong event type: %d",xev->type);
     return;
   }
   Display* disp = xev->display;
   Window requestor = xev->window;
  */
  Display* disp = ((UDispX11*)e.getDisp())->getSysDisp();
  Window requestor = ((UHardwinX11*)e.getWin()->getHardwin())->getSysWin();

  XConvertSelection(disp, XA_PRIMARY/*selection*/, XA_STRING/*target*/, 
                    atoms.UMS_WINDOW, // property
                    requestor, 
                    /*xev->time*/ e.getWhen());
  
  // ceci entraine la generation d'un SelectionNotify event par le
  // serveur si tout marche bien
  // ce SelectionNotify event est ensuite recu par configureEH qui
  // a alors a charge de realiser l'operation d'insertion
}

/* ==================================================== ======== ======= */
// recupere effectivement le contenu de la selection.
// l'appel de cette fct est provoque par pasteSelectionRequest()
// the XEvent must be a XSelectionEvent event.

void UDispX11::pasteSelectionCB(void* xev) {
  XSelectionEvent* xsel = (XSelectionEvent*)xev;
  UStr* paste_str = null;
  int paste_pos = 0;
  
  if (xsel->property != None && xsel->selection == XA_PRIMARY
      && xsel->target == XA_STRING 
      && (paste_str = getPasteTarget(paste_pos))
      ) {
    long req_offset = 0;
    long req_length = MaxSelRequestSize(xsel->display);
    Atom type_return;
    int format_return;
    unsigned long nitems_return = 0, bytes_after_return = 1;
    unsigned char *prop_return = null;

    // NOTE: pour bien faire il faudrait faire une boucle pour pouvoir
    // recuperer les messages de longue taille qui excedent les limites
    // du serveur X (ie. MaxSelRequestSize). Dans ce cas il faudrait
    // alors appeler XGetWindowProperty() avec delete_property == FALSE
    // puis appeler XDeleteProperty() qunad tout a ete recupere

    // XGetWindowProperty always allocates one extra byte in prop_return
    // (even if the property is zero length) and sets it to zero so that
    // simple properties consisting of characters do not have to be
    // copied into yet another string before use.

    Bool req_stat =
      XGetWindowProperty(xsel->display, xsel->requestor, xsel->property,
                         req_offset, req_length,
                         True,/*delete property*/
                         AnyPropertyType,/*req_type= Any type*/
                         &type_return, &format_return,
                         &nitems_return, &bytes_after_return,
                         &prop_return);

    if (req_stat == Success
        && nitems_return > 0
        && prop_return != null
        && type_return == XA_STRING
        && format_return == 8
        ) {
      paste_str->insert(paste_pos, (char*)prop_return);
    }

    //desallouer la memoire allouee par XGetWindowProperty
    if (prop_return != null) XFree((char*)prop_return);
  }

  clearPasteTarget();
  // DEVRAIT egalement debloquer appli !!!!
}


}
#endif  // UBIT_WITH_X11


