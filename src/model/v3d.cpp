//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// v3d.cpp
//
// V3d I/Os
//
// Author   : This file has been written by Yann Renard
// Copyright: This file is totaly free and you may distribute
//            it to anyone you want, without modifying this
//            header. If you use it in a commercial project (?)
//            or in bigger project (!), I would be glad to know about it :) Please mail me...
//            be glad to know about it, please mail me  myself_yr@hotmail.com
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "v3d.hpp"
#include "bone.hpp"	// BoneMesh, BoneVertex
#include "file.hpp"	// open, close


V3d::V3d()
{
}

/** V3D internal format parser */
void V3d::readV3D(BoneMesh *result, BoneVertex *skel, char *filename, float scale)
{
  File *file = new File();
  FILE *fp = file->open(filename, "rb");
  if (! fp) return;

  //echo("readV3D: reading mesh and skeleton from V3D file %s", filename);

  // Reading name
  char name[128];

  readStr(fp, name);
  //echo("face: %s", name);
  result->setName(name);

  // Reading vertices
  int vertices = readInt(fp);
  for (int i=0; i < vertices; i++) {
    float x = file->read_float_be(fp) * scale;
    float y = file->read_float_be(fp) * scale;
    float z = file->read_float_be(fp) * scale;
    result->addVertex(x, y, z);
  }
  //echo("face: Vertices added: %i", vertices);

  int facets = file->read_long_be(fp);
  for (int i=0; i < facets; i++) {
    // Reading indices
    int index1 = file->read_long_be(fp);
    int index2 = file->read_long_be(fp);
    int index3 = file->read_long_be(fp);
    result->addTriangle(index1, index2, index3);
    BoneTriangle *triangle = result->triangleList.getElemAt(i);

    // Reading color
    float r = file->read_float_be(fp);
    float g = file->read_float_be(fp);
    float b = file->read_float_be(fp);
    float a = file->read_float_be(fp);
    triangle->setColor(r, g, b, a);

    // Reading texture coordinantes
    triangle->u1 = file->read_float_be(fp);
    triangle->v1 = file->read_float_be(fp);
    triangle->u2 = file->read_float_be(fp);
    triangle->v2 = file->read_float_be(fp);
    triangle->u3 = file->read_float_be(fp);
    triangle->v3 = file->read_float_be(fp);
  }
  //echo("face: Faces added   : %i", facets);

  // Reading skeleton
  skel->readSkel(fp, scale);
  result->rebuildNormals();
  file->close();
  delete file;
  //echo("skeleton added");
}


#if 0 //notused --------------------------------------------------------------------

void V3d::writeV3D(BoneMesh *outMesh, BoneVertex *skeletonRoot, char *filename)
{
  File *file = new File();
  FILE *fp = file->open(filename, "wb");
  if (! fp) return;

  if (! outMesh->vertexListCompiled) outMesh->compileVertexList();
  if (! outMesh->triangleListCompiled) outMesh->compileTriangleList();

  int i, index1, index2, index3;

  //echo("writeV3D: writing mesh and skeleton to V3D file");

  // Writing name
  writeString(fp, outMesh->getName());

  // Writing vertices coordinates
  writeInt(fp, outMesh->vertices);
  for (i=0; i < outMesh->vertices; i++) {
    writeFloat(fp, outMesh->vertex[i]->iniPosition.x);
    writeFloat(fp, outMesh->vertex[i]->iniPosition.y);
    writeFloat(fp, outMesh->vertex[i]->iniPosition.z);
  }
  //echo("           Vertices added: %i", outMesh->vertices);

  // Writing triangles
  writeInt(fp, outMesh->triangles);
  for (i=0; i < outMesh->triangles; i++) {
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
    writeFloat(fp, outMesh->triangle[i]->r);
    writeFloat(fp, outMesh->triangle[i]->g);
    writeFloat(fp, outMesh->triangle[i]->b);
    writeFloat(fp, outMesh->triangle[i]->a);
    // Writing triangle texture coordinates
    writeFloat(fp, outMesh->triangle[i]->u1);
    writeFloat(fp, outMesh->triangle[i]->v1);
    writeFloat(fp, outMesh->triangle[i]->u2);
    writeFloat(fp, outMesh->triangle[i]->v2);
    writeFloat(fp, outMesh->triangle[i]->u3);
    writeFloat(fp, outMesh->triangle[i]->v3);
  }
  //echo("           Faces added   : %i", outMesh->triangles);

  // Writing skeleton
  skeletonRoot->writeToFile(fp);
  //echo("           Skeleton added!");
  file->close();
  delete file;
}

void V3d::readVRMLfile(BoneMesh *result, char *filename, float size, float centerx, float centery, float centerz, int colorMask)
{
  //echo("Creating TRI mesh with vrml file");
  //echo("           Filename      : [%s]", filename);

  char endLine = '\0';
  char skippedText[1024];
  float x,y,z;
  int index1, index2, index3, indexEnd;
  int vertices=0, facets=0;

  // First look if the file exists
  File *file = new File();
  FILE *fp = file->open(filename, "rt");
  if (! fp) return;
  file->close();
  delete file;

  // Now that we know it exists, we'll look after the vertices part
  File *file2 = new File();
  fp = file2->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "point") != 0) && (!feof(fp))) {
    fscanf(fp, "%s", skippedText);
  }
  if (!feof(fp)) {
    fscanf(fp, "%s\n", skippedText);
    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%f %f %f%c", &x, &y, &z, &endLine);
      result->addVertex(centerx + x * size, centery + y * size, centerz + z * size);
      vertices ++;
    }
    //echo("           Vertices added: %i", vertices);
  }
  file2->close();
  delete file2;

  // We'll look after the facets part
  File *file3 = new File();
  fp = file3->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "coordIndex") != 0) && (!feof(fp))) {
    fscanf(fp, "%s", skippedText);
  }
  if (!feof(fp)) {
    fscanf(fp, "%s", skippedText);
    endLine = '\0';
    while (endLine != ']') {
      fscanf(fp, "%d, %d, %d, %d%c", &index1, &index2, &index3, &indexEnd, &endLine);
      result->addTriangle(index1, index2, index3);
      facets ++;
    }
    //echo("           Faces added   : %i", facets);
  }
  file3->close();
  delete file3;

  // We'll now go on color part
  File *file4 = new File();
  fp = file4->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "color") != 0) && (!feof(fp)))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp))
    fscanf(fp, "%s", skippedText);
  if (!feof(fp) && strcmp(skippedText, "NULL")) {
    BoneTriangle *triangle;
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
        r = (triangle->vertex1->iniPosition.x + 1) / 2;
        g = (triangle->vertex1->iniPosition.y + 1) / 2;
        b = (triangle->vertex1->iniPosition.z + 1) / 2;
      }
      triangle->setColor(r,g,b,1);
    }
  }
  file4->close();
  delete file4;

  // And finaly come on texture mapping coordinates
  File *file5 = new File();
  fp = file5->open(filename, "rt");
  strcpy(skippedText, "");
  while ((strcmp(skippedText, "texCoord") != 0) && (!feof(fp))) {
    fscanf(fp, "%s", skippedText);
  }
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
  file5->close();
  delete file5;
  result->rebuildNormals();
  result->setName(filename);
}

#endif //notused --------------------------------------------------------------------
