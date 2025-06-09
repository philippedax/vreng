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
// texture.cpp
//
// Texture handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "texture.hpp"
#include "img.hpp"	// Img
#include "object.hpp"	// Object
#include "http.hpp"	// httpOpen
#include "url.hpp"	// check
#include "format.hpp"	// getLoaderByUrl
#include "thread.hpp"	// Vpthread_mutex_t
#include "pref.hpp"	// quality3D

#include <list>


// local variables
static std::list<Texture*> textureList;
static Img *default_img = NULL;
static GLuint last_texid = 0;
static Vpthread_mutex_t texture_mutex, *tex_pmutex = &texture_mutex;


/** Texture initialization */
void Texture::init()
{
  default_img = Img::init();
  initMutex(tex_pmutex);
}

/** Updates textures - called by render() */
void Texture::update()
{
  lockMutex(tex_pmutex);
  //-------------- lock

  for (std::list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->img) {
      if (! (*it)->img->pixmap) {
        //error("updateTextures pixmap=null: id=%d url=%s", (*it)->tex_id, (*it)->url);
        continue;
      }
      glBindTexture(GL_TEXTURE_2D, (*it)->tex_id);

      if (! (*it)->img->wellsized()) {	// image must be resized
        Img *img1 = NULL;
        if (! (img1 = (*it)->img->resize(Img::SIZE, Img::SIZE))) {
          error("updateTextures: id=%d u=%s", (*it)->tex_id, (*it)->url);
          continue;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, 3, img1->width, img1->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, img1->pixmap);
        if (img1) delete img1;
      }
      else {				// image well sized
        if (! (*it)->img->nummipmaps) {	// no mipmap
          glTexImage2D(GL_TEXTURE_2D, 0, 3, (*it)->img->width, (*it)->img->height, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, (*it)->img->pixmap);
        }
        else {				// have mipmap
          GLsizei mipw = (*it)->img->width;
          GLsizei miph = (*it)->img->height;
          int mipc = ((*it)->img->bpp == Img::RGB) ? 8 : 16;
          int off = 0;

          // setup some parameters for texture filters and mipmapping
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          // upload mipmaps to video memory
          for (GLint mip = 0; mip < (*it)->img->nummipmaps; ++mip) {
            GLsizei mips = ((mipw + 3) / 4) * ((miph + 3) / 4) * mipc;	// mip size
            glCompressedTexImage2D(GL_TEXTURE_2D, mip, (*it)->img->bpp,
                                   mipw, miph, 0, mips, (*it)->img->pixmap + off);
            mipw = MAX(mipw >> 1, 1);
            miph = MAX(miph >> 1, 1);
            off += mips;
          }
        }
      }
      // once the texture resized we can delete its container img
      if ((*it)->loaded) {
        if ((*it)->img) delete (*it)->img;	// sometimes opengl crashes FIXME
        (*it)->img = NULL;
      }
    }
  }
  //-------------- unlock
  unlockMutex(tex_pmutex);
}

/** image reader */
int imageReader(void *_tex, char *buf, int len)
{
  Texture *tex = static_cast<Texture *> (_tex);

  return tex->http->httpRead(buf, len);        // return read length
}

/** Selects a texture loader */
void Texture::selectLoader(void *_tex, Http *_http)
{
  if (! _http) return;
  Texture *tex = static_cast<Texture *> (_tex);
  tex->http = _http;

  uint8_t format = 0;
  Img *img = NULL;

  // get the format type by file extension
  format = Format::getLoaderByUrl(tex->url);
  switch (format) {	// call the appropriated loader
    case IMG_GIF: img = Img::loadGIF(tex, imageReader); break;
    case IMG_PNG: img = Img::loadPNG(tex, imageReader); break;
    case IMG_JPG: img = Img::loadJPG(tex, imageReader); break;
    case IMG_PPM: img = Img::loadPPM(tex, imageReader); break;
    case IMG_PGM: img = Img::loadPGM(tex, imageReader); break;
    case IMG_BMP: img = Img::loadBMP(tex, imageReader); break;
    case IMG_TGA: img = Img::loadTGA(tex, imageReader); break;
    case IMG_SGI: img = Img::loadSGI(tex, imageReader); break;
    case IMG_PCX: img = Img::loadPCX(tex, imageReader); break;
    case IMG_DDS: img = Img::loadDDS(tex, imageReader); break;
    case IMG_TIF: img = Img::loadTIF(tex, imageReader); break;
    case IMG_XPM: img = Img::loadXPM(tex, imageReader); break;
    case IMG_XBM: img = Img::loadXBM(tex, imageReader); break;
    case IMG_PSD: img = Img::loadPSD(tex, imageReader); break;
    case IMG_SVG: img = Img::loadSVG(tex, imageReader); break;
    case IMG_NULL: return;
    default:
      echo("texture: unrecognized image format");
      return;
  }

  lockMutex(tex_pmutex);
  //-------------- lock
  tex->img = img;
  tex->loaded = true;
  //-------------- unlock
  unlockMutex(tex_pmutex);
}

/** Constructor */
Texture::Texture(const char *_url)
{
  http = NULL;
  url = NULL;
  img = NULL;
  object = NULL;
  loaded = false;

  textureList.push_back(this);
  tex_id = create();
  last_texid = tex_id;

  if (! url)
    url = new char[URL_LEN];
  strcpy(url, _url);
  // load image
  switch (Format::getLoaderByUrl(const_cast<char *> (_url))) {
    case IMG_GIF: Http::httpOpen(_url, selectLoader, this, 1); break; // multi-threaded
    default:      Http::httpOpen(_url, selectLoader, this, 0); break;
  }
  new_texture++;
}

/** Quit - Destructor */
Texture::~Texture()
{
  if (default_img) delete default_img;
  del_texture++;
}

/** Opens an image texture - returns tex_id */
GLuint Texture::open(const char *_url)
{
  if (! Url::check(_url)) return 0;

  Texture * texture = new Texture(_url);	// new entry in cache

  texture->tex_id = create();		// creates texture and return texid
  last_texid = texture->tex_id;
  //echo("texture: id=%d %s", texture->tex_id, _url);

  // we must download the texture now
  strcpy(texture->url, _url);
  switch (Format::getLoaderByUrl(const_cast<char *> (_url))) {
    case IMG_GIF: Http::httpOpen(_url, selectLoader, texture, 1); break; // multi-threaded
    default:      Http::httpOpen(_url, selectLoader, texture, 0); break;
  }
  return texture->tex_id;
}

/** Creates texid and returns it. */
GLuint Texture::create()
{
  GLuint id = 0;

  glGenTextures(1, (GLuint *) &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, default_img->width, default_img->height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, default_img->pixmap);
  GLint param = (::g.pref.quality3D) ? GL_LINEAR : GL_NEAREST;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return id;
}

/** Gets current texid */
GLuint Texture::current()
{
  return last_texid;
}

/** Gets texture by its ID */
Texture * Texture::getTexById(GLuint texid)
{
  for (std::list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->tex_id == texid) {
      return *it;	// texture is in the cache
    }
  }
  return NULL;
}

/** Gets texture-id by its url */
GLuint Texture::getIdByUrl(const char *url)
{
  open(url);
  for (std::list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if (! strcmp((*it)->url, url)) {
      return (*it)->tex_id;
    }
  }
  return 0;
}
/** Gets texture-id by its object */
GLuint Texture::getIdByObject(Object *wo)
{
  for (std::list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->object == wo) {
      return (*it)->tex_id;
    }
  }
  return 0;
}
