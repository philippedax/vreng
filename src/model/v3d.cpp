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
