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
/*
 * GIF Decoder (c) 1997
 * Fabrice BELLARD
 */
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// open, open
#include "texture.hpp"	// Texture


typedef struct {
  int	width;		// The dimensions of the screen
  int	height;		// (not those of the image)
  int	global;		// Is there a global color map?
  int	globalbits;	// Number of bits of global colors
  int	backclr;	// Background color
  int	colres;		// Number of bits of color resol
  int	*interleavetable;
  uint8_t globalmap[256][3];

  FILE *fp;
  class Img *img;

/* lzw */
  int sizbuf, bbits;
  uint32_t bbuf;
  uint8_t buf[256], *pbuf;
  int cursize, codesize, clear_code, end_code, newcodes;
  int slot, fc, oc;
  uint8_t *stack, *suffix;
  int16_t *prefix;
  uint8_t *sp;
} GifInfo;

// local

static int offset = 0;
static void LZWDecodeInit(GifInfo *g, int csize);
static void LZWDecode(GifInfo *g, uint8_t *buf, int len);
static int LZWDecodeEnd(GifInfo *g);
static int gifReadSignature(GifInfo *g);
static int gifReadScreen(GifInfo *g);
static int gifReadBlocks(GifInfo *g);


Img * Img::loadGIF(void *_tex, ImageReader read_func)
{
  GifInfo s, *g = &s;
  Texture *tex = static_cast<Texture *>(_tex);

  Cache *cache = new Cache();
  if ((g->fp = cache->open(tex->url, tex->http)) == NULL) {
    error("can't read %s", tex->url);
    delete cache;
    return NULL;
  }

  g->img = NULL;

  if (gifReadSignature(g)) {
    cache->close();
    if (cache) delete cache;
    return NULL;
  }
  if (gifReadScreen(g)) {
    cache->close();
    if (cache) delete cache;
    return NULL;
  }
  if (gifReadBlocks(g)) {
    cache->close();
    if (cache) delete cache;
    return NULL;
  }

  //dax Cache::closeCache(g->fp);
  cache->close();
  if (cache) delete cache;
  return g->img;
}

/** image data */
static inline int gifread(GifInfo *g, uint8_t *buf, int len)
{
  return fread(reinterpret_cast<char *>(buf), 1, len, g->fp);
}

static int gifGetByte(GifInfo *g)
{
  uint8_t c;

  if ((gifread(g, &c, 1)) != 1) return -1;
  offset++;
  return c;
}

static int gifReadSignature(GifInfo *g)
{
  uint8_t buf[6];

  if (gifread(g, buf, 6) != 6) return -2;
  if (strncmp((const char *) buf, "GIF", 3) ||
     (strncmp((const char *) &buf[3], "87a", 3) &&
      strncmp((const char *) &buf[3], "89a", 3)))
    return -3;
  return 0;
}

static int gifReadScreen(GifInfo *g)
{
  uint8_t buf[7];

  if (gifread(g, buf, 7) != 7) return -4;
  if (buf[6]) return -5;

  g->width  	= buf[0] + (buf[1] << 8);
  g->height 	= buf[2] + (buf[3] << 8);
  g->global	= buf[4] & 0x80;
  g->colres	= ((buf[4] & 0x70) >> 4) +1;
  g->backclr	= buf[5];
  if (g->global) {
    g->globalbits = (buf[4] & 0x07) + 1;
    gifread(g, (uint8_t *) g->globalmap, 3*(1<<g->globalbits));
  }
  g->img = new Img(g->width, g->height, Img::RGB);
  return (g->img == NULL);
}

