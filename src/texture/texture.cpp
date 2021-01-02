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
#include "vreng.hpp"
#include "texture.hpp"
#include "http.hpp"	// httpOpen
#include "img.hpp"	// Img
#include "format.hpp"	// getLoaderByMime
#include "thread.hpp"	// Vpthread_mutex_t
#include "pref.hpp"	// quality3D
#include "wobject.hpp"	// WObject

#include <list>
using namespace std;

/* local variables */
static list<Texture*> textureList;
static Img *default_img = NULL;
static Vpthread_mutex_t texture_mutex, *tex_pmutex = &texture_mutex;
static GLuint last_texid = 0;


void Texture::initCache()
{
  default_img = Img::init();
  initMutex(tex_pmutex);
}

void Texture::closeCache()
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
        error("updateTextures pixmap=null: n=%d w=%d h=%d u=%s", (*it)->id, (*it)->img->width, (*it)->img->height, (*it)->url);
        continue;
      }
      glBindTexture(GL_TEXTURE_2D, (*it)->id);

      if (! (*it)->img->wellSized()) { /* image to resize */
        Img *img1 = NULL;
        if ((img1 = (*it)->img->resize(Img::SIDE_SIZE, Img::SIDE_SIZE)) == NULL) {
          //error("updateTextures: n=%d u=%s", (*it)->id, (*it)->url);
          continue;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, 3, img1->width, img1->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, img1->pixmap);
        if (img1) delete img1;
      }
      else {	/* image well sized */
        if (! (*it)->img->nummipmaps) {
          glTexImage2D(GL_TEXTURE_2D, 0, 3, (*it)->img->width, (*it)->img->height, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, (*it)->img->pixmap);
        }
        else {
          GLsizei mipWidth = (*it)->img->width;
          GLsizei mipHeight = (*it)->img->height;
          int blockSize = ((*it)->img->channel == Img::RGB) ? 8 : 16;
          int offset = 0;

          /* setup some parameters for texture filters and mipmapping */
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          /* upload mipmaps to video memory */
          for (GLint mip = 0; mip < (*it)->img->nummipmaps; ++mip) {
            GLsizei mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * blockSize;

            glCompressedTexImage2D(GL_TEXTURE_2D, mip, (*it)->img->channel,
                                   mipWidth, mipHeight, 0, mipSize,
                                   (*it)->img->pixmap + offset);
            mipWidth = MAX(mipWidth >> 1, 1);
            mipHeight = MAX(mipHeight >> 1, 1);
            offset += mipSize;
          }
        }
      }
      // once the texture resized we can delete its container img
      if ((*it)->loaded) {
        if ((*it)->img) delete (*it)->img; // opengl has its own copy of pixels
        (*it)->img = NULL;
      }
    }
  }
  //-------------- unlock
  unlockMutex(tex_pmutex);
}

int readImg(void *_tex, char *buf, int len)
{
  Texture *tex = (Texture *) _tex;

  return tex->http->httpRead(buf, len);        // return read length
}

void Texture::httpReader(void *_tex, Http *_http)
{
  if (! _http) return; // error while opening
  Texture *tex = (Texture *) _tex;

  tex->http = _http;
  trace(DBG_IMG, "texture: mime=%s url=%s", tex->mime, tex->url);

  Img *img = NULL;

  // get the loader type by mime type or file extension
  uint8_t loader = Format::getLoaderByMime(tex->mime);
  switch (loader) {
  case IMG_NULL:
    loader = Format::getLoaderByUrl(tex->url);
    break;
  }

  // call the appropriated loader
  switch (loader) {
  case IMG_GIF: img = Img::loadGIF(tex, readImg); break;
  case IMG_PNG: img = Img::loadPNG(tex, readImg); break;
  case IMG_JPG: img = Img::loadJPG(tex, readImg); break;
  case IMG_PPM: img = Img::loadPPM(tex, readImg); break;
  case IMG_PGM: img = Img::loadPGM(tex, readImg); break;
  case IMG_BMP: img = Img::loadBMP(tex, readImg); break;
  case IMG_TGA: img = Img::loadTGA(tex, readImg); break;
  case IMG_SGI: img = Img::loadSGI(tex, readImg); break;
  case IMG_PCX: img = Img::loadPCX(tex, readImg); break;
  case IMG_DDS: img = Img::loadDDS(tex, readImg); break;
  case IMG_TIF: img = Img::loadTIF(tex, readImg); break;
  case IMG_XPM: img = Img::loadXPM(tex, readImg); break;
  case IMG_XBM: img = Img::loadXBM(tex, readImg); break;
  case IMG_PSD: img = Img::loadPSD(tex, readImg); break;
  case IMG_NULL: return;
  default:
    warning("texture: unrecognized image format");
    return;
  }

  lockMutex(tex_pmutex);
  tex->img = img;
  tex->loaded = true;
  unlockMutex(tex_pmutex);
}

