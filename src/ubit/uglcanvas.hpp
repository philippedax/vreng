/************************************************************************
 *
 *  uglcanvas.hpp: a widget for rendering OpenGL graphics (requires X11 and OpenGL)
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

#ifndef _uglcanvas_hpp_
#define	_uglcanvas_hpp_ 1
#include <ubit/udefs.hpp>
#if UBIT_WITH_GL

# include <ubit/usubwin.hpp>
# include <ubit/ugl.hpp>
namespace ubit {
  
  /** A widget for rendering OpenGL graphics.
   * Notes:
   * - this widget creates a (shared) OpenGL visual and its (own) openGL context 
   *   the first time it is shown. If a GLvisual has already been created (by 
   *   another widget or any other mean) it is used by this widget.
   * - the depth (the "bpp") of the GLvisual is the depth that is used for 
   *   displaying the widgets of the UAppli. It can be set by calling: 
   *   UAppli::conf.setBpp(int) *before* the UAppli instance is created
   * - the other widgets of the UAppli can't be rendered by using OpenGL if
   *   UGlcanvas is used (hence UAppli::conf.useGLRender() won't work properly)
   *
   */
  class UGlcanvas : public USubwin {
  public:
    UCLASS(UGlcanvas)
        
    UGlcanvas(UArgs = UArgs::none);
    virtual ~UGlcanvas();
    
    //void shareContextResources(UGlcontext* = DEFAULT_GC);
    /**< specifies that the resources of the GL context are shared with this argument.
     * This function must be called immediately after the UGlcanvas is created:
     * - the resources of the GL context of this canvas will be shared with those
     *   of the given argument if it encapsulates a GL context.
     * - the default argument DEFAULT_GC indicates that the ressources will be shared
     *   with those of the default GC of the application (@see UDisp::getDefaultGC()).
     *   This will have no effect if the application was not launched in GL mode
     */
    
    virtual void makeCurrent();
    /**< makes this widget the current widget for OpenGL (GL will use its UGlcontext).
     * there is generally no need to call makeCurrent() explictely because this is
     * done automatically before initGL(), resizeGL() and paintGL()
     */
    
    virtual void swapBuffers();
    /**< swaps the screen content with the off-screen buffer.
     * this function only works in double buffer mode (this is the default, see UConf).
     * There is generally no need to call swapBuffers() explictely because this is
     * done automatically after paintGL(). 
     */
    
    bool isInit() const {return is_init;}
    ///< returns true if the widgets has been initialized (and the GL context created).
    
    UGlcontext* getGlcontext() const;
    ///< returns the GC that is associated this window for drawing in the canvas area.
    
  protected:
    virtual void initGL() {}
    /**< called once before the first call of paintGL() or resizeGL().
     * Note: makeCurrent() is automatically called before initGL()
     */
    
    virtual void paintGL(UPaintEvent&) {}
    /**< called when the widget is repainted.
     * Notes:
     * - makeCurrent() is automatically called before paintGL()
     * - swapBuffers() is automatically called after paintGL().
     */
    
    virtual void resizeGL(UResizeEvent&, int w, int h) {}
    /**< called when the widget is resized.
     * Note: makeCurrent() is automatically called before resizeGL()
     */
    
    // - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:  
    virtual bool realize();
    
    virtual void initImpl();
    ///< Impl: init the widget and calls initGL().
    
    virtual void paintImpl(UPaintEvent&);
    ///< Impl: calls makeCurrent() then paintGL() and swapBuffers() (when applicable).
    
    virtual void resizeImpl(UResizeEvent&);    
    /**< Impl: calls makeCurrent() then resizeGL().
     * note that this function first calls USubwin::resizeImpl().
     */
    
  private:
    bool is_init, share_glresources;
    //UGlcontext *glcontext, *shared_res_ctx;
  };
  
  inline UGlcanvas& uglcanvas(UArgs a = UArgs::none) {return *new UGlcanvas(a);}
  ///< shortcut function that creates a new UGlcanvas.
  
}
#endif // UBIT_WITH_GL
#endif
