//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "bone.hpp"
#include "file.hpp"	// openFile


// TODO : remove next lines !
int plainRendering = 1;
int boneEditing = 0;
int axisRendering = 0;
char *selectedNodeName = NULL;


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
#if 0 //crash
  //PD this line is correct, but temporally commented because
  //PD a segfault occurs later in the removeLink destructor
  emptyLinkList();
#endif
}

//-----------------
// Accessing datas
void Bone::registerMesh(BoneMesh *zeMesh)
{
  meshToMove = zeMesh;
  trace(DBG_MAN, " Registered as movable mesh: [%s]", zeMesh->getName());
}

void Bone::registerSkeleton(BoneVertex *zeRoot)
{
  skeleton = zeRoot;
  trace(DBG_MAN, " Registered as skeleton: [%s]", zeRoot->getName());
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

  for (int i=0; i<links; i++) delete link[i];

  linkList.empty();
  compileLinkList();
}

inline float Bone::getLength(Vertex *vertex, BoneVertex *node)
{
  Vect3D nullVector(0., 0., 0.);
  Vect3D nodePosition;
  Vect3D distance;

  // First calculate the node's absolute position
  nodePosition = node->initialMatrix * nullVector;

  // Then for eache vertex, try to find the distance to the node
  distance = nodePosition - vertex->initialPosition;
  return distance.length();
}

inline void getDistanceFromAndOnBone(Vertex *vertex, BoneVertex *a, BoneVertex *b, float *time, float *dist)
{
  Vect3D nullVector(0., 0., 0.);
  Vect3D aPosition; aPosition = a->initialMatrix * nullVector;
  Vect3D bPosition; bPosition = b->initialMatrix * nullVector;
  Vect3D abVector; abVector = bPosition - aPosition;
  Vect3D paVector; paVector = aPosition - vertex->initialPosition;

  *time = -Vect3D::dotProduct(paVector, abVector) / Vect3D::dotProduct(abVector, abVector);
  *time = CROP(0.0001, *time, 0.9999);

  Vect3D phVector;
  phVector = abVector;
  phVector = phVector * *time;
  phVector = phVector + paVector;
  *dist = Vect3D::dotProduct(phVector, phVector);
  *dist = (*dist > 0.00000001 ? *dist : 0.00000001);
}

inline float Bone::getWeight(Vertex *vertex, BoneVertex *node)
{
  float result = 0;
  float dist, time;

  if (node->father != NULL) {
    getDistanceFromAndOnBone(vertex, node->father, node, &time, &dist);
    result += (time) / (dist * dist);
  }

  for (int i=0; i<node->children; i++) {
    getDistanceFromAndOnBone(vertex, node->child[i], node, &time, &dist);
    result += (time) / (dist * dist);
  }

  result /= node->children + (node->father == NULL ? 0:1);
  result *= node->influenceScaleFactor;
  return result;
}

void normalize(BoneLink **temporaryLink, int temporaryLinks)
{
  float totalWeight = 0;

  for (int i=0; i<temporaryLinks; i++)
    if (temporaryLink[i] != NULL) totalWeight += temporaryLink[i]->weight;
  for (int i=0; i<temporaryLinks; i++)
    if (temporaryLink[i] != NULL) temporaryLink[i]->weight /= totalWeight;
}

