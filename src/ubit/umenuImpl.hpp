/* ==================================================== ======== ======= *
 *
 *  umenuImpl.hpp
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

#ifndef _umenuImpl_hpp_
#define	_umenuImpl_hpp_ 1
#include <vector>
namespace ubit {

/** [Implementation] manages the stack of menus that are currently opened on a given EventFlow.
*/
class UMenuManager {
public:
  UMenuManager(UEventFlow*);
  ~UMenuManager();

  UMenu* getDeepestMenu() const {return active_menu;}
  ///< returns the deepest menu in the menu stack (if any, null otherwise).

  bool contains(UMenu* menu) const;
  // returns true if this menu is in the menu stack.

  bool contains(UView*) const;
  // returns true if this view is contained in a menu in the menu stack.

  void closeAllMenus(bool clear_top_menu);
  ///< closes all menus in the menu stack.   
  
  void closeSubMenus(UMenu* from_menu, bool including_this_menu);
  ///< closes the submenus of this menu (calls closeAllMenus if menu is not found).

  void openMenu(UView* opener, UMenu*, UDisp*);
  void openMenuAfterDelay();
  void closeMenuAfterDelay();

  void menuChildCB(UInputEvent&, UBox* menu_or_menubar);
 
protected:
  friend class UMenu;
  friend class UMenubar;
  friend class UEventFlow;
  std::vector<UMenu*> menustack;  // stack of menus that are currently being grabbed
  int menu_count;
  UDisp* disp;
  UEventFlow& eflow; 
  UView *active_opener, *possible_opener, *possible_closer;
  UMenu *active_menu, *possible_opener_menu, *possible_closer_menu;
  uptr<UTimer> kk_open_timer, kk_close_timer;  // uptr: dont destroy Timers  $$$
  UBox* top_menu;
};

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

