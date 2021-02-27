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
#include "cache.hpp"	// openCache
#include "file.hpp"	// openFile
#include "texture.hpp"	// Texture


Reader::Reader(void *tex, ImageReader _read_func)
{
  new_imgreader++;
  img_hdl = tex;
  read_func = _read_func;
}

FILE * Reader::getFileCache(void *_tex, bool flagclose)
{
  FILE *fp = NULL;
  Texture *tex = (Texture *) _tex;

  fp = getFileCache(tex);
  if (flagclose)
    File::closeFile(fp);
  return fp;
}

FILE * Reader::getFileCache(Texture *tex)
{
  FILE *f = Cache::openCache(tex->url, tex->http);
  if (! f) {
    error("Reader: can't open %s", tex->url);
    return NULL;
  }
  return f;
}

FILE * Reader::getFileCache(const char *url, char *filepath)
{
  FILE *f;

  if ((f = File::openFile(filepath, "rb")) == NULL) {
    if ((f = File::openFile(filepath, "wb")) == NULL) {
      error("getFileCache: can't create %s", filepath);
      return NULL;
    }
    int len;
    char buf[BUFSIZ];
    while ((len = this->read_func(img_hdl, buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, len, f);
    }
  }
  return f;
}

char * Reader::getFilename(void *_tex)
{
  static char filepath[PATH_LEN];
  Texture *tex = (Texture *) _tex;

  Cache::setCacheName(tex->url, filepath);
  return filepath;
}

uint8_t Reader::getChar()
{
  uint8_t data;

  if (this->read_func(img_hdl, (char *) &data, 1) < 1) data = 0;
  ch = data;
  return data;
}

uint8_t Reader::getChar(FILE *fp)
{
  ch = getc(fp);
  return ch;
}

uint8_t Reader::getChar1()
{
  getChar();
  while (ch == '#') {
    do
      getChar();
    while (ch != 0 && ch != '\n' && ch != '\r');
    getChar();
  }
  return ch;
}

uint8_t Reader::getChar1(FILE *fp)
{
  getChar(fp);
  while (ch == '#') {
    do
      getChar(fp);
    while (ch != 0 && ch != '\n' && ch != '\r');
    getChar(fp);
  }
  return ch;
}

void Reader::skipSpaces()
{
  while (isspace(ch)) getChar1();
}

void Reader::skipSpaces(FILE *fp)
{
  while (isspace(ch)) getChar1(fp);
}

int16_t Reader::getShort(FILE *fp)
{
  int c, c1;
  c = getc(fp);  c1 = getc(fp);
  return ((int16_t) c) + (((int16_t) c1) << 8);
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

uint32_t Reader::getUInt(FILE *fp)
{
  int c, c1, c2, c3;

  c = getc(fp);  c1 = getc(fp);  c2 = getc(fp);  c3 = getc(fp);
  return ((uint32_t) c) +
         (((uint32_t) c1) << 8) +
         (((uint32_t) c2) << 16) +
         (((uint32_t) c3) << 24);
}

int32_t Reader::getInt(FILE *fp)
{
  int n = 0;

  if (!isdigit(ch)) return -1; // returns -1 if error
  do {
    n = n*10 + ch-'0';
    getChar1(fp);
  } while (isdigit(ch));
  return n;
}


static uint8_t imgbuf[BUFSIZ];
static int32_t imgbuf_pos = 0;
static int32_t imgbuf_len = 0;

void Reader::reset()
{
  imgbuf_pos = imgbuf_len = 0;
}

uint8_t Reader::getByte(void *texhdl, ImageReader read_func)
{
  if (imgbuf_pos >= imgbuf_len) {
    imgbuf_pos = 0;
    imgbuf_len = read_func(texhdl, (char *) imgbuf, sizeof(imgbuf));
  }
  return imgbuf[imgbuf_pos++];
}
