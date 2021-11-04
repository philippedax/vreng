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
#ifndef MOVIE_HPP
#define MOVIE_HPP

#include "wobject.hpp"

#define MOVIE_TYPE	25
#define MOVIE_NAME	"Movie"


#if HAVE_LIBMPEG
// Warning! there is a typedef conflict with Boolean type
// between mpeg.h and X11/Intrinsic.h
#define Boolean Fuck_Boolean_X11
#include <mpeg.h>
#endif

/**
 * Movie class
 */
class Movie: public WObject {
private:
  static const float FPS;	///< max number of frames decoded per rendering

  float rate;			///< asked avi/mpeg frames per second
  float fps;			///< coded frames per sec
  GLuint texid;			///< texture id
  uint16_t width;		///< mpeg width in pixels
  uint16_t height;		///< mpeg height in pixels
  uint16_t texsiz;		///< texture size in pixels
  uint16_t frame;		///< the current frame being displayed
  uint8_t vidfmt;		///< video format (mpg | avi)
  bool begin;			///< first frame begin playing
  FILE *fp;			///< the corresponding file pointer
  uint8_t *vidbuf;		///< buffer of the video
  GLubyte *texmap;		///< pixmap of the texture
  ImageDesc *mpeg;		///< this is a structure used by mpeg_lib to store infos
  class Avi *avi;		///< avi instance

public:
  enum {
    INACTIVE,	// to indicate
    PLAYING,	// the screen
    PAUSE,	// state
    LOOP = 16	// Loop or stop at end of film
  };

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Movie(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from file line. */

  virtual void changePermanent(float lasting);
  /**< Change permanent (frames). */

private:
  virtual void parser(char *l);
  /**< Parses vre line. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void inits();
  /**< Inits. */

  virtual void aviInit();
  /**< Inits Avi. */

  virtual void mpegInit();
  /**< Inits Mpeg. */

  // GUI callbacks
  static void play(Movie *po, void *d, time_t s, time_t u);
  static void stop(Movie *po, void *d, time_t s, time_t u);
  static void pause(Movie *po, void *d, time_t s, time_t u);
  static void rewind(Movie *po, void *d, time_t s, time_t u);
  static void loop(Movie *po, void *d, time_t s, time_t u);
  static void debug(Movie *po, void *d, time_t s, time_t u);
};

#endif
