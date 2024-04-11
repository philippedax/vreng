//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
#include "str.hpp"
#include "format.hpp"


static const struct _file imgs[] = {
  { EXT_GIF,  MIME_GIF,  IMG_GIF },
  { EXT_PNG,  MIME_PNG,  IMG_PNG },
  { EXT_JPG,  MIME_JPEG, IMG_JPG },
  { EXT_JPEG, MIME_JPEG, IMG_JPG },
  { EXT_PPM,  MIME_PPM,  IMG_PPM },
  { EXT_PGM,  MIME_PGM,  IMG_PGM },
  { EXT_BMP,  MIME_BMP,  IMG_BMP },
  { EXT_TGA,  MIME_TGA,  IMG_TGA },
  { EXT_PCX,  MIME_PCX,  IMG_PCX },
  { EXT_XBM,  MIME_XBM,  IMG_XBM },
  { EXT_XPM,  MIME_XPM,  IMG_XPM },
  { EXT_RGB,  MIME_RGB,  IMG_SGI },
  { EXT_RGBA, MIME_RGBA, IMG_SGI },
  { EXT_LA,   MIME_LA,   IMG_SGI },
  { EXT_BW,   MIME_BW,   IMG_SGI },
  { EXT_TIF,  MIME_TIF,  IMG_TIF },
  { EXT_TIFF, MIME_TIF,  IMG_TIF },
  { EXT_SVG , MIME_SVG,  IMG_SVG },
  { EXT_DDS,  MIME_DDS,  IMG_DDS },
  { EXT_DXT1, MIME_DDS,  IMG_DDS },
  { EXT_DXT2, MIME_DDS,  IMG_DDS },
  { EXT_DXT3, MIME_DDS,  IMG_DDS },
  { EXT_DXT4, MIME_DDS,  IMG_DDS },
  { EXT_DXT5, MIME_DDS,  IMG_DDS },
  { EXT_PSD,  MIME_PSD,  IMG_PSD },
  { EXT_PHP,  MIME_PLAIN,IMG_NULL },
  { NULL,     NULL,      0 }
};

static const struct _file models[] = {
  { EXT_VNP,  MIME_VNP,  MODEL_OFF },
  { EXT_OFF,  MIME_OFF,  MODEL_OFF },
  { EXT_LWO,  MIME_LWO,  MODEL_LWO },
  { EXT_3DS,  MIME_3DS,  MODEL_3DS },
  { EXT_ASE,  MIME_ASE,  MODEL_ASE },
  { EXT_OBJ,  MIME_OBJ,  MODEL_OBJ },
  { EXT_MD2,  MIME_MD2,  MODEL_MD2 },
  { EXT_MD3,  MIME_MD3,  MODEL_MD3 },
  { EXT_MDL,  MIME_MDL,  MODEL_MDL },
  { EXT_DXF,  MIME_DXF,  MODEL_DXF },
  { EXT_X3D,  MIME_X3D,  MODEL_X3D },
  { EXT_V3D,  MIME_V3D,  MODEL_V3D },
  { NULL,     NULL,      0 }
};

