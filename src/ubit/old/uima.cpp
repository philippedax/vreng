/* ***********************************************************************
 *
 *  uima.cpp: images
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <list>
#include <iostream>
#include <cstring>
#include <ubit/ufile.hpp>
#include <ubit/ucall.hpp>
#include <ubit/uupdatecontext.hpp>
#include <ubit/ugraph.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uview.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uima.hpp>
#include <ubit/upix.hpp>
#include <ubit/uevent.hpp>
#include <ubit/uappli.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uconf.hpp>
#include <ubit/uhardima.hpp>
using namespace std;
namespace ubit {

/* ==================================================== ===== ======= */

UIma::UIma(const char* file_name, bool load_now) {
  name = null;
  show_unknown_ima = false;
  setImpl(file_name);
  if (load_now) loadNow();
}

UIma::UIma(const UStr& _filename, bool load_now) {
  name = null;
  show_unknown_ima = false;
  setImpl(_filename.c_str());
  if (load_now) loadNow();
}

UIma::UIma(const char** xpm_data, bool load_now) {
  name = null;
  show_unknown_ima = false;
  setImpl(xpm_data);
  if (load_now) loadNow();
}

UIma::UIma(const char** xpm_data, UConst m) : UData(m) {
  name = null;
  show_unknown_ima = false;
  setImpl(xpm_data);
}

UIma::UIma(int w, int h) {
  name = null;
  show_unknown_ima = false;
  setImpl(w, h);
}

UIma::~UIma() {
  if (name) free(name);
  cleanCache();
}

void UIma::cleanCache() {
  // NB: on ne detruit pas data (data n'est pas copie et 
  // pointe donc sur l'original)
  for (list<UHardIma*>::iterator p = natimas.begin(); p != natimas.end(); p++) {
    delete (*p);
  }
  natimas.clear();
}

/* ==================================================== [Elc] ======= */

void UIma::update() {
  _parents.updateAutoParents(UUpdate::LAYOUT_PAINT);
}

//const char* UIma::getNameChars() const {return name;}

bool UIma::isRealized() const {
  return (natimas.empty() ? false : (*natimas.begin())->isRealized());
}

int UIma::getHeight() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getHeight());
}

int UIma::getWidth() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getWidth());
}

int UIma::getBpp() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getBpp());
}

int UIma::getTransparency() const {
  return (natimas.empty() ? 0 : (*natimas.begin())->getTransparency());
}

/*
unsigned long UIma::getPixel(int x, int y) {
  return natimas.empty() ? 0 : (*natimas.begin())->getPixel(x, y);
}

void UIma::setPixel(unsigned long p, int x, int y) {
  if (!natimas.empty()) (*natimas.begin())->setPixel(p, x, y);
}
*/
/* ==================================================== [Elc] ======= */

UIma& UIma::operator=(const UIma& ima2) {
  if (checkConst()) return *this;
  if (&ima2 == this) return *this;
  
  setImpl((char*)null);
  name = ima2.name ? UCstr::dup(ima2.name) : null;
  stat = ima2.stat;

  if (ima2.natimas.empty()) {
    changed(true);
    return *this;
  }
  
  natimas.push_back((*ima2.natimas.begin())->createScaledClone(1.,1.));
  changed(true);
  return *this;
}

/* ==================================================== ===== ======= */

void UIma::resize(int max_width, int max_height, bool preserve_ratio, bool dont_magnify) {
  if (checkConst()) return;// false;
  if (natimas.empty()) {
    if (stat==0) loadNow();
  }
  if (natimas.empty()) return;// false;
  if (getWidth()<=0 || getHeight()<=0) return;// false;

  float xscale = (max_width<=0)  ? 1. : (float)max_width/getWidth();
  float yscale = (max_height<=0) ? 1. : (float)max_height/getHeight();
  if (preserve_ratio) {
    float xyscale = min(xscale, yscale);   // preserve aspect ratio
    xscale = yscale = xyscale;
  }

  // case preserve_smaller : resize if ima is too large
  if (!dont_magnify || (xscale < 1. && yscale < 1.)) {
    UHardIma* ni = (*natimas.begin())->createScaledClone(xscale,yscale);
    cleanCache();
    natimas.push_back(ni);
    changed(true);
  }
  //return true;
}

/* ==================================================== ===== ======= */

bool UIma::rescale(float xscale, float yscale) {
  if (checkConst()) return false;
  if (natimas.empty()) {
    if (stat==0) loadNow();
  }
  if (natimas.empty()) return false;

  UHardIma* ni = (*natimas.begin())->createScaledClone(xscale,yscale);
  cleanCache();
  natimas.push_back(ni);
  changed(true);
  return true;
}

