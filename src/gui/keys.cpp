//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
/*
 *  keys.cpp : key management and correspondance with VREng
 *  NOTE: this file should be common to all X-Window GUI variants
 *
 *  VREng Project
 *  Author: Eric Lecolinet
 *
 *  (C) 1999-2008 Eric Lecolinet - ENST Paris
 *  WWW: http://www.enst.fr/~elc/ubit
 */
#include "vreng.hpp"
#include "widgets.hpp"
#include "useraction.hpp"
#include "move.hpp"	// changeKey

#if (UBIT_VERSION_MAJOR < 6 || UBIT_VERSION_MAJOR >= 6 && UBIT_WITH_X11) //UBIT6
#include <X11/keysym.h>	// XK_*
#endif

static long convertKey(long keycode, int keychar, int& vrkey);

/** Use this callback function for handling AUTOREPEAT Key event
 * Autorepeat is logically suppressed by annulating corresponding Press
 * and Release events.
 * Principle: Release events are postponed and temporarily stored in KRmask
 * They are processed when coming back to the RenderingWorkProc of the
 * mainLoop if not annulated in the meantime by subsequent Press events.
 */
void GuiWidgets::processKey(long keysym, int keychar, bool press)
{
  // converts the X keysym into a Vreng change key (vrkey) and returns
  // a keymask which is an hexa value for marking released keys in the KRmask
  int vrkey;
  long keymask = convertKey(keysym, keychar, vrkey);
  if (keymask == 0) return;    // return if null (undefined or not a vrkey)

  if (postponedKRcount < 0) {
    fprintf(stderr, "!negative KRcount => reset\n"); // should never happen!
    postponedKRmask = 0;
    postponedKRcount = 0;
  }

  if (press) {
    if (postponedKRmask & keymask) {
      // le Press annule le Release correspondant dans le KRmask
      postponedKRmask &= ~keymask;	// remove keymask from KRmask
      postponedKRcount--;		// une touche en moins dans KRlist
      // rien d'autre a faire puisque c'etait juste une anulation
    }
    else {  // traitement normal d'un Press
      //fprintf(stderr, "KPress change or activate Key( %d ) \n", vrkey);
      if (vrkey >= MAXKEYS || vrkey < 0) {
        return;
      }
      struct timeval time;
      gettimeofday(&time, NULL);
      changeKey(vrkey, press, time.tv_sec, time.tv_usec);
    }
  }
  else { // release
    // too many keys stored ==> flush the KRlist
    if (postponedKRcount > KRKey::KRBUF_MAX) {
      fprintf(stderr, "too many keys stored => flush: KRmask=%lx\n", postponedKRmask);
      flushPostponedKRs();
    }
    else { // normal case: postpone until we come back to the mainLoop
      // so that this Key Release event can possibly be annulate
      // if there is a subsequent (and corresponding) Key Press event
      postponedKRmask |= keymask;	// add keymask to KRmask

      // add this event to the KRlist of postponed Key Releases
      postponedKRbuf[postponedKRcount].vrkey = vrkey;
      gettimeofday(&(postponedKRbuf[postponedKRcount].time), NULL);
      postponedKRcount++;
    }
  }
}

void GuiWidgets::flushPostponedKRs()
{
  for (int ix = 0; ix < postponedKRcount; ix++) {
    KRKey &k = postponedKRbuf[ix];
    changeKey(k.vrkey, false, k.time.tv_sec, k.time.tv_usec);
  }
  postponedKRmask = 0;
  postponedKRcount = 0;
}

