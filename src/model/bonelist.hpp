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
#ifndef BONELIST_HPP
#define BONELIST_HPP


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

#endif
