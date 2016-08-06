/* ***********************************************************************
 *
 *  uhardima.cpp : native images and pixmaps
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * NOTE: this file contains source code that was adapted from gifrsize.c from
 * the Gif-Lib written by Gershon Elber (Ver 0.1, Jul. 1989)
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
#include <cstring>
#include <cmath>
#include <ubit/ufile.hpp>
#include <ubit/ucall.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ufont.hpp>
#include <ubit/uima.hpp>
#include <ubit/ustr.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uhardima.hpp>

#if UBIT_WITH_X11 
#  include <ubit/udispX11.hpp>
#  define DestroyImage(I) XDestroyImage(I)
#  define DestroyPixmap(D,Pix) XFreePixmap(D,Pix)
#  define CreatePixmap(D,Win,w,h,bpp) XCreatePixmap(D,Win,w,h,bpp)
#  define PutPixel(I,X,Y,P) XPutPixel(I,X,Y,P)
#  define GetPixel(I,X,Y) XGetPixel(I,X,Y)
#  define AllocColor(D,Map,Col) XAllocColor(D,Map,Col)
#  define SetForeground(G,P) (G).foreground = (P)
#  define SetBackground(G,P) (G).background = (P)
#  define DrawImage(D,Wn,G,I,Xs,Ys,Xd,Yd,W,H) XPutImage(D,Wn,G,I,int(Xs),int(Ys),int(Xd),int(Yd),int(W),int(H))
#  define CreateGC(D,W,M,V)  XCreateGC(D,W,M,V)
#  define DestroyGC(D,G)     XFreeGC(D,G)
#  define GCFOREGROUND       GCForeground
#  define GCBACKGROUND       GCBackground
typedef Display* USysDisp;
typedef Window   USysWin;
typedef XVisualInfo* USysVisual;
typedef GC USysGC;
typedef Colormap USysColormap;
typedef XGCValues GCValues;

//#elif UBIT_WITH_GDK
//#  include <ubit/udispGDK.hpp>
// etc...

#endif

using namespace std; 
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

UHardIma::~UHardIma() {}
  
int UHardIma::read(const UStr& fname, const char* _ftype, int wmax, int hmax) {
  if (fname.empty()) return UFilestat::CannotOpen;
  
  UStr ftype = _ftype;
  if (ftype.empty()) ftype = fname.suffix();
  if (ftype.empty()) return UFilestat::UnknownType;
  
#if UBIT_WITH_GIF
  else if (ftype.equals("gif",true))
    return UImaGIF::read(*this, fname, wmax, hmax);
#endif
  
#if UBIT_WITH_JPEG
  else if (ftype.equals("jpg",true) || ftype.equals("jpeg",true))     
    return UImaJPEG::read(*this, fname, wmax, hmax);
#endif
  
#if UBIT_WITH_XPM
  else if (ftype.equals("xpm",true))
    return UImaXPM::read(*this, fname, wmax, hmax);
#endif
  
  else return UFilestat::UnknownType;
}

int UHardIma::readFromData(const char** xpm_data, int wmax, int hmax) {
  if (!xpm_data) return UFilestat::CannotOpen;
  return UImaXPM::readFromData(*this, xpm_data, wmax, hmax);
}

/* ==================================================== [Elc] ======= */
#ifdef UBIT_WITH_GL

UHardImaGL::UHardImaGL(UDisp* d, int w, int h, int transparency_hint) {
  disp = d;
  pixels = null;
  texid = 0;
  setRaster(w, h, transparency_hint);
}

/*
UHardImaGL::UHardImaGL(UDisp* nd, unsigned char* _glpix, int w, int h) {
  natdisp = nd;
  width = w;
  height = h;
  bpp = 32; 
  transparency = 8;
  pixels = _glpix;
  texid = 0;
}
*/

UHardImaGL::~UHardImaGL() {      // glcontext dependent!!!
  delete[] pixels;
  if (texid != 0) glDeleteTextures(1, &texid);
}

bool UHardImaGL::isRealized() const {
  //EX: return (glpix != null);
  return (texid != 0 || pixels != null);  // ????
}

/* ==================================================== ===== ======= */

void UHardImaGL::setRaster(int w, int h, int transparency_hint) {   // glcontext dependent!!!
  delete[] pixels;
  if (texid != 0) glDeleteTextures(1, &texid);

  if (w > 0 && h > 0) {
    width = w; height = h;
    bpp = 32; transparency = 8;
    // for some reason gluBuild2DMipmaps() crashes in some case if w*h is used
    // but seems to be OK if another line is added
    // pixels = new unsigned char[w * h * bpp/8];
    pixels = new unsigned char[w * (h+1) * bpp/8];
    texid = 0;                                // ... ??? !!!!@@@@@ createText?
  }
  else {
    width = height = 0;
    bpp = 0; transparency = 0;
    pixels = null;
    texid = 0;
  }
}

