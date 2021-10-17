//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "wobject.hpp"	// WObject

#ifndef MIME_LEN
#define MIME_LEN 32
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

class Http;

/**
 * Texture class
 * describes a texture entry in the memory cache
 */
class Texture {

 private:
  bool tex_loaded;		///< loaded flag
  char mime[MIME_LEN];		///< mime type of the texture

  static void httpReader(void *_tex, Http *_http);
  /**< Downloads a texture. */

  static GLuint create();
  /**< Creates texture and returns texid. */

  static void listTextures();
  /**< List textureList. */

 public:
  GLuint id;			///< texture id
  class Img *img;		///< img structure
  class Http *http;		///< http handle
  class WObject *object;	///< getIdByObject()
  char url[URL_LEN];		///< url where is the texture

  Texture(const char *url);
  Texture();
  /**< Constructors. */

  virtual ~Texture();
  /**< Destructor. */

  static void init();
  /**< Initializes the texture cache. */

  static void quit();
  /**< Quits the texture cache. */

  static void update();
  /**< Updates textures (resizing). */

  static GLuint open(const char *url);
  /**< Opens a texture. */

  static GLuint current();
  /**< Gets current texture id. */

  static GLuint getIdByUrl(const char *url);
  /**< Gets a texture id by its url. */

  static GLuint getIdByObject(WObject *wo);
  /**< Gets a texture id by its object. */

  static Texture * getTexByUrl(const char *url);
  /**< Gets a texture by its url. */

  static Texture * getTexById(GLuint texid);
  /**< Gets a texture by its texture id. */

  static char * getUrlById(GLuint texid);
  /**< Gets a texture url by its texture id. */

  void setMime(char *p);
  /**< Sets mime type. */
};

#endif
