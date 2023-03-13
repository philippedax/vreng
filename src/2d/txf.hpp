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
/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

#ifndef TXF_HPP
#define TXF_HPP

#include <list>

#define TXF_FORMAT_BYTE		0
#define TXF_FORMAT_BITMAP	1

struct TexGlyphInfo {
  uint16_t c;	// Potentially support 16-bit glyphs
  uint8_t width;
  uint8_t height;
  signed char xoffset;
  signed char yoffset;
  signed char advance;
  char dummy;		// Space holder for alignment reasons
  short x;
  short y;
};

struct TexGlyphVertexInfo {
  GLfloat t0[2];
  GLshort v0[2];
  GLfloat t1[2];
  GLshort v1[2];
  GLfloat t2[2];
  GLshort v2[2];
  GLfloat t3[2];
  GLshort v3[2];
  GLfloat advance;
};

struct TexFont {
  GLuint texobj;
  int tex_width;
  int tex_height;
  int max_ascent;
  int max_descent;
  int num_glyphs;
  int min_glyph;
  int range;
  uint8_t *teximage;
  TexGlyphInfo *tgi;
  TexGlyphVertexInfo *tgvi;
  TexGlyphVertexInfo **lut;
};


/**
 * Txf class
 */
class Txf {

public:
  Txf(const char *url);	///< constructor

  virtual ~Txf();	///< destructor

  static Txf * load(const char *url);
  /**< get Txf from cache or downloading. */

  void bindTexture();
  /**< bind font texture. */

  GLuint buildTexture(GLuint texobj, GLboolean setupMipmaps);
  /**< build texture. */

  void render(const char *string, int len);
  /**< render string. */

  virtual void render(int c);
  /**< render one character. */

private:
  static std::list<Txf*> txfList; ///< txf list

  uint16_t  num;		///< Txf number
  char url[URL_LEN];		///< font url
  Txf *next;			///< next Txf
  struct TexFont *texfont;	///< texfont ptr

  const char * getUrl() const;
  /**< Gets url name */

  TexGlyphVertexInfo * getGlyph(int c);

  uint16_t getCurrentNumber() const;
  /**< get Txf current font number  */

  static const char *errorString();

  Txf * getByUrl(const char *url);
  /**< get Txf by its url  */

  Txf * getByNumber(uint16_t num);
  /**< get Txf by its number  */

  static void reader(void *atexfont, class Http *http);
  /**< http texfont loader */

};

#endif
