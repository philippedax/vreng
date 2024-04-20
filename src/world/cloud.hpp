//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// cloud.hpp
//---------------------------------------------------------------------------
#ifndef CLOUD_HPP
#define CLOUD_HPP

#include "wobject.hpp"

#define CLOUD_TYPE	57
#define CLOUD_NAME	"Cloud"


typedef struct {
  GLfloat x, y, z, r;	///< pos + radius
  GLint dlist;		///< displaylist
} tObloid;


/**
 * Cloud class
 */
class Cloud: public WO {

 private:
  static const uint16_t NUMBER;
  static const float SPEED;
  static const float FAR;

  uint16_t number;	///< number of obloids
  float speed;		///< speed
  GLfloat color[4];	///< color
  tObloid *obloids;	///< obloids

 protected:
  bool snowing;		///< flag snowing
  bool raining;		///< flag raining

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Cloud(char *l);	///< Constructor

  static WO * (creator)(char *l);
  /**< Creation from a file */

  void changePermanent(float lasting);
  /**< Equation to move permanently */

  void render();
  /**< Specific rendering */

  void quit();
  /**< Quits properly */

 private:
  Pos origpos;

  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets default values */

  void behaviors();
  /**< Sets behaviors */

  void inits();
  /**< Sets specific inits */

  void build();
  void rebuild();
  /** Builds obloids */

  void draw(int n);
  /** Draws an obloid */

  void geometry();
  /**< Builds geometry */

  // Callbacks
  void snow(Cloud *cloud, void* d, time_t s, time_t u);
  void rain(Cloud *cloud, void* d, time_t s, time_t u);
  void sound(Cloud *cloud, void* d, time_t s, time_t u);
  static void snow_cb(Cloud *cloud, void* d, time_t s, time_t u);
  static void rain_cb(Cloud *cloud, void* d, time_t s, time_t u);
  static void sound_cb(Cloud *cloud, void* d, time_t s, time_t u);
  static void pause_cb(Cloud *cloud, void* d, time_t s, time_t u);
};

#endif
