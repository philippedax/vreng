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
// v3d.hpp
//
// Author   : This file has been written by Yann Renard
// Copyright: This file is totaly free and you may distribute
//            it to anyone you want, without modifying this
//            header. If you use it in a commercial project (?)
//            or in bigger project (!), I would be glad to know
//            about it :) Please mail me...
//            be glad to know about it, please mail me myself_yr@hotmail.com
//---------------------------------------------------------------------------
#ifndef V3D_HPP
#define V3D_HPP

#include "bone.hpp"     // BoneMesh, BoneVertex

/**
 * V3d class
 */
class V3d {
  static const float SCALE;

 public:
  V3d();			///< Constructor
  virtual ~V3d() {};		///< Destructor

  static void readV3D(BoneMesh *result, BoneVertex *skeletonRoot, char *filename, float size = 1.);
};

#endif
