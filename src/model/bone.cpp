//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
// bone.cpp
//
// Author   : Yann Renard
// Copyright: This file is totaly free and you may distribute
//            it to anyone you want, without modifying this
//            header. If you use it in a commercial project (?)
//            or in bigger project (!), I would be glad to know about it :)
//            myself_yr@hotmail.com
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "bone.hpp"
#include "file.hpp"	// open, close


Bone::Bone()
{
  mesh = NULL;
  skel = NULL;
  link = NULL;
  links = 0;
  compiled = 0;
}

Bone::~Bone()
{
  //dax this line is correct, but temporally commented because
  //dax a segfault occurs later in the delLink destructor
  //dax emptyLinkList();
}

//-----------------
// Accessing datas
void Bone::registerMesh(BoneMesh *_mesh)
{
  mesh = _mesh;
  //echo("registered as mesh: [%s]", mesh->getName());
}

void Bone::registerSkel(BoneVertex *_skel)
{
  skel = _skel;
  //echo("registered as skel: [%s]", skel->getName());
}

//--------------------------------------
// Scales on both mesh and skel
void Bone::scale(float sx, float sy, float sz)
{
  if (! mesh) return;
  if (! skel) return;

  mesh->scale(sx, sy, sz);
  skel->scale(sx, sy, sz);
  genLinkList();
}

//------------------
// Links management
void Bone::makeLinkList()
{
  link = linkList.getTable(&links);
  compiled = 1;
}

void Bone::emptyLinkList()
{
  if (! compiled) makeLinkList();

  for (int i=0; i < links; i++) {
    delete link[i];
  }

  linkList.empty();
  makeLinkList();
}

float Bone::getLength(Vertex *vertex, BoneVertex *node)
{
  Vect3D nullvect(0, 0, 0);

  // First calculate the node's absolute position
  Vect3D nodePosition = node->iniMatrix * nullvect;

  // then for each vertex, try to find the distance to the node
  Vect3D distance = nodePosition - vertex->iniPos;
  return distance.length();
}

void getDist(Vertex *vertex, BoneVertex *a, BoneVertex *b, float *time, float *dist)
{
  Vect3D nullvect(0, 0, 0);
  Vect3D aPosition = a->iniMatrix * nullvect;
  Vect3D bPosition = b->iniMatrix * nullvect;
  Vect3D abVector = bPosition - aPosition;
  Vect3D paVector = aPosition - vertex->iniPos;

  *time = -Vect3D::dotProduct(paVector, abVector) / Vect3D::dotProduct(abVector, abVector);
  *time = CROP(0.0001, *time, 0.9999);

  Vect3D phVector;
  phVector = abVector;
  phVector = phVector * *time;
  phVector = phVector + paVector;
  *dist = Vect3D::dotProduct(phVector, phVector);
  *dist = (*dist > 0.00000001 ? *dist : 0.00000001);
}

float Bone::getWeight(Vertex *vertex, BoneVertex *node)
{
  float result = 0;
  float dist, time;

  if (node->father) {
    getDist(vertex, node->father, node, &time, &dist);
    result += (time) / (dist * dist);
  }

  for (int i=0; i < node->children; i++) {
    getDist(vertex, node->child[i], node, &time, &dist);
    result += (time) / (dist * dist);
  }

  result /= node->children + (node->father == NULL ? 0:1);
  result *= node->influenceScaleFactor;
  return result;
}

void normalize(BoneLink **tlink, int tlinks)
{
  float totalWeight = 0;

  for (int i=0; i < tlinks; i++) {
    if (tlink[i])
      totalWeight += tlink[i]->weight;
  }
  for (int i=0; i < tlinks; i++) {
    if (tlink[i])
      tlink[i]->weight /= totalWeight;
  }
}

