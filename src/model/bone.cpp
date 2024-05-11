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
  meshToMove = NULL;
  skeleton = NULL;
  link = NULL;
  links = 0;
  linkListCompiled = 0;
}

Bone::~Bone()
{
  //dax this line is correct, but temporally commented because
  //dax a segfault occurs later in the removeLink destructor
  //dax emptyLinkList();
}

//-----------------
// Accessing datas
void Bone::registerMesh(BoneMesh *_mesh)
{
  meshToMove = _mesh;
  trace(DBG_MAN, " Registered as movable mesh: [%s]", _mesh->getName());
}

void Bone::registerSkeleton(BoneVertex *_skel)
{
  skeleton = _skel;
  trace(DBG_MAN, " Registered as skeleton: [%s]", _skel->getName());
}

//--------------------------------------
// Operations on both mesh and skeleton
void Bone::scale(float sx, float sy, float sz)
{
  if (meshToMove == NULL) return;
  if ( skeleton  == NULL) return;

  meshToMove->scale(sx, sy, sz);
  skeleton->scale(sx, sy, sz);
  generateLinkList();
}

//------------------
// Links management
void Bone::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
}

void Bone::emptyLinkList()
{
  if (! linkListCompiled) compileLinkList();

  for (int i=0; i < links; i++) {
    delete link[i];
  }

  linkList.empty();
  compileLinkList();
}

float Bone::getLength(Vertex *vertex, BoneVertex *node)
{
  Vect3D nullvect(0, 0, 0);

  // First calculate the node's absolute position
  Vect3D nodePosition = node->iniMatrix * nullvect;

  // Then for each vertex, try to find the distance to the node
  Vect3D distance = nodePosition - vertex->iniPosition;
  return distance.length();
}

void getDistanceFromAndOnBone(Vertex *vertex, BoneVertex *a, BoneVertex *b, float *time, float *dist)
{
  Vect3D nullvect(0, 0, 0);
  Vect3D aPosition = a->iniMatrix * nullvect;
  Vect3D bPosition = b->iniMatrix * nullvect;
  Vect3D abVector = bPosition - aPosition;
  Vect3D paVector = aPosition - vertex->iniPosition;

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

  if (node->father != NULL) {
    getDistanceFromAndOnBone(vertex, node->father, node, &time, &dist);
    result += (time) / (dist * dist);
  }

  for (int i=0; i < node->children; i++) {
    getDistanceFromAndOnBone(vertex, node->child[i], node, &time, &dist);
    result += (time) / (dist * dist);
  }

  result /= node->children + (node->father == NULL ? 0:1);
  result *= node->influenceScaleFactor;
  return result;
}

void normalize(BoneLink **tempLink, int tempLinks)
{
  float totalWeight = 0;

  for (int i=0; i < tempLinks; i++) {
    if (tempLink[i] != NULL)
      totalWeight += tempLink[i]->weight;
  }
  for (int i=0; i < tempLinks; i++) {
    if (tempLink[i] != NULL)
      tempLink[i]->weight /= totalWeight;
  }
}