/* ==================================================== ===== ======= */

int UIma::read(const UStr& fname, int max_w, int max_h) {
  setImpl(fname.c_str());
  realize(max_w, max_h);
  // ne fait rien si UHardwin::read() tient compte du max pour ce fichier
  if (max_w > 0 || max_h > 0) resize(max_w, max_h, true, true);
  changed(true);  // *apres* le realize
  return stat;
}

int UIma::read(const char* fname, int max_w, int max_h) {
  setImpl(fname);
  realize(max_w, max_h);
  // ne fait rien si natima::read() tient compte du max pour ce fichier
  if (max_w > 0 || max_h > 0) resize(max_w, max_h, true, true);
  changed(true); // *apres* le realize
  return stat;
}

int UIma::loadNow() {
  if (!name && !data) return stat = UFilestat::CannotOpen;
  realize(0,0);
  changed(true);  // *apres* le realize
  return stat;
}

int UIma::loadFromData(const char** xpm_data) {
  setImpl(xpm_data);
  realize(0,0);
  changed(true); // *apres* le realize
  return stat;
}

/* ==================================================== ======== ======= */

void UIma::set(const UStr& fname) {
  if (checkConst()) return;
  setImpl(fname.c_str());
  changed(true);
}

void UIma::set(const char* fname) {
  if (checkConst()) return;
  setImpl(fname);
  changed(true);
}

void UIma::set(const char** xpm_data) {
  if (checkConst()) return;
  setImpl(xpm_data);
  changed(true);
}

/* ==================================================== ======== ======= */

void UIma::setImpl(const char* fname) {
  cleanCache();
  if (name) free(name);
  name = fname ? UCstr::dup(fname) : null;
  data = null;
  stat = UFilestat::NotOpened;
  mode = READ_FROM_FILE;
}

void UIma::setImpl(const char** xpm_data) {
  cleanCache();
  if (name) free(name);
  name = null;
  data = xpm_data;  //!!att: NOT copied!
  stat = UFilestat::NotOpened;
  mode = READ_FROM_DATA;
}

// ATT: pas pret pour OpenGL!! (a cause de createImage)
void UIma::setImpl(int w, int h) {
  cleanCache();
  if (name) free(name);
  name = null;
  data = null;
  stat = UFilestat::NotOpened;
  mode = CREATE;
  UHardIma* ni = null;
  
  if (w > 0 && h > 0) {
#if UBIT_WITH_GL
    if (UAppli::isUsingGL()) {
      ni = new UHardImaGL(UAppli::getDisp(), w, h);
      goto DONE;
    }
#endif
#if WITH_2D_GRAPHICS
    ni = new UHardIma2D(UAppli::getDisp(), w, h);
    goto DONE;   // just to avoid a compiler warning
#endif
  }
  
  DONE: if (ni) {
    stat = UFilestat::Opened;
    natimas.push_back(ni);
  }
}

/* ==================================================== [Elc] ======= */

void UIma::getSize(UUpdateContext& ctx, UDimension& dim) const {
  UDisp* d = ctx.getDisp();

  if (stat == UFilestat::NotOpened && natimas.empty()) {
    realize(0 ,0, d, false);
  }

  // !!ATTENTION: l'image peut etre partage par plusieurs objets,
  // d'ou la necessite d'initialiser TOUS les links correctement
  // meme si l'image en elle-meme n'est charge qu'une seule fois

  if (natimas.empty()) {
    if (show_unknown_ima) {            // empecher recursion infinie
      if (this != d->getConf().unknow_image) {
        d->getConf().unknow_image->getSize(ctx, dim);
      }
      else dim.width = dim.height = 0;
    }
    else dim.width = dim.height = 0;
  }

  else {
    // it's useless to rescale the image at this stage : we'll
    // do it when the image is actually painted (hence hidden images,
    // for instance clipped images, wont be rescaled)
    UHardIma* ni = (*natimas.begin());
    //dim.width  = ni->getWidth()  * ctx.xyscale + 0.5;   // ex int()
    //dim.height = ni->getHeight() * ctx.xyscale + 0.5;
    dim.width  = ni->getWidth()  * ctx.xyscale;   // ex int()
    dim.height = ni->getHeight() * ctx.xyscale;

  }
}

/* ==================================================== [Elc] ======= */

