//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#ifndef GUY_HPP
#define GUY_HPP

#include "wobject.hpp"

class Http;

#define GUY_TYPE 34
#define GUY_NAME "Guy"


#define CYCLES		50
#define MAX_JOINTS	5
#define MAX_POINTS	34	// 2 end point ones and 10 in the middle
#define HEAD_R		0.08	// d=16
#define NECK_H		0.05	//
#define NECK_R		0.04	//
#define BUST_H		0.60	//
#define BUST_W		0.22	//
#define BUST_L		0.52	//
#define ULEG_H		0.37	//
#define LLEG_H		0.37	//
#define UARM_H		0.33	//
#define LARM_H		0.33	//
#define ANKLE_R		0.03	//
#define FOOT_H		0.11	//
#define HIP_R		0.09	// d=18
#define SHOULDER_R	0.04	// d=8
#define KNEE_R		0.05	// d=10
#define ELBOW_R		0.03	// d=6
#define WRIST_R		0.03	// d=6
#define BREA_R		0.06	// d


typedef struct {
  int numpoints;
  float coords[MAX_POINTS];
  float angles[MAX_POINTS];
} tGuyCtrlPts;


/**
 * Guy class
 */
class Guy: public WObject {

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

  static uint16_t RATE;
  static uint8_t GUY_PARTS;
  static uint8_t OVERSAMPLE;
  static const char DEF_URL_GUY[];
  static const float BUST_COLOR[];
  static const float LEGS_COLOR[];
  static const float SKIN_COLOR[];
  static const float FEET_COLOR[];

  uint8_t stp;		///< position in cycle, start in middle
  float incstp;		///< step increment
  GLint dlist;		///< guy displaylist
  bool sex;		///< sex toggle 0=male 1=female
  bool walking;		///< walk toggle
  bool animing;		///< anim toggle
  bool showing;		///< showing flag
  bool flying;		///< flying flag
  bool control;		///< control by user
  uint8_t numjoints;	///< joinpoint number
  tGuyCtrlPts *curve;	///< series of ctrl points for a joint
  float skin_color[4];  ///< skin color
  float bust_color[4];  ///< bust color
  float legs_color[4];  ///< legs color
  float feet_color[4];  ///< feet color
  float cycles[2][MAX_JOINTS][CYCLES]; ///< array of angles

public:
  static const OClass oclass;	///< class variable.

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Guy(char *l);		///< Constructor from file.

  Guy();		///< Constructor from localuser (avatar).

  static WObject * (creator)(char *l);

  virtual void changePermanent(float lasting);

  virtual void render();
  /**< Specific rendering. */

  virtual void quit();
  /**< Quits properly. */

  virtual void setAniming(bool flag);
  /**< Set animing flag. */

  virtual void setFlying(bool flag);
  /**< Set flying flag. */

  virtual void setShowing(bool flag);
  /**< Set showing flag. */

private:
  virtual void parser(char *l);
  /**< Parses file line. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void makeSolid();
  /**< Builds geometry. */

  virtual void behavior();
  /**< Sets behavior. */

  virtual void inits();
  /**< Do specific initializations. */

  virtual void draw();
  /**< Draws the body. */

  virtual void setPose();
  /**< unused */

  virtual const char * getUrl() const;

  static void httpReader(void *oa, Http *http);
  /**< Reads cset files. */

  virtual void computeCurve(uint8_t joint);
  /**< Computes cset curves. */

  virtual void draw_bust();
  virtual void draw_neck();
  virtual void draw_brea();
  virtual void draw_head();
  virtual void draw_uleg();
  virtual void draw_lleg();
  virtual void draw_foot();
  virtual void draw_uarm();
  virtual void draw_larm();

  virtual void display_body();
  /**< Displays the body. */

  virtual void display_bust();
  virtual void display_neck();
  virtual void display_brea(bool side);
  virtual void display_head();
  virtual void display_leg(bool side);
  virtual void display_arm(bool side);

  // GUI callbacks
  static void animate_cb(Guy *o, void *d, time_t s, time_t u);
  static void walking_cb(Guy *o, void *d, time_t s, time_t u);
};

#endif