Texture::Texture(const char *url)
{
  if (! check(url)) return;

  id = 0;
  http = NULL;
  img = NULL;
  loaded = false;
  *mime = '\0';

  strcpy(this->url, url);
  textureList.push_back(this);
  id = getid();
  last_texid = id;
  //error("%d %s", id, url);
  // load image
  switch (Format::getLoaderByUrl((char*) url)) {
    case IMG_GIF: Http::httpOpen(url, httpReader, this, 1); break; // multi-threaded
    default:      Http::httpOpen(url, httpReader, this, 0); break;
  }
  new_texture++;
}

Texture::Texture()
{
  new_texture++;
  id = 0;
  http = NULL;
  img = NULL;
  loaded = false;
  *mime = '\0';

  textureList.push_back(this);
}

Texture::~Texture()
{
  del_texture++;
}

bool Texture::check(const char *url)
{
  if (! url) { error("getFromCache: url NULL"); return false; }
  if (! isprint(*url)) {
    error("getFromCache: url not printable");
    for (int i=0; i<16; i++) fprintf(stderr, "%02x ", url[i]);
    error("");
    return false;
  }
  return true;
}

GLuint Texture::exist(const char *url)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it) {
    if (! strcmp((*it)->url, url)) {	/* texture is yet in the cache */
      return (*it)->id;
    }
  }
  return 0;
}

GLuint Texture::get(const char *url, WObject *wo)
{
  GLuint texid = getFromCache(url);
  if (texid)
    this->object = wo;
  return texid;
}

GLuint Texture::getFromCache(const char *url)
{
  if (! check(url)) return 0;

/*dax is util ?
  GLuint texid = exist(url);
  if (texid) return texid;
*/

  Texture * texture = new Texture();	// new entry in cache
  strcpy(texture->url, url);
  texture->id = getid();		// creates texture and return texid
  last_texid = texture->id;
  //trace(DBG_IMG, "texture: id=%d %s", texture->id, url);

  /* we must download the texture now */
  switch (Format::getLoaderByUrl((char*) url)) {
  case IMG_GIF: Http::httpOpen(url, httpReader, texture, 1); break; // multi-threaded
  default:      Http::httpOpen(url, httpReader, texture, 0); break;
  }
  return texture->id;
}

GLuint Texture::getid()
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

Texture * Texture::getEntryByUrl(const char *url)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
    if (! strcmp((*it)->url, url)) return *it;	// texture is in the cache
  /* we will download the texture later */
  return NULL;
}

Texture * Texture::getEntryById(GLuint texid)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
    if ((*it)->id == texid) return *it;	// texture is in the cache
  return NULL;
}

GLuint Texture::getIdByUrl(const char *url)
{
  getFromCache(url);
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
    if (! strcmp((*it)->url, url)) return (*it)->id;
  return 0;
}

GLuint Texture::getIdByObject(WObject *wo)
{
  //dax Texture::listTextures();
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
    if ((*it)->object == wo) return (*it)->id;
  return 0;
}

char * Texture::getUrlById(GLuint texid)
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
    if ((*it)->id == texid) return (*it)->url;	// texture is in the cache
  return NULL;
}

void Texture::setMime(char *p)
{
  if (strlen(p) < MIME_LEN) strcpy(mime, p);
  else {
    strncpy(mime, p, MIME_LEN-1);
    mime[MIME_LEN-1] = 0;
    error("mime type too long = %s", p);
  }
}

void Texture::listTextures()
{
  for (list<Texture*>::iterator it = textureList.begin(); it != textureList.end(); ++it)
    error("%d %s", (*it)->id, (*it)->url);
  return;
}
