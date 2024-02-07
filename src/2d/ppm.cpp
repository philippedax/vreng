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
#include "file.hpp"	// open, close
#include "texture.hpp"	// Texture


Img * Img::loadPPM(void *_tex, ImageReader read_func)
{
  Texture *tex = static_cast<Texture *>(_tex);

  Cache *cache = new Cache();
  FILE *f;
  if ((f = cache->open(tex->url, tex->http)) == NULL) return NULL;

  /* we read the header */
  int width, height;
  char magic[2];
  char *p, buf[BUFSIZ];
  
  magic[0] = getc(f);
  magic[1] = getc(f); 
  if (magic[0] != 'P' || magic[1] != '6') return NULL;
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
    fread((char *)img->pixmap + y*width*Img::RGB, 1, width*Img::RGB, f);
  }

  cache->close();
  delete cache;
  return img;
}

/* save file in PPM */
void Img::savePPM(const char *filename, GLenum mode)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  int width = viewport[2]; if (width & 1) ++width;	//even values
  int height = viewport[3]; if (height & 1) ++height;

  File *file = new File();
  FILE *f;
  if ((f = file->open(filename, "wb")) == NULL) {
    perror("open"); return;
  }
  fprintf(f, "P6\n#\n%d %d\n255\n", width, height);	// magic number P6

  glReadBuffer(mode);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  uint8_t *pixmap = new uint8_t[Img::RGB * width * height];
  uint8_t *ppix = pixmap + (Img::RGB * width * (height-1));

  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixmap);

  for (int j=height; j-- ; ppix -= Img::RGB * width) {
    fwrite(ppix, 1, Img::RGB * width, f);
  }

  delete[] pixmap;
  file->close();
  delete file;
}