void UHardImaGL::setRasterAndAdopt(unsigned char* pixbuf, int w, int h) {   // glcontext dependent!!!
  delete[] pixels;
  if (texid != 0) glDeleteTextures(1, &texid);

  if (pixbuf && w > 0 && h > 0) {
    width = w; height = h;
    bpp = 32; transparency = 8;
    pixels = pixbuf;
    texid = 0;                                // ... ??? !!!!@@@@@ createText?
  }
  else {
    width = height = 0;
    bpp = 0; transparency = 0;
    pixels = null;
    texid = 0;
  }  
}

/* ==================================================== ===== ======= */
#if WITH_2D_GRAPHICS

void UHardImaGL::setRasterAndAdopt(USysIma ima, USysIma imashape) {   // glcontext dependent!!!
#if UBIT_WITH_X11
  UDispX11* d = (UDispX11*)disp;
#else
  UDispGDK* d = (UDispGDK*)disp;
#endif
  
  delete[] pixels;
  if (texid != 0) glDeleteTextures(1, &texid);

  if (!ima) {
    width = height = 0; 
    bpp = 0; transparency = 0;
    pixels = null;
    texid = 0;
  }
  
  else {
    width = ima->width; height = ima->height; 
    bpp = 32; transparency = 8;   //(imashape ? 1 : 0);
    pixels = new unsigned char[width * height * bpp/8];
    
    register unsigned char* p = pixels;
    register unsigned long
      val,
      red_mask   = d->getRedMask(),
      green_mask = d->getGreenMask(),
      blue_mask  = d->getBlueMask(),
      red_shift  = d->getRedShift(),
      green_shift= d->getGreenShift(),
      blue_shift = d->getBlueShift();
    
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        val = SysGetPixel(ima, x, y);
        *p++ = ((val & red_mask)   >> red_shift);
        *p++ = ((val & green_mask) >> green_shift);
        *p++ = ((val & blue_mask)  >> blue_shift);
        if (imashape) {*p++ = SysGetPixel(imashape, x, y) ? 0xff : 0;}
        else *p++ = 0xff;
      }
    }
  
    texid = 0;                                // ... ??? !!!!@@@@@ createText?
  }
  
  if (ima) DestroyImage(ima);
  if (imashape) DestroyImage(imashape);
}

#endif
/* ==================================================== ===== ======= */

static UHardImaGL* _createScaledClone(UDisp* to_nd, UDisp* from_nd,
                                     UHardImaGL* from_ima, float xscale, float yscale);
                                     
UHardIma* UHardImaGL::createScaledClone(float xscale, float yscale, UDisp* to_nd) {
  if (!to_nd) to_nd = disp;
  if (!disp) {
    UAppli::error("UHardIma::createScaledClone","no available display");
    return null;
  }
  return _createScaledClone(to_nd, disp, this, xscale, yscale);
}

/* ==================================================== ===== ======= */

void UHardImaGL::createTexFromPixels() const {   // glcontext dependent!!!
  if (!pixels) return;
  if (texid != 0) glDeleteTextures(1, &texid);

  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);
  
  //GL_NEAREST  GL_LINEAR  GL_LINEAR_MIPMAP_LINEAR
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //GLenum format = (alpha==0 ? GL_RGB : GL_RGBA);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

