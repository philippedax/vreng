//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#ifndef BONE_HPP
#define BONE_HPP

#include "vec3.hpp"

class BoneLink;


//---------------------------------------------------------------------------

/**
 * La classe BoneNode represente un noeud de la liste chainee.
 * Elle contient:
 * - un pointeur sur l'element stocke dans ce noeud
 * - un pointeur sur le noeud suivant
 * - un pointeur sur le noeud precedent
 *  Cette classe ne fait que gerer ces champs (lecture/ecriture).
 */
template <class BoneElem> class BoneNode {

 private:
  BoneNode * nextNode;  ///< Noeud precedent this dans la liste.
  BoneNode * prevNode;  ///< Noeud suivant this dans la liste.
  BoneElem * element;   ///< Element de la liste contenu dans this.

 public:
  BoneNode() {
    nextNode = NULL;
    prevNode = NULL;
    element  = NULL;
  }
  /**< Constructeur de noeud, initialise tous les pointeurs a NULL */

  BoneNode(BoneNode & node) {
    nextNode = node.nextNode;
    prevNode = node.prevNode;
    element  = node.element;
  }
  /**<
   * Constructeur de noeud par recopie, recopie tous les champs
   * du noeud passe en parametre
   */

  virtual ~BoneNode() {}
  ///< destructor

  // Methodes de remplissage des champs d'un noeud
  inline void setPrev(BoneNode *zePrev) { prevNode = zePrev; }
  inline void setNext(BoneNode *zeNext) { nextNode = zeNext; }
  inline void setElement(BoneElem *zeElement) { element = zeElement; }
  inline void setValue(BoneNode *zePrev, BoneNode *zeNext, BoneElem *zeElement) {
    setNext(zeNext);
    setPrev(zePrev);
    setElement(zeElement);
  }

  // Methodes de lecture des champs d'un noeud
  inline BoneNode *getNext (void) { return nextNode; }
  inline BoneNode *getPrev (void) { return prevNode; }
  inline BoneElem *getElem (void) { return element; }
 };


/**
 * Class BoneList:
 * la classe liste est la classe principale de gestion de la liste chainee.
 * Il stocke les noeuds contenant les pointeurs sur elements a la queu leu leu
 * en commencant par 'listHead'
 * Methodes : void  empty (void)
 *            int   count (void)
 *            void  addElement (BoneElem * zeElement)
 *            void  addList (BoneList * zeList)
 *            void  removeElement (int index)
 *            void  removeElement (BoneElem * ptr)
 *            BoneElem * getElemAt (int index)
 *            BoneElem ** getNiceTable (int * count)
 */
