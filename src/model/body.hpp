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
  static const GLfloat PHALANX2_LEN;

 public:
  GLfloat length;		///< phalanx length

  Phalanx2();			///< constructor
  virtual ~Phalanx2() {}	///< destructor

  virtual int init();
  /**< Init Phalanx2 */
};

/**
 * Phalanx class
 */
class Phalanx {
 private:
  static const GLfloat PHALANX_LEN;

 public:
  GLfloat length;		///< phalanx length
  GLfloat aflexion;		///< phalanx flexion angle
  class Phalanx2 *phalanx2;	///< phalanx2 ptr

  Phalanx(Phalanx2 *phalanx2);	///< constructor
  virtual ~Phalanx() {}		///< destructor

  virtual int init();
  /**< Init phalanx */

  virtual void flexion(GLfloat a);
  /**< Bend phalanx */
};

/**
 * Finger class
 */
class Finger {
 private:
  static const GLfloat FINGER_LEN;

 public:
  GLfloat length;		///< finger lenght
  GLfloat aflexion;		///< finger flexion angle
  GLfloat aabduct;		///< finger abduct angle
  class Phalanx *phalanx;	///< phalanx ptr

  Finger(Phalanx *phalanx);	///< constructor
  virtual ~Finger() {}		///< destructor

  virtual int init();
  /**< Init finger */

  virtual void flexion(GLfloat a);
  /**< Bend finger */

  virtual void abduct(GLfloat a);
  /**< Tilt finger */
};

/**
 * Hand class // Wrist
 */
class Hand {
 private:
  static const GLfloat HAND_LEN;

 public:
  GLfloat length;		///< hand length
  GLfloat aflexion;		///< hand flexion angle
  GLfloat aabduct;		///< hand abduct angle
  GLfloat atorsion;		///< hand torsion angle
  class Finger *fingers[5];	///< five fingers

  Hand(Finger **finger);	///< constructor
  virtual ~Hand() {}		///< destructor

  virtual int init();
  /**< Init hand */

  virtual void flexion(GLfloat a);
  /**< Bend hand */

  virtual void abduct(GLfloat a);
  /**< Tilt hand */

  virtual void torsion(GLfloat a);
  /**< Turn hand */
};

/**
 * Forearm class // Elbow
 */
class Forearm {
 private:
  static const GLfloat FOREARM_LEN;
  static const GLfloat ELBOW_RAD;

 public:
  GLfloat length;		///< forearm length
  GLfloat aflexion;		///< elbow flexion angle
  GLfloat atorsion;		///< elbow torsion angle
  class Hand *hand;		///< hand ptr

  Forearm(Hand *hand);		///< constructor
  virtual ~Forearm() {}		///< destructor

  virtual int init();
  /**< Init forearm */

  virtual void flexion(GLfloat a);
  /**< Bend elbow */

  virtual void torsion(GLfloat a);
  /**< Turn elbow */
};

/**
 * Arm class
 */
class Arm {
 private:
  static const GLfloat ARM_LEN;

 public:
  GLfloat length;		///< arm length
  GLfloat aflexion;		///< arm flexion angle
  GLfloat aabduct;		///< arm abduct angle
  GLfloat atorsion;		///< arm torsion angle
  class Forearm *forearm;	///< forearm ptr

  Arm(Forearm *forearm);	///< constructor
  virtual ~Arm() {}		///< destructor

  virtual int init();
  /**< Init arm */

  virtual void abduct(GLfloat a);
  /**< Raise arm */

  virtual void flexion(GLfloat a);
  /**< Rote arm */

  virtual void torsion(GLfloat a);
  /**< Turn arm */
};

/**
 * Shoulder class
 */
class Shoulder {
 private:
  static const GLfloat SHOULDER_RAD;

 public:
  GLfloat radius;		///< shoulder radius
  GLfloat aflexion;		///< shoulder flexion angle
  GLfloat aabduct;		///< shoulder abduct angle
  class Arm *arm;		///< arm ptr

  Shoulder(Arm *arm);		///< constructor
  virtual ~Shoulder() {}	///< destructor

  virtual int init();
  /**< Init shoulder */

  virtual void flexion(GLfloat distance);
  /**< Avance shoulder */

  virtual void abduct(GLfloat distance);
  /**< Raise shoulder */
};

/**
 * HeadBody class
 */
class HeadBody {
 private:
  static const GLfloat HEAD_RAD;

 public:
  GLfloat radius;		///< head radius
  GLfloat aflexion;		///< head flexion angle
  GLfloat aabduct;		///< head abduct angle
  GLfloat atorsion;		///< head torsion angle

  HeadBody();			///< constructor
  virtual ~HeadBody() {}	///< destructor

  virtual int init();
  /**< Init head */

  virtual void flexion(GLfloat a);
  /**< Bend head */

  virtual void abduct(GLfloat a);
  /**< Tilt head */

  virtual void torsion(GLfloat a);
  /**< Turn head */
};

/**
 * Neck class
 */
class Neck {
 private:
  static const GLfloat NECK_LEN;

 public:
  GLfloat length;		///< neck length
  GLfloat aflexion;		///< neck flexion angle
  GLfloat aabduct;		///< neck abduct angle
  GLfloat atorsion;		///< neck torsion angle
  class HeadBody *head;		///< head ptr