// -> Will call the method to generate links and then update the weight
// and optionally remove unsignificant links (that would speed down the cpu)
void Bone::genLinkList()
{
  // we need both mesh and skel !
  if (! mesh) return; // to avoid NULL pointer exception
  if (! skel) return; // to avoid NULL pointer exception

  BoneVertex *tnode;
  tnode = skel->getBone("root");
  if (tnode)
    tnode->influenceScaleFactor = 10;
  tnode = skel->getBone("lipsRoot");
  if (tnode)
    tnode->influenceScaleFactor = 10;
  tnode = skel->getBone("frontRoot");
  if (tnode)
    tnode->influenceScaleFactor = 10;

  // we start link generation with an empty link list of course
  emptyLinkList();

  // if the mesh has not compiled hos vertices list, we'll do it
  if (! mesh->vertexDone)
    mesh->makeVertexList();

  // first, we need to generate the initial matrices for all the nodes of the skel
  glPushMatrix();
   glLoadIdentity();
   skel->genIniMatrix();
  glPopMatrix();

  // we'll store the skel nodes into a list
  BoneList <BoneVertex> nodeList;
  BoneVertex **node;
  int nodes;

  addNodeAndChildren(skel, &nodeList);
  node = nodeList.getTable(&nodes);

  // now we'll go throw each vertex of the mesh calculating the weight of this node
  BoneList <BoneLink> tlinkList;
  BoneLink **tlink;
  BoneLink *temp;
  int  tlinks;

  for (int i=0; i < mesh->vertices; i++) {
    tlinkList.empty();
    for (int j=0; j < nodes; j++) {
      // create a link between the vertex and the node with an influence
      // proportional to the inverse of the distance ( so far vertices
      // will be less influenced by the node than near vertices )
      temp = new BoneLink(mesh->vertex[i], node[j], getWeight(mesh->vertex[i], node[j]));

      // we now save this new link in our list
      tlinkList.add(temp);
    }
    tlink = tlinkList.getTable(&tlinks);

    // now sorting the links per weight
    for (int j=1; j < tlinks; j++) {
      temp = tlink[j];
      int k = j;
      while ((k > 0) && (tlink[k-1]->weight < temp->weight)) {
	tlink[k] = tlink[k-1];
	k--;
      }
      tlink[k] = temp;
    }
    // removing unsignificant links
    float seuil = 0.3 * tlink[0]->weight;
    for (int j=0; j < tlinks; j++) {
      if (tlink[j]->weight < seuil) {
        delete tlink[j];
        tlink[j] = NULL;
      }
    }
    // record the selected links in the list
    normalize(tlink, tlinks);
    for (int j=0; j < tlinks; j++) {
      if (tlink[j])
        linkList.add(tlink[j]);
    }
  }

  // now that we have all the links, we may compile the link list in here
  makeLinkList();

  for (int i=0; i < links; i++) {
    link[i]->notifyTarget();
  }
  for (int i=0; i < mesh->vertices; i++) {
    mesh->vertex[i]->makeLinkList();
  }
  //echo("selected links: [%2.2f%%]", (links*100.) / (mesh->vertices * nodes));
}

// Main rendering method, will draw the skel and the mesh
void Bone::render()
{
  //echo("render bone");
  // now, we'll render the 3d mesh
  if (! mesh->triangleDone) {
    mesh->makeTriangleList();
  }

  BoneTriangle *tri;
  Vect3D *normal;
  Vect3D *v1, *v2, *v3;
  Vect3D *n1, *n2, *n3;

  glColor3f(0.7, 0.7, 0.8);

  glPushMatrix();
  //glEnable(GL_LIGHTING);
  //glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_DIFFUSE);

  glBegin(GL_TRIANGLES);
  for (int i=0; i < mesh->triangles; i++) {
    tri = mesh->triangle[j];
    glColor3f(tri->R, tri->G, tri->B);
    //glTexCoord2f(tri->u1, tri->v1);
    normal = &tri->iniNormal;
    glNormal3f(normal->x, normal->y, normal->z);

    // vertexes
    //echo("RGB: %.1f %.1f %.1f rgb: %.1f %.1f %.1f", tri->R,tri->G,tri->B,tri->r,tri->g,tri->b);
    v1 = &tri->vertex1->curPos;
    v2 = &tri->vertex2->curPos;
    v3 = &tri->vertex3->curPos;
    glVertex3f(v1->x, v1->y, v1->z);
    glVertex3f(v2->x, v2->y, v2->z);
    glVertex3f(v3->x, v3->y, v3->z);
    //echo("%.1f %.1f %.1f, %.1f %.1f %.1f, %.1f %.1f %.1f", v1->x, v1->y, v1->z, v2->x, v2->y, v2->z, v3->x, v3->y, v3->z);

    // normals
    n1 = &tri->vertex1->curNormal;
    n2 = &tri->vertex2->curNormal;
    n3 = &tri->vertex3->curNormal;
    //glNormal3f(n1->x, n1->y, n1->z);
    //glNormal3f(n2->x, n2->y, n2->z);
    //glNormal3f(n3->x, n3->y, n3->z);
  }
  glEnd();
  //glDisable(GL_TEXTURE_2D);
  //glDisable(GL_LIGHTING);
  glPopMatrix();
}

