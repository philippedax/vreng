//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
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
// glpng.cpp
//
// PNG loader library for OpenGL v1.45 (10/07/00)
// by Ben Wyatt ben@wyatt100.freeserve.co.uk
// Using LibPNG 1.0.2 and ZLib 1.1.3
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the author be held liable for any damages arising from the
// use of this software.
// 
// Permission is hereby granted to use, copy, modify, and distribute this
// source code, or portions hereof, for any purpose, without fee, subject to
// the following restrictions:
//  * 1. The origin of this source code must not be misrepresented. You must not
//    claim that you wrote the original software. If you use this software in
//    a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered versions must be plainly marked as such and must not be
//    misrepresented as being the original source.
// 3. This notice must not be removed or altered from any source distribution.
//---------------------------------------------------------------------------
#include <GL/gl.h>
#include <stdlib.h>
#include <math.h>
#include "glpng.hpp"

#include "png.h"	// src/ext/mypng


/** Used to decide if GL/gl.h supports the paletted extension */
#ifdef GL_COLOR_INDEX1_EXT
#define SUPPORTS_PALETTE_EXT
#endif

static unsigned char DefaultAlphaCallback(unsigned char red, unsigned char green, unsigned char blue) {
  return 255;
}

static unsigned char StencilRed = 0, StencilGreen = 0, StencilBlue = 0;
static unsigned char (*AlphaCallback)(unsigned char red, unsigned char green, unsigned char blue) = DefaultAlphaCallback;
static int StandardOrientation = 0;
static GLint MaxTextureSize = 0;

/* screenGamma = displayGamma/viewingGamma
 * displayGamma = CRT has gamma of ~2.2
 * viewingGamma depends on platform. PC is 1.0, Mac is 1.45, SGI defaults
 * to 1.7, but this can be checked and changed w/ /usr/sbin/gamma command.
 * If the environment variable VIEWING_GAMMA is set, adjust gamma per this value.
 */
#ifdef _MAC
  static double screenGamma = 2.2 / 1.45;
#elif SGI
  static double screenGamma = 2.2 / 1.7;
#else /* PC/default */
  static double screenGamma = 2.2 / 1.0;
#endif

static char gammaExplicit = 0;  /*if  */

static void checkForGammaEnv()
{
  double viewingGamma;
  char *gammaEnv = getenv("VIEWING_GAMMA");

  if (gammaEnv && !gammaExplicit) {
    sscanf(gammaEnv, "%lf", &viewingGamma);
    screenGamma = 2.2/viewingGamma;
  }
}

/** Returns a safe texture size to use (ie a power of 2), based on the current texture size "i" */
static int SafeSize(int i)
{
  if (i > MaxTextureSize) return MaxTextureSize;

  for (int p = 0; p < 24; p++) {
    if (i <= (1<<p))
      return 1<<p;
  }

  return MaxTextureSize;
}

/** Resize the texture since gluScaleImage doesn't work on everything */
static void Resize(int components, const png_bytep d1, int w1, int h1, png_bytep d2, int w2, int h2)
{
  const float sx = static_cast<float>(w1/w2), sy = static_cast<float>(h1/h2);
  int x, y, xx, yy, c;
  png_bytep d;

  for (y = 0; y < h2; y++) {
    yy = (int) (y*sy)*w1;
    for (x = 0; x < w2; x++) {
      xx = (int) (x*sx);
      d = d1 + (yy+xx)*components;
      for (c = 0; c < components; c++) {
        *d2++ = *d++;
      }
    }
  }
}

#define GET(o) ((int)*(data + (o)))

