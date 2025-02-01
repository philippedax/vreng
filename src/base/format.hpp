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
// format.hpp
//---------------------------------------------------------------------------
#ifndef FORMAT_HPP
#define FORMAT_HPP


/* file extensions */
#define EXT_GIF		"gif"
#define EXT_TIF		"tif"
#define EXT_TIFF	"tiff"
#define EXT_JPG		"jpg"
#define EXT_JPEG	"jpeg"
#define EXT_PNG		"png"
#define EXT_BMP		"bmp"
#define EXT_PPM		"ppm"
#define EXT_PGM		"pgm"
#define EXT_PCX		"pcx"
#define EXT_XBM		"xbm"
#define EXT_XPM		"xpm"
#define EXT_PHP		"php"
#define EXT_TGA		"tga"
#define EXT_RGB		"rgb"
#define EXT_RGBA	"rgba"
#define EXT_BW		"bw"
#define EXT_LA		"la"
#define EXT_DDS		"dds"
#define EXT_DXT1	"dxt1"
#define EXT_DXT2	"dxt2"
#define EXT_DXT3	"dxt3"
#define EXT_DXT4	"dxt4"
#define EXT_DXT5	"dxt5"
#define EXT_VRE		"vre"
#define EXT_VNP		"vnp"
#define EXT_OFF		"off"
#define EXT_LWO		"lwo"
#define EXT_3DS		"3ds"
#define EXT_ASE		"ase"
#define EXT_OBJ		"obj"
#define EXT_X3D		"x3d"
#define EXT_V3D		"v3d"
#define EXT_MD2		"md2"
#define EXT_MD3		"md3"
#define EXT_MDL		"mdl"
#define EXT_DXF		"dxf"
#define EXT_MAN		"man"
#define EXT_AC3D	"ac3d"
#define EXT_TXF		"txf"
#define EXT_HTML	"html"
#define EXT_HTM		"htm"
#define EXT_XML		"xml"
#define EXT_CSS		"css"
#define EXT_PDF		"pdf"
#define EXT_URL		"url"
#define EXT_TXT		"txt"
#define EXT_PS		"ps"
#define EXT_PSD		"psd"
#define EXT_GZ		"gz"
#define EXT_DOC		"doc"
#define EXT_XLS		"xls"
#define EXT_PPT		"ppt"
#define EXT_RTF		"rtf"
#define EXT_MP2		"mp2"
#define EXT_MP3		"mp3"
#define EXT_WAV		"wav"
#define EXT_AU		"au"
#define EXT_AVI		"avi"
#define EXT_MP4		"mp4"
#define EXT_MPG		"mpg"
#define EXT_MPEG	"mpeg"
#define EXT_MID		"mid"
#define EXT_MIDI	"midi"
#define EXT_SMIL	"smil"
#define EXT_SVG		"svg"
#define EXT_TAR		"tar"
#define EXT_TGZ		"tgz"
#define EXT_TARGZ	"tar.gz"
#define EXT_JAVA	"java"
#define EXT_JAR		"jar"
#define EXT_C		"c"
#define EXT_H		"h"
#define EXT_CC		"cc"
#define EXT_HH		"hh"
#define EXT_CPP		"cpp"
#define EXT_HPP		"hpp"
#define EXT_PL		"pl"
#define EXT_PY		"py"
#define EXT_VRE		"vre"
#define EXT_BAP		"bap"
#define EXT_FAP		"fap"
#define EXT_BVH		"bvh"

/* MIME subtypes */
#define MIME_GIF	"gif"
#define MIME_TIF	"tiff"
#define MIME_TIFF	"tiff"
#define MIME_JPG	"jpg"
#define MIME_JPEG	"jpeg"
#define MIME_PNG	"png"
#define MIME_BMP	"bmp"
#define MIME_AU		"basic"
#define MIME_TGA	"x-tga"
#define MIME_PPM	"x-portable-pixmap"
#define MIME_PGM	"x-portable-graymap"
#define MIME_XBM	"x-xbitmap"
#define MIME_XPM	"x-xpixmap"
#define MIME_PCX	"x-pcx"
#define MIME_PHP	"x-php"
#define MIME_RGB	"x-rgb"
#define MIME_RGBA	"x-rgba"
#define MIME_LA		"x-la"
#define MIME_BW		"x-bw"
#define MIME_DDS	"x-dds"
#define MIME_PSD	"x-psd"
#define MIME_VRE	"x-vre"
#define MIME_MD2	"x-md2"
#define MIME_MD3	"x-md3"
#define MIME_MDL	"x-mdl"
#define MIME_VNP	"x-vnp"
#define MIME_OFF	"x-off"
#define MIME_LWO	"x-lwo"
#define MIME_3DS	"x-3ds"
#define MIME_ASE	"x-ase"
#define MIME_OBJ	"x-obj"
#define MIME_X3D	"x3d+xml"
#define MIME_V3D	"v3d"
#define MIME_DXF	"x-dxf"
#define MIME_AC3D	"x-ac3d"
#define MIME_TXF	"x-txf"
#define MIME_WAV	"x-wav"
#define MIME_SVG	"svg+xml"
#define MIME_VLC	"x-vlc"
#define MIME_PLAIN	"plain"

