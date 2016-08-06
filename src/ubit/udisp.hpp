/*************************************************************************
 *
 *  udisp.cpp: Graphical Display (may be remotely located)
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

#ifndef _udisp_hpp_
#define	_udisp_hpp_ 1
#include <list>
#include <ubit/udefs.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uelem.hpp>
#include <ubit/ugeom.hpp>
namespace ubit {
  
class UGraphImpl;
class UCursorImpl;
  
/** Display Context.
  * A UAppli can open windows on several displays, which can be on remote machines
  * when X11 is used as a windowing systems. A UDisp contains the data related to 
  * a given display (or to a specific screen of a given display when screens are
  * logically separated, ie. when a window cannot move freely from one screen
  * to another one).
  *
  * UWin objects (and UFrame, UDialog, UMenu subclasses) can be attached to a
  * specific UDisp by using UDisp::add(). Widgets that are contained in several 
  * UWin objects ("contained" meaning they are a child of them) are AUTOMATICCALY
  * REPLICATED and synchronized on all these windows.
  *
  * UAppli derives from UDisp. The UAppli object (which must be unique) represents
  * the default display.
  */
class UDisp {
public:
  static UDisp* create(const UStr& display_name);
  /**< creates and opens a new Display.
  * 'display_name' is the name of the X Window server, it must be specified as follows:
  * - hostname:screen_no
  * or just:
  * - hostname  (screen 0 is taken in this case)
  *
  * NOTES:
  * - the UAppli is the default UDisp, its ID number is 0.
  * - the default BPP (number of bits per pixel) is 24, this can be changed globally
  *   for the whole application in UConf or for a given display by calling UDisp(UConf&)
  */
    
  virtual ~UDisp();

  bool isOpened() const {return is_opened;}
  ///< returns true if the Display was sucessufully opened.
  
  int getID() const {return id;}
  /**< returns the ID of this Display.
   * the ID of the default Display is 0 (the default Display is the UAppli).
   */
  
  const UConf& getConf() const {return conf;}
  ///< returns the configuration of this Display.

  int getBpp() const {return bpp;}
  /**< returns the number of bit per pixel of this UDisp.
   * when X11 is used, this bpp may differ from the default BPP of the X11 display.
   */
  
  const UStr& getDisplayName() const {return display_name;}
  ///< returns the name of the UDisp's X11 server (when X11 is used).
    
  int getScreenWidth() const {return screen_width;}
  ///< returns the width of the UDisp's screen.
  
  int getScreenHeight() const {return screen_height;}
  ///< returns the height of the UDisp's screen.
  
  void setPixelPerInch(double pixel_per_inch);
  ///< changes the pixel density (number of pixels per inch).
  
  void setPixelPerMM(double pixel_per_mm);
  ///< changes the pixel density (number of pixels per mm).
       
  // - - - windows - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual void add(UWin&);
  /**< add a window (UFrame, UDialog, etc) to this display.
   * Note that windows are initially hidden, their show() method must be called
   * to make them visible. Moreover, as show() calculates the window size, it should
   * called after adding the window children (see also: UWin::adjustSize()).
   * @see: UAppli::add() and classes UWin, UFrame, UDialog, UMenu.
   */ 
  
  void add(UWin*);
  ///< see add(UWin&).
    
  virtual void remove(UWin&, bool auto_delete);
  /**< remove a window from this display.
   * for explanations on 'auto_delete' see: UElem::remove()
   */ 
    
  void remove(UWin*, bool auto_delete);
  ///< see remove(UWin&, bool auto_delete).
  
  // - - - pointer - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual void setPointerPos(const UPoint& screen_pos) = 0;
  ///< move the pointer to this position on the screen.

  virtual UPoint getPointerPos() const = 0;
  ///< returns the position of the pointer on the screen.

  virtual int getPointerState() const = 0;
  ///< returns the state of the modifier keys and pointer buttons.

  virtual bool grabPointer(const UCursor* = null) = 0;
  /**< starts a physical grab of the native pointer.
    * the UCursor arg is the cursor that is shown during the grab. A default
    * value (such as UCursor::crosshair) is used if this argument is null.
    * returns true if the pointer could be grabbed.
    * BEWARE that grabPointer() may lock the X server! @see ungrabPointer().
    */
  
  virtual void ungrabPointer() = 0;
  /**< ends a physical grab of the native pointer.
    * @see grabPointer() for details.
    */

