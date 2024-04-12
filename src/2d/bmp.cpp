//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
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
Img * Img::loadBMP(void *_tex, ImageReader read_func)
{
  struct bmp_header h;

  Texture *tex = static_cast<Texture *>(_tex);

  Cache *cache = new Cache();
  FILE *f = cache->open(tex->url, tex->http);
  if (! f) {
    error("cant't read %s", tex->url);
    delete cache;
    return NULL;
  }

  h.magic1 = cache->read_char(f);
  h.magic2 = cache->read_char(f);
  if (h.magic1 != 'B' || h.magic2 != 'M') {
    error("LoadBMP: %s not a bmp file magic=%c%c", cache->getFilename(tex), h.magic1, h.magic2);
errbmp:
    if (cache) {
      cache->close();
      delete cache;
    }
    return NULL;
  }

  fseek(f, 4L, SEEK_CUR);	// reserved and ignored
  fseek(f, 4L, SEEK_CUR);	// file_size wrong
  h.data_offset = cache->read_long_le(f);
  fseek(f, 4L, SEEK_CUR);	// header_size
  h.width = cache->read_long_le(f);
  h.height = cache->read_long_le(f);
  //echo("bmp: w=%d h=%d", h.width, h.height);
  fseek(f, 2L, SEEK_CUR);	// planes
  h.bit_count = cache->read_short_le(f);
  if (h.bit_count != 24) {
    error("loadBMP: don't support %d bpp", h.bit_count);
    goto errbmp;
  }
  h.compression = cache->read_long_le(f);
  if (h.compression != 0) {
    error("loadBMP: compression not supported");
    goto errbmp;
  }
  h.image_size = cache->read_long_le(f);
  fseek(f, 0L, SEEK_END);
  h.file_size = ftell(f);
  if (h.image_size == 0) {
    h.image_size = h.file_size - h.data_offset;
  }

  //echo("loadBMP: w=%d h=%d c=%d s=%d", h.width, h.height, h.bit_count, h.image_size);

  Img *img = new Img(h.width, h.height, Img::RGB);

  // we read the data
  fseek(f, h.data_offset, 0);
  fread(reinterpret_cast<char *>(img->pixmap), 1, h.image_size, f);

  if (cache) {
    cache->close();
    delete cache;
  }

  //Inverse R et B
  for (int i=0; i < h.width*h.height ; i++) {
    uint8_t t = img->pixmap[i*3];
    img->pixmap[i*3] = img->pixmap[i*3+2];
    img->pixmap[i*3+2] = t;
  }

  return img;
}
