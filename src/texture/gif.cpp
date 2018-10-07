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
/* GIF Decoder (c) 1997 Fabrice BELLARD */
#include "vreng.hpp"
#include "img.hpp"
#include "cache.hpp"	// openCache
#include "file.hpp"	// closeFile
#include "texture.hpp"	// Texture


typedef struct {
  int	screenwidth;	// The dimensions of the screen
  int	screenheight;	// (not those of the image)
  int	global;		// Is there a global color map?
  int	globalbits;	// Number of bits of global colors
  int	backclr;	// Background color
  int	colres;		// Number of bits of color resol
  int	*interleavetable;
  uint8_t globalmap[256][3];

  FILE *fp;
  class Reader *ir;
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


static int offset;
static void LZWDecodeInit(GifInfo *s, int csize);
static void LZWDecode(GifInfo *s, uint8_t *buf, int len);
static int LZWDecodeEnd(GifInfo *s);
static int gifReadSignature(GifInfo *s);
static int gifReadScreen(GifInfo *s);
static int gifReadBlocks(GifInfo *s);


Img * Img::loadGIF(void *tex, ImageReader read_func)
{
  GifInfo s1, *s = &s1;

  Texture *_tex = (Texture *) tex;
  FILE *f;
  if ((f = Cache::openCache(_tex->url, _tex->http)) == NULL) return NULL;

  s->fp = f;
  s->ir = 0;
  s->img = NULL;

  if (gifReadSignature(s)) return NULL;
  if (gifReadScreen(s)) return NULL;
  if (gifReadBlocks(s)) return NULL;

  File::closeFile(f);
  return s->img;
}

/* image data */
static inline int gifread(GifInfo *s, uint8_t *buf, int len)
{
  return fread((char *) buf, 1, len, s->fp);
}

static int gifGetByte(GifInfo *s)
{
  uint8_t ch;

  if ((gifread(s, &ch, 1)) != 1) return -1;
  offset++;
  return ch;
}

static int gifReadSignature(GifInfo *s)
{
  uint8_t buf[6];

  if (gifread(s, buf, 6) != 6) return -2;
  if (strncmp((const char *) buf, "GIF", 3) ||
     (strncmp((const char *) &buf[3], "87a", 3) &&
      strncmp((const char *) &buf[3], "89a", 3)))
    return -3;
  return 0;
}

static int gifReadScreen(GifInfo *s)
{
  uint8_t buf[7];
			
  if (gifread(s, buf, 7) != 7) return -4;
  if (buf[6]) return -5;

  s->screenwidth  = buf[0] + (buf[1] << 8);
  s->screenheight = buf[2] + (buf[3] << 8);
  s->global	  = buf[4] & 0x80;
  s->colres	  = ((buf[4] & 0x70) >> 4) +1;
  s->backclr	  = buf[5];
  if (s->global) {
    s->globalbits = (buf[4] & 0x07) + 1;
    gifread(s, (uint8_t *) s->globalmap, 3*(1<<s->globalbits));
  }
  s->img = new Img(s->screenwidth, s->screenheight, Img::RGB);
  return (s->img == NULL);
}

static int gifReadImage(GifInfo *s)
{
  int nb_pass, pass_height, line_start, line_inc;
  int localbits, ncolors;
  uint8_t *pix, *pix1;
  uint8_t buf[10];
  uint8_t localmap[256][3];

  if ((gifread(s, buf, 10)) != 10) return -6;
  int left		= buf[0] + (buf[1] << 8);
  int top		= buf[2] + (buf[3] << 8);
  int width		= buf[4] + (buf[5] << 8);
  int height		= buf[6] + (buf[7] << 8);
  int interleaved	= buf[8] & 0x40;
  int local		= buf[8] & 0x80;
  if (left+width>s->screenwidth || top+height>s->screenheight) return -7;

  if (local) {
    localbits = (buf[8] & 0x7) + 1;
    ncolors = 1 << localbits;
    gifread(s, (uint8_t *) localmap, 3*ncolors);
  }
  else if (s->global) {
    ncolors = (1 << s->globalbits);
    memcpy(localmap, s->globalmap, 3*ncolors);
  }
  else return -8;

  int cbyte = buf[9];
  if (cbyte<2 || cbyte>8) return -9;

  LZWDecodeInit(s, cbyte);

  if (interleaved) nb_pass=4;
  else nb_pass=1;
  uint8_t *line_buf = new uint8_t[width];

  for (int pass=0; pass < nb_pass; pass++) {
    switch(pass) {
    case 0:
      line_start=0;
      if (interleaved) {
	pass_height=8;
	line_inc=8;
      } else {
	pass_height=1;
	line_inc=1;
      }
      break;
    case 1:
      pass_height=4;
      line_start=4;
      line_inc=8;
      break;
    case 2:
      pass_height=2;
      line_start=2;
      line_inc=4;
      break;
    default:
      pass_height=1;
      line_start=1;
      line_inc=2;
      break;
    }
    for (int line=line_start; line < height; line += line_inc) {
      LZWDecode(s, line_buf, width);
      pix = (uint8_t *)s->img->pixmap + ((left + top * s->screenwidth) + (s->screenwidth * line)) * 3;
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
	  memcpy(pix, pix1, s->screenwidth*3);
	  pix += s->screenwidth*3;
	}
      }
    }
  }
  LZWDecodeEnd(s);
  delete[] line_buf;
  return 0;
}