static int gifReadImage(GifInfo *g)
{
  int nb_pass, pass_height, line_start, line_inc;
  int localbits, ncolors;
  uint8_t *pix, *pix1;
  uint8_t buf[10];
  uint8_t localmap[256][3];

  if ((gifread(g, buf, 10)) != 10) return -6;

  int left		= buf[0] + (buf[1] << 8);
  int top		= buf[2] + (buf[3] << 8);
  int width		= buf[4] + (buf[5] << 8);
  int height		= buf[6] + (buf[7] << 8);
  int interleaved	= buf[8] & 0x40;
  int local		= buf[8] & 0x80;
  if (left+width>g->width || top+height>g->height) return -7;

  if (local) {
    localbits = (buf[8] & 0x7) + 1;
    ncolors = 1 << localbits;
    gifread(g, (uint8_t *) localmap, 3*ncolors);
  }
  else if (g->global) {
    ncolors = (1 << g->globalbits);
    memcpy(localmap, g->globalmap, 3*ncolors);
  }
  else return -8;

  int cbyte = buf[9];
  if (cbyte<2 || cbyte>8) return -9;

  LZWDecodeInit(g, cbyte);

  if (interleaved) nb_pass=4;
  else nb_pass=1;
  uint8_t *line_buf = new uint8_t[width];

  for (int pass=0; pass < nb_pass; pass++) {
    switch(pass) {
    case 0:
      line_start = 0;
      if (interleaved) {
	pass_height = 8;
	line_inc = 8;
      } else {
	pass_height = 1;
	line_inc = 1;
      }
      break;
    case 1:
      pass_height = 4;
      line_start = 4;
      line_inc = 8;
      break;
    case 2:
      pass_height = 2;
      line_start = 2;
      line_inc = 4;
      break;
    default:
      pass_height = 1;
      line_start = 1;
      line_inc = 2;
      break;
    }
    for (int line=line_start; line < height; line += line_inc) {
      LZWDecode(g, line_buf, width);
      pix = (uint8_t *)g->img->pixmap + ((left + top * g->width) + (g->width * line)) * 3;
      pix1 = pix;
      for (int i=0; i < width; i++) {
	int c = line_buf[i];
	pix[0] = localmap[c][0];
	pix[1] = localmap[c][1];
	pix[2] = localmap[c][2];
	pix += 3;
      }
      if (pass_height > 1) {
	for (int j=1; j < pass_height; j++) {
	  memcpy(pix, pix1, g->width*3);
	  pix += g->width*3;
	}
      }
    }
  }
  LZWDecodeEnd(g);
  delete[] line_buf;
  return 0;
}

static void gifReadExtension(GifInfo *g)
{
  uint8_t buf[255];

  int c = gifGetByte(g);
  while (1) {
    int count = gifGetByte(g);
    if (count <= 0) break;
    gifread(g, buf, count);
    switch(c) { case 0xF9: /* graphic control extension */ break; }
  }
}

/** read gif blocks until we find an image block */
static int gifReadBlocks(GifInfo *g)
{
  int c, err;

  while (1) {
    c = gifGetByte(g);
    switch (c) {
    case 0x2C:
      err = gifReadImage(g);
      if (err != 0) return err;
      break;
    case 0x3B: /* trailer */ return 0;
    case 0x21:
      gifReadExtension(g);
      break;
    default:
      trace(DBG_2D, "gifReadBlocks: ch=%02x off=%d(0x%x)", c, offset, offset);
      return -10;
    }
  }
}

/** LZW decompression */
#define MAXBITS	12
#define	SIZLZW	(1<<MAXBITS)

static void LZWDecodeInit(GifInfo *g, int csize)
{
  g->codesize = csize;
  g->cursize = g->codesize + 1;
  g->clear_code = 1 << g->codesize;
  g->end_code = g->clear_code + 1;
  g->slot = g->newcodes = g->clear_code + 2;
  g->oc = 0;
  g->fc = 0;
  g->stack = new uint8_t[SIZLZW];
  g->suffix = new uint8_t[SIZLZW];
  g->prefix = new int16_t[SIZLZW];
  g->sp = g->stack;
  g->sizbuf = 1;
  g->bbuf = 0;
  g->bbits = 0;
}

