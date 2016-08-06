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
#ifndef MUSIC_HPP
#define MUSIC_HPP

#include "wobject.hpp"

#define MUSIC_TYPE	24
#define MUSIC_NAME	"Music"


/**
 * Music class
 */
class Music: public WObject {
friend class Model;	// play

private:
  uint8_t fmt;		///< audio format
  char statestr[8];	///< action string

public:
  bool repeat;	///< repeat flag

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

  virtual const OClass* getOClass() {return &oclass;}	///<virtual inst. method

  static void funcs();	///< init funclist

  Music(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Create from file */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< Intersects with another object */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position */

  virtual void quit();
  /**< Quits properly */

private:
  virtual void parser(char *l);
  /**< Parses line */

  virtual void defaults();
  /**< Sets default values */

  virtual int format();
  /**< Gets audio format */

  // GUI callbacks
  static void play(Music *o, void *d, time_t s, time_t u);
  static void stop(Music *o, void *d, time_t s, time_t u);
  static void pause(Music *o, void *d, time_t s, time_t u);
  static void cont(Music *o, void *d, time_t s, time_t u);
  static void loop(Music *o, void *d, time_t s, time_t u);
};

#endif
