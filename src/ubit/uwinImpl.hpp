/************************************************************************
 *
 *  uwinImpl.hpp: ubit windows implementation
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

#ifndef _uwinImpl_hpp_
#define	_uwinImpl_hpp_ 1
#include <ubit/uwin.hpp>
namespace ubit {
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /* [Impl] Window list.
   * Used for implementation purpose, UWinList elements are not counted has parents 
   * when auto deleting children. They are considered as genuine lists, not viewable
   * elements when displayingviews
   */
  class UWinList : public UElem {
  public:
    UCLASS(UWinList)
    UWinList(const UArgs& a = UArgs::none) : UElem(a) {}
    virtual int getDisplayType() const {return WINLIST;}   // redefined
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  class UWinImpl {
  public:
    /// window types.
    enum WinType {
      NONE,
      //NOTUSED,
      EXTWIN,
      PIXMAP,
      HARDWIN = 8,     // can't be instanciated
      SUBWIN,
      TOPLEVEL = 16,   // can't be instanciated
      FRAME,
      MAINFRAME, 
      DIALOG, 
      MENU
    };
    virtual ~UWinImpl() {};
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /* [Impl] Soft (= simulated) Window.
   * abstract class: @see UHardwinX11, UHardwinGLUT, etc.
   */
  class USoftwinImpl : public UWinImpl {
  public:
    USoftwinImpl(UWin&);
    virtual ~USoftwinImpl();
    
    UPos& pos() {return *ppos;}
    
    UView* getActualView(UView* winviews);
    void setActualView(UView* winview);
    
    void doUpdate(const UUpdate&, UWin*, UView* winview);
    
  private:
    uptr<UPos> ppos;
    UView* actual_view;  
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /* [Impl] Hard (= real) Window.
   * abstract class: @see UHardwinX11, UHardwinGLUT, etc.
   */
  class UHardwinImpl : public UWinImpl {
  public:  
    virtual ~UHardwinImpl();
    
    UDisp* getDisp() const {return disp;}
    UWin*  getWin()  const {return win;}
    WinType getWinType() const {return WinType(wintype);}  
    UGlcontext* getGlcontext() const {return glcontext;}
    
    bool isPixmap()   const {return wintype == PIXMAP;}  
    bool isSubwin()   const {return wintype == SUBWIN;}
    bool isHardwin()  const {return wintype > HARDWIN;}
    bool isTopLevel() const {return wintype > TOPLEVEL;}
    
    virtual void realize(WinType, float w, float h) = 0;
    virtual bool isRealized() const = 0;
    
    virtual void show(bool = true) = 0;
    virtual void toBack() = 0;
    virtual void toFront() = 0;
    
    virtual UPoint getScreenPos() const = 0;
    /**< returns window position relatively to the screen.
     * contrary to getPos(), this function returns the screen position for all windows
     * (even for subwindows embedded in another window)
     */
    
    virtual UPoint getPos() const = 0;  
    /**< returns window position relatively to its parent window (or the screen).
     * the screen position is returned for first level windows (ie. windows that
     * are not subwindows embedded in another window)
     */
    
    virtual void setPos(const UPoint&) = 0;
    /**< changes window position relatively to its parent window (or the screen).
     * changes position relatively to the screen for first level windows (ie. windows 
     * that are not subwindows embedded in another window)
     */
    
    virtual UDimension getSize() const = 0;
    ///< returns window size.
    
    virtual void setSize(const UDimension&) = 0;
    ///< changes window size.
    
    virtual UStr getTitle() const = 0;
    ///< returns window title.
    
    virtual void setTitle(const UStr&) = 0;
    ///< changes window title.
    
    virtual UStr getIconTitle() const = 0;
    ///< returns icon title.
    
    virtual void setIconTitle(const UStr&) = 0;  
    ///< changes icon title.
    
    virtual void setCursor(const UCursor*) = 0;
    ///< changes window cursor (can be null).
    
    virtual void setClassProperty(const UStr& instance_name, const UStr& class_name) = 0;
    ///< changes the WM_CLASS property when X11 is used.
    
  private:
    UHardwinImpl(const UHardwinImpl&);
    UHardwinImpl& operator=(const UHardwinImpl&);
    
  protected:
    UHardwinImpl(UDisp*, UWin*);
    
    // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#ifndef NO_DOC
  public:
    UChildren* getSoftwinList();
    UWinList*  getSoftwins();
    UWinList*  obtainSoftwins();
    void addSoftwin(UWin* softwin, UWin* hardwin, UDisp* disp, bool add_to_the_end);
    // add to the end add_to_the_end is true, to the beginning otherwise
    void removeSoftwin(UWin* softwin);
    
    void doUpdate(const UUpdate&, UWin*, UView* winview);
    void doUpdateImpl(const UUpdate&, UWin*, UView* winview, const UDimension* size);
    
  protected:
    friend class UDisp;
    friend class UWin;
    friend class UView;
    friend class UGraph;
    friend class UPaintEvent;
    friend class UInputEvent;
    friend class UAppliImpl;
    friend class UGlcanvas;
    unsigned char wintype;       // one of UWinImpl::WinType
    unsigned char must_update;   // see UAppliImpl::processUpdateRequests
    UDisp* disp;
    UWin* win;
    UChildren* softwin_list;
    // glcanvas windows and GLUT window have their own UGlcontext
    UGlcontext* glcontext;
#endif
  };  
   
}
#endif



