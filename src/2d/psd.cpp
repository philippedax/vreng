//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
// Photoshop PSD loader -- PD by Thatcher Ulrich, integration by Nicholas Schulz, tweaked by STB
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, close
#include "texture.hpp"	// Texture


typedef struct {
   uint32_t img_x, img_y;
   int img_n, img_out_n;
   uint8_t *img_buffer, *img_buffer_end;
   FILE *img_file;
} stbi;


static int get8(stbi *s)
{
  if (s->img_file) {
    int c = fgetc(s->img_file);
    return c == EOF ? 0 : c;
  }
  if (s->img_buffer < s->img_buffer_end)
    return *s->img_buffer++;
  return 0;
}

static void skip(stbi *s, int n)
{
  if (s->img_file) fseek(s->img_file, n, SEEK_CUR);
}

static int get16(stbi *s)
{
  int z = get8(s);
  return (z << 8) + get8(s);
}

static uint32_t get32(stbi *s)
{
  uint32_t z = get16(s);
  return (z << 16) + get16(s);
}

static uint8_t compute_y(int r, int g, int b)
{
  return (uint8_t) (((r*77) + (g*150) + (29*b)) >> 8);
}

// Generic converter from built-in comp to dstcomp
static uint8_t *convert_format(uint8_t *data, uint8_t srccomp, uint8_t dstcomp, uint16_t x, uint16_t y)
{
  if (dstcomp == srccomp) return data;

  uint8_t *good = new uint8_t[dstcomp * x * y];

  for (int j=0; j < y; j++) {
    uint8_t *src = data + j * x * srccomp;
    uint8_t *dst = good + j * x * dstcomp;
    #define COMBO(a,b) ((a)*8+(b))
    #define CASE(a,b)  case COMBO(a,b): for (int i=x-1; i >= 0; --i, src += a, dst += b)
    // convert source image with srccomp bpp to one with dstcomp bpp;
    // avoid switch per pixel, so use switch per scanline and massive macros
    switch (COMBO(srccomp, dstcomp)) {
      CASE(1,2) dst[0]=src[0], dst[1]=255;
      break;
      CASE(1,3) dst[0]=dst[1]=dst[2]=src[0];
      break;
      CASE(1,4) dst[0]=dst[1]=dst[2]=src[0], dst[3]=255;
      break;
      CASE(2,1) dst[0]=src[0];
      break;
      CASE(2,3) dst[0]=dst[1]=dst[2]=src[0];
      break;
      CASE(2,4) dst[0]=dst[1]=dst[2]=src[0], dst[3]=src[1];
      break;
      CASE(3,4) dst[0]=src[0],dst[1]=src[1],dst[2]=src[2],dst[3]=255;
      break;
      CASE(3,1) dst[0]=compute_y(src[0],src[1],src[2]);
      break;
      CASE(3,2) dst[0]=compute_y(src[0],src[1],src[2]), dst[1]=255;
      break;
      CASE(4,1) dst[0]=compute_y(src[0],src[1],src[2]);
      break;
      CASE(4,2) dst[0]=compute_y(src[0],src[1],src[2]), dst[1]=src[3];
      break;
      CASE(4,3) dst[0]=src[0],dst[1]=src[1],dst[2]=src[2];
      break;
    }
    #undef CASE
  }
  if (data) delete[] data;
  return good;
}

