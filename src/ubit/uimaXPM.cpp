/* ***********************************************************************
 *
 *  uimaXPM.cpp : glue with the XPM library.
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
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/ucall.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ufont.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uima.hpp>
#include <ubit/uhardima.hpp>

#if WITH_2D_GRAPHICS
# if UBIT_WITH_X11 && UBIT_WITH_XPM
#  include <X11/xpm.h>
#  include <ubit/udispX11.hpp>
# elif UBIT_WITH_GDK
#  include <ubit/udispGDK.hpp>
# endif
#endif

using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT


#if UBIT_WITH_GL

// use custom adapter if GL is available
// (!PROBLEM: won't work if the toolkit is used in X11 mode)

static const char* nextWord(const char* s, char* word, int wordsize) {
  int w = 0;
  --wordsize;  // final 0
  while (*s && *s != ' ' && *s != '\t' && *s != '\n' && *s != '\r') {
    if (w < wordsize) word[w++] = *s;
    ++s;
  }
  word[w] = 0;
  while (*s && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')) s++;
  return s;
}


int UImaXPM::read(UHardIma& natima, const UStr& path, int maxw, int maxh) {
  //UAppli::error("UImaXPM::read","Not available with this implementation");
  UStr fname = path.expand();
  if (fname.empty()) return UFilestat::CannotOpen;
  
  ifstream in(fname.c_str());
  if (!in) return UFilestat::CannotOpen;
  
  string line;
  getline(in, line);
  if (line != "/* XPM */") return UFileStat::InvalidData;
  
  int w = 0, h = 0, ncolors = 0, nchars = 0;
  int line_count = 0;
  int data_max = 100;
  int stat = 0;
  const char ** data = (const char **)malloc(data_max * sizeof(char*));
  
  while ((getline(in, line))) {
    // find opening "
    const char* p1 = line.c_str();
    while (*p1 && *p1!='"') p1++;
    if (*p1!='"') continue;
    
    // find ending "
    const char* p2= p1 + 1;
    while (*p2 && *p2!='"') p2++;
    if (*p2!='"') continue;
    
    int ll = max(w, (int)line.length());
    char* s = new char[ll+1];
    strncpy(s, p1+1, p2-p1-1);
    s[p2-p1-1] = 0;
    
    if (line_count==0) {
      int nargs = sscanf(s, "%d%d%d%d", &w, &h, &ncolors, &nchars);
      if (nargs < 4 || w <= 0 || h <= 0 || ncolors <= 0 || nchars <= 0 || nchars >2) 
        goto INVALID;
      w = w * nchars;
    }
    
    if (line_count >= data_max) {
      data_max *= 2;
      data = (const char **)realloc(data, data_max * sizeof(char*));
    }
    data[line_count] = s;
    ++line_count;
  }
  
  if (line_count != h+ncolors+1) goto INVALID;
  stat = readFromData(natima, data, maxw, maxh);
  
  for (int k = 0; k < line_count; ++k) delete data[k];
  free(data);
  return stat;
INVALID:  
  return UFileStat::InvalidData;
}


