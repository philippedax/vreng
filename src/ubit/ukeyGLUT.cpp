/************************************************************************
 *
 *  ukeyGLUT.cpp: key symbols for GLUT
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
#if UBIT_WITH_GLUT

#include <ubit/ugl.hpp>
#include <ubit/ukey.hpp>
namespace ubit {

const int 
  UModifier::LeftButton  = 1<<8, 
  UModifier::MidButton   = 1<<9, 
  UModifier::RightButton = 1<<10,
  UModifier::ShiftDown   = GLUT_ACTIVE_SHIFT, // 1<<0
  UModifier::ControlDown = GLUT_ACTIVE_CTRL,  // 1<<1
  &UModifier::MetaDown   = 1<<3,              // NOT defined by GLUT
  &UModifier::AltDown    = GLUT_ACTIVE_ALT,   // 1<<2
  UModifier::AltGraphDown= 0x2000;            // ????

  /*
const int
  UModifier::MButton1 = UModifier::LeftButton, 
  UModifier::MButton2 = UModifier::MidButton, 
  UModifier::MButton3 = UModifier::RightButton ,
  UModifier::ShiftButton =   UModifier::ShiftDown,
  UModifier::ControlButton = UModifier::ControlDown;
*/
  
const int 
  UKey::BackSpace = 127,
  UKey::Tab    = 9,
  UKey::Clear  = 0,
  UKey::Enter  = 13,  // comme F13 ?
  UKey::Pause  = 0,
  UKey::ScrollLock = 0,
  UKey::Escape = 27,
  UKey::Delete = 8,
  UKey::Print  = 0,
  UKey::Insert = GLUT_KEY_INSERT,
  UKey::Undo   = 0,
  UKey::Again  = 0,
  UKey::Menu   = 0,
  UKey::Find   = 0,
  UKey::Cancel = 0,
  UKey::Help   = 0,
  
  UKey::NumLock  = 0,
  UKey::CapsLock = 0,
  UKey::Shift    = 0,
  UKey::Control  = 0,
  UKey::Meta     = 0,
  UKey::Alt      = 0,

  UKey::Home = GLUT_KEY_HOME,
  UKey::End  = GLUT_KEY_END,
  UKey::Left = GLUT_KEY_LEFT,
  UKey::Up   = GLUT_KEY_UP,
  UKey::Right= GLUT_KEY_RIGHT,
  UKey::Down = GLUT_KEY_DOWN,
  UKey::PageUp   = GLUT_KEY_PAGE_UP,
  UKey::PageDown = GLUT_KEY_PAGE_DOWN,
   
  // function keys
  UKey::F1 = GLUT_KEY_F1,
  UKey::F2 = GLUT_KEY_F2,
  UKey::F3 = GLUT_KEY_F3,
  UKey::F4 = GLUT_KEY_F4,
  UKey::F5 = GLUT_KEY_F5,
  UKey::F6 = GLUT_KEY_F6,
  UKey::F7 = GLUT_KEY_F7,
  UKey::F8 = GLUT_KEY_F8,
  UKey::F9 = GLUT_KEY_F9,
  UKey::F10 = GLUT_KEY_F10,
  UKey::F11 = GLUT_KEY_F11,
  UKey::F12 = GLUT_KEY_F12,
  UKey::F13 = 13,  // comme Enter ?
  UKey::F14 = 14,
  UKey::F15 = 15,
  UKey::F16 = 16,
  UKey::F17 = 17,
  UKey::F18 = 18,
  UKey::F19 = 19,
  UKey::F20 = 20,
  UKey::F21 = 21,
  UKey::F22 = 22,
  UKey::F23 = 23,
  UKey::F24 = 24;

void UModifier::mapKeys(UDisp* nd) { }
void UKey::mapKeys(UDisp* nd) { }

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

