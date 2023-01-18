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
/* GiMd2Viewer - Quake2 model viewer
 * Copyright (C) 1998  Lionel ULMER <bbrox@mygale.org>
 */
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, close
#include "texture.hpp"	// Texture


typedef struct {
  uint8_t Manufacturer;
  uint8_t Version;
  uint8_t Encoding;
  uint8_t BitsPerPixel;
  uint8_t Xmin_l; uint8_t Xmin_h;
  uint8_t Ymin_l; uint8_t Ymin_h;
  uint8_t Xmax_l; uint8_t Xmax_h;
  uint8_t Ymax_l; uint8_t Ymax_h;
  uint8_t HDPI_l; uint8_t HDPI_h;
  uint8_t VDPI_l; uint8_t VDPI_h;
  uint8_t Colormap[48];
  uint8_t Reserved;
  uint8_t NPlanes;
  uint8_t BytesPerLine_l; uint8_t BytesPerLine_h;
  uint8_t PaletteInfo_l; uint8_t PaletteInfo_h;
  uint8_t HScreenSize_l; uint8_t HScreeSize_h;
  uint8_t VScreenSize_l; uint8_t VScreeSize_h;
  uint8_t Filler[54];
} PcxInfo;


Img * Img::loadPCX(void *tex, ImageReader read_func)
{
  PcxInfo pcxInfo;
  struct { uint8_t r, g, b; } palette[256];

  Texture *texture = (Texture *) tex;

  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(texture->url, texture->http)) == NULL) return NULL;

  /* loads the Header */
  fread((char *) &pcxInfo, 1, sizeof(pcxInfo), f);

  /* test if this is really a PCX file */
  if (pcxInfo.Manufacturer != 0x0A) {
    error("loadPCX: not a PCX format: %x", pcxInfo.Manufacturer);
    cache->close();
    delete cache;
    return NULL;
  }

  /* only decode one type of PCX */
  if ((pcxInfo.Version != 0x05) || (pcxInfo.BitsPerPixel != 8) || (pcxInfo.NPlanes != 0x01))
    cache->close();
    delete cache;
    return NULL;

  /* gets the image dimensions */
  uint16_t width = ((pcxInfo.Xmax_l | (pcxInfo.Xmax_h << 8)) - (pcxInfo.Xmin_l | (pcxInfo.Xmin_h << 8))) + 1;
  uint16_t height = ((pcxInfo.Ymax_l | (pcxInfo.Ymax_h << 8)) - (pcxInfo.Ymin_l | (pcxInfo.Ymin_h << 8))) + 1;
  uint16_t bpl = (pcxInfo.BytesPerLine_l | (pcxInfo.BytesPerLine_h << 8));

  /* temporary indexed buffer */
  uint8_t *pix = new uint8_t[width * height];

  /* start decoding the file */
  uint8_t *tmp = pix;
  for (int y=0; y < height; y++) {
    int count = 0;
    uint8_t data = 0x00;

    /* decodes one line */
    int x = 0;
    while (x < bpl) {
      if (count > 0) {
	if (x < width)
	  *(tmp++) = data;
	x++;
	count--;
      } else {
	data = getc(f);
	if ((data & 0xC0) == 0xC0) {
	  count = data & 0x3F;
	  data = getc(f);
	} else {
	  count = 1;
	}
      }
    }
  }

  /* decodes the palette */
  if (getc(f) != 0x0C) {
    delete[] pix;
    cache->close();
    delete cache;
    return NULL;
  }
  for (int i=0; i < 256; i++) {
    palette[i].r = getc(f);
    palette[i].g = getc(f);
    palette[i].b = getc(f);
  }

  /* allocates image */
  Img *img = new Img(width, height, Img::RGB);

  /* converts the indexed buffer to RGB */
  tmp = pix;
  uint8_t *pixmap = img->pixmap;
  for (int x=0; x < width * height; x++) {
    *(pixmap++) = palette[*tmp].r;
    *(pixmap++) = palette[*tmp].g;
    *(pixmap++) = palette[*tmp].b;
    tmp++;
  }
  delete[] pix;
  cache->close();
  delete cache;

  return img;
}
