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
#ifndef BODY_HPP
#define BODY_HPP

#include "wobject.hpp"	// Pos
#include "bap.hpp"
#include "fap.hpp"
#include "off.hpp"	// Off
#include "obj.hpp"	// Obj

// Body
#define B_HEIGHT	1.70
#ifdef OFF_SCALING	//see off.hpp
#define B_SCALE		40.41	//0.02476  // 1/40.41
#else
#define B_SCALE		1
#endif


/**
 * Phalanx2 class
 */
class Phalanx2 {
 private:
  static const float PHALANX2_LEN;

 public:
  float length;		///< phalanx length

  Phalanx2();			///< constructor
  virtual ~Phalanx2() {}	///< destructor

  int init();
  /**< Init Phalanx2 */
};

/**
 * Phalanx class
 */
class Phalanx {
 private:
  static const float PHALANX_LEN;

 public:
  float length;		///< phalanx length
  float aflexion;		///< phalanx flexion angle
  class Phalanx2 *phalanx2;	///< phalanx2 ptr

  Phalanx(Phalanx2 *phalanx2);	///< constructor
  virtual ~Phalanx() {}		///< destructor

  int init();
  /**< Init phalanx */

  void flexion(float a);
  /**< Bend phalanx */
};

/**
 * Finger class
 */
class Finger {
 private:
  static const float FINGER_LEN;

 public:
  float length;		///< finger lenght
  float aflexion;		///< finger flexion angle
  float aabduct;		///< finger abduct angle
  class Phalanx *phalanx;	///< phalanx ptr

  Finger(Phalanx *phalanx);	///< constructor
  virtual ~Finger() {}		///< destructor

  int init();
  /**< Init finger */

  void flexion(float a);
  /**< Bend finger */

  void abduct(float a);
  /**< Tilt finger */
};

/**
 * Hand class // Wrist
 */
class Hand {
 private:
  static const float HAND_LEN;

 public:
  float length;		///< hand length
  float aflexion;		///< hand flexion angle
  float aabduct;		///< hand abduct angle
  float atorsion;		///< hand torsion angle
  class Finger *fingers[5];	///< five fingers

  Hand(Finger **finger);	///< constructor
  virtual ~Hand() {}		///< destructor

  int init();
  /**< Init hand */

  void flexion(float a);
  /**< Bend hand */

  void abduct(float a);
  /**< Tilt hand */

  void torsion(float a);
  /**< Turn hand */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};

/**
 * Forearm class // Elbow
 */
class Forearm {
 private:
  static const float FOREARM_LEN;
  static const float ELBOW_RAD;

 public:
  float length;		///< forearm length
  float aflexion;		///< elbow flexion angle
  float atorsion;		///< elbow torsion angle
  class Hand *hand;		///< hand ptr

  Forearm(Hand *hand);		///< constructor
  virtual ~Forearm() {}		///< destructor

  int init();
  /**< Init forearm */

  void flexion(float a);
  /**< Bend elbow */

  void torsion(float a);
  /**< Turn elbow */

  float a_flexion() const;
  float a_torsion() const;
};

/**
 * Arm class
 */
class Arm {
 private:
  static const float ARM_LEN;

 public:
  float length;		///< arm length
  float aflexion;		///< arm flexion angle
  float aabduct;		///< arm abduct angle
  float atorsion;		///< arm torsion angle
  class Forearm *forearm;	///< forearm ptr

  Arm(Forearm *forearm);	///< constructor
  virtual ~Arm() {}		///< destructor

  int init();
  /**< Init arm */

  void flexion(float a);
  /**< Rote arm */

  void abduct(float a);
  /**< Raise arm */

  void torsion(float a);
  /**< Turn arm */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};

/**
 * Shoulder class
 */
class Shoulder {
 private:
  static const float SHOULDER_RAD;

 public:
  float radius;		///< shoulder radius
  float aflexion;		///< shoulder flexion angle
  float aabduct;		///< shoulder abduct angle
  class Arm *arm;		///< arm ptr

  Shoulder(Arm *arm);		///< constructor
  virtual ~Shoulder() {}	///< destructor

  int init();
  /**< Init shoulder */

  void flexion(float distance);
  /**< Avance shoulder */

  void abduct(float distance);
  /**< Raise shoulder */

  float a_flexion() const;
  float a_abduct() const;
};

/**
 * HeadBody class
 */
class HeadBody {
 private:
  static const float HEAD_RAD;

 public:
  float radius;		///< head radius
  float aflexion;		///< head flexion angle
  float aabduct;		///< head abduct angle
  float atorsion;		///< head torsion angle

  HeadBody();			///< constructor
  virtual ~HeadBody() {}	///< destructor

  int init();
  /**< Init head */

  void flexion(float a);
  /**< Bend head */

