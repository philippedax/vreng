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
// widget.hpp
//---------------------------------------------------------------------------
#ifndef WIDGET_HPP
#define WIDGET_HPP

#define WIDGET_TYPE	55
#define WIDGET_NAME	"Widget"

#include "object.hpp"


/**
 * Widget class
 */
class Widget : public Object {
 private:
  bool focus;
  GLuint texture;		///< texture number
  uint16_t tex_width;		///< texture width
  uint16_t tex_height;		///< texture height
  GLfloat u, v;			///< witdh and height screen ratios
  GLubyte *def_pixmap;		///< default texture pixmap
  GLubyte *tex_pixmap;		///< current pixmap
  GLfloat vertices[4*3];	///< screen geometry
  GLdouble modelmat[16];	///< modelisation matrix
  GLdouble projmat[16];		///< projection data coordinates
  GLint viewport[4];

 public:
  static const OClass oclass;   ///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Widget(char *l);	///< Constructor
  ~Widget();		///< Destructor

  static Object * (creator)(char *l);
  /**< Creates from fileline */

  void render();
  /**< Render the object */

  bool mouseEvent(int16_t x, int16_t y, uint8_t button);
  /**< Redirect mouse events */

  bool keyEvent(const char *key, bool is_down);
  /**< Redirect keyboard events */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets defaults values */

  void behaviors();
  /**< Sets behaviors */

  void inits();
  /**< Do specific inits */

  void setTexture(bool mipmap);
  /**< Build a texture */

  void defaultPixmap();
  /**< Build the default pixmap */

  void drawScreen();
  /**< Draw the screen */

  void buildScreen();
  /**< Build the screen */

  // GUI callbacks
  static void takeFocus(Widget *o, void *d, time_t s, time_t u);
  static void leaveFocus(Widget *o, void *d, time_t s, time_t u);
};

#endif