int UImaXPM::readFromData(UHardIma& natima, const char** xpmdata, int maxw, int maxh) {
  const char* line = xpmdata[0];
  int w = 0, h = 0, ncolors = 0, nchars = 0;
  int nargs = sscanf(line, "%d%d%d%d", &w, &h, &ncolors, &nchars);
  
  if (nargs < 4 || w <= 0 || h <= 0 || ncolors <= 0 || nchars <= 0 || nchars >2) 
    return UFileStat::InvalidData;
  
  natima.setRaster(w, h, 8);
  
  struct ColCell {
    unsigned char r, g, b;
  } coltab[65536];   // 2^16
  long icode, transpcolor = 65536+10;
  
  for (int l = 0; l < ncolors; ++l) {
    line = xpmdata[l+1];
    char code[5], type[5], value[50];
    
    line = nextWord(line, code, sizeof(code));
    
    if (nchars == 1) {
      if (code[0] == 0) icode = ' '; else icode = code[0];
    }
    else {
      if (code[0] == 0) icode = ' ' * 256 + ' ';
      else if (code[1] == 0) icode = code[0] * 256 + ' ';
      else icode = code[0] * 256 + code[1];
    }
    
    while (*line) {
      line = nextWord(line, type, sizeof(type));
      line = nextWord(line, value, sizeof(value));
      //NB: att aux greyXXXX pas dans la base des couleurs
      
      if ((type[0] == 'c' || type[0] == 'C') && type[1] == 0 && *value) {
        if (UCstr::compare(value, "none",true) == 0)
          transpcolor = icode;
        else {
          URgba c;
          UColor::parseColor(value, c);
          coltab[icode].r = c.getRedI(); 
          coltab[icode].g = c.getGreenI();
          coltab[icode].b = c.getBlueI();
        }
      }
    }
  }
  
  unsigned char* p = ((UHardImaGL*)&natima)->getPixels();
  const char** pixels = xpmdata + 1 + ncolors;
  
  if (nchars == 1) {
    for (int l = 0; l < h; ++l) {
      line = pixels[l];
      
      for (int c = 0; c < w; ++c, p+=4) {
        int val = line[c];
        if (val == 0) return UFileStat::InvalidData;
        
        if (val == transpcolor) p[0] = p[1] = p[2] = p[3] = 0;
        else {
          ColCell& c = coltab[val];
          p[0] = c.r; p[1] = c.g; p[2] = c.b; p[3] = 0xff;
        }
      }
    }
  }
  
  else if (nchars == 2) {
    int w2 = w * 2;  // two times more chars
    
    for (int l = 0; l < h; ++l) {
      line = pixels[l];
      
      for (int c = 0; c < w2; c+=2, p+=4) {
        int val = line[c] * 256 + line[c+1];
        if (val == 0) return UFileStat::InvalidData;
        
        if (val == transpcolor) p[0] = p[1] = p[2] = p[3] = 0;
        else {
          ColCell& c = coltab[val];
          p[0] = c.r; p[1] = c.g; p[2] = c.b; p[3] = 0xff;
        }
      }
    }
  }
  
  else return UFileStat::InvalidData;
  
  return UFileStat::Opened;
}

// -----------------------------------------------------------------------------
#elif (UBIT_WITH_X11 && UBIT_WITH_XPM)

static void initXPMattributes(UDisp* _d, XpmAttributes& a, XpmColorSymbol& sy) {
  UDispX11* d = (UDispX11*)_d;
  
  // cherche couleurs approchees avec une large tolerance
  a.exactColors = false;
  a.closeness = 100000;
  
  // specifier la bonne combinaison depth / visual / colormap c'est-a-dire
  // celle qui est definie par de UDisp
  a.depth = d->getBpp();
  a.colormap = d->getSysColormap();
  a.valuemask = XpmExactColors | XpmCloseness | XpmDepth | XpmColormap;
  
  // !att: visual can be null in which case it should not be given 
  // as an attribute to Xpm functions
  // (note that the visual also specifies the screen number)
  if ((a.visual = d->getSysVisual()->visual)) a.valuemask |= XpmVisual;
  
  /*
   if (!transp_bg) {   // no transparent -> take specified color as bg
   sy.name  = NULL;
   sy.value = (char*)"none";
   sy.pixel = bg;
   attributes.colorsymbols = &sy;
   attributes.numsymbols   = 1;
   attributes.valuemask   |= XpmColorSymbols;
   }*/
}

static int decodeXPMstatus(int stat) {
  if (stat >= 0) return UFilestat::Opened;
  else switch(stat) {
    case XpmFileInvalid:
      return UFilestat::InvalidData;
    case XpmOpenFailed:
      return UFilestat::CannotOpen;
    case XpmNoMemory:
      return UFilestat::NoMemory;
    default:
      return UFilestat::MiscError;
  }
}

