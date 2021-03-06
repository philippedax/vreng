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


/**
 * Texture class
 * describes a texture entry in the memory cache
 */
class Texture {

 private:
  bool loaded;			///< load flag

 public:
  GLuint id;			///< texture id
  class Http *http;		///< http handle
  class Img *img;		///< img structure
  class Solid *solid;
  class WObject *object;
  char url[URL_LEN];		///< url where is the texture
  char mime[MIME_LEN];		///< mime type of the texture

  Texture(const char *url);
  Texture();
  /**< Constructor */

  virtual ~Texture();
  /**< Destructor */

  static void httpReader(void *_tex, Http *_http);
  /**< Downloads a texture */

  static void initCache();
  /**< Initializes the texture cache */

  static void closeCache();
  /**< Close the texture cache */

  static bool check(const char *url);
  /**< Check url integity */

  static GLuint exist(const char *url);
  /**< Check existence of texture integity */

  static GLuint getid();
  /**< Gets texture id */

  GLuint get(const char *url, WObject *wo);
  /**< Gets a texture from the cache */

  static GLuint getFromCache(const char *url);
  /**< Gets a texture from the cache */

  static GLuint current();
  /**< Gets current texture id */

  static GLuint getIdByUrl(const char *url);
  /**< Gets a texture id by its url */

  static GLuint getIdByObject(WObject *wo);
  /**< Gets a texture id by its object */

  static Texture * getEntryByUrl(const char *url);
  /**< Gets a tc by its url */

  static Texture * getEntryById(GLuint texid);
  /**< Gets a texture by its texture id */

  static char * getUrlById(GLuint texid);
  /**< Gets a texture url by its texture id */

  static void update();
  /**< Updates textures (resizing) */

  void setMime(char *p);
  /**< copy mime type */

  static void listTextures();
  /**< List textureList */
};

#endif
