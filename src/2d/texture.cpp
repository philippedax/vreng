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
#include "vreng.hpp"
#include "texture.hpp"
#include "img.hpp"	// Img
#include "wobject.hpp"	// WO
#include "http.hpp"	// httpOpen
#include "url.hpp"	// check
#include "format.hpp"	// getLoaderByMime
#include "thread.hpp"	// Vpthread_mutex_t
#include "pref.hpp"	// quality3D

#include <list>

using namespace std;


/* local variables */
static list<Texture*> textureList;
static Img *default_img = NULL;
static GLuint last_texid = 0;
static Vpthread_mutex_t texture_mutex, *tex_pmutex = &texture_mutex;


void Texture::init()
{
  default_img = Img::init();
  initMutex(tex_pmutex);
}

void Texture::quit()
{
  if (default_img) delete default_img;
}

/* called by Render() */
void Texture::update()
{
  lockMutex(tex_pmutex);
  //-------------- lock

  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->img) {
      if (! (*it)->img->pixmap) {
        //error("updateTextures pixmap=null: id=%d url=%s", (*it)->id, (*it)->url);
        continue;
      }
      glBindTexture(GL_TEXTURE_2D, (*it)->id);

      if (! (*it)->img->sized()) {
        /* image to resize */
        Img *img1 = NULL;
        //echo("sized: %s", (*it)->url);
        if ((img1 = (*it)->img->resize(Img::SIZE, Img::SIZE)) == NULL) {
          error("updateTextures: id=%d u=%s", (*it)->id, (*it)->url);
          continue;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, 3, img1->width, img1->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, img1->pixmap);
        delete img1;
      }
      else {
        /* image well sized */
        if (! (*it)->img->nummipmaps) {
          /* no mipmap */
          glTexImage2D(GL_TEXTURE_2D, 0, 3, (*it)->img->width, (*it)->img->height, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, (*it)->img->pixmap);
        }
        else {
          /* have mipmap */
          GLsizei mipw = (*it)->img->width;
          GLsizei miph = (*it)->img->height;
          int mipc = ((*it)->img->bpp == Img::RGB) ? 8 : 16;
          int off = 0;

          /* setup some parameters for texture filters and mipmapping */
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          /* upload mipmaps to video memory */
          for (GLint mip = 0; mip < (*it)->img->nummipmaps; ++mip) {
            GLsizei mips = ((mipw + 3) / 4) * ((miph + 3) / 4) * mipc;	// mip size

            glCompressedTexImage2D(GL_TEXTURE_2D, mip, (*it)->img->bpp,
                                   mipw, miph, 0, mips,
                                   (*it)->img->pixmap + off);
            mipw = MAX(mipw >> 1, 1);
            miph = MAX(miph >> 1, 1);
            off += mips;
          }
        }
      }
      // once the texture resized we can delete its container img
      if ((*it)->loaded) {
        delete (*it)->img;	// sometimes opengl crashes FIXME
        (*it)->img = NULL;
      }
    }
  }
  //-------------- unlock
  unlockMutex(tex_pmutex);
}

int imageReader(void *_tex, char *buf, int len)
{
  Texture *tex = (Texture *) _tex;

  return tex->http->httpRead(buf, len);        // return read length
}

void Texture::reader(void *_tex, Http *_http)
{
  if (! _http) return;
  Texture *tex = (Texture *) _tex;

  tex->http = _http;
  trace(DBG_2D, "texture: mime=%s url=%s", tex->mime, tex->url);

  Img *img = NULL;

  // get the format type by mime type or file extension
  uint8_t format = Format::getLoaderByMime(tex->mime);
  switch (format) {
  case IMG_NULL:
    format = Format::getLoaderByUrl(tex->url);
    break;
  }

  // call the appropriated loader
  switch (format) {
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

Texture::Texture(const char *url)
{
  http = NULL;
  img = NULL;
  object = NULL;
  loaded = false;
  *mime = '\0';

  textureList.push_back(this);
  //dax object = WO::getSolid()->wobject;
  id = create();
  last_texid = id;

  if (url) {
    strcpy(this->url, url);
    // load image
    switch (Format::getLoaderByUrl((char*) url)) {
      case IMG_GIF: Http::httpOpen(url, reader, this, 1); break; // multi-threaded
      default:      Http::httpOpen(url, reader, this, 0); break;
    }
  }
  new_texture++;
}

Texture::~Texture()
{
  del_texture++;
}

GLuint Texture::open(const char *url)
{
  if (! Url::check(url)) return 0;

  Texture * texture = new Texture(url);	// new entry in cache

  texture->id = create();		// creates texture and return texid
  last_texid = texture->id;
  //trace(DBG_2D, "texture: id=%d %s", texture->id, url);

  /* we must download the texture now */
  strcpy(texture->url, url);
  switch (Format::getLoaderByUrl((char*) url)) {
    case IMG_GIF: Http::httpOpen(url, reader, texture, 1); break; // multi-threaded
    default:      Http::httpOpen(url, reader, texture, 0); break;
  }
  return texture->id;
}

/* Creates texid and returns it. */
GLuint Texture::create()
{
  GLuint texid = 0;

  glGenTextures(1, (GLuint *) &texid);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, default_img->width, default_img->height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, default_img->pixmap);
  GLint param = (::g.pref.quality3D) ? GL_LINEAR : GL_NEAREST;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return texid;
}

GLuint Texture::current()
{
  return last_texid;
}

Texture * Texture::getTexById(GLuint texid)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->id == texid) {
      return *it;	// texture is in the cache
    }
  }
  return NULL;
}

GLuint Texture::getIdByUrl(const char *url)
{
  open(url);
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if (! strcmp((*it)->url, url)) {
      return (*it)->id;
    }
  }
  return 0;
}

GLuint Texture::getIdByObject(WO *wo)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->object == wo) {
      return (*it)->id;
    }
  }
  return 0;
}

#if 0 //notused
Texture * Texture::getTexByUrl(const char *url)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if (! strcmp((*it)->url, url)) {
      return *it;	// texture is in the cache
    }
  }
  /* we will download the texture later */
  return NULL;
}

char * Texture::getUrlById(GLuint texid)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if ((*it)->id == texid) {
      return (*it)->url;	// texture is in the cache
    }
  }
  return NULL;
}
#endif //notused

void Texture::setMime(char *p)
{
  if (strlen(p) < MIME_LEN) {
    strcpy(mime, p);
  }
  else {
    strncpy(mime, p, MIME_LEN-1);
    mime[MIME_LEN-1] = 0;
    error("mime type too long = %s", p);
  }
}

void Texture::listTextures()
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    echo("%d %s", (*it)->id, (*it)->url);
  }
  return;
}
