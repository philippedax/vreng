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
/*  load TGA file
 *      written by Alexander Zaprjagaev
 *      frustum@public.tsu.ru
 */
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, close
#include "file.hpp"	// open, close
#include "texture.hpp"	// Texture


/** TGA info
typedef struct {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
} TGAINFO;
**/


Img * Img::loadTGA(void *tex, ImageReader read_func)
{
  uint16_t width, height;
  uint8_t  bpp;
  uint8_t  rle, TgaInfo[18];
  int size;

  Texture *texture = (Texture *) tex;

  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(texture->url, texture->http)) == NULL) return NULL;

  /* we read the header */
  fread(&TgaInfo, 1, 18, f);  // Read header
  width = TgaInfo[12] + TgaInfo[13] * 256;
  height = TgaInfo[14] + TgaInfo[15] * 256;
  switch (TgaInfo[16]) {      // Read only 32 && 24 bit per pixel
  case 32:
    bpp = 4; // 32 bit per pixel (RGBA)
    break;
  case 24:
    bpp = 3; // 24 bit per pixel (RGB)
    break;
  default:
    cache->close();
    delete cache;
    return NULL;
  }
  trace(DBG_IMG, "loadTGA: w=%d h=%d c=%d", width, height, bpp);
  size = width * height * bpp;

  Img *img = new Img(width, height, Img::RGB);

  // we read the data
  uint8_t *data = new uint8_t[size];  //alloc pixmap
  if (!data) {
    cache->close();
    delete cache;
    return NULL;
  }

  fseek(f, TgaInfo[0], SEEK_CUR);
  int i = 0, j;
  uint8_t *pdata = data;

  switch (TgaInfo[2]) {
  case 2:     // Unmapped RGB image
    fread(data, 1, size, f);
    break;
  case 10:    // Run length encoded
    while (i < size) {
      fread(&rle, 1, 1, f);
        if (rle & 0x80) {
          rle ^= 0x80;
          fread(pdata, 1, bpp, f);
          pdata += bpp;
          for (int j = 0; j < rle * bpp; j++) {
            *pdata = *(pdata - bpp);
            pdata++;
          }
          i += bpp * (rle + 1);
        }
        else {
          int k = bpp * (rle + 1);
          fread(pdata, 1, k, f);
          pdata += k;
          i += k;
        }
      }
      break;
   default:
     cache->close();
     delete cache;
     delete[] data;
     return NULL;
  }
  for (i=0, j=0; i < size; i += bpp, j += 3) {  // BGR -> RGB
    img->pixmap[j]   = data[i+2];
    img->pixmap[j+1] = data[i+1];
    img->pixmap[j+2] = data[i];
  }
  cache->close();
  delete cache;
  delete[] data;
  return img;
}

void Img::saveTGA(const char *filename, GLint width, GLint height, const GLubyte *data)
{
  File *file = new File();
  FILE *f = file->open(filename, "w");
  if (f) {
    const GLubyte *pdata = data;

    fputc(0, f); /* ID Length, 0 => No ID */
    fputc(0, f); /* Color Map Type, 0 => No color map included */
    fputc(2, f); /* Image Type, 2 => Uncompressed, True-color Image */
    fputc(0, f); /* Next five bytes are about the color map entries */
    fputc(0, f); /* 2 bytes Index, 2 bytes length, 1 byte size */
    fputc(0, f);
    fputc(0, f);
    fputc(0, f);
    fputc(0, f); /* X-origin of Image */
    fputc(0, f);
    fputc(0, f); /* Y-origin of Image */
    fputc(0, f);
    fputc(width & 0xff, f);      /* Image Width */
    fputc((width>>8) & 0xff, f);
    fputc(height & 0xff, f);     /* Image Height */
    fputc((height>>8) & 0xff, f);
    fputc(24, f);  /* Pixel Depth, 0x18 => 24 Bits */
    fputc(0x20, f); /* Image Descriptor */
    file->close();
    delete file;

    File *fileout = new File();
    f = fileout->open(filename, "ab");  /* reopen in binary append mode */
    for (int y=height-1 ; y>=0; y--) {
      for (int x=0; x<width; x++) {
        int i = (y*width + x) * 4;
        fputc(pdata[i+2], f); /* write blue */
        fputc(pdata[i+1], f); /* write green */
        fputc(pdata[i], f);   /* write red */
      }
    }
    fileout->close();
    delete fileout;
  }
}
