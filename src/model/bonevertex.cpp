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
#include "bonevertex.hpp"
#include "bonelink.hpp"
#include "bone.hpp"
#include "file.hpp"	// openFile


BoneVertex::BoneVertex()
{
  initialPosition.set(0,0,0);
  initialAngle    = 0;
  initialAxis.    set(0,1,0);
  currentPosition.set(0,0,0);
  currentAngle    = 0;
  currentAxis.    set(0,1,0);

  child    = NULL;
  father   = NULL;
  children = 0;
  childListCompiled = 0;
  link     = NULL;
  links    = 0;
  linkListCompiled = 0;

  influenceScaleFactor = 1;
  animated = 0;
}

BoneVertex::BoneVertex(Vect3D & zePosition, float zeAngle, Vect3D & zeAxis)
{
  initialPosition = zePosition;
  initialAngle    = zeAngle;
  initialAxis     = zeAxis;
  currentPosition = zePosition;
  currentAngle    = zeAngle;
  currentAxis     = zeAxis;

  child    = NULL;
  father   = NULL;
  children = 0;
  childListCompiled = 0;
  link     = NULL;
  links    = 0;
  linkListCompiled = 0;
  animated = 0;
}

BoneVertex::BoneVertex(Vect3D *zePosition, float zeAngle, Vect3D *zeAxis)
{
  initialPosition = *zePosition;
  initialAngle    =  zeAngle;
  initialAxis     = *zeAxis;
  currentPosition = *zePosition;
  currentAngle    =  zeAngle;
  currentAxis     = *zeAxis;

  child    = NULL;
  father   = NULL;
  children = 0;
  childListCompiled = 0;
  link     = NULL;
  links    = 0;
  linkListCompiled = 0;
  animated = 0;
}

BoneVertex::~BoneVertex()
{
  // first delete all the children
  if (! childListCompiled) compileChildList();
  for (int i=0; i < children; i++) delete child[i];
  childList.empty();

  // Now, delete the selected links for this node
  if (! linkListCompiled) compileLinkList();
  for (int i=0; i < links; i++) delete link[i];
  linkList.empty();
}

// Accessing initial position datas
void BoneVertex::setInitialPosition(Vect3D &zePosition)
{
  initialPosition =  zePosition;
  currentPosition =  zePosition;
}

void BoneVertex::setInitialPosition(Vect3D *zePosition)
{
  initialPosition = *zePosition;
  currentPosition = *zePosition;
}

void BoneVertex::setInitialPosition(float ox, float oy, float oz)
{
  initialPosition = Vect3D(ox, oy, oz);
  currentPosition = Vect3D(ox, oy, oz);
}

void BoneVertex::setInitialRotation(float zeAngle, Vect3D &zeAxis)
{
  initialAngle    =  zeAngle;
  initialAxis     =  zeAxis;
  currentAngle    =  zeAngle;
  currentAxis     =  zeAxis;
}

void BoneVertex::setInitialRotation(float zeAngle, Vect3D *zeAxis)
{
  initialAngle    =  zeAngle;
  initialAxis     = *zeAxis;
  currentAngle    =  zeAngle;
  currentAxis     = *zeAxis;
}

void BoneVertex::setInitialRotation(float zeAngle, float axisx, float axisy, float axisz)
{
  initialAngle    =  zeAngle;
  initialAxis     =  Vect3D(axisx, axisy, axisz);
  currentAngle    =  zeAngle;
  currentAxis     =  Vect3D(axisx, axisy, axisz);
}

// Accessing animation position datas
void BoneVertex::setPos(Vect3D & zePosition)
{
  currentPosition =  zePosition;
}

void BoneVertex::setPos(Vect3D *zePosition)
{
  currentPosition = *zePosition;
}

void BoneVertex::setPos(float ox, float oy, float oz)
{
  currentPosition =  Vect3D(ox, oy, oz);
}

void BoneVertex::setRot(float zeAngle, Vect3D &zeAxis)
{
  currentAngle    =  zeAngle;
  currentAxis     =  zeAxis;
}

void BoneVertex::setRot(float zeAngle, Vect3D *zeAxis)
{
  currentAngle    =  zeAngle;
  currentAxis     = *zeAxis;
}

void BoneVertex::setRot(float zeAngle, float axisx, float axisy, float axisz)
{
  currentAngle    =  zeAngle;
  currentAxis     =  Vect3D(axisx, axisy, axisz);
}

// Accessing current position datas (during animation)
// with relative values (realtive to initial position)
void BoneVertex::resetPos()
{
  currentPosition = initialPosition;
}

void BoneVertex::resetRot()
{
  currentAngle = initialAngle;
  currentAxis  = initialAxis;
}

void BoneVertex::setTrans(Vect3D &delta)
{
  currentPosition = currentPosition + delta;
}

void BoneVertex::setTrans(Vect3D *delta)
{
  currentPosition = currentPosition + *delta;
}

void BoneVertex::setTrans(float dx, float dy, float dz)
{
  currentPosition.x += dx;
  currentPosition.y += dy;
  currentPosition.z += dz;
}