static int HalfSize(GLint components, GLint width, GLint height, const unsigned char *data, unsigned char *d, int filter)
{
  int line = width*components;

  if (width > 1 && height > 1) {
    if (filter) {
      for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
          for (int c = 0; c < components; c++) {
            *d++ = (GET(0)+GET(components)+GET(line)+GET(line+components)) / 4;
            data++;
          }
          data += components;
        }
        data += line;
      }
    }
    else {
      for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
          for (int c = 0; c < components; c++) {
            *d++ = GET(0);
            data++;
          }
          data += components;
        }
        data += line;
      }
    }
  }
  else if (width > 1 && height == 1) {
    if (filter) {
      for (int y = 0; y < height; y += 1) {
        for (int x = 0; x < width; x += 2) {
          for (int c = 0; c < components; c++) {
            *d++ = (GET(0)+GET(components)) / 2;
            data++;
          }
          data += components;
        }
      }
    }
    else {
      for (int y = 0; y < height; y += 1) {
        for (int x = 0; x < width; x += 2) {
          for (int c = 0; c < components; c++) {
            *d++ = GET(0);
            data++;
          }
          data += components;
        }
      }
    }
  }
  else if (width == 1 && height > 1) {
    if (filter) {
      for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 1) {
          for (int c = 0; c < components; c++) {
            *d++ = (GET(0)+GET(line)) / 2;
            data++;
          }
        }
        data += line;
      }
    }
    else {
      for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 1) {
          for (int c = 0; c < components; c++) {
            *d++ = GET(0);
            data++;
          }
        }
        data += line;
      }
    }
  }
  else {
    return 0;
  }

  return 1;
}
#undef GET

