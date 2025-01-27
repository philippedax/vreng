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
// clip.hpp
//---------------------------------------------------------------------------
#ifndef CLIP_HPP
#define CLIP_HPP

#include "wobject.hpp"

#define CLIP_TYPE	26
#define CLIP_NAME	"Clip"

/**
 * Clip class
 */
class Clip: public Object {
 private:
  uint8_t fmt;		///< video format
  char url2[URL_LEN];	///< url where is video

 public:
  /* video formats */
  enum {
    FMT_MPEG,
    FMT_RTPAV,
    FMT_MPEG4,
    FMT_FLASH
  };

  /* states */
  enum {
    RTP_INACTIVE,
    RTP_ACTIVE
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Clip(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Create from fileline */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< When an other object intersects */

  void quit();
  /**< Quits */

  void stopRtp();
  /**< Stops Rtp streams */

 private:
  void parser(char *l);
  /**< Parses */

  void startRtp();
  /**< Starts Rtp streams */

  void pauseRtp();
  /**< Suspends Rtp streams */

  void contRtp();
  /**< Resumes Rtp streams */

  // GUI callbacks
  static void play(Clip *o, void *d, time_t s, time_t u);
  static void stop(Clip *o, void *d, time_t s, time_t u);
  static void pause(Clip *o, void *d, time_t s, time_t u);
  static void cont(Clip *o, void *d, time_t s, time_t u);
};

#endif
