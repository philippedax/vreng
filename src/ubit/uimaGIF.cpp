/* ==================================================== ======== ======= *
*
* uimaGIF.cpp : glue with the GIF/UNGIF library  (libungif)
* file adapted from the UNGIF "util" program "gif2x11.c" by Gershon Elber.
* "Gif-Lib" - Yet another gif library. Written by:  Gershon Elber
*
* Note: as the GIF image format is based on the (patented) LZW compression
* algorithm, this file does not contain code for compressing GIF images.
* Code which does gif decoding only seems not to be covered by the patent:
* check the ungif web site (or your own lawyer :-) for details.
*
* Latest versions of libungif are available from:
*    ftp://prtr-13.ucsc.edu/pub/libungif/
*
* You can see a web page with late breaking announcements at:
*    http://prtr-13.ucsc.edu/~badger/software/libungif.shtml
*
*  Ubit Project - Eric Lecolinet - ENST Paris
*  Part of the Ubit Toolkit: A Brick Construction Game Model for Creating GUIs
*  http://www.enst.fr/~elc/ubit
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

//if defined (HAVE_LIBGIF) || defined(HAVE_LIBUNGIF) 
#if UBIT_WITH_GIF
#include <iostream>
#include <X11/Xmd.h>  // for WORD64 and CARD32           !!!!!

#if defined (__MSDOS__) && !defined(__DJGPP__) && !defined(__GNUC__)
#  include <graphics.h>
#  include <stdlib.h>
#  include <alloc.h>
#  include <io.h>
#  include <dos.h>
#  include <bios.h>
#  include <sys\stat.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#endif 

#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>

extern "C" {
#  include <gif_lib.h>
}

#include <ubit/udefs.hpp>
#include <ubit/ufile.hpp>
#include <ubit/uima.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uhardima.hpp>
using namespace std;

#ifdef __MSDOS__
extern unsigned int
_stklen = 16384;	  // Increase default stack size
#endif /* __MSDOS__ */

namespace ubit {
  
struct UGif {
  UGif(UHardIma& nima, const UStr& fpath, int max_w, int max_h);
  ~UGif();
  int getStat() const {return stat;};
  
private:
  void setSize(int w, int h, int max_w, int max_h);
  void read();
  void readImage();
  void readExtension();
  void createConvtable();

  struct GifError {
    int errtype;
    GifError(int _errtype) : errtype(_errtype) {}
  };
  
  UHardIma& natima;
  GifFileType* gfile;
  int stat;
  int imacount;
#if WITH_2D_GRAPHICS
  USysIma ima, imashape;
  USysPixel* convtable;
#endif
#if UBIT_WITH_GL
  unsigned char* pixels;
#endif
  GifRowType buf;
  int transpcolor;
  ColorMapObject* gif_colormap;
  
