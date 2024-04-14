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
// pgm.cpp
//
// PGM Loader
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, close
#include "texture.hpp"	// Texture


Img * Img::loadPGM(void *_tex, ImageReader read_func)
{
  Texture *tex = static_cast<Texture *>(_tex);

  Cache *cache = new Cache();
  FILE *f = cache->open(tex->url, tex->http);
  if (! f) {
    error("can't read %s", tex->url);
    delete cache;
    return NULL;
  }

  /* we read the header */
  int width, height;
  char magic[2];
  char *p, buf[BUFSIZ];

  magic[0] = getc(f);
  magic[1] = getc(f);
  if (magic[0] != 'P' || magic[1] != '5') {
    cache->close();
    delete cache;
    return NULL;
  }
  fgets(buf, 256, f);
  fgets(buf, 256, f);
  fgets(buf, 256, f);
  fgets(buf, 256, f);
  width = atoi(buf);
  p = strchr(buf, ' ');
  height = atoi(++p);
  fgets(buf, 256, f); // maxval

  Img *img = new Img(width, height, Img::RGB);

  /* we read the data */
  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {
      fread(reinterpret_cast<char *>(img->pixmap) + (y*width+x) * Img::RGB, 1, 1, f);
      // convert Gray to R = G = B
      img->pixmap[((y*width+x)*Img::RGB)+1] = img->pixmap[(y*width+x)*Img::RGB];
      img->pixmap[((y*width+x)*Img::RGB)+2] = img->pixmap[(y*width+x)*Img::RGB];
    }
  }
  cache->close();
  delete cache;

  return img;
}
