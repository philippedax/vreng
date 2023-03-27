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
#ifndef HUMAN_HPP
#define HUMAN_HPP

#include "wobject.hpp"	// Pos

#define HUMAN_TYPE 84
#define HUMAN_NAME "Human"

struct _tMaterial {
  float ambient[3];
  float diffuse[3];
  float specular[3];
  float emission[3];
  float alpha;
  float shininess;
  int texture;
};

struct _tBody {
  uint8_t part;         // part number
  uint16_t offset;      // relative offset from faces
  uint16_t vcount;      // vertices count
  uint8_t mat;          // material index
};

enum {
  MAN_FOREARM_R,
  MAN_FOREARM_L,
  MAN_THIGH_R,
  MAN_THIGH_L,
  MAN_ELBOW_R,
  MAN_ARM_R,
  MAN_ARM_L,
  MAN_ELBOW_L,
  MAN_SHOULDER_R,
  MAN_HIP_R,
  MAN_HIP_L,
  MAN_SHOULDER_L,
  MAN_FOOT_R,
  MAN_FOOT_L,
  MAN_ANKLE_R,
  MAN_ANKLE_L,
  MAN_SHIN_R,
  MAN_SHIN_L,
  MAN_KNEE_R,
  MAN_KNEE_L,
  MAN_BELT,
  MAN_PELVIC,
  MAN_HAIRS,
  MAN_HEAD,
  MAN_CHEST,
  MAN_NECK,
  MAN_HAND_R,
  MAN_HAND_L,
  MAN_WRIST_R,
  MAN_WRIST_L,
  BODYPARTS
};


/**
 * Human class
 */
class Human : public WObject {

public:
  Pos pos;		///< position and BB

  Human();		///< constructor from user

  Human(char *l);	///< constructor from e vre file

  virtual ~Human();	///< destructor

  void draw();

  static const OClass oclass;   ///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();  ///< init funclist.

  static WObject * (creator)(char *l);

  void changePermanent(float lasting);
  
  void render();
  /**< Specific rendering. */

  GLint displaylist();

  void quit();  
  /**< Quits properly. */

  void armShowing(int *dir);

private:
  float width;
  float depth;
  float height;
  bool usercontrol;	///< control by localuser
  GLint dlist;		///< display list

  void parser(char *l);
  /**< Parses file line. */
  
  void behavior();
  /**< Sets behavior. */
  
  void makeSolid();
  /**< Makes solid. */

  static void myMaterial(GLenum mode, float *f, float alpha);

  static void setMaterial(int i);
};

#endif
