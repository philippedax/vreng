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
#include "texture.hpp"	// Texture

const uint16_t Img::SIDE_SIZE = 256;


void Img::defaults()
{
  new_image++;
  nummipmaps = 0;
  anim = false;
  pixmap = NULL;
}

/** Img constructor */
Img::Img(uint16_t _width, uint16_t _height, uint8_t _channel)
{
  defaults();
  width = _width;
  height = _height;
  channel = _channel;
  if ((pixmap = new GLubyte[_width * _height * _channel]) == NULL) {
    error("Img: can't new pixmap"); return;
  }
}

Img::Img(uint16_t _width, uint16_t _height, uint8_t _channel, int32_t _nummipmaps, uint32_t size)
{
  defaults();
  width = _width;
  height = _height;
  channel = _channel;
  nummipmaps = _nummipmaps;
  if ((pixmap = new GLubyte[size]) == NULL) {
    error("Img: can't new pixmap"); return;
  }
}

/** Img destructor */
Img::~Img()
{
  del_image++;
  delete[] pixmap;
  pixmap = NULL;
}

Img * Img::init()
{
  uint8_t channel = RGB;
  Img * default_img = new Img(SIDE_SIZE, SIDE_SIZE, RGB);

  GLubyte *pixmap = default_img->pixmap;
  for (int i=0; i < SIDE_SIZE*SIDE_SIZE; i++) {
    pixmap[channel*i+0] = pixmap[channel*i+1] = pixmap[channel*i+2] = 0x80; //grey
#if 0   // RGBA
    pixmap[channel*i+3] = 0xff;
#endif
  }
  return default_img;
}


bool Img::wellSized()
{
  if ( (width != SIDE_SIZE   || height != SIDE_SIZE)   &&
       (width != SIDE_SIZE/2 || height != SIDE_SIZE/2) &&
       (width != SIDE_SIZE/4 || height != SIDE_SIZE/4) &&
       (width != SIDE_SIZE/8 || height != SIDE_SIZE/8)
     ) return false;
  return true;
}

/* Resampling */

/**
 * bilinear interpolation with xf, yf normalized to 2^16
 */
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
  if (channel != RGB &&
      channel != BW &&
      channel != RGBA) {
        error("resize invalid channel: f=%d w=%d h=%d", channel, width, height);
        return NULL;
  }
  if (pixmap == NULL) {
    warning("resize pixmap null");
    return NULL;
  }

  Img *img_new = new Img(width_new, height_new, channel);
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
        for (int k=0; k<channel; k++) {
#if 0 //DAX segfault
          pix_new[k] = interpol(pixmap[(yi     * width+xi)   * channel+k],
                                pixmap[(yi     * width+xi+1) * channel+k],
                                pixmap[((yi+1) * width+xi)   * channel+k],
                                pixmap[((yi+1) * width+xi+1) * channel+k],
                                xf, yf);
#else
	  ;
#endif
        }
      }
      else {
        for (int k=0; k<channel; k++) {
          pix_new[k] = pixmap[(yi * width + xi) * channel+k];
        }
      }
      pix_new += channel;
      xnew += x1incr;
    }
    ynew += y1incr;
  }
  return img_new;
}
