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
#ifndef MODEL_HPP
#define MODEL_HPP

#include "wobject.hpp"

#define MODEL_TYPE 	46
#define MODEL_NAME 	"Model"


/**
 * Model class
 */
class Model: public WObject {

 private:
  static const float LSPEED; ///< default linear speed
  static const float DEF_SCALE; ///< default scale

  float lspeed;         ///< linear speed.
  float aspeed;         ///< angular speed.
  uint8_t model_t;	///< type of model.
  float scale;		///< scale to apply.
  bool taken;		///< taken or not by user.
  bool rendered;	///< rendered or not by user.
  bool transform;	///< transform or not.
  int texid;		///< texture number.
  char *texurl;		///< texture url.
  char *sndurl;		///< sound url.
  char *bvhurl;		///< bvh url.
  float color[3];	///< main color.

  // model pointers
  class Lwo  *lwo;	///< Lwo Model pointer.
  class _3ds *ds3;	///< 3ds Model pointer.
  class Ase  *ase;	///< Ase Model pointer.
  class Obj  *obj;	///< Obj Model pointer.
  class Md2  *md2;	///< Md2 Model pointer.
  class Dxf  *dxf;	///< Dxf Model pointer.
  class Off  *off;	///< Off Model pointer.
  class X3d  *x3d;	///< X3d Model pointer.
  class Bvh  *bvh;	///< Bvh Model pointer.

 public:
  static const OClass oclass;	///< class variable.

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Model(char *l);	///< Constructor.

  Model(WObject *user, char *_url, float _scale);     ///< Constructor from GUI.

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  virtual bool isMoving();
  /**< Checks if is moving. */

  virtual void changePermanent(float lasting);
  /**< Imposed movement. */

  virtual void quit();
  /**< Quits. */

  virtual void render();
  virtual void render(const Pos &pos);
  virtual void render(const Pos &pos, float *color);
  /**< Renders the model. */

 private:
  virtual void parser(char *l);
  /**< Parses file line. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void makeSolid();
  /**< Builds solid geometry. */

  virtual void setName();
  /**< Sets an implicited name. */

  virtual void behavior();
  /**< Sets behavior flags. */

  virtual void loader();
  /**< Loads the model. */

  virtual void scaler();
  /**< Scales or not the model. */

  virtual void drawer();
  /**< Draws the model. */

  virtual void preRender();
  /**< Makes translations ans rotations. */

  virtual void postRender();
  /**< Makes glPopMatrix. */

  // GUI callbacks
  static void sound(Model *o, void *d, time_t s, time_t u);
};

#endif
