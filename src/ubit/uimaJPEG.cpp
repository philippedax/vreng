/*************************************************************************
 *
 *  uimaJPEG.cpp : glue with the JPEG library (libjpeg.so)
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 *  file adapted from the JPEG-lib "example.c" program
 *  the JPEG library is developed by the Independent JPEG Group (IJG)
 *  http://www.ijg.org
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
#if UBIT_WITH_JPEG

// NB: stdio.h and not cstdio because these defs are needed by jpeglib.h
#include <stdio.h>

/* Include file for users of JPEG library.
 * You will need to have included system headers that define at least
 * the typedefs FILE and size_t before you can include jpeglib.h.
 * (stdio.h is sufficient on ANSI-conforming systems.)
 * You may also wish to include "jerror.h".
 */

//EX: include <X11/Xmd.h>     // for WORD64 and CARD32
#include <unistd.h>
#include <sys/types.h>
// ??? _LP64 TOUJOURS DEFINI ???  @@@!!!
#if _LP64
#  define WORD64
#endif 

extern "C" {
#undef HAVE_STDLIB_H     // pour eviter un warning inutile
#include <jpeglib.h>
}
#include <setjmp.h>
#include <iostream>
#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/uima.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uhardima.hpp>
using namespace std;
namespace ubit {

// NOTE: actuellement les images sont toujours lues sur le UDisp d'origine
// (celui de UAppli) puis converties sur d'autres UDisp via les fonctions
// ad hoc de UHardIma. La consequence est que RGBmaps ne contient en fait
// qu'une seule map, celle du UDisp d'origine (mais on pourrait imaginer
// de relire l'image pour chaque UDisp, en particulier pour obtenir une
// neilleure qualite quand le UDisp d'origine est 15 bits

struct UJPEG {
  UJPEG(UHardIma&, const UStr& fname, int max_w, int max_h);
  ~UJPEG();
  int getStat() const {return stat;};

private:
  void readHeader();
  void readData();
  void setSize(int w, int h, int max_w, int max_h);

  // This struct contains the JPEG decompression parameters and pointers to
  //working space (which is allocated as needed by the JPEG library).
  struct jpeg_decompress_struct cinfo;
  
  // We use our private extension JPEG error handler.
  // Note that this struct must live as long as the main JPEG parameter
  // struct, to avoid dangling-pointer problems.
  struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
    static void my_error_exit(j_common_ptr cinfo);
  };
  struct my_error_mgr jerr;
  
  UHardIma& natima;
  FILE* jfile;
  int stat;
  int image_width, image_height;
  float skip_w, skip_h;
#if WITH_2D_GRAPHICS
  USysIma xima;
  USysPixel *red_map, *green_map, *blue_map;
#endif
#if UBIT_WITH_GL
  unsigned char* pixels;
#endif
  JSAMPARRAY buffer;		// output row buffer
  
  void (UJPEG::*put_line)(int x1, int x2, int y);
  void putLine_GL32(int x1, int x2, int y);
  void putLine_2DAny(int x1, int x2, int y);
  void putLine_X32(int x1, int x2, int y);
  void putLine_X16(int x1, int x2, int y);
  
#if WITH_2D_GRAPHICS
  struct RGBmaps {
    ~RGBmaps();
    void getColormaps(UDisp*, USysPixel*& red_map, USysPixel*& green_map, 
                      USysPixel*& blue_map);
    void realizeColormaps(UDisp*, USysPixel*& red_map, USysPixel*& green_map, 
                          USysPixel*& blue_map);
    std::vector<USysPixel*> red_maps;
    std::vector<USysPixel*> green_maps;
    std::vector<USysPixel*> blue_maps;
  };
  static RGBmaps* rgb_maps;  // shared: RGB maps for all displays
#endif
};

#if WITH_2D_GRAPHICS
UJPEG::RGBmaps* UJPEG::rgb_maps = null;  // shared: RGB maps for all displays
#endif

/* ==================================================== ======== ======= */

int UImaJPEG::read(UHardIma& nima, const UStr& fpath, int wmax, int hmax) {
  UJPEG jpeg(nima, fpath, wmax, hmax);
 
 // At this point you may want to check to see whether any corrupt-data
 // warnings occurred (test whether jerr.pub.num_warnings is nonzero).
 return jpeg.getStat();
}

