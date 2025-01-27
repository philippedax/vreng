//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// hairs.hpp
//---------------------------------------------------------------------------
#ifndef HAIRS_HPP
#define HAIRS_HPP

#define HAIRS_TYPE 	77
#define HAIRS_NAME 	"Hairs"

#include "object.hpp"
#include "vector3.hpp"
#include "lwo.hpp"	// Lwo


#define NBHAIR		100 //1000
#define HAIRSIZE	4 //8
#define HAIRLENGTH	.04 //4
#define HAIRNCOLOR	4
#define LENGTH2		(4. * HAIRLENGTH)
#define MAXL		(1.2 * HAIRLENGTH * 1.2 * HAIRLENGTH)
#define MAXL2		(1.1 * HAIRLENGTH)
#define STIFFNESS	0.1 //0.2
#define STIFF2		0.005 //0.007
#define FROT		0.1 // 0.1
#define HAIRGRAVITY	0.03
#define OBJSIZE		0.1 //0.4 //O 150
#define RANDX		(OBJSIZE*0.02)
#define RANDY		(OBJSIZE*0.02)
#define RANDZ		(OBJSIZE*0.05)
#define PUT_COORD	1
#define PUT_NORMAL	2

template <class T>
inline T rnd(T n) { return (rand()*n)/(RAND_MAX); }


/**
 * Color4f class
 */
class Color4f {
 public:
  union {
    struct { float r,g,b,a; };
    float t[4];
  };

  Color4f() : r(0.),g(0.),b(0.),a(1) {}
  Color4f(float rr, float gg=0.0f, float bb=0.0f, float aa=1.0f) : r(rr),g(gg),b(bb),a(aa) {}
  Color4f operator *(float f) { return Color4f(r*f, g*f, b*f, a); }
  void put();
};

/**
 * HVertex class
 */
class HVertex {
 public:
  Vector3 n;
  Vector3 p;

  void put(int pdata);
};

/**
 * Material class
 */
class Material {
 public:
  Color4f ambient;
  Color4f diffuse;
  Color4f specular;
  Color4f emission;
  int shininess;
  int flag;
  enum {
    Smooth=1,
    Double=2,
    Lighting=4,
    Blend=8,
    Additive=16
  };

  Material() : ambient(.2,.2,.2), diffuse(.8,.8,.8), specular(0,0,0), emission(0,0,0), shininess(0), flag(Smooth) {}
  int put();
  char name[16];
};

/**
 * TriFace class
 */
class TriFace {
 public:
  HVertex *v[3];
  Vector3 n;
  Material *m;

  void normal();
  void put(int pdata);
  void draw(int pdata);
};

/**
 * Surface class
 */
class Surface : public Material {
 public:
  int nbf;
  int nbts; // nb triangle strip
  TriFace *fc;

  Surface() : nbf(0), nbts(0), fc(NULL) {}  ///< Constructor.
  void draw(HVertex *p0=NULL);
};

/**
 * HObject class
 */
class HObject {
 public:
  int nbp, nbf, nbs;
  HVertex *pt;
  TriFace *fc;
  Surface *sf;
  float scal;

  HObject();
  ~HObject();
  void draw();
  Surface * findSurface(const char *name);
  //static bool getValue(FILE *fp, char *s, int *l);
};

/**
 * Node class
 */
class Node {
 public:
  Vector3 p;
  Vector3 v;

  void put(int pdata);
};

/**
 * Line class
 */
class Line {
 public:
  Vector3 n;
  Node pt[HAIRSIZE];

  void init(Vector3 p0, Vector3 dp);
  void draw(int pdata);
};


/**
 * CHair class
 */
class Hair {
 public:
  HObject *o;
  Surface *s;
  Material m;
  Color4f c[HAIRNCOLOR];
  Line ln[NBHAIR];

  Hair() : o(NULL), s(NULL) {};  ///< Constructor.

  void init(HObject *o, Surface *s, float scale);
  /**< Initializations. */

  void draw();
  /**< Draws hair. */

  void animate(float dt, Vector3 depl);
  /**< Animates hair. */
};

/**
 * Hairs class
 */
class Hairs: public Object {

 private:
  Hair *hair;
#if 0 //dax use Lwo
  Lwo *obj;
#else
  HObject *obj;
#endif
  int dlist;    ///< display list
  bool objchanged;
  bool affobj;
  float angle;
  float scale;
  Vector3 posh, pos0, pos1;

 public:
  static const OClass oclass;  ///< class variable.
  const OClass* getOClass() {return &oclass;}

  static void funcs();  ///< init funclist.

  Hairs(char *l);	///< Constructor.

  static Object * (creator)(char *l);
  ///< Creates from fileline.

  void changePermanent(float lasting);
  void render();
  void quit();

 private:
  void parser(char *l);
  /**< Parses. */

  void defaults();
  /**< Sets default values. */

  bool loader(const char *url, float scale);
  /**< Loader LWO model. */

  void draw();
  /**< Draws hairs. */

  static void reader(void *alwo, class Http *http);
};

#endif