static int LZWDecodeEnd(GifInfo *g)
{
  int err = 0, size;

  /* end block */
  if (g->sizbuf > 0) {
    size = gifGetByte(g);
    if (size != 0) err = -1;
  }
  //dax if (g->stack) delete[] g->stack;
  //dax if (g->prefix) delete[] g->prefix;
  //dax if (g->suffix) delete[] g->suffix;
  return err;
}


#define GETCODE(c)			\
{					\
  if (bbits>=cursize) {			\
    c=bbuf & curmask;			\
    bbuf>>=cursize;			\
    bbits-=cursize;			\
  } else {				\
    while (bbits<=24) {			\
      sizbuf--;				\
      if (sizbuf<=0) {			\
	if (sizbuf<0) {			\
	  sizbuf=0;			\
	} else {			\
	  sizbuf=gifGetByte(g);		\
	  gifread(g,g->buf,sizbuf);	\
	}				\
	pbuf=g->buf;			\
      }					\
      bbuf|=(int)(*pbuf++) << bbits;	\
      bbits+=8;				\
    }					\
    c=bbuf & curmask;			\
    bbuf>>=cursize;			\
    bbits-=cursize;			\
  }					\
}

/**
 * Gif LZW Decoder
 */
static void LZWDecode(GifInfo *g, uint8_t *out_buf, int out_len)
{
  int sizbuf, bbits;
  uint32_t bbuf;
  uint8_t *pbuf, *obuf;
  int olen;
  int c, code, cursize, curmask, clear_code, end_code, newcodes;
  int top_slot, slot, fc, oc;
  uint8_t *stack, *suffix, *sp;
  int16_t *prefix;

  cursize = g->cursize;
  slot = g->slot;
  oc = g->oc;
  fc = g->fc;
  sp = g->sp;

  sizbuf = g->sizbuf;
  bbuf = g->bbuf;
  bbits = g->bbits;
  pbuf = g->pbuf;

  clear_code = g->clear_code;
  end_code = g->end_code;
  newcodes = g->newcodes;
  clear_code = g->clear_code;
  stack = g->stack;
  prefix = g->prefix;
  suffix = g->suffix;

  top_slot = (1 << cursize);
  curmask =  top_slot - 1;
  obuf = out_buf;
  olen = out_len;

  if (olen == 0) goto _end;

  while (sp > stack) {
    *obuf++ = *(--sp);
    if ( (--olen) == 0 ) goto _end;
  }

  while (1) {
    GETCODE(c);
    if (c == end_code) break;
    if (c == clear_code) {
      cursize = g->codesize+1;
      top_slot = 1<<cursize;
      curmask=top_slot - 1;
      slot = newcodes;
      do {
	GETCODE(c);
      } while (c == clear_code);
      if (c == end_code) break;
      fc = oc = c;
      *obuf++ = c;
      if ((--olen) == 0) break;
    }
    else {
      code = c;
      if (code >= slot) {
	*sp++ = fc;
	code = oc;
      }
      while (code >= newcodes) {
	*sp++ = suffix[code];	//segfault snowleopard
	code = prefix[code];
      }
      *sp++ = code;
      if (slot < top_slot) {
	suffix[slot] = fc = code;
	prefix[slot++] = oc;
	oc = c;
      }
      if (slot >= top_slot) {
	if (cursize < MAXBITS) {
	  cursize++;
	  top_slot <<= 1;
	  curmask=top_slot - 1;
	}
      }
      while (sp > stack) {
	*obuf++ = *(--sp);
	if ((--olen) == 0) goto _end;
      }
    }
  }
 _end:

  g->cursize = cursize;
  g->slot = slot;
  g->oc = oc;
  g->fc = fc;
  g->sp = sp;

  g->sizbuf = sizbuf;
  g->bbuf = bbuf;
  g->bbits = bbits;
  g->pbuf = pbuf;
}
