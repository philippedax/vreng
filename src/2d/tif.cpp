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
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, close
#include "texture.hpp"	// Texture

#if HAVE_TIFFIO_H
#include <tiffio.h>
#endif


/** Load a TIFF image */
Img * Img::loadTIF(void *_tex, ImageReader read_func)
{
#if HAVE_LIBTIFF
  // downloads the tiff file and put it into the cache

  Texture *tex = (Texture *) _tex;
  char filename[PATH_LEN] = {0};

  Cache::setCachePath(tex->url, filename);
  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(filename, tex->http)) == NULL) {
    delete cache;
    return NULL;
  }
  cache->close();
  delete cache;

  // opens the tiff file
  TIFF *fp;
  if (! (fp = TIFFOpen(filename, "r"))) return NULL;

  /* reads the header */
  uint16_t width, height;
  uint8_t bpp;

  TIFFGetField(fp, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(fp, TIFFTAG_IMAGELENGTH, &height);
  TIFFGetField(fp, TIFFTAG_SAMPLESPERPIXEL, &bpp);

  if (bpp <= Img::RGB) bpp = Img::RGB;
  else bpp = Img::RGBA;

  trace(DBG_2D, "loadTIF: w=%d h=%d c=%d", width, height, bpp);

  // always 4 bytes per pixel for this
  uint32_t * tmpImage = (uint32_t *)_TIFFmalloc((tsize_t)(width * height * sizeof(uint32_t)));

  // reads the data with the library
  if (! TIFFReadRGBAImage(fp, width, height, tmpImage, 0)) {
    error("loadTIF: error reading file %s", filename);
    return NULL;
  }

  // allocs img
  Img *img = new Img(width, height, bpp);

  // converts component format
  uint8_t *pixptr = img->pixmap;

  // fills the pixmap
  for (int i=0; i < height ; i++) {
    for (int j=0; j < width ; j++) {
      uint32_t pixel = tmpImage[(i*width) + j];

      *pixptr++ = (uint8_t)TIFFGetR(pixel);
      *pixptr++ = (uint8_t)TIFFGetG(pixel);
      *pixptr++ = (uint8_t)TIFFGetB(pixel);
      if (bpp == Img::RGBA) {
        *pixptr++ = (uint8_t)TIFFGetA(pixel);
      }
    }
  }
  _TIFFfree(tmpImage);
  TIFFClose(fp);

  return img;
#else
  return NULL;
#endif
}
