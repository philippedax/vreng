//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
/* Copyright (c) Mark J. Kilgard, 1997. */
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "txf.hpp"
#include "http.hpp"	// httpOpen
#include "cache.hpp"	// openCache
#include "file.hpp"	// closeFile

#include <list>
using namespace std;

list<Txf*> Txf::txfList;

// local
static const char *lastError;
static uint16_t txf_number = 0;


Txf::Txf(const char *_url)
{
  num = ++txf_number;
  strcpy(url, _url);
}

Txf::~Txf()
{
#if 0 //texfnts are kept in memory
  if (! texfont) return;
  delete[] texfont->tgi;
  delete[] texfont->tgvi;
  delete[] texfont->lut;
  delete[] texfont->teximage;
  delete[] texfont;
#endif
  txfList.remove(this);
}

/* Loads texture font. */
Txf * Txf::load(const char *url)
{
  trace(DBG_IMG, "load: loading %s", url);

  Txf * txf = new Txf(url);	// new entry

  txfList.push_back(txf);	// add to cache list

  /* load font */
  Http::httpOpen(url, httpReader, txf, 0);

  return txf;
}

const char * Txf::getUrl() const
{
  return (const char *) url;
}

void Txf::httpReader(void *_txf, Http *http)
{
  Txf *txf = (Txf *) _txf;
  if (! txf) return;

  FILE *f = Cache::openCache(txf->getUrl(), http);
  if (! f) { error("can't open %s", txf->getUrl()); return; }

  txf->texfont = new struct TexFont[1];
  txf->texfont->tgi = NULL;
  txf->texfont->tgvi = NULL;
  txf->texfont->lut = NULL;
  txf->texfont->teximage = NULL;

  int format, stride, width, height, max_glyph, min_glyph, endianness, swap;
  GLfloat w, h, xstep, ystep;
  char tmp;

  char fileid[4];
  int got = fread((char *) fileid, 1, 4, f);
  if (got != 4 || strncmp(fileid, "\377txf", 4)) {
    lastError = "not a texture font file."; goto error;
  }

  got = fread((char *) &endianness, sizeof(int), 1, f);
  if (got == 1 && endianness == 0x12345678)      swap = 0;
  else if (got == 1 && endianness == 0x78563412) swap = 1;
  else { lastError = "not a texture font file."; goto error; }

#define EXPECT(v) if (got != (int)v) {lastError="premature end of file"; goto error;}
  got = fread((char *) &format, sizeof(int), 1, f); EXPECT(1);
  got = fread((char *) &txf->texfont->tex_width, sizeof(int), 1, f); EXPECT(1);
  got = fread((char *) &txf->texfont->tex_height, sizeof(int), 1, f); EXPECT(1);
  got = fread((char *) &txf->texfont->max_ascent, sizeof(int), 1, f); EXPECT(1);
  got = fread((char *) &txf->texfont->max_descent, sizeof(int), 1, f); EXPECT(1);
  got = fread((char *) &txf->texfont->num_glyphs, sizeof(int), 1, f); EXPECT(1);

  if (swap) {
    SWAPL(&format, tmp);
    SWAPL(&txf->texfont->tex_width, tmp);
    SWAPL(&txf->texfont->tex_height, tmp);
    SWAPL(&txf->texfont->max_ascent, tmp);
    SWAPL(&txf->texfont->max_descent, tmp);
    SWAPL(&txf->texfont->num_glyphs, tmp);
  }
  trace(DBG_IMG, "txfReader: fmt=%d w=%d h=%d ascent=%d descent=%d glyphs=%d", format, txf->texfont->tex_width, txf->texfont->tex_height, txf->texfont->max_ascent, txf->texfont->max_descent, txf->texfont->num_glyphs);

  txf->texfont->tgi = new TexGlyphInfo[txf->texfont->num_glyphs];
  if (txf->texfont->tgi == NULL) { lastError = "out of memory."; goto error; }
  got = fread((char *) txf->texfont->tgi, sizeof(TexGlyphInfo), txf->texfont->num_glyphs, f);
  EXPECT(txf->texfont->num_glyphs);

  if (swap) {
    for (int i=0; i < txf->texfont->num_glyphs; i++) {
      SWAPS(&txf->texfont->tgi[i].c, tmp);
      SWAPS(&txf->texfont->tgi[i].x, tmp);
      SWAPS(&txf->texfont->tgi[i].y, tmp);
    }
  }
  txf->texfont->tgvi = new TexGlyphVertexInfo[txf->texfont->num_glyphs];
  if (txf->texfont->tgvi == NULL) { lastError = "out of memory."; goto error; }

  w = txf->texfont->tex_width;
  h = txf->texfont->tex_height;
  xstep = 0.5 / w;
  ystep = 0.5 / h;
  for (int i=0; i < txf->texfont->num_glyphs; i++) {
    TexGlyphInfo *tgi = &txf->texfont->tgi[i];
    txf->texfont->tgvi[i].t0[0] = tgi->x / w + xstep;
    txf->texfont->tgvi[i].t0[1] = tgi->y / h + ystep;
    txf->texfont->tgvi[i].v0[0] = tgi->xoffset;
    txf->texfont->tgvi[i].v0[1] = tgi->yoffset;
    txf->texfont->tgvi[i].t1[0] = (tgi->x + tgi->width) / w + xstep;
    txf->texfont->tgvi[i].t1[1] = tgi->y / h + ystep;
    txf->texfont->tgvi[i].v1[0] = tgi->xoffset + tgi->width;
    txf->texfont->tgvi[i].v1[1] = tgi->yoffset;
    txf->texfont->tgvi[i].t2[0] = (tgi->x + tgi->width) / w + xstep;
    txf->texfont->tgvi[i].t2[1] = (tgi->y + tgi->height) / h + ystep;
    txf->texfont->tgvi[i].v2[0] = tgi->xoffset + tgi->width;
    txf->texfont->tgvi[i].v2[1] = tgi->yoffset + tgi->height;
    txf->texfont->tgvi[i].t3[0] = tgi->x / w + xstep;
    txf->texfont->tgvi[i].t3[1] = (tgi->y + tgi->height) / h + ystep;
    txf->texfont->tgvi[i].v3[0] = tgi->xoffset;
    txf->texfont->tgvi[i].v3[1] = tgi->yoffset + tgi->height;
    txf->texfont->tgvi[i].advance = tgi->advance;
  }

  min_glyph = txf->texfont->tgi[0].c;
  max_glyph = txf->texfont->tgi[0].c;
  for (int i=1; i < txf->texfont->num_glyphs; i++) {
    if (txf->texfont->tgi[i].c < min_glyph)  min_glyph = txf->texfont->tgi[i].c;
    if (txf->texfont->tgi[i].c > max_glyph)  max_glyph = txf->texfont->tgi[i].c;
  }
  txf->texfont->min_glyph = min_glyph;
  txf->texfont->range = max_glyph - min_glyph + 1;

  txf->texfont->lut = new TexGlyphVertexInfo*[txf->texfont->range];
  if (txf->texfont->lut == NULL) {
    lastError = "out of memory.";
    goto error;
  }
  for (int i=0; i < txf->texfont->num_glyphs; i++) {
    txf->texfont->lut[txf->texfont->tgi[i].c - txf->texfont->min_glyph] = &txf->texfont->tgvi[i];
  }

  uint8_t *texbitmap;

  switch (format) {
  case TXF_FORMAT_BYTE:
    txf->texfont->teximage = new uint8_t[txf->texfont->tex_width * txf->texfont->tex_height];
    if (txf->texfont->teximage == NULL) { lastError = "out of memory."; goto error; }
    got = fread((char *) txf->texfont->teximage, 1, txf->texfont->tex_width*txf->texfont->tex_height, f);
    EXPECT(txf->texfont->tex_width * txf->texfont->tex_height);
    break;
  case TXF_FORMAT_BITMAP:
    width = txf->texfont->tex_width;
    height = txf->texfont->tex_height;
    stride = (width + 7) >> 3;
    if ((texbitmap = new uint8_t[stride * height]) == NULL) {
      lastError = "out of memory.";
      goto error;
    }
    got = fread((char *) texbitmap, 1, stride * height, f);
    EXPECT(stride * height);
    if ((txf->texfont->teximage = new uint8_t[width * height]) == NULL) {
      lastError = "out of memory.";
      goto error;
    }
    for (int i=0; i < height; i++) {
      for (int j=0; j < width; j++) {
        if (texbitmap[i * stride + (j >> 3)] & (1 << (j & 7))) {
          txf->texfont->teximage[i * width + j] = 255;
        }
      }
    }
    if (texbitmap) delete[] texbitmap;
    texbitmap = NULL;
    break;
  }
  File::closeFile(f);
  return;

error:
  error("txf err: %s", lastError);
  if (txf->texfont) {
    delete[] txf->texfont->tgi;
    delete[] txf->texfont->tgvi;
    delete[] txf->texfont->lut;
    delete[] txf->texfont->teximage;
    delete[] txf->texfont; txf->texfont = NULL;
  }
  File::closeFile(f);
  return;
}

