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
#include "reader.hpp"	// Reader
#include "cache.hpp"	// openCache
#include "file.hpp"	// closeFile
#include "texture.hpp"	// Texture

#if HAVE_TIFFIO_H
#include <tiffio.h>
#endif


/** Load a TIFF image */
Img * Img::loadTIF(void *_tex, ImageReader read_func)
{
#if HAVE_LIBTIFF
  // downloads the tiff file and put it into the cache
  Reader *ir = new Reader(_tex, read_func);

  FILE *f;
  Texture *tex = (Texture *) _tex;
  char cachepath[PATH_LEN] = {0};
  Cache::setCachePath(tex->url, cachepath);
  if ((f = Cache::openCache(cachepath, tex->http)) == NULL) {
    delete ir;
    return NULL;
  }
  File::closeFile(f);
  delete ir;

  // opens the tiff file
  TIFF *fp;
  if (! (fp = TIFFOpen(ir->getFilename(tex), "r"))) return NULL;

  /* reads the header */
  uint16_t width, height;
  uint8_t channel;

  TIFFGetField(fp, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(fp, TIFFTAG_IMAGELENGTH, &height);
  TIFFGetField(fp, TIFFTAG_SAMPLESPERPIXEL, &channel);

  if (channel <= Img::RGB) channel = Img::RGB;
  else channel = Img::RGBA;

  trace(DBG_IMG, "loadTIF: w=%d h=%d c=%d", width, height, channel);

  // always 4 bytes per pixel for this
  uint32 * tmpImage = (uint32 *)_TIFFmalloc((tsize_t)(width * height * sizeof(uint32_t)));

  // reads the data with the library
  if (! TIFFReadRGBAImage(fp, width, height, tmpImage, 0)) {
    error("loadTIF: error reading file %s", ir->getFilename(tex));
    return NULL;
  }

  // allocs img
  Img *img = new Img(width, height, channel);

  // converts component format
  uint8_t *pixptr = img->pixmap;

  // fills the pixmap
  for (int i=0; i < height ; i++) {
    for (int j=0; j < width ; j++) {
      uint32_t pixel = tmpImage[(i*width) + j];

      *pixptr++ = (uint8)TIFFGetR(pixel);
      *pixptr++ = (uint8)TIFFGetG(pixel);
      *pixptr++ = (uint8)TIFFGetB(pixel);
      if (channel == Img::RGBA) {
        *pixptr++ = (uint8)TIFFGetA(pixel);
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
