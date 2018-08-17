/************************************************************************
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

#include <iostream>
#include <algorithm>
#include <ubit/ubit_features.h>
#include <ubit/ucall.hpp>
#include <ubit/uwin.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/uscrollbar.hpp>
#include <ubit/uselection.hpp>
#include <ubit/uconf.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ucursor.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ufontImpl.hpp>
#include <ubit/uon.hpp>
#include <ubit/utimer.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/udispX11.hpp>
#include <ubit/udispGLUT.hpp>
//#include <ubit/udispGDK.hpp>
#include <ubit/uhardfont.hpp>
#include <ubit/urendercontext.hpp>
#include <ubit/uglcontext.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

// ==================================================== [Ubit Toolkit] =========

UDisp* UDisp::create(const UStr& dname) {
  UDisp* d = null;
#if UBIT_WITH_GLUT
  d = new UDispGLUT(dname);
#elif UBIT_WITH_X11
  d = new UDispX11(dname);
//#elif UBIT_WITH_GDK
//  d = new UDispGDK(dname);
#else
  UAppli::fatalError("UDisp::create","no available underlying windowing toolkit")
#endif
  // ne peut pas etre dans UDisp constr, car ceux des subclasses doivent etre executes
  if (d && d->isOpened()) UAppli::impl.displist.push_back(d);
  return d;
}

// ==================================================== [Ubit Toolkit] =========

UDisp::UDisp(const UStr& dname) :
id(UAppli::impl.displist.size()), 
conf(UAppli::conf), 
display_name(dname),
bpp(UAppli::conf.bpp),
depth_size(UAppli::conf.depth_size),
stencil_size(UAppli::conf.stencil_size),
screen_width(0), screen_height(0),
screen_width_mm(0), screen_height_mm(0),
is_opened(false), 
default_context(null), current_glcontext(null), 
app_motion_time(0), nat_motion_time(0),
red_mask(0), green_mask(0), blue_mask(0),
red_shift(0), green_shift(0), blue_shift(0), 
red_bits(0), green_bits(0), blue_bits(0),
IN_TO_PX(0.), CM_TO_PX(0.), MM_TO_PX(0.), PT_TO_PX(0.), PC_TO_PX(0.),
paste_str(null),
paste_pos(0) {
  // if :screen not found add ":0" (except if display_name is empty)
  if (!display_name.empty() && display_name.rfind(':') < 0) display_name &= ":0";
  
  if (UAppli::isUsingGL()) {
#ifndef UBIT_WITH_GL
    UAppli::error("UDisp","OpenGL mode requested but the Ubit GUI toolkit was not compiled with the OpenGL library");
#endif
  }
}

UDisp::~UDisp() {
  UAppli::deleteNotify(this);
} 

/*
 bool UDisp::isGLAvailable() const {
 #if UBIT_WITH_GL
 return true;
 #else
 return false;
 #endif
 }
 */
// ==================================================== [Ubit Toolkit] =========

URenderContext* UDisp::getDefaultContext() {
  if (default_context) return default_context;
  
  // if default_context is not initialized, search (by order of preference):
  // the first mainframe, then the first frame, then the first window
  UHardwinImpl* main_hw = null;
  
  for (HardwinList::iterator k = hardwin_list.begin(); k != hardwin_list.end(); ++k) {
    UWinImpl::WinType wtype = (*k)->getWinType();
    if (wtype == UWinImpl::MAINFRAME) {
      main_hw = *k;
      break;
    }
    else if (wtype == UWinImpl::FRAME) {
      if (!main_hw) main_hw = *k;
    }
    else if (wtype > UWinImpl::HARDWIN) {
      if (!main_hw || main_hw->getWinType() != UWinImpl::FRAME) main_hw = *k;
    }
  }
  default_context = main_hw->getGlcontext();
  default_context->setDest(main_hw, 0, 0);
  return default_context;
}

void UDisp::makeDefaultContextCurrentIfNeeded() {
  if (!UAppli::isUsingGL()) return;
  
  // note that getDefaulContext() inits 'default_context' if not already done
  URenderContext* dc = getDefaultContext();
  if (!current_glcontext || !current_glcontext->isSharedWith(dc)) dc->makeCurrent(); 
}

// ==================================================== [Ubit Toolkit] =========

