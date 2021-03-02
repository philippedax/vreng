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
    WObject *object;
    Size size;
    Dist dist;
  } Vicin;

  typedef struct s_visualPosition {
    WObject *object;
    int nbPixels;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
  } VisualPosition;

//   typedef struct typeTable{
//     char typeName[100];
//     struct typeTable suivant;
//   } TypeTable

  WObject *refObject;
  std::string refObjectName;

  User *user;
  Dist userDist;
  Size userSize;
  Size objectSize;

  Vicin* vicinList;
  VisualPosition* visualList;

  std::list<Solid*> solidList; ///< solids list
  int listSize;
  int visualListSize;

  /**** METHODS ****/

  static Dist computeDistance(WObject *obj1, WObject *obj2);
  static Size computeSize(WObject *obj);

  static int cmpDistance(const void *t1, const void *t2);
  virtual void sortDistance();

  static int cmpInterest(const void *t1, const void *t2);
  virtual void sortInterest();

  static int cmpVisual(const void *t1, const void *t2);
  virtual void sortVisual();

  virtual bool uselessType(WObject *obj);

  virtual void describeTopo(Vicin tmp);
  virtual void describeVisual();

  virtual void actionList();

  virtual void setSize(WObject *user);

public:

  Vicinity();
  Vicinity(std::string _objectName);

  virtual ~Vicinity();

  virtual std::string getObjectName() { return refObjectName; }
  virtual WObject* getObject() { return refObject; }

  virtual void analyseTopo();
  virtual void analyseVisual(int details);
  virtual void analyseVicinity();
  virtual WObject* searchProximityObject(char **typeObj, int nbre);

  virtual void analyseScene();
  /**<
   * describe the scene viewed by the user and use the vicinity list for the AOI
   */

  static void show();
  static void show(const char *str);

  static int* getTypeFromAction(const char *actionName);
  static char* translateNum2Type(int i);

};

#endif
