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
#ifndef GRID_HPP
#define GRID_HPP

#include <ubit/ubit.hpp>
using namespace ubit;

/**
 * Grid class
 *
 * 3D help to visualize, displays grids 2D/3D
 */
class Grid {

 private:
  static const uint8_t GRID_HEIGHT;
  static const uint8_t GRID_SLICE;
  static const uint8_t SCROLL_MAX;
  static const uint8_t SCROLL_INCR;

  bool visible;
  GLint dlist;			///< gl display list
  float posx, posy, posz;	///< grid position
  float rotx, roty, rotz;	///< grid orientation
  int width, depth, height;	///< grid dimensions
  int i_width, i_depth, i_height;
  GLfloat red, green, blue, alpha;
  int behavior;			///< grid behavior
  bool overlap;			///< overlapped grid
  bool grid3d;			///< grid 3D
  GLfloat glmat[16];		///< matrix projection

  enum grid_behavior {
    STICK,
    FOLLOW,
    SFOLLOW
  }; // behaviors

 public:
  Grid();
  /**< Constructor */

  virtual ~Grid() {};
  /**< Destructor */

  virtual void render();
  /**< Renders grid */

  virtual void init(int depth, int width, int height);
  /**< Intilialization */

  virtual class UBox * gridBox();
  /**< Builds a dialog box */

  static Grid * grid();
  /**< Returns grid instance */

  virtual void toggleGrid();
  virtual void toggleGrid3d();
  virtual void toggleOverlap();
  virtual void toggleBehavior(int _behavior);

 private:
  UScrollbar *s_width, *s_height, *s_depth, *s_red, *s_green, *s_blue,
             *s_x, *s_y, *s_z, *s_rotx, *s_roty, *s_rotz;
  virtual void setWidth(UEvent &v);
  virtual void setHeight(UEvent &v);
  virtual void setDepth(UEvent &v);
  virtual void setRed(UEvent &v);
  virtual void setGreen(UEvent &v);
  virtual void setBlue(UEvent &v);

  virtual void setPosX(UEvent &v);
  virtual void setPosY(UEvent &v);
  virtual void setPosZ(UEvent &v);
  virtual void setRotX(UEvent &v);
  virtual void setRotY(UEvent &v);
  virtual void setRotZ(UEvent &v);

  virtual void draw();		///< draws grids

  virtual void defaults();	///< default values
  virtual void genScrollbar();
};

#endif
