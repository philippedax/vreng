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
#include "reader.hpp"
#include "cache.hpp"	// open
#include "file.hpp"	// open
#include "texture.hpp"	// Texture


// used by bmp, tif

Reader::Reader(void *tex, ImageReader _read_func)
{
  img_handle = tex;
  read_func = _read_func;
}

uint32_t Reader::getUInt(FILE *f)
{
  int c1, c2, c3, c4;

  c1 = getc(f);  c2 = getc(f);  c3 = getc(f);  c4 = getc(f);
  return ((uint32_t) c1) +
         (((uint32_t) c2) << 8) +
         (((uint32_t) c3) << 16) +
         (((uint32_t) c4) << 24);
}

int16_t Reader::getShort(FILE *f)
{
  int c1 = getc(f);
  int c2 = getc(f);
  return ((int16_t) c1) + (((int16_t) c2) << 8);
}

char * Reader::getFilename(void *_tex)
{
  static char filepath[PATH_LEN] = {0};
  Texture *tex = (Texture *) _tex;

  Cache::setCachePath(tex->url, filepath);
  return filepath;
}

#if 0 //notused

FILE * Reader::getFileCache(void *_tex, bool flagclose)
{
  Texture *tex = (Texture *) _tex;

  FILE *fp = getFileCache(tex);
  if (flagclose) {
    File::closeFile(fp);	// file not opened, libtiff does the job
  }
  return fp;
}

/* Opens a file in the cache */
FILE * Reader::getFileCache(Texture *tex)
{
  Cache *cache = new Cache();
  FILE *fp = cache->open(tex->url, tex->http);
  if (! fp) {
    error("Reader: can't open %s", tex->url);
    delete cache;
    return NULL;
  }
  return fp;
}

/* Opens a file given by its url and puts it in the cache - notused */
FILE * Reader::getFileCache(const char *url, char *filepath)
{
  FILE *fpi = NULL, *fpo = NULL;

  File *filein = new File();
  File *fileout = new File();
  if ((fpi = filein->open(filepath, "rb")) == NULL) {
    if ((fpo = fileout->open(filepath, "wb")) == NULL) {
      error("getFileCache: can't create %s", filepath);
      delete fileout;
      return NULL;
    }
    int len;
    char buf[BUFSIZ];
    while ((len = this->read_func(img_handle, buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, len, fpo);
    }
    fileout->close();
    delete fileout;
    if ((fpi = filein->open(filepath, "rb")) == NULL) {
      return NULL;
    }
  }
  return fpi;
}

uint8_t Reader::getChar()
{
  uint8_t data;

  if (this->read_func(img_handle, (char *) &data, 1) < 1) {
    data = 0;
  }
  ch = data;
  return data;
}

uint8_t Reader::getChar(FILE *f)
{
  ch = getc(f);
  return ch;
}

uint8_t Reader::getChar1()
{
  getChar();
  while (ch == '#') {
    do {
      getChar();
    } while (ch != 0 && ch != '\n' && ch != '\r');
    getChar();
  }
  return ch;
}

uint8_t Reader::getChar1(FILE *f)
{
  getChar(f);
  while (ch == '#') {
    do {
      getChar(f);
    } while (ch != 0 && ch != '\n' && ch != '\r');
    getChar(f);
  }
  return ch;
}

int32_t Reader::getInt()
{
  int n = 0;

  if (! isdigit(ch)) return -1; // returns -1 if error
  do {
    n = n*10 + ch-'0';
    getChar1();
  } while (isdigit(ch));
  return n;
}

int32_t Reader::getInt(FILE *f)
{
  int n = 0;

  if (!isdigit(ch)) return -1; // returns -1 if error
  do {
    n = n*10 + ch-'0';
    getChar1(f);
  } while (isdigit(ch));
  return n;
}

static uint8_t buf[BUFSIZ];
static int32_t pos = 0;
static int32_t len = 0;

void Reader::reset()
{
  pos = len = 0;
}

uint8_t Reader::getByte(void *texhdl, ImageReader read_func)
{
  if (pos >= len) {
    pos = 0;
    len = read_func(texhdl, (char *) buf, sizeof(buf));
  }
  return buf[pos++];
}

void Reader::skipSpaces()
{
  while (isspace(ch)) getChar1();
}

void Reader::skipSpaces(FILE *f)
{
  while (isspace(ch)) getChar1(f);
}

#endif // not-used