// -> Will call the method to generate links and then update the weight
// and optionally remove unsignificant links (that would speed down the cpu)
void Bone::generateLinkList()
{
  // We need both meshToMove and skeleton !
  if (meshToMove == NULL) return; // To avoid NULL pointer exception
  if ( skeleton  == NULL) return; // To avoid NULL pointer exception

  BoneVertex *tempNode;
  tempNode = skeleton->findChild("root");
  if (tempNode != NULL)
    tempNode->influenceScaleFactor = 10.;
  tempNode = skeleton->findChild("lipsRoot");
  if (tempNode != NULL)
    tempNode->influenceScaleFactor = 10.;
  tempNode = skeleton->findChild("frontRoot");
  if (tempNode != NULL)
    tempNode->influenceScaleFactor = 10.;

  // We start link generation with an empty link list of course
  emptyLinkList();

  // If the mesh has not compiled hos vertices list, we'll do it
  if (! meshToMove->vertexListCompiled) meshToMove->compileVertexList();

  // First, we need to generate the initial matrices for
  // all the nodes of the skeleton
  glPushMatrix();
   glLoadIdentity();
   skeleton->generateInitialMatrix();
  glPopMatrix();

  //--- We'll store the skeleton nodes into a list
  BoneList < BoneVertex > nodeList;
  BoneVertex **node;
  int nodes;

  addNodeAndChildren(skeleton, &nodeList);
  node = nodeList.getNiceTable(&nodes);

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  // Now we'll go throw each vertex of the mesh calculating
  // the weight of this node
  BoneList < BoneLink > temporaryLinkList;
  BoneLink **temporaryLink;
  BoneLink *temp;
  int  temporaryLinks;
#ifdef DEBUG_FACE
  int progress = -1;
  int progressold = -1;
#endif

  for (int i=0; i<meshToMove->vertices; i++) {
#ifdef DEBUG_FACE
    // Prints info on progress...
    progressold = progress;
    progress = (i * 10) / (meshToMove->vertices - 1);
    if (progress != progressold) {
      fprintf(stderr, "%i%%... ", progress * 10);
      fflush(stderr);
    }
#endif

    temporaryLinkList.empty();
    for (int j=0; j<nodes; j++) {
      // And create a link between the vertex and the node with an influence
      // proportional to the inverse of the distance ( so far vertices
      // will be less influenced by the node than near vertices )
      temp = new BoneLink(meshToMove->vertex[i], node[j], getWeight(meshToMove->vertex[i], node[j]));
      // We now save this new link in our list
      temporaryLinkList.addElement(temp);
    }
    temporaryLink = temporaryLinkList.getNiceTable(&temporaryLinks);

    // Now sorting the links per weight
    for (int j=1; j<temporaryLinks; j++) {
      temp = temporaryLink[j];
      int k = j;
      while ((k > 0) && (temporaryLink[k-1]->weight < temp->weight)) {
	temporaryLink[k] = temporaryLink[k-1];
	k--;
      }
      temporaryLink[k] = temp;
    }

    // Now removing unsignificant links
    float seuil = 0.3 * temporaryLink[0]->weight;
    for (int j=0; j<temporaryLinks; j++) {
      if (temporaryLink[j]->weight < seuil)
        { delete temporaryLink[j]; temporaryLink[j] = NULL; }
    }

    // Record the selected links in the list
    normalize (temporaryLink, temporaryLinks);
    for (int j=0; j<temporaryLinks; j++)
      if (temporaryLink[j] != NULL) linkList.addElement(temporaryLink[j]);
  }

  // Now that we have all the links, we may compile the link list in here
  compileLinkList();

  for (int i=0; i<links; i++) link[i]->notifyTarget();
  for (int i=0; i<meshToMove->vertices; i++) meshToMove->vertex[i]->compileLinkList();

#ifdef DEBUG_FACE
  fprintf(stderr, "\n");
  fflush(stderr);
#endif
  trace(DBG_MAN, "selected links: [%2.2f%%]", (links * 100.) / (meshToMove->vertices * nodes));
}

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
   glVertex3f(0,0,0);
  glEnd();

  glBegin(GL_LINES);
   glColor3f(1, 0, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(__AXIS_SIZE__, 0, 0);
   glColor3f(0, 1, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(0, __AXIS_SIZE__, 0);
   glColor3f(0, 0, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, __AXIS_SIZE__);
  glEnd();
}

void renderLocalCoordinate2() // This is for selected node
{
  glColor3f(1, 1, 1);
  glBegin(GL_POINTS);
   glVertex3f(0,0,0);
  glEnd();

  glBegin(GL_LINES);
   glColor3f(1, 1, 0);
   glVertex3f(0, 0, 0);
   glVertex3f(__AXIS_SIZE__, 0, 0);
   glColor3f(0, 1, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, __AXIS_SIZE__, 0);
   glColor3f(1, 0, 1);
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, __AXIS_SIZE__);
  glEnd();
}

