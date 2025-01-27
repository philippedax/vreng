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
// music.hpp
//---------------------------------------------------------------------------
#ifndef MUSIC_HPP
#define MUSIC_HPP

#include "wobject.hpp"

#define MUSIC_TYPE	24
#define MUSIC_NAME	"Music"


/**
 * Music class
 */
class Music: public Object {
 private:
  uint8_t fmt;		///< audio format
  bool repeat;		///< repeat flag
  char statestr[8];	///< action string

 public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number
  };

  /* audio formats */
  enum {
    FMT_NONE,
    FMT_MP3,
    FMT_MIDI,
    FMT_AU,
    FMT_WAV
  };

  /* states */
  enum {
    INACTIVE,
    PLAYING,
    PAUSING,
    LOOPING
  };

  /* actions */
  enum {
    PLAY,
    STOP,
    PAUSE,
    CONT,
    LOOP,
    MOVE
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}	///<inst. method

  static void funcs();	///< init funclist

  Music(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Create from file */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< Intersects with another object */

  void quit();
  /**< Quits properly */

 private:
  void parser(char *l);
  /**< Parses line */

  void defaults();
  /**< Sets default values */

  int format();
  /**< Gets audio format */

  // GUI callbacks
  static void play(Music *o, void *d, time_t s, time_t u);
  static void stop(Music *o, void *d, time_t s, time_t u);
  static void pause(Music *o, void *d, time_t s, time_t u);
  static void cont(Music *o, void *d, time_t s, time_t u);
  static void loop(Music *o, void *d, time_t s, time_t u);
};

#endif