  virtual bool pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                          UCursor* cursor = null, UCall* callback = null) = 0;
  /**< grabs the pointer and lets the user pick a window interactively with the mouse.
    * (note: this function only works when X11 is used).
    * - returns false if no window was picked
    * - 'x_in_win' and 'y_in_win' are the position of the pointer in the window
    * - 'window' must be previously created by createWinImpl(null). Its content is set
    *   so that it points to teh picked window
    * - 'cursor' specifies the cursor that is shown during the grab operation 
    *   (UCursor::crosshair is used is this arg. is null)
    * - if 'callback' is not null, it is fired when the mouse is pressed, moved, 
    *   released during the grab operation. 'callback' is a callback object created
    *   by one of the ucall<> templates (@see UCall).
    */
    
  // - - - event flow - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  UEventFlow* getChannelFlow(int channel) const;
  /**< returns the Event Flow for this channel on this display (null if not found).
    * 0 is the channel of the native Event Flow on this display. Don't confuse
    * channels with event flow IDs: channels depend on displays, while IDs are
    * unique for the whole UAppli. For instance, if an appli is displayed on 2
    * displays, it will have (at least) 2 event flows, 1 on disp1 and 1 on disp2.
    * The channel of both event flow will be 0, but their IDs will differ.
    * @see: UAppli class and UAppli::getFlow()
    */
  
  UEventFlow* obtainChannelFlow(int channel);
  /**< gets or creates the Event Flow that corresponds to this channel on this display.
    * calls getDispFlow() then creates a new flow if getDispFlow() returns null.
    * @see getDispFlow() for details.
    */
  
  USelection* getChannelSelection(int channel);
  /**< returns the text selection of a given event flow on this display.
    * @see getDispFlow() for details.
    */
  
  // - - - cut & paste - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 
  const UStr& getCopyBuffer() const {return copy_buffer;}
  ///< returns the copy buffer.

  UStr* getPasteTarget(int& pos) const;
  ///< [impl] returns the string (and the pos in this string) where the paste will take place.
    
  void clearPasteTarget();
  ///< [impl] clears the paste target.

  virtual void copySelection(UMouseEvent&, USelection&);
  /**< [impl] copies the content of the selection in the copy buffer and tells the X server we own the X selection.
  * note: there is 1 selection per UEventFlow but only 1 copy_buffer per UDisp
  */
  
  virtual void pasteSelection(UMouseEvent&, UStr* paste_str, int paste_pos);
  /**< [impl] pastes the X selection into this UStr at this pos.
    * ATTENTION: c'est lie a *chaque* disp et c'est ASYNCHRONE: le paste
    * n'est pas fait immediatement mais par appel ulterieur d'un callback
    */
      
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Internals.
#ifndef NO_DOC
public:
  URenderContext* getDefaultContext();  
  void makeDefaultContextCurrentIfNeeded();
  
  void addHardwin(UWin*);
  void removeHardwin(UWin*);
  
  virtual unsigned long createColorPixel(const URgba&) = 0;  // for 2D_GRAPHICS
  virtual UHardwinImpl* createWinImpl(UWin*) = 0;
  virtual UCursorImpl*  createCursorImpl(int curtype) = 0;
  virtual void          deleteCursorImpl(UCursorImpl*) = 0;
  
  virtual UHardFont* getFont(const UFontDesc*);
  /**< returns (possibily creating) the requested font (see NOTES).
   * IMPORTANT NOTES: 
   * - 1. fonts depends on the default Graphics Context (@see getDefaultGC()).
   *   Hence, when OpenGL is used, fonts can only be drawn by using this GC or by
   *   a GC that shares the displaylists with this GC (@see UGraph::createGC)
   * - 2. this function may change the current GC: the default GC will then become
   *   the current GC (by calling getDefaultGC()->makeCurrent()). Programs that use
   *   multiple GC may need to change the current GC after calling getFont().
   */
  
  virtual UHardFont* realizeFont(const UFont&);
  virtual UHardFont* realizeFont(const UFontDesc&);  
  virtual void realizeFontFamily(const UFontFamily&);
  
  // - - - WITHOUT OpenGL 
  
  unsigned long getBlackPixel() const {return black_pixel;}  
  unsigned long getWhitePixel() const {return white_pixel;}
  
  // attention: les masks varient suivant l'architecture (big endian ou little endian).
  unsigned long getRedMask() const {return red_mask;}  
  unsigned long getGreenMask() const {return green_mask;}  
  unsigned long getBlueMask() const {return blue_mask;}  
  int getRedShift()  const {return red_shift;}  
  int getGreenShift()const {return green_shift;}  
  int getBlueShift() const {return blue_shift;}  
  int getRedBits()   const {return red_bits;}  
  int getGreenBits() const {return green_bits;}  
  int getBlueBits()  const {return blue_bits;} 
  static void countBits(unsigned long mask, int& bits, int& shift);
       
  // - - - Event management 
  
  UEventFlow* obtainFlow(unsigned int ev_state, int channel);
  ///< returns the corresponding Event Flow (creates it if does not already exist).
  
  virtual void onPaint(UView* winview, float x, float y, float w, float h);
  virtual void onResize(UView* winview, const UDimension& size);
  void onResizeCB(UView* winview);

  virtual bool setSelectionOwner(UMouseEvent&) {return false;}
  ///< [X11 only] tells the X-server the appli owns the X selection.
  
  virtual void pasteSelectionRequest(UMouseEvent&) {}
  /**< [X11 only] asks the X selection.
    * cette fct ne recupere PAS tout de suite le contenu de la selection
    * mais entrainera un appel ulterieur de pasteSelectionCallback().
    * the UEvent must be a button event. 
    */
  
  virtual void copySelectionCB(void* system_event) {}
  /**< [X11 only] copie reellement la selection
    * (ou plus exactement envoie son contenu a l'appli qui la demande).
    * l'event doit etre un XSelectionRequestEvent
    */
  
  virtual void pasteSelectionCB(void* system_event) {}
  /**< [X11 only] recupere effectivement le contenu de la X selection.
    * l'appel de cette fct est provoque par pasteSelectionRequest()
    * the XEvent must be a XSelectionEvent event.
    */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
