//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include "wobject.hpp"

#define ANIMATOR_TYPE	38
#define ANIMATOR_NAME	"Animator"


/**
 * Animator class
 */
class Animator: public WO {

protected:
  uint8_t frame;	///< current frame number
  uint8_t nb_frames;	///< number of frames
  float   ttl;		///< time to live

public:
  /* properties */
  enum {
    PROPFRAME,
    PROPNBFRAMES,
    PROPTTL,
    PROPS
  };

  /* states */
  enum {
    INACTIVE,
    ACTIVE
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Animator(char *l);	///< Constructor

  static WO * (creator)(char *l);

  bool isMoving();

  void changePosition(float lasting);

  void updateTime(time_t, time_t, float *);

  bool whenIntersect(WO *pcur, WO *pold);

  void quit();

private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

  // GUI and network property change callbacks
  static void get_frame(Animator *pcur, Payload *pp);
  static void put_frame(Animator *po, Payload *pp);
  static void get_nb_frames(Animator *pcur, Payload *pp);
  static void put_nb_frames(Animator *po, Payload *pp);
  static void get_ttl(Animator *pcur, Payload *pp);
  static void put_ttl(Animator *po, Payload *pp);
  static void start(Animator *po, void *d, time_t s, time_t u);
  static void stop(Animator *po, void *d, time_t s, time_t u);
};

#endif
