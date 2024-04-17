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
// grid.hpp
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
  int behavior;			///< grid behavior
  bool overlap;			///< overlapped grid
  bool grid3d;			///< grid 3D
  GLfloat glmat[16];		///< matrix projection

  // behaviors
  enum grid_behavior {
    STICK,
    FOLLOW,
    SFOLLOW
  };

 public:
  GLfloat color[4];		///< color
  uint8_t i_width, i_depth, i_height; ///< size increments

  Grid();
  /**< Constructor. */

  virtual ~Grid() {};
  /**< Destructor. */

  virtual void render();
  /**< Renders grid. */

  void init(uint16_t depth, uint16_t width, uint16_t height);
  /**< Intilialization. */

  class UBox * gridBox();
  /**< Builds a dialog box. */

  static Grid * grid();
  /**< Returns grid instance. */

  void reset();
  void toggleGrid2d();
  void toggleGrid3d();
  void toggleOverlap();
  void toggleBehavior(int behavior);

  void setColor(const float *color);
  void setWidthIncr(uint8_t val);
  void setHeightIncr(uint8_t val);
  void setDepthIncr(uint8_t val);

 private:
  UScrollbar *s_width, *s_height, *s_depth, *s_red, *s_green, *s_blue, *s_alpha,
             *s_x, *s_y, *s_z, *s_rotx, *s_roty, *s_rotz;
  void setWidth(UEvent &v);
  void setHeight(UEvent &v);
  void setDepth(UEvent &v);
  void setRed(UEvent &v);
  void setGreen(UEvent &v);
  void setBlue(UEvent &v);
  void setAlpha(UEvent &v);

  void setPosX(UEvent &v);
  void setPosY(UEvent &v);
  void setPosZ(UEvent &v);
  void setRotX(UEvent &v);
  void setRotY(UEvent &v);
  void setRotZ(UEvent &v);

  void draw();		///< draws grids

  void defaults();	///< default values
  void genValues();
};

#endif
