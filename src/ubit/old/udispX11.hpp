/*************************************************************************
 *
 *  UDispX11.hpp : Native Layer: platform dependent implementation
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#ifndef UDispX11_hpp
#define	UDispX11_hpp 1

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <ubit/ugl.hpp>
#include <ubit/udisp.hpp>
#include <ubit/uhardwinX11.hpp>
#include <ubit/uglcontext.hpp>
namespace ubit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** X11 cursor.
*/
struct UCursorImpl {
  Cursor cursor;
};
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** X11 atoms.
*/
struct UAtomsX11 {
  Atom PRIMARY_SELECTION, SECONDARY_SELECTION,
  WM_PROTOCOLS, WM_DELETE_WINDOW, WM_TAKE_FOCUS,
  UMS_WINDOW, UMS_MESSAGE;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** X11 display.
*/
class UDispX11 : public UDisp {
public:
  UDispX11(const UStr&);
  virtual ~UDispX11();

  virtual void   setPointerPos(const UPoint& screen_pos);
  virtual UPoint getPointerPos() const;
  virtual int    getPointerState() const;  
  virtual bool   grabPointer(const UCursor*);
  virtual void   ungrabPointer();  
  virtual bool   pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                            UCursor* cursor = null, UCall* callback = null);
  
  virtual unsigned long createColorPixel(const URgba&);  // for 2D_GRAPHICS
  virtual UHardwinX11*  createWinImpl(UWin*);
  virtual UCursorImpl*  createCursorImpl(int curtype);
  virtual void          deleteCursorImpl(UCursorImpl*);
  
  Display* getSysDisp()       const {return sys_disp;}
  Screen*  getSysScreen()     const {return sys_screen;}
  XVisualInfo* getSysVisual() const {return sys_visual;}
  Window   getSysRoot()       const;
  Window   getSysFrame()      const;
  Colormap getSysColormap()   const {return sys_cmap;}
  
  const UAtomsX11& getAtoms() const {return atoms;}
  /**< returns useful atoms.
  * - UMS_WINDOW: a X window that has this property will receive UMS events.
  * - UMS_MESSAGE: the property for exchanging messages between applications.
  * see also: umsproto.hpp and the UMS server (available in directory ubit/ums).
  */

# if UBIT_WITH_GL
  XVisualInfo* createGlvisual(int r, int g, int b, int a, int depth);
  ///< [GL only] creates a new GL visual.

  GLXContext createGlcontext(UGlcontext* shareList);
  ///< [GL only] creates a GL context.
  
  void destroyGlcontext(GLXContext);
  ///< [GL only] destroys a GL context.
# endif
    
  bool setVisual(XVisualInfo* visual, Colormap = None);
  /**< [X11 only] changes the visual of this display.
    * NOTE: this function must be called before the windows are realized.
    * Arguments:
    * - the visual, which is not copied and should NOT be destroyed
    * - the Colormap is automatically created if not specified
    * - returns true if this setting succeded 
    */
  
  bool setVisual(int bpp, Colormap = None);
  /**< [X11 only] changes the visual of this display according to the requested BPP.
    * NOTE: this function must be called before the windows are realized.
    * Arguments:
    * - the number of bit per pixel (BPP)
    * - the Colormap is automatically created if not specified
    * - returns true if this setting succeded 
    */
  
 protected:
  friend class UAppli;
  friend class UAppliImpl;
  friend class UGraph;
  friend class UHardwinImpl;
  friend class UHardwinX11;
  
  Display* sys_disp;
  Screen* sys_screen;
  XVisualInfo* sys_visual;
  Colormap sys_cmap;
  int xconnection;
  bool xsync;
  Window default_pixmap;
  UAtomsX11 atoms;
  UHardwinImpl* mainframe;    // for getting bpp, for drawing pixmaps, etc.
  
# if UBIT_WITH_GL
  // glvisual = sys_visual in GLMode; null in X11mode except if there is a glcanvas
  XVisualInfo* glvisual;
# endif

  virtual void startAppli();
  virtual void quitAppli();
  virtual void startLoop(bool main_loop);   
  virtual void quitLoop(bool main_loop);
  
  virtual bool setSelectionOwner(UMouseEvent&);   
  virtual void copySelectionCB(void* system_event);
  virtual void pasteSelectionRequest(UMouseEvent&);
  virtual void pasteSelectionCB(void* system_event);
  
  void dispatchEvent(XEvent*);    // dispatches events to widgets.
};


}
#endif
#endif // UBIT_WITH_X11
