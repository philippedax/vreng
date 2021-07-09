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
#ifndef GUIDE_HPP
#define GUIDE_HPP

#include "wobject.hpp"

#define GUIDE_TYPE	48
#define GUIDE_NAME	"guide"


/**
 * Guide class
 */
class Guide: public WObject {

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

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Guide(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Create from fileline */

  virtual void updateTime(time_t sec, time_t usec, float *lasting);

  virtual void changePermanent(float lasting);
  /**< Perpetually movement */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an other object intersects */

  virtual bool whenIntersectOut(WObject *pcur, WObject *pold);
  /**< When an object leaves intersection */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position */

  virtual void render();
  /**< Renders the trajectory */

  virtual void quit();
  /**< Quits */

private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< init default values */

  virtual void initUser();
  /**< Sets User on the guide */

  virtual void motion(float *dx, float *dy, float *dz);
  /**< Returns deltas */

  virtual void progress(WObject *po);
  /**< Progress along the guide */

  virtual void draw(float *color);
  /**< Draws the trajectory */

  virtual void restore();
  /**< Restores initial positions */

  /** Callbacks from GUI */
  static void visit(Guide *o, void *d, time_t s, time_t u);
  static void showhide(Guide *o, void *d, time_t s, time_t u);
  static void pausecontinue(Guide *o, void *d, time_t s, time_t u);
  static void test(Guide *o, void *d, time_t s, time_t u);
  static void reset(Guide *o, void *d, time_t s, time_t u);
};

#endif
