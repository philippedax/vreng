//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
// 
// Copyright (C) 1997-2012 Philippe Dax
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
// avi.cpp
//
// AVI Loader (Audio Video Interleave)
//
// Some utilities for writing and reading AVI files.
// These are not intended to serve for a full blown
// AVI handling software (this would be much too complex)
// The only intention is to write out MJPEG encoded
// AVIs with sound and to be able to read them back again.
//
// Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de>
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "avi.hpp"
#include "http.hpp"	// Http
#include "cache.hpp"	// open, close
#include "endian.hpp"	// bigEndian


void Avi::defaults()
{
  fp = NULL;
  url = NULL;
  cache = NULL;
  fps = 0;
  width = 0;
  height = 0;
}

Avi::Avi()
{
  defaults();
}

Avi::Avi(const char *_url)
{
  defaults();
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
  Http::httpOpen(url, reader, this, 0);
}

Avi::~Avi()
{
  if (url) delete[] url;
  if (cache) {
    cache->close();
    delete cache;
  }
}

void Avi::getInfos(uint16_t *_width, uint16_t *_height, float *_fps) const
{
  if (width) *_width = width;
  if (height) *_height = height;
  if (fps) *_fps = static_cast<float>(fps);
}

void Avi::reader(void *_avi, Http *http)
{     
  Avi *avi = static_cast<Avi *>(_avi);
  if (! avi) return;
        
  Cache *cache = new Cache();
  avi->fp = cache->open(http->url, http);
}

FILE * Avi::getFile() const
{
  return fp;
}

/**
 * Return codes:
 *    0 = OK
 *    1 = reached EOF
 *    2 = not a AVI file
 */
int32_t Avi::read_header()
{
  int32_t tag, len, nam;
  int32_t lasttag = 0;
  int32_t vids_strh_seen = 0;
  int32_t vids_strf_seen = 0;
  uint32_t args[64];

  /* Read first 12 bytes and check that this is an AVI file */
  if (fread(&tag, 4, 1, fp) != 1) return 1;
  if (fread(&len, 4, 1, fp) != 1) return 1;
  if (fread(&nam, 4, 1, fp) != 1) return 1;
  if (tag != RIFFtag || nam != MAKEFOURCC('A','V','I',' ')) {
    error("avi: not a AVI file");
    return ERR_NO_AVI;
  }

  /* Read all tags until we encounter a 'LIST xxx movi' tag */
  while (1) {
    if (fread(&tag, 4, 1, fp) != 1) return 1;
    if (fread(&len, 4, 1, fp) != 1) return 1;
    if (Endian::bigEndian()) {
      char tmp;
      SWAPL(&len, tmp);
    }
    if (len&1) len++; /* Odd values are padded */
    //echo("avi: tag=%08x len=%d", tag, len);
    /* Unless a LIST tag hasn't name movi, it is ignored */
    if (tag == LISTtag) {
      if (fread(&nam, 4, 1, fp) != 1) return 1;
      //echo("avi: nam=%08x", nam);
      if (nam == MAKEFOURCC('m','o','v','i')) {
        //echo("avi: movi");
        return 0;
      }
      continue;
    }

    /* Read the arguments of this tag, 256 bytes are sufficient, remainder is skipped */
    if (len <= 256) {
      if (fread(args, len, 1, fp) != 1) return 1;
    }
    else {
      if (fread(args, 256, 1, fp) != 1) return 1;
      if (fseek(fp, len-256, SEEK_CUR)) return 1;
    }

    /* Interpret the tag and its args */
    switch (tag) {
      case strhtag:
        if (args[0] == MAKEFOURCC('v','i','d','s')) {
          if (args[1] != MAKEFOURCC('M','J','P','G') &&
              args[1] != MAKEFOURCC('m','j','p','g') &&
              args[1] != MAKEFOURCC('M','S','V','C') &&
              args[1] != MAKEFOURCC('m','s','v','c')) {
            error("avi: args1 no mjpg %08x", args[1]);
            return ERR_NO_MJPG;
          }
          if (Endian::bigEndian()) {
            char t;
            SWAPL(&args[5], t);
            SWAPL(&args[6], t);
          }
          if (args[5] != 0) fps = (double)args[6] / (double)args[5];
          vids_strh_seen = 1;
          lasttag = 1; /* vids */
        }
        else if (args[0] == MAKEFOURCC('a','u','d','s')) lasttag = 2; /* auds */
        break;
      case strftag:
        if (lasttag == 1) {
          if (Endian::bigEndian()) {
            char t;
            SWAPL(&args[1], t);
            SWAPL(&args[2], t);
          }
          width  = args[1];
          height = args[2];
          //echo("avi: width=%d height=%d", width, height);
          if (args[4] != MAKEFOURCC('M','J','P','G') &&
              args[4] != MAKEFOURCC('m','j','p','g') &&
              args[4] != MAKEFOURCC('C','R','A','M') &&
              args[4] != MAKEFOURCC('c','r','a','m') &&
              args[4] != MAKEFOURCC('M','S','V','C') &&
              args[4] != MAKEFOURCC('m','s','v','c')) {
            error("avi: args4 no mjpg %08x", args[4]);
            return ERR_NO_MJPG;
          }
          vids_strf_seen = 1;
        }
        break;
      default:
        lasttag = 0;
        break;
    }
  }
  if (!vids_strh_seen || !vids_strf_seen) return ERR_NO_VIDS;
  return 0;
}

