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
#if 0  // tout ceci ne sert a rien
  // alphanumeric
  UKey::_0 = XK_0,    // ATT: XK_KP_0 etc. !!!!
  UKey::_1 = XK_1,
  UKey::_2 = XK_2,
  UKey::_3 = XK_3,
  UKey::_4 = XK_4,
  UKey::_5 = XK_5,
  UKey::_6 = XK_6,
  UKey::_7 = XK_7,
  UKey::_8 = XK_8,
  UKey::_9 = XK_9,
  UKey::A = XK_A,
  UKey::B = XK_B,
  UKey::C = XK_C,
  UKey::D = XK_D,
  UKey::E = XK_E,
  UKey::F = XK_F,
  UKey::G = XK_G,
  UKey::H = XK_H,
  UKey::I = XK_I,
  UKey::J = XK_J,
  UKey::K = XK_K,
  UKey::L = XK_L,
  UKey::M = XK_M,
  UKey::N = XK_N,
  UKey::O = XK_O,
  UKey::P = XK_P,
  UKey::Q = XK_Q,
  UKey::R = XK_R,
  UKey::S = XK_S,
  UKey::T = XK_T,
  UKey::U = XK_U,
  UKey::V = XK_V,
  UKey::W = XK_W,
  UKey::X = XK_X,
  UKey::Y = XK_Y,
  UKey::Z = XK_Z,

  // symbols
  UKey::ParenLeft = XK_parenleft,
  UKey::ParenRight = XK_parenright,
  UKey::BracketLeft = XK_bracketleft,
  UKey::BracketRight = XK_bracketright,
  UKey::BraceLeft = XK_braceleft,
  UKey::BraceRight = XK_braceright,
  
  UKey::Plus = XK_plus,
  UKey::Minus = XK_minus,
  UKey::Asterisk = XK_asterisk,
  UKey::Slash = XK_slash,
  // Add, Substract
  // UKey::Multiply = XK_multiply,
  // UKey::Divide = XK_division,
  UKey::Less = XK_less,
  UKey::Equals = XK_equal,
  UKey::Greater = XK_greater,
  
  UKey::Space = XK_space,
  UKey::At = XK_at,
  UKey::NumberSign = XK_numbersign,
  UKey::Exclam = XK_exclam,
  UKey::ExclamDown	= XK_exclamdown,
  UKey::Quote = XK_apostrophe,
  UKey::QuoteDbl = XK_quotedbl,
  UKey::Currency = XK_currency,
  UKey::Dollar = XK_dollar,
  UKey::Percent = XK_percent,
  UKey::Ampersand = XK_ampersand,
  UKey::Comma = XK_comma,
  UKey::Period = XK_period,
  UKey::Colon = XK_colon,
  UKey::Semicolon = XK_semicolon,
  UKey::Backslash = XK_backslash,
  UKey::Underscore = XK_underscore,
  UKey::Question = XK_question,
  //UKey::Questiondown = XK_questiondown,
  UKey::Bar = XK_bar,
  
  /* REMOVED
  UKey::Circum = XK_asciicircum,
  UKey::Tilde = XK_asciitilde,
  //=XK_quoteleft 0x060	// deprecated
  */
  /* ADDED */
  UKey::Section = XK_section,  
  UKey::Degree = XK_degree,    
  UKey::Cent = XK_cent,
  UKey::Sterling = XK_sterling,
  UKey::Yen = XK_yen,  
  UKey::DeadGrave = XK_dead_grave,
  UKey::DeadAcute = XK_dead_acute,
  UKey::DeadCircumflex = XK_dead_circumflex,
  UKey::DeadTilde = XK_dead_tilde,
  UKey::DeadMacron = XK_dead_macron,
  UKey::DeadBreve = XK_dead_breve,
  UKey::DeadAbovedot = XK_dead_abovedot,
  UKey::DeadDiaeresis = XK_dead_diaeresis,
  UKey::DeadAbovering = XK_dead_abovering,
  UKey::DeadDoubleacute = XK_dead_doubleacute,
  UKey::DeadCaron = XK_dead_caron,
  UKey::DeadCedilla = XK_dead_cedilla,
  UKey::DeadOgonek = XK_dead_ogonek,
  UKey::DeadIota = XK_dead_iota,
  /* END ADDED */
  
  // accents
  UKey::Grave = XK_grave,     // ATT: XK_egrave !!!
  UKey::Diaeresis = XK_diaeresis,
  UKey::Macron = XK_macron,	 
  UKey::Acute = XK_acute,
  UKey::Cedilla = XK_cedilla,
  UKey::Agrave = XK_Agrave,
  UKey::Aacute = XK_Aacute,	 
  UKey::Acircumflex = XK_Acircumflex,	 
  UKey::Atilde = XK_Atilde,
  UKey::Adiaeresis = XK_Adiaeresis,
  UKey::Ccedilla = XK_Ccedilla,
  UKey::Egrave = XK_Egrave,
  UKey::Eacute = XK_Eacute,
  UKey::Ecircumflex = XK_Ecircumflex,
  UKey::Ediaeresis = XK_Ediaeresis,
  UKey::Igrave = XK_Igrave,
  UKey::Iacute = XK_Iacute,
  UKey::Icircumflex = XK_Icircumflex,
  UKey::Idiaeresis = XK_Idiaeresis,
  UKey::Ntilde = XK_Ntilde,
  UKey::Ograve = XK_Ograve,
  UKey::Oacute = XK_Oacute,
  UKey::Ocircumflex = XK_Ocircumflex,
  UKey::Otilde = XK_Otilde,
  UKey::Odiaeresis = XK_Odiaeresis,
  UKey::Ooblique = XK_Ooblique,
  UKey::Ugrave = XK_Ugrave,
  UKey::Uacute = XK_Uacute,
  UKey::Ucircumflex = XK_Ucircumflex,
  UKey::Udiaeresis = XK_Udiaeresis,
  UKey::Yacute = XK_Yacute,
  UKey::Ydiaeresis = XK_ydiaeresis;
     
  //UKey::Degree = XK_degree,	 
  //UKey::Cent = XK_cent,
  //UKey::Sterling = XK_sterling,
  //UKey::Yen = XK_yen,  
  //UKey::Nobreakspace = XK_nobreakspace,
  //UKey::Brokenbar = XK_brokenbar,	 
  //UKey::Section = XK_section,	 
  //UKey::Copyright = XK_copyright,	 
  //UKey::Ordfeminine = XK_ordfeminine,	 
  //UKey::Guillemotleft = XK_guillemotleft,	 
  //UKey::Notsign = XK_notsign,	 
  //UKey::Hyphen = XK_hyphen,	 
  //UKey::Registered = XK_registered,	 
  //UKey::Plusminus = XK_plusminus,	 
  //UKey::Twosuperior = XK_twosuperior,	 
  //UKey::Threesuperior = XK_threesuperior,	 
  //UKey::Mu = XK_mu,
  //UKey::Paragraph = XK_paragraph,
  //UKey::Periodcentered = XK_periodcentered,
  //UKey::Onesuperior = XK_onesuperior,	 
  //UKey::Masculine = XK_masculine,
  //UKey::Guillemotright = XK_guillemotright,	 
  //UKey::Onequarter = XK_onequarter,
  //UKey::Onehalf = XK_onehalf,
  //UKey::Threequarters = XK_threequarters,	 
  //UKey::Aring = XK_Aring,
  //UKey::AE = XK_AE,
  //UKey::ETH = XK_ETH,
  //UKey::THORN = XK_THORN,	 
  //UKey::Ssharp = XK_ssharp,	 
