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
#include "bonemesh.hpp"


BoneMesh::BoneMesh()
{
  vertex = NULL;
  vertices = 0;
  vertexListCompiled = 0;
  triangle = NULL;
  triangles = 0;
  triangleListCompiled = 0;

  setName((char*)" Mesh NoName ");
}

BoneMesh::~BoneMesh()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  for (int i=0; i < vertices; i++) delete vertex[i];
  for (int j=0; j < triangles; j++) delete triangle[j];
}

void BoneMesh::addVertex(Vect3D & zePosition)
{
  vertexList.addElem(new Vertex(zePosition));
  vertexListCompiled = 0;
}

void BoneMesh::addVertex(Vect3D * zePosition)
{
  vertexList.addElem(new Vertex(zePosition));
  vertexListCompiled = 0;
}

void BoneMesh::addVertex(float ox, float oy, float oz)
{
  vertexList.addElem(new Vertex(ox, oy, oz));
  vertexListCompiled = 0;
}

void BoneMesh::addTriangle(int index1, int index2, int index3)
{
  if (! vertexListCompiled) compileVertexList();

  BoneTriangle * tri = new BoneTriangle();
  tri->addVertex(vertex[index1], index1, -1, -1);
  tri->addVertex(vertex[index2], index2, -1, -1);
  tri->addVertex(vertex[index3], index3, -1, -1);

  triangleList.addElem(tri);
  triangleListCompiled = 0;
}

void BoneMesh::scale(float sx, float sy, float sz)
{
  if (! vertexListCompiled) compileVertexList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->initialPosition.x *= sx;
    vertex[i]->initialPosition.y *= sy;
    vertex[i]->initialPosition.z *= sz;
    vertex[i]->currentPosition.x *= sx;
    vertex[i]->currentPosition.y *= sy;
    vertex[i]->currentPosition.z *= sz;
  }
}

void BoneMesh::rebuildNormals()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  for (int i=0; i < vertices; i++) vertex[i]->initialNormal.reset();

  for (int i=0; i < triangles; i++) {
    triangle[i]->rebuildNormal();
    triangle[i]->vertex1->initialNormal += triangle[i]->initialNormal;
    triangle[i]->vertex2->initialNormal += triangle[i]->initialNormal;
    triangle[i]->vertex3->initialNormal += triangle[i]->initialNormal;
  }

  for (int i=0; i < vertices; i++) vertex[i]->initialNormal.normalize();

  projectLight();
}

void BoneMesh::projectLight()
{
  if (! triangleListCompiled) compileTriangleList();

  Vect3D lightDirection(0,0,1);
  Vect3D lightDiffuse(1,1,1);
  Vect3D lightSpecular(1,1,1);

  for (int i=0; i < triangles; i++) {
    Vect3D mat(triangle[i]->colorRed, triangle[i]->colorGreen, triangle[i]->colorBlue);
    float cosine = Vect3D::dotProduct(lightDirection, triangle[i]->initialNormal);
    cosine = CROP(-1, cosine, 1);
    if (cosine > 0) {
      float cosPower = 64;
      float cosinePower = pow(cosine, cosPower);
      mat.x *= lightDiffuse.x * cosine + lightSpecular.x * cosinePower;
      mat.y *= lightDiffuse.y * cosine + lightSpecular.y * cosinePower;
      mat.z *= lightDiffuse.z * cosine + lightSpecular.z * cosinePower;
      if (mat.x > 1) mat.x = 1;
      if (mat.y > 1) mat.y = 1;
      if (mat.z > 1) mat.z = 1;
    }
    else mat.reset();

    triangle[i]->finalRed   = mat.x;
    triangle[i]->finalGreen = mat.y;
    triangle[i]->finalBlue  = mat.z;
  }
}

void BoneMesh::compileVertexList()
{
  vertex = vertexList.getNiceTable(&vertices);
  vertexListCompiled = 1;
}