  void abduct(float a);
  /**< Tilt head */

  void torsion(float a);
  /**< Turn head */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};

/**
 * Neck class
 */
class Neck {
 private:
  static const float NECK_LEN;

 public:
  float length;		///< neck length
  float aflexion;		///< neck flexion angle
  float aabduct;		///< neck abduct angle
  float atorsion;		///< neck torsion angle
  class HeadBody *head;		///< head ptr

  Neck(HeadBody *head);		///< constructor
  virtual ~Neck() {}		///< destructor

  int init();
  /**< Init neck */

  void flexion(float a);
  /**< Bend neck */

  void abduct(float a);
  /**< Tilt neck */

  void torsion(float a);
  /**< Turn neck */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};

/**
 * Foot class // Ankle
 */
class Foot {
 private:
  static const float FOOT_LEN;

 public:
  float length;		///< foot length
  float aflexion;		///< foot flexion angle
  float aabduct;		///< foot abduct angle
  float atorsion;		///< foot torsion angle

  Foot();			///< constructor
  virtual ~Foot() {}		///< destructor

  int init();
  /**< Init foot */

  void flexion(float a);
  /**< Bend foot */

  void abduct(float a);
  /**< Tilt foot */

  void torsion(float a);
  /**< Turn foot */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};

/**
 * Shin class // Knee
 */
class Shin {
 private:
  static const float SHIN_LEN;

 public:
  float length;		///< shin length
  float aflexion;		///< knee flexion angle
  float atorsion;		///< knee torsion angle
  class Foot *foot;		///< foot ptr

  Shin(Foot *foot);		///< constructor
  virtual ~Shin() {}		///< destructor

  int init();
  /**< Init shin */

  void flexion(float a);
  /**< Bend knee */

  void torsion(float a);
  /**< Turn knee */

  float a_flexion() const;
  float a_torsion() const;
};

/**
 * Thigh class // Hip
 */
class Thigh {
 private:
  static const float THIGH_LEN;

 public:
  float length;		///< thigh length
  float aflexion;		///< thigh flexion angle
  float aabduct;		///< thigh abduct angle
  float atorsion;		///< thigh torsion angle
  class Shin *shin;		///< shin ptr

  Thigh(Shin *shin);		///< constructor
  virtual ~Thigh() {}		///< destructor

  int init();
  /**< Init thigh */

  void flexion(float a);
  /**< Rote thigh */

  void abduct(float a);
  /**< Raise thigh */

  void torsion(float a);
  /**< Turn thigh */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};

/**
 * Chest class
 */
class Chest {
 private:
  static const float CHEST_LEN;

 public:
  float length;		///< chest length
  float aflexion;		///< chest flexion angle
  float aabduct;		///< chest abduct angle
  float atorsion;		///< chest torsion angle

  Chest();			///< constructor
  //Chest(Thigh **thighs, Shoulder **shoulders, Neck *neck);
  virtual ~Chest() {}		///< destructor

  //int init(class Body *body);
  /**< Init chest */

  void flexion(float a);
  /**< Roll chest */

  void abduct(float a);
  /**< Tilt chest */

  void torsion(float a);
  /**< Turn chest */

  float a_flexion() const;
  float a_abduct() const;
  float a_torsion() const;
};


/**
 * Body-Parts structure
 */
struct sBodyParts {
  uint8_t model;	///< model type used
  bool loaded;		///< flag loaded or not
  float scale;		///< scale
  float scales[3];	///< scales
  int texid;		///< texture number
  float color[3];	///< color
  class Off *off;	///< Off ptr
  class Obj *obj;	///< Obj ptr
  char url[URL_LEN];	///< url of this part
  char texurl[URL_LEN]; ///< texture url of this part
};

struct sBodyToken {
  char name[16];  	///< name of this part
  uint8_t num;		///< part number
};

/**
 * Join-points structure
 */
struct sJP {
  float *x;
  float *y;
  float *z;
};

/**
 * Body class
 */
class Body {
  friend class Bap;
  friend class Humanoid;
  friend class Human;
  friend class Man;

 protected:

  /* join points numbers */
  enum {
    UPPER_NECK,	//
    LOWER_NECK,	//
    SPINAL,	//
    PELVIC,	//
    L_SHOULDER,	//
    L_ELBOW,	//
    L_WRIST,	//
    R_SHOULDER,	//
    R_ELBOW,	//
    R_WRIST,	//
    L_HIP,	//
    L_KNEE,	//
    L_ANKLE,	//
    R_HIP,	//
    R_KNEE,	//
    R_ANKLE,	//
    MAX_JP
  };