TexGlyphVertexInfo * Txf::getGlyph(int c)
{
  TexGlyphVertexInfo *tgvi;

  /* Automatically substitute uppercase letters with lowercase if not
     uppercase available (and vice versa). */
lastchance:
  if ((c >= texfont->min_glyph) && (c < texfont->min_glyph + texfont->range)) {
    if ((tgvi = texfont->lut[c - texfont->min_glyph]) != 0)
      return tgvi;
    if (islower(c)) {
      c = toupper(c);
      if ((c >= texfont->min_glyph) && (c < texfont->min_glyph + texfont->range))
        return texfont->lut[c - texfont->min_glyph];
    }
    if (isupper(c)) {
      c = tolower(c);
      if ((c >= texfont->min_glyph) && (c < texfont->min_glyph + texfont->range))
        return texfont->lut[c - texfont->min_glyph];
    }
  }
  error("texfont: unavailable font character \"%c\" (%d)", isprint(c) ? c : ' ', c);
  c = ' ';
  goto lastchance;
  /* NOTREACHED */
}

GLuint Txf::buildTexture(GLuint texobj, GLboolean setupMipmaps)
{
  if (! texfont) return (GLuint) 0;

  if (texfont->texobj == 0) {
    if (texobj == 0)
      glGenTextures(1, &texfont->texobj);
    else
      texfont->texobj = texobj;
  }

  glBindTexture(GL_TEXTURE_2D, texfont->texobj);

  /* Use GL_INTENSITY4 as internal texture format since we want to use as
     little texture memory as possible. */
  if (setupMipmaps) {
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_INTENSITY4,
                      texfont->tex_width, texfont->tex_height,
	              GL_LUMINANCE, GL_UNSIGNED_BYTE, texfont->teximage);
  }
  else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY4,
	         texfont->tex_width, texfont->tex_height, 0,
		 GL_LUMINANCE, GL_UNSIGNED_BYTE, texfont->teximage);
  }
  return texfont->texobj;
}