void UJPEG::setSize(int w, int h, int max_w, int max_h) {
  image_width = w;
  image_height = h;
  skip_w = skip_h = 1;
  
  if (max_w > 0 && max_h > 0) {
    float xscale = (float)max_w / w;
    float yscale = (float)max_h / h;
    float xyscale = min(xscale, yscale);   // preserve ratio
    // rescale if image is too large
    if (xyscale < 1.) {
      image_width = int(w * xyscale);
      skip_w = (float)w / image_width;
      
      image_height = int(h * xyscale);
      skip_h = (float)h / image_height;
    }
  }
}

UJPEG::UJPEG(UHardIma& nima, const UStr& fpath, int max_w, int max_h)
: natima(nima) {
  jfile = null;
  buffer = null;
  put_line = null;
  stat = UFileStat::Opened;
#if WITH_2D_GRAPHICS
  xima = null;
  red_map = null; green_map = null; blue_map = null;
#endif
#if UBIT_WITH_GL
  pixels = null;
#endif

  /* we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */
  if (fpath.empty() || !(jfile = fopen(fpath.c_str(), "rb"))) {
    stat = UFilestat::CannotOpen;
    return;
  }

  // We set up the normal JPEG error routines, then override error_exit
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_mgr::my_error_exit;

  // Establish the setjmp return context for my_error_exit to use
  if (setjmp(jerr.setjmp_buffer)) {
    stat = UFilestat::InvalidData;
    return;
  }

  readHeader();
  
  // NB: dont confuse cinfo.image_height/width (the size of image) with
  // with cinfo.output_height/width (the size if rescaled) which it not
  // yet defined at this stage

  if (cinfo.image_width == 0 || cinfo.image_height == 0
      // we can only decode RGB (num=3) or greayscale (num=1)
      || (cinfo.num_components != 3 && cinfo.num_components != 1)) {
    stat = UFilestat::InvalidData;
    return;
  }

  setSize(cinfo.image_width, cinfo.image_height, max_w, max_h);
  natima.setRaster(image_width, image_height, 0);
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if UBIT_WITH_GL
  if (dynamic_cast<UHardImaGL*>(&natima)) {
    pixels = ((UHardImaGL*)&natima)->getPixels();
    if (!pixels) stat = UFilestat::NoMemory;
    else goto DONE;
  }
#endif

#if WITH_2D_GRAPHICS
  if (dynamic_cast<UHardIma2D*>(&natima)) {
    xima = ((UHardIma2D*)&natima)->getIma();
    if (!xima) stat = UFilestat::NoMemory;
    else goto DONE;
  }
#endif

  {  // cas d'erreur (impossible sauf si ni GL ni 2D_GRAPHICS)
    UAppli::internalError("UImaJPEG","could not create image");
    stat = UFilestat::MiscError;
    return;
  }
  
DONE: readData();
}

/* ==================================================== ======== ======= */
// note that ima is not deleted by the destructor

UJPEG::~UJPEG() {
  if (jfile) {
    // We need to clean up the JPEG object, close the input file, and return
    // !ATT: faut-il faire jpeg_destroy_decompress si longjmp ??
    jpeg_destroy_decompress(&cinfo); // ne rien faire si pas lu
    fclose(jfile);
  }
}

/* ==================================================== ======== ======= */
// NOTE:
// la solution: red_map[prow[0]] + green_map[prow[1]] + blue_map[prow[2]]
// est plus generale que: 
//    (prow[0]<<red_shift) + (prow[1]<<green_shift) + (prow[2]<<blue_shift);
// pour 2 raisons:
// a) on est sur d'avoir les bonnes couleurs (sauf en pseudo-color !)
// b) si chaque pixel fait 15 bits (ipaq) les couleurs vont baver
//    car celles de jpeg font 8 bits et celles de X 5 ou 6 bits
//    (il faudrait egalement faire un XOR pour chaque composante)
// d'autre part
// l'expression: red_map[*prow++] + green_map[*prow++] + blue_map[*prow++]
// est indefinie (qunad les ++ sont-ils effectues ?)

#if UBIT_WITH_GL 
  // ATT: on ne tient plus compte de xin2 mais seulement de image_width !!!
