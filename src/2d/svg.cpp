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
#include "texture.hpp"	// Texture

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg.h>
#include <nanosvgrast.h>


/** Load a SVG image */
Img * Img::loadSVG(void *_tex, ImageReader read_func)
{
  // downloads the svg file and put it into the cache
  Texture *tex = (Texture *) _tex;
  char cachepath[PATH_LEN] = {0};

  Cache::setCachePath(tex->url, cachepath);
  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(cachepath, tex->http)) == NULL) {
    delete cache;
    return NULL;
  }
  cache->close();
  delete cache;

  NSVGimage *image = NULL;
  NSVGrasterizer *rast = NULL;
  int w, h;

  // opens the svg file
  image = nsvgParseFromFile(cachepath, "px", 96);
  if (image == NULL) {
    error("could not open SVG image");
    nsvgDelete(image);
    return NULL;
  }
  w = (int)image->width;
  h = (int)image->height;
  trace(DBG_2D, "loadSVG: w=%d h=%d", w, h);

  // allocs img
  Img *img = new Img(w, h, 4);

  // fills the pixmap
  rast = nsvgCreateRasterizer();
  if (rast == NULL) {
    error("could not init rasterizer");
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);
    return NULL;
  }

  echo("rasterizing image %d x %d", w, h);
  nsvgRasterize(rast, image, 0,0,1, img->pixmap, w, h, w*4);

  nsvgDeleteRasterizer(rast);
  nsvgDelete(image);

  return img;
}
