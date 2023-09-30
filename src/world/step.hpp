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
#ifndef STEP_HPP
#define STEP_HPP

#include "wobject.hpp"
#include <vector>

#define STEP_TYPE	6
#define STEP_NAME	"Step"

/**
 * Step class
 */
class Step: public WO {

 protected:
  static const float LSPEED;	///< linear speed
  static vector<Step*> stepList;	///< vector of steps (stair)
  static vector<Step*> escaList;	///< vector of steps (escalator)
  static vector<Step*> travList;	///< vector of steps (travelator)

  bool mobile;		///< flag escalator or travelator
  bool escalator;	///< flag escalator
  bool travelator;	///< flag travelator
  bool stair;		///< flag stair
  bool spiral;		///< flag spiral staircase
  bool stuck;		///< flag if user is on escalator/travelator
  int dir;		///< direction up=1 or down=-1 or horizontal=0
  float height;		///< escalator height
  float length;		///< travelator length
  float speed;		///< linear speed of escalator/travelator
  uint8_t nsteps;	///< number of steps
  Pos initialpos;	///< initial step position
  Step *nextstep;	///< list of steps

 public:
  static const float JUMP;	///< jump

  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Step() {}		///< Constructor
  Step(char *l);	///< Constructor

  Step(WO *user, char *form);	///< Constructor from GUI

  Step(Pos& newpos, Pos& _initialpos, const char *name, const char *geom, bool _mobile, float _height, float _speed, int _dir);
  /**< Constructor for structure of steps */

  static WO * (creator)(char *l);
  /**< Create from fileline */

  bool whenIntersect(WO *pcur, WO *pold);
  /**< When an other object intersects */

  bool whenIntersectOut(WO *pcur, WO *pold);
  /**< When an other object leaves intersection */

  void updateTime(time_t s, time_t us, float *lasting);
  /**< Updates lasting time */

  void changePermanent(float lasting);
  /**< Permanent movement */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position */

  void running();
  /**< Mecanism is running */

  void pause();
  /**< Pauses/Continues */

  virtual void quit();
  /**< Quits */

protected:
  // Gui callbacks
  static void pause_cb(Step *po, void *d, time_t s, time_t u);

private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Default values */

  virtual void behaviors();
  /**< Sets behaviors */

  virtual void build();
  /**< Builds stair or escalator */
};

#endif
