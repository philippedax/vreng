//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// vicinity.hpp
//---------------------------------------------------------------------------
#ifndef VICINITY_HPP
#define VICINITY_HPP

#include <string>
#include <list>
#include "wobject.hpp"


/**
 * Vicinity class
 */
class Vicinity {
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
    Object *object;
    Size size;
    Dist dist;
  } Vicin;

  typedef struct s_visuPos {
    Object *object;
    int nbPixels;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
  } VisuPos;

  Object *obj;
  std::string objName;
  class User *user;
  Dist userDist;
  Size userSize;
  Size objSize;
  Vicin* viciList;
  VisuPos* visuList;
  std::list<class Solid*> solidLst; ///< solids list
  int listsize;
  int visuListSize;

  /* private methods */

  // eval methods
  static Dist evalDist(Object *obj1, Object *obj2);
  static Size evalSize(Object *obj);

  // compare and sort methods
  static int cmpDist(const void *t1, const void *t2);
  void sortDist();

  static int cmpInterest(const void *t1, const void *t2);
  void sortInterest();

  static int cmpVisual(const void *t1, const void *t2);
  void sortVisual();

  bool uselessType(Object *obj);

  void describeTopo(Vicin tmp);
  void describeVisual();

  void setSize(Object *user);

  void actionList();

 public:
  Vicinity();
  Vicinity(std::string _objName);

  virtual ~Vicinity();

  std::string getObjectName() { return objName; }
  Object* getObject() { return obj; }

  void analTopo();
  void analVisual(int details);
  void analVicinity();
  Object* searchProximityObject(char **typeObj, int nbre);

  void analScene();
  /**< describe the scene viewed by the user and use the vicinity list for the AOI */

  static void show();
  static void show(const char *str);

  static int* getTypeFromAction(const char *actionName);
  static char* translateNum2Type(int i);
};

#endif
