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
// render.hpp
//---------------------------------------------------------------------------
#ifndef RENDER_HPP
#define RENDER_HPP

#include "matvec.hpp"   // V3 M4
#include <list>

class WO;
class Solid;


/**
 * Render class
 *
 * OpenGL Graphic Context and Rendering
 */
class Render {
 friend class User;		///< access to some members.
 friend class Solid;		///< access to some members.

 private:
  static const int SEL_BUFSIZ;	///< selection buffer size

  V3 bbox_min;			///< minimal bbox.
  V3 bbox_max;			///< maximal bbox.
  M4 camera_pos;		///< camera position.
  bool first_bb;		///< first bbox.
  bool quality;			///< flag quality yes/no.
  bool flash;			///< flag flash.
  uint8_t view;			///< local user View type.
  bool viewMap;                 ///< local user Map.
  bool viewSat;                 ///< local Satellite.
  bool viewObj;                 ///< local Object.
  GLfloat third_yRot;		///< local user y rotation for Third Person view.
  GLfloat third_xRot;		///< local user x rotation for Third Person view.
  GLfloat third_Near;		///< local user distance for Third Person view.
  GLfloat turna;		///< local user rotation for Turn Around view.
  GLfloat pitch;		///< local user pitch rotation.
  V3 satPos;			///< satellite position.
  V3 satRot;			///< satellite orientation.

  // rendering setup
  void lighting();		///< Set lights.
  void materials();		///< Set colors of materials.

  // rendering categories
  void renderSolids(bool mini);		///< general rendering.
  void renderOpaque(bool mini);		///< opaque solids.
  void renderTransparent(bool mini);  	///< transparent solids.
  void renderGround();  		///< ground solids.
  void renderModel(); 	 		///< model solids.
  void renderUser(); 	 		///< user solids.
  void renderFlary(); 	 		///< flary solids.

  void scissors();
  /**< Renders scissors. */

  // compare functions
  static bool compDist(const void *t1, const void *t2);	///< compare distances to eyes.
  static bool compSize(const void *t1, const void *t2);	///< compare surfaces sizes.
  static bool compFrame(const void *t1, const void *t2);///< compare nbframes.

 public:

  /* points of views */
  enum view_mode {
    VIEW_FIRST_PERSON,
    VIEW_THIRD_PERSON,
    VIEW_THIRD_PERSON_FAR,
    VIEW_THIRD_PERSON_FRONT,
    VIEW_VERTICAL_NEAR,
    VIEW_VERTICAL_FAR,
    VIEW_VERTICAL_FROM_SKY,
    VIEW_GROUND_LEVEL,
    VIEW_TURN_AROUND,
    VIEW_VERTICAL_FROM_OBJECT,
    VIEW_WIRED,
    VIEW_SCISSOR,
    VIEW_NUMBER
  };
  
  //
  // Lists of solids
  //
  std::list<Solid*> solidList;		///< solids list.
  std::list<Solid*> relsolidList;	///< relative solids list.
  std::list<Solid*> opaqueList;		///< opaque solids list.
  std::list<Solid*> transparentList;	///< transparent solids list.
  std::list<Solid*> groundList;		///< ground solids list.
  std::list<Solid*> modelList;		///< model solids list.
  std::list<Solid*> userList;		///< user solids list.
  std::list<Solid*> flaryList;		///< flary solids list.
  std::list<Solid*> getSolidList()	{ return solidList; } ///< Returns the rendering solid list.

  class Wheel *wheel;           ///< experimental

  void showSolidList();
  /**< Shows all solids of the solid list. */

  /////////////
  // Rendering
  /////////////

 public:
  Render();
  /**< Constructor. */

  void render();
  /**< Renders the current scene in the current buffer. */

  void minirender();
  /**< Renders minimal. */

 private:
  void refreshBackground();	///< refresh buffer.

  /////////////
  // Views
  /////////////

  void setViewMode(uint8_t mode);
  /**< Sets the view mode. */

  uint8_t getViewMode() const;
  /**< Gets the current view mode. */

  void switchViewMap();
  /**< Switchs the view map. */

  void switchViewSat();
  /**< Switchs the view sat. */

  void switchViewObj();
  /**< Switchs the view from object. */

  //dax void showView(float posx, float posy, float posz);
  void showView();
  /**< show the view. */

  void resetView();
  /**< Resets view. */

  void setPitch(GLfloat angle);
  /**< Sets the pitch angle. */

 private:
  void showMap();
  /**< show the small map. */

  void showSat();
  /**< show the satellite view. */

  /////////////
  // Camera
  /////////////

 public:
  /** Camera handling */
  struct sCamera {
    GLfloat fovy, near, far;
  };

  void cameraUser();
  /**< Sets camera values (fovy, near, far). */

  void setCameraPosition(M4 *vr_mat);
  /**<
   * Sets camera position:
   * Matrix given the camera position in the world,
   * specify the orthogonal transformation to apply
   * on world coordoninates to screen coordoninates.
   * not fully implemented.
   */

  void cameraPosition();		///< Set camera position.
  void cameraPosition(WO *object);	///< Set camera position.

  void cameraProjection(GLfloat fovy, GLfloat near, GLfloat far);
  /**<
   * Projection definition.
   * - fovy: angle of field width (in degree)
   * - near: distance between the eye and the projection plan
   * - far : distance between the eye and the clipping plan
   */

  void computeCameraProjection();
  /**< called by aiinit. */

  void calculateFov(GLfloat x, GLfloat y, GLfloat z, GLfloat az, char* filname);
  /**< calculate the FOV of the user on the position x,y,z and az. */

  void setCameraScissor(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat rotz);
  /**< move the intelligente satellite camera to the good position. */

  /////////////
  // Selection
  /////////////

 public:
  uint16_t bufferSelection(GLint x, GLint y);
  uint16_t bufferSelection(GLint x, GLint y, GLint depth);
  /**< Returns the object's num displayed in (x,y) on the screen. */

  V3 getVisiblePosition(WO *po);
  /**< get the 3D position of the object on the user screen. */

  WO** getVisibleObjects(char **listetype, int nbr, int* nbrElemts);
  /**< Get the object list where each object have a type present in the given list. */

  WO** getDrawedObjects(int* nbr);
  /**< get all drawed Objects on the screen. */

 private:
  void recordObject(WO *po);
  /**< Sets object name in Z-buffer for selection. */

  static int compareHit(const void *t1, const void *t2);
  /**< Sorts all elements of the selection buffer. */

  /////////////
  // BB
  /////////////

 public:
  void getBB(V3& bbmax, V3& bbmin, bool _framed);
  /**< Gets min/max BB. */

  void setBB(GLfloat w, GLfloat d, GLfloat h);
  void setBB(const GLfloat *v);
  /**< Sets min/max BB. */

  void updBB(GLfloat az);
  /**< Updates BB according to its orientation. */

  /////////////
  // Effects
  /////////////

 public:
  void setFlash();
  /**< Makes a flash (capture). */

  void highlight(char * object_type, int typeflash);
  /**< Enables flashy for all same type object. */

  /////////////
  // Click
  /////////////

 public:
  void clickDirection(GLint x, GLint y, V3 *dir);
  /**< Converts (x,y) screen coord into a direction from eyes to the click. */

  /////////////
  // Config
  /////////////

 public:
  void init(bool _quality);
  /**< Initialization. */

  GLint haveDepth();
  GLint haveTextures();
  GLint haveStencil();
  GLint haveClips();
  /**< Checks OpenGL buffers. */

  static void stat();

  /////////////
  // Quit
  /////////////

 public:
  void quit();			///< Closes the 3d renderer.
};

#endif
