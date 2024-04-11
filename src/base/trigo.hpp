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
#ifndef TRIGO_HPP
#define TRIGO_HPP

#ifndef M_PI
#define M_PI            3.14159265 // pi
#endif
#ifndef M_2PI
#define M_2PI		6.28318530 //2pi 
#endif
#ifndef M_PI_2
# define M_PI_2         1.570795 // pi/2
#endif
#ifndef M_PI_4
# define M_PI_4         0.785395 // pi/4
#endif
#define M_2PI_5		1.256637 // 2pi/5
#define M_PI_180	0.017453 // pi/180
#ifndef M_1_PI
# define M_1_PI         0.31831 // 1/pi
#endif
#ifndef M_SQRT2
# define M_SQRT2        1.41421 // sqrt(2)
#endif
#ifndef M_SQRT1_2
# define M_SQRT1_2      0.70710 // sqrt(2)/2
#endif
#ifndef M_SQRT3
# define M_SQRT3        1.732051 // sqrt(3)
# define M_SQRT3_2	0.866025 // sqrt(3)/2
#endif
#define EPSILON		1e-6

#define DEG2RAD(x) ((float) (M_PI/180.)*(x))
#define RAD2DEG(x) ((float) (180./M_PI)*(x))

void initTrigo();

float deltaAngle(float a1, float a2);
/**< Returns angular variation modulo 2PI */

int radian2degree(float);
/**< Converts radian ro degree */

float Cos(double);
float Sin(double);
float Tan(double);

#endif
