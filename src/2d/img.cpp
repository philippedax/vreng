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
// img.cpp
//
// Image Pixmap handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "img.hpp"
#include "texture.hpp"	// Texture

const uint16_t Img::SIZE = 256;


void Img::defaults()
{
  new_image++;
  nummipmaps = 0;
  pixmap = NULL;
}

/** Img constructor */
Img::Img(uint16_t w, uint16_t h, uint8_t b)
{
  defaults();
  width = w;
  height = h;
  bpp = b;
  if ((pixmap = new GLubyte[w * h * b]) == NULL) {
    error("Img: can't new pixmap"); return;
  }
}

Img::Img(uint16_t w, uint16_t h, uint8_t b, int32_t _nummipmaps, uint32_t size)
{
  defaults();
  width = w;
  height = h;
  bpp = b;
  nummipmaps = _nummipmaps;
  if ((pixmap = new GLubyte[size]) == NULL) {
    error("Img: can't new pixmap"); return;
  }
}

/** Img destructor */
Img::~Img()
{
  del_image++;
  if (pixmap) {
    delete[] pixmap;	// segfault FIXME!!!
    pixmap = NULL;
  }
}

/** Creates img */
Img * Img::init()
{
  uint8_t bpp = RGBA;
  Img * def_img = new Img(SIZE, SIZE, RGBA);	//dax *2 avoids segfault !

  for (int i=0; i < SIZE*SIZE; i++) {
    def_img->pixmap[bpp*i+0] = def_img->pixmap[bpp*i+1] = def_img->pixmap[bpp*i+2] = 0x80; //grey
    def_img->pixmap[bpp*i+3] = 0xff;	// a=1
  }
  return def_img;
}

/** Checks if image is well sized */
bool Img::wellsized()
{
  if ( (width != SIZE   || height != SIZE)   &&
       (width != SIZE/2 || height != SIZE/2) &&
       (width != SIZE/4 || height != SIZE/4) &&
       (width != SIZE/8 || height != SIZE/8)
     ) return false;
  return true;
}

/** bilinear interpolation with xf, yf normalized to 2^16 */
static inline
int interpol(int v00, int v01, int v10, int v11, int xf, int yf)
{
  int v0 = v00 + (((v01-v00) * xf) >> NORM8_BITS);
  int v1 = v10 + (((v11-v10) * xf) >> NORM8_BITS);

  return (v0 + (((v1-v0) * yf) >> NORM8_BITS));
}

/**
 * Resize image
 * called by updateTextures
 * TODO: more accurate resampling
 */
Img * Img::resize(uint16_t width_new, uint16_t height_new)
{
  if (bpp != RGB && bpp != BW && bpp != RGBA) {
    error("resize invalid bpp: f=%d w=%d h=%d", bpp, width, height);
    return NULL;
  }

  Img *img_new = new Img(width_new, height_new, bpp);
  GLubyte *pix_new = (GLubyte *) img_new->pixmap;

  uint16_t x1incr = ((width - 1)  * NORM_8) / (width_new - 1);
  uint16_t y1incr = ((height - 1) * NORM_8) / (height_new - 1);

  int x, y, xnew, ynew;

  for (ynew=0, y=0; y < height_new; y++) {
    int xi, yi, xf, yf;
    for (xnew=0, x=0; x < width_new; x++) {
      xi = xnew >> (NORM16_BITS - NORM8_BITS);
      yi = ynew >> (NORM16_BITS - NORM8_BITS);
      xf = xnew & ((1 << (NORM16_BITS - NORM8_BITS))-1);
      yf = ynew & ((1 << (NORM16_BITS - NORM8_BITS))-1);

      if (xi < (width-1) && yi < (height-1)) {
        for (int k=0; k<bpp; k++) {
          //DAX segfault
          pix_new[k] = interpol(pixmap[(yi     * width+xi)   * bpp +k],
                                pixmap[(yi     * width+xi+1) * bpp +k],
                                pixmap[((yi+1) * width+xi)   * bpp +k],
                                pixmap[((yi+1) * width+xi+1) * bpp +k],
                                xf, yf);
        }
      }
      else {
        for (int k=0; k<bpp; k++) {
          pix_new[k] = pixmap[(yi * width + xi) * bpp +k];		// segfault FIXME!
        }
      }
      pix_new += bpp;
      xnew += x1incr;
    }
    ynew += y1incr;
  }
  return img_new;
}
