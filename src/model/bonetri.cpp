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
#include "vreng.hpp"
#include "bonetri.hpp"


// Construct / destruct
Triangle::Triangle()
{
    vertex1 = NULL;
    vertex2 = NULL;
    vertex3 = NULL;
    initialNormal.reset();
    currentNormal.reset();
    setColor(0.5, 0.5, 0.5, 1);
}

Triangle::~Triangle() {}

// Accessing datas
void Triangle::addVertex(Vertex *zeVertex, int index, float u=-1, float v=-1)
{
  if (( u == -1 ) && ( v == -1 )) {
     u = zeVertex -> initialPosition.x / 3.0f;
     v = zeVertex -> initialPosition.y / 3.0f;
  }
  if (vertex1 == NULL) {
    vertex1 = zeVertex; index1 = index; u1 = u; v1 = v;
  }
  else if (vertex2 == NULL) {
    vertex2 = zeVertex; index2 = index; u2 = u; v2 = v;
  }
  else if (vertex3 == NULL) {
    vertex3 = zeVertex; index3 = index; u3 = u; v3 = v;
  }
}

void Triangle::rebuildNormal()
{
  Vect3D edge1;
  Vect3D edge2;
  edge1 = vertex1->initialPosition - vertex2->initialPosition;
  edge2 = vertex1->initialPosition - vertex3->initialPosition;
  initialNormal.crossProduct(edge1, edge2);
  initialNormal.normalize();
  currentNormal = initialNormal;
}

void Triangle::setColor(float r=0.5, float g=0.5, float b=0.5, float a=1)
{
  colorRed   = finalRed   = r;
  colorGreen = finalGreen = g;
  colorBlue  = finalBlue  = b;
  colorAlpha = finalAlpha = a;
}