template <class BoneElem> class BoneList {

 private:
  BoneNode <BoneElem> *listHead;	///< Tete de la liste chainee
  BoneNode <BoneElem> *listEnd;		///< Queue de la liste chainee
  BoneElem            **element;	///< Table contenant les pointeur sur element lors de la compilation de la liste en tableau
  int                  elements;	///< Nombre d'elements dans la table lors de la compilation de la liste en tableau
  int                  built;	///< Masque disant si la liste est compilee ou non

  inline BoneNode <BoneElem> *getFirstNode() { return listHead; }
  /**< Recherche du premier noeud de la liste */

  inline BoneNode <BoneElem> *getLastNode() { return listEnd; }
  /**< Recherche du dernier noeud de la liste */

  /** Compilation de la liste chainee en tableau
   * Cette methode ne s'execute que si la liste a ete modifiee
   * dans la passe (addElement ou removeElement).
   */
  inline void rebuild() {
    if (built) return;

    BoneNode <BoneElem> *currentNode = listHead;

    elements = count();
    FREE(element);
    element = (BoneElem **) malloc (elements * sizeof(BoneElem *));
    for (int i=0; i<elements; i++, currentNode = currentNode->getNext())
      element[i] = currentNode->getElem();
    built = 1;
  }

  /** Fonction recursive de vidage de la liste, il
   * s'agit de d'appeler le vidage du fils puis de liberer le fils
   */
  inline void recursiveEmpty(BoneNode <BoneElem> *currentNode) {
    if (currentNode) {
      recursiveEmpty(currentNode->getNext());
      delete currentNode;
    }
  }

 public:

  BoneList() {
    listHead = NULL;
    listEnd  = NULL;
    element  = NULL;
    elements = 0;
    built    = 0;
  }
  ///< Constructeur par defaut. Il initialise la liste comme une liste vide

  virtual ~BoneList() { empty(); }
  ///< Le destructeur vide la liste de son contenu

  // Vidage de la liste enleve les noeuds un par un et efface le tableau compile
  inline void empty() {
    recursiveEmpty(listHead);
    listHead = NULL;
    listEnd  = NULL;

    //dax segfault FREE(element);
    element = NULL;
    built = 0;
  }

  // Comptage du nombre d'elements de la liste
  inline int count() {
    BoneNode <BoneElem> *currentNode = listHead;
    int result = 0;
    while (currentNode) {
      currentNode = currentNode->getNext();
      result++;
    }
    return result;
  }

  // Fonction d'acces a un element de la liste retourne NULL si
  // l'index est inferieur a 0 ou superieur au nombre d'elements
  inline BoneElem *getElemAt(int index) {
    BoneNode<BoneElem> *currentNode = listHead;
    BoneElem *result = NULL;
    while (currentNode && index > 0) {
      currentNode = currentNode->getNext();
      index--;
    }
    if (currentNode && index == 0)
      result = currentNode->getElem();
    return result;
  }

  // Ajouter un element dans la liste, cet element est ajoute en queue de liste
  inline void addElement(BoneElem *zeElement) {
    BoneNode<BoneElem> *newNode = new BoneNode<BoneElem>();
    newNode->setElement(zeElement);

    if (listHead == NULL) {
      listHead = newNode;
      listEnd  = newNode;
    }
    else {
      listEnd->setNext(newNode);
      newNode->setPrev(listEnd);
      listEnd = newNode;
    }
    built = 0;
  }

  // Ajouter une liste dans cette liste
  inline void addList(BoneList *zeList) {
    BoneNode<BoneElem> *newNode = zeList->getFirstNode();

    if (listHead == NULL) {
      listHead = newNode;
      listEnd  = newNode;
    }
    else {
      listEnd->setNext(newNode);
      newNode->setPrev(listEnd);
      listEnd = newNode;
    }
    built = 0;
  }

  // Enlever un element dans la liste, tous les elements
  // suivants remontent d'un rang
  inline void removeElement(int index) {
    if (listHead == NULL) return;

    BoneNode<BoneElem> *prevNode;
    BoneNode<BoneElem> *currentNode;
    BoneNode<BoneElem> *nextNode;

    if (index == 0) { // Cas particulier de la tete
      prevNode = NULL;
      nextNode = listHead->getNext();
      delete listHead;
      listHead = nextNode;
    }
    else { // Autre cas
      currentNode = listHead;
      while ((index > 0) && currentNode) {
        currentNode = currentNode->getNext();
        index--;
      }
      if (currentNode) {
        nextNode = currentNode->getNext();
        prevNode = currentNode->getPrev();
        delete currentNode;
        if (nextNode) nextNode->setPrev(prevNode);
        if (prevNode) prevNode->setNext(nextNode);
      }
    }
    built = 0;
  }

  // Enlever un element dans la liste, tous les elements
  // suivants remontent d'un rang
  inline void removeElement(BoneElem *ptr) {
    if (listHead == NULL) return;

    BoneNode<BoneElem> *prevNode;
    BoneNode<BoneElem> *currentNode;
    BoneNode<BoneElem> *nextNode;

    if (ptr == listHead->getElem()) { // Cas particulier de la tete
      prevNode = NULL;
      nextNode = listHead->getNext();
      delete listHead;
      listHead = nextNode;
    }
    else { // Autre cas
      currentNode = listHead;
      while ((currentNode) && (currentNode->getElem() != ptr))
        currentNode = currentNode->getNext();
      if (currentNode) {
        nextNode = currentNode->getNext();
        prevNode = currentNode->getPrev();
        delete currentNode;
        if (nextNode) nextNode->setPrev(prevNode);
        if (prevNode) prevNode->setNext(nextNode);
      }
    }
    built = 0;
  }

  // Compilation de la liste en un tableau. Cette methode
  // retourne le nombre d'elements dans la variable count
  inline BoneElem **getNiceTable(int *count) {
    if (! built) rebuild();
    if (count) *count = elements;
    return element;
  }
};

//---------------------------------------------------------------------------

/**
 * BoneName class
 */
class Bonename {
 public:
  char *name;	///< name

  Bonename();
  Bonename(char *op);
  Bonename(Bonename *op);
  Bonename(Bonename &op);

  virtual ~Bonename();

  virtual void setName(char *zeName);

  virtual char *getName();
};

//---------------------------------------------------------------------------

/**
 * BoneVertex class
 *
 * This class represents the extremity of a bone
 * So a bone is made of one of this class (that
 * contains translation/rotation) and a reference
 * to it's father ( the extremity it depends of )
 * This extremity also has several children whom
 * he'll be the father ( see that like a tree )
 */
class BoneVertex : public Bonename {
 public:

  // Datas in the bone vertex class

  // -> Position at the begining
  Vect3D initialPosition;
  float  initialAngle;
  Vect3D initialAxis;