void BoneVertex::setScale(float scalex, float scaley, float scalez)
{
  currentPosition.x *= scalex;
  currentPosition.y *= scaley;
  currentPosition.z *= scalez;
}

void BoneVertex::setScale(float scale)
{
  currentPosition = currentPosition * scale;
}

// Modifying the node and its children (definitive)
void BoneVertex::scale(float sx, float sy, float sz)
{
  if (! childListCompiled) compileChildList();

  initialPosition.x *= sx;
  initialPosition.y *= sy;
  initialPosition.z *= sz;
  currentPosition.x *= sx;
  currentPosition.y *= sy;
  currentPosition.z *= sz;

  for (int i=0; i<children; i++)
    child[i]->scale(sx, sy, sz);
}

// Updating the father of this boneVertex
void BoneVertex::setFather(BoneVertex *zeFather)
{
  father = zeFather;
}

// Adding children
void BoneVertex::addChild(BoneVertex *newChild)
{
  childList.addElem(newChild);
  newChild->setFather(this);
  childListCompiled = 0;
}

// Removing a child and its children
void BoneVertex::removeChild(const char *zeName)
{
  BoneVertex *b = getBone(zeName);
  if (! b) return;
  if (b == this) return;

  childList.delElem(b);
  childListCompiled = 0;
  delete b;
}

// Finding a boneVertex in the tree using its name
BoneVertex *BoneVertex::getBone(const char *zeName)
{
  BoneVertex *result = NULL;

  if (! childListCompiled) compileChildList();

  if (strcmp(zeName, getName()) == 0)
    result = this;
  else {
    int i=0;
    while ((! result) && (i < children))
      result = child[i++]->getBone(zeName);
  }
  return result;
}

// Adding a link
void BoneVertex::addLink(BoneLink *zeLink)
{
  linkList.addElem(zeLink);
  linkListCompiled = 0;
}

// Removing a link
void BoneVertex::removeLink(BoneLink *zeLink)
{
  linkList.delElem(zeLink);
  linkListCompiled = 0;
}

// Compiling the lists
void BoneVertex::compileChildList()
{
  child = childList.getNiceTable(&children);
  childListCompiled = 1;
}

void BoneVertex::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
}

// Generating the initial matrix for this node
void BoneVertex::generateInitialMatrix()
{
  // First, we'll need to know all the childs
  if (! childListCompiled) compileChildList();

  glPushMatrix();

  // Now I let OpenGL calculate the matrix
  glTranslatef(initialPosition.x, initialPosition.y, initialPosition.z);
  glRotatef   (initialAngle, initialAxis.x , initialAxis.y, initialAxis.z);
  // save it
  glGetFloatv (GL_MODELVIEW_MATRIX, initialMatrix);

  // Here is a nice matrix inversion
  // 1. m gets the transposed rotation part of the matrix
  float m[16];
  m[0] = initialMatrix[ 0]; m[4] = initialMatrix[ 1]; m[ 8] = initialMatrix[ 2]; m[12] = 0;
  m[1] = initialMatrix[ 4]; m[5] = initialMatrix[ 5]; m[ 9] = initialMatrix[ 6]; m[13] = 0;
  m[2] = initialMatrix[ 8]; m[6] = initialMatrix[ 9]; m[10] = initialMatrix[10]; m[14] = 0;
  m[3] = 0;                 m[7] = 0;                 m[11] = 0;                 m[15] = 1;
  // 2. v gets an inverted translation from the matrix
  Vect3D v(-initialMatrix[12], -initialMatrix[13], -initialMatrix[14]);
  // 3. v is to be done in the inverted coordinate system so multiply
  v = m * v;
  // 4. now we have the nice inverted matrix :)
  m[12] = v.x;
  m[13] = v.y;
  m[14] = v.z;
  // 5. Stores it !
  for (int j=0; j<16 ; j++) initialMatrixInverted[j] = m[j];

  // Now, store the rotation matrices (for normals computation)
  int off=0;
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++, off++) {
      if ((i<3) && (j<3)) {
	initialRotMatrix        [off] = initialMatrix        [off];
	initialRotMatrixInverted[off] = initialMatrixInverted[off];
      }
      else if (i==j) {
	initialRotMatrix        [off] = 1;
	initialRotMatrixInverted[off] = 1;
      }
      else {
	initialRotMatrix        [off] = 0;
	initialRotMatrixInverted[off] = 0;
      }
    }
  }

  // Now, let's do the same for children :)
  for (int i=0; i < children; i++)
    child[i]->generateInitialMatrix();

  glPopMatrix();
}

