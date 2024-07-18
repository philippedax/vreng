//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// col.hpp
//---------------------------------------------------------------------------
#ifndef COL_HPP
#define COL_HPP


// intersection cases
enum {
  NO_INTER,     // no intersect
  IN2,          // obj1 inside obj2
  IN1,          // obj2 inside obj1
  INTERSECT     // intersect partiel
};

//
// Collisions (see col.cpp)
//
/**
 * class Col
 */
class Col: public WO {
friend class WO;

 public:
  void generalIntersect(WO *wo, OList *vicinityList);
  /**< General intersection of objects. */

  void copyPositionAndBB(Pos &newpos);
  void copyPositionAndBB(WO *o);
  /**< Copy object position and Bounding Box. */

  int projectPositionOnObstacle(Pos &pcur, Pos &wo, Pos &obstacle);
  /**< Projects object position on an obstacle. */

  void computeNormal(Pos &mobil, Pos &stil, V3 *normal);
  void computeNormal(WO *mobil, V3 *normal);
  /**< Computes the normal of still object. */

  bool projectPosition(WO *pcur, WO *wo);
  /**< Projects object position. */

  void bounceTrajectory(WO *wo, V3 *norm);
  /**< Intersects with wall. */

  void updatePosition();
  /**< Updating 3D. */

  void updatePositionAndGrid(Pos &oldpos);
  /**< Updating 3D and grid position. */

  void updatePositionAndGrid(WO *wo);
  /**< Updating 3D and grid position. */

  void updateDist();
  /**< Updating distance to localuser. */

 private:
  bool ingoingNeighbor(WO *wo, WO *neighbor);
  /** Checks ingoing intersection with a neighbor. */

  bool outgoingNeighbor(WO *wo, WO *neighbor);
  /** Checks outgoing intersection with a neighbor. */

  void ingoingWalls(WO *wo);
  /** Checks ingoing intersection with walls. */

  void initPosition();
  /**< Init 3D and grid position. */
};

#endif