/* sans MipMaps
void UHardImaGL::createTexFromPixels() const {
  if (!pixels) return;
  if (texid != 0) glDeleteTextures(1, &gltex);

  float log_w = ::log2(width);
  float log_h = ::log2(height);
  // arrondir au plus proche
  //if (log_w - int(log_w) > 0.5) log_w += 1;
  //if (log_h - int(log_h) > 0.5) log_h += 1;
  int to_w = int(exp2(int(log_w)));
  int to_h = int(exp2(int(log_h)));
  
  // la taille min des textures est de 64x64
  // A FAIRE: verifier la taille max!
  if (to_w < 64) to_w = 64;
  if (to_h < 64) to_h = 64;
  
  glGenTextures(1, &gltex);
  glBindTexture(GL_TEXTURE_2D, gltex);
  //GL_NEAREST  GL_LINEAR  GL_LINEAR_MIPMAP_LINEAR
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  //GLenum format = (alpha==0 ? GL_RGB : GL_RGBA);
  GLenum format = GL_RGBA;  // GL_RGB deconne for some reason
  unsigned char* to_pix = null;
  
  if (to_w == width && to_h == height) {
    to_pix = glpix;
  }
  else {
    //to_pix = new unsigned char[to_w * to_h * (alpha==0 ? 3 : 4)];
    to_pix = new unsigned char[to_w * to_h * 4];
    int stat = gluScaleImage(format, width, height, GL_UNSIGNED_BYTE, glpix,
                             to_w, to_h, GL_UNSIGNED_BYTE, to_pix);
    cerr << "InitSize " << width << " " << height
      << " TO "  << to_w << " " << to_h << " log " << log_w << " " << log_h
      << " pix " << int(glpix) << " " <<int(to_pix) << " stat " << stat << endl;
    delete[] glpix;
    glpix = to_pix;
  }
  
  glTexImage2D(GL_TEXTURE_2D,
               0, //level
               format,  //internalFormat: pourrait etre tjrs RGBA?
               to_w, to_h, 0,  //border
               format,  //pixel format
               GL_UNSIGNED_BYTE,
               to_pix);
*/

#endif  // UBIT_WITH_GL

/* ==================================================== [Elc] ======= */
#if WITH_2D_GRAPHICS

UHardIma2D::UHardIma2D(UDisp* d, int w, int h, int transparency_hint) {
  disp = d;
  sys_ima = sys_imashape = null;
  setRaster(w, h, transparency_hint);
}

UHardIma2D::~UHardIma2D() {
  if (sys_ima) DestroyImage(sys_ima);
  if (sys_imashape) DestroyImage(sys_imashape);
  sys_ima = null;
  sys_imashape = null;
}

void UHardIma2D::setRaster(int w, int h, int transparency_hint) {
  if (sys_ima) DestroyImage(sys_ima);
  if (sys_imashape) DestroyImage(sys_imashape);
  scale = 1.0;

  if (w > 0 && h > 0) {
    width = w; height = h;
    bpp = disp->getBpp(); 
    sys_ima = createEmptySysIma(disp, w, h, bpp);
    if (transparency_hint) {
      transparency = 1;
      sys_imashape = createEmptySysIma(disp, w, h, 1);
    }
    else {
      transparency = 0;
      sys_imashape = null;
    }
  }
  else {
    width = height = 0;
    bpp = 0; transparency = 0;
    sys_ima = sys_imashape = null;
  }
}

void UHardIma2D::setRasterAndAdopt(USysIma ima, USysIma imashape) {
  if (sys_ima) DestroyImage(sys_ima);
  if (sys_imashape) DestroyImage(sys_imashape);
  scale = 1.0;
  
  if (ima) {
    width = ima->width; height = ima->height; 
    bpp = ima->depth; transparency = (imashape ? 1 : 0);
    sys_ima = ima; sys_imashape = imashape;
  }
  else {
    width = height = 0; 
    bpp = 0; transparency = 0;
    sys_ima = sys_imashape = null;
  }
}

UHardIma* UHardIma2D::createScaledClone(float xscale, float yscale, UDisp* to_nd) {
  // NB: createScaledSysIma necessite natdisp mais en fait ca devrait pas
  if (!disp) {
    UAppli::error("UHardIma::createScaledClone","no available display");
    return null;
  }
  if (!to_nd) to_nd = disp;
  
  USysIma ima_clone = null, imashape_clone = null;
  if (sys_ima) 
    ima_clone = createScaledSysIma(to_nd, disp, sys_ima, xscale, yscale);
  if (sys_imashape)
    imashape_clone = createScaledSysIma(to_nd, disp, sys_imashape, xscale, yscale);
  
  UHardIma2D* ni = new UHardIma2D(to_nd);
  ni->setRasterAndAdopt(ima_clone, imashape_clone);
  return ni;
}

/* ==================================================== ======== ======= */
/* ==================================================== ======== ======= */
//NOTE: les NatPix ne sont pas utilises en mode OpenGL

UHardPix::UHardPix(UDisp* d, UHardIma2D* ima) {
  disp = d;
  sys_pix = sys_pixshape = null;
  if (ima) setRasterFrom(ima->getIma(), ima->getImaShape());
  else setRasterFrom(null, null);
}