void BoneMesh::compileTriangleList()
{
  triangle = triangleList.getNiceTable(&triangles);
  triangleListCompiled = 1;

  // Moving textureCoordinate vertex up to face
  for (int i=0; i < triangles; i++) {
    if (triangle[i]->vertex1->u != -1) {
      if (triangle[i]->vertex1->v != -1) {
        triangle[i]->u1 = triangle[i]->vertex1->u;
        triangle[i]->v1 = triangle[i]->vertex1->v;
      }
    }
    if (triangle[i]->vertex2->u != -1) {
      if (triangle[i]->vertex2->v != -1) {
         triangle[i]->u2 = triangle[i]->vertex2->u;
       	 triangle[i]->v2 = triangle[i]->vertex2->v;
      }
    }
    if (triangle[i]->vertex3->u != -1) {
      if (triangle[i]->vertex3->v != -1) {
         triangle[i]->u3 = triangle[i]->vertex3->u;
	 triangle[i]->v3 = triangle[i]->vertex3->v;
      }
    }
  }
}

#if 0 //notused

void BoneMesh::generateBoundingBox()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  maxx = maxy = maxz = -1000000.;
  minx = miny = minz =  1000000.;

  for (int i=0; i < vertices; i++) {
    if (vertex[i]->initialPosition.x > maxx) maxx = vertex[i]->initialPosition.x;
    if (vertex[i]->initialPosition.y > maxy) maxy = vertex[i]->initialPosition.y;
    if (vertex[i]->initialPosition.z > maxz) maxz = vertex[i]->initialPosition.z;
    if (vertex[i]->initialPosition.x < minx) minx = vertex[i]->initialPosition.x;
    if (vertex[i]->initialPosition.y < miny) miny = vertex[i]->initialPosition.y;
    if (vertex[i]->initialPosition.z < minz) minz = vertex[i]->initialPosition.z;
   }
}

void BoneMesh::flipNormals()
{
  for (int i=0; i < triangles; i++) {
    Vertex *tmpVertex;
    float tmpU;
    float tmpV;
    int tmpIndex;

    SWAP(triangle[i]->vertex1, triangle[i]->vertex2, tmpVertex);
    SWAP(triangle[i]-> u1    , triangle[i]-> u2    , tmpU     );
    SWAP(triangle[i]-> v1    , triangle[i]-> v2    , tmpV     );
    SWAP(triangle[i]-> index1, triangle[i]-> index2, tmpIndex );
  }
  rebuildNormals();
}

void BoneMesh::translate(float dx, float dy, float dz)
{
  if (! vertexListCompiled) compileVertexList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->initialPosition.x += dx;
    vertex[i]->initialPosition.y += dy;
    vertex[i]->initialPosition.z += dz;
    vertex[i]->currentPosition.x += dx;
    vertex[i]->currentPosition.y += dy;
    vertex[i]->currentPosition.z += dz;
  }
}

void BoneMesh::mirrorx()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->initialPosition.x *= -1;
    vertex[i]->currentPosition.x *= -1;
  }
  flipNormals();
}

void BoneMesh::mirrory()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->initialPosition.y *= -1;
    vertex[i]->currentPosition.y *= -1;
  }
  flipNormals();
}

void BoneMesh::mirrorz()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->initialPosition.z *= -1;
    vertex[i]->currentPosition.z *= -1;
  }
  flipNormals();
}

void BoneMesh::planarMappingX()
{
  generateBoundingBox();

  float max = MAX(MAX(maxx, maxy), maxz);
  float min = MIN(MIN(minx, miny), minz);
  float dy = ((max - min) - (maxy - miny)) / 2;
  float dz = ((max - min) - (maxz - minz)) / 2;

  for (int i=0; i < triangles; i++) {
    triangle[i]->u1 =     (dy + triangle[i]->vertex1->initialPosition.y - miny) / (max - min);
    triangle[i]->u2 =     (dy + triangle[i]->vertex2->initialPosition.y - miny) / (max - min);
    triangle[i]->u3 =     (dy + triangle[i]->vertex3->initialPosition.y - miny) / (max - min);
    triangle[i]->v1 = 1 - (dz + triangle[i]->vertex1->initialPosition.z - minz) / (max - min);
    triangle[i]->v2 = 1 - (dz + triangle[i]->vertex2->initialPosition.z - minz) / (max - min);
    triangle[i]->v3 = 1 - (dz + triangle[i]->vertex3->initialPosition.z - minz) / (max - min);
  }
}

