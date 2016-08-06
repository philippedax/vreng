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
// ------------------------------------------------------------ //
// Author   : Yann Renard 				        //
// Copyright: This file is totaly free and you may distribute   //
//            it to anyone you want, without modifying this     //
//            header. If you use it in a commercial project (?) //
//            or in bigger project (!), I would be glad to know //
//            about it :) Please mail me...                     //
//            be glad to know about it, please mail me          //
//                myself_yr@hotmail.com                         //
// ------------------------------------------------------------ //
#ifndef V3D_HPP
#define V3D_HPP

#include "bonemesh.hpp"
#include "bonevertex.hpp"


/**
 * V3d class
 */
class V3d {
 public:

  V3d() {}		///< constructor
  virtual ~V3d() {}	///< destructor

  static void readV3Dfile(BoneMesh *result,
			BoneVertex *skeletonRoot,
			char *filename,
			float size = 1.);
#if 0 //not used
  static void readVRMLfile(BoneMesh *result,
			char *filename,
			float size = 1.,
			float centrex = 0.,
			float centrey = 0.,
			float centrez = 0.,
			int colorMask = 0);
#endif //not used
};

#endif
