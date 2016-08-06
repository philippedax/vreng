/************************************************************************
 *
 *  upiemenu.hpp : Pie and Control menus
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

#ifndef _upiemenu_hpp_
#define _upiemenu_hpp_ 1
#include <ubit/uboxgeom.hpp>
#include <ubit/umenu.hpp>
namespace ubit {
  
/** Pie menu.
  * Pie menus can have up to 8 items that are counted from 0 to 7 in anticlockwise
  * order, starting from the right direction of the x axis.
  * Pie menus can be nested by using the addSubMenu() method. 
  *
  * @see: show() methods and UPopmenu class to see how to open menus
  * @see also: UCtlmenu subclass for creating control menus.
  *
  * Event management:
  * each menu item can have callbacks that are fired on these conditions:
  * - UOn::action : this item has been activated (the mouse is released)
  * - UOn::arm : this item has been selected (the mouse enters this item)
  * - UOn::disarm : this item has been deselected (the mouse leaves this item)
  * - UOn::mdrag : the mouse is dragged in this item
  *
  * Item callbacks can have a UMouseEvent argument.
  * - getSource() returns the menu (NOT the item!)
  * - getView() returns the view of the menu
  * - getTarget() returns the item that is selected
  * - getX(), getY(), etc. are relative to the top/left origin of the menu
  *   (ie. to the menu view returned by getView())
  * - isFirstDrag() returns true at the beginning of a drag sequence
  *   (only once, just after UOn::arm).
  *
  * Example:
  * <pre>
  *   class Demo {
  *   public:
  *       void cut();
  *       void copy();
  *       void paste();
  *       ....
  *   };
  *
  *   Demo* d = new Demo();
  *   UPiemenu* pmenu = new UPiemenu(); 
  *
  *   pmenu->item(0).add("Cut" + UOn::action / ucall(d, &Demo::cut))
  *   pmenu->item(1).add("Copy" + UOn::action / ucall(d, &Demo::copy))
  *   pmenu->item(2).add("Paste" + UOn::action / ucall(d, &Demo::paste))
  * </pre>
  * @see also: UCall  
 */
class UPiemenu : public UPopmenu {
public:
  UCLASS(UPiemenu)

  UPiemenu();
  ///< creates a new pie menu; @see also UCtlmenu and shortcut function upiemenu().
  
  virtual ~UPiemenu();

  static UStyle* createStyle();

  virtual UBox& item(int n);
  /**< gets or creates the Nth item in the menu.
    * the last item is returned if N is too large
    * The item is created implicitely if it does not exist.
    * @see also: getItem(int n);
    */
  
  virtual UBox* getItem(int n) const;
  /**< returns the Nth item in the menu.
    * N must be a valid value (@see UPiemenu), null is returned otherwise. 
    * The item is NOT created if it does not exist.
    * @see also: item(int n);
    */
   
  virtual UBox* getSelectedItem() const;
  ///< returns the item that is currently selected (null if none).

  virtual int getSelectedIndex() const;
  ///< returns the index of item that is currently selected (-1 if none).

  virtual void addSubMenu(int N, UPiemenu* submenu);
  ///< adds a submenu to this menu.
  
  void addSubMenu(int N, UPiemenu& submenu) {addSubMenu(N, &submenu);}
  ///< adds a submenu to this menu.
  
  virtual void open(UMouseEvent&);
  /** opens the menu at the mouse event location (after waiting for getShowDelay()); MUST be used in UCall statements.
   * the delay can be changed by setShowDelay().
   */
    
  virtual void show(bool state, UDisp*);
  
  virtual void show(bool state = true) {UPiemenu::show(state,null);}
  ///< shows/hides the menu immediately (@see: UMenu::show(bool state)).
   
  unsigned long getShowDelay() const {return show_delay;}
  ///< returns the delay before the menu is opened (default = 300 ms)

  void setShowDelay(unsigned long d) {show_delay = d;}
  ///< changes the delay before the menu is opened (default = 300 ms)