static uint8_t *psd_load(stbi *s, uint16_t *x, uint16_t *y, uint8_t *srccomp, uint8_t dstcomp)
{
  uint16_t nbpixels;
  uint8_t  nbbpps;
  uint8_t  compression, bpp;
  uint16_t count, len;
  uint16_t w, h;
  uint8_t  *data = NULL;

  // Check identifier
  if (get32(s) != 0x38425053) {	// "8BPS"
    error("invalid PSD image");
    return NULL;
  }
  // Check file type version.
  if (get16(s) != 1) {
    error("unsupported version of PSD image");
    return NULL;
  }
  // Skip 6 reserved bytes.
  skip(s, 6);
  // Read the number of bpps (R, G, B, A).
  nbbpps = get16(s);
  if (nbbpps > 16) {
    error("unsupported number of bpps in PSD image");
    return NULL;
  }
  // Read the rows and columns of the image.
  h = get32(s);
  w = get32(s);
  // Make sure the depth is 8 bits.
  if (get16(s) != 8) {
    error("PSD bit depth is not 8 bit");
    return NULL;
  }
  // Make sure the color mode is RGB.
  //   0: Bitmap
  //   1: Grayscale
  //   2: Indexed color
  //   3: RGB color
  //   4: CMYK color
  //   7: Multibpp
  //   8: Duotone
  //   9: Lab color
  if (get16(s) != 3) {
    error("PSD is not in RGB color format");
    return NULL;
  }
  skip(s, get32(s));	// Skip the Mode Data. (It's the palette for indexed color)
  skip(s, get32(s));	// Skip the image resources. (resolution, pen tool paths, etc)
  skip(s, get32(s));	// Skip the reserved data.
  // Find out if the data is compressed.
  // Known values: 0: no compression 1: RLE compressed
  compression = get16(s);
  if (compression > 1) {
    error("PSD has an unknown compression format");
    return NULL;
  }

  // Create the destination image.
  data = new uint8_t[4 * w*h];
  if (!data) {
    error("PSD out of memory");
    return NULL;
  }
  nbpixels = w * h;

  // Finally, the image data.
  if (compression) {
    // RLE as used by .PSD and .TIFF
    // Loop until you get the number of unpacked bytes you are expecting:
    //     Read the next source byte into n.
    //     If n is between 0 and 127 inclusive, copy the next n+1 bytes literally.
    //     Else if n is between -127 and -1 inclusive, copy the next byte -n+1 times.
    //     Else if n is 128, noop.
    // Endloop
    // The RLE-compressed data is preceeded by a 2-byte data count for each row in the data,
    // which we're going to just skip.
    skip(s, h * nbbpps * 2);

    // Read the RLE data by bpp.
    for (bpp=0; bpp < 4; bpp++) {
      uint8_t *p = data + bpp;
      if (bpp >= nbbpps) {
        // Fill this bpp with default data.
        for (int i=0; i < nbpixels; i++) {
          *p = (bpp == 3 ? 255 : 0), p += 4;
        }
      }
      else {
        // Read the RLE data.
        count = 0;
        while (count < nbpixels) {
          len = get8(s);
          if (len == 128) {
            ; // No-op.
          }
          else if (len < 128) {
            // Copy next len+1 bytes literally.
            len++;
            count += len;
            while (len) {
              *p = get8(s);
              p += 4;
              len--;
            }
          }
          else if (len > 128) {
            uint32_t  val;
            // Next -len+1 bytes in the dest are replicated from next source byte.
            // (Interpret len as a negative 8-bit int.)
            len ^= 0x0FF;
            len += 2;
            val = get8(s);
            count += len;
            while (len) {
              *p = val;
              p += 4;
              len--;
            }
          }
        }
      }
    }
  } //end compression
  else {
    // We're at the raw image data.  It's each bpp in order (Red, Green, Blue, Alpha, ...)
    // where each bpp consists of an 8-bit value for each pixel in the image.

    // Read the data by bpp.
    for (bpp=0; bpp < 4; bpp++) {
      uint8_t *p = data + bpp;
      if (bpp > nbbpps) {
        // Fill this bpp with default data.
        for (int i=0; i < nbpixels; i++) *p = (bpp == 3) ? 255 : 0, p += 4;
      }
      else {
        // Read the data.
        count = 0;
        for (int i=0; i < nbpixels; i++) {
          *p = get8(s);
          p += 4;
        }
      }
    }
  }

  if (dstcomp && dstcomp != 4) {
    data = convert_format(data, 4, dstcomp, w, h);
    if (data == NULL) return NULL; // convert_format frees input on failure
  }
  if (srccomp) {
    *srccomp = nbbpps;
  }
  *y = h;
  *x = w;

  //echo("psd_load data=%x",data);
  return data;
}

Img * Img::loadPSD(void *_tex, ImageReader read_func)
{
  stbi s;
  uint16_t width, height;
  uint8_t bpp, dstcomp = Img::RGB;

  Texture *tex = static_cast<Texture *>(_tex);

  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(tex->url, tex->http)) == NULL) return NULL;

  s.img_file = f;

  static uint8_t *data = psd_load(&s, &width, &height, &bpp, dstcomp);
  Img *img = new Img(width, height, Img::RGB);
  img->pixmap = data;

  cache->close();
  delete cache;
  return img;
}
