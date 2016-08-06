//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// Copyright (c) 2005-2006 David HENRY
//
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
#include "img.hpp"
#include "cache.hpp"	// openCache
#include "file.hpp"	// closeFile
#include "texture.hpp"	// Texture


#ifndef _WIN32
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif /* GL_GLEXT_PROTOTYPES */
#else
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2D = NULL;
#endif /* _WIN32 */


/* DDS texture info */
typedef struct {
  GLsizei width;
  GLsizei height;
  GLenum channel;
  GLint internal_channel;
  GLuint id;
  GLubyte *texels;
  GLint numMipmaps;
  FILE *fp;
} dds_t;

/* DirectDraw's structures */
typedef struct {
  GLuint size;
  GLuint flags;
  GLuint fourCC;
  GLuint bpp;
  GLuint redMask;
  GLuint greenMask;
  GLuint blueMask;
  GLuint alphaMask;
} DDPixelFormat;

typedef struct {
  GLuint caps;
  GLuint caps2;
  GLuint caps3;
  GLuint caps4;
} DDSCaps;

typedef struct {
  GLuint lowVal;
  GLuint highVal;
} DDColorKey;

typedef struct {
  GLuint size;
  GLuint flags;
  GLuint height;
  GLuint width;
  GLuint pitch;
  GLuint depth;
  GLuint mipMapLevels;
  GLuint alphaBitDepth;
  GLuint reserved;
  GLuint surface;
  DDColorKey ckDestOverlay;
  DDColorKey ckDestBlt;
  DDColorKey ckSrcOverlay;
  DDColorKey ckSrcBlt;
  DDPixelFormat channel;
  DDSCaps caps;
  GLuint textureStage;
} DDSurfaceDesc;

#define MAKEID(ch0, ch1, ch2, ch3) \
  (GLuint)( \
    (((GLuint)(GLubyte)(ch0) << 24) & 0xFF000000) | \
    (((GLuint)(GLubyte)(ch1) << 16) & 0x00FF0000) | \
    (((GLuint)(GLubyte)(ch2) <<  8) & 0x0000FF00) | \
     ((GLuint)(GLubyte)(ch3)        & 0x000000FF) )

#define DXT1 MAKEID('D', 'X', 'T', '1')
#define DXT3 MAKEID('D', 'X', 'T', '3')
#define DXT5 MAKEID('D', 'X', 'T', '5')

#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif


/** DDS loader */
Img * Img::loadDDS(void *tex, ImageReader read_func)
{
  DDSurfaceDesc ddsd;
  char magic[4];
  long size, curr, end;

  const char *glexts = (const char *)glGetString (GL_EXTENSIONS);

  /* check for S3TC support */
  if (!strstr (glexts, "GL_EXT_texture_compression_s3tc")) {
    error("GL_EXT_texture_compression_s3tc is required for DDS textures!");
    return NULL;
  }
  /* init extensions */
#ifdef _WIN32
  glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)
    wglGetProcAddress ("glCompressedTexImage2DARB");
#endif

  dds_t *dds = new dds_t[1];
  if (! dds) return NULL;
  memset(dds, 0, sizeof(dds_t));

  Texture *_tex = (Texture *) tex;
  if ((dds->fp = Cache::openCache(_tex->url, _tex->http)) == NULL) return NULL;

  /* read magic number and check if valid .dds file */
  fread(&magic, sizeof(char), 4, dds->fp);
  if (strncmp (magic, "DDS ", 4) != 0) {
    error("file \"%s\" is not a valid .dds file!", Cache::getFileName(_tex->url));
    File::closeFile(dds->fp);
    return NULL;
  }

  /* get the surface descriptor */
  fread(&ddsd, sizeof(ddsd), 1, dds->fp);

  dds->width = ddsd.width;
  dds->height = ddsd.height;
  dds->numMipmaps = ddsd.mipMapLevels;

  trace(DBG_IMG, "dds: width=%d height=%d numMipmaps=%d",
                  dds->width, dds->height, dds->numMipmaps);

  switch (ddsd.channel.fourCC) {
  case DXT1:
    dds->channel = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    dds->internal_channel = Img::RGB;
    break;
  case DXT3:
    dds->channel = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    dds->internal_channel = Img::RGBA;
    break;
  case DXT5:
    dds->channel = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    dds->internal_channel = Img::RGBA;
    break;
  default:
    error("the file \"%s\" doesn't appear to be compressed using DXT1, DXT3, or DXT5! [%x]",
          Cache::getFileName(_tex->url), ddsd.channel.fourCC);
    File::closeFile(dds->fp);
    delete[] dds;
    return NULL;
  }

  /* calculate pixel data size */
  curr = ftell(dds->fp);
  fseek(dds->fp, 0, SEEK_END);
  end = ftell(dds->fp);
  fseek(dds->fp, curr, SEEK_SET);
  size = end - curr;

  /* alloc the data */
  Img *img = new Img(dds->width, dds->height, Img::RGB, dds->numMipmaps, size);
  dds->texels = img->pixmap;

  /* read pixel data with mipmaps */
  fread(dds->texels, sizeof (GLubyte), size, dds->fp);

  File::closeFile(dds->fp);
  delete[] dds;

  return img;
}