void UJPEG::putLine_GL32(int xin1, int xin2, int yout) {
  JSAMPROW pin = buffer[0];
  unsigned char* pout = &(pixels[yout * image_width * 4]);
  unsigned char* pout_end = pout + image_width * 4;

  if (cinfo.output_components == 3) {      // RGB
    // buggy a cause des erreurs d'arrondis!
    //for (float x = x1; x < x2; x+=skip_w, pout += 4) 
    for (float xin = xin1; pout < pout_end; xin+=skip_w, pout+=4) {
      pin = buffer[0] + 3 * int(xin);
      *pout     = *pin;
      *(pout+1) = *(pin+1);
      *(pout+2) = *(pin+2);
      *(pout+3) = 0xff;       // pas de transparence:
    }
  }
  else if (cinfo.output_components == 1) {  // GREYSCALE
    for (float xin = xin1; pout < pout_end; xin+=skip_w, pout+=4) {
      pin = buffer[0] + int(xin);
      *pout     = *pin;
      *(pout+1) = *pin;
      *(pout+2) = *pin;
      *(pout+3) = 0xff;       // pas de transparence:
    }
  }
}

#endif
/* ==================================================== ======== ======= */
#if WITH_2D_GRAPHICS

// x2 exclus
void UJPEG::putLine_2DAny(int xin1, int xin2, int yout) {
  JSAMPROW pin = buffer[0];
  int xout = 0;
  int xout_end = image_width;

  if (cinfo.output_components == 3) {      // RGB
    //for (float xin = x1; xin < x2; xin+=skip_w, xout++) {
    for (float xin = xin1; xout < xout_end; xin+=skip_w, xout++) {
      pin = buffer[0] + 3 * int(xin);
      SysPutPixel(xima, xout,yout, red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]]);
    }
  }

  else if (cinfo.output_components == 1) {  // GREYSCALE
    //for (float x = x1; x < x2; x+=skip_w, xout++) {
    for (float xin = xin1; xout < xout_end; xin+=skip_w, xout++) {
      pin = buffer[0] + int(xin);
      SysPutPixel(xima, xout,yout, red_map[*pin] + green_map[*pin] + blue_map[*pin]);
    }
  }
}

#if UBIT_WITH_X11

void UJPEG::putLine_X32(int xin1, int xin2, int yout) {
  JSAMPROW pin = buffer[0];
  unsigned char* pout = &((unsigned char*)xima->data)[yout * xima->bytes_per_line];
  unsigned char* pout_end = pout + image_width*4; //32bpp

#ifndef WORD64
  //static CARD32 const byteorderpixel = MSBFirst << 24;
  static int32_t const byteorderpixel = MSBFirst << 24;

  if ((*((const char*)&byteorderpixel) == xima->byte_order)) {
    // buggy a cause des erreurs d'arrondis!
    // for (float x = x1; x < x2; x+=skip_w, pout+=4, i++) {
    for (float xin = xin1; pout <  pout_end; xin+=skip_w, pout+=4) {
      pin = buffer[0] + 3 * int(xin);
      //*((CARD32*)pout) = red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]];
      *((int32_t*)pout) = red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]];
    }
  }
  else
#endif
  if (xima->byte_order == MSBFirst) {
    //for (float x = x1; x < x2; x+=skip_w, pout+=4) {
    for (float xin = xin1; pout <  pout_end; xin+=skip_w, pout+=4) {
      pin = buffer[0] + 3 * int(xin);
      unsigned long pixel = red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]];
      pout[0] = pixel >> 24;
      pout[1] = pixel >> 16;
      pout[2] = pixel >> 8;
      pout[3] = pixel;
    }
  }
  else {
    //for (float x = x1; x < x2; x+=skip_w, pout+=4) {
    for (float xin = xin1; pout <  pout_end; xin+=skip_w, pout+=4) {
      pin = buffer[0] + 3 * int(xin);
      unsigned long pixel = red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]];
      pout[3] = pixel >> 24;
      pout[2] = pixel >> 16;
      pout[1] = pixel >> 8;
      pout[0] = pixel;
    }
  }
}