  Neck(HeadBody *head);		///< constructor
  virtual ~Neck() {}		///< destructor

  virtual int init();
  /**< Init neck */

  virtual void flexion(GLfloat a);
  /**< Bend neck */

  virtual void abduct(GLfloat a);
  /**< Tilt neck */

  virtual void torsion(GLfloat a);
  /**< Turn neck */
};

/**
 * Foot class // Ankle
 */
class Foot {
 private:
  static const GLfloat FOOT_LEN;

 public:
  GLfloat length;		///< foot length
  GLfloat aflexion;		///< foot flexion angle
  GLfloat aabduct;		///< foot abduct angle
  GLfloat atorsion;		///< foot torsion angle

  Foot();			///< constructor
  virtual ~Foot() {}		///< destructor

  virtual int init();
  /**< Init foot */

  virtual void flexion(GLfloat a);
  /**< Bend foot */

  virtual void abduct(GLfloat a);
  /**< Tilt foot */

  virtual void torsion(GLfloat a);
  /**< Turn foot */
};

/**
 * Shin class // Knee
 */
class Shin {
 private:
  static const GLfloat SHIN_LEN;

 public:
  GLfloat length;		///< shin length
  GLfloat aflexion;		///< knee flexion angle
  GLfloat atorsion;		///< knee torsion angle
  class Foot *foot;		///< foot ptr

  Shin(Foot *foot);		///< constructor
  virtual ~Shin() {}		///< destructor

  virtual int init();
  /**< Init shin */

  virtual void flexion(GLfloat a);
  /**< Bend knee */

  virtual void torsion(GLfloat a);
  /**< Turn knee */
};

/**
 * Thigh class // Hip
 */
class Thigh {
 private:
  static const GLfloat THIGH_LEN;

 public:
  GLfloat length;		///< thigh length
  GLfloat aflexion;		///< thigh flexion angle
  GLfloat aabduct;		///< thigh abduct angle
  GLfloat atorsion;		///< thigh torsion angle
  class Shin *shin;		///< shin ptr

  Thigh(Shin *shin);		///< constructor
  virtual ~Thigh() {}		///< destructor

  virtual int init();
  /**< Init thigh */

  virtual void abduct(GLfloat a);
  /**< Raise thigh */

  virtual void flexion(GLfloat a);
  /**< Rote thigh */

  virtual void torsion(GLfloat a);
  /**< Turn thigh */
};

/**
 * Chest class
 */
class Chest {
 private:
  static const GLfloat CHEST_LEN;

 public:
  GLfloat length;		///< chest length
  GLfloat aflexion;		///< chest flexion angle
  GLfloat aabduct;		///< chest abduct angle
  GLfloat atorsion;		///< chest torsion angle

  Chest();			///< constructor
  //Chest(Thigh **thighs, Shoulder **shoulders, Neck *neck);
  virtual ~Chest() {}		///< destructor

  //virtual int init(class Body *body);
  /**< Init chest */

  virtual void flexion(GLfloat a);
  /**< Roll chest */

  virtual void abduct(GLfloat a);
  /**< Tilt chest */

  virtual void torsion(GLfloat a);
  /**< Turn chest */
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
  int num;		///< part number
};

/**
 * Joint-Points structure
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
  GLfloat tx, ty, tz;		///< body translations
  GLfloat rx, ry, rz;		///< body rotations
  GLfloat bscale;		///< body scale factor
  GLfloat skin[3];		///< skin color
  GLfloat cloth[3];		///< cloth color
  GLfloat color[3];		///< color
  char *url;

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
  class WObject *wobject;	///< wobject ptr.

  Body();
  Body(const char *url);	///< constructor.

  virtual ~Body();		///< destructor.

  virtual void init();
  /**< Inits body. */

  virtual void animate();
  /**< Animates body. */

  virtual void anim(int param);
  /**< Animates body. */

  virtual void render(Pos& pos);
  /**< Renders body and face. */

#if 0 //dax OK
  virtual void loadBodyParts(class Http *http);
#else
  virtual void loadBodyParts(FILE *f);
#endif
  /**< Loads body's drawable parts. */

  //virtual void setJointPoint(uint8_t indice, float *_jp);
  /**< Sets one joint point for this indice. */

  virtual void setColors(float *_skin, float *_cloth);
  /**< Sets colors for skin and cloth. */

protected:
  virtual void load(const char *url);
  /**< Loads body joint points. */

  static void httpReader(void *o, class Http *http);
  /**< Reads joint points par Http. */

  virtual void draw();
  /**< Draws body's drawable parts in displaylists. */

  virtual void display();
  /**< Displays body and face. */

  virtual void display(uint8_t part);
  /**< Displays a part of the body. */

  virtual bool isLoaded(uint8_t part);
  /**< Returns true if body part exists. */

  virtual void jpTrans(uint8_t part);
  /**< Translates to the Joint Point. */

  virtual void jpUntrans(uint8_t part);
  /**< Translates reverse from the Joint Point. */

  virtual char * getTok(char *l, int *tok);
  virtual char * skipEqual(char *l);
  virtual char * getUrl() const;

};


#endif