// Private methods
void Bone::addNodeAndChildren(BoneVertex *boneVertex, BoneList <BoneVertex> *list)
{
  if (! boneVertex->childDone)
    boneVertex->makeChildList();
  list->add(boneVertex);

  for (int i=0; i < boneVertex->children; i++) {
    addNodeAndChildren(boneVertex->child[i], list);
  }
}

//---------------------------------------------------------------------------

BoneLink::BoneLink(Vertex *_vertex, BoneVertex *_boneVertex = NULL, float _weight = 0.)
{
  vertex     = NULL;
  boneVertex = NULL;
  setVertex(_vertex);
  setBoneVertex(_boneVertex);
  setWeight(_weight);
}

BoneLink::~BoneLink()
{
  if (vertex) vertex->delLink(this);
  if (boneVertex) boneVertex->delLink(this);
}

void BoneLink::setVertex(Vertex *_vertex)
{
  if (vertex) vertex->delLink(this);
  vertex = _vertex;
}

void BoneLink::setBoneVertex(BoneVertex *_boneVertex)
{
  if (boneVertex) boneVertex->delLink(this);
  boneVertex = _boneVertex;
}

void BoneLink::notifyTarget()
{
  if (vertex) vertex->addLink(this);
  if (boneVertex) boneVertex->addLink(this);
}

void BoneLink::setWeight(float _weight)
{
  weight = _weight;
}

//---------------------------------------------------------------------------

BoneMesh::BoneMesh()
{
  vertex = NULL;
  vertices = 0;
  vertexDone = 0;
  triangle = NULL;
  triangles = 0;
  triangleDone = 0;

  setName(const_cast<char*> (" Mesh NoName "));
}

BoneMesh::~BoneMesh()
{
  if (! vertexDone) makeVertexList();
  if (! triangleDone) makeTriangleList();

  for (int i=0; i < vertices; i++) delete vertex[i];
  for (int j=0; j < triangles; j++) delete triangle[j];
}

void BoneMesh::addVertex(Vect3D &pos)
{
  vertexList.add(new Vertex(pos));
  vertexDone = 0;
}

void BoneMesh::addVertex(Vect3D *pos)
{
  vertexList.add(new Vertex(pos));
  vertexDone = 0;
}

void BoneMesh::addVertex(float ox, float oy, float oz)
{
  vertexList.add(new Vertex(ox, oy, oz));
  vertexDone = 0;
}

void BoneMesh::addTriangle(int index1, int index2, int index3)
{
  if (! vertexDone) makeVertexList();

  BoneTriangle * tri = new BoneTriangle();
  tri->addVertex(vertex[index1], index1, -1, -1);
  tri->addVertex(vertex[index2], index2, -1, -1);
  tri->addVertex(vertex[index3], index3, -1, -1);

  triangleList.add(tri);
  triangleDone = 0;
}

void BoneMesh::scale(float sx, float sy, float sz)
{
  if (! vertexDone) makeVertexList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->iniPos.x *= sx;
    vertex[i]->iniPos.y *= sy;
    vertex[i]->iniPos.z *= sz;
    vertex[i]->curPos.x *= sx;
    vertex[i]->curPos.y *= sy;
    vertex[i]->curPos.z *= sz;
  }
}

