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
#ifndef IMG_HPP
#define IMG_HPP

#include "prof.hpp"	// new_

#define NORM8_BITS	8
#define NORM16_BITS	16
#define NORM_8		(1 << NORM8_BITS)


typedef int (*ImageReader) (void *tex, char *buf, int len);


/**
 * Img class
 */
class Img {
public:

  /**! we only deal with 8 bits RGB images */
  enum internalchannel_t {
    BW = 1,	///< black & white.
    LA,		///< luninance alpha.
    RGB,	///< red green blue.
    RGBA	///< red green blue alpha.
  };

  static const uint16_t SIZE;   ///< default image side size.

  uint16_t width;	///< image width in pixels.
  uint16_t height;	///< image height in pixels.
  uint8_t channel;	///< bytes per pixel.
  int32_t nbmipmaps;	///< number of mipmaps.
  uint8_t *pixmap;	///< image data.
  //bool anim;		///< flag still or anim.

  Img(uint16_t width, uint16_t height, uint8_t channel);
  Img(uint16_t width, uint16_t height, uint8_t channel, int32_t nbmipmaps, uint32_t size);
  /**< Constructors. */

  virtual ~Img();
  /**< Destructor. */

  static Img * init();
  /**< set a default image. */

  virtual bool sized();
  /**< checks if image is well sized or not. */

  Img* resize(uint16_t width_new, uint16_t height_new);
  /**< Resizes image. */

  /** Image readers. */
  static Img * loadPPM(void *tex, ImageReader rf);
  static Img * loadPGM(void *tex, ImageReader rf);
  static Img * loadGIF(void *tex, ImageReader rf);
  static Img * loadJPG(void *tex, ImageReader rf);
  static Img * loadPNG(void *tex, ImageReader rf);
  static Img * loadPCX(void *tex, ImageReader rf);
  static Img * loadSGI(void *tex, ImageReader rf);
  static Img * loadBMP(void *tex, ImageReader rf);
  static Img * loadTGA(void *tex, ImageReader rf);
  static Img * loadDDS(void *tex, ImageReader rf);
  static Img * loadTIF(void *tex, ImageReader rf);
  static Img * loadXPM(void *tex, ImageReader rf);
  static Img * loadXBM(void *tex, ImageReader rf);
  static Img * loadPSD(void *tex, ImageReader rf);

  /** Image writers. */
  static void saveJPG(const char *file, GLint width, GLint height, GLint quality, const GLubyte *buf);
  static void savePNG(const char *file, GLint width, GLint height);
  static void saveTGA(const char *file, GLint width, GLint height, const GLubyte *buf);
  static void savePPM(const char *file, GLenum mode);

private:
  void defaults();
  /**< Set defaults values */
};


#endif
