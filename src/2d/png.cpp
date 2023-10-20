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
#include "img.hpp"	// Img
#include "cache.hpp"	// open, close
#include "file.hpp"	// open, close
#include "texture.hpp"	// Texture
#include "glpng.hpp"	// pngLoadF


Img * Img::loadPNG(void *tex, ImageReader read_func)
{
  Texture *texture = (Texture *) tex;

  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(texture->url, texture->http)) == NULL) return NULL;

  pngRawInfo rawinfo;

  /* we read the data */
  if (! pngLoadF(f, PNG_NOMIPMAP, PNG_SOLID, &rawinfo)) {
    error("can't load png file");
    cache->close();
    delete cache;
    return NULL;
  }
  cache->close();
  delete cache;
  //echo("loadPNG: w=%d h=%d d=%d a=%d", rawinfo.Width, rawinfo.Height, rawinfo.Depth, rawinfo.Alpha);

  Img *img = new Img(rawinfo.Width, rawinfo.Height, Img::RGB);

  img->pixmap = rawinfo.Data;
  return img;
}

void Img::savePNG(const char *filename, GLint width, GLint height) 
{
#if 0 //dax old png version 1.2 uncompatible
#if HAVE_PNG_H
#include <png.h>
  int         rowStride    = (width * 3 + 3) & ~0x3;
  png_bytep*  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  png_structp pngWrite;
  png_infop   pngInfo;
  int         pixel_I;
  GLubyte     *image;

  /* create file */
  File *file = new File();
  FILE *f = file->open(filename, "wb");
  if (!f) {
    error("savePNG: File could not be opened for writing"); return;
  }

  image = (GLubyte *) malloc(width * height * sizeof(GLubyte) * 3);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

  png_bytep pixels = (png_bytep) image;

  for ( pixel_I = 0 ; pixel_I < height ; pixel_I++ ) {
    row_pointers[pixel_I] = (png_bytep) &pixels[rowStride * (height - pixel_I - 1)];
  }

  pngWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pngWrite) {
    error("savePNG: png_create_write_struct failed");
    file->close();
    delete file;
    return;
  }
  pngInfo = png_create_info_struct(pngWrite);
  if (!pngInfo) {
    error("savePNG: png_create_info_struct failed");
    file->close();
    delete file;
    return;
  }

  png_init_io(pngWrite, f);
  png_set_compression_level(pngWrite, Z_BEST_COMPRESSION);
  png_set_IHDR(pngWrite, pngInfo, width, height, 8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT);
  png_write_info(pngWrite, pngInfo);
  png_write_image(pngWrite, row_pointers);
  png_write_end(pngWrite, pngInfo);

  /* Clean Up */
  png_destroy_info_struct(pngWrite, &pngInfo);
  png_destroy_write_struct(&pngWrite, NULL);
  free( row_pointers );
  free(image);
  file->close();
  delete file;
#endif
#endif //dax
}