void BoneMesh::rebuildNormals()
{
  if (! vertexDone) makeVertexList();
  if (! triangleDone) makeTriangleList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->iniNormal.reset();
  }
  for (int i=0; i < triangles; i++) {
    triangle[i]->rebuildNormal();
    triangle[i]->vertex1->iniNormal += triangle[i]->iniNormal;
    triangle[i]->vertex2->iniNormal += triangle[i]->iniNormal;
    triangle[i]->vertex3->iniNormal += triangle[i]->iniNormal;
  }
  for (int i=0; i < vertices; i++) {
    vertex[i]->iniNormal.normalize();
  }
  projectLight();
}

void BoneMesh::projectLight()
{
  if (! triangleDone) makeTriangleList();

  Vect3D lightdir(0,0,1);
  Vect3D lightdiff(1,1,1);
  Vect3D lightspec(1,1,1);

  for (int i=0; i < triangles; i++) {
    Vect3D mat(triangle[i]->r, triangle[i]->g, triangle[i]->b);
    float cosine = Vect3D::dotProduct(lightdir, triangle[i]->iniNormal);
    cosine = CROP(-1, cosine, 1);
    if (cosine > 0) {
      float power = pow(cosine, 64);
      mat.x *= lightdiff.x * cosine + lightspec.x * power;
      mat.y *= lightdiff.y * cosine + lightspec.y * power;
      mat.z *= lightdiff.z * cosine + lightspec.z * power;
      if (mat.x > 1) mat.x = 1;
      if (mat.y > 1) mat.y = 1;
      if (mat.z > 1) mat.z = 1;
    }
    else mat.reset();

    triangle[i]->R = mat.x;
    triangle[i]->G = mat.y;
    triangle[i]->B = mat.z;
  }
}

void BoneMesh::makeVertexList()
{
  vertex = vertexList.getTable(&vertices);
  vertexDone = 1;
}