void UJPEG::putLine_X16(int xin1, int xin2, int yout) {
  JSAMPROW pin = buffer[0];
  unsigned char* pout = &((unsigned char*)xima->data)[yout * xima->bytes_per_line];
  unsigned char* pout_end = pout + image_width*2; //16bpp

  if (xima->byte_order == MSBFirst) {
    //for (float x = x1; x < x2; x+=skip_w, pout+=2) {
    for (float xin = xin1; pout <  pout_end; xin+=skip_w, pout+=2) {
      pin = buffer[0] + 3 * int(xin);
      unsigned long pixel = red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]];
      pout[0] = pixel >> 8;
      pout[1] = pixel;
    }
  }
  else {
    //for (float x = x1; x < x2; x+=skip_w, pout+=2) {
    for (float xin = xin1; pout <  pout_end; xin+=skip_w, pout+=2) {
      pin = buffer[0] + 3 * int(xin);
      unsigned long pixel = red_map[pin[0]] + green_map[pin[1]] + blue_map[pin[2]];
      pout[1] = pixel >> 8;
      pout[0] = pixel;
    }
  }
}

#endif
#endif
/******************** JPEG DECOMPRESSION SAMPLE INTERFACE *******************/

/* This half of the example shows how to read data from the JPEG decompressor.
 * It's a bit more refined than the above, in that we show:
 *   (a) how to modify the JPEG library's standard error-reporting behavior;
 *   (b) how to allocate workspace using the library's memory manager.
 *
 * Just to make this example a little different from the first one, we'll
 * assume that we do not intend to put the whole image into an in-memory
 * buffer, but to send it line-by-line someplace else.  We need a one-
 * scanline-high JSAMPLE array as a work buffer, and we will let the JPEG
 * memory manager allocate it for us.  This approach is actually quite useful
 * because we don't need to remember to deallocate the buffer separately: it
 * will go away automatically when the JPEG object is cleaned up.
 */

/* ==================================================== ======== ======= */
/*
 * Sample routine for JPEG decompression.  We assume that the source file name
 * is passed in.  We want to return 1 on success, 0 on error.
 */
void UJPEG::readHeader() {

  /* Step 1: allocate and initialize JPEG decompression object */

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, jfile);
  
  /* Step 3: read file parameters with jpeg_read_header() */

  jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */
}

/* ==================================================== ======== ======= */
/* ==================================================== ======== ======= */

void UJPEG::readData() {
  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 

  // physical row width in output buffer: JSAMPLEs per row in output buffer
  int row_stride = cinfo.output_width * cinfo.output_components;

  // Make a one-row-high sample array that will go away when done with image
  buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
 
#if UBIT_WITH_GL
  if (dynamic_cast<UHardImaGL*>(&natima)) {
    put_line = &UJPEG::putLine_GL32;
    goto DONE;
  }
#endif

#if WITH_2D_GRAPHICS
  if (dynamic_cast<UHardIma2D*>(&natima)) {
    if (!rgb_maps) rgb_maps = new RGBmaps();
    rgb_maps->getColormaps(natima.getDisp(), red_map, green_map, blue_map);

# if UBIT_WITH_GDK
    put_line = &UJPEG::putLine_2DAny;
# else
    // !ATT: the put_line fct must be set here in an appropriate way
    if (xima->bits_per_pixel == 32 && cinfo.output_components == 3)
      put_line = &UJPEG::putLine_X32;  // ONLY for RGB!
    else if (xima->bits_per_pixel == 16 && cinfo.output_components == 3)
      put_line = &UJPEG::putLine_X16;  // ONLY for RGB!
    else
      put_line = &UJPEG::putLine_2DAny;
# endif
    goto DONE;
  }
#endif

  {  // cas d'erreur (impossible sauf si ni GL ni 2D_GRAPHICS)
    UAppli::internalError("UJPEG","wrong image class");
    stat = UFilestat::MiscError;
    return;
  }

DONE: /* Step 6: while (scan lines remain to be read) */
 
  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  int outy = 0;
  float nexty = 0;
  for (int y = 0; cinfo.output_scanline < cinfo.output_height; y++) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    jpeg_read_scanlines(&cinfo, buffer, 1);
    if (y >= nexty) {
      (this->*put_line)(0, cinfo.output_width, outy);
      nexty += skip_h;
      outy++;
    }
  }

  /* Step 7: Finish decompression */

  jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  // fait dans le destructeur: jpeg_destroy_decompress(&cinfo);

  /* And we're done! */
}