static int gifReadExtension(GifInfo *s)
{
  uint8_t buf[255];

  int code = gifGetByte(s);
  while (1) {
    int count = gifGetByte(s);
    if (count <= 0) break;
    gifread(s, buf, count);
    switch(code) { case 0xF9: /* graphic control extension */ break; }
  }
  return 0;
}

/* read gif blocks until we find an image block */
static int gifReadBlocks(GifInfo *s)
{
  int ch, err;

  while (1) {
    ch = gifGetByte(s);
    switch (ch) {
    case 0x2C:
      err = gifReadImage(s);
      if (err != 0) return err;
      break;
    case 0x3B: /* trailer */ return 0;
    case 0x21:
      err = gifReadExtension(s);
      if (err != 0) return err;
      break;
    default:
      trace(DBG_IMG, "gifReadBlocks: ch=%02x off=%d(0x%x)", ch, offset, offset);
      //dax continue;
      return -10;
    }
  }
}	

/* LZW decompression */
#define MAXBITS	12
#define	SIZLZW	(1<<MAXBITS)

static void LZWDecodeInit(GifInfo *s, int csize)
{
  s->codesize = csize;
  s->cursize = s->codesize + 1;
  s->clear_code = 1 << s->codesize;
  s->end_code = s->clear_code + 1;
  s->slot = s->newcodes = s->clear_code + 2;
  s->oc = 0;
  s->fc = 0;
  s->stack = new uint8_t[SIZLZW];
  s->suffix = new uint8_t[SIZLZW];
  s->prefix = new int16_t[SIZLZW];
  s->sp = s->stack;
  s->sizbuf = 1;
  s->bbuf = 0;
  s->bbits = 0;
}

static int LZWDecodeEnd(GifInfo *s)
{
  int err = 0, size;

  /* end block */
  if (s->sizbuf > 0) {
    size = gifGetByte(s);
    if (size != 0) err = -1;
  }
  if (s->stack) delete[] s->stack;
  if (s->prefix) delete[] s->prefix;
  if (s->suffix) delete[] s->suffix;
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
	  sizbuf=gifGetByte(s);		\
	  gifread(s,s->buf,sizbuf);	\
	}				\
	pbuf=s->buf;			\
      }					\
      bbuf|=(int)(*pbuf++) << bbits;	\
      bbits+=8;				\
    }					\
    c=bbuf & curmask;			\
    bbuf>>=cursize;			\
    bbits-=cursize;			\
  }					\
}

/*
 * Gif LZW Decoder
 */
static void LZWDecode(GifInfo *s, uint8_t *out_buf, int out_len)
{
  int sizbuf, bbits;
  uint32_t bbuf;
  uint8_t *pbuf, *obuf;
  int olen;
  int c, code, cursize, curmask, clear_code, end_code, newcodes;
  int top_slot, slot, fc, oc;
  uint8_t *stack, *suffix, *sp;
  int16_t *prefix;

/* cache rw */
  cursize = s->cursize;
  slot = s->slot;
  oc = s->oc;
  fc = s->fc;
  sp = s->sp;

  sizbuf = s->sizbuf;
  bbuf = s->bbuf;
  bbits = s->bbits;
  pbuf = s->pbuf;

/* cache r */
  clear_code = s->clear_code;
  end_code = s->end_code;
  newcodes = s->newcodes;
  clear_code = s->clear_code;
  stack = s->stack;
  prefix = s->prefix;
  suffix = s->suffix;

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
      cursize = s->codesize+1;
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

/* cache rw */
  s->cursize = cursize;
  s->slot = slot;
  s->oc = oc;
  s->fc = fc;
  s->sp = sp;

  s->sizbuf = sizbuf;
  s->bbuf = bbuf;
  s->bbits = bbits;
  s->pbuf = pbuf;
}
