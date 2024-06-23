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
//                myself_yr@hotmail.com                         //
// ------------------------------------------------------------ //
#ifndef BONETRI_HPP
#define BONETRI_HPP

#include "bonevertex.hpp"


/**
 * BoneTriangle class
 */
class BoneTriangle {
 public:
  // Datas for the triangle class
  Vertex * vertex1; float u1, v1; int index1;
  Vertex * vertex2; float u2, v2; int index2;
  Vertex * vertex3; float u3, v3; int index3;
  Vect3D initialNormal;
  Vect3D currentNormal;
  float colorRed, colorGreen, colorBlue, colorAlpha; // Initial colors
  float finalRed, finalGreen, finalBlue, finalAlpha; // Projected colors

  // Construct / destruct
  BoneTriangle();

  virtual ~BoneTriangle();

  // Accessing datas
  //void addVertex(Vertex *zeVertex, int index, float u=-1, float v=-1);
  void addVertex(Vertex *zeVertex, int index, float u, float v);

  void rebuildNormal();

  void setColor(float r, float g, float b, float a);
};

#endif
