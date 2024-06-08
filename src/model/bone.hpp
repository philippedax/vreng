//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// bone.hpp
//
// Author   : Yann Renard
// Copyright: This file is totaly free and you may distribute
//            it to anyone you want, without modifying this
//            header. If you use it in a commercial project (?)
//            or in bigger project (!), I would be glad to know
//            about it :) Please mail me... myself_yr@hotmail.com
//---------------------------------------------------------------------------
#ifndef BONE_HPP
#define BONE_HPP

#include "vec3.hpp"

class BoneLink;

/**
 * BoneNode class
 *
 * La classe BoneNode represente un noeud de la liste chainee.
 * Elle contient:
 * - un pointeur sur l'element stocke dans ce noeud
 * - un pointeur sur le noeud suivant
 * - un pointeur sur le noeud precedent
 *  Cette classe ne fait que gerer ces champs (lecture/ecriture).
 */
template <class BoneElem> class BoneNode {

 private:
  BoneNode *nextNode;  ///< Noeud precedent this dans la liste.
  BoneNode *prevNode;  ///< Noeud suivant this dans la liste.
  BoneElem *element;   ///< Element de la liste contenu dans this.

 public:
  BoneNode() {
    nextNode = NULL;
    prevNode = NULL;
    element  = NULL;
  }
  /**< Constructeur de noeud, initialise tous les pointeurs a NULL */

  BoneNode(BoneNode &node) {
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
  void setPrev(BoneNode *prev) { prevNode = prev; }
  void setNext(BoneNode *next) { nextNode = next; }
  void setElement(BoneElem *elem) { element = elem; }
  void setValue(BoneNode *prev, BoneNode *next, BoneElem *elem) {
    setNext(next);
    setPrev(prev);
    setElement(elem);
  }

  // Methodes de lecture des champs d'un noeud
  BoneNode *getNext(void) { return nextNode; }
  BoneNode *getPrev(void) { return prevNode; }
  BoneElem *getElem(void) { return element;  } // FIXME! segfault
};


/**
 * BoneList class
 *
 * Class BoneList:
 * la classe liste est la classe principale de gestion de la liste chainee.
 * Elle stocke les noeuds contenant les pointeurs sur elements a la queu leu leu
 * en commencant par 'listHead'
 * Methodes : void  empty (void)
 *            int   count (void)
 *            void  addElement (BoneElem *elem)
 *            void  addList (BoneList * list)
 *            void  delElement (int index)
 *            void  delElement (BoneElem * ptr)
 *            BoneElem * getElement (int index)
 *            BoneElem ** getNiceTable (int * count)
 */
template <class BoneElem> class BoneList {

 private:
  BoneNode <BoneElem> *listHead;	///< Tete de la liste chainee
  BoneNode <BoneElem> *listEnd;		///< Queue de la liste chainee
  BoneElem            **element;	///< Table contenant les pointeurs sur element lors de la compilation de la liste en tableau
  int                  elements;	///< Nombre d'elements dans la table lors de la compilation de la liste en tableau
  int                  built;		///< Masque disant si la liste est compilee ou non

  BoneNode <BoneElem> *getFirstNode() { return listHead; }
  /**< Recherche du premier noeud de la liste */

  BoneNode <BoneElem> *getLastNode() { return listEnd; }
  /**< Recherche du dernier noeud de la liste */

  /** Compilation de la liste chainee en tableau
   * Cette methode ne s'execute que si la liste a ete modifiee
   * dans la passe (addElement ou delElement).
   */
  void rebuild()
  {
    if (built) return;

    BoneNode <BoneElem> *node = listHead;

    elements = count();
    FREE(element);
    element = (BoneElem **) malloc(elements * sizeof(BoneElem *));
    for (int i=0; i<elements; i++, node = node->getNext()) {
      element[i] = node->getElem();
    }
    built = 1;
  }

  /** Fonction recursive de vidage de la liste, il
   * s'agit d'appeler le vidage du fils puis de liberer le fils
   */
  void recursiveEmpty(BoneNode <BoneElem> *node)
  {
    if (node) {
      recursiveEmpty(node->getNext());
      delete node;
    }
  }

 public:
  BoneList()
  {
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
  void empty()
  {
    recursiveEmpty(listHead);
    listHead = NULL;
    listEnd  = NULL;
    element  = NULL;
    built    = 0;
  }

  // Comptage du nombre d'elements de la liste
  int count()
  {
    BoneNode <BoneElem> *node = listHead;
    int n = 0;
    while (node) {
      node = node->getNext();
      n++;
    }
    return n;
  }

  // Fonction d'acces a un element de la liste retourne NULL si
  // l'index est inferieur a 0 ou superieur au nombre d'elements
  BoneElem *getElement(int index)
  {
    BoneNode<BoneElem> *node = listHead;
    BoneElem *result = NULL;
    while (node && index > 0) {
      node = node->getNext();
      index--;
    }
    if (node && index == 0)
      result = node->getElem();
    return result;
  }

  // Ajouter un element dans la liste, cet element est ajoute en queue de liste
  void addElement(BoneElem *elem)
  {
    BoneNode<BoneElem> *newNode = new BoneNode<BoneElem>();
    newNode->setElement(elem);

    if (! listHead) {
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
  void addList(BoneList *list)
  {
    BoneNode<BoneElem> *newNode = list->getFirstNode();

    if (! listHead) {
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
  void delElement(int index)
  {
    if (! listHead) return;

    BoneNode<BoneElem> *prevNode;
    BoneNode<BoneElem> *node;
    BoneNode<BoneElem> *nextNode;

    if (index == 0) { // Cas particulier de la tete
      prevNode = NULL;
      nextNode = listHead->getNext();
      delete listHead;
      listHead = nextNode;
    }
    else { // Autre cas
      node = listHead;
      while ((index > 0) && node) {
        node = node->getNext();
        index--;
      }
      if (node) {
        nextNode = node->getNext();
        prevNode = node->getPrev();
        delete node;
        if (nextNode) nextNode->setPrev(prevNode);
        if (prevNode) prevNode->setNext(nextNode);
      }
    }
    built = 0;
  }

  // Enlever un element dans la liste, tous les elements
  // suivants remontent d'un rang
  void delElement(BoneElem *ptr)
  {
    if (! listHead) return;

    BoneNode<BoneElem> *prevNode;
    BoneNode<BoneElem> *node;
    BoneNode<BoneElem> *nextNode;

    if (ptr == listHead->getElem()) { // Cas particulier de la tete
      prevNode = NULL;
      nextNode = listHead->getNext();
      delete listHead;
      listHead = nextNode;
    }
    else { // Autre cas
      node = listHead;
      while ((node) && (node->getElem() != ptr)) {
        node = node->getNext();
      }
      if (node) {
        nextNode = node->getNext();
        prevNode = node->getPrev();
        delete node;
        if (nextNode) nextNode->setPrev(prevNode);
        if (prevNode) prevNode->setNext(nextNode);
      }
    }
    built = 0;
  }

  // Compilation de la liste en un tableau. Cette methode
  // retourne le nombre d'elements dans la variable count
  BoneElem **getNiceTable(int *count)
  {
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

  void setName(char *Name);

  char *getName();
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

  // -> Position at the begining
  Vect3D iniPosition;
  float  iniAngle;
  Vect3D iniAxis;

  // -> Position during the animation
  Vect3D curPosition;
  float  curAngle;
  Vect3D curAxis;

  // -> Children management
  BoneList <BoneVertex> childList;
  BoneVertex **child;
  BoneVertex *father;
  int children;
  int childListCompiled;

  // -> Links management
  // (a link is defined by a bone vertex, a vertex of the mesh and a weight)
  BoneList <BoneLink> linkList;
  BoneLink **link;
  int links;
  int compiled;

  // -> Saves of the matrices
  float iniMatrix[16];
  float iniMatrixInverted[16];
  float curMatrix[16];
  // -> Same for rotation matrices (may be used for normal computations)
  float iniRotMatrix[16];
  float iniRotMatrixInverted[16];
  float curRotMatrix[16];

  // -> a field to scale the influence of this boneVertex
  float influenceScaleFactor;

  // -> a field to tell if this boneVertex should be animated or not
  //dax int animated;

  // Construct / destruct
  BoneVertex();
  BoneVertex(Vect3D &position, float angle, Vect3D &axis);
  BoneVertex(Vect3D *position, float angle, Vect3D *axis);
  virtual ~BoneVertex();

  // Accessing initial position datas
  void setIniPos(Vect3D &position);
  void setIniPos(Vect3D *position);
  void setIniPos(float ox, float oy, float oz);
  void setIniRot(float angle, Vect3D &axis);
  void setIniRot(float angle, Vect3D *axis);
  void setIniRot(float angle, float axisx, float axisy, float axisz);

  // Accessing current position datas (during animation)
  void setPos(Vect3D &position);
  void setPos(Vect3D *position);
  void setPos(float ox, float oy, float oz);
  void setRot(float angle, Vect3D &axis);
  void setRot(float angle, Vect3D *axis);
  void setRot(float angle, float axisx, float axisy, float axisz);

  // Accessing current position datas (during animation)
  // with relative values (realtive to initial position)
  void resetPos();
  void resetRot();
  void setTrans(Vect3D &delta);
  void setTrans(Vect3D *delta);
  void setTrans(float dx, float dy, float dz);
  void setScale(float scalex, float scaley, float scalez);
  void setScale(float scale);

  // Actions on this node and its children
  // Those actions are definitives
  void scale(float sx, float sy, float sz);

  // Children list managing
  void setBone(BoneVertex *Father);
  void addBone(BoneVertex *newChild);
  void delBone(const char *Name);
  BoneVertex *getBone(const char *Name);

  // Link list managing
  void addLink(BoneLink *link);
  void delLink(BoneLink *link);

  // Intern functions to compile the lists... should be private maybe ?
  void makeChildList();
  void makeLinkList();
  void genIniMatrix();		// needs glPush / glPop and glLoadIdentity at the begining
  void genCurrentMatrix();	// needs glPush / glPop and glLoadIdentity at the begining

  // I/O functions
  void read(char *filename, float size = 1.);
  void readSkel(FILE *file, float scale = 1.);
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
  unsigned int val;
  val = b1;
  val <<= 8; val += b2;
  val <<= 8; val += b3;
  val <<= 8; val += b4;
  return val;
}

inline float readFloat(FILE *in)
{
  int val = readInt(in);
  float *valptr = (float *) &val;
  return *valptr;
}

inline char *readStr(FILE *in, char *str)
{
  int i=0;
  while ( (str[i++] = readChar(in)) != '\0' );
  return str;
}

//---------------------------------------------------------------------------

/**
 * Vertex class
 */
class Vertex {
 public:
  // Champs de la classe vertex
  Vect3D iniPosition;
  Vect3D curPosition;
  Vect3D iniNormal;
  Vect3D curNormal;

  // -> Liens
  BoneList <BoneLink> linkList;
  BoneLink **link;
  int links;
  int compiled;

  // -> Coordonnees de mapping
  float u,v;

  // Constructeurs / destructeur
  Vertex();
  Vertex(Vect3D &position);
  Vertex(Vect3D *position);
  Vertex(float ox, float oy, float oz);
  virtual ~Vertex() {}

  // Acces aux champs
  void setPosition(Vect3D &position);
  void setPosition(Vect3D *position);

  // Gestion des liens
  void addLink(BoneLink *link);
  void delLink(BoneLink *link);

  // Acces aux champs de transformation initiaux
  void makeLinkList();

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

  BoneLink(Vertex *Vertex, BoneVertex *BoneVertex, float weight);
  /**< Constructor. */

  virtual ~BoneLink();
  /**< Destructor. */

  void setVertex(Vertex *Vertex);
  void setBoneVertex(BoneVertex *BoneVertex);
  void notifyTarget();
  void setWeight(float weight);
};

//---------------------------------------------------------------------------

/**
 * BoneTriangle class
 */
class BoneTriangle {
 public:

  // Datas for the triangle class
  Vertex *vertex1; float u1, v1; int index1;
  Vertex *vertex2; float u2, v2; int index2;
  Vertex *vertex3; float u3, v3; int index3;
  Vect3D iniNormal;
  Vect3D curNormal;
  float r, g, b, a; // Initial colors
  float R, G, B, A; // Projected colors

  // Construct / destruct
  BoneTriangle();

  virtual ~BoneTriangle();

  // Accessing datas
  void addVertex(Vertex *vertex, int index, float u, float v);

  void rebuildNormal();

  //void setColor(float r=0.5, float g=0.5, float b=0.5, float a=1);
  void setColor(float r, float g, float b, float a);
};

//---------------------------------------------------------------------------

/**
 * BoneMesh class
 */
class BoneMesh : public Bonename {
 public:

  // Gestion des sommets
  BoneList <Vertex> vertexList;
  Vertex **vertex;
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
  void addVertex(Vect3D & position);
  void addVertex(Vect3D * position);
  void addVertex(float ox, float oy, float oz);
  void addTriangle(int index1, int index2, int index3);

  // Fonctions internes
  void makeVertexList();
  void makeTriangleList();

  // Modification du mesh
  void rebuildNormals();
  void scale (float sx, float sy, float sz);
  void projectLight();
};

//---------------------------------------------------------------------------

/**
 * Bone class
 *
 * This is the main class that is to animate the bones
 * and the 3d mesh correctly... It creates the link
 * between the different vertices and bones and
 * calculates the new position each time you ask
 *
 * Typical use of this class :
 *   myBone.registerMesh ( oneMesh );
 *   myBone.registerSkel ( oneSkeleton );
 *   myBone.makeLinkList ();
 *   ... in loop ...
 *     change skeleton current pos
 *   myBone.animate ();
 *   myBone.render ();
 *   ... end loop ...
 */
class Bone {
 public:

  /* What we'll animate */
  BoneMesh *mesh;
  BoneVertex *skel;

  /* Links management */
  BoneList <BoneLink> linkList;
  BoneLink **link;
  int links;
  int compiled;

  Bone();		///< Constructor
  virtual ~Bone();	///< Destructor

  // Accessing datas
  void registerMesh(BoneMesh *mesh);
  void registerSkel(BoneVertex *root);

  // Operations on both mesh and skeleton
  void scale(float sx, float sy, float sz);

  // List compilation
  void makeLinkList();
  void emptyLinkList();
  void genLinkList();
  float getLength(Vertex *vertex, BoneVertex *node);
  float getWeight(Vertex *vertex, BoneVertex *node);

  // Rendering and animating the mesh and skeleton
  void render();
  //notused void renderSkeletonNode(BoneVertex *node);
  //notused void animate();
  //notused void animateSkeletonNode(BoneVertex *node);

 private:
  void addNodeAndChildren(BoneVertex *boneVertex, BoneList <BoneVertex> *list);
};

#endif
