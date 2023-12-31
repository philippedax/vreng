//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef VICINITY_HPP
#define VICINITY_HPP

#include <string>
#include <list>
#include "wobject.hpp"

class User;
class Solid;

/**
 * Vicinity class
 */
class Vicinity {
  friend class User;	///< access to many members
  friend class Render;  ///< access to Rendering

 private:
  typedef enum {
    DIST_INCL,
    DIST_INTER,
    DIST_STUCK,
    DIST_NEAR,
    DIST_FAR
  } Dist;

  typedef enum {
    SIZE_TINY,
    SIZE_SMALL,
    SIZE_NORMAL,
    SIZE_LARGE,
    SIZE_HUGE
  } Size;

  typedef struct s_vicin {
    WO *object;
    Size size;
    Dist dist;
  } Vicin;

  typedef struct s_visualPos {
    WO *object;
    int nbPixels;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
  } VisualPos;

//   typedef struct typeTable{
//     char typeName[100];
//     struct typeTable suivant;
//   } TypeTable

  WO *obj;
  std::string objName;

  User *user;
  Dist userDist;
  Size userSize;
  Size objSize;

  Vicin* vicinList;
  VisualPos* visualList;

  std::list<Solid*> solidLst; ///< solids list
  int listSize;
  int visualListSize;

  /* private methods */

  static Dist computeDist(WO *obj1, WO *obj2);
  static Size computeSize(WO *obj);

  static int compDist(const void *t1, const void *t2);
  void sortDist();

  static int compInterest(const void *t1, const void *t2);
  void sortInterest();

  static int compVisual(const void *t1, const void *t2);
  void sortVisual();

  bool uselessType(WO *obj);

  void describeTopo(Vicin tmp);
  void describeVisual();

  void setSize(WO *user);

  void actionList();

 public:
  Vicinity();
  Vicinity(std::string _objectName);

  virtual ~Vicinity();

  std::string getObjectName() { return objName; }
  WO* getObject() { return obj; }

  void analTopo();
  void analVisual(int details);
  void analVicinity();
  WO* searchProximityObject(char **typeObj, int nbre);

  void analScene();
  /**<
   * describe the scene viewed by the user and use the vicinity list for the AOI
   */

  static void show();
  static void show(const char *str);

  static int* getTypeFromAction(const char *actionName);
  static char* translateNum2Type(int i);
};

#endif
