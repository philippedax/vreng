//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
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
  static const float DEF_SCALE; ///< default scale

  uint8_t model_t;	///< type of model.
  float scale;		///< scale to apply.
  bool taken;		///< taken or not by user.
  int texid;		///< texture number.
  float color[3];	///< main color.
  char *texurl;		///< texture url.
  char *bvhurl;		///< bvh url.
  char *sndurl;		///< sound url.

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
  class Man  *man;	///< Man Model pointer.

 public:
  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Model(char *l);	///< Constructor.

  Model(WObject *user, char *_url, float _scale);     ///< Constructor from GUI.

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  bool isMoving();
  /**< Checks if is moving. */

  void changePermanent(float lasting);
  /**< Imposed movement. */

  void quit();
  /**< Quits. */

  void render();
  /**< Renders the model. */

 private:
  void parser(char *l);
  /**< Parses file line. */

  void defaults();
  /**< Sets default values. */

  void geometry();
  /**< Builds solid geometry. */

  void setName();
  /**< Sets an implicited name. */

  void behaviors();
  /**< Sets behaviors flags. */

  void loader();
  /**< Loads the model. */

  void scaler();
  /**< Scales or not the model. */

  void drawer();
  /**< Draws the model. */

  void preRender();
  /**< Makes translations ans rotations. */

  void postRender();
  /**< Makes glPopMatrix. */

  // GUI callbacks
  static void sound(Model *o, void *d, time_t s, time_t u);
};

#endif
