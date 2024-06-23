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
#ifndef BONEMESH_HPP
#define BONEMESH_HPP

#include "bonename.hpp"
#include "bonevertex.hpp"
#include "bonetri.hpp"
#include "bonelink.hpp"
#include "bonelist.hpp"


/**
 * BoneMesh class
 */
class BoneMesh : public Bonename {
 public:
  // Gestion des sommets
  BoneList < Vertex > vertexList;
  Vertex ** vertex;
  int vertices;
  int vertexListCompiled;

  // Gestion des faces triangulaires
  BoneList<BoneTriangle> triangleList;
  BoneTriangle **triangle;
  int triangles;
  int triangleListCompiled;

  // Infos de boite englobante
  float minx, miny, minz;
  float maxx, maxy, maxz;

  // Constructeurs / destructeur
  BoneMesh();
  virtual ~BoneMesh();

  // Actions sur les champs
  void addVertex(Vect3D & zePosition);
  void addVertex(Vect3D * zePosition);
  void addVertex(float ox, float oy, float oz);
  void addTriangle(int index1, int index2, int index3);

  // Fonctions internes
  void compileVertexList();
  void compileTriangleList();

  // Modification du mesh
  void rebuildNormals();
  void scale (float sx, float sy, float sz);
  void projectLight();

#if 0 //notused
  void generateBoundingBox();
  void flipNormals();
  void translate(float dx, float dy, float dz); // Definitive action !
  void mirrorx();
  void mirrory();
  void mirrorz();
  void planarMappingX();
  void planarMappingY();
  void planarMappingZ();
  void cylinderMapping();
#endif

};

#endif