/* image paths */
#define ICO_DEF		"/icon/txt.gif"
#define ICO_HTML	"/icon/html.gif"
#define ICO_XML		"/icon/xml.gif"
#define ICO_CSS		"/icon/css.gif"
#define ICO_URL		"/icon/url.gif"
#define ICO_TXT		"/icon/txt.gif"
#define ICO_PHP		"/icon/php.gif"
#define ICO_GIF		"/icon/gif.gif"
#define ICO_TIF		"/icon/tiff.gif"
#define ICO_JPG		"/icon/jpg.gif"
#define ICO_PNG		"/icon/png.gif"
#define ICO_PDF		"/icon/pdf.gif"
#define ICO_PS		"/icon/ps.gif"
#define ICO_WORD	"/icon/word.gif"
#define ICO_XLS		"/icon/xls.gif"
#define ICO_PPT		"/icon/ppt.gif"
#define ICO_RTF		"/icon/rtf.gif"
#define ICO_MP3		"/icon/mp3.gif"
#define ICO_MPG		"/icon/mpg.gif"
#define ICO_MIDI	"/icon/midi.gif"
#define ICO_C		"/icon/c.gif"
#define ICO_H		"/icon/h.gif"
#define ICO_CPP		"/icon/cpp.gif"
#define ICO_PL		"/icon/pl.gif"
#define ICO_PY		"/icon/py.gif"
#define ICO_SMIL	"/icon/smil.gif"
#define ICO_SVG		"/icon/svg.gif"
#define ICO_TGZ		"/icon/tgz.gif"
#define ICO_AU		"/icon/au.gif"
#define ICO_AVI		"/icon/avi.gif"

/* Image loaders */
enum {
  IMG_NULL,
  IMG_GIF,
  IMG_TIF,
  IMG_JPG,
  IMG_PNG,
  IMG_PPM,
  IMG_PGM,
  IMG_PCX,
  IMG_BMP,
  IMG_TGA,
  IMG_SGI,
  IMG_XBM,
  IMG_XPM,
  IMG_DDS,
  IMG_PSD,
  IMG_SVG,
  IMG_NIL
};

/* Model loaders */
enum {
  MODEL_NULL,
  MODEL_VNP,
  MODEL_OFF,
  MODEL_LWO,
  MODEL_3DS,
  MODEL_ASE,
  MODEL_OBJ,
  MODEL_MD2,
  MODEL_MD3,
  MODEL_MDL,
  MODEL_DXF,
  MODEL_MAN,
  MODEL_AC3D,
  MODEL_X3D,
  MODEL_V3D,
  MODEL_NIL
};

/* Players */
enum {
  PLAYER_WEB,
  PLAYER_PDF,
  PLAYER_PS,
  PLAYER_OFFICE,
  PLAYER_MP3,
  PLAYER_WAV,
  PLAYER_AU,
  PLAYER_MPG,
  PLAYER_MIDI,
  PLAYER_SMI,
  PLAYER_SVG,
  PLAYER_WGET,
  PLAYER_MP4,
  PLAYER_AVI,
  PLAYER_SWF,
  PLAYER_NIL
};

/**
 * file structure
 */
struct _file {
  const char *ext;	///< file extention
  const char *mime;	///< sub type mime
  uint8_t loader;	///< loader number
};

/**
 * icon structure
 */
struct _icons {
  const char *ext;	///< file extention
  const char *img;	///< image url
  uint8_t player;	///< player number
};

/**
 * Format class
 */
class Format {
 public:
  static uint8_t getModelByUrl(char *url);
  static uint8_t getModelByMime(const char *str);
  static uint8_t getPlayerByUrl(char *url);
  static uint8_t getPlayerByExt(const char *str);
  static bool    getExt(char *str, char *ext);
  static void    getImgByExt(char *str, char *img);
  static uint8_t getLoaderByUrl(char *url);
  //notused static uint8_t getLoaderByMime(const char *str);

};

#endif