// Now let's have a look to the current matrix, same code as above
void BoneVertex::generateCurrentMatrix()
{
  if (! childListCompiled) compileChildList();

  glPushMatrix();

  glTranslatef(currentPosition.x, currentPosition.y, currentPosition.z);
  glRotatef(currentAngle, currentAxis.x , currentAxis.y, currentAxis.z);
  glGetFloatv(GL_MODELVIEW_MATRIX, currentMatrix);

  // Now we have the current transformation matrix,
  // we'll extract the rotation part so we can
  // move the normals of the mesh correctly to
  // update the lighting !
  currentRotMatrix[ 0] = currentMatrix[ 0];
  currentRotMatrix[ 1] = currentMatrix[ 1];
  currentRotMatrix[ 2] = currentMatrix[ 2];
  currentRotMatrix[ 3] = 0;
  currentRotMatrix[ 4] = currentMatrix[ 4];
  currentRotMatrix[ 5] = currentMatrix[ 5];
  currentRotMatrix[ 6] = currentMatrix[ 6];
  currentRotMatrix[ 7] = 0;
  currentRotMatrix[ 8] = currentMatrix[ 8];
  currentRotMatrix[ 9] = currentMatrix[ 9];
  currentRotMatrix[10] = currentMatrix[10];
  currentRotMatrix[11] = 0;
  currentRotMatrix[12] = 0;
  currentRotMatrix[13] = 0;
  currentRotMatrix[14] = 0;
  currentRotMatrix[15] = 1;

  // And now, just generate the childrem matrices
  for (int i=0; i<children; i++)
    child[i]->generateCurrentMatrix();

  glPopMatrix();
}

// I/O functions
void BoneVertex::read(char *filename, float scale)
{
  FILE *fp = File::openFile(filename, "rb");
  if (! fp) {
    error("BoneVertex::read unable to open: [%s] for read", filename); return;
  }

  readFromFile(fp, scale);
  File::closeFile(fp);
}

void BoneVertex::readFromFile(FILE *fp, float scale)
{
  char nameCurrent[512];
  float posx, posy, posz;
  float angle;
  float axisx, axisy, axisz;

  readStr(fp, nameCurrent);

  posx = readFloat(fp) * scale;
  posy = readFloat(fp) * scale;
  posz = readFloat(fp) * scale;

  angle = readFloat(fp);
  axisx = readFloat(fp);
  axisy = readFloat(fp);
  axisz = readFloat(fp);

  setName(nameCurrent);
  setInitialPosition(posx, posy, posz);
  setInitialRotation(angle, axisx, axisy, axisz);

  int cpt = readInt(fp); // Number of children
  for (int i=0; i<cpt; i++) {
    BoneVertex *bv = new BoneVertex();
    addChild(bv);
    bv->readFromFile(fp);
  }

  compileChildList();
}

#if 0 //notused
void BoneVertex::write(char *filename)
{
  FILE *fp = File::openFile(filename, "wb");
  if (! fp) {
    printf("BoneVertex::write unable to open: [%s] for write\n", filename);
    return;
  }
  writeToFile(fp);
  File::closeFile(fp);
}

void BoneVertex::writeToFile(FILE *fp)
{
  if (! childListCompiled) compileChildList();

  writeString(fp, getName());

  float posx, posy, posz;
  float angle;
  float axisx, axisy, axisz;

  posx = initialPosition.x;
  posy = initialPosition.y;
  posz = initialPosition.z;
  angle = initialAngle;
  axisx = initialAxis.x;
  axisy = initialAxis.y;
  axisz = initialAxis.z;

  writeFloat(fp, posx);
  writeFloat(fp, posy);
  writeFloat(fp, posz);
  writeFloat(fp, angle);
  writeFloat(fp, axisx);
  writeFloat(fp, axisy);
  writeFloat(fp, axisz);

  writeInt(fp, children);

  for (int i=0; i<children; i++)
    child[i]->writeToFile(fp);
}

void BoneVertex::print(int cpt, FILE *fp)
{
  if (! childListCompiled) compileChildList();
  int i;

  for (i=0; i<cpt; i++)
    fprintf(fp, " |   ");
  if (children != 0) fprintf(fp, "[-] ");
  else               fprintf(fp, " |- ");
  fprintf(fp, "%s\n", getName());
  for (i=0; i<children; i++)
    child[i]->print(cpt+1, fp);
}
#endif


Vertex::Vertex()
{
  defaults();
}

Vertex::Vertex(Vect3D &zePosition)
{
  initialPosition = zePosition;
  currentPosition = zePosition;
  defaults();
}

Vertex::Vertex(Vect3D *zePosition)
{
  initialPosition = *zePosition;
  currentPosition = *zePosition;
  defaults();
}

Vertex::Vertex(float ox, float oy, float oz)
{
  initialPosition.set(ox, oy, oz);
  currentPosition.set(ox, oy, oz);
  defaults();
}

void Vertex::defaults()
{
  link  = NULL;
  links = 0;
  linkListCompiled = 0;
  initialNormal.reset();
  currentNormal.reset();
  u = -1.;
  v = -1.;
}

void Vertex::setPosition(Vect3D &zePosition)
{
  initialPosition =  zePosition;
  currentPosition =  zePosition;
}

void Vertex::setPosition(Vect3D *zePosition)
{
  initialPosition = *zePosition;
  currentPosition = *zePosition;
}

void Vertex::addLink(BoneLink *zeLink)
{
  linkList.addElem(zeLink);
  linkListCompiled = 0;
}

void Vertex::removeLink(BoneLink *zeLink)
{
  linkList.delElem(zeLink);
  linkListCompiled = 0;
}

void Vertex::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
}
