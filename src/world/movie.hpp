//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// movie.hpp
//---------------------------------------------------------------------------
#ifndef MOVIE_HPP
#define MOVIE_HPP

#include "object.hpp"

#define MOVIE_TYPE	25
#define MOVIE_NAME	"Movie"

#include <mpeg.h>	// ImageDesc


/**
 * Movie class
 */
class Movie: public Object {
 private:
  static const float FPS;	///< max number of frames decoded per rendering
  static const float DEF_PROJ;	///< distance between projector and screen

  float rate;			///< asked avi/mpg frames per second
  float fps;			///< coded frames per sec
  GLuint texid;			///< texture id
  uint16_t texsiz;		///< texture size in pixels
  uint16_t frame;		///< the current frame being displayed
  uint16_t width;		///< video width in pixels
  uint16_t height;		///< video height in pixels
  uint8_t vidfmt;		///< video format (mpg | avi)
  bool anim;			///< animated or not
  bool begin;			///< first frame begin playing
  GLfloat dist;			///< distance between projector and screen
  V3 dim;			///< dimension of screen projection
  FILE *fp;			///< the corresponding file pointer
  uint8_t *videobuf;		///< buffer of the video
  GLubyte *texframe;		///< pixmap of the texture
  ImageDesc *mpg;		///< this is a structure used by mpeg_lib to store infos
  char *line;			///< line copy
  class Avi *avi;		///< avi instance
  class File *file;		///< file mpg instance
  class Spot *spot;		///< Spot instance

 public:
  enum {
    INACTIVE,	// to indicate
    PLAYING,	// the screen
    PAUSE,	// state
    LOOP = 16	// Loop or stop at end of film
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Movie(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Creates from file line. */

  void permanent(float lasting);
  /**< Change permanent (frames). */

  void quit();

 private:
  void parser(char *l);
  /**< Parses vre line. */

  void defaults();
  /**< Sets default values. */

  void open_mpg();
  void open_avi();
  void init_tex();
  void play_mpg();
  void play_avi();
  void bind_frame();
  void inits();
  /**< Inits. */

  // Actions
  void play();
  void stop();
  void pause();
  void rewind();
  void loop();

  // GUI callbacks
  static void play_cb(Movie *po, void *d, time_t s, time_t u);
  static void stop_cb(Movie *po, void *d, time_t s, time_t u);
  static void pause_cb(Movie *po, void *d, time_t s, time_t u);
  static void loop_cb(Movie *po, void *d, time_t s, time_t u);
  static void rewind_cb(Movie *po, void *d, time_t s, time_t u);
};

#endif