void UDisp::add(UWin& win) {  
  winlist.add(win);   // add to the window list

  // !!ATT: marche pas si meme win ajoutee plusieurs fois !!
  //UChild* c = null;
  //if (win.pbegin() != win.pend()) c = &win.pbegin().parent().getChild();
  win.initViewImpl(/*c,*/ null, this);

  // ne pas afficher de fenetre avant lancement de la mainloop
  if (UAppli::isRunning() && win.isShowable()) win.show(true);
}

void UDisp::remove(UWin& win, bool auto_delete) {
  win.show(false);
  winlist.remove(win, auto_delete);
}

void UDisp::add(UWin* win) {  
  if (!win) UAppli::error("UDisp::add","null UWin* argument");
  else add(*win);
}

void UDisp::remove(UWin* win, bool auto_delete) {  
  if (!win) UAppli::error("UDisp::remove","null UWin* argument");
  else remove(*win, auto_delete);
}

// ==================================================== [Ubit Toolkit] =========

void UDisp::addHardwin(UWin* win) {
  if (!win) return;
  UHardwinImpl* hw = win->hardImpl();
  //UHardwinImpl* hw = win->getHardwin(this);  PAS BON !!!
  //cerr << "addHardwin " <<win->hardImpl() << " "<< hw << endl;
  if (!hw) return;
  if (find(hardwin_list.begin(), hardwin_list.end(), hw) == hardwin_list.end())
    hardwin_list.push_back(hw);      // ne pas ajouter 2 fois
}

void UDisp::removeHardwin(UWin* win) {
  if (!win) return;
  UHardwinImpl* hw = win->hardImpl();
  // UHardwinImpl* hw = win->getHardwin(this); PAS BON !!!
  //cerr << "removeHardwin "  <<win->hardImpl() << " "<< hw << endl;
  if (!hw) return;
  HardwinList::iterator i = find(hardwin_list.begin(), hardwin_list.end(), hw);
  if (i != hardwin_list.end()) hardwin_list.erase(i);
}

// ==================================================== [Ubit Toolkit] =========

void UDisp::setPixelPerMM(double pixel_per_mm) {
  MM_TO_PX = pixel_per_mm;
  CM_TO_PX = pixel_per_mm * 10;
  IN_TO_PX = pixel_per_mm * 25.4;  // generally close to 72 
  PT_TO_PX = pixel_per_mm * 25.4 / 72.;       //  pt = 1/72 in
  PC_TO_PX = 12 * pixel_per_mm * 25.4 / 72.;  // 1 pc = 12 pt  
}

void UDisp::setPixelPerInch(double pixel_per_inch) {
  MM_TO_PX = pixel_per_inch / 25.4;
  CM_TO_PX = pixel_per_inch / 25.4 * 10;
  IN_TO_PX = pixel_per_inch;
  PT_TO_PX = pixel_per_inch / 72.;       //  pt = 1/72 in
  PC_TO_PX = 12 * pixel_per_inch / 72.;  // 1 pc = 12 pt  
}

UStr* UDisp::getPasteTarget(int& pos) const {
  pos = paste_pos; 
  return paste_str;
}

void UDisp::clearPasteTarget() {
  paste_str = null;
  paste_pos = 0;
}

void UDisp::copySelection(UMouseEvent& e, USelection& textsel) {
  textsel.copyText(copy_buffer);
  setSelectionOwner(e);
}

void UDisp::pasteSelection(UMouseEvent& e, UStr* _paste_str, int _paste_pos) {
  // il faut effacer toutes les selections avant de faire un paste (sinon tout
  // va se melanger n'importe comment)  
  const UFlowList& flist = UAppli::getFlowList();
  for (UFlowList::const_iterator it = flist.begin(); it != flist.end(); ++it) {
    if (&(*it)->getDisp() == this && (*it)->getSelection()) 
      (*it)->getSelection()->clear();
  }
  
  // NB: pour interdire un improbable delete de paste_str avant
  // pasteSelectionCallback() paste_str est un uptr<>
  paste_str = _paste_str;
  paste_pos = _paste_pos;
  pasteSelectionRequest(e);
}

/* ==================================================== ======== ======= */

USelection* UDisp::getChannelSelection(int _channel) {
  UEventFlow* fl = getChannelFlow(_channel);
  if (!fl) return null;
  else return fl->getSelection();
}