void BoneMesh::makeTriangleList()
{
  triangle = triangleList.getTable(&triangles);
  triangleDone = 1;

  // moving textureCoordinate vertex up to face
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

//---------------------------------------------------------------------------

Bonename::Bonename()
{
  name = NULL;
  setName(const_cast<char *>("NoName"));
}

Bonename::Bonename(char *op)
{
  name = new char[strlen(op) + 1];
  strcpy(name, op);
}

Bonename::Bonename(Bonename *op)
{
  name = new char[strlen(op->name) + 1];
  strcpy(name, op->name);
}

Bonename::Bonename(Bonename &op)
{
  name = new char[strlen(op.name) + 1];
  strcpy(name, op.name);
}

Bonename::~Bonename()
{
  if (name) delete[] name;
  name = NULL;
}

void Bonename::setName(char *_name)
{
  if (name) delete[] name;
  name = new char[strlen(_name) + 1];
  strcpy(name, _name);
}

char * Bonename::getName()
{
  return name;
}

//---------------------------------------------------------------------------

BoneVertex::BoneVertex()
{
  iniPos.set(0,0,0);
  iniAngle = 0;
  iniAxis. set(0,1,0);
  curPos.set(0,0,0);
  curAngle = 0;
  curAxis. set(0,1,0);

  child    = NULL;
  father   = NULL;
  children = 0;
  childDone = 0;
  link     = NULL;
  links    = 0;
  compiled = 0;

  influenceScaleFactor = 1;
}

BoneVertex::BoneVertex(Vect3D &pos, float angle, Vect3D &axis)
{
  iniPos = pos;
  iniAngle = angle;
  iniAxis  = axis;
  curPos = pos;
  curAngle = angle;
  curAxis  = axis;

  child    = NULL;
  father   = NULL;
  children = 0;
  childDone = 0;
  link     = NULL;
  links    = 0;
  compiled = 0;
}

BoneVertex::BoneVertex(Vect3D *pos, float angle, Vect3D *axis)
{
  iniPos = *pos;
  iniAngle =  angle;
  iniAxis  = *axis;
  curPos = *pos;
  curAngle =  angle;
  curAxis  = *axis;

  child    = NULL;
  father   = NULL;
  children = 0;
  childDone = 0;
  link     = NULL;
  links    = 0;
  compiled = 0;
}

BoneVertex::~BoneVertex()
{
  // first delete all the children
  if (! childDone) makeChildList();
  for (int i=0; i < children; i++) {
    delete child[i];
  }
  childList.empty();

  // delete the selected links for this node
  if (! compiled) makeLinkList();
  for (int i=0; i < links; i++) {
    //dax-segfault delete link[i];
  }
  linkList.empty();
}

// Accessing initial pos datas
void BoneVertex::setIniPos(Vect3D &pos)
{
  iniPos = pos;
  curPos = pos;
}

void BoneVertex::setIniPos(Vect3D *pos)
{
  iniPos = *pos;
  curPos = *pos;
}

void BoneVertex::setIniPos(float ox, float oy, float oz)
{
  iniPos = Vect3D(ox, oy, oz);
  curPos = Vect3D(ox, oy, oz);
}

void BoneVertex::setIniRot(float angle, Vect3D &axis)
{
  iniAngle = angle;
  iniAxis  = axis;
  curAngle = angle;
  curAxis  = axis;
}

void BoneVertex::setIniRot(float angle, Vect3D *axis)
{
  iniAngle = angle;
  iniAxis  = *axis;
  curAngle = angle;
  curAxis  = *axis;
}

void BoneVertex::setIniRot(float angle, float axisx, float axisy, float axisz)
{
  iniAngle = angle;
  iniAxis  = Vect3D(axisx, axisy, axisz);
  curAngle = angle;
  curAxis  = Vect3D(axisx, axisy, axisz);
}

// Accessing animation position datas
void BoneVertex::setPos(Vect3D &pos)
{
  curPos = pos;
}

void BoneVertex::setPos(Vect3D *pos)
{
  curPos = *pos;
}

void BoneVertex::setPos(float ox, float oy, float oz)
{
  curPos = Vect3D(ox, oy, oz);
}

void BoneVertex::setRot(float angle, Vect3D &axis)
{
  curAngle = angle;
  curAxis  = axis;
}

void BoneVertex::setRot(float angle, Vect3D *axis)
{
  curAngle = angle;
  curAxis  = *axis;
}

void BoneVertex::setRot(float angle, float axisx, float axisy, float axisz)
{
  curAngle = angle;
  curAxis  = Vect3D(axisx, axisy, axisz);
}

// Accessing current position datas (during animation)
// with relative values (realtive to initial position)
void BoneVertex::setPos()
{
  curPos = iniPos;
}

void BoneVertex::setRot()
{
  curAngle = iniAngle;
  curAxis  = iniAxis;
}

void BoneVertex::setTrans(Vect3D &delta)
{
  curPos = curPos + delta;
}

void BoneVertex::setTrans(Vect3D *delta)
{
  curPos = curPos + *delta;
}

void BoneVertex::setTrans(float dx, float dy, float dz)
{
  curPos.x += dx;
  curPos.y += dy;
  curPos.z += dz;
}

void BoneVertex::setScale(float scalex, float scaley, float scalez)
{
  curPos.x *= scalex;
  curPos.y *= scaley;
  curPos.z *= scalez;
}

void BoneVertex::setScale(float scale)
{
  curPos = curPos * scale;
}

// Modifying the node and its children (definitive)
void BoneVertex::scale(float sx, float sy, float sz)
{
  if (! childDone) makeChildList();

  iniPos.x *= sx;
  iniPos.y *= sy;
  iniPos.z *= sz;
  curPos.x *= sx;
  curPos.y *= sy;
  curPos.z *= sz;

  for (int i=0; i < children; i++) {
    child[i]->scale(sx, sy, sz);
  }
}

// Updating the father of this boneVertex
void BoneVertex::setBone(BoneVertex *bv)
{
  father = bv;
}

// Adding children
void BoneVertex::addBone(BoneVertex *child)
{
  childList.add(child);
  child->setBone(this);
  childDone = 0;
}

// Removing a child and its children
void BoneVertex::delBone(const char *name)
{
  BoneVertex *b = getBone(name);
  if (! b) return;
  if (b == this) return;

  childList.del(b);
  childDone = 0;
  delete b;
}

// Finding a boneVertex in the tree using its name
BoneVertex *BoneVertex::getBone(const char *name)
{
  BoneVertex *res = NULL;

  if (! childDone) makeChildList();

  if (! strcmp(name, getName())) {
    res = this;
  }
  else {
    int i=0;
    while ((! res) && (i < children)) {
      res = child[i++]->getBone(name);
    }
  }
  return res;
}

// Adding a link
void BoneVertex::addLink(BoneLink *link)
{
  linkList.add(link);
  compiled = 0;
}

// Removing a link
void BoneVertex::delLink(BoneLink *link)
{
  linkList.del(link);
  compiled = 0;
}

// Compiling the lists
void BoneVertex::makeChildList()
{
  child = childList.getTable(&children);
  childDone = 1;
}

void BoneVertex::makeLinkList()
{
  link = linkList.getTable(&links);
  compiled = 1;
}

// Generating the initial matrix for this node
void BoneVertex::genIniMatrix()
{
  // First, we'll need to know all the childs
  if (! childDone) makeChildList();

  glPushMatrix();

  // let OpenGL calculate the matrix
  glTranslatef(iniPos.x, iniPos.y, iniPos.z);
  glRotatef(iniAngle, iniAxis.x, iniAxis.y, iniAxis.z);
  // save it
  glGetFloatv(GL_MODELVIEW_MATRIX, iniMatrix);

  // here is a nice matrix inversion
  // 1. m gets the transposed rotation part of the matrix
  float m[16];
  m[0] = iniMatrix[ 0]; m[4] = iniMatrix[ 1]; m[ 8] = iniMatrix[ 2]; m[12] = 0;
  m[1] = iniMatrix[ 4]; m[5] = iniMatrix[ 5]; m[ 9] = iniMatrix[ 6]; m[13] = 0;
  m[2] = iniMatrix[ 8]; m[6] = iniMatrix[ 9]; m[10] = iniMatrix[10]; m[14] = 0;
  m[3] = 0;             m[7] = 0;             m[11] = 0;             m[15] = 1;
  // 2. v gets an inverted translation from the matrix
  Vect3D v(-iniMatrix[12], -iniMatrix[13], -iniMatrix[14]);
  // 3. v is to be done in the inverted coordinate system so multiply
  v = m * v;
  // 4. now we have the nice inverted matrix :)
  m[12] = v.x;
  m[13] = v.y;
  m[14] = v.z;
  // 5. Stores it !
  for (int j=0; j<16 ; j++) {
    iniMatrixInverted[j] = m[j];
  }

  // store the rotation matrices (for normals computation)
  int off = 0;
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++, off++) {
      if ((i<3) && (j<3)) {
	iniRotMatrix        [off] = iniMatrix        [off];
	iniRotMatrixInverted[off] = iniMatrixInverted[off];
      }
      else if (i==j) {
	iniRotMatrix        [off] = 1;
	iniRotMatrixInverted[off] = 1;
      }
      else {
	iniRotMatrix        [off] = 0;
	iniRotMatrixInverted[off] = 0;
      }
    }
  }

  // let's do the same for children :)
  for (int i=0; i < children; i++) {
    child[i]->genIniMatrix();
  }

  glPopMatrix();
}

