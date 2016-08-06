/************************************************************************
 *
 *  ueventflow.hpp: Event Flow
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

#ifndef _ueventflow_hpp_
#define	_ueventflow_hpp_  1
#include <ubit/uappli.hpp>
#include <ubit/uevent.hpp>
#include <ubit/uviewImpl.hpp>
namespace ubit {
  
  class UMenuManager;
  
  /** Event Flow.
   * a Ubit application can manage 1 or several Event Flows that are 
   * comptelety separated. This is useful for Two-handed interaction or 
   * groupware (each user controlling his own pointer on the screen(s)). 
   * See class UAppli and UAppli::getFlow().
   *
   * Note: the ID of the native Event Flow is 0 on all Displays.
   */
  class UEventFlow {
  public:
    UEventFlow(UDisp&, int channel);
    /**< creates a new Event Flow on this Display.
     * the ID of the native Event Flow is 0 on all Displays.
     */
    
    virtual ~UEventFlow();
    
    int getID() const {return id;}
    /**< returns the global ID of this Event Flow.
     * this is a unique ID for the whole application.
     * returns 0 for the native Event Flow on the Default Display.
     * @see also: getChannel()
     */
    
    int getChannel() const {return channel;}
    /**< returns the channel of this Event Flow relatively to its Display.
     * this value is NOT unique for the whole application but depends on the display
     * that this event flow id connected to.
     * Remarks:
     * - returns 0 for the native Event Flow
     * - several Event Flows can have the same channel but on different displays 
     *   (for instance, each Display have an Event Flow on channel 0)
     * @see also: getID()
     */
    
    UDisp& getDisp() const {return disp;}
    ///< returns the display that controls this event flow.
    
    void setFocus(UView*);
    /**< gives the input focus to this view.
     * no object gets the focus if argument is null. The focus is reset when the user 
     * clicks on a the view of widget that can handle input from the keyboard 
     */
    
    USelection* getSelection() {return &selection;}
    ///< returns the text selection of this event flow.
    
    UMenuManager& getMenuManager() const {return menu_man;}
    
    UObject* getUserData() {return user_data;}
    ///< gets a handle to user data.  
    
    void setUserData(UObject* obj) {user_data = obj;}
    ///< sets a handle to user data.
    
    void closeAllMenus();
    ///< close all menus related to this eventflow.  
    
    void deleteNotify(UView* deleted_view);
    void deleteNotify(UElem* deleted_group);
    
    void redirectMousePress(UMouseEvent&, UView* winview);
    
    // - - - Impl.  - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    void mousePress(UView* win_view, unsigned long time, int state,
                    const UPoint& win_pos, const UPoint& abs_pos, int btn);
    void mouseRelease(UView* win_view, unsigned long time, int state,
                      const UPoint& win_pos, const UPoint& abs_pos, int btn);
    void mouseMotion(UView* win_view, unsigned long time, int state,
                     const UPoint& win_pos, const UPoint& abs_pos);
    void wheelMotion(UView* win_view, unsigned long time, int state,
                     const UPoint& win_pos, const UPoint& abs_pos, int type, int delta);
    void keyPress(UView* win_view, unsigned long time, int state, int keycode, short keychar);
    void keyRelease(UView* win_view, unsigned long time, int state, int keycode, short keychar);
    void winEnter(UView* win_view, unsigned long time);
    void winLeave(UView* win_view, unsigned long time);
    
    void setCursor(UEvent&, const UCursor*);
    void startAutoRepeat(UEvent&);
    void stopAutoRepeat(UEvent&);
    
  protected:
    void boxCross(UView* box_view, unsigned long time, int state, const UCursor*, bool is_browsing);
    bool mustCloseMenus(UView* source_view);
    void autoRepeatCB(UTimerEvent&);
    void openTipCB(UTimerEvent&);
    void openTipRequest(UEvent&);
    void closeTipRequest(UEvent&);
    UWin* retrieveTelePointer(UDisp*);
    void showTelePointers(UMouseEvent&, int mode = 0);  
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  private:
    friend class UAppli;
    friend class UEvent;
    friend class UElem;
    static int TIP_X_SHIFT;
    int id, channel;
    UDisp& disp;
    
    struct LastPressed {
      LastPressed() {reset();}
      void reset();
      void set(UView* source_view, const UPoint& win_pos, const UPoint& screen_pos, const UViewFind&);
      UView* view;
      UBox* box;
      UPoint win_in_screen;        // pos of win in the screen (for drag and release events)
      U3DcanvasView* canvas_view;  // != null if the source is in a 3Dwidget
      U3Dpos* refpos_in_canvas;    // 3Dpos of the 3Dwidget in canvas_view (if it is != null)
      UBehavior behavior;
    } lastPressed;
    
    UView *lastEntered, *currentFocus;
    UElem *lastArmed, *beingClicked, *dragSource, *dropTarget;  
    UPoint click_pos;
    int    click_count;
    unsigned long click_time;
    const class UCursor* lastCursor;    // cursor being currently shown
    uptr<UTimer> auto_repeat_timer;     // timer fo auto_repeat actions
    uptr<UTimer> tip_timer;             // timer for tool tips
    UWin& tip_win;                      // window for tool tips
    UMenuManager& menu_man;             // menubars & pulldown menus manager
    USelection& selection;              // text selection management
    std::vector<class UWin*> tele_pointers;  // remote pointers
    UObject* user_data;
  };
  
}
#endif