// -> Will call the method to generate links and then update the weight
// and optionally remove unsignificant links (that would speed down the cpu)
void Bone::generateLinkList()
{
  // We need both meshToMove and skeleton !
  if (meshToMove == NULL) return; // To avoid NULL pointer exception
  if ( skeleton  == NULL) return; // To avoid NULL pointer exception

  BoneVertex *tempnode;
  tempnode = skeleton->findBone("root");
  if (tempnode)
    tempnode->influenceScaleFactor = 10.;
  tempnode = skeleton->findBone("lipsRoot");
  if (tempnode)
    tempnode->influenceScaleFactor = 10.;
  tempnode = skeleton->findBone("frontRoot");
  if (tempnode)
    tempnode->influenceScaleFactor = 10.;

  // We start link generation with an empty link list of course
  emptyLinkList();

  // If the mesh has not compiled hos vertices list, we'll do it
  if (! meshToMove->vertexListCompiled)
    meshToMove->compileVertexList();

  // First, we need to generate the initial matrices for all the nodes of the skeleton
  glPushMatrix();
   glLoadIdentity();
   skeleton->generateIniMatrix();
  glPopMatrix();

  // We'll store the skeleton nodes into a list
  BoneList <BoneVertex> nodeList;
  BoneVertex **node;
  int nodes;

  addNodeAndChildren(skeleton, &nodeList);
  node = nodeList.getNiceTable(&nodes);

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Now we'll go throw each vertex of the mesh calculating
  // the weight of this node
  BoneList <BoneLink> tempLinkList;
  BoneLink **tempLink;
  BoneLink *temp;
  int  tempLinks;

  for (int i=0; i < meshToMove->vertices; i++) {
    tempLinkList.empty();
    for (int j=0; j < nodes; j++) {
      // And create a link between the vertex and the node with an influence
      // proportional to the inverse of the distance ( so far vertices
      // will be less influenced by the node than near vertices )
      temp = new BoneLink(meshToMove->vertex[i], node[j], getWeight(meshToMove->vertex[i], node[j]));
      // We now save this new link in our list
      tempLinkList.addElement(temp);
    }
    tempLink = tempLinkList.getNiceTable(&tempLinks);

    // Now sorting the links per weight
    for (int j=1; j < tempLinks; j++) {
      temp = tempLink[j];
      int k = j;
      while ((k > 0) && (tempLink[k-1]->weight < temp->weight)) {
	tempLink[k] = tempLink[k-1];
	k--;
      }
      tempLink[k] = temp;
    }
    // Now removing unsignificant links
    float seuil = 0.3 * tempLink[0]->weight;
    for (int j=0; j < tempLinks; j++) {
      if (tempLink[j]->weight < seuil) {
        delete tempLink[j];
        tempLink[j] = NULL;
      }
    }
    // Record the selected links in the list
    normalize(tempLink, tempLinks);
    for (int j=0; j < tempLinks; j++) {
      if (tempLink[j])
        linkList.addElement(tempLink[j]);
    }
  }

  // Now that we have all the links, we may compile the link list in here
  compileLinkList();

  for (int i=0; i < links; i++) {
    link[i]->notifyTarget();
  }
  for (int i=0; i < meshToMove->vertices; i++) {
    meshToMove->vertex[i]->compileLinkList();
  }
  //echo("selected links: [%2.2f%%]", (links*100.) / (meshToMove->vertices * nodes));
}

#if 0 //notused
// The render part of this file has been written very quickly, for tests only !
#define __AXIS_SIZE__ 0.5f