  uint8_t drawparts;		///< number of drawable parts
  uint8_t model;		///< model type used (Off, Obj,...)
  GLint dlist;			///< body display-list
  float tx, ty, tz;		///< body translations
  float rx, ry, rz;		///< body rotations
  float bscale;			///< body scale factor
  float skin[3];		///< skin color
  float cloth[3];		///< cloth color
  float color[3];		///< color
  char *url;			///< body url

  // body parts
  HeadBody *head;		///< 1 head
  Neck     *neck;		///< 1 neck
  Chest    *chest;		///< 1 chest
  Thigh    *thigh_r;		///< right thigh
  Thigh    *thigh_l;		///< left thigh
  Shin     *shin_r;		///< right shin
  Shin     *shin_l;		///< left shin
  Foot     *foot_r;		///< right foot
  Foot     *foot_l;		///< left foot
  Shoulder *shoulder_r;		///< right shoulder
  Shoulder *shoulder_l;		///< left shoulder
  Arm      *arm_r;		///< right arm
  Arm      *arm_l;		///< left arm
  Forearm  *forearm_r;		///< right forearm
  Forearm  *forearm_l;		///< left forearm
  Hand     *hand_r;		///< right hand
  Hand     *hand_l;		///< left hand
  Finger   *fingers_r[5];	///< 5 right fingers
  Finger   *fingers_l[5];	///< 5 left fingers
  Phalanx2 *phalanx2[10];	///< 10 phalanx2
  Phalanx  *phalanx[10];	///< 10 phalanx

  struct sBodyParts *bodyparts; ///< body parts array

public:

  /* body drawable parts */
  enum body_parts {
    HEAD,	// 00
    NECK,	// 01
    CHEST,	// 02
    HIPS,	// 03
    L_ARM,	// 04
    L_FOREARM,	// 05
    L_HAND,	// 06
    R_ARM,	// 07
    R_FOREARM,	// 08
    R_HAND,	// 09
    L_THIGH,	// 10
    L_SHIN,	// 11
    L_FOOT,	// 12
    R_THIGH,	// 13
    R_SHIN,	// 14
    R_FOOT,	// 15
    ABDOMEN,	// 16
    L_COLLAR,	// 17
    R_COLLAR,	// 18
    BELT,	// 19
    L_THUMB,	// 20
    L_INDEX,	// 21
    L_MIDDLE,	// 22
    L_RING,	// 23
    L_PINKY,	// 24
    R_THUMB,	// 25
    R_INDEX,	// 26
    R_MIDDLE,	// 27
    R_RING,	// 28
    R_PINKY,	// 29
    SKIRT,	// 30
    MAX_PARTS	// 31
  };

  struct sJP jp;		///< jointpoints.
  class Bap *bap;		///< bap ptr.
  class Face *face;		///< face ptr.
  class WO *wobject;		///< wobject ptr.

  Body();
  Body(const char *url);	///< constructor.

  virtual ~Body();		///< destructor.

  void init();
  /**< Inits body. */

  void animate();
  /**< Animates body. */

  void animReset();
  void animHead(float deg, uint8_t axis);
  void animNeck(float deg, uint8_t axis);
  void animChest(float deg, uint8_t axis);
  void animLeg(float deg, uint8_t side, uint8_t axis);
  void animShin(float deg, uint8_t side, uint8_t axis);
  void animFoot(float deg, uint8_t side, uint8_t axis);
  void animArm(float deg, uint8_t side, uint8_t axis);
  void animForearm(float deg, uint8_t side, uint8_t axis);
  void animHand(float deg, uint8_t side, uint8_t axis);
  /**x Animates members. */

  void anim(int param);
  /**< Animates body. */

  virtual void render(Pos& pos);
  /**< Renders body and face. */

#if 0 //dax OK
  void loadBodyParts(class Http *http);
#else
  void loadBodyParts(FILE *f);
#endif
  /**< Loads body's drawable parts. */

  void setColors(float *_skin, float *_cloth);
  /**< Sets colors for skin and cloth. */

protected:
  void load(const char *url);
  /**< Loads body joint points. */

  static void reader(void *o, class Http *http);
  /**< Reads joint points. */

  void draw();
  /**< Draws body's drawable parts in displaylists. */

  void display();
  /**< Displays body and face. */

  void display(uint8_t part);
  /**< Displays a part of the body. */

  bool isLoaded(uint8_t part);
  /**< Returns true if body part exists. */

  void transP(uint8_t part);
  /**< Translates to the Joint Point. */

  void transN(uint8_t part);
  /**< Translates reverse from the Joint Point. */

  void rotX(int param, uint8_t model);
  void rotY(int param, uint8_t model);
  void rotZ(int param, uint8_t model);
  void rotX(int param, float angle);
  void rotY(int param, float angle);
  void rotZ(int param, float angle);
  /**< Rotates around the Joint Point. */

  char * getTok(char *l, int *tok);
  char * skipEqual(char *l);
  char * getUrl() const;

};

#endif