private:
  friend class UAppli;
  friend class UAppliImpl;
  friend class UWin;
  friend class UHardwinImpl;
  friend class UView;
  friend class UGraph;
  friend class UEvent;
  friend class UFontFamily;
  friend class UFontMetrics;
  friend class UGlcontext;
  friend class UGlcanvas;
  friend class ULength;
  friend class UFont;
  UDisp(const UDisp&);
  UDisp& operator=(const UDisp&); 
  
protected:
  UDisp(const UStr& display_name);

  virtual void startAppli() = 0;
  virtual void quitAppli() = 0;
  virtual void startLoop(bool main_loop) = 0;  // starts the mainloop or a subloop.
  virtual void quitLoop(bool main_loop) = 0;   // quits the mainloop or a subloop.

  // attention: l'ordre de destruction importe ici: winlist doit etre
  // detruit en premier ca ca entraine des destructions qui font reference
  // a flowlist => winlist doit etre declare en DERNIER
  int id;
  UConf& conf;             // configuration data (may be shared with the UAppli)
  UStr display_name;       // name of the X Display
  int bpp;                   // bit per pixel
  int depth_size, stencil_size;
  int screen_width, screen_height;
  int screen_width_mm, screen_height_mm;
  bool is_opened;
  URenderContext *default_context;
  const UGlcontext *current_glcontext;
  std::vector<UHardFont**> font_map;
  unsigned long app_motion_time, nat_motion_time;  // for lag control
  unsigned long black_pixel, white_pixel, red_mask, green_mask, blue_mask;
  int red_shift, green_shift, blue_shift, red_bits, green_bits, blue_bits;  
  double IN_TO_PX, CM_TO_PX, MM_TO_PX, PT_TO_PX, PC_TO_PX;
  
  typedef std::list<UHardwinImpl*> HardwinList;
  HardwinList hardwin_list;  // list of hardwins that are event managed

  // copySelection() copies the content of the selection in this buffer
  // note: there is 1 selection per UEventFlow but only 1 copy_buffer per UDisp
  UStr copy_buffer;

  // pasteSelection() specifies that the selection will be pasted in this UStr
  // at this pos (uptr<> to avoid deletion)
  uptr<UStr> paste_str;
  int paste_pos;

  // attention: winlist doit etre detruit en premier!
  UElem winlist;  // list of windows belonging to this display
    
  struct LastResize {
    LastResize() : count(0), winview(null) {}
    int count;
    UView* winview;
    UDimension newsize;
  } last_resize;
  
#endif // NO_DOC
};

}
#endif
