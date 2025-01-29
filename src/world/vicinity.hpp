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
#include "object.hpp"


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
    int nbpixels;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
  } VisuPos;

  Object *obj;
  std::string objname;
  class User *user;
  Dist userdist;
  Size usersize;
  Size objsize;
  Vicin* vicilist;
  VisuPos* visulist;
  int visusize;
  int listsize;
  std::list<class Solid*> solidlst; ///< solids list

  /* private methods */

  // eval methods
  static Dist evalDist(Object *obj1, Object *obj2);
  static Size evalSize(Object *obj);

  // compare and sort methods
  static int cmpDist(const void *t1, const void *t2);
  static int cmpInterest(const void *t1, const void *t2);
  static int cmpVisual(const void *t1, const void *t2);
  static void show();

  void sortDist();
  void sortInterest();
  void sortVisual();
  bool uselessType(Object *obj);
  void describeTopo(Vicin tmp);
  void describeVisual();
  void setSize(Object *user);
  void actionList();
  void analTopo();
  void analVisual(int details);
  void analVicinity();

 public:
  Vicinity();
  Vicinity(std::string _objname);

  virtual ~Vicinity();

  std::string getObjectName() { return objname; }

  Object* searchProximityObject(char **typeObj, int nbre);

  void analScene();
  /**< describe the scene viewed by the user and use the vicinity list for the AOI */

  static void show(const char *str);
  static char* translateNum2Type(int i);
  static int* getTypeFromAction(const char *actionName);
};

#endif
