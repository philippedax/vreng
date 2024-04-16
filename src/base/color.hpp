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
// color.hpp
//---------------------------------------------------------------------------
#ifndef COLOR_HPP
#define COLOR_HPP


/**
 * Color class
 */
class Color {
 private:
  GLfloat color[4];

 public:
  Color(const float* _color);

  Color(float r, float g, float b, float a);

  virtual ~Color() {};

  operator const GLfloat* const() const {return color;}

  bool operator==(const Color&) const;

  const static Color black, white, red, green, blue, grey, yellow, cyan, magenta, orange, pink;

  static void getRGB(const char *name, float *v);
};


#endif
