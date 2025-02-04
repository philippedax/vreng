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
// step.hpp
//---------------------------------------------------------------------------
#ifndef STEP_HPP
#define STEP_HPP

#include "object.hpp"
#include <vector>

#define STEP_TYPE	6
#define STEP_NAME	"Step"


/**
 * Step class
 */
class Step: public Object {
 protected:
  static const float LSPEED;	///< linear speed
  std::vector<Step*> stairList;	///< vector of steps (stair)
  std::vector<Step*> escaList;	///< vector of steps (escalator)
  std::vector<Step*> travList;	///< vector of steps (travolator)
  bool mobile;			///< flag escalator or travolator
  bool escalator;		///< mode escalator
  bool travolator;		///< mode travolator
  bool stair;			///< mode stair
  bool spiral;			///< mode spiral staircase
  bool stuck;			///< if user follows on escalator/travolator
  int dir;			///< direction up=1 or down=-1 or horizontal=0
  float height;			///< escalator height
  float length;			///< travolator length
  float speed;			///< linear speed of escalator/travolator
  Pos ipos;			///< initial step position

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

  Step(Object *user, char *form);	///< Constructor from GUI

  Step(Pos& newpos, Pos& _ipos, const char *name, const char *geom, bool _mobile, float _height, float _speed, int _dir);
  /**< Constructor for structure of steps */

  static Object * (creator)(char *l);
  /**< Create from fileline */

  bool intersect(Object *pcur, Object *pold);
  /**< When an other object intersects */

  bool intersectOut(Object *pcur, Object *pold);
  /**< When an other object leaves intersection */

  void timing(time_t s, time_t us, float *lasting);
  /**< Updates lasting time */

  void permanent(float lasting);
  /**< Permanent movement */

  bool publish(const Pos &oldpos);
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
  /**< Parses vre line. */

  virtual void defaults();
  /**< Default values. */

  virtual void inits();
  /**< Do initialization. */

  virtual void build();
  /**< Builds stair or escalator structure. */
};

#endif