void renderOneBone(BoneVertex *node)
{
  if (node->father != NULL) {
    Vect3D nullVector(0,0,0);
    Vect3D fPos; fPos = node->father->currentMatrix * nullVector;
    Vect3D tPos; tPos = node->        currentMatrix * nullVector;

    glVertex3f(fPos.x, fPos.y, fPos.z);
    glVertex3f(tPos.x, tPos.y, tPos.z);
  }
  for (int i=0; i < node->children; i++)
    renderOneBone(node->child[i]);
}

// Main rendering method, will draw the skeleton and the mesh
void Bone::render()
{
  // First we draw the skeleton with local coordinates
  if (axisRendering) {
    glPointSize(5.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    renderSkeletonNode(skeleton);
    glBegin(GL_LINES);
     glColor3f(0.4, 0.4, 0.4);
     renderOneBone(skeleton);
    glEnd();
  }

  // Now, we'll render the 3d mesh on the screen
  if (! meshToMove->triangleListCompiled) meshToMove->compileTriangleList();

  Triangle *triangle;
  Vect3D *v1, *v2, *v3;
  Vect3D *n1, *n2, *n3;
  Vect3D *normal;

  glColor3f(0.7, 0.7, 0.8);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  if (plainRendering) {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
#ifdef __GL_TEXTURE__
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
#endif
    glBegin(GL_TRIANGLES);
  }
  else {
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
  }

  for (int j=0; j < meshToMove->triangles; j++) {
    triangle = meshToMove->triangle[j];
    v1 = &triangle->vertex1->currentPosition;
    v2 = &triangle->vertex2->currentPosition;
    v3 = &triangle->vertex3->currentPosition;
#ifndef __GL_TEXTURE__
    if (plainRendering)
      glColor3f(triangle->finalRed, triangle->finalGreen, triangle->finalBlue);
    else if (! boneEditing)
      glColor3f(triangle->colorRed, triangle->colorGreen, triangle->colorBlue);
    else
      glColor3f(0.2,0.2,0.6);
#endif
    normal = &triangle->initialNormal;

    n1 = &triangle->vertex1->currentNormal;
    n2 = &triangle->vertex2->currentNormal;
    n3 = &triangle->vertex3->currentNormal;

    if (! plainRendering)
      glBegin(GL_LINE_LOOP);
    glNormal3f(normal->x, normal->y, normal->z);
#ifdef __GL_TEXTURE__
    glTexCoord2f(triangle->u1, triangle->v1);
#endif
    glNormal3f(n1->x, n1->y, n1->z);
    glVertex3f(v1->x, v1->y, v1->z);
#ifdef __GL_TEXTURE__
    glTexCoord2f(triangle->u2, triangle->v2);
#endif
    glNormal3f(n2->x, n2->y, n2->z);
    glVertex3f(v2->x, v2->y, v2->z);
#ifdef __GL_TEXTURE__
    glTexCoord2f(triangle->u3, triangle->v3);
#endif
    glNormal3f(n3->x, n3->y, n3->z);
    glVertex3f(v3->x, v3->y, v3->z);
    if (! plainRendering)
      glEnd();
  }
  if (plainRendering)
    glEnd();
  glDisable(GL_TEXTURE_2D);
}

// Recursive part of rendering the skeleton
void Bone::renderSkeletonNode(BoneVertex *node)
{
  if (selectedNodeName == NULL) return;

  glPushMatrix();
  glTranslatef(node->currentPosition.x, node->currentPosition.y, node->currentPosition.z);
  glRotatef(node->currentAngle, node->currentAxis.x, node->currentAxis.y, node->currentAxis.z);

  if (! strcmp(selectedNodeName, node->name)) renderLocalCoordinate2();
  else                                        renderLocalCoordinate1();

  // Look at the children of this node
  for (int i=0; i < node->children; i++)
    renderSkeletonNode(node->child[i]);
  glPopMatrix();
}

// Here comes the morphing part of this code
// This will animate the object
void Bone::animate()
{
  if (! boneEditing) {
    // First we reset to 0,0,0 all the vertices of the mesh
    for (int i=0; i < meshToMove->vertices; i++) {
      Vertex *currentVertex = meshToMove->vertex[i];
      if (currentVertex->links != 0) {
        currentVertex->currentPosition.reset();
        currentVertex->currentNormal.reset();
      }
      else {
        currentVertex->currentPosition = currentVertex->initialPosition;
        currentVertex->currentNormal   = currentVertex->initialNormal;
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
    for (int i=0; i < meshToMove->vertices; i++) {
      meshToMove->vertex[i]->currentPosition = meshToMove->vertex[i]->initialPosition;
      meshToMove->vertex[i]->currentNormal   = meshToMove->vertex[i]->initialNormal;
    }
}

// Recursive part of the animating
inline void Bone::animateSkeletonNode(BoneVertex *node)
{
  // The first time, the link list won't be compiled for this node
  // so we'll dot it
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
    //    newPosition += w . M2 . M1-1 . initialPosition
    // Since the weight are normalized to 100% [0.0f .. 1.0f], we
    // should have a normalized result (means no scaling here) for all
    // the vertices

    tempPos  = zeVertex->initialPosition;
    tempPos *= node->initialMatrixInverted;
    tempPos *= node->currentMatrix;
    tempPos *= zeLink-> weight;
    zeVertex->currentPosition += tempPos;
  }

  // And now, we'll add the other links actions
  for (int i=0; i < node->children; i++)
    animateSkeletonNode(node->child[i]);
}

//-----------------
// Private methods
void Bone::addNodeAndChildren(BoneVertex *boneVertex, BoneList < BoneVertex > *list)
{
  if (! boneVertex->childListCompiled) boneVertex->compileChildList();

  list->addElement(boneVertex);

  for (int i=0; i < boneVertex->children; i++)
    addNodeAndChildren(boneVertex->child[i], list);
}

//---------------------------------------------------------------------------

BoneLink::BoneLink(Vertex *zeVertex, BoneVertex *zeBoneVertex = NULL, float zeWeight = 0.)
{
  vertex        = NULL;
  boneVertex    = NULL;
  setVertex(zeVertex);
  setBoneVertex(zeBoneVertex);
  setWeight(zeWeight);
}

BoneLink::~BoneLink()
{
  if (vertex) vertex->removeLink(this);
  if (boneVertex) boneVertex->removeLink(this);
}

void BoneLink::setVertex(Vertex *zeVertex)
{
  if (vertex) vertex->removeLink(this);
  vertex = zeVertex;
}

void BoneLink::setBoneVertex(BoneVertex *zeBoneVertex)
{
  if (boneVertex) boneVertex->removeLink(this);
  boneVertex = zeBoneVertex;
}

void BoneLink::notifyTarget()
{
  if (vertex) vertex->addLink(this);
  if (boneVertex) boneVertex->addLink(this);
}

void BoneLink::setWeight(float zeWeight)
{
  weight = zeWeight;
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
  vertexList.addElement(new Vertex(zePosition));
  vertexListCompiled = 0;
}

void BoneMesh::addVertex(Vect3D * zePosition)
{
  vertexList.addElement(new Vertex(zePosition));
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

  Triangle * tri = new Triangle();
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

//---------------------------------------------------------------------------

Bonename::Bonename()
{
  name = NULL;
  setName((char *)"NoName");
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

void Bonename::setName(char *zeName)
{
  if (name) delete[] name;
  name = new char[strlen(zeName) + 1];
  strcpy(name, zeName);
}

char * Bonename::getName()
{
  return name;
}

//---------------------------------------------------------------------------

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

// And... Accessing animation position datas
void BoneVertex::setCurrentPosition(Vect3D & zePosition)
{
  currentPosition =  zePosition;
}

void BoneVertex::setCurrentPosition(Vect3D *zePosition)
{
  currentPosition = *zePosition;
}

void BoneVertex::setCurrentPosition(float ox, float oy, float oz)
{
  currentPosition =  Vect3D(ox, oy, oz);
}

void BoneVertex::setCurrentRotation(float zeAngle, Vect3D &zeAxis)
{
  currentAngle    =  zeAngle;
  currentAxis     =  zeAxis;
}

void BoneVertex::setCurrentRotation(float zeAngle, Vect3D *zeAxis)
{
  currentAngle    =  zeAngle;
  currentAxis     = *zeAxis;
}

void BoneVertex::setCurrentRotation(float zeAngle, float axisx, float axisy, float axisz)
{
  currentAngle    =  zeAngle;
  currentAxis     =  Vect3D(axisx, axisy, axisz);
}

// Accessing current position datas (during animation)
// with relative values (realtive to initial position)
void BoneVertex::resetCurrentPosition()
{
  currentPosition = initialPosition;
}

void BoneVertex::resetCurrentRotation()
{
  currentAngle = initialAngle;
  currentAxis  = initialAxis;
}

void BoneVertex::translateCurrentPosition(Vect3D &delta)
{
  currentPosition = currentPosition + delta;
}

void BoneVertex::translateCurrentPosition(Vect3D *delta)
{
  currentPosition = currentPosition + *delta;
}

void BoneVertex::translateCurrentPosition(float dx, float dy, float dz)
{
  currentPosition.x += dx;
  currentPosition.y += dy;
  currentPosition.z += dz;
}

void BoneVertex::scaleCurrentPosition(float scalex, float scaley, float scalez)
{
  currentPosition.x *= scalex;
  currentPosition.y *= scaley;
  currentPosition.z *= scalez;
}

void BoneVertex::scaleCurrentPosition(float scale)
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
  childList.addElement(newChild);
  newChild->setFather(this);
  childListCompiled = 0;
}

// Removing a child and its children
void BoneVertex::removeChild(const char *zeName)
{
  BoneVertex *tmp = findChild(zeName);
  if (tmp == NULL) return;
  if (tmp == this) return;

  childList.removeElement(tmp);
  childListCompiled = 0;
  delete tmp;
}

// Finding a boneVertex in the tree using its name
BoneVertex *BoneVertex::findChild(const char *zeName)
{
  BoneVertex *result = NULL;

  if (! childListCompiled) compileChildList();

  if (strcmp(zeName, getName()) == 0)
    result = this;
  else {
    int i=0;
    while ((result == NULL) && (i < children))
      result = child[i++]->findChild(zeName);
  }
  return result;
}

// Adding a link
void BoneVertex::addLink(BoneLink *zeLink)
{
  linkList.addElement(zeLink);
  linkListCompiled = 0;
}

// Removing a link
void BoneVertex::removeLink(BoneLink *zeLink)
{
  linkList.removeElement(zeLink);
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
  // I save it
  glGetFloatv (GL_MODELVIEW_MATRIX, initialMatrix);

  // Here is a nice matrix inversion
  // 1. tmp gets the transposed rotation part of the matrix
  float tmp[16];
  tmp[0] = initialMatrix[ 0]; tmp[4] = initialMatrix[ 1]; tmp[ 8] = initialMatrix[ 2]; tmp[12] = 0;
  tmp[1] = initialMatrix[ 4]; tmp[5] = initialMatrix[ 5]; tmp[ 9] = initialMatrix[ 6]; tmp[13] = 0;
  tmp[2] = initialMatrix[ 8]; tmp[6] = initialMatrix[ 9]; tmp[10] = initialMatrix[10]; tmp[14] = 0;
  tmp[3] = 0;                 tmp[7] = 0;                 tmp[11] = 0;                 tmp[15] = 1;
  // 2. tmpVect gets an inverted translation from the matrix
  Vect3D tmpVect(-initialMatrix[12], -initialMatrix[13], -initialMatrix[14]);
  // 3. tmpVect is to be done in the inverted coordinate system so multiply
  tmpVect = tmp * tmpVect;
  // 4. now we have the nice inverted matrix :)
  tmp[12] = tmpVect.x;
  tmp[13] = tmpVect.y;
  tmp[14] = tmpVect.z;
  // 5. Stores it !
  for (int j=0; j<16 ; j++) initialMatrixInverted[j] = tmp[j];

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
  if (fp == NULL) {
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

  readString(fp, nameCurrent);

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
    BoneVertex *tmp = new BoneVertex();
    addChild(tmp);
    tmp->readFromFile(fp);
  }

  compileChildList();
}

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
  linkList.addElement(zeLink);
  linkListCompiled = 0;
}

void Vertex::removeLink(BoneLink *zeLink)
{
  linkList.removeElement(zeLink);
  linkListCompiled = 0;
}

void Vertex::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

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
  for (int i=0; i < vertices; i++) {
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

