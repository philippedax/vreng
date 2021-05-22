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
#ifndef RENDER_HPP
#define RENDER_HPP

#include <list>
using namespace std;

class WObject;
class Solid;

/**
 * Render class
 *
 * OpenGL Graphic Context and Rendering
 */
class Render {
 friend class User;		///< access to many members.
 friend class Solid;		///< access to many members.

public:

  /* points of views */
  enum view_mode {
    VIEW_FIRST_PERSON,
    VIEW_THIRD_PERSON,
    VIEW_THIRD_PERSON_FAR,
    VIEW_VERTICAL,
    VIEW_VERTICAL_FAR,
    VIEW_GROUND_LEVEL,
    VIEW_TURN_AROUND,
    VIEW_WIRED,
    VIEW_SCISSOR,
    VIEW_NUMBER
  };
  
  //
  // Lists of solids
  //
  list<Solid*> solidList;	///< solids list.
  list<Solid*> relsolidList;	///< relative solids list.
  list<Solid*> transparentList;	///< transparent solids list.
  list<Solid*> opaqueList;	///< opaque solids list.
  list<Solid*> flaryList;	///< flary solids list.
  list<Solid*> groundList;	///< ground solids list.
  list<Solid*> modelList;	///< model solids list.
  list<Solid*> userList;	///< user solids list.

  Render();
  /**< Constructor. */

  /////////////
  // Config
  /////////////

  void init(bool _quality);
  /**< Initialization. */

  GLint haveDepth();
  GLint haveTextures();
  GLint haveStencil();
  GLint haveClips();
  /**< Checks OpenGL buffers. */

  static void stat();

  /////////////
  // Rendering
  /////////////

  void render();
  /**< Renders the current scene in the current buffer. */

  void minirender();
  /**< Renders minimal. */

  /////////////
  // Solid
  /////////////

  list<Solid*> getSolidList() { return solidList; }
  /**< return the rendering solid list. */

  void addToList(Solid* solid);
  /**< add solid in rendering list. */

  void delFromList(Solid* solid);
  /**< remove solid from rendering list. */

  void getBB(V3& bbmax, V3& bbmin, bool _framed);
  /**< Gets min/max BB. */

  void setBB(GLfloat w, GLfloat d, GLfloat h);
  void setBB(const GLfloat *v);
  /**< Sets min/max BB. */

  void updateBB(GLfloat az);
  /**< Updates BB according to its orientation. */

  /////////////
  // Views
  /////////////

  void setViewMode(uint8_t mode);
  /**< Sets the view mode. */

  uint8_t getViewMode() const;
  /**< Gets the current view mode. */

  void switchViewMap();
  /**< Switchs the view map. */

  void setPitch(GLfloat angle);
  /**< Sets the pitch angle. */

  /////////////
  // Camera
  /////////////

  void camera();
  /**< Repaint. */

  void resetCamera();
  /**< Resets view. */

  void setCameraPosition(M4 *vr_mat);
  /**<
   * Sets camera position:
   * Matrix given the camera position in the world,
   * specify the orthogonal transformation to apply
   * on world coordoninates to screen coordoninates.
   * not fully implemented.
   */

  void cameraProjection(GLfloat fovy, GLfloat near, GLfloat far);
  /**<
   * Projection definition.
   * - fovy: angle of field width (in degree)
   * - near: distance between the eye and the projection plan
   * - far : distance between the eye and the clipping plan
   */

  void computeCameraProjection();

  void calculateFov(GLfloat x, GLfloat y, GLfloat z, GLfloat az, char* filname);
  /**< calculate the FOV of the user on the position x,y,z and az. */

  void setCameraScissor(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat rotz);
  /**< move the intelligente satellite camera to the good position. */

  /** Camera handling */
  struct sCamera {
    GLfloat fovy, near, far;
  };

  void cameraPosition();	///< Set camera position.

  void clearBuffer();		///< Clear everything.

  /////////////
  // Effects
  /////////////

  void setFlash();
  /**< accessor to flash. */

  void setAllTypeFlashy(char * object_type, int typeflash);
  /**< enable flashy for all same type object. */

  /////////////
  // Selection
  /////////////

  uint16_t bufferSelection(GLint x, GLint y);
  uint16_t bufferSelection(GLint x, GLint y, GLint depth);
  /**< Returns the object's num displayed in (x,y) on the screen. */

  V3 getVisiblePosition(WObject *po);
  /**< get the 3D position of the object on the user screen. */

  WObject** getVisibleObjects(char **listetype, int nbr, int* nbrElemts);
  /**< Get the object list where each object have a type present in the given list. */

  WObject** getDrawedObjects(int* nbr);
  /**< get all drawed Objects on the screen. */

  void analyseScene(char* nameObj);


  void quit();			///< Closes the 3d renderer.

  void clickDirection(GLint x, GLint y, V3 *dir);
  /**< Converts (x,y) screen coord into a direction from eyes to the click. */

  void showSolidList();
  /**< show all solid of the render list. */


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
  bool viewSat;                 ///< local user satellite view.
  GLfloat thirdPerson_yRot;	///< local user y rotation for Third Person view.
  GLfloat thirdPerson_xRot;	///< local user x rotation for Third Person view.
  GLfloat thirdPerson_Near;	///< local user distance for Third Person view.
  GLfloat turnAround;		///< local user rotation for Turn Around view.
  GLfloat pitch;		///< local user pitch rotation.
  V3 satPos;			///< satellite position
  V3 satRot;			///< satellite orientation
  V3 mapPos;			///< map position
  class Wheel *wheel;		///< experimental

  // rendering setup
  void lighting();		///< Set lights.
  void materials();		///< Set colors of materials.

  // rendering
  void renderSolids();		///< general rendering.
  void renderOpaque();		///< opaque solids
  void renderTransparent();  	///< transparent solids
  void renderGround();  	///< ground solids
  void renderModel(); 	 	///< model solids
  void renderUser(); 	 	///< user solids
  void renderFlary(); 	 	///< flary solids

  static bool compDist(const void *t1, const void *t2);	///< compare distantes to eyes
  static bool compSize(const void *t1, const void *t2);	///< compare surfaces sizes
  static bool compFrame(const void *t1, const void *t2);///< compare nbframes

  void putSelbuf(WObject *po);
  /**< Sets object name in Z-buffer for selection. */

  void showMap();
  /**< show the small map. */

  void showSat();
  /**< show the satellite view. */

  static int compareHit(const void *t1, const void *t2);
  /**< Sorts all elements of the selection buffer. */

};

#endif
