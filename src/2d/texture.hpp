//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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


#ifndef MIME_LEN
#define MIME_LEN 32
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif


/**
 * Texture class
 */
class Texture {

 private:
  bool loaded;		///< load flag
  char *mime;		///< mime type of the texture

 public:
  GLuint tex_id;	///< texture id
  class Img *img;	///< img structure
  class Http *http;	///< http handle
  class WO *object;	///< object referant
  char *url;		///< url where is the texture

  Texture(const char *url);
  /**< Constructor. */

  virtual ~Texture();
  /**< Destructor. */

  static void selectLoader(void *_tex, Http *_http);
  /**< Selects a texture loader. */

  static void init();
  /**< Initializes the texture cache. */

  static void quit();
  /**< Close the texture cache. */

  static GLuint open(const char *url);
  /**< Opens a texture. */

  static GLuint current();
  /**< Gets current texture id. */

  static GLuint getIdByUrl(const char *url);
  /**< Gets a texture id by its url. */

  static Texture * getTexById(GLuint texid);
  /**< Gets a texture by its texture id. */

  static GLuint getIdByObject(class WO *wo);
  /**< Gets a texture id by its object. */

  static void update();
  /**< Updates textures (resizing). */

  void setMime(char *p);
  /**< copy mime type. */

  static void listTextures();
  /**< List textureList. */

  //notused static char * getUrlById(GLuint texid);
  //notused /**< Gets a texture url by its texture id. */
  //notused static Texture * getTexByUrl(const char *url);
  //notused /**< Gets a tc by its url. */

 private:
  static GLuint create();
  /**< Creates texture id. */
};

#endif