#endif

  // ===========================================================================
  /* media

static const int   // may not be defined
  Standby,
  VolumeDown,
  VolumeMute,
  VolumeUp,
  MediaPlay,
  MediaStop,
  MediaPrevious,
  MediaNext,
  HomePage,
  Search,
  MediaRecord,
  Back,
  Forward,
  Stop,
  Refresh,
  PowerOff,
  WakeUp,
  Eject,
  Sleep,
  OpenUrl,
  Favorites,
  Zoom,
  LaunchMail,
  LaunchMedia,
  Launch0,
  Launch1,
  Launch2,
  Launch3,
  Launch4,
  Launch5,
  Launch6,
  Launch7,
  Launch8,
  Launch9;
};

#ifdef XF86XK_Standby
const int
  UKey::Standby = XF86XK_Standby,	 
  UKey::VolumeDown = XF86XK_AudioLowerVolume,	 
  UKey::VolumeMute = XF86XK_AudioMute,
  UKey::VolumeUp = XF86XK_AudioRaiseVolume, 
  UKey::MediaPlay = XF86XK_AudioPlay,
  UKey::MediaStop = XF86XK_AudioStop,	 
  UKey::MediaPrevious = XF86XK_AudioPrev,	 
  UKey::MediaNext = XF86XK_AudioNext,
  UKey::HomePage = XF86XK_HomePage,
  UKey::Search = XF86XK_Search,
  UKey::MediaRecord = XF86XK_AudioRecord,	 

  UKey::Back = XF86XK_Back,	 
  UKey::Forward = XF86XK_Forward,	 
  UKey::Stop = XF86XK_Stop,
  UKey::Refresh = XF86XK_Refresh,	 
  UKey::PowerOff = XF86XK_PowerOff,
  UKey::WakeUp = XF86XK_WakeUp,
  UKey::Eject = XF86XK_Eject,
  UKey::Sleep = XF86XK_Sleep,
  UKey::OpenUrl = XF86XK_OpenURL,
  UKey::Favorites = XF86XK_Favorites,
  UKey::Zoom = XF86XK_ZoomIn,

  UKey::LaunchMail = XF86XK_Mail,
  UKey::LaunchMedia = XF86XK_Start,
  UKey::Launch0	= XF86XK_Launch0,
  UKey::Launch1	= XF86XK_Launch1,	 
  UKey::Launch2	= XF86XK_Launch2,
  UKey::Launch3	= XF86XK_Launch3,
  UKey::Launch4	= XF86XK_Launch4,
  UKey::Launch5	= XF86XK_Launch5,
  UKey::Launch6	= XF86XK_Launch6,
  UKey::Launch7	= XF86XK_Launch7,
  UKey::Launch8	= XF86XK_Launch8,
  UKey::Launch9	= XF86XK_Launch9;
#endif

*/
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
        KeySym keysym = XKeycodeToKeysym(sysdisp, keycode, 0);
        
        if (keysym == XK_Meta_L || keysym == XK_Meta_R)
          _MetaDown = (1 << m);
        else if (keysym == XK_Alt_L || keysym == XK_Alt_R)
          _AltDown = (1 << m);
        else if (keysym == XK_Mode_switch)
          _ModeSwitch = (1 << m);
      }
    }
  }
  XFreeModifiermap(modmap);
}

void UKey::mapKeys(UDisp*) { }

}
#endif
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

