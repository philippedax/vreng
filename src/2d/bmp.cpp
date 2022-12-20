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
#include "cache.hpp"    // open, close
#include "texture.hpp"  // Texture
#include "reader.hpp"	// Reader


struct bmp_header {
  uint8_t magic1;	// = 'B'	:0
  uint8_t magic2;	// = 'M'	:1
  int32_t file_size;	// 		:2
  int32_t reserved;	// 		:6
  int32_t data_offset;	// = 54 0x36	:10
  int32_t header_size;	// = 40 0x28	:14
  int32_t width;	// 		:18
  int32_t height;	// 		:22
  int16_t planes;	// = 1		:26
  int16_t bit_count;	// 1..24	:28
  int32_t compression;	// = 0		:30
  int32_t image_size;	// 		:34
  int32_t x_ppm;	// X pix/meter	:38
  int32_t y_ppm;	// Y pix/meter	:42
  int32_t colors_used;	// 		:46
  int32_t colors_vip;	// colors	:50
};			// 		:54


/** Load a 24bpp BMP image */
Img * Img::loadBMP(void *tex, ImageReader read_func)
{
  int32_t width, height;
  int32_t file_size, data_offset, image_size;
  int16_t compression, bit_count;
  int magic1, magic2;

  Texture *texture = (Texture *) tex;

  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(texture->url, texture->http)) == NULL) return NULL;

  /* we read the header */
  Reader *ir = new Reader(tex, read_func);

  magic1 = getc(f); magic2 = getc(f);
  if (magic1 != 'B' || magic2 != 'M') {
    error("LoadBMP: %s not a bmp file magic=%c%c", ir->getFilename(tex), magic1, magic2);
    cache->close();
    delete cache;
    return NULL;
  }

  fseek(f, 4L, SEEK_CUR);	// reserved and ignored
  fseek(f, 4L, SEEK_CUR);	// file_size wrong
  data_offset = ir->getUInt(f);
  fseek(f, 4L, SEEK_CUR);	// header_size
  width = ir->getUInt(f);
  height = ir->getUInt(f);
  fseek(f, 2L, SEEK_CUR);	// planes
  bit_count = ir->getShort(f);
  if (bit_count != 24) {
    error("loadBMP: don't support %d bpp", bit_count);
    cache->close();
    delete cache;
    return NULL;
  }
  compression = ir->getUInt(f);
  if (compression != 0) {
    error("loadBMP: compression not supported");
    cache->close();
    delete cache;
    return NULL;
  }
  image_size = ir->getUInt(f);
  fseek(f, 0L, 2);
  file_size = ftell(f);
  if (image_size == 0) {
    image_size = file_size - data_offset;
  }

  delete ir;

  trace(DBG_IMG, "loadBMP: w=%d h=%d c=%d s=%d", width, height, bit_count, image_size);

  Img *img = new Img(width, height, Img::RGB);

  // we read the data
  fseek(f, (long) data_offset, 0);
  fread((char *) img->pixmap, 1, image_size, f);

  cache->close();
  delete cache;

  //Inverse R et B
  for (int i=0; i < width*height ; i++) {
    uint8_t t = img->pixmap[i*3];
    img->pixmap[i*3] = img->pixmap[i*3+2];
    img->pixmap[i*3+2] = t;
  }

  return img;
}
