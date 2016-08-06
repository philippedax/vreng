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
#include "v3d.hpp"
#include "file.hpp"	// openFile


//-- VRENG 3D intern format
void V3d::readV3Dfile(BoneMesh *result, BoneVertex *skeletonRoot, char *filename, float scale)
{
  FILE *fp = File::openFile(filename, "rb");
  if (fp == NULL) return;

  trace(DBG_MAN, "readV3Dfile: reading mesh and skeleton from V3D file %s", filename);

  // Reading name
  char name[512];
  readString(fp, name);
  result->setName(name);

  // Reading vertices
  int vertices = readInt(fp);
  for (int i=0; i<vertices; i++) {
    float x = readFloat(fp) * scale;
    float y = readFloat(fp) * scale;
    float z = readFloat(fp) * scale;
    result->addVertex(x,y,z);
  }
  trace(DBG_MAN, "Vertices added: %i", vertices);

  int facets = readInt(fp);
  for (int i=0; i<facets; i++) {
    // Reading indices
    int index1 = readInt(fp);
    int index2 = readInt(fp);
    int index3 = readInt(fp);
    result->addTriangle(index1, index2, index3);
    Triangle *triangle = result->triangleList.getElemAt(i);
    // Reading color
    float r = readFloat(fp);
    float g = readFloat(fp);
    float b = readFloat(fp);
    float a = readFloat(fp);
    triangle->setColor(r,g,b,a);
    // Reading texture coordinantes
    triangle->u1 = readFloat(fp);
    triangle->v1 = readFloat(fp);
    triangle->u2 = readFloat(fp);
    triangle->v2 = readFloat(fp);
    triangle->u3 = readFloat(fp);
    triangle->v3 = readFloat(fp);
  }
  trace(DBG_MAN, "Faces added   : %i", facets);

  // Reading skeleton
  skeletonRoot->readFromFile(fp, scale);
  trace(DBG_MAN, "Skeleton added!");
  File::closeFile(fp);
  result->rebuildNormals();
}

#if 0 //not used

void V3d::writeV3Dfile(BoneMesh *outMesh, BoneVertex *skeletonRoot, char *filename)
{
  FILE *fp = File::openFile(filename, "wb");
  if (fp == NULL) return;

  if (! outMesh->vertexListCompiled) outMesh->compileVertexList();
  if (! outMesh->triangleListCompiled) outMesh->compileTriangleList();

  int i, index1, index2, index3;

  trace(DBG_FORCE, "writeV3Dfile: writing mesh and skeleton to V3D file");

  // Writing name
  writeString(fp, outMesh->getName());

  // Writing vertices coordinates
  writeInt(fp, outMesh->vertices);
  for (i=0; i<outMesh->vertices; i++) {
    writeFloat(fp, outMesh->vertex[i]->initialPosition.x);
    writeFloat(fp, outMesh->vertex[i]->initialPosition.y);
    writeFloat(fp, outMesh->vertex[i]->initialPosition.z);
  }
  trace(DBG_FORCE, "           Vertices added: %i", outMesh->vertices);

  // Writing triangles
  writeInt(fp, outMesh->triangles);
  for (i=0; i<outMesh->triangles; i++) {
    index1 = 0;
    index2 = 0;
    index3 = 0;
    while (outMesh->vertex[index1] != outMesh->triangle[i]->vertex1) index1++;
    while (outMesh->vertex[index2] != outMesh->triangle[i]->vertex2) index2++;
    while (outMesh->vertex[index3] != outMesh->triangle[i]->vertex3) index3++;
    // Writing indices
    writeInt(fp, index1);
    writeInt(fp, index2);
    writeInt(fp, index3);
    // Writing triangle color
    writeFloat(fp, outMesh->triangle[i]->colorRed);
    writeFloat(fp, outMesh->triangle[i]->colorGreen);
    writeFloat(fp, outMesh->triangle[i]->colorBlue);
    writeFloat(fp, outMesh->triangle[i]->colorAlpha);
    // Writing triangle texture coordinates
    writeFloat(fp, outMesh->triangle[i]->u1);
    writeFloat(fp, outMesh->triangle[i]->v1);
    writeFloat(fp, outMesh->triangle[i]->u2);
    writeFloat(fp, outMesh->triangle[i]->v2);
    writeFloat(fp, outMesh->triangle[i]->u3);
    writeFloat(fp, outMesh->triangle[i]->v3);
  }
  trace(DBG_FORCE, "           Faces added   : %i", outMesh->triangles);

  // Writing skeleton
  skeletonRoot->writeToFile(fp);
  trace(DBG_FORCE, "           Skeleton added!");
  File::closeFile(fp);
}

void V3d::readVRMLfile(BoneMesh *result, char *filename, float size, float centerx, float centery, float centerz, int colorMask)
{
  trace(DBG_MAN, "Creating TRI mesh with vrml file");
  trace(DBG_MAN, "           Filename      : [%s]", filename);

  char endLine = '\0';
  char skippedText[1024];
  float x,y,z;
  int index1, index2, index3, indexEnd;
  int vertices=0, facets=0;

  // First look if the file exists
  FILE *fp = File::openFile(filename, "rt");
  if (fp == NULL) return;
  File::closeFile(fp);

  // Now that we know it exists, we'll look after the vertices part
  fp = File::openFile(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "point") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp)) {
    fscanf(fp, "%s\n", skippedText);
    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f %f%c", &x, &y, &z, &endLine);
      result->addVertex(centerx + x * size, centery + y * size, centerz + z * size);
      vertices ++;
    }
    trace(DBG_MAN, "           Vertices added: %i", vertices);
  }
  File::closeFile(fp);

  // We'll look after the facets part
  fp = File::openFile(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "coordIndex") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp)) {
    fscanf(fp, "%s", skippedText);
    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%d, %d, %d, %d%c", &index1, &index2, &index3, &indexEnd, &endLine);
      result->addTriangle(index1, index2, index3);
      facets ++;
    }
    trace(DBG_MAN, "           Faces added   : %i", facets);
  }
  File::closeFile(fp);

  // We'll now go on color part
  fp = File::openFile(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "color") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp) && strcmp(skippedText, "NULL")) {
    Triangle * triangle;
    float r,g,b;
    int cpt=0;

    printf("[%s]\n", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);

    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f %f%c", &r, &g, &b, &endLine);
      triangle = result->triangleList.getElemAt(cpt++);

      if ((r==g) && (g==b) && (colorMask != 0)) {
        r = (triangle->vertex1->initialPosition.x + 1) / 2;
        g = (triangle->vertex1->initialPosition.y + 1) / 2;
        b = (triangle->vertex1->initialPosition.z + 1) / 2;
      }
      triangle->setColor(r,g,b,1);
    }
  }
  File::closeFile(fp);

  // And finaly come on texture mapping coordinates
  fp = File::openFile(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "texCoord") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp)) {
    Vertex * vertex;
    float u,v;
    int cpt=0;

    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);
    fscanf(fp, "%s", skippedText);

    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f%c", &u, &v, &endLine);
      vertex = result->vertexList.getElemAt(cpt++);
      vertex->u = u;
      vertex->v = v;
    }
  }
  File::closeFile(fp);
  result->rebuildNormals();
  result->setName(filename);
}

#endif //not used