void UIma::paint(UGraph& g, UUpdateContext& props, const URect& r) const {
  if (natimas.empty()) {
    if (show_unknown_ima) {                //empecher recursion infinie
      if (this != g.getDisp()->getConf().unknow_image) 
        g.getDisp()->getConf().unknow_image->paint(g, props, r);
    }
    //else nop;
    return;
  }
  
  g.drawIma(*this, r.x, r.y, props.xyscale);

/*
#if UBIT_WITH_GL
  if (UAppli::isUsingGL()) {
    // une seule natima dans ce cas (la premiere)
    // ATT: on ne gere pas les multidisp             !!!!!!!
    if (!natimas.empty()) {
      UHardIma* ni = *(natimas.begin());
      if (ni) g.drawNatIma(*(UHardImaGL*)ni, r.x, r.y, props.xyscale);
    }
    return;
  }
#endif
#if WITH_2D_GRAPHICS
  UHardIma* ni = getOrCreateIma(g.getDisp(), props.xyscale);
  if (ni) g.drawNatIma(*ni, r.x, r.y);
#endif
*/
}

/* ==================================================== [Elc] ======= */
#if WITH_2D_GRAPHICS

UHardIma* UIma::getOrCreateIma(UDisp* d, float xyscale) const {
  UHardIma* ni = findImaInCache(d, xyscale);
  if (!ni) ni = addImaInCache(d, xyscale);
  return ni;
}

UHardIma* UIma::findImaInCache(UDisp* d, float xyscale) const {
  // search in the cache if this image was already created with the same size
  // AND THE SAME DISPLAY !!
  for (list<UHardIma*>::iterator p = natimas.begin(); p != natimas.end(); ++p) {
    if ((*p)->getScale() == xyscale && (*p)->disp == d)
      return *p;
  }
  return null; // not found
}

// we destroy all copies except the 1st one which is the original image
// and the last one as it is likely to be used again soon

UHardIma* UIma::addImaInCache(UDisp* d, float xyscale) const {
  if (natimas.empty()) return null;
  
  list<UHardIma*>::iterator p = natimas.begin();
  p++;                                // skip 1st = the original
  while (p != natimas.end()) {
    if ((*p)->disp != d) p++;
    else { 
      list<UHardIma*>::iterator p2 = p; p2++;
      if (p2 == natimas.end()) p++;    // dont destroy the last one !!
      else {
        delete *p;
        natimas.erase(p);
        p = p2;
      }
    }
  }

  // NB image depth must be <= to the NatDisp depth
  UHardIma* ni = (*natimas.begin())->createScaledClone(xyscale, xyscale, d);
  if (ni) {
    if (UAppli::isUsingGL()) {
      UAppli::internalError("UIma::addImaInCache","invalid when using OpenGL");
      return null;
    }
    ((UHardIma2D*)ni)->scale = xyscale;   // !! att au cast !
    natimas.push_back(ni);
  }

  return ni; 
}

#endif
/* ==================================================== [Elc] ======= */

void UIma::getFullPath(UStr& path, const char* name) {
  if (!name || name[0] == '\0')
    path = "";
  else if (name[0] == '.' || name[0] == '/')
    path = name;
  else       // no prefix ==> add imap_path
    path = UAppli::getImaPath() & name;
}

int UIma::realize(int max_w, int max_h, UDisp* disp, bool force_reload) const {
  if (!disp) disp = UAppli::getDisp();
  // ne pas essayer de recharger une seconde fois quelque soit
  // le resultat de cet essai (pour ne pas boucler sur le load)
  if (force_reload || (stat == UFilestat::NotOpened && natimas.empty())) {
    UHardIma* ni = null;
    
#if UBIT_WITH_GL
    if (UAppli::isUsingGL()) {
      ni = new UHardImaGL(disp, 0, 0);
      goto DONE;
    }
#endif
#if WITH_2D_GRAPHICS
    ni = new UHardIma2D(disp, 0, 0);
    goto DONE;   // just to avoid a compiler warning
#endif

  DONE:
    if (mode == READ_FROM_DATA) {
      if (!data) stat = UFilestat::InvalidData;
      else {
        stat = ni->readFromData(data, max_w, max_h);
        if (stat <= 0) delete ni; else natimas.push_back(ni);
      }
    }
    
    else if (mode == READ_FROM_FILE) {
      UStr fpath;
      getFullPath(fpath, name);
      stat = ni->read(fpath, null, max_w, max_h);
      if (stat <= 0) delete ni; else natimas.push_back(ni);
    }
    
    /*
    else if (mode == CREATE) {
      ... dans setImpl();
    }
    */
    
    else delete ni;  // ni cree inutilement
  }

  return stat;
}

}