// Local coordinate system rendering
// Origin is white
// x axis is red
// y axis is green
// z axis is blue
void renderLocalCoordinate1() // This is for unselected node
{
  glColor3f(1, 1, 1);
  glBegin(GL_POINTS);
   glVertex3f(0,0,0);			// O
  glEnd();

  glBegin(GL_LINES);
   glColor3f(1, 0, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(__AXIS_SIZE__, 0, 0);	// X
   glColor3f(0, 1, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(0, __AXIS_SIZE__, 0);	// Y
   glColor3f(0, 0, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, __AXIS_SIZE__);	// Z
  glEnd();
}

void renderLocalCoordinate2() // This is for selected node
{
  glColor3f(1, 1, 1);
  glBegin(GL_POINTS);
   glVertex3f(0,0,0);			// O
  glEnd();

  glBegin(GL_LINES);
   glColor3f(1, 1, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(__AXIS_SIZE__, 0, 0);	// X
   glColor3f(0, 1, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, __AXIS_SIZE__, 0);	// Y
   glColor3f(1, 0, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, __AXIS_SIZE__);	// Z
  glEnd();
}

void renderOneBone(BoneVertex *node)
{
  if (node->father != NULL) {
    Vect3D nullvect(0, 0, 0);
    Vect3D fPos = node->father->curMatrix * nullvect;
    Vect3D tPos = node->curMatrix * nullvect;

    glVertex3f(fPos.x, fPos.y, fPos.z);
    glVertex3f(tPos.x, tPos.y, tPos.z);
  }
  for (int i=0; i < node->children; i++) {
    renderOneBone(node->child[i]);
  }
}
#endif //notused

// Main rendering method, will draw the skeleton and the mesh
void Bone::render()
{
#if 0 //notused
  // First we draw the skeleton with local coordinates
  if (axisRendering) {
    glPointSize(5.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    //renderSkeletonNode(skeleton);
    glBegin(GL_LINES);
     glColor3f(0.4, 0.4, 0.4);
     renderOneBone(skeleton);
    glEnd();
  }
#endif //notused

  // Now, we'll render the 3d mesh on the screen
  if (! meshToMove->triangleListCompiled) {
    meshToMove->compileTriangleList();
  }

  BoneTriangle *triangle;
  Vect3D *v1, *v2, *v3;
  Vect3D *n1, *n2, *n3;
  Vect3D *normal;

  glColor3f(0.7, 0.7, 0.8);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_DIFFUSE);
  //tex glEnable(GL_TEXTURE_2D);
  //tex glDisable(GL_COLOR_MATERIAL);
  glBegin(GL_TRIANGLES);

  for (int j=0; j < meshToMove->triangles; j++) {
    triangle = meshToMove->triangle[j];
    v1 = &triangle->vertex1->curPosition;
    v2 = &triangle->vertex2->curPosition;
    v3 = &triangle->vertex3->curPosition;
    glColor3f(triangle->R, triangle->G, triangle->B);
    glColor3f(triangle->r, triangle->g, triangle->b);
    normal = &triangle->iniNormal;

    n1 = &triangle->vertex1->curNormal;
    n2 = &triangle->vertex2->curNormal;
    n3 = &triangle->vertex3->curNormal;

    glNormal3f(normal->x, normal->y, normal->z);
    //tex glTexCoord2f(triangle->u1, triangle->v1);
    glNormal3f(n1->x, n1->y, n1->z);
    glVertex3f(v1->x, v1->y, v1->z);
    //tex glTexCoord2f(triangle->u2, triangle->v2);
    glNormal3f(n2->x, n2->y, n2->z);
    glVertex3f(v2->x, v2->y, v2->z);
    //tex glTexCoord2f(triangle->u3, triangle->v3);
    glNormal3f(n3->x, n3->y, n3->z);
    glVertex3f(v3->x, v3->y, v3->z);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

// Recursive part of rendering the skeleton
void Bone::renderSkeletonNode(BoneVertex *node)
{
#if 0 //dax
  if (selectedNodeName == NULL) return;

  glPushMatrix();
  glTranslatef(node->curPosition.x, node->curPosition.y, node->curPosition.z);
  glRotatef(node->curAngle, node->curAxis.x, node->curAxis.y, node->curAxis.z);

  if (! strcmp(selectedNodeName, node->name))
    renderLocalCoordinate2();
  else
    renderLocalCoordinate1();

  // Look at the children of this node
  for (int i=0; i < node->children; i++) {
    renderSkeletonNode(node->child[i]);
  }
  glPopMatrix();
#endif
}

// Here comes the morphing part of this code
// This will animate the object
void Bone::animate()
{
#if 0 //dax
  if (! boneEditing) {
    // First we reset to 0,0,0 all the vertices of the mesh
    for (int i=0; i < meshToMove->vertices; i++) {
      Vertex *curVertex = meshToMove->vertex[i];
      if (curVertex->links != 0) {
        curVertex->curPosition.reset();
        curVertex->curNormal.reset();
      }
      else {
        curVertex->curPosition = curVertex->iniPosition;
        curVertex->curNormal   = curVertex->iniNormal;
      }
    }

    // Then we generate the current matrix of each part of the skeleton
    glPushMatrix();
     glLoadIdentity();
     skeleton->generateCurrentMatrix();
    glPopMatrix();

    // And now we can perform the recursive calls to create our new 3d mesh
    animateSkeletonNode(skeleton);
  }
  else
#endif
  for (int i=0; i < meshToMove->vertices; i++) {
    meshToMove->vertex[i]->curPosition = meshToMove->vertex[i]->iniPosition;
    meshToMove->vertex[i]->curNormal   = meshToMove->vertex[i]->iniNormal;
  }
}

// Recursive part of the animating
void Bone::animateSkeletonNode(BoneVertex *node)
{
  // The first time, the link list won't be compiled for this node so we'll dot it
  if (! node->linkListCompiled) node->compileLinkList();

  BoneLink *zeLink;
  Vertex *zeVertex;
  Vect3D tempPos, normal;

  // We now look at each link in this node and find the related vertex
  for (int i=0; i < node->links; i++) {
    zeLink = node->link[i];
    zeVertex = zeLink->vertex;

    // Now that we have M1 ( Initial matrix for this node )
    //                  M2 ( Current matrix for this node )
    //              and  w ( Weight of this link for the vertex )
    // We increase newPosition as follozs :
    //    newPosition += w . M2 . M1-1 . iniPosition
    // Since the weight are normalized to 100% [0.0f .. 1.0f], we
    // should have a normalized result (means no scaling here) for all
    // the vertices

    tempPos  = zeVertex->iniPosition;
    tempPos *= node->iniMatrixInverted;
    tempPos *= node->curMatrix;
    tempPos *= zeLink->weight;
    zeVertex->curPosition += tempPos;
  }

  // And now, we'll add the other links actions
  for (int i=0; i < node->children; i++) {
    animateSkeletonNode(node->child[i]);
  }
}

//-----------------
// Private methods
void Bone::addNodeAndChildren(BoneVertex *boneVertex, BoneList <BoneVertex> *list)
{
  if (! boneVertex->childListCompiled) boneVertex->compileChildList();

  list->addElement(boneVertex);

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
  if (vertex) vertex->removeLink(this);
  if (boneVertex) boneVertex->removeLink(this);
}

void BoneLink::setVertex(Vertex *_vertex)
{
  if (vertex) vertex->removeLink(this);
  vertex = _vertex;
}

void BoneLink::setBoneVertex(BoneVertex *_boneVertex)
{
  if (boneVertex) boneVertex->removeLink(this);
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
  vertexListCompiled = 0;
  triangle = NULL;
  triangles = 0;
  triangleListCompiled = 0;

  setName(const_cast<char*>(" Mesh NoName "));
}

BoneMesh::~BoneMesh()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

  for (int i=0; i < vertices; i++) delete vertex[i];
  for (int j=0; j < triangles; j++) delete triangle[j];
}

void BoneMesh::addVertex(Vect3D &position)
{
  vertexList.addElement(new Vertex(position));
  vertexListCompiled = 0;
}

void BoneMesh::addVertex(Vect3D *position)
{
  vertexList.addElement(new Vertex(position));
  vertexListCompiled = 0;
}

void BoneMesh::addVertex(float ox, float oy, float oz)
{
  vertexList.addElement(new Vertex(ox, oy, oz));
  vertexListCompiled = 0;
}

void BoneMesh::addTriangle(int index1, int index2, int index3)
{
  if (! vertexListCompiled) compileVertexList();

  BoneTriangle * tri = new BoneTriangle();
  tri->addVertex(vertex[index1], index1, -1, -1);
  tri->addVertex(vertex[index2], index2, -1, -1);
  tri->addVertex(vertex[index3], index3, -1, -1);

  triangleList.addElement(tri);
  triangleListCompiled = 0;
}

void BoneMesh::scale(float sx, float sy, float sz)
{
  if (! vertexListCompiled) compileVertexList();

  for (int i=0; i < vertices; i++) {
    vertex[i]->iniPosition.x *= sx;
    vertex[i]->iniPosition.y *= sy;
    vertex[i]->iniPosition.z *= sz;
    vertex[i]->curPosition.x *= sx;
    vertex[i]->curPosition.y *= sy;
    vertex[i]->curPosition.z *= sz;
  }
}

void BoneMesh::rebuildNormals()
{
  if (! vertexListCompiled) compileVertexList();
  if (! triangleListCompiled) compileTriangleList();

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
  if (! triangleListCompiled) compileTriangleList();

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
  iniPosition.set(0,0,0);
  iniAngle    = 0;
  iniAxis.    set(0,1,0);
  curPosition.set(0,0,0);
  curAngle    = 0;
  curAxis.    set(0,1,0);

  child    = NULL;
  father   = NULL;
  children = 0;
  childListCompiled = 0;
  link     = NULL;
  links    = 0;
  linkListCompiled = 0;

  influenceScaleFactor = 1;
}

BoneVertex::BoneVertex(Vect3D &position, float angle, Vect3D &axis)
{
  iniPosition = position;
  iniAngle    = angle;
  iniAxis     = axis;
  curPosition = position;
  curAngle    = angle;
  curAxis     = axis;

  child    = NULL;
  father   = NULL;
  children = 0;
  childListCompiled = 0;
  link     = NULL;
  links    = 0;
  linkListCompiled = 0;
}

BoneVertex::BoneVertex(Vect3D *position, float angle, Vect3D *axis)
{
  iniPosition = *position;
  iniAngle    =  angle;
  iniAxis     = *axis;
  curPosition = *position;
  curAngle    =  angle;
  curAxis     = *axis;

  child    = NULL;
  father   = NULL;
  children = 0;
  childListCompiled = 0;
  link     = NULL;
  links    = 0;
  linkListCompiled = 0;
}

BoneVertex::~BoneVertex()
{
  // first delete all the children
  if (! childListCompiled) compileChildList();
  for (int i=0; i < children; i++) {
    delete child[i];
  }
  childList.empty();

  // Now, delete the selected links for this node
  if (! linkListCompiled) compileLinkList();
  for (int i=0; i < links; i++) {
    //dax-segfault delete link[i];
  }
  linkList.empty();
}

// Accessing initial position datas
void BoneVertex::setIniPos(Vect3D &position)
{
  iniPosition =  position;
  curPosition =  position;
}

void BoneVertex::setIniPos(Vect3D *position)
{
  iniPosition = *position;
  curPosition = *position;
}

void BoneVertex::setIniPos(float ox, float oy, float oz)
{
  iniPosition = Vect3D(ox, oy, oz);
  curPosition = Vect3D(ox, oy, oz);
}

void BoneVertex::setIniRot(float angle, Vect3D &axis)
{
  iniAngle    =  angle;
  iniAxis     =  axis;
  curAngle    =  angle;
  curAxis     =  axis;
}

void BoneVertex::setIniRot(float angle, Vect3D *axis)
{
  iniAngle    =  angle;
  iniAxis     = *axis;
  curAngle    =  angle;
  curAxis     = *axis;
}

void BoneVertex::setIniRot(float angle, float axisx, float axisy, float axisz)
{
  iniAngle    =  angle;
  iniAxis     =  Vect3D(axisx, axisy, axisz);
  curAngle    =  angle;
  curAxis     =  Vect3D(axisx, axisy, axisz);
}

// And... Accessing animation position datas
void BoneVertex::setPos(Vect3D &position)
{
  curPosition =  position;
}

void BoneVertex::setPos(Vect3D *position)
{
  curPosition = *position;
}

void BoneVertex::setPos(float ox, float oy, float oz)
{
  curPosition =  Vect3D(ox, oy, oz);
}

void BoneVertex::setRot(float angle, Vect3D &axis)
{
  curAngle    =  angle;
  curAxis     =  axis;
}

void BoneVertex::setRot(float angle, Vect3D *axis)
{
  curAngle    =  angle;
  curAxis     = *axis;
}

void BoneVertex::setRot(float angle, float axisx, float axisy, float axisz)
{
  curAngle    =  angle;
  curAxis     =  Vect3D(axisx, axisy, axisz);
}

// Accessing current position datas (during animation)
// with relative values (realtive to initial position)
void BoneVertex::resetPos()
{
  curPosition = iniPosition;
}

void BoneVertex::resetRot()
{
  curAngle = iniAngle;
  curAxis  = iniAxis;
}

void BoneVertex::setTrans(Vect3D &delta)
{
  curPosition = curPosition + delta;
}

void BoneVertex::setTrans(Vect3D *delta)
{
  curPosition = curPosition + *delta;
}

void BoneVertex::setTrans(float dx, float dy, float dz)
{
  curPosition.x += dx;
  curPosition.y += dy;
  curPosition.z += dz;
}

void BoneVertex::setScale(float scalex, float scaley, float scalez)
{
  curPosition.x *= scalex;
  curPosition.y *= scaley;
  curPosition.z *= scalez;
}

void BoneVertex::setScale(float scale)
{
  curPosition = curPosition * scale;
}

// Modifying the node and its children (definitive)
void BoneVertex::scale(float sx, float sy, float sz)
{
  if (! childListCompiled) compileChildList();

  iniPosition.x *= sx;
  iniPosition.y *= sy;
  iniPosition.z *= sz;
  curPosition.x *= sx;
  curPosition.y *= sy;
  curPosition.z *= sz;

  for (int i=0; i < children; i++) {
    child[i]->scale(sx, sy, sz);
  }
}

// Updating the father of this boneVertex
void BoneVertex::setBone(BoneVertex *_father)
{
  father = _father;
}

// Adding children
void BoneVertex::addBone(BoneVertex *newChild)
{
  childList.addElement(newChild);
  newChild->setBone(this);
  childListCompiled = 0;
}

// Removing a child and its children
void BoneVertex::removeBone(const char *name)
{
  BoneVertex *tmp = findBone(name);
  if (tmp == NULL) return;
  if (tmp == this) return;

  childList.removeElement(tmp);
  childListCompiled = 0;
  delete tmp;
}

// Finding a boneVertex in the tree using its name
BoneVertex *BoneVertex::findBone(const char *name)
{
  BoneVertex *result = NULL;

  if (! childListCompiled) compileChildList();

  if (strcmp(name, getName()) == 0) {
    result = this;
  }
  else {
    int i=0;
    while ((result == NULL) && (i < children)) {
      result = child[i++]->findBone(name);
    }
  }
  return result;
}

// Adding a link
void BoneVertex::addLink(BoneLink *link)
{
  linkList.addElement(link);
  linkListCompiled = 0;
}

// Removing a link
void BoneVertex::removeLink(BoneLink *link)
{
  linkList.removeElement(link);
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
void BoneVertex::generateIniMatrix()
{
  // First, we'll need to know all the childs
  if (! childListCompiled) compileChildList();

  glPushMatrix();

  // Now I let OpenGL calculate the matrix
  glTranslatef(iniPosition.x, iniPosition.y, iniPosition.z);
  glRotatef(iniAngle, iniAxis.x, iniAxis.y, iniAxis.z);
  // I save it
  glGetFloatv(GL_MODELVIEW_MATRIX, iniMatrix);

  // Here is a nice matrix inversion
  // 1. tmp gets the transposed rotation part of the matrix
  float tmp[16];
  tmp[0] = iniMatrix[ 0]; tmp[4] = iniMatrix[ 1]; tmp[ 8] = iniMatrix[ 2]; tmp[12] = 0;
  tmp[1] = iniMatrix[ 4]; tmp[5] = iniMatrix[ 5]; tmp[ 9] = iniMatrix[ 6]; tmp[13] = 0;
  tmp[2] = iniMatrix[ 8]; tmp[6] = iniMatrix[ 9]; tmp[10] = iniMatrix[10]; tmp[14] = 0;
  tmp[3] = 0;                 tmp[7] = 0;                 tmp[11] = 0;                 tmp[15] = 1;
  // 2. tmpVect gets an inverted translation from the matrix
  Vect3D tmpVect(-iniMatrix[12], -iniMatrix[13], -iniMatrix[14]);
  // 3. tmpVect is to be done in the inverted coordinate system so multiply
  tmpVect = tmp * tmpVect;
  // 4. now we have the nice inverted matrix :)
  tmp[12] = tmpVect.x;
  tmp[13] = tmpVect.y;
  tmp[14] = tmpVect.z;
  // 5. Stores it !
  for (int j=0; j<16 ; j++) {
    iniMatrixInverted[j] = tmp[j];
  }

  // Now, store the rotation matrices (for normals computation)
  int off=0;
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

  // Now, let's do the same for children :)
  for (int i=0; i < children; i++) {
    child[i]->generateIniMatrix();
  }

  glPopMatrix();
}

// Now let's have a look to the current matrix, same code as above
void BoneVertex::generateCurrentMatrix()
{
  if (! childListCompiled) compileChildList();

  glPushMatrix();

  glTranslatef(curPosition.x, curPosition.y, curPosition.z);
  glRotatef(curAngle, curAxis.x , curAxis.y, curAxis.z);
  glGetFloatv(GL_MODELVIEW_MATRIX, curMatrix);

  // Now we have the current transformation matrix,
  // we'll extract the rotation part so we can
  // move the normals of the mesh correctly to
  // update the lighting !
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

  // And now, just generate the childrem matrices
  for (int i=0; i < children; i++) {
    child[i]->generateCurrentMatrix();
  }

  glPopMatrix();
}

// I/O functions
void BoneVertex::read(char *filename, float scale)
{
  File *file = new File();
  FILE *fp = file->open(filename, "rb");
  if (fp == NULL) {
    error("BoneVertex::read unable to open: [%s]", filename);
    return;
  }

  readFromFile(fp, scale);
  file->close();
  delete file;
}

void BoneVertex::readFromFile(FILE *fp, float scale)
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
    BoneVertex *tmp = new BoneVertex();
    addBone(tmp);
    tmp->readFromFile(fp, scale);
  }
  compileChildList();
}

Vertex::Vertex()
{
  defaults();
}

Vertex::Vertex(Vect3D &position)
{
  iniPosition = position;
  curPosition = position;
  defaults();
}

Vertex::Vertex(Vect3D *position)
{
  iniPosition = *position;
  curPosition = *position;
  defaults();
}

Vertex::Vertex(float ox, float oy, float oz)
{
  iniPosition.set(ox, oy, oz);
  curPosition.set(ox, oy, oz);
  defaults();
}

void Vertex::defaults()
{
  link  = NULL;
  links = 0;
  linkListCompiled = 0;
  iniNormal.reset();
  curNormal.reset();
  u = -1.;
  v = -1.;
}

void Vertex::setPosition(Vect3D &position)
{
  iniPosition =  position;
  curPosition =  position;
}

void Vertex::setPosition(Vect3D *position)
{
  iniPosition = *position;
  curPosition = *position;
}

void Vertex::addLink(BoneLink *link)
{
  linkList.addElement(link);
  linkListCompiled = 0;
}

void Vertex::removeLink(BoneLink *link)
{
  //dax-segfault linkList.removeElement(link);
  linkListCompiled = 0;
}

void Vertex::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
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
  setColor(0.5, 0.5, 0.5, 1);
}

BoneTriangle::~BoneTriangle() {}

/** Accessing datas */
void BoneTriangle::addVertex(Vertex *_vertex, int index, float u=-1, float v=-1)
{
  if (( u == -1 ) && ( v == -1 )) {
     u = _vertex->iniPosition.x / 3.0;
     v = _vertex->iniPosition.y / 3.0;
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
  Vect3D edge1 = vertex1->iniPosition - vertex2->iniPosition;
  Vect3D edge2 = vertex1->iniPosition - vertex3->iniPosition;
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