UHardPix::~UHardPix() {
#if UBIT_WITH_X11
  UDispX11* d = (UDispX11*)disp;
#else
  UDispGDK* d = (UDispGDK*)disp;
#endif
  if (disp && d->getSysDisp() 
      //obs: && this != natdisp->getUnknownPixmap()
      ) {
    if (sys_pix) DestroyPixmap(d->getSysDisp(), sys_pix);
    if (sys_pixshape) DestroyPixmap(d->getSysDisp(), sys_pixshape);
  }
}

/*
 UHardPix::UHardPix(UDisp* nd, USysIma ima, USysIma imashape) {
   natdisp = nd;
   sys_pix = sys_pixshape = USysNull;
   setRaster(ima, imashape);
 }
 
 UHardPix::UHardPix(UDisp* nd, USysPix p, USysPix m, int ww, int hh) {
   natdisp = nd;
   height = hh; 
   width = ww;
   bpp = 24;
   transparency = (m ? 1 : 0);
   sys_pix = p;
   sys_pixshape = m;
   scale = 1.0;
 }
 */

void UHardPix::setRasterFrom(USysIma _ima, USysIma _imashape) {
#if UBIT_WITH_X11
  UDispX11* d = (UDispX11*)disp;
#else
  UDispGDK* d = (UDispGDK*)disp;
#endif
  
  scale = 1.0;
  if (!_ima) {
    height = 0; 
    width = 0;
    bpp = 0;
    transparency = 0;
    sys_pix = USysNull;
    sys_pixshape = USysNull;
  }
  
  else if (_ima->width == 0 || _ima->height == 0) {
    UAppli::warning("UHardPix::setRasterFrom", "Null_width_or_height");
    // test important car X plante sur XCreatePixmap si une dimension est nulle
    return;
  }
  else {
    USysDisp sys_disp = d->getSysDisp();
    USysWin sys_frame = d->getSysFrame();
    width  = _ima->width;
    height = _ima->height;
    bpp = _ima->depth;
    transparency = (_imashape ? 1 : 0);
    sys_pix = CreatePixmap(sys_disp, sys_frame, width, height, _ima->depth);
    GCValues gval;
    SetForeground(gval, d->getBlackPixel());
    SetBackground(gval, d->getWhitePixel());
    USysGC gc = CreateGC(sys_disp, sys_pix, GCFOREGROUND|GCBACKGROUND, &gval);
    
    DrawImage(sys_disp, sys_pix, gc, _ima, 0,0,0,0, width, height);
    DestroyGC(sys_disp, gc);
    
    if (!_imashape)
      sys_pixshape = USysNull;
    else {
      sys_pixshape = CreatePixmap(sys_disp, sys_frame, width, height, _imashape->depth);
      SetForeground(gval, 1);
      SetBackground(gval, 0);
      //!ATT: le GC est de depth 1 dans ce cas !
      USysGC gc = CreateGC(sys_disp, sys_pixshape, GCFOREGROUND|GCBACKGROUND, &gval);
      DrawImage(sys_disp, sys_pixshape, gc, _imashape, 0,0,0,0, width, height);
      DestroyGC(sys_disp, gc);
    }
  }
}

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */
// NOTE: this part was adapted from gifrsize.c from the Gif-Lib 
// written by Gershon Elber (Ver 0.1, Jul. 1989)

namespace I2D {

struct RGBConvCopy {
  UDisp *to_nd, *from_nd;
  USysIma to_ima, from_ima;
  int to_ima_w, from_ima_w, to_ima_h, from_ima_h;
  
  int from_red_shift, from_green_shift, from_blue_shift; 
  int to_red_shift, to_green_shift, to_blue_shift; 

  static void init_shift(int& to_shift, int& from_shift, 
                         int to_nd_shift, int to_nd_bits,
                         int from_nd_shift, int from_nd_bits) {
    from_shift = from_nd_shift;
    to_shift = to_nd_shift;
    int delta_bits = from_nd_bits - to_nd_bits;
    if (delta_bits > 0) from_shift += delta_bits;
    else if (delta_bits < 0) to_shift += -delta_bits;
  }

  void init() {
    to_ima_w = to_ima->width;
    from_ima_w = from_ima->width;
    to_ima_h = to_ima->height;
    from_ima_h = from_ima->height;
    
    init_shift(to_red_shift, from_red_shift, 
               to_nd->getRedShift(), to_nd->getRedBits(), 
               from_nd->getRedShift(), from_nd->getRedBits());

    init_shift(to_green_shift, from_green_shift, 
               to_nd->getGreenShift(), to_nd->getGreenBits(), 
               from_nd->getGreenShift(), from_nd->getGreenBits());

    init_shift(to_blue_shift, from_blue_shift, 
               to_nd->getBlueShift(), to_nd->getBlueBits(), 
               from_nd->getBlueShift(), from_nd->getBlueBits());
  }