/**
 * Return codes:
 *    1 = video data read
 *    0 = reached EOF
 *   -1 = video buffer too small
 */
int Avi::read_data(uint8_t *vidbuf, uint32_t max_vid, int32_t *retlen)
{
  uint32_t tag, len;

  while (fp) {
    /* Read tag and length */
    if (fread(&tag, 4, 1, fp) != 1 ||
        fread(&len, 4, 1, fp) != 1 ) {
      error("avi: read error");
      fp = NULL;
      return -1;
    }
    if (Endian::bigEndian()) {
      char t;
      SWAPL(&len, t);
    }
    if (len&1) len++; /* Odd values are padded */

    switch (tag) {
      case MAKEFOURCC('0','0','d','b'):
      case MAKEFOURCC('0','0','d','c'):
        if (len > max_vid) {
          error("avi: video buffer too small len=%d", len);
          fp = NULL;
          return -1;	// ERR
        }
        *retlen = len;
        if (fread(vidbuf, len, 1, fp) != 1) {
          error("avi: eof! len=%d", len);
          fp = NULL;
          return 0;	// EOF
        }
        return 1;	// OK
        break;
      default:
        if (fseek(fp, len, SEEK_CUR)) {
          fp = NULL;
          return 0;	// EOF
        }
        break;
    }
  }
  return 0;
}

#if 0 //notused

int Avi::read_data(uint8_t *vidbuf, uint32_t max_vid, uint8_t *audbuf, uint32_t max_aud, int32_t *retlen)
{
  uint32_t tag;
  uint32_t n;

  while (fp) {
    /* Read tag and length */
    if (fread(&tag, 4, 1, fp) != 1 ||
        fread(&n, 4, 1, fp) != 1 ) {
      //error("avi: read error");
      fp = NULL;
      return 0;
    }
#if WORDS_BIGENDIAN
    char tmp;
    SWAPL(&n, tmp);
#endif
    if (n&1) n++; /* Odd values are padded */

    switch (tag) {
      case MAKEFOURCC('0','0','d','b'):
      case MAKEFOURCC('0','0','d','c'):
        if (n > (uint32_t) max_vid) {
          error("avi: video buffer too small n=%d", n);
          fp = NULL;
          return -1;
        }
        *retlen = n;
        if (fread(vidbuf, n, 1, fp) != 1) {
          error("avi: eof! n=%d", n);
          fp = NULL;
          return 0;
        }
        return 1;
        break;
      case MAKEFOURCC('0','1','w','b'):
        if (n > (uint32_t) max_aud) return -2;
        *retlen = n;
        if (fread(audbuf, n, 1, fp) != 1) return 0;
        return 2;
        break;
      default:
        if (fseek(fp, n, SEEK_CUR)) {
          error("avi: ignore tag=%08x n=%d", tag, n);
          fp = NULL;
          return 0;
        }
        break;
    }
  }
  return 0;
}
#endif //notused