UEventFlow* UDisp::getChannelFlow(int _channel) const {
  UFlowList& flist = UAppli::impl.flowlist; 
  for (unsigned int k = 0; k < flist.size(); ++k) {
    if (&flist[k]->getDisp() == this && flist[k]->getChannel() == _channel)
      return flist[k];
  }
  return null;
}

UEventFlow* UDisp::obtainChannelFlow(int _channel) {
  UEventFlow* fl = getChannelFlow(_channel);
  if (!fl) fl = new UEventFlow(*this, _channel);
  return fl;
}

// ==================================================== [Ubit Toolkit] =========

void UDisp::countBits(unsigned long mask, int& bits, int& shift) {
  bool in_tab = true; shift = 0; bits = 0;
  while (mask != 0) {
    if (in_tab) {
      if ((mask & 0x1) == 0) shift++; else {in_tab = false; bits++;}
    }
    else {
      if ((mask & 0x1) == 0) break; else bits++;
    }
    mask = mask >> 1;
  }
}

// ==================================================== [Ubit Toolkit] =========
// Fonts

UHardFont* UDisp::getFont(const UFontDesc* f) { // !NOTE: may change the glcontext!
  if (!f) return null;
  makeDefaultContextCurrentIfNeeded();
   
  const UFontFamily& ff = *(f->family);
  if (!ff.ready || ff.ffindex >= (signed)font_map.size() || !font_map[ff.ffindex] 
      || f->findex < 0 || !font_map[ff.ffindex][f->findex])
    return realizeFont(*f);
  else return font_map[ff.ffindex][f->findex];
}

void UDisp::realizeFontFamily(const UFontFamily& fam) {
  if (fam.ffindex < 0) {      // new FontFamily
    fam.ffindex = fam.family_count++;
  }
  
  // increase the fontmap vector
  if (fam.ffindex >= (signed)font_map.size()) {
    for (int k = (signed)font_map.size(); k <= fam.ffindex; ++k) {
      font_map.push_back(null);
    }
  }
  
  // this font family has changed => remove entries in fontmap
  if (!fam.ready && font_map[fam.ffindex]) {    
    unsigned int fontmap_size = 4 * (fam.fsizes.size() + 1);
    UHardFont** fam_map = font_map[fam.ffindex];
    
    for (unsigned int k = 0; k < fontmap_size; ++k) {
      // delete if not shared
      if (fam_map[k] && fam_map[k]->count <= 1) delete fam_map[k];
    }
    delete [] fam_map;
    font_map[fam.ffindex] = null;
  }
  
  // note that the size vector may have been changed
  fam.initSizes();
  
  // allocates the corresponding font_map element
  if (!font_map[fam.ffindex]) {
    unsigned int fontmap_size = 4 * (fam.fsizes.size() + 1);
    UHardFont** fam_map = new UHardFont*[fontmap_size];
    
    for (unsigned int k = 0; k < fontmap_size; ++k) fam_map[k] = null;
    font_map[fam.ffindex] = fam_map;
  }
  
  fam.ready = true;
}


UHardFont* UDisp::realizeFont(const UFont& font) {
  UFontDesc f(font);
  return realizeFont(f);
}