static long convertKey(long keycode, int keychar, int& vrkey)
{
  long keymask = 0;
  vrkey = 0;
  
  if      (keycode == UKey::Up)       { keymask = 1<<0;  vrkey = KEY_AV; }  // move forward
  else if (keycode == UKey::Down)     { keymask = 1<<1;  vrkey = KEY_AR; }  // move backward
  else if (keycode == UKey::Left)     { keymask = 1<<2;  vrkey = KEY_GA; }  // turn left
  else if (keycode == UKey::Right)    { keymask = 1<<3;  vrkey = KEY_DR; }  // turn right
  else if (keycode == UKey::PageUp)   { keymask = 1<<11; vrkey = KEY_JU; }  // move up
  else if (keycode == UKey::PageDown) { keymask = 1<<12; vrkey = KEY_JD; }  // move down
  else if (keycode == UKey::Insert)   { keymask = 1<<6;  vrkey = KEY_MT; }  // roll up
  else if (keycode == UKey::Delete)   { keymask = 1<<7;  vrkey = KEY_DE; }  // roll down
  else if (keycode == UKey::Home)     { keymask = 1<<8;  vrkey = KEY_HZ; }  // stand up
  else if (keycode == UKey::End)      { keymask = 1<<13; vrkey = KEY_VI; }  // accelerator
  else if (keycode == UKey::BackSpace) {
    GuiWidgets::callAction(UserAction::ASPEEDLESS); return 0; // decrease aspeed
  }
  else if (keycode == UKey::Tab) {
    GuiWidgets::callAction(UserAction::ASPEEDMORE); return 0; // increase aspeed
  }
  else {
    switch (keychar) {
      case '<': keymask = 1<<4;          vrkey = KEY_SG; break;  // left translation
      case '>': keymask = 1<<5;          vrkey = KEY_SD; break;  // right translation
      case 'l': keymask = 1<<9;          vrkey = KEY_TL; break;  // tilt left
      case 'r': keymask = 1<<10;         vrkey = KEY_TR; break;  // tilt right
      case 'u': keymask = 1<<11 | 1<<13; vrkey = KEY_JU; break;  // up translation
      case ' ': keymask = 1<<13;         vrkey = KEY_VI; break;  // accelerator
      case '=': GuiWidgets::callAction(UserAction::FOVYDEF);    return 0;  // original fovy
      case '-': GuiWidgets::callAction(UserAction::FOVYLESS);   return 0;  // decrease fovy
      case '+': GuiWidgets::callAction(UserAction::FOVYMORE);   return 0;  // increase fovy
      case '.': GuiWidgets::callAction(UserAction::LSPEEDDEF);  return 0;  // original lspeed
      case 's': GuiWidgets::callAction(UserAction::LSPEEDLESS); return 0;  // decrease lspeed
      case 'f': GuiWidgets::callAction(UserAction::LSPEEDMORE); return 0;  // increase lspeed
      case ',': GuiWidgets::callAction(UserAction::ASPEEDDEF);  return 0;  // original aspeed
      case 'b': GuiWidgets::callAction(UserAction::BULLET);     return 0;  // launche bullet
      case 'd': GuiWidgets::callAction(UserAction::DART);       return 0;  // launche dart
      case 'v': GuiWidgets::callAction(UserAction::SWITCHVIEW); return 0;  // switch view
      case 'x': GuiWidgets::callAction(UserAction::TPVIEWROTL); return 0;
      case 'c': GuiWidgets::callAction(UserAction::TPVIEWROTR); return 0;
      case 'q': GuiWidgets::callAction(UserAction::TPVIEWROTU); return 0;
      case 'w': GuiWidgets::callAction(UserAction::TPVIEWROTD); return 0;
      case 'p': GuiWidgets::callAction(UserAction::TPVIEWROTN); return 0;
      case 'm': GuiWidgets::callAction(UserAction::TPVIEWROTF); return 0;
      case 'D': GuiWidgets::callAction(UserAction::PITCHMORE);  return 0;  // increase pitch
      case 'U': GuiWidgets::callAction(UserAction::PITCHLESS);  return 0;  // decrease pitch
      case 'R': GuiWidgets::callAction(UserAction::ROLLMORE);   return 0;  // increase roll
      case 'L': GuiWidgets::callAction(UserAction::ROLLLESS);   return 0;  // decrease roll
      default: return 0; break; // == 0 => undefined key
    }
  }

  return keymask;
}