void BoneMesh::planarMappingY()
{
  generateBoundingBox();

  float max = MAX(MAX(maxx, maxy), maxz);
  float min = MIN(MIN(minx, miny), minz);
  float dx = ((max - min) - (maxx - minx)) / 2;
  float dz = ((max - min) - (maxz - minz)) / 2;

  for (int i=0; i < triangles; i++) {
    triangle[i]->u1 =     (dz + triangle[i]->vertex1->initialPosition.z - minz) / (max - min);
    triangle[i]->u2 =     (dz + triangle[i]->vertex2->initialPosition.z - minz) / (max - min);
    triangle[i]->u3 =     (dz + triangle[i]->vertex3->initialPosition.z - minz) / (max - min);
    triangle[i]->v1 = 1 - (dx + triangle[i]->vertex1->initialPosition.x - minx) / (max - min);
    triangle[i]->v2 = 1 - (dx + triangle[i]->vertex2->initialPosition.x - minx) / (max - min);
    triangle[i]->v3 = 1 - (dx + triangle[i]->vertex3->initialPosition.x - minx) / (max - min);
  }
}

void BoneMesh::planarMappingZ()
{
  generateBoundingBox();

  float max = MAX(MAX(maxx, maxy), maxz);
  float min = MIN(MIN(minx, miny), minz);
  float dx = ((max - min) - (maxx - minx)) / 2;
  float dy = ((max - min) - (maxy - miny)) / 2;

  for (int i=0; i < triangles; i++) {
    triangle[i]->u1 =     (dx + triangle[i]->vertex1->initialPosition.x - minx) / (max - min);
    triangle[i]->u2 =     (dx + triangle[i]->vertex2->initialPosition.x - minx) / (max - min);
    triangle[i]->u3 =     (dx + triangle[i]->vertex3->initialPosition.x - minx) / (max - min);
    triangle[i]->v1 = 1 - (dy + triangle[i]->vertex1->initialPosition.y - miny) / (max - min);
    triangle[i]->v2 = 1 - (dy + triangle[i]->vertex2->initialPosition.y - miny) / (max - min);
    triangle[i]->v3 = 1 - (dy + triangle[i]->vertex3->initialPosition.y - miny) / (max - min);
  }
}

void BoneMesh::cylinderMapping()
{
  generateBoundingBox();

  // Now create coordinates
  // u is the vertical coordinate
  // v is the horizontal one so we have to calculate the angle
  for (int i=0; i < triangles; i++) {
    float u,v,angle;
    u = (triangle[i]->vertex1->initialPosition.y - miny) / (maxy - miny);
    angle = atan2(triangle[i]->vertex1->initialPosition.x,
	      triangle[i]->vertex1->initialPosition.z);
    v = (angle + M_PI) / (2 * M_PI);
    triangle[i]->u1 = u;
    triangle[i]->v1 = v;

    u = (triangle [i]->vertex2->initialPosition.y - miny) / (maxy - miny);
    angle = atan2(triangle[i]->vertex2->initialPosition.x,
	      triangle[i]->vertex2->initialPosition.z);
    v = (angle + M_PI) / (2 * M_PI);
    triangle[i]->u2 = u;
    triangle[i]->v2 = v;

    u = (triangle[i]->vertex3->initialPosition.y - miny) / (maxy - miny);
    angle = atan2(triangle[i]->vertex3->initialPosition.x,
	      triangle[i]->vertex3->initialPosition.z);
    v = (angle + M_PI) / (2 * M_PI);
    triangle[i]->u3 = u;
    triangle[i]->v3 = v;
  }
}

#endif