// let's have a look to the current matrix, same code as above
void BoneVertex::genCurrMatrix()
{
  if (! childDone) makeChildList();

  glPushMatrix();

  glTranslatef(curPos.x, curPos.y, curPos.z);
  glRotatef(curAngle, curAxis.x , curAxis.y, curAxis.z);
  glGetFloatv(GL_MODELVIEW_MATRIX, curMatrix);

  // we have the current transformation matrix,
  // we'll extract the rotation part so we can
  // move the normals of the mesh correctly to update the lighting !
  curRotMatrix[ 0] = curMatrix[ 0];
  curRotMatrix[ 1] = curMatrix[ 1];
  curRotMatrix[ 2] = curMatrix[ 2];
  curRotMatrix[ 3] = 0;
  curRotMatrix[ 4] = curMatrix[ 4];
  curRotMatrix[ 5] = curMatrix[ 5];
  curRotMatrix[ 6] = curMatrix[ 6];
  curRotMatrix[ 7] = 0;
  curRotMatrix[ 8] = curMatrix[ 8];
  curRotMatrix[ 9] = curMatrix[ 9];
  curRotMatrix[10] = curMatrix[10];
  curRotMatrix[11] = 0;
  curRotMatrix[12] = 0;
  curRotMatrix[13] = 0;
  curRotMatrix[14] = 0;
  curRotMatrix[15] = 1;

  // and now, just generate the childrem matrices
  for (int i=0; i < children; i++) {
    child[i]->genCurrMatrix();
  }

  glPopMatrix();
}

