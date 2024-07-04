//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
// guy.hpp
//---------------------------------------------------------------------------
#ifndef GUY_HPP
#define GUY_HPP

#include "wobject.hpp"

class Http;

#define GUY_TYPE 34
#define GUY_NAME "Guy"


#define CYCLES		50
#define MAX_JOINTS	5	// uleg + lleg + foot + uarm + larm
#define MAX_POINTS	34	// 2 end point and 10 in the middle

// body parts
#define HEAD_R		0.08	// d=16
#define NECK_H		0.07	// 07
#define NECK_R		0.05	// d=10
#define BUST_H		0.60	//
#define BUST_W		0.22	//
#define BUST_L		0.46	// 52
#define ULEG_H		0.37	//
#define LLEG_H		0.33	//
#define UARM_H		0.29	// 33
#define LARM_H		0.31	//
#define ANKLE_R		0.04	// d=8
#define FOOT_H		0.12	//
#define HIP_R		0.08	// d=16
#define SHOULDER_R	0.04	// d=8
#define KNEE_R		0.03	// d=6
#define ELBOW_R		0.03	// d=6
#define WRIST_R		0.03	// d=6
#define BREA_R		0.05	// d=10


typedef struct {
  int numpoints;
  float coords[MAX_POINTS];
  float angles[MAX_POINTS];
} tCset;


/**
 * Guy class
 */
class Guy: public WO {

 private:
  enum {
    BUST,
    NECK,
    HEAD,
    ULEG,
    LLEG,
    FOOT,
    UARM,
    LARM,
    BREA
  };

  enum {
    CSET_ULEG,
    CSET_LLEG,
    CSET_FOOT,
    CSET_UARM,
    CSET_LARM
  };

  enum {
    L_SIDE,	// left
    R_SIDE	// right
  };

  static uint16_t RATE;
  static uint8_t GUY_PARTS;
  static uint8_t OVERSAMPLE;
  static const char DEF_URL_GUY[];
  static const float BUST_COLOR[];
  static const float LEGS_COLOR[];
  static const float SKIN_COLOR[];
  static const float FEET_COLOR[];

  uint8_t step;		///< position in cycle, start in middle
  float incstep;	///< step increment
  GLint dlist;		///< guy displaylist
  bool sex;		///< sex toggle 0=male 1=female
  bool walking;		///< walk toggle
  bool animing;		///< anim toggle
  bool showing;		///< showing flag
  bool flying;		///< flying flag
  bool control;		///< control by user
  uint8_t numjoints;	///< joinpoint number
  tCset *curve;		///< series of ctrl points for a jointpoint
  float skin_color[4];  ///< skin color
  float bust_color[4];  ///< bust color
  float legs_color[4];  ///< legs color
  float feet_color[4];  ///< feet color
  float cycles[2][MAX_JOINTS][CYCLES]; ///< array of angles

 public:
  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Guy(char *l);		///< Constructor from file.
  Guy();		///< Constructor from localuser (avatar).

  static WO * (creator)(char *l);

  void changePermanent(float lasting);

  void render();
  /**< Specific rendering. */

  void quit();
  /**< Quits properly. */

  void setAniming(bool flag);
  /**< Set animing flag. */

  bool isAnim() const;
  /**< Test animing. */

  void setFlying(bool flag);
  /**< Set flying flag. */

  bool isFlying() const;
  /**< Test flying. */

  void setShowing(bool flag);
  /**< Set showing flag. */

 private:
  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

  void geometry();
  /**< Builds geometry. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Do specific initializations. */

  void draw();
  /**< Draws the body. */

  void setPose();
  /**< unused */

  static void reader(void *oa, Http *http);
  /**< Reads cset files. */

  void computeCurve();
  /**< Computes cset curves. */

  void draw_bust();
  void draw_neck();
  void draw_brea();
  void draw_head();
  void draw_uleg();
  void draw_lleg();
  void draw_foot();
  void draw_uarm();
  void draw_larm();

  void render_bust();
  void render_neck();
  void render_brea(bool side);
  void render_head();
  void render_leg(bool side);
  void render_arm(bool side);

  // GUI callbacks
  static void animate_cb(Guy *o, void *d, time_t s, time_t u);
  static void walking_cb(Guy *o, void *d, time_t s, time_t u);
};

#endif