UHardFont* UDisp::realizeFont(const UFontDesc& _f) {
  const UFontFamily* fam = _f.family;
  // on ne peut pas modifier _f directement car _f.family doit
  // rester null dans les cas des modifiers UFont::small etc
  UFontDesc f = _f;
  
  // sets and initializes the font family if needed
  if (!fam) f.family = fam = UAppli::conf.default_font->family;
  
  if (!fam->ready || fam->ffindex >= (signed)font_map.size() || !font_map[fam->ffindex])
    realizeFontFamily(*fam);
  
  if (f.findex < 0) f.setScale(1.);      // sinon index serait a -1 !
  
  UHardFont* nf = font_map[fam->ffindex][f.findex];
  if (nf && nf->isReady()) return nf;  // font already loaded
  
  // ALL fonts are created in the default GC of the display !!!
  // => fonts won't be usuable with other OpenGL GCs except those created in sharelists mode
  //if (current_rc != getDefaultRC()) getDefaultRC()->makeCurrent();
  makeDefaultContextCurrentIfNeeded();

  nf = new UHardFont(this, f);
  
  if (!nf->isReady()) delete nf;
  else return font_map[fam->ffindex][f.findex] = nf;  // DONE!
  
  // otherwise...
  UStr s = "Can't load font '"; s &= fam->name;
  if (f.styles & UFont::BOLD) s &= " bold";
  if (f.styles & UFont::ITALIC) s &= " italic";
  s &= "' (using alternate font)";
  UAppli::warning("UDisp::realizeFont", s.c_str());
  
  // try the same font without stylistic options
  if (f.styles != 0)  {
    UFontDesc fd2 = f;
    fd2.styles = 0;
    fd2.setScale(1.);    // sinon fd2.index serait incorrect
    UHardFont* nf2 = getFont(&fd2);
    if (nf2 && nf2->isReady()) {
      nf2->count++;   // shared font
      return font_map[fam->ffindex][f.findex] = nf2;
    }
  }
  
  // try the default font 
  if (f.family->name != UFontFamily::defaults.name)  {
    UFontDesc fd2 = f;
    fd2.family = &UFontFamily::defaults;
    //fd2.styles = 0;
    fd2.setScale(1.);    // sinon fd2.index serait incorrect
    UHardFont* nf2 = getFont(&fd2);
    if (nf2 && nf2->isReady()) {
      nf2->count++;  // shared font
      return font_map[fam->ffindex][f.findex] = nf2;
    }
  }
  
  // try another size
  const int LAST_RESORT_FONT_SIZE = 12;
  
  if (f.actual_size != LAST_RESORT_FONT_SIZE)  {
    UFontDesc fd2 = f;
    fd2.styles = 0;
    fd2.def_size = LAST_RESORT_FONT_SIZE;
    fd2.setScale(1.);    // sinon fd2.index serait incorrect
    UHardFont* nf2 = getFont(&fd2);
    if (nf2 && nf2->isReady()) {
      nf2->count++;   // shared font
      return font_map[fam->ffindex][f.findex] = nf2;
    }
  }
  
  // plus aucun espoir...
  return null;
}

// ==================================================== [Ubit Toolkit] =========

UEventFlow* UDisp::obtainFlow(unsigned int estate, int channel) {
  if (estate & UMS_EVENT_MASK) {                         // ???? !!! PBM si pas X11 ???
    // create a new flow or returns an existing flow if ID already used.
    return obtainChannelFlow(channel);
  }
  return UAppli::impl.disp->obtainChannelFlow(0);  // default
}

void UDisp::onResize(UView* winview, const UDimension& size) {
  // ce test est pas bon car winview peut avoir ete change sans que onResize()
  // ait ete mis a jour (c'est le cas avec GLUT par exple)
  //if (winview->getWidth() != size.width || winview->getHeight() != size.height) {
    
    // different winview: purge requests on previous winview
    if (last_resize.winview != null && last_resize.winview != winview) {
      last_resize.count = 1;  // do it now
      onResizeCB(last_resize.winview);
      last_resize.winview = null;
      last_resize.count = 0;
    }
    
    last_resize.count++;
    last_resize.winview = winview;
    last_resize.newsize = size;
    UAppli::postpone(ucall(this, winview, &UDisp::onResizeCB));
  //}
}

void UDisp::onResizeCB(UView* winview) {
  if (last_resize.winview == winview && last_resize.count == 1) {
    last_resize.winview = null;
    UDimension& size = last_resize.newsize;
    
    UWin* win = winview->getWin();
    UHardwinImpl* hw = win->getHardwin(this);
    //cerr << "onResizeCB " <<win->hardImpl() << " "<< hw << endl;
    hw->doUpdateImpl(UUpdate::LAYOUT, win, winview, &size);
    win->repaint();
  }
  last_resize.count--;
}

void UDisp::onPaint(UView* winview, float x, float y, float w, float h) {
  if (last_resize.count == 0) {  
    URect clip(x, y, w, h);
    winview->updatePaint(&clip);
  }
}

// Notifies the UMS when a window is opened/closed.
//void UDisp::showNotify(USysWindow w, bool shows) {
// if (!disp.umsclient) return;
// if (shows) disp.umsclient->winOpened(w);
// else disp.umsclient->winClosed(w);
//}

}
