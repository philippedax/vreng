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
// vec3.cpp
//
// Author (10/06/01) :
// Ben Humphrey, Game Programmer
// DigiBen@GameTutorials.com
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "vec3.hpp"


/** Calculates a vector between 2 points and returns the result */
Vec3 Vec3::subVect(Vec3 p1, Vec3 p2)
{
  Vec3 v;

  v.x = p1.x - p2.x;
  v.y = p1.y - p2.y;
  v.z = p1.z - p2.z;
  return v;
}

/** Adds 2 vectors together and returns the result */
Vec3 Vec3::addVect(Vec3 v1, Vec3 v2)
{
  Vec3 v;

  v.x = v2.x + v1.x;
  v.y = v2.y + v1.y;
  v.z = v2.z + v1.z;
  return v;
}

/** Divides a vector by a single number (scalar) and returns the result */
Vec3 Vec3::divVect(Vec3 v1, float val)
{
  Vec3 v;

  v.x = v1.x / val;
  v.y = v1.y / val;
  v.z = v1.z / val;
  return v;
}

/** Returns the cross product between 2 vectors */
Vec3 Vec3::crossVect(Vec3 v1, Vec3 v2)
{
  Vec3 v;

  v.x = ((v1.y * v2.z) - (v1.z * v2.y));
  v.y = ((v1.z * v2.x) - (v1.x * v2.z));
  v.z = ((v1.x * v2.y) - (v1.y * v2.x));
  return v;
}

/** Returns the normal of a vector */
Vec3 Vec3::normVect(Vec3 v)
{
  float magnitude = static_cast<float>(Mag(v));  // Get the magnitude

  v.x /= magnitude;
  v.y /= magnitude;
  v.z /= magnitude;
  return v;
}
