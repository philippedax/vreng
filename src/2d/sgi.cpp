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
#include "file.hpp"	// littleEndian, convertLong
#include "texture.hpp"	// Texture


typedef struct {
  uint16_t imagic;
  uint16_t type;
  uint16_t dim;
  uint16_t width, height, channel;
  uint32_t min, max;
  uint32_t wasteBytes;
  char name[80];
  uint32_t colorMap;
  FILE *f;
  uint8_t *tmp, *tmpR, *tmpG, *tmpB;
  uint32_t rleEnd;
  uint32_t *rowStart;
  uint32_t *rowSize;
} SgiInfo;


static
void bwtorgb(uint8_t *b, uint8_t *l, int n)
{
  while (n--) {
    l[0] = *b; l[1] = *b; l[2] = *b;
    l += 3; b++;
  }
}

static
void latorgb(uint8_t *b, uint8_t *a, uint8_t *l, int n)
{
  while (n--) {
    l[0] = *b; l[1] = *b; l[2] = *b;
    l += 3; b++; a++;
  }
}

static
void rgbtorgb(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *l, int n)
{
  while (n--) {
    l[0] = r[0]; l[1] = g[0]; l[2] = b[0];
    l += 3; r++; g++; b++;
  }
}

static
void rgbatorgb(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a, uint8_t *l, int n)
{
  while (n--) {
    l[0] = r[0]; l[1] = g[0]; l[2] = b[0];
    l += 3; r++; g++; b++; a++;
  }
}

static
void getRow(SgiInfo *sgi, uint8_t *buf, int y, int z)
{
  if ((sgi->type & 0xFF00) == 0x0100) { // storage = RLE (run length encoding)
    fseek(sgi->f, (uint32_t) sgi->rowStart[y + z*sgi->height], SEEK_SET);
    fread(sgi->tmp, 1, (uint32_t) sgi->rowSize[y + z*sgi->height], sgi->f);

    uint8_t *iPtr = sgi->tmp;
    uint8_t *oPtr = buf;
    for (;;) {
      uint8_t pixel = *iPtr++;
      int count = (int)(pixel & 0x7F);
      if (!count)
	return;
      if (pixel & 0x80) {
	while (count--) *oPtr++ = *iPtr++;
      }
      else {
	pixel = *iPtr++;
	while (count--) *oPtr++ = pixel;
      }
    }
  }
  else {	// storage = VERBATIM
    fseek(sgi->f, 512 + (y*sgi->width) + (z*sgi->width*sgi->height), SEEK_SET);
    fread(buf, 1, sgi->width, sgi->f);
  }
}

/** SGI loader */
Img * Img::loadSGI(void *tex, ImageReader read_func)
{
  SgiInfo *sgi = new SgiInfo[1];
  if (! sgi) return NULL;

  Texture *texture = (Texture *) tex;
  Cache *cache = new Cache();
  if ((sgi->f = cache->open(texture->url, texture->http)) == NULL) return NULL;

  fread(sgi, 1, 12, sgi->f); // header

  if (File::littleEndian())
    File::convertShort(&sgi->imagic, 6);
  sgi->tmp  = new uint8_t[sgi->width*256];
  if (!sgi->tmp) return NULL;

  if ((sgi->type & 0xFF00) == 0x0100) {	// storage = RLE
    int x = sgi->height * sgi->channel * sizeof(uint32_t);
    sgi->rowStart = new uint32_t[x];
    sgi->rowSize  = new uint32_t[x];
    if (!sgi->rowStart || !sgi->rowSize) return NULL;
    sgi->rleEnd = 512 + (2 * x);
    fseek(sgi->f, 512, SEEK_SET);
    fread(sgi->rowStart, 1, x, sgi->f);
    fread(sgi->rowSize, 1, x, sgi->f);
    if (File::littleEndian()) {
      File::convertLong(sgi->rowStart, x/(int)sizeof(uint32_t));
      File::convertLong((uint32_t *)sgi->rowSize, x/(int)sizeof(uint32_t));
    }
  }

  trace(DBG_IMG, "sgi: w=%d h=%d c=%d", sgi->width, sgi->height, sgi->channel);

  /* alloc the data */
  Img *img = new Img(sgi->width, sgi->height, Img::RGB);

  uint8_t *rbuf = new uint8_t[sgi->width];
  uint8_t *gbuf = new uint8_t[sgi->width];
  uint8_t *bbuf = new uint8_t[sgi->width];
  uint8_t *abuf = new uint8_t[sgi->width];
  if (!img->pixmap || !rbuf || !gbuf || !bbuf || !abuf) return NULL;

  /* read the data */
  uint8_t *lptr = img->pixmap;
  for (int y=0; y < sgi->height; y++) {
    if (sgi->channel == 4) {	// RGBA
      getRow(sgi, rbuf, y, 0);
      getRow(sgi, gbuf, y, 1);
      getRow(sgi, bbuf, y, 2);
      getRow(sgi, abuf, y, 3);
      rgbatorgb(rbuf, gbuf, bbuf, abuf, lptr, sgi->width);
    }
    else if (sgi->channel == 3) {	// RGB
      getRow(sgi, rbuf, y, 0);
      getRow(sgi, gbuf, y, 1);
      getRow(sgi, bbuf, y, 2);
      rgbtorgb(rbuf, gbuf, bbuf, lptr, sgi->width);
    }
    else if (sgi->channel == 2) {	// LA
      getRow(sgi, rbuf, y, 0);
      getRow(sgi, abuf, y, 1);
      latorgb(rbuf, abuf, lptr, sgi->width);
    }
    else {			// BW
      getRow(sgi, rbuf, y, 0);
      bwtorgb(rbuf, lptr, sgi->width);
    }
    lptr += (sgi->width * Img::RGB);
  }

  cache->close();
  delete cache;
  delete[] sgi->tmp;
  delete[] sgi;
  delete[] rbuf;
  delete[] gbuf;
  delete[] bbuf;
  delete[] abuf;

  return img;
}