void Txf::bindTexture()
{
  glBindTexture(GL_TEXTURE_2D, texfont->texobj);
}

// Renders a glyph.
void Txf::render(int c)
{
  TexGlyphVertexInfo *tgvi;
  if ((tgvi = getGlyph(c)) == NULL) return;

  glBegin(GL_QUADS);
  glTexCoord2fv(tgvi->t0);
  glVertex2sv(tgvi->v0);
  glTexCoord2fv(tgvi->t1);
  glVertex2sv(tgvi->v1);
  glTexCoord2fv(tgvi->t2);
  glVertex2sv(tgvi->v2);
  glTexCoord2fv(tgvi->t3);
  glVertex2sv(tgvi->v3);
  glEnd();

  glTranslatef(tgvi->advance, 0, 0);
}

// Renders a string of glyphs.
void Txf::render(const char *s, int l)
{
  for (int i=0; i < l; i++) {
    render(s[i]);	// render glyph by glyph
  }
}

Txf * Txf::getByUrl(const char *url)
{
  for (list<Txf*>::iterator it = txfList.begin(); it != txfList.end() ; ++it)
    if (! strcmp((*it)->url, url))
      return (*it);	// txf is in the cache
  return NULL;		// we must dowdload the txf latter
}

Txf * Txf::getByNumber(uint16_t num)
{
  for (list<Txf*>::iterator it = txfList.begin(); it != txfList.end() ; ++it)
    if ((*it)->num == num)
      return (*it);	// txf is in the cache
  return NULL;
}

uint16_t Txf::getCurrentNumber() const
{
  return txf_number;
}

const char * Txf::errorString()
{
  return lastError;
}
