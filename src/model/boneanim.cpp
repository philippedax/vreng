//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)
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
#include "boneanim.hpp"


// TODO : remove next lines !
int plainRendering = 1;
int boneEditing = 0;
int axisRendering = 0;
char *selectedNodeName = NULL;


BoneAnimator::BoneAnimator()
{
  mesh = NULL;
  skel = NULL;
  link = NULL;
  links = 0;
  linkListCompiled = 0;
}

BoneAnimator::~BoneAnimator()
{
#if 0 //crash
  //PD this line is correct, but temporally commented because
  //PD a segfault occurs later in the removeLink destructor
  emptyLinkList();
#endif
}

//-----------------
// Accessing datas
void BoneAnimator::registerMesh(BoneMesh *zeMesh)
{
  mesh = zeMesh;
  trace1(DBG_MAN, " Registered as movable mesh: [%s]", zeMesh->getName());
}

void BoneAnimator::registerSkeleton(BoneVertex *zeRoot)
{
  skel = zeRoot;
  trace1(DBG_MAN, " Registered as skeleton: [%s]", zeRoot->getName());
}

//--------------------------------------
// Operations on both mesh and skeleton
void BoneAnimator::scale(float sx, float sy, float sz)
{
  if (! mesh) return;
  if (! skel) return;

  mesh->scale(sx, sy, sz);
  skel->scale(sx, sy, sz);
  genLinkList();
}

//------------------
// Links management
void BoneAnimator::compileLinkList()
{
  link = linkList.getNiceTable(&links);
  linkListCompiled = 1;
}

void BoneAnimator::emptyLinkList()
{
  if (! linkListCompiled) compileLinkList();

  for (int i=0; i<links; i++) delete link[i];

  linkList.empty();
  compileLinkList();
}

inline float BoneAnimator::getLength(Vertex *vertex, BoneVertex *node)
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

inline float BoneAnimator::getWeight(Vertex *vertex, BoneVertex *node)
{
  float result = 0;
  float dist, time;

  if (node->father) {
    getDistanceFromAndOnBone(vertex, node->father, node, &time, &dist);
    result += (time) / (dist * dist);
  }

  for (int i=0; i<node->children; i++) {
    getDistanceFromAndOnBone(vertex, node->child[i], node, &time, &dist);
    result += (time) / (dist * dist);
  }

  result /= node->children + (! node->father ? 0:1);
  result *= node->influenceScaleFactor;
  return result;
}

void normalize(BoneLink **temporaryLink, int temporaryLinks)
{
  float totalWeight = 0;

  for (int i=0; i<temporaryLinks; i++) {
    if (temporaryLink[i]) totalWeight += temporaryLink[i]->weight;
  }
  for (int i=0; i<temporaryLinks; i++) {
    if (temporaryLink[i]) temporaryLink[i]->weight /= totalWeight;
  }
}

// -> Will call the method to generate links and then update the weight
// and optionally remove unsignificant links (that would speed down the cpu)
void BoneAnimator::genLinkList()
{
  // We need both mesh and skeleton !
  if (! mesh) return; // To avoid NULL pointer exception
  if (! skel) return; // To avoid NULL pointer exception

  BoneVertex *tempNode;
  tempNode = skel->getBone("root");
  if (tempNode)
    tempNode->influenceScaleFactor = 10.;
  tempNode = skel->getBone("lipsRoot");
  if (tempNode)
    tempNode->influenceScaleFactor = 10.;
  tempNode = skel->getBone("frontRoot");
  if (tempNode)
    tempNode->influenceScaleFactor = 10.;

  // We start link generation with an empty link list of course
  emptyLinkList();

  // If the mesh has not compiled hos vertices list, we'll do it
  if (! mesh->vertexListCompiled) mesh->compileVertexList();

  // First, we need to generate the initial matrices for
  // all the nodes of the skeleton
  glPushMatrix();
   glLoadIdentity();
   skel->generateInitialMatrix();
  glPopMatrix();

  //--- We'll store the skeleton nodes into a list
  BoneList < BoneVertex > nodeList;
  BoneVertex **node;
  int nodes;

  addNodeAndChildren(skel, &nodeList);
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

  for (int i=0; i<mesh->vertices; i++) {
#ifdef DEBUG_FACE
    // Prints info on progress...
    progressold = progress;
    progress = (i * 10) / (mesh->vertices - 1);
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
      temp = new BoneLink(mesh->vertex[i], node[j], getWeight(mesh->vertex[i], node[j]));
      // We now save this new link in our list
      temporaryLinkList.addElem(temp);
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
    for (int j=0; j<temporaryLinks; j++) {
      if (temporaryLink[j]) linkList.addElem(temporaryLink[j]);
    }
  }

  // Now that we have all the links, we may compile the link list in here
  compileLinkList();

  for (int i=0; i<links; i++) link[i]->notifyTarget();
  for (int i=0; i<mesh->vertices; i++) mesh->vertex[i]->compileLinkList();

#ifdef DEBUG_FACE
  fprintf(stderr, "\n");
  fflush(stderr);
#endif
  trace1(DBG_MAN, "selected links: [%2.2f%%]", (links * 100.) / (mesh->vertices * nodes));
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
  if (node->father) {
    Vect3D nullVector(0,0,0);
    Vect3D fPos; fPos = node->father->currentMatrix * nullVector;
    Vect3D tPos; tPos = node->        currentMatrix * nullVector;

    glVertex3f(fPos.x, fPos.y, fPos.z);
    glVertex3f(tPos.x, tPos.y, tPos.z);
  }
  for (int i=0; i < node->children; i++) {
    renderOneBone(node->child[i]);
  }
}