  void (UGif::*put_line)(int x1, int x2, int y);
  void putLine_GL32(int x1, int x2, int y);
  void fillBackground_GL32(int x1, int x2, int y1, int y2);
  void putLine_2DAny(int x1, int x2, int y);
  void fillBackground_2DAny(int x1, int x2, int y1, int y2);
  void putLine_X32(int x1, int x2, int y);
  void putLine_X16(int x1, int x2, int y);
};

static int
InterlacedOffset[] = { 0, 4, 2, 1 },  /* The way Interlaced image should. */
InterlacedJumps[]  = { 8, 8, 4, 2 };   /* be read - offsets and jumps... */

/* ==================================================== [Elc] ======= */
/* ==================================================== ===== ======= */

int UImaGIF::read(UHardIma& nima, const UStr& fpath, int wmax, int hmax) {
  UGif ug(nima, fpath, wmax, hmax);    // !! wmax, hmax only for GL !!!!!!
  return ug.getStat();
}

/* ==================================================== ===== ======= */

UGif::UGif(UHardIma& nima, const UStr& fpath, int max_w, int max_h)
: natima(nima) {
  gfile = null;
  imacount = 0;
#if WITH_2D_GRAPHICS
  ima = null;
  imashape = null;
  convtable = null;
#endif
#if UBIT_WITH_GL
  pixels = null;
#endif
  buf = null;
  transpcolor = -1;  // no transp color if value < 0
  put_line = null;
  stat = UFilestat::Opened;

  //if (from_stdin == 0) {
#if defined(GIFLIB_MAJOR) && (GIFLIB_MAJOR >= 5)
  // There was an API change in version 5.0.0.
  int gif_error = GIF_ERROR;
  if (!(gfile = DGifOpenFileName(fpath.c_str(), &gif_error))) {
#else
  if (!(gfile = DGifOpenFileName(fpath.c_str()))) {
#endif
    stat = UFilestat::CannotOpen;
    return;
  }

/*   // Use the stdin instead
  else {
#ifdef __MSDOS__
    setmode(0, O_BINARY);
#endif
    if ((gfile = DGifOpenFileHandle(0)) == NULL) {
      stat = UFilestat::CannotOpen; return; 
    }
  }
*/

  try {
    read();
  }
  catch(GifError e) {
    stat = e.errtype;
  }
}

/* ==================================================== ======== ======= */
// note that ima and imashape are not deleted by the destructor

UGif::~UGif() {
  delete buf;
#if WITH_2D_GRAPHICS
  delete convtable;
#endif
#if defined(GIFLIB_MAJOR) && (GIFLIB_MAJOR >= 5)
  // There was an API change in version 5.0.0.
  int gif_error = GIF_ERROR;
  if (gfile) DGifCloseFile(gfile, &gif_error);
#else
  if (gfile) DGifCloseFile(gfile);
#endif
}

/*
void UGif::setSize(int w, int h, int max_w, int max_h) {
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
*/
/* ==================================================== ======== ======= */
/* ==================================================== ======== ======= */

void UGif::read() {  
  GifRecordType recordType;

  do {
    if (DGifGetRecordType(gfile, &recordType) == GIF_ERROR)
      throw GifError(UFilestat::InvalidData);

    switch (recordType) {
      case IMAGE_DESC_RECORD_TYPE:
        // this is not really an error but we dont want to read
        // multiple GIF images
        if (imacount > 0) throw GifError(UFilestat::Opened);
        imacount++;
        readImage();
        break;

      case EXTENSION_RECORD_TYPE:
        readExtension();
        break;

      case TERMINATE_RECORD_TYPE:
        break;

      default:		    // Should be traps by DGifGetRecordType.
        break;
    }
  }
  while (recordType != TERMINATE_RECORD_TYPE);
}

/* ==================================================== ======== ======= */
/* ==================================================== ======== ======= */
#if UBIT_WITH_GL

void UGif::putLine_GL32(int x1, int x2, int y) {
  unsigned char* p = &(pixels[(y * gfile->SWidth + x1) * 4]);
  unsigned char* p_end = &(pixels[(y * gfile->SWidth + x2) * 4]);
  //unsigned char* p_end = p + ..image_width * 4;
 
  //for (float x = x1; p < p_end; x+=skip_w, p+=4) {
  for (int x = 0; p < p_end; x+=1, p+=4) {
    unsigned char val = buf[int(x)];
    if (val == transpcolor) {
      p[0] = p[1] = p[2] = p[3] = 0;
    }
    else {
      p[0] = gif_colormap->Colors[val].Red;
      p[1] = gif_colormap->Colors[val].Green;
      p[2] = gif_colormap->Colors[val].Blue;
      p[3] = 0xff;
    }
  }
}

// x2 and y2 exclus
void UGif::fillBackground_GL32(int x1, int x2, int y1, int y2) {
  if (x1 >= x2 || y2 >= y1) return;
  unsigned char val = gfile->SBackGroundColor;
  
  for (int y = y1; y < y2; y++) {
    for (int x = x1; x < x2; x++) {
      unsigned char* p = &(pixels[(y * gfile->SWidth + x) * 4]);
      if (val == transpcolor) {
        p[0] = p[1] = p[2] = p[3] = 0;
      }
      else {
        p[0] = gif_colormap->Colors[val].Red;
        p[1] = gif_colormap->Colors[val].Green;
        p[2] = gif_colormap->Colors[val].Blue;
        p[3] = 0xff;
      }
    }
  }
}

#endif
/* ==================================================== ======== ======= */
#if WITH_2D_GRAPHICS

// x2 exclus
void UGif::putLine_2DAny(int x1, int x2, int y) {
  for (int x = x1; x < x2; x++) {
    SysPutPixel(ima, x, y, convtable[buf[x]]);
    if (imashape) SysPutPixel(imashape, x, y, (buf[x] != transpcolor));
  }
}

// x2 and y2 exclus
void UGif::fillBackground_2DAny(int x1, int x2, int y1, int y2) {
  if (x1 >= x2 || y2 >= y1) return;
  
  unsigned long bgpix = convtable[gfile->SBackGroundColor];
  bool bgpix_opaque = (gfile->SBackGroundColor != transpcolor);
  
  for (int y = y1; y < y2; y++) {
    for (int x = x1; x < x2; x++) {
      SysPutPixel(ima, x, y, bgpix);
      if (imashape) SysPutPixel(imashape, x, y, bgpix_opaque);
    }
  }
}


#if UBIT_WITH_X11

void UGif::putLine_X32(int x1, int x2, int y) {
  unsigned char* p = &((unsigned char*)ima->data)[y*ima->bytes_per_line];

#ifndef WORD64
  static CARD32 const byteorderpixel = MSBFirst << 24;

  if ((*((const char *)&byteorderpixel) == ima->byte_order)) {
    for (int x = x1; x < x2; x++, p+=4)
      *((CARD32 *)p) = convtable[buf[x]];
  }
  else
#endif
  if (ima->byte_order == MSBFirst) {
    for (int x = x1; x < x2; x++, p+=4) {
      unsigned long pixel = convtable[buf[x]];
      p[0] = pixel >> 24;
      p[1] = pixel >> 16;
      p[2] = pixel >> 8;
      p[3] = pixel;
    }
  }
  else {
    for (int x = x1; x < x2; x++, p+=4) {
      unsigned long pixel = convtable[buf[x]];
      p[3] = pixel >> 24;
      p[2] = pixel >> 16;
      p[1] = pixel >> 8;
      p[0] = pixel;
    }
  }
}

void UGif::putLine_X16(int x1, int x2, int y) {
  unsigned char* p = &((unsigned char*)ima->data)[y*ima->bytes_per_line];

  if (ima->byte_order == MSBFirst) {
    for (int x = x1; x < x2; x++, p+=2) {
      unsigned long pixel = convtable[buf[x]];
      p[0] = pixel >> 8;
      p[1] = pixel;
    }
  }
  else {
    for (int x = x1; x < x2; x++, p+=2) {
      unsigned long pixel = convtable[buf[x]];
      p[1] = pixel >> 8;
      p[0] = pixel;
    }
  }
}

#endif
#endif
/* ==================================================== [Elc] ======= */

void UGif::readImage() {
  if (DGifGetImageDesc(gfile) == GIF_ERROR)
    throw GifError(UFilestat::InvalidData);
  
  int row = gfile->Image.Top;   // Image Position relative to Screen
  int col = gfile->Image.Left;
  int iw = gfile->Image.Width;
  int ih = gfile->Image.Height;
  
  //skip_w = skip_h = 1;
  //setSize(..., max_w, max_h); ??? cf JPEG @@@

  if (gfile->Image.Left + iw > gfile->SWidth || gfile->Image.Top + ih > gfile->SHeight) {
    throw GifError(UFilestat::InvalidData); // image is not confined to screen dimension
  }

  // line buffer for reading the GIF file
  buf = new GifPixelType[iw * sizeof(GifPixelType)];

  // create the new image
  natima.setRaster(gfile->SWidth, gfile->SHeight, (transpcolor >= 0));
  // if (!natima) throw GifError(UFilestat::NoMemory);

  // GIF ColorMap (used by GL and by createContable for X11)
  gif_colormap = (gfile->Image.ColorMap ? gfile->Image.ColorMap : gfile->SColorMap);

#if UBIT_WITH_GL
  if (dynamic_cast<UHardImaGL*>(&natima)) {
    pixels = ((UHardImaGL*)&natima)->getPixels();
    put_line = &UGif::putLine_GL32;
    goto DONE;
  }
#endif

#if WITH_2D_GRAPHICS
  if (dynamic_cast<UHardIma2D*>(&natima)) {
    ima = ((UHardIma2D*)&natima)->getIma();
    imashape = ((UHardIma2D*)&natima)->getImaShape();
    createConvtable();
# if UBIT_WITH_GDK
    put_line = &UGif::putLine_2DAny;
# else
    // !ATT: the put_line fct must be set here in an appropriate way
    if (ima->bits_per_pixel == 32 && imashape == null)
      put_line = &UGif::putLine_X32;
    else if (ima->bits_per_pixel == 16 && imashape == null)
      put_line = &UGif::putLine_X16;
    else
      put_line = &UGif::putLine_2DAny;
# endif
    goto DONE;
  }
#endif

  { // cas d'erreur (impossible sauf si ni GL ni 2D_GRAPHICS)
    UAppli::internalError("UGif::readImage","could not create image");
    stat = UFilestat::MiscError;
    return;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DONE:  
  if (gfile->Image.Interlace) {  // Need to perform 4 passes on the images:
    for (int k = 0; k < 4; k++) {
      for (int y = row+InterlacedOffset[k]; y<row+ih; y+=InterlacedJumps[k]){
        if (DGifGetLine(gfile, buf, iw) == GIF_ERROR)
          throw GifError(UFilestat::InvalidData);
        (this->*put_line)(col, col + iw, y);
      }
    }
  }
  else {
    for (int y = row; y < row+ih; y++) {
      if (DGifGetLine(gfile, buf, iw) == GIF_ERROR)
        throw GifError(UFilestat::InvalidData);
      (this->*put_line)(col, col + iw, y);
    }
  }
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if UBIT_WITH_GL
  //if (natima.getType() == UHardIma::GL32) {
  if (dynamic_cast<UHardImaGL*>(&natima)) {
    fillBackground_GL32(0, col, row, row+ih);
    fillBackground_GL32(col+iw, gfile->SWidth, row, row+ih);
    fillBackground_GL32(0, gfile->SWidth, 0, row);
    fillBackground_GL32(0, gfile->SWidth, row+ih, gfile->SHeight);
    return;
  }
#endif

#if WITH_2D_GRAPHICS
  if (dynamic_cast<UHardIma2D*>(&natima)) {
    fillBackground_2DAny(0, col, row, row+ih);
    fillBackground_2DAny(col+iw, gfile->SWidth, row, row+ih);
    fillBackground_2DAny(0, gfile->SWidth, 0, row);
    fillBackground_2DAny(0, gfile->SWidth, row+ih, gfile->SHeight);
    return;
  }
#endif
  
  // cas d'erreur (impossible sauf si ni GL ni 2D_GRAPHICS)
  UAppli::internalError("UHardGifReader","wrong image format");
}

/* ==================================================== ======== ======= */
#if WITH_2D_GRAPHICS

void UGif::createConvtable() {
  // XColorTable will hold the GIF image colors. Note the table
  // has 256 entry which is the maximum allowed in GIF format
  if (gif_colormap->ColorCount > 256) gif_colormap->ColorCount = 256; // protection!
  USysColor colors[256];
  
  for (int i = 0; i < gif_colormap->ColorCount; i++) {
    colors[i].red   = gif_colormap->Colors[i].Red << 8;
    colors[i].green = gif_colormap->Colors[i].Green << 8;
    colors[i].blue  = gif_colormap->Colors[i].Blue << 8;
#ifndef UBIT_WITH_GDK
    colors[i].flags = DoRed | DoGreen | DoBlue;
#endif
  }
  
  //  create color conversion table
  convtable = 
    UHardIma2D::allocSysColors(natima.getDisp(), colors, gif_colormap->ColorCount);  
}

#endif
/* ==================================================== ======== ======= */

void UGif::readExtension() {
  GifByteType *extension = null;
  int extcode = 0;

  // Skip any extension blocks in file:
  if (DGifGetExtension(gfile, &extcode, &extension) == GIF_ERROR)
    throw GifError(UFilestat::InvalidData);

  if (extension) {
    if (extcode == 0xF9) {
      if (extension[1] & 1) transpcolor = int(extension[4]);
    }
    
    //else if (extcode == 0xFE) {
      //if (!comment) {
      //  comment = malloc(*extension+1);
      //  memcpy(comment, extension+1, *extension);
      //  comment[*extension] = '\0';
      //}
    }

  while (extension != NULL) {
    if (DGifGetExtensionNext(gfile, &extension) == GIF_ERROR)
      throw GifError(UFilestat::InvalidData);

    if (extension) {
      if (extcode == 0xF9) {
        if (extension[1] & 1) transpcolor = int(extension[4]);
      }
    }
  }
}

}
#endif