  inline void copy_from2to(int to_x, int to_y, int from_x, int from_y) { 
    unsigned long from_pix = SysGetPixel(from_ima, from_x, from_y);
    unsigned long red   = (from_pix & from_nd->getRedMask())   >> from_red_shift ;
    unsigned long green = (from_pix & from_nd->getGreenMask()) >> from_green_shift;
    unsigned long blue  = (from_pix & from_nd->getBlueMask())  >> from_blue_shift;
     
    // diviser ou masquer eventuellemnet !!!!
    PutPixel(to_ima, to_x, to_y, 
             (red << to_red_shift) 
             + (green << to_green_shift) 
             + (blue  << to_blue_shift)
	     );
  }

  // copies in the same image 'to_ima'
  inline void copy_to2to(int to_x, int to_y, int from_x, int from_y) { 
    PutPixel(to_ima, to_x, to_y, SysGetPixel(to_ima, from_x, from_y));
  }
};

/* ==================================================== ======== ======= */

struct SimpleCopy {
  UDisp *to_nd, *from_nd;
  USysIma to_ima, from_ima;
  int to_ima_w, from_ima_w, to_ima_h, from_ima_h;

  void init() {
    to_ima_w = to_ima->width;
    from_ima_w = from_ima->width;
    to_ima_h = to_ima->height;
    from_ima_h = from_ima->height;
  }
  
  // copies from 'from_ima' to 'to_ima'
  inline void copy_from2to(int to_x, int to_y, int from_x, int from_y) { 
    PutPixel(to_ima, to_x, to_y, SysGetPixel(from_ima, from_x, from_y));
  }
  
  // copies in the same image 'to_ima'
  inline void copy_to2to(int to_x, int to_y, int from_x, int from_y) { 
    PutPixel(to_ima, to_x, to_y, SysGetPixel(to_ima, from_x, from_y));
  }
};

} //end namespace 

#endif //WITH_2D_GRAPHICS

/* ==================================================== ======== ======= */
namespace IM {

#if UBIT_WITH_GL
  
struct GlCopy {
  UDisp *to_nd, *from_nd;
  UHardImaGL* to_ima, *from_ima;
  int to_ima_w, from_ima_w, to_ima_h, from_ima_h;
  int to_scanline, from_scanline;
  unsigned char *to_pixels, *from_pixels;
  
  void init() {
    to_ima_w = to_ima->getWidth();
    from_ima_w = from_ima->getWidth();
    to_ima_h = to_ima->getHeight();
    from_ima_h = from_ima->getHeight();
    
    to_scanline = to_ima_w * to_ima->getBpp()/8;
    from_scanline = from_ima_w * from_ima->getBpp()/8;
    to_pixels = to_ima->getPixels();
    from_pixels = from_ima->getPixels();
  }
  
  // copies from 'from_ima' to 'to_ima'
  inline void copy_from2to(int to_x, int to_y, int from_x, int from_y) {
    //PutPixel(to_ima, to_x, to_y, SysGetPixel(from_ima, from_x, from_y));
    int to = to_y * to_scanline + to_x;
    int from = from_y * from_scanline + from_x;
    to_pixels[to] = from_pixels[from];
    to_pixels[to+1] = from_pixels[from+1];
    to_pixels[to+2] = from_pixels[from+2];
    to_pixels[to+3] = from_pixels[from+3];
  }
  
  // copies in the same image 'to_ima'
  inline void copy_to2to(int to_x, int to_y, int from_x, int from_y) { 
    //PutPixel(to_ima, to_x, to_y, SysGetPixel(to_ima, from_x, from_y));
    int to = to_y * to_scanline + to_x;
    int from = from_y * to_scanline + from_x;
    to_pixels[to] = to_pixels[from];
    to_pixels[to+1] = to_pixels[from+1];
    to_pixels[to+2] = to_pixels[from+2];
    to_pixels[to+3] = to_pixels[from+3];    
  }
};
  
#endif
/* ==================================================== ======== ======= */

template <class ND, class IMA, class COPY>
struct ResizeAlgo : public COPY {

