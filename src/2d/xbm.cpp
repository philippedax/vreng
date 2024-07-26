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
// xbm.cpp
//
// XBM Loader (X BitMap)
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, close
#include "texture.hpp"	// Texture


static int hexval(char ch)
{
  static char hex[] = "0123456789ABCDEFabcdef";
  int val = 0;

  char *p = strchr(hex, ch);
  val = p-hex;
  val = (val>15)?val-6:val;
  return val;
}

Img * Img::loadXBM(void *_tex, ImageReader read_func)
{
  int x = 0;
  uint16_t width = 0, height = 0;
  uint8_t colorfg = 0;	// black
  uint8_t colorbg = 255;// white
  char line[256];
  
  Texture *tex = static_cast<Texture *>(_tex);

  Cache *cache = new Cache();
  FILE *f = cache->open(tex->url, tex->http);
  if (! f) {
    error("can't read %s", tex->url);
    delete cache;
    return NULL;
  }

  Img *img = NULL;

  /* Read the XBM image file header information */
  while (fgets(line, 255, f)) {
    char *p;
    if (strlen(line) < 5) continue; /* ignore junk lines */
    if (line[0] == '#') {
      if ((p = strstr(line, "width"))) {
        width = atoi(p+5);
      }
      else if ((p = strstr(line, "height"))) {
        height = atoi(p+6);
      }
    }
    else if ((p = strstr(line, "static"))) {
      img = new Img(width, height, Img::RGB);
    }

    /* Read the XBM image file data */
    else { /* find  0xhh, sets of 8 bits */
      for (int i=0; i < strlen(line)-4; i++) {
        if (line[i] == '{') continue;
        if (line[i] == ' ') continue;
        if (line[i] == '0' && (line[i+1] == 'x' || line[i+1] == 'X')) {
          char h1, h2, ch;
          h1 = hexval(line[i+2]);
          h2 = hexval(line[i+3]);
          ch = h1*16+h2;
          for (int k=0; k<8; k++) {
            img->pixmap[x*3+0] = colorbg;
            img->pixmap[x*3+1] = colorbg;
            img->pixmap[x*3+2] = colorbg;
            if (ch & (1<<k)) {
              img->pixmap[x*3+0] = colorfg;
              img->pixmap[x*3+1] = colorfg;
              img->pixmap[x*3+2] = colorfg;
            }
            x++;
          }
          i = i+4; /* also skip comma */
        }
      }
    }  
  }
  cache->close();
  delete cache;
  return img;
}
