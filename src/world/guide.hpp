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
// guide.hpp
//---------------------------------------------------------------------------
#ifndef GUIDE_HPP
#define GUIDE_HPP

#include "object.hpp"

#define GUIDE_TYPE	48
#define GUIDE_NAME	"guide"


/**
 * Guide class
 */
class Guide: public Object {
 private:
  static const uint8_t GUIDE_DIM;	///< max size of the path
  static const float GUIDE_DELTAZ;	///< step in Z
  static const float GUIDE_COLOR[3];	///< path's default color

  bool testing;		///< flag mode testing
  bool oneway;		///< flag mode one-way/round-trip
  bool show;		///< flag show path
  bool stuck;		///< flag inside/outside
  bool restored;	///< flag restored
  uint8_t segs;		///< max segment number in this path
  uint8_t seg;		///< current segment number in this path
  GLint dlist;		///< display list

 public:
  bool pause;		///< flag pause continue

  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Guide(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Create from fileline. */

  void updateTime(time_t sec, time_t usec, float *lasting);

  void changePermanent(float lasting);
  /**< Perpetually movement. */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< When an other object intersects. */

  bool whenIntersectOut(Object *pcur, Object *pold);
  /**< When an object leaves intersection. */

  bool publish(const Pos &oldpos);
  /**< Publishes new position. */

  void render();
  /**< Renders the trajectory. */

  void quit();
  /**< Quits. */

 private:
  void parser(char *l);
  /**< Parses. */

  void defaults();
  /**< Sets default values. */

  void behaviors();
  /**< Sets behaviors of the guide. */

  void stuckUser();
  /**< Sets User on the guide. */

  void motion(float *dx, float *dy, float *dz);
  /**< Returns deltas. */

  void progress(Object *po);
  /**< Progress along the guide. */

  void draw(float *color);
  /**< Draws the trajectory. */

  void restore();
  /**< Restores initial positions. */

  /** Callbacks from GUI */
  static void visit_cb(Guide *o, void *d, time_t s, time_t u);
  static void showhide_cb(Guide *o, void *d, time_t s, time_t u);
  static void pausecontinue_cb(Guide *o, void *d, time_t s, time_t u);
  static void test_cb(Guide *o, void *d, time_t s, time_t u);
  static void reset_cb(Guide *o, void *d, time_t s, time_t u);
};

#endif