  // -> Position during the animation
  Vect3D currentPosition;
  float  currentAngle;
  Vect3D currentAxis;

  // -> Children management
  BoneList < BoneVertex > childList;
  BoneVertex **child;
  BoneVertex *father;
  int children;
  int childListCompiled;

  // -> Links management
  // (a link is defined by a bone vertex, a vertex of the mesh and a weight)
  BoneList < BoneLink > linkList;
  BoneLink **link;
  int links;
  int linkListCompiled;

  // -> Saves of the matrices
  float initialMatrix[16];
  float initialMatrixInverted[16];
  float currentMatrix[16];
  // -> Same for rotation matrices (may be used for normal computations)
  float initialRotMatrix[16];
  float initialRotMatrixInverted[16];
  float currentRotMatrix[16];

  // -> a field to scale the influence of this boneVertex
  float influenceScaleFactor;

  // -> a field to tell if this boneVertex should animated or not
  int animated;

  // Construct / destruct
  BoneVertex();
  BoneVertex(Vect3D & zePosition, float zeAngle, Vect3D & zeAxis);
  BoneVertex(Vect3D * zePosition, float zeAngle, Vect3D * zeAxis);
  virtual ~BoneVertex();

  // Accessing initial position datas
  void setInitialPosition(Vect3D & zePosition);
  void setInitialPosition(Vect3D * zePosition);
  void setInitialPosition(float ox, float oy, float oz);
  void setInitialRotation(float zeAngle, Vect3D & zeAxis);
  void setInitialRotation(float zeAngle, Vect3D * zeAxis);
  void setInitialRotation(float zeAngle, float axisx, float axisy, float axisz);

  // Accessing current position datas ( during animation)
  void setCurrentPosition(Vect3D & zePosition);
  void setCurrentPosition(Vect3D * zePosition);
  void setCurrentPosition(float ox, float oy, float oz);
  void setCurrentRotation(float zeAngle, Vect3D & zeAxis);
  void setCurrentRotation(float zeAngle, Vect3D * zeAxis);
  void setCurrentRotation(float zeAngle, float axisx, float axisy, float axisz);

  // Accessing current position datas ( during animation)
  // with relative values (realtive to initial position)
  void resetCurrentPosition(void);
  void resetCurrentRotation(void);
  void translateCurrentPosition(Vect3D & delta);
  void translateCurrentPosition(Vect3D * delta);
  void translateCurrentPosition(float dx, float dy, float dz);
  void scaleCurrentPosition(float scalex, float scaley, float scalez);
  void scaleCurrentPosition(float scale);

  // Actions on this node and its children
  // Those actions are definitives
  void scale(float sx, float sy, float sz);

  // Children list managing
  void setFather(BoneVertex *zeFather);
  void addChild(BoneVertex *newChild);
  void removeChild(const char *zeName);
  BoneVertex *findChild(const char *zeName);

  // Link list managing
  void addLink(BoneLink *zeLink);
  void removeLink(BoneLink *zeLink);

  // Intern functions to compile the lists... should be private maybe ?
  void compileChildList();
  void compileLinkList();
  void generateInitialMatrix(); // needs glPush / glPop and glLoadIdentity at the begining
  void generateCurrentMatrix(); // needs glPush / glPop and glLoadIdentity at the begining

  // I/O functions
  void read(char *filename, float size = 1.);
  void readFromFile(FILE *file, float size = 1.);

};

// Reading part
inline char readChar(FILE *in)
{
  return fgetc(in);
}

inline short readShort(FILE *in)
{
  char b1 = fgetc(in);
  char b2 = fgetc(in);
  return (b1<<8) + b2;
}

inline int readInt(FILE *in)
{
  unsigned char b1 = fgetc(in);
  unsigned char b2 = fgetc(in);
  unsigned char b3 = fgetc(in);
  unsigned char b4 = fgetc(in);
  unsigned int result;
  result = b1;
  result <<= 8; result += b2;
  result <<= 8; result += b3;
  result <<= 8; result += b4;
  return result;
}

inline float readFloat(FILE *in)
{
  int value = readInt(in);
  float *valuePtr = (float *) &value;
  return *valuePtr;
}

inline char *readString(FILE *in, char *str)
{
  int cpt=0;
  while ((str[cpt++] = readChar(in)) != '\0');
  return str;
}

/**
 * Vertex class
 */
class Vertex {
 public:
  // Champs de la classe vertex
  Vect3D initialPosition;
  Vect3D currentPosition;
  Vect3D initialNormal;
  Vect3D currentNormal;

  // -> Liens
  BoneList < BoneLink > linkList;
  BoneLink **link;
  int links;
  int linkListCompiled;

  // -> Coordonnees de mapping
  float u,v;

