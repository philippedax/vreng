/* ***********************************************************************
 *
 *  uhardima.hpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 1999-2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#ifndef _uhardima_hpp_
#define	_uhardima_hpp_ 1
#include <ubit/ubit_features.h>
#include <ubit/udisp.hpp>
#if UBIT_WITH_GL
#  include <ubit/ugl.hpp>
#endif

#if WITH_2D_GRAPHICS

#if UBIT_WITH_X11
#  include <X11/Xlib.h>
#  include <X11/Xutil.h> 
#define SysPutPixel(I,X,Y,P)  XPutPixel(I,X,Y,P)
#define SysGetPixel(I,X,Y)    XGetPixel(I,X,Y)
#define USysNull       None
typedef XImage*        USysIma;
typedef Pixmap         USysPixmap;
typedef unsigned long  USysPixel;
typedef XColor         USysColor;
typedef XPoint         USysPoint;

#elif UBIT_WITH_GDK
#  include <gdk/gdk.h> 
#define SysPutPixel(I,X,Y,P)  gdk_image_put_pixel(I,X,Y,P)
#define SysGetPixel(I,X,Y)    gdk_image_get_pixel(I,X,Y)
#define USysNull       NULL
typedef GdkImage*      USysIma;
typedef GdkPixmap*     USysPixmap;
typedef guint32        USysPixel;
typedef GdkColor       USysColor;
typedef GdkPoint       USysPoint;
#endif

#endif

namespace ubit {
  
  struct UImaGIF {
    static int read(UHardIma&, const UStr& fname, int wmax, int hmax);
  };
  
  struct UImaJPEG {
    static int read(UHardIma&, const UStr& fname, int wmax, int hmax);
  };
  
  struct UImaXPM {
    static int read(UHardIma&, const UStr& fname, int wmax, int hmax);
    static int readFromData(UHardIma&, const char** xpm_data, int wmax, int hmax);
  };
  
  /* ==================================================== ===== ======= */
  /** [Impl] image implementation (encapsulates system resources).
   */
  class UHardIma {
  public:
    virtual ~UHardIma();
    
    virtual int read(const UStr& filename, const char* filetype = null, 
                     int max_w = 0, int max_h = 0);
    /**< reads and init an image from a file.
     * the filetype can be one of "gif" "jpeg" "xpm" or null in which case the type
     * is deduced from the filename suffix. returns the UFileStat.
     */
    
    virtual int readFromData(const char **xpm_data, int max_w = 0, int max_h = 0);
    ///< reads and init an image from compiled XPM data.
    
    virtual void setRaster(int width, int height, int alpha_hint) = 0;
    /**< inits the raster with this size (previous data is destroyed if any).
     * the pixel raster is created if width AND height are > 0. It is null otherwise.
     * The previous pixel raster is always destroyed, if any.
     *
     * 'alpha_hint' specifies if the buffer has an alpha channel. it can be 0 (no alpha)
     * or 1 (bitmask) for X11 and 8 (8 alpha bits) for GL.
     */
    
#if WITH_2D_GRAPHICS
    virtual void setRasterAndAdopt(USysIma image, USysIma shapemask) = 0;
    /**< inits the raster from these image and adopt then (see NOTE!).
     * image & shapemask ARE ADOPTED and MUST NOT be accessed after this call: 
     * they may be destroyed immediately by this call, or at a later time when the
     * UHardIma instance is deleted. 
     *
     * 'shapemask' is a bitmap image that specifies the shape of the image.
     *  The image is opaque if 'shapemask' is null: @see getTransparency().
     */
#endif
    
    virtual UHardIma* createScaledClone(float xscale, float yscale, UDisp* = 0) = 0;
    ///< creates a scaled copy of this image.
    
    virtual int getWidth()  const {return width;}
    virtual int getHeight() const {return height;}
    virtual int getBpp()    const {return bpp;}
    
    virtual int getTransparency()  const {return transparency;}
    ///< 0 (opaque), 1(bitmask) or >1(alpha channel).
    
    UDisp* getDisp() const {return disp;}
    virtual float getScale() const = 0;
    virtual bool isRealized() const = 0;
    
  protected:
    friend class UIma;
    friend class UGraphCtxX11;
    friend class UGraphCtxGDK;
    UDisp* disp;
    int width, height;
    unsigned char bpp;
    unsigned char transparency;
    UHardIma() {};
    UHardIma& operator=(const UHardIma&);
  };
  
  /* ==================================================== ===== ======= */
#if UBIT_WITH_GL
  /** [Impl] OpenGL implementation of UHardIma (requires OpenGL).
   */
  class UHardImaGL : public UHardIma {
  public:
    UHardImaGL(UDisp*, int width = 0, int height = 0, int transparency_hint = 24);
    /**< creates an image with this size.
     * the pixel raster is created if width AND height are > 0. It is null otherwise.
     */
    
    virtual ~UHardImaGL();
    
    virtual bool isRealized() const;
    
    virtual void setRaster(int width, int height, int transparency_hint);
    
    virtual void setRasterAndAdopt(unsigned char* pixels, int width, int height);
    /**< inits the raster from these pixel buffer and adopts it (see NOTE!).
     * the 'pixels' buffer IS ADOPTED and MUST NOT be accessed after this call: 
     * it may be destroyed immediately by this call, or at a later time when the
     * UHardIma instance is deleted. 
     *
     * The 'pixels' buffer must formatted according to GL_RGBA, with 8 bits per RGBA component.
     */
    
#if WITH_2D_GRAPHICS
    virtual void setRasterAndAdopt(USysIma ima, USysIma imashape);
#endif
    
    virtual UHardIma* createScaledClone(float xscale, float yscale, UDisp* = 0);
    
    unsigned char* getPixels() {return pixels;}
    // USysPixel getPixel(int x, int y);
    // void setPixel(USysPixel p, int x, int y);
    
    virtual float getScale() const {return 1.;}
    
    GLuint getTexID() const {return texid;}
    
  protected:
    friend class UGlcontext;
    // inherited int width, height; ATTENTION: taille LOGIQUE de l'image, pas de 
    // la texture, qui est toujours une puissance de 2!
    unsigned char* pixels;   // image data for OpenGL
    mutable GLuint texid;    // texture ID
    void createTexFromPixels() const;
  };
  