/** Replacement for gluBuild2DMipmaps so GLU isn't needed */
static void Build2DMipmaps(GLint components, GLint width, GLint height, GLenum format, const unsigned char *data, int filter)
{
  int level = 0;
  unsigned char *d = static_cast<unsigned char *>(malloc((width/2)*(height/2)*components+4));
  const unsigned char *last = data;

  glTexImage2D(GL_TEXTURE_2D, level, components, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  level++;

  while (HalfSize(components, width, height, last, d, filter)) {
    if (width  > 1) width  /= 2;
    if (height > 1) height /= 2;

    glTexImage2D(GL_TEXTURE_2D, level, components, width, height, 0, format, GL_UNSIGNED_BYTE, d);
    level++;
    last = d;
  }
  free(d);
}

int APIENTRY pngLoadF(FILE *fp, int mipmap, int trans, pngRawInfo *pinfo)
{
  GLint pack, unpack;
  unsigned char header[8];
  png_structp png;
  png_infop   info;
  png_infop   endinfo;
  png_bytep   data, data2;
  png_bytep   *row_p;
  double      fileGamma;

  png_uint_32 width, height, rw, rh;
  int depth, color;
  png_uint_32 i;

  fread(header, 1, 8, fp);
  //dax if (!png_check_sig(header, 8)) return 0;

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(png);
  endinfo = png_create_info_struct(png);

  png_init_io(png, fp);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);
  png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);

  if (pinfo) {
    pinfo->Width  = width;
    pinfo->Height = height;
    pinfo->Depth  = depth;
  }
  if (MaxTextureSize == 0)
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);

  if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);
  if (color&PNG_COLOR_MASK_ALPHA && trans != PNG_ALPHA) {
    png_set_strip_alpha(png);
    color &= ~PNG_COLOR_MASK_ALPHA;
  }

  /*--GAMMA--*/
  checkForGammaEnv();
  if (png_get_gAMA(png, info, &fileGamma))
    png_set_gamma(png, screenGamma, fileGamma);
  else
    png_set_gamma(png, screenGamma, 1.0/2.2);

  png_read_update_info(png, info);

  data = static_cast<png_bytep>(malloc(png_get_rowbytes(png, info)*height));
  row_p = static_cast<png_bytep *>(malloc(sizeof(png_bytep)*height));

  for (i = 0; i < height; i++) {
    if (StandardOrientation)
      row_p[height - 1 - i] = &data[png_get_rowbytes(png, info)*i];
    else
      row_p[i] = &data[png_get_rowbytes(png, info)*i];
  }

  png_read_image(png, row_p);
  free(row_p);

  rw = SafeSize(width), rh = SafeSize(height);

  if (rw != width || rh != height) {
    const int bpps = png_get_rowbytes(png, info)/width;

    data2 = static_cast<png_bytep>(malloc(rw*rh*bpps));

    Resize(bpps, data, width, height, data2, rw, rh);

    width = rw, height = rh;
    free(data);
    data = data2;
  }

  { /* OpenGL stuff */
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (trans == PNG_SOLID || trans == PNG_ALPHA || color == PNG_COLOR_TYPE_RGB_ALPHA || color == PNG_COLOR_TYPE_GRAY_ALPHA) {
      GLenum glformat;
      GLint glcomponent;

      switch (color) {
        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_RGB:
        case PNG_COLOR_TYPE_PALETTE:
          glformat = GL_RGB;
          glcomponent = 3;
          if (pinfo) pinfo->Alpha = 0;
          break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
        case PNG_COLOR_TYPE_RGB_ALPHA:
          glformat = GL_RGBA;
          glcomponent = 4;
          if (pinfo) pinfo->Alpha = 8;
          break;
        default:
          //error("glformat not set");
          return 0;
      }
      if (mipmap == PNG_BUILDMIPMAPS)
        Build2DMipmaps(glcomponent, width, height, glformat, data, 1);
      else if (mipmap == PNG_SIMPLEMIPMAPS)
        Build2DMipmaps(glcomponent, width, height, glformat, data, 0);
      else
        glTexImage2D(GL_TEXTURE_2D, mipmap, glcomponent, width, height, 0, glformat, GL_UNSIGNED_BYTE, data);
    }
    else {
      png_bytep p, endp, q;
      int r, g, b, a;

      p = data, endp = p+width*height*3;
      q = data2 = static_cast<png_bytep>(malloc(sizeof(png_byte)*width*height*4));

      if (pinfo) pinfo->Alpha = 8;

#define FORSTART \
  do { \
    r = *p++; /*red  */ \
    g = *p++; /*green*/ \
    b = *p++; /*blue */ \
    *q++ = r; \
    *q++ = g; \
    *q++ = b;
#define FOREND \
  q++; \
} while (p != endp);
#define ALPHA *q

      switch (trans) {
        case PNG_CALL_BACK:
          FORSTART
            ALPHA = AlphaCallback((unsigned char) r, (unsigned char) g, (unsigned char) b);
          FOREND
          break;
        case PNG_STENCIL:
          FORSTART
            if (r == StencilRed && g == StencilGreen && b == StencilBlue)
              ALPHA = 0;
            else
              ALPHA = 255;
          FOREND
          break;
        case PNG_BLEND1:
          FORSTART
            a = r+g+b;
            if (a > 255) ALPHA = 255; else ALPHA = a;
          FOREND
          break;
        case PNG_BLEND2:
          FORSTART
            a = r+g+b;
            if (a > 255*2) ALPHA = 255; else ALPHA = a/2;
          FOREND
          break;
        case PNG_BLEND3:
          FORSTART
            ALPHA = (r+g+b)/3;
          FOREND
          break;
        case PNG_BLEND4:
          FORSTART
            a = r*r+g*g+b*b;
            if (a > 255) ALPHA = 255; else ALPHA = a;
          FOREND
          break;
        case PNG_BLEND5:
          FORSTART
            a = r*r+g*g+b*b;
            if (a > 255*2) ALPHA = 255; else ALPHA = a/2;
          FOREND
          break;
        case PNG_BLEND6:
          FORSTART
            a = r*r+g*g+b*b;
            if (a > 255*3) ALPHA = 255; else ALPHA = a/3;
          FOREND
          break;
        case PNG_BLEND7:
          FORSTART
            a = r*r+g*g+b*b;
            if (a > 255*255) ALPHA = 255; else ALPHA = (int) sqrt(a);
          FOREND
          break;
      }
#undef FORSTART
#undef FOREND
#undef ALPHA
      if (mipmap == PNG_BUILDMIPMAPS)
        Build2DMipmaps(4, width, height, GL_RGBA, data2, 1);
      else if (mipmap == PNG_SIMPLEMIPMAPS)
        Build2DMipmaps(4, width, height, GL_RGBA, data2, 0);
      else
        glTexImage2D(GL_TEXTURE_2D, mipmap, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);

      free(data2);
    }
    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
  } /* OpenGL end */

  png_read_end(png, endinfo);
  png_destroy_read_struct(&png, &info, &endinfo);

  //dax free(data);
  pinfo->Data = data;
  return 1;
}
