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
#ifndef VRELET_HPP
#define VRELET_HPP

#define VRELET_TYPE	36
#define VRELET_NAME	"Vrelet"

#include "wobject.hpp"
#include "vjc.hpp"


#define VRELET_NAME_LEN    64	// Class name limit

/** List of 3D points */
typedef struct _PointList {
  V3 point;			///< point position
  struct _PointList *next;	///< next point
} tPointList;

/** 2D objects */
typedef struct _Object2D {
  tPointList *points;		///< list of points
  V3 color;			///< point color
  int type;			///< type
  int tag;			///< tag
  struct _Object2D *next;	///< next object
} tObject2D;


/**
 * Vrelet class
 */
class Vrelet: public WObject {

 public:

  int incrx, incry;		///< increments x y
  float fx, fy;			///< ratios x y
  char app[VRELET_NAME_LEN];	///< class name
  char url[VRELET_NAME_LEN];	///< codebase URL
  V3 posDelta, angDelta; 	///< requested deltas, (x,y,z) and (az,ay,ax)
  tObject2D *o2;		///< List of 2D Objects this Vrelet has

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;} ///< virtual inst. method

  Vrelet(char *l);	///< Constructor

  static void funcs();	///< init funclist

  static WObject * (creator)(char *l); ///< Creates from fileline

  virtual bool isMoving();
  /**< Returns yes if the child has sent a delta request */

  virtual void changePosition(float lasting);
  /**< Propagates the last deltas to the object's position */

  virtual void updateTime(time_t s, time_t us, float *lasting);
  /**< Dummy: always say we need to move */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< Notify the controler that an ingoing intersection occured */

  virtual bool whenIntersectOut(WObject *pcur, WObject *pold);
  /**< Notify the controler that an outgoing intersection occured */

  virtual void render();
  /**< Displays */

  virtual void click(V3 dir);
  /**< React to a user click on our surface */

  virtual void quit();
  /**< Turn the child on (start it up) */

 private:

  int wantDelta;	///< true if the child has requested a position delta
  bool needRedraw;	///< true if a new shape has been added/removed to the list of 2D objects
  GLint dlist;		///< OpenGL displaylist constructed from the above list

  virtual void defaults();
  /**< Default values */

  virtual void parser(char *l);
  /**<
   * Vrelet parser.
   * Configuration line structure:
   * - (name)
   * - position (X Y Z AZ AX)
   * - geometry (see solid.cc)
   * - incrx and incry of the main surface (coordinates passed to the client
   *   will be in the range ([-incrx;incrx], [-incry;incry])
   * - x and y aspect ratio (client incrx/object width)
   * - java class for the client (must implement fr.enst.vreng.<VrengClient>)
   * - codebase URL for that class (optional)
   */

  virtual void draw();
  /**<
   * Make sure our asynch sockets are open if they need to be.
   * If they are, see if the client said something between now
   * and the last render call.
   * Finally, draw this Vrelet's 2D object list.
   */

  virtual void sendPos(WObject *po);
  /**<
   * Send some object's position to the child.
   * If the position is that of the current object,
   * the type is set to 0, otherwise it's set to
   * the object's real type.
   * Packet contents:
   * - signed 8bit type
   * - signed 32bit net object id
   * - signed 16bit port id and object id
   * - signed 32bit x, y, z
   * - signed 32bit az, ax, 0
   * All position/angle variables are multiplied by 1000
   * (sending floats is not currently possible)
   */

  virtual void sendPosError(int type, int ssrc, int port, int id);
  /**<
   * Same as sendPos, except that no position is sent.
   * This is to indicate to the controler application that
   * the object it's working on no longer exists.
   */

  virtual void sendIntersect(WObject *po, WObject *old, int inOrOut);
  /**<
   * Send the client a notification that a thing just entered/exited
   * its bounding box
   */

  virtual void answerTypeQuery(int type);
  /**<
   * Answer a child's query.
   * Right now, the only query available is a query
   * to the list of mobile objects, asking for a given type
   * of WObject. A list of all the object ids is returned to the child.
   */

  virtual void processClient();
  /**<
   * Deal with input from the client app.
   * This function is called at every render phase.
   * The client socket is probed for incoming packets,
   * with an empty timeout. If no packets were queued,
   * the function returns immediatly.
   * No more than one packet is dealt with in one go.
   * Messages dealt with:
   * - MSGT_ADD: adds 2D objects to this object's rendering
   * - MSGT_DEL: removes one or all of the existing 2d objects
   * - MSGT_POS:
   *   MSGV_ASK: sends the queried object's position back to the client
   *   MSGV_SET: sets this object's position to the parameters passed in
   *   MSGV_UPD: considers the params passed as deltas on the object's position
   * - MSGT_QUERY: Answers to type queries (returns a list of mobile objects)
   */

  virtual void setPos(WObject *po);
  /**< Set initial position of this object */

  virtual void deltaPos(WObject *po);
  /**< Update position of this object */

  virtual void sendClick(int x, int y);
  /**<
   * Send a notification to the child saying a click occured.
   * The coordinates sent are relative to the object's main surface
   * (which would be described by the vectors (1,0,0), (0,1,0),
   * and a normal of (0,0,1)). This only works well if the object
   * is very thin. Clicks hitting the other surface (norm=(0,0,-1))
   * are ignored.
   * Packet contents:
   * - signed 32bit x
   * - signed 32bit y
   */

  // 2D objects
  tObject2D * newObject2D(tObject2D *list, int type, int tag, V3 color);
  tObject2D * removeObject2D(tObject2D *list, int type, int tag);

  void addPoint(tObject2D *, V3 pt);
  void freeObject2D(tObject2D *);
  void freeObject2DList(tObject2D *);
  void freePointList(tPointList *);

};

#endif