// I/O functions
void BoneVertex::read(char *filename, float scale)
{
  File *file = new File();
  FILE *fp = file->open(filename, "rb");
  if (! fp) {
    error("BoneVertex::read unable to open: [%s]", filename);
    return;
  }
  readSkel(fp, scale);
  file->close();
  delete file;
}

void BoneVertex::readSkel(FILE *fp, float scale)
{
  char name[128];

  readStr(fp, name);

  float posx = readFloat(fp) * scale;
  float posy = readFloat(fp) * scale;
  float posz = readFloat(fp) * scale;
  float angle = readFloat(fp);
  float axisx = readFloat(fp);
  float axisy = readFloat(fp);
  float axisz = readFloat(fp);

  setName(name);
  setIniPos(posx, posy, posz);
  setIniRot(angle, axisx, axisy, axisz);

  int n = readInt(fp); // number of children
  for (int i=0; i < n; i++) {
    BoneVertex *bv = new BoneVertex();
    addBone(bv);
    bv->readSkel(fp, scale);
  }
  makeChildList();
}

//---------------------------------------------------------------------------

Vertex::Vertex()
{
  defaults();
}

Vertex::Vertex(Vect3D &pos)
{
  iniPos = pos;
  curPos = pos;
  defaults();
}

Vertex::Vertex(Vect3D *pos)
{
  iniPos = *pos;
  curPos = *pos;
  defaults();
}

Vertex::Vertex(float ox, float oy, float oz)
{
  iniPos.set(ox, oy, oz);
  curPos.set(ox, oy, oz);
  defaults();
}

void Vertex::defaults()
{
  link  = NULL;
  links = 0;
  compiled = 0;
  iniNormal.reset();
  curNormal.reset();
  u = -1;
  v = -1;
}

void Vertex::setPos(Vect3D &pos)
{
  iniPos = pos;
  curPos = pos;
}

void Vertex::setPos(Vect3D *pos)
{
  iniPos = *pos;
  curPos = *pos;
}

void Vertex::addLink(BoneLink *link)
{
  linkList.add(link);
  compiled = 0;
}

void Vertex::delLink(BoneLink *link)
{
  //dax-segfault linkList.del(link);
  compiled = 0;
}

void Vertex::makeLinkList()
{
  link = linkList.getTable(&links);
  compiled = 1;
}

//---------------------------------------------------------------------------

/** Constructor / destructor */
BoneTriangle::BoneTriangle()
{
  vertex1 = NULL;
  vertex2 = NULL;
  vertex3 = NULL;
  iniNormal.reset();
  curNormal.reset();
  setColor(.5, .5, .5, 1);
}

BoneTriangle::~BoneTriangle() {}

/** Accessing datas */
void BoneTriangle::addVertex(Vertex *_vertex, int index, float u=-1, float v=-1)
{
  if (( u == -1 ) && ( v == -1 )) {
     u = _vertex->iniPos.x / 3.0;
     v = _vertex->iniPos.y / 3.0;
  }
  if (vertex1 == NULL) {
    vertex1 = _vertex;
    index1 = index;
    u1 = u; v1 = v;
  }
  else if (vertex2 == NULL) {
    vertex2 = _vertex;
    index2 = index;
    u2 = u; v2 = v;
  }
  else if (vertex3 == NULL) {
    vertex3 = _vertex;
    index3 = index;
    u3 = u; v3 = v;
  }
}

void BoneTriangle::rebuildNormal()
{
  Vect3D edge1 = vertex1->iniPos - vertex2->iniPos;
  Vect3D edge2 = vertex1->iniPos - vertex3->iniPos;
  iniNormal.crossProduct(edge1, edge2);
  iniNormal.normalize();
  curNormal = iniNormal;
}

void BoneTriangle::setColor(float _r=0.5, float _g=0.5, float _b=0.5, float _a=1)
{
  r = R = _r;
  g = G = _g;
  b = B = _b;
  a = A = _a;
}