  const UPoint& getMouseMovement() const {return mousepos;}
  ///< returns the location of the last DRAG event from the location of the ARM event.
  
  UWin* getContainingWin(UDisp* = null) const;  
  ///< returns the window where the menu is currently opened.
  
  UPoint getCenterInContainingWin(UDisp* = null) const;
  ///< returns the location of the menu center from the origin of getContainingWin().
  
  bool isSpringMode() const {return spring_mode;}
  void setSpringMode(bool state = true) {spring_mode = state;}

  // properties 
  
  UColor& pieColor()       {return *ppie_color;}
  UColor& pieBorderColor() {return *ppie_border_color;}
  UColor& sliceColor()     {return *pslice_color;}
  UColor& centerColor()    {return *pcenter_color;}
  UColor& centerBorderColor() {return *pcenter_border_color;}

  ULength& pieRadius()     {return pie_radius;}
  ULength& centerRadius()  {return center_radius;}
  ULength& itemRadius()    {return item_radius;}
    
  // - - callbacks - - 

  virtual void pressCB(UMouseEvent&);
  ///< [impl] called when the mouse is pressed.
 
  virtual void releaseCB(UMouseEvent&);
  ///< [impl] called when the mouse is released.
  
  virtual void motionCB(UMouseEvent&);
  ///< [impl] called when the mouse is dragged.

  virtual void hideCB(UEvent&);
  ///< [impl] called when the menu is hidden.

  virtual void paintCB(UPaintEvent&);
  ///< [impl] called when the menu is repainted.
    
  virtual void resizeCB(UResizeEvent&);
  ///< [impl] called when the menu is resized.
    
  virtual void armItemCB(UMouseEvent& e, UPiemenu* submenu);
  ///< [impl] called when an item is armed.

  virtual void disarmItemCB(UInputEvent& e, UPiemenu* submenu);
  ///< [impl] called when an item is disarmed.

  // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  
  enum MenuType {CONCENTRIC, COMPOUND, MULTISTROKE};
  
  void setMenuType(int);
  MenuType getMenuType(int) {return MenuType(menu_style);}
  
  UScale* getScale();
  float getScaleValue();
    
  virtual UBox* createItem();
  ///< called internally to create a new pie item (can be redefined).

  virtual void disarmItem(UInputEvent&, bool is_browsing);
  ///< [impl] disarms the item that is currently armed (if any).
 
  virtual float getCentredCoords(UMouseEvent&, UPoint& p, bool& must_forward);
  ///< [impl] tranforms events coords to centred coords.

  virtual short getItemFromPos(const UPoint&) const;

  virtual void reset(UMouseEvent*);
  ///< [impl] called when the menu is reset by show().

  virtual void startNoviceMode();
  ///< [impl] starts the novide mode: called by show().
  
  virtual void forwardToMenu(UPiemenu* submenu);
  ///< [impl] sets the submenu that is currently active.
 
   virtual void receiveFromMenu(UPiemenu* supermenu);
   ///< [impl] sets the supermenu that is currently active.
 
   virtual void unlinkFromAndToMenus();
   ///< [impl] detatches from the current sub and supermenu.
 
protected:
  unsigned char menu_style;
  unsigned char spring_mode:1, ctlmenu_mode:1, novice_mode:1, in_show_function:1; 
  int curitem;
  unsigned long show_delay;
  UPoint armpos, mousepos;
  USize pie_size;
  uptr<UTimer> ptimer;
  //uptr<UBox> parmed; plantage: parmed n'appartient pas au menu!
  UBox* parmed;
  UPiemenu *from_menu, *to_menu;
  UElem gitems;
  ULength pie_radius, center_radius, item_radius;
  uptr<UColor> ppie_color, pcenter_color, pslice_color, ppie_border_color, pcenter_border_color;
#endif
};

inline UPiemenu& upiemenu() {return *new UPiemenu();}
///< shortcut function that creates a new pie menu.

}
#endif