static const struct _icons icons[] = {
  { EXT_HTML, ICO_HTML, PLAYER_WEB },
  { EXT_HTM,  ICO_HTML, PLAYER_WEB },
  { EXT_XML,  ICO_XML,  PLAYER_WEB },
  { EXT_URL,  ICO_URL,  PLAYER_WEB },
  { EXT_TXT,  ICO_TXT,  PLAYER_WEB },
  { EXT_PHP,  ICO_PHP,  PLAYER_WEB },
  { EXT_GIF,  ICO_GIF,  PLAYER_WEB },
  { EXT_JPG,  ICO_JPG,  PLAYER_WEB },
  { EXT_JPEG, ICO_JPG,  PLAYER_WEB },
  { EXT_TIF,  ICO_TIF,  PLAYER_WEB },
  { EXT_PNG,  ICO_PNG,  PLAYER_WEB },
  { EXT_PDF,  ICO_PDF,  PLAYER_PDF },
  { EXT_PS,   ICO_PS,   PLAYER_PS },
  { EXT_DOC,  ICO_WORD, PLAYER_OFFICE },
  { EXT_XLS,  ICO_XLS,  PLAYER_OFFICE },
  { EXT_RTF,  ICO_RTF,  PLAYER_OFFICE },
  { EXT_PPT,  ICO_PPT,  PLAYER_OFFICE },
  { EXT_MP2,  ICO_MP3,  PLAYER_MP3 },
  { EXT_MP3,  ICO_MP3,  PLAYER_MP3 },
  { EXT_WAV,  ICO_AU,   PLAYER_WAV },
  { EXT_AU,   ICO_AU,   PLAYER_AU },
  { EXT_AVI,  ICO_AVI,  PLAYER_AVI },
  { EXT_MPG,  ICO_MPG,  PLAYER_MPG },
  { EXT_MIDI, ICO_MIDI, PLAYER_MIDI },
  { EXT_MID,  ICO_MIDI, PLAYER_MIDI },
  { EXT_C,    ICO_C,    PLAYER_WEB },
  { EXT_H,    ICO_H,    PLAYER_WEB },
  { EXT_CC,   ICO_CPP,  PLAYER_WEB },
  { EXT_CPP,  ICO_CPP,  PLAYER_WEB },
  { EXT_HPP,  ICO_H,    PLAYER_WEB },
  { EXT_HH,   ICO_H,    PLAYER_WEB },
  { EXT_SMIL, ICO_SMIL, PLAYER_WEB },
  { EXT_SVG,  ICO_SVG,  PLAYER_WEB },
  { EXT_TGZ,  ICO_TGZ,  PLAYER_WGET },
  { EXT_TAR,  ICO_TGZ,  PLAYER_WGET },
  { EXT_TARGZ,ICO_TGZ,  PLAYER_WGET },
  { NULL,     ICO_TXT,  PLAYER_WEB }
};


uint8_t Format::getLoaderByMime(const char *mime)
{
  const struct _file *t = imgs;
  for ( ; t->ext ; t++) {
    if (!strcmp(mime, t->mime))
      break;
  }
  return t->loader;
}

uint8_t Format::getLoaderByUrl(char *url)
{
  char *p = url + strlen(url)-1;
  const struct _file *t = imgs;

  while (p>url && *p != '.') p--;
  if (*p == '.') {
    p++;
    for ( ; t->ext ; t++) {
      if (!strcmp(p, t->ext))
        break;
    }
  }
  return t->loader;
}

/** Returns a loader by mime */
uint8_t Format::getModelByMime(const char *mime)
{
  const struct _file *t = models;
  for ( ; t->ext ; t++) {
    if (!strcmp(mime, t->mime))
      break;
  }
  return t->loader;
}

/** Returns a loader by url */
uint8_t Format::getModelByUrl(char *url)
{
  char *p = url + strlen(url)-1;
  const struct _file *t = models;

  while (p>url && *p != '.') p--;
  if (*p == '.') {
    p++;
    for ( ; t->ext ; t++) {
      if (!strcmp(p, t->ext))
        break;
    }
  }
  return t->loader;
}

/** Returns a player */
uint8_t Format::getPlayerByUrl(char *url)
{
  char *p = url + strlen(url)-1;
  const struct _icons *t = icons;

  while (p>url && *p != '.') p--;
  if (*p == '.') {
    p++;
    for ( ; t->ext ; t++) {
      if (!strcmp(p, t->ext))
        break;
    }
  }
  return t->player;
}

/** fills ext */
bool Format::getExt(char *str, char *ext)
{
  char *p, *pdot = NULL;

  if (str[strlen(str) - 1] == '/') {
    strcpy(ext, EXT_HTML);
  }
  else if ((p = strrchr(str, '.')) == 0)
    return false;
  else {
    p++;
    if (! stringcmp(p, EXT_GZ)) {
      pdot = --p;
      *pdot = 0;
      if ((p = strrchr(str, '.')) == 0)
        return false;
      else {
        p++;
        strncpy(ext, p, sizeof(ext) - 1);
        *pdot = '.';
      }
    }
    else {
      strncpy(ext, p, sizeof(ext) - 1);
    }
  }
  return true;
}

/** fills img */
void Format::getImgByExt(const char *ext, char *img)
{
  const struct _icons *t = icons;
  for ( ; t->ext ; t++) {
    if (!strcasecmp(t->ext, ext))
      break;
  }
  strcpy(img, t->img);
  return;
}

uint8_t Format::getPlayerByExt(const char *ext)
{
  const struct _icons *t = icons;
  for ( ; t->ext ; t++) {
    if (!strcasecmp(t->ext, ext))
      break;
  }
  return t->player;
}