int UImaXPM::read(UHardIma& natima, const UStr& path, int maxw, int maxh) {
  UDispX11* d = (UDispX11*)natima.getDisp();
  XpmAttributes attr;
  XpmColorSymbol symbol;  
  initXPMattributes(d, attr, symbol);
  
  XImage *ima = null, *shape = null;
  int stat = XpmReadFileToImage(d->getSysDisp(), const_cast<char*>(path.c_str()),
                                &ima, &shape, &attr);
  stat = decodeXPMstatus(stat);
  // !WARNING: ima, shape can NOT be used after this call (because they may be deleted):
  if (stat >= 0) natima.setRasterAndAdopt(ima, shape);
  return stat;
}

int UImaXPM::readFromData(UHardIma& natima, const char** xpmdata, 
                          int maxw, int maxh) {
  UDispX11* d = (UDispX11*)natima.getDisp();
  XpmAttributes attr;
  XpmColorSymbol symbol;  
  initXPMattributes(d, attr, symbol);
  
  XImage *ima = null, *shape = null;
  int stat = XpmCreateImageFromData(d->getSysDisp(), const_cast<char**>(xpmdata),
                                    &ima, &shape, &attr);
  stat = decodeXPMstatus(stat);
  if (stat >= 0) natima.setRasterAndAdopt(ima, shape);
  return stat;
}

// -----------------------------------------------------------------------------
#elif UBIT_WITH_GDK

int UImaXPM::read(UHardIma& natima, const UStr& path, int maxw, int maxh) {
  UDispGDK* d = (UDispGDK*)natima.getDisp();
  // !! VRAIMENT BIZARRE: on cree un pixmap puis une image qui sera convertie plus tard en pixmap!!
  GdkBitmap *pixshape = null;
  GdkPixmap* pix = gdk_pixmap_create_from_xpm(d->getSysFrame(), &pixshape, NULL, path.c_str());
  
  GdkImage *ima = null, *shape = null;
  if (pix) {
    gint w,h;
    gdk_drawable_get_size(pix, &w, &h);
    ima = gdk_drawable_get_image(pix, 0, 0, w, h);
  }
  if (pixshape) {
    gint w,h;
    gdk_drawable_get_size(pixshape, &w, &h);
    shape = gdk_drawable_get_image(pixshape, 0, 0, w, h);    
  }
  //if (ima) natima = new UHardIma(d, ima, shape);
  // !WARNING: ima, shape can NOT be used after this call (because they may be deleted):
  if (ima) natima.setRasterAndAdopt(ima, shape);
  
  if (pix) g_object_unref(pix);
  if (pixshape) g_object_unref(pixshape);
  return ima ? UFileStat::Opened : UFileStat::CannotOpen;
}


int UImaXPM::readFromData(UHardIma& natima, const char** xpmdata, int maxw, int maxh) {
  UDispGDK* d = (UDispGDK*)natima.getDisp();
  // !! VRAIMENT BIZARRE: on cree un pixmap puis une image qui sera convertie plus tard en pixmap!!
  GdkBitmap *pixshape = null;  
  GdkPixmap* pix = gdk_pixmap_create_from_xpm_d(d->getSysFrame(), &pixshape, 
                                                NULL, (char**)xpmdata);
  GdkImage *ima = null, *shape = null;
  if (pix) {
    gint w,h;
    gdk_drawable_get_size(pix, &w, &h);
    ima = gdk_drawable_get_image(pix, 0, 0, w, h);
  }
  if (pixshape) {
    gint w,h;
    gdk_drawable_get_size(pixshape, &w, &h);
    shape = gdk_drawable_get_image(pixshape, 0, 0, w, h);    
  }
  //if (ima) natima = new UHardIma(d, ima, shape);
  if (ima) natima.setRasterAndAdopt(ima, shape);
  
  if (pix) g_object_unref(pix);
  if (pixshape) g_object_unref(pixshape);
  return ima ? UFileStat::Opened : UFileStat::CannotOpen;
}

#endif
}