  // Constructeurs / destructeur
  Vertex();
  Vertex(Vect3D & zePosition);
  Vertex(Vect3D * zePosition);
  Vertex(float ox, float oy, float oz);
  virtual ~Vertex() {}

  // Acces aux champs
  void setPosition(Vect3D & zePosition);
  void setPosition(Vect3D * zePosition);

  // Gestion des liens
  void addLink(BoneLink *zeLink);
  void removeLink(BoneLink *zeLink);

  // Acces aux champs de transformation initiaux
  void compileLinkList();

private:
  void defaults();

};

//---------------------------------------------------------------------------

/**
 * BoneLink class
 */
class BoneLink {
public:
  Vertex *vertex;		///< vertex.
  BoneVertex *boneVertex;	///< BoneVertex.
  float weight;			///< weight.

  BoneLink(Vertex *zeVertex, BoneVertex *zeBoneVertex, float zeWeight);
  /**< Constructor. */

  virtual ~BoneLink();
  /**< Destructor. */

  virtual void setVertex(Vertex *zeVertex);
  virtual void setBoneVertex(BoneVertex *zeBoneVertex);
  virtual void notifyTarget();
  virtual void setWeight(float zeWeight);

};

//---------------------------------------------------------------------------

/**
 * BoneTriangle class
 */
class BoneTriangle {
 public:

  // Datas for the triangle class
  Vertex * vertex1; float u1, v1; int index1;
  Vertex * vertex2; float u2, v2; int index2;
  Vertex * vertex3; float u3, v3; int index3;
  Vect3D initialNormal;
  Vect3D currentNormal;
  float colorRed, colorGreen, colorBlue, colorAlpha; // Initial colors
  float finalRed, finalGreen, finalBlue, finalAlpha; // Projected colors

  // Construct / destruct
  BoneTriangle();

  virtual ~BoneTriangle();

  // Accessing datas
  //void addVertex(Vertex *zeVertex, int index, float u=-1, float v=-1);
  virtual void addVertex(Vertex *zeVertex, int index, float u, float v);

  void rebuildNormal();

  //virtual void setColor(float r=0.5, float g=0.5, float b=0.5, float a=1);
  virtual void setColor(float r, float g, float b, float a);
};

//---------------------------------------------------------------------------

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
  virtual void addVertex(Vect3D & zePosition);
  virtual void addVertex(Vect3D * zePosition);
  virtual void addVertex(float ox, float oy, float oz);
  virtual void addTriangle(int index1, int index2, int index3);

  // Fonctions internes
  void compileVertexList();
  void compileTriangleList();

  // Modification du mesh
  virtual void rebuildNormals();
  virtual void scale (float sx, float sy, float sz);
  virtual void projectLight();

};

//---------------------------------------------------------------------------

/**
 * This is the main class that is to animate the bones
 * and the 3d mesh correctly... It creates the link
 * between the different vertices and bones and
 * calculates the new position each time you ask
 *
 * Typical use of this class :
 *   myBone.registerMesh ( oneMesh );
 *   myBone.registerSkeleton ( oneSkeleton );
 *   myBone.compileLinkList ();
 *   ... in loop ...
 *     change skeleton current pos
 *   myBone.animate ();
 *   myBone.render ();
 *   ... end loop ...
 */

class Bone {

 public:

  /* What we'll animate */
  BoneMesh *meshToMove;
  BoneVertex *skeleton;

  /* Links management */
  BoneList <BoneLink> linkList;
  BoneLink **link;
  int links;
  int linkListCompiled;

  Bone();		///< Constructor
  virtual ~Bone();	///< Destructor

  // Accessing datas
  virtual void registerMesh(BoneMesh *zeMesh);
  virtual void registerSkeleton(BoneVertex *zeRoot);

  // Operations on both mesh and skeleton
  virtual void scale(float sx, float sy, float sz);

  // List compilation
  virtual void compileLinkList();
  virtual void emptyLinkList();
  virtual void generateLinkList();
  inline float getLength(Vertex *vertex, BoneVertex *node);
  inline float getWeight(Vertex *vertex, BoneVertex *node);

  // Rendering and animating the mesh and skeleton
  virtual void render();
  virtual void renderSkeletonNode(BoneVertex *node);
  virtual void animate();
  virtual void animateSkeletonNode(BoneVertex *node);

 private:
  void addNodeAndChildren(BoneVertex *boneVertex, BoneList < BoneVertex > *list);
};

//---------------------------------------------------------------------------

/**
 * V3d class
 */
class V3d {
 public:

  V3d() {}		///< constructor
  virtual ~V3d() {}	///< destructor

  static void readV3Dfile(BoneMesh *result,
			BoneVertex *skeletonRoot,
			char *filename,
			float size = 1.);
};

//---------------------------------------------------------------------------

#endif