  ResizeAlgo(ND* _to_nd, IMA _to_ima, ND* _from_nd, IMA _from_ima) {
    // g++4 requires this-> before the names of inherited vars in templates
    this->to_nd    = _to_nd;
    this->to_ima   = _to_ima;
    this->from_nd  = _from_nd;
    this->from_ima = _from_ima;
    this->init();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  inline void resizeLine(int in_y, int out_y, float xscale) {
    int in_w  = this->from_ima_w;
    int out_w = this->to_ima_w;
    int in_x, out_x;
    float out_flx = 0.0;
    int last_out_x = 0;
    
    for (in_x = 0; in_x < in_w; out_flx += xscale, in_x++) {
      
      out_x = (int)out_flx;
      if (out_x > out_w) out_x = out_w;
      
      for (; last_out_x < out_x; last_out_x++) 
        this->copy_from2to(last_out_x, out_y, in_x, in_y);
    }
    
    // fill the line if needed
    for (in_x = in_w - 1; last_out_x < out_w; last_out_x++)
      this->copy_from2to(last_out_x, out_y, in_x, in_y);
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  inline void duplicateLine(int to_w, int to_y, int& last_to_y) {
    for (int to_x = 0; to_x < to_w; to_x++) 
      this->copy_to2to(to_x, last_to_y, to_x, to_y);
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  void resizeImage(float xscale, float yscale) {
    int from_h = this->from_ima_h;
    int from_y;
    int to_w = this->to_ima_w;
    int to_h = this->to_ima_h;
    int to_y;
    float y = 0.0;
    int last_to_y = 0;
    
    for (from_y = 0; from_y < from_h; y += yscale, from_y++) {
      to_y = int(y);
      
      //NB: don't call resizeLine(...,out_y..) if out_y >= out_h !
      if (to_y >= to_h) break;
      
      if (last_to_y < to_y) {
        resizeLine(from_y, to_y, xscale);
        
        // duplicate additionnal lines 
        for ( ; last_to_y < to_y; last_to_y++)
          duplicateLine(to_w, to_y, last_to_y);
      }
    }
    
    // add additionnal lines at the end if scale is not dividable
    ++last_to_y;               //skip out_y
    
    if (last_to_y < to_h) {
      to_y = last_to_y;
      
      // from_y -> from_h-1 pour ne pas sortir des limites de l'image
      resizeLine(from_h-1, to_y, xscale);
      
      while (++last_to_y < to_h)
        duplicateLine(to_w, to_y, last_to_y);
    }
  }
};

} //end namespace
/* ==================================================== ======== ======= */
#if UBIT_WITH_GL

static UHardImaGL* _createScaledClone(UDisp* to_nd, UDisp* from_nd,
                                     UHardImaGL* from_ima, float xscale, float yscale) {
 
  UHardImaGL* clone = new UHardImaGL(to_nd, 
                                   int(from_ima->getWidth() * xscale + 0.5),
                                   int(from_ima->getHeight() * yscale + 0.5));
   
  IM::ResizeAlgo<UDisp, UHardImaGL*, IM::GlCopy>  resalgo(to_nd, clone, from_nd, from_ima);
  resalgo.resizeImage(xscale, yscale);
  return clone;
}

#endif
/* ==================================================== ======== ======= */
#if WITH_2D_GRAPHICS

/* NB:
* - undefined result if the NatDisp is different from those of the
*   source_ima (invalid colors if the Visual or the depth are different)
* - image depth must be <= to the NatDisp depth
*/
USysIma UHardIma2D::createScaledSysIma(UDisp* to_nd, UDisp* from_nd,
                                        USysIma from_ima, float xscale, float yscale) {
  int to_depth;
  if (from_ima->depth <= to_nd->getBpp()) to_depth = from_ima->depth;
  else to_depth = to_nd->getBpp();
  
  USysIma to_ima = createEmptySysIma(to_nd,
                                     int(from_ima->width  * xscale + 0.5),
                                     int(from_ima->height * yscale + 0.5),
                                     to_depth);
  if (!to_ima) {
    // inutile: message deja affiche par createEmptyXImage()
    // UAppli::error("UHardIma::createScaledSysIma", "can't create image");
    return null;
  }
  
  if (to_nd == from_nd || to_depth == 1) {
    // if to_nd == from_nd or if depth == 1 no convertion is needed
    IM::ResizeAlgo<UDisp, USysIma, I2D::SimpleCopy> resalgo(to_nd, to_ima, from_nd, from_ima);
    resalgo.resizeImage(xscale, yscale);
  }
  else {
    // RGB values are converted according to the characteristics of
    // to_nd and from_nd
    IM::ResizeAlgo<UDisp, USysIma, I2D::RGBConvCopy> resalgo(to_nd, to_ima, from_nd, from_ima);
    resalgo.resizeImage(xscale, yscale);
  }
  return to_ima;
}

/* ==================================================== ===== ======= */

USysIma UHardIma2D::createEmptySysIma(UDisp* _nd, int width, int height, int depth) {
  if (width <= 0 || height <= 0 || depth <= 0) {
    UAppli::error("UHardIma::createEmptySysIma","null or negative size or depth");
    return null;
  }
  if (depth > _nd->getBpp()) {
    UAppli::error("UHardIma::createEmptySysIma","requested depth %d is too large for this display", depth);
    return null;
  }
  
#if UBIT_WITH_GDK
  UDispGDK* nd = (UDispGDK*)_nd;
  USysIma ima = gdk_image_new(GDK_IMAGE_FASTEST, nd->getSysVisual(), width, height);
  if (!ima) UAppli::error("UHardIma::createEmptySysIma","cant create this image on this display");
  return ima;
  
#elif UBIT_WITH_X11
  UDispX11* nd = (UDispX11*)_nd;
  int bitmap_pad;
  if (depth > 16) bitmap_pad = 32;
  else if (depth > 8) bitmap_pad = 16;    // ?? CORRECT SUR IPAQ ??
  else bitmap_pad = 8;
  
  USysIma ima = 
    XCreateImage(nd->getSysDisp(), nd->getSysVisual()->visual, 
                 depth, ZPixmap, 
                 0,              // offset (# of pixels to ignore at the
                 // beginning of the scanline)
                 NULL,	         // data will be created later
                 width, height,  // requested size
                 bitmap_pad,     // bitmap_pad (quantum of a scanline)
                 0); // bytes_per_line (0 : continuous scanlines=> calculated auto.
  
  if (!ima) {
    UAppli::error("UHardIma::createEmptySysIma","cant create this image on this display");
    return null;
  }
  
  // once ima is created, we can get the actual line width via field 
  // 'bytes_per_line' 
  // !NOTE: it is impossible to guess this value before the XImage 
  // is created as X may attribute more bit per pixel than what we requested
  // (for instance a 32bit-per-pix graphics card will probably attribute
  // 32 bits even if we asked for 24 bits!)
  // => !!!WRONG:
  // datasize = ((float)attr.width * attr.height * nd->getBpp() / 8. + 1);
  
  ima->data = (char*)malloc(ima->bytes_per_line * height);
  if (ima->data) 
    return ima;
  else {
    UAppli::error("UHardIma::createEmptyXImage","No more memory");
    XDestroyImage(ima);
    return null;
  }
#endif // X11
}

/* ==================================================== ===== ======= */

bool UHardIma2D::blendSysIma(UDisp* _nd, USysIma xima1, USysPixel pixel2, float alpha) {
#if UBIT_WITH_X11
  UDispX11* nd = (UDispX11*)_nd;
#else
  UNatDispGDK* nd = (UNatDispGDK*)_nd;
#endif
  
  if (xima1->depth > nd->getBpp()) {
    UAppli::error("UHardIma2D::blendSysIma","source and destination have incompatible depths");
    return false;
  }  

  USysVisual v = nd->getSysVisual();
#if UBIT_WITH_X11
  if (!v || v->c_class != TrueColor) {
    UAppli::error("UHardIma2D::blendSysIma","alpha blending requires TrueColors");
    return false;
  }
#endif
  int ww = xima1->width;
  int hh = xima1->height;
  register unsigned long pix1;
  register float alpha_compl = 1.0 - alpha;
  float 
    pix2_red_alpha   = (pixel2 & v->red_mask)   * alpha,
    pix2_green_alpha = (pixel2 & v->green_mask) * alpha,
    pix2_blue_alpha  = (pixel2 & v->blue_mask)  * alpha;
  unsigned long 
    red_mask   = v->red_mask, 
    green_mask = v->green_mask,
    blue_mask  = v->blue_mask;
  
  for (int y = 0; y < hh; y++)
    for (int x = 0; x < ww; x++) {
      pix1 = SysGetPixel(xima1, x, y); 
      PutPixel(xima1, x, y,
               ((unsigned long)((pix1&red_mask)*alpha_compl + pix2_red_alpha)   & red_mask)
               + 
               ((unsigned long)((pix1&green_mask)*alpha_compl+pix2_green_alpha) & green_mask)
               + 
               ((unsigned long)((pix1 & blue_mask)*alpha_compl+pix2_blue_alpha) & blue_mask)
               );
    }
  return true;
}

/* ==================================================== ======== ======= */

bool UHardIma2D::blendSysIma(UDisp* _d, USysIma xima1, USysIma xima2, float alpha) {
#if UBIT_WITH_X11
  UDispX11* d = (UDispX11*)_d;
#else
  UDispGDK* d = (UDispGDK*)_d;
#endif
  if (xima1->depth != xima2->depth || xima1->depth > d->getBpp()) {
    UAppli::error("UHardIma2D::blendSysIma","source and destination have incompatible depths");
    return false;
  }
  
  USysVisual v = d->getSysVisual();
#if UBIT_WITH_X11
  if (!v || v->c_class != TrueColor) {
    UAppli::error("UHardIma2D::blendSysIma","alpha blending requires TrueColors");
    return false;
  }
#endif

  float alpha_compl = 1.0 - alpha;
  int ww = min(xima1->width, xima2->width);
  int hh = min(xima1->height, xima2->height);
  unsigned long red, blue, green;
  register unsigned long 
    pix1, pix2,
    red_mask   = v->red_mask, 
    green_mask = v->green_mask,
    blue_mask  = v->blue_mask;
 
  for (int y = 0; y < hh; y++)
    for (int x = 0; x < ww; x++) {
      pix1 = SysGetPixel(xima1, x, y); 
      pix2 = SysGetPixel(xima2, x, y);
      red   =(unsigned long)((pix1&red_mask)*alpha_compl   + (pix2&red_mask)*alpha);
      green =(unsigned long)((pix1&green_mask)*alpha_compl + (pix2&green_mask)*alpha);
      blue  =(unsigned long)((pix1&blue_mask)*alpha_compl  + (pix2&blue_mask)*alpha);

      PutPixel(xima1, x, y,
               (red & red_mask) + (green & green_mask) + (blue & blue_mask));
    }
  return true;
}

/* ==================================================== ===== ======= */
// essaye de tout caser dans la Colormap donnee en arg.
// returns the convtable
// NEW: modifie colors

USysPixel* UHardIma2D::allocSysColors(UDisp* _d, USysColor* colors, int col_count) {
#if UBIT_WITH_X11
  UDispX11* d = (UDispX11*)_d;
#else
  UDispGDK* d = (UDispGDK*)_d;
#endif
  USysColormap cmap = d->getSysColormap();
  USysPixel* convtable = new USysPixel[col_count];
  register int k;
  // on essaye d'allouer les couleurs dans la cmap et on voit ce qui reste
  int todoCount = 0;
  int* todo = new int[col_count];

  for (k = 0; k < col_count; k++) {
    if (AllocColor(d->getSysDisp(), cmap, &colors[k])) 
      convtable[k] = colors[k].pixel;
    else {
      convtable[k] = colors[k].pixel;  // pas pire que 0 ?
      todo[todoCount] = k;
      todoCount++;
    }
  }

  // -- si on a reussi a tout caser : c'est fini!
  if (todoCount == 0) {
    delete[] todo;
    return convtable;
  }

#ifdef UBIT_WITH_GDK
  UAppli::error("UHardIma::allocXColors","cant find approximate colors in GDK mode: not implemented");
  return convtable;
#else
  // -- sinon: on associe aux couleurs non-allouees la couleur la plus proche 
  // dans la colormap

  // pas toujours vrai si Visuel specifique ... !!
  // mais comment retrouver la vraie taille de cmap ???

  USysDisp display = d->getSysDisp();
  int num_xcolors = DisplayCells(display, XScreenNumberOfScreen(d->getSysScreen()));

  XColor* xcolors = new XColor[num_xcolors];
  for (k = 0; k < num_xcolors; k++) xcolors[k].pixel = k;
  XQueryColors(display, cmap, xcolors, num_xcolors);

  unsigned long green, red, blue;

  for (k = 0; k < todoCount; k++) {
    red   = colors[todo[k]].red;
    green = colors[todo[k]].green;
    blue  = colors[todo[k]].blue;

    unsigned long dist, mindist = 0xffffffff;
    int jmin = 0;

    for (int j = 0; j < num_xcolors; j++) {
      long r = red   - xcolors[j].red;
      long g = green - xcolors[j].green;
      long b = blue  - xcolors[j].blue;
      if (r <0) r = -r;
      if (g <0) g = -g;
      if (b <0) b = -b;

      // NB: on pourrait faire nettement mieux: voir par exemple create.c
      // dans la distrib XPM
      dist = r + g + b;
      if (dist < mindist) {mindist = dist; jmin = j;}
    }

    convtable[todo[k]] = jmin;
  }

  delete[] xcolors;
  delete[] todo;
  return convtable;
#endif
}

#endif
}

