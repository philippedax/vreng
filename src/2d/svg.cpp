//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2023 Philippe Dax
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

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvg.h>
#include <nanosvgrast.h>


/** Load a SVG image */
Img * Img::loadSVG(void *_tex, ImageReader read_func)
{
  // downloads the svg file and put it into the cache
  Texture *tex = (Texture *) _tex;
  char filename[PATH_LEN] = {0};

  Cache::setCachePath(tex->url, filename);
  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(filename, tex->http)) == NULL) {
    delete cache;
    return NULL;
  }
  cache->close();
  delete cache;

  NSVGimage *svgimage = NULL;
  NSVGrasterizer *rast = NULL;
  int width, height;

  // opens the svg file
  svgimage = nsvgParseFromFile(filename, "px", 96);
  if (svgimage == NULL) {
    error("could not open SVG image %s", filename);
    nsvgDelete(svgimage);
    return NULL;
  }
  width = (int)svgimage->width;
  height = (int)svgimage->height;
  trace(DBG_2D, "loadSVG: w=%d h=%d", width, height);

  // allocs img
  Img *img = new Img(width, height, Img::RGB);

  // fills the pixmap
  rast = nsvgCreateRasterizer();
  if (rast == NULL) {
    error("could not init rasterizer");
    nsvgDeleteRasterizer(rast);
    nsvgDelete(svgimage);
    return NULL;
  }

  //echo("rasterizing svgimage %d x %d", width, height);
  nsvgRasterize(rast, svgimage, 0,0,1, img->pixmap, width, height, width*Img::RGB);

  nsvgDeleteRasterizer(rast);
  nsvgDelete(svgimage);

  return img;
}
