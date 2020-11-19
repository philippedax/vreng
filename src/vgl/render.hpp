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
 friend class WObject;		///< access to many members.
 friend class Solid;		///< access to many members.

public:

  /* views */
  enum view_mode {
    VIEW_FIRST_PERSON,
    VIEW_THIRD_PERSON,
    VIEW_THIRD_PERSON_FAR,
#if 0
    VIEW_FIRST_PERSON_VIS,
#endif
    VIEW_VERTICAL,
    VIEW_VERTICAL_FAR,
    VIEW_GROUND_LEVEL,
    VIEW_TURN_AROUND,
    VIEW_WIRED,
    VIEW_SCISSOR,
    VIEW_NUMBER
  };


  Render();
  /**< Constructor. */

  void config(bool _quality);
  /**< Initialization. */

  GLint haveDepth();
  GLint haveTextures();
  GLint haveStencil();
  GLint haveClips();
  /**< Checks buffers. */

  static void stat();

  /////////////
  // Rendering
  /////////////

  void render();
  /**< Renders the current scene in the current buffer. */

  // Drawing
  //virtual void render3D(rendering_mode mode, uint8_t layer);
  ///**< Issue the OpenGL commands to draw the solid in the given mode.
       //It is called several times with "layer" increasing from 0 to ? in
       //order to allow drawing at different layers. */

  void minirender();
  /**< Renders minimal. */

  /////////////
  // Solid
  /////////////

  std::list<Solid*> getSolidList() { return solidList; }
  /**< return the rendering solid list. */

  void addSolidInList(Solid* solid);
  /**< add solid in rendering list. */

  void removeSolidFromList(Solid* solid);
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

#if 0 //not used
   void cameraPerspective(GLfloat fovy, GLfloat near, GLfloat far, GLfloat ratio);
#endif //not used

  /////////////
  // Effects
  /////////////

  void setFlash();
  /**< accessor to flash. */

  void setAllTypeFlashy(char * object_type, int typeflash);
  /**< enable flashy for all same type object. */

  V3 getMapUserPos() { return mapUserPos; }
  /**<
   * return the x,y position of the user on the map
   * and z is a bool what say if map is on/off.
   */

  /////////////
  // Selection
  /////////////

  uint16_t getBufferSelection(GLint x, GLint y, GLint zdepth);
  /**<
   * Returns the solid's num displayed in (x,y) on the screen.
   * We use here coordinates IN THE ZBUFFER and no in the display window.
   */

  V3 getVisiblePosition(WObject *po);
  /**< get the 3D position of the object on the user screen. */

  WObject** getVisibleTypedObjects(char **listetype, int nbr, int* nbrElemts);
  /**< Get the object list where each object have a type present in the given list. */

  WObject** getDrawedObjects(int* nbr);
  /**< get All drawed Objects on the screen. */

  void analyseUserScene(char* nameObj);

  /////////////
  // Misc
  /////////////

  void quit();			///< Closes the 3d display.

  void clickDirection(GLint x, GLint y, V3 *dir);
  /**< Converts (x,y) screen coord into a direction from eyes to the click. */

  /////////////
  // Debug
  /////////////

  void showSolidList();
  /**< show all solid of the render list. */


public:
  /**
   * Camera handling
   */
  struct sCamera { GLfloat fovy, near, far; };

  void cameraPosition();	///< Set camera position.
  void clearGLBuffer();		///< Clear everything.

private:

  static const int SEL_BUFSIZ;
  
  std::list<Solid*> solidList;	///< rendering solids list.

  V3 bbox_min;			///< minimal bbox.
  V3 bbox_max;			///< maximal bbox.
  M4 camera_pos;		///< camera position.
  bool first_bbox;		///< first bbox.
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
  V3 satellitePos;
  V3 satelliteRot;
  V3 mapUserPos;
  int numberVisibleSolid;
  int numberBlinkSolid;
  int numberFlashySolid;

  class Wheel *wheel;		///< experimental

  // rendering setup
  void lighting();		///< Set lights.
  void materials();		///< Set colors of materials.

  // effective objects rendering
  void objectsRendering(bool select);	///< Render solids in display-list.
  void specificRender(uint32_t n, uint8_t prior);	///< Special rendering.
  void specificStill(uint32_t n, uint8_t prior);	///< Special rendering.
  void specificMobile(uint32_t n, uint8_t prior);	///< Special rendering.
  void specificInvisible(uint32_t n, uint8_t prior);	///< Special rendering.
  void specificFluid(uint32_t n, uint8_t prior);	///< Special rendering.

  // effective solids rendering
  void solidsOpaque(bool zsel, list<Solid*>::iterator psu, uint8_t prior);	///< opaque solids
  void solidsTranslucid(bool zsel, list<Solid*>::iterator psu, uint8_t prior);  ///< translucid solids

  void putSelbuf(WObject *po);
  /**< Sets object name in Z-buffer for selection. */

  void map();
  /**< show the small map. */

  void satellite();
  /**< show the satellite view. */

  static int compareHit(const void *t1, const void *t2);
  /**< Sorts all elements of the selection buffer. */

};

#endif
