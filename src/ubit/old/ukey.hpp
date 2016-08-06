/************************************************************************
 *
 *  ukey.hpp : key symbols
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

#ifndef _ukey_hpp_
#define _ukey_hpp_ 1
#include <ubit/udefs.hpp>
namespace ubit {
  
  /** Modifier masks.
   * modifier masks are returned by UInputEvent::getModifiers(), UMouseEvent::getButton(),
   * UMouseEvent::getButtons().
   * @see also class UKey for key codes.
   */
  class UModifier {
  public:
    static const int
    LeftButton, RightButton, MidButton,
    ShiftDown, ControlDown, &MetaDown, &AltDown, AltGraphDown;
    
    static void mapKeys(UDisp*);
    ///< impl: init keys mapping.
  };
  
  /** Key codes.
   * key codes are returned by UKeyEvent::getKeyCode()
   * Note: do not confuse UKey::Shift (key code) with UMod::ShiftMod (modifier mask)
   * @see also class UMod for modifier mask.
   */      
  class UKey {
  public:    
    static const int
    BackSpace, Tab, Clear, Enter, Pause, ScrollLock, Escape, Delete,
    Undo, Again, Print, Insert, Menu, Find, Cancel, Help,
    NumLock, CapsLock, Shift, Control, Meta, Alt,
    Home, End, Left, Up, Right, Down, PageUp, PageDown,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, 
    F18, F19, F20, F21, F22, F23, F24;
    
    static void mapKeys(UDisp*);
    ///< impl: init keys mapping.
  };
  
}
#endif