// Main rendering method, will draw the skeleton and the mesh
void BoneAnimator::render()
{
  // First we draw the skeleton with local coordinates
  if (axisRendering) {
    glPointSize(5.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    renderSkeletonNode(skel);
    glBegin(GL_LINES);
     glColor3f(0.4, 0.4, 0.4);
     renderOneBone(skel);
    glEnd();
  }

  // Now, we'll render the 3d mesh on the screen
  if (! mesh->triangleListCompiled) mesh->compileTriangleList();

  BoneTriangle *triangle;
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

  for (int j=0; j < mesh->triangles; j++) {
    triangle = mesh->triangle[j];
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
void BoneAnimator::renderSkeletonNode(BoneVertex *node)
{
  if (! selectedNodeName) return;

  glPushMatrix();
  glTranslatef(node->currentPosition.x, node->currentPosition.y, node->currentPosition.z);
  glRotatef(node->currentAngle, node->currentAxis.x, node->currentAxis.y, node->currentAxis.z);

  if (! strcmp(selectedNodeName, node->name)) renderLocalCoordinate2();
  else                                        renderLocalCoordinate1();

  // Look at the children of this node
  for (int i=0; i < node->children; i++) {
    renderSkeletonNode(node->child[i]);
  }
  glPopMatrix();
}

// Here comes the morphing part of this code
// This will animate the object
void BoneAnimator::animate()
{
  if (! boneEditing) {
    // First we reset to 0,0,0 all the vertices of the mesh
    for (int i=0; i < mesh->vertices; i++) {
      Vertex *currentVertex = mesh->vertex[i];
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
     skel->generateCurrentMatrix();
    glPopMatrix();

    // And now we can perform the recursive calls to create our new 3d mesh
    animateSkeletonNode(skel);
  }
  else
    for (int i=0; i < mesh->vertices; i++) {
      mesh->vertex[i]->currentPosition = mesh->vertex[i]->initialPosition;
      mesh->vertex[i]->currentNormal   = mesh->vertex[i]->initialNormal;
    }
}

// Recursive part of the animating
inline void BoneAnimator::animateSkeletonNode(BoneVertex *node)
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
#if 0 //DEBUG
    normal  = zeVertex->initialNormal;
    normal *= node->initialRotMatrixInverted;
    normal *= node->currentRotMatrix;
    normal *= zeLink->weight;
    zeVertex->currentNormal += normal;
#endif
  }

  // And now, we'll add the other links actions
  for (int i=0; i < node->children; i++) {
    animateSkeletonNode(node->child[i]);
  }
}

//-----------------
// Private methods
void BoneAnimator::addNodeAndChildren(BoneVertex *boneVertex, BoneList < BoneVertex > *list)
{
  if (! boneVertex->childListCompiled) boneVertex->compileChildList();

  list->addElem(boneVertex);

  for (int i=0; i < boneVertex->children; i++) {
    addNodeAndChildren(boneVertex->child[i], list);
  }
}