/*
 * SOME FINE POINTS:
 *
 * In the above code, we ignored the return value of jpeg_read_scanlines,
 * which is the number of scanlines actually read.  We could get away with
 * this because we asked for only one line at a time and we weren't using
 * a suspending data source.  See libjpeg.doc for more info.
 *
 * We cheated a bit by calling alloc_sarray() after jpeg_start_decompress();
 * we should have done it beforehand to ensure that the space would be
 * counted against the JPEG max_memory setting.  In some systems the above
 * code would risk an out-of-memory error.  However, in general we don't
 * know the output image dimensions before jpeg_start_decompress(), unless we
 * call jpeg_calc_output_dimensions().  See libjpeg.doc for more about this.
 *
 * Scanlines are returned in the same order as they appear in the JPEG file,
 * which is standardly top-to-bottom.  If you must emit data bottom-to-top,
 * you can use one of the virtual arrays provided by the JPEG memory manager
 * to invert the data.  See wrbmp.c for an example.
 *
 * As with compression, some operating modes may require temporary files.
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.doc.
 */

/* ==================================================== ======== ======= */
/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * Our example here shows how to override the "error_exit" method so that
 * control is returned to the library's caller when a fatal error occurs,
 * rather than calling exit() as the standard error_exit method does.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.  (If we were using C++, we'd say we
 * were making a subclass of the regular error handler.)
 *
 * Here's the extended error handler struct:
 */

/*
 * Here's the routine that will replace the standard error_exit method:
 */
//extern "C" {

void UJPEG::my_error_mgr::my_error_exit(j_common_ptr cinfo) {
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_mgr* myerr = (my_error_mgr*) cinfo->err;
  
  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  // (*cinfo->err->output_message) (cinfo);
  
  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}
//}

/* ==================================================== ======== ======= */
#if WITH_2D_GRAPHICS

UJPEG::RGBmaps::~RGBmaps() {
  // a completer: detruire les colormaps !!!!!
}

void UJPEG::RGBmaps::getColormaps(UDisp* nd, USysPixel*& red_map,
                                     USysPixel*& green_map, USysPixel*& blue_map) {
  int id = nd->getID();
  if (id < (int)red_maps.size() && red_maps[id] != null) {
    red_map   = red_maps[id];
    green_map = green_maps[id];
    blue_map  = blue_maps[id];
  }
  else {
     // allouer puis stocker les nouvelles tables
    realizeColormaps(nd, red_map, green_map, blue_map);
  }
}

void UJPEG::RGBmaps::realizeColormaps(UDisp* nd, USysPixel*& red_map,
                                         USysPixel*& green_map, USysPixel*& blue_map) {
  USysColor red_colors[256], green_colors[256], blue_colors[256];
  
  for (int i = 0; i < 256; i++) {
    red_colors[i].red = (i << 8);
    red_colors[i].green = 0;
    red_colors[i].blue  = 0;

    green_colors[i].red = 0;
    green_colors[i].green = (i << 8);
    green_colors[i].blue  = 0;

    blue_colors[i].red = 0;
    blue_colors[i].green = 0;
    blue_colors[i].blue  = (i << 8);
    
#ifndef UBIT_WITH_GDK
    red_colors[i].flags = DoRed;
    green_colors[i].flags = DoGreen;
    blue_colors[i].flags = DoBlue;
#endif
  }

  // ne marche pas en pseudo-colors
  red_map   = UHardIma2D::allocSysColors(nd, red_colors, 256);
  green_map = UHardIma2D::allocSysColors(nd, green_colors, 256);
  blue_map  = UHardIma2D::allocSysColors(nd, blue_colors, 256);

  int id = nd->getID();

  for (int k = red_maps.size(); k <= id; k++) {  // allonger les tables;
    red_maps.push_back(null);
    green_maps.push_back(null);
    blue_maps.push_back(null);
  }

  red_maps[id]   = red_map;
  green_maps[id] = green_map;
  blue_maps[id]  = blue_map;
}

#endif // !defined(UBIT_WITH_SDL)

}
#endif