#endif // UBIT_WITH_GL
  
  /* ==================================================== ===== ======= */
  
#if WITH_2D_GRAPHICS
  
  /** [Impl]  X11/GDK implementation of UHardIma (requires X11 or GDK).
   * can have a "transparent background" (mais seul XPM Reader lit images transp !!)
   */
  class UHardIma2D : public UHardIma {
  public:
    UHardIma2D(UDisp*, int width = 0, int height = 0, int transparency_hint = 0);
    /**< creates an image with this size.
     * the pixel raster is created if width AND height are > 0. It is null otherwise.
     */
    
    virtual ~UHardIma2D();
    
    virtual void setRaster(int width, int height, int transparency_hint);
    virtual void setRasterAndAdopt(USysIma ima, USysIma shape);
    
    virtual UHardIma* createScaledClone(float xscale, float yscale, UDisp* = 0);
    
#if UBIT_WITH_GDK
    unsigned long getPixel(int x, int y) {return gdk_image_get_pixel(sys_ima, x, y);}
    void setPixel(unsigned long p, int x, int y) {gdk_image_put_pixel(sys_ima, p, x, y);}
#elif UBIT_WITH_X11
    unsigned long getPixel(int x, int y) {return XGetPixel(sys_ima, x, y);}
    void setPixel(unsigned long p, int x, int y) {XPutPixel(sys_ima, p, x, y);}
#endif
    
    virtual float getScale() const {return scale;}
    
    USysIma getIma() {return sys_ima;}
    USysIma getImaShape() {return sys_imashape;}
    virtual bool isRealized() const {return sys_ima != null;}
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    static USysIma createEmptySysIma(UDisp*, int width, int height, int depth);
    /**< creates a system (eg. X11) image with an empty (but allocated) data buffer.
     * this function allocates the image and its 'data' buffer
     * - size and depth must be > 0
     * - depth must be <= to the UDisp depth
     * - returns null if the image can't be created
     */
    
    static USysIma createScaledSysIma(UDisp* to_nd, UDisp* from_nd,
                                      USysIma from_ima, float xscale, float yscale);
    /**< creates a scaled copy of an XImage.
     * args:
     * - to_nd   : the UDisp of the XImage to be created
     * - from_nd : the UDisp of from_ima
     * - this functions converts image pixels according to both NatDisps
     * - returns null if the image could not be created
     */
    
    static bool blendSysIma(UDisp*, USysIma i1, USysIma i2, float alpha);
    /**< blends i1 with i2 (the resulting image is stored in i1).
     * requires TrueColors and returns false if images or natdisp are not compatible.
     */
    
    static bool blendSysIma(UDisp*, USysIma i1, USysPixel p2, float alpha);
    /**< blends i1 with p2 (the resulting image is stored in i1).
     * requires TrueColors and returns false if images or natdisp are not compatible.
     */
    
    static USysPixel* allocSysColors(UDisp*, USysColor* colors, int col_count); 
    ///< allocates the xcolors and returns the conversion table (which must be deleted[] after use).
    
  protected:
    friend class UIma;
    friend class UX11context;
    USysIma sys_ima, sys_imashape;
    float scale;
  };
  
  
  /* ==================================================== ===== ======= */
  /** [Impl]. native pixmap (encapsulates the corresponding X resources).
   */
  class UHardPix {
  public:
    UHardPix(UDisp*, UHardIma2D* image = null);
    /**< creates a system (eg. X11) pixmap.
     * if 'image' is not null, the UHardPix is init from this image.
     */
    
    virtual ~UHardPix();
    
    virtual void setRasterFrom(const USysIma image, const USysIma shapemask = 0);
    /**< init the raster from these images.
     * 'shapemask' is a bitmap that specifies the shape of the pixmap.
     * The pixmap is opaque if 'shapemask' is null: @see getTransparency().
     *
     * Note that ima & shape are not adopted: they can be used (or destroyed) 
     * after calling this function.
     */
    
    virtual int getWidth()  const {return width;}
    virtual int getHeight() const {return height;}
    virtual int getBpp()    const {return bpp;}
    virtual int getTransparency()  const {return transparency;}
    UDisp* getDisp() const {return disp;}
    virtual float getScale() const {return scale;}
    virtual bool isRealized() const {return sys_pix != USysNull;}
    
    USysPixmap getPixmap()      {return sys_pix;}
    USysPixmap getPixmapShape() {return sys_pixshape;}
    
  private:
    friend class UPix;
    friend class UX11context;
    UDisp* disp;
    int width, height;
    unsigned char bpp;
    unsigned char transparency;
    USysPixmap sys_pix, sys_pixshape;
    float scale; 
  };
  
#endif
  
}
#endif
