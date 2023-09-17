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
#include "vreng.hpp"


// local
static float sintab[360];


/** Returns angular variation modulo 2PI */
float deltaAngle(float a1, float a2)
{
  float da = a1 - a2;

  da -= M_2PI * ((int)(da / M_2PI));
  if (da < -M_PI)
    da += M_2PI;
  if (da > M_PI)
    da -= M_2PI;
  return da;
}

/** Converts radian to degree */
int radian2degree(float r)
{
  if (r < 0)
    r += (double) M_2PI;
  int d = (int) (r / (double) M_2PI * 3600);
  if (d % 10 > 5)
    d += 5;
  d /= 10;
  d %= 360;
  return d;
}

/** Intialialize table of sinus per degree */
void initTrigo()
{
  for (int d=0; d < 360; d++) {
    double r = (float) d * (M_2PI / 360);
    sintab[d] = (float) sin(r);
  }
}

float Sin(double r)
{
  return ((float) sintab[radian2degree((float) r)]);
}

float Cos(double r)
{
  return ((float) (Sin(r + (double) M_PI/2)));
}

float Tan(double r)
{
  return ((float) (Sin(r) / Cos(r)));
}
