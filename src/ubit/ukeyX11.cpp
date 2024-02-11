/************************************************************************
 *
 *  ukeyX11.cpp: key symbols for X11
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

#include <X11/keysym.h> 
//#include <X11/XF86keysym.h>        // @@@!!!
#include <ubit/udispX11.hpp>
#include <ubit/ukey.hpp>
namespace ubit {
  
  /* PROBLEMES / QUESTIONS :
  * XK_KP_Enter et NumLeft, NumUp, NumRight, KpDown, et Home, etc. et F1, F2 ...
  */
  
#define ALT_GRAPH_MASK  0x2000

static int _MetaDown, _AltDown, _ModeSwitch;

const int 
  UModifier::LeftButton  = Button1Mask, 
  UModifier::MidButton   = Button2Mask, 
  UModifier::RightButton = Button3Mask,
  UModifier::ShiftDown   = ShiftMask,
  UModifier::ControlDown = ControlMask,
  &UModifier::MetaDown   = _MetaDown,         // !! defini dynamiquement !!
  &UModifier::AltDown    = _AltDown,          // !! defini dynamiquement !!
  UModifier::AltGraphDown= ALT_GRAPH_MASK;

  /*
const int
  UModifier::MButton1 = UModifier::LeftButton, 
  UModifier::MButton2 = UModifier::MidButton, 
  UModifier::MButton3 = UModifier::RightButton ,
  UModifier::ShiftButton =   UModifier::ShiftDown,
  UModifier::ControlButton = UModifier::ControlDown;
*/
  
const int 
  UKey::BackSpace = XK_BackSpace,
  UKey::Tab = XK_Tab,
  UKey::Clear = XK_Clear,
  UKey::Enter = XK_Return,         // ATT: XK_Linefeed, XP_KP_Enter !!!!
  UKey::Pause = XK_Pause,
  UKey::ScrollLock = XK_Scroll_Lock,
  UKey::Escape = XK_Escape,
  UKey::Delete = XK_Delete,
  UKey::Print = XK_Print,
  UKey::Insert = XK_Insert,
  UKey::Undo = XK_Undo,
  UKey::Again = XK_Redo,
  UKey::Menu = XK_Menu,
  UKey::Find = XK_Find,
  UKey::Cancel = XK_Cancel,
  UKey::Help = XK_Help,
  // XK_Sys_Req, XK_Select, XK_Execute, XK_Begin, XK_Break
  
  UKey::NumLock = XK_Num_Lock,
  UKey::CapsLock = XK_Caps_Lock, // XK_Shift_Lock,
  UKey::Shift = XK_Shift_L,      // XK_Shift_R set to XK_Shift_L in unadisp::getKey()
  UKey::Control = XK_Control_L,  // idem
  UKey::Meta = XK_Meta_L,        // idem
  UKey::Alt = XK_Alt_L,          // idem
  //UKey::ModeSwitch = XK_Mode_switch,  // touche Alt avec Mac, ModeChange en Java ???
  /*
   XK_Super_L, XK_Super_R, XK_Hyper_L, XK_Hyper_R
   XK_script_switch
   Multi_key = XK_Multi_key,	 
   Codeinput = XK_Codeinput,	 
   SingleCandidate	= XK_SingleCandidate,	 
   MultipleCandidate = XK_MultipleCandidate,	 
   PreviousCandidate = XK_PreviousCandidate;
   */
   
  // cursor
  UKey::Home = XK_Home,
  UKey::End = XK_End,
  UKey::Left = XK_Left,
  UKey::Up = XK_Up,
  UKey::Right = XK_Right,
  UKey::Down = XK_Down,
  UKey::PageUp = XK_Page_Up,
  UKey::PageDown = XK_Page_Down,
  // XK_Prior, XK_Next
   
  // function keys
  UKey::F1 = XK_F1,
  UKey::F2 = XK_F2,
  UKey::F3 = XK_F3,
  UKey::F4 = XK_F4,
  UKey::F5 = XK_F5,
  UKey::F6 = XK_F6,
  UKey::F7 = XK_F7,
  UKey::F8 = XK_F8,
  UKey::F9 = XK_F9,
  UKey::F10 = XK_F10,
  UKey::F11 = XK_F11,
  UKey::F12 = XK_F12,
  UKey::F13 = XK_F13,
  UKey::F14 = XK_F14,
  UKey::F15 = XK_F15,
  UKey::F16 = XK_F16,
  UKey::F17 = XK_F17,
  UKey::F18 = XK_F18,
  UKey::F19 = XK_F19,
  UKey::F20 = XK_F20,
  UKey::F21 = XK_F21,
  UKey::F22 = XK_F22,
  UKey::F23 = XK_F23,
  UKey::F24 = XK_F24;

// ===========================================================================
// retrieve the modifiers and the corresponding masks
// ATT: pas correct si plusieurs diplays car ils peuvent avoir des mods differnts !
// le pbm c'est qu'il est impossible de savoir a priori la signification
// des Mod?Mask et ce que valent meta_mask, alt_mask, alt_graph_mask

 void UModifier::mapKeys(UDisp* d) {
   static bool mods_init = false; 
   if (mods_init) return;
   mods_init = true;

  Display* sysdisp = ((UDispX11*)d)->getSysDisp();
  _MetaDown = Mod1Mask;  // valeur qui peut varier suivant les plateformes
  _AltDown  = Mod2Mask;  // idem
  _ModeSwitch = 0;       // cf a la fin
  
  _MetaDown = _AltDown = _ModeSwitch = 0;
  XModifierKeymap* modmap = XGetModifierMapping(sysdisp);
  if (!modmap) return;
  
  // 8 modifiers dans tous les cas de figures (definis ou pas)
  for (int m = 0; m < 8; m++) {
    // plusieurs keycodes par modifiers, de 0 a max_keypermod
    for (int c = 0; c < modmap->max_keypermod; c++) {
      KeyCode keycode = modmap->modifiermap[m * modmap->max_keypermod + c];
      if (keycode != 0) {
        int keysyms_per_keycode_return;
        KeySym *keysym = XGetKeyboardMapping(sysdisp, keycode, 0, &keysyms_per_keycode_return);
        
        if ((unsigned long)keysym == XK_Meta_L || (unsigned long)keysym == XK_Meta_R)
          _MetaDown = (1 << m);
        else if ((unsigned long)keysym == XK_Alt_L || (unsigned long)keysym == XK_Alt_R)
          _AltDown = (1 << m);
        else if ((unsigned long)keysym == XK_Mode_switch)
          _ModeSwitch = (1 << m);
        XFree(keysym);
      }
    }
  }
  XFreeModifiermap(modmap);
}

void UKey::mapKeys(UDisp*) { }

}
#endif
