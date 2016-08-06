/******************************************************************************
 *                              VREL COMPONENTS                               *
 *                                                                            *
 *                            Copyright (C) 2000                              *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin      *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *****************************************************************************/

#ifndef INSTLIST_H
#define INSTLIST_H

#include "inst.hpp"

/**
 * class of lists of instructions
 */
class Instruction_liste : public Instruction
{
 public:

  Instruction * actuelle;
  // C'est "actuelle" qui pointe sur l'instruction à effectuer.

  Instruction_liste * suivante;
  // L'enchainement des instructions du programme est assuré par l'enchainement
  // des objets "Instruction_liste" qui pointent vers les objets "Instruction".

  Instruction_liste ();
  // Constructeur de la classe. Initialise "actuelle" et "suivante" comme
  // pointeurs NULL.

  void addInstruction (Instruction *);
  // Méthode permettant de faire pointer l'attribut "actuelle" de cet objet 
  // vers un objet "Instruction", et de créer l'objet "Instruction_liste" 
  // pointé par "suivante".

  void exec ();
  // Méthode appelant la méthode "exec()" de "actuelle" si "actuelle != NULL"
  // et la méthode "exec" de "suivante" si "suivante != NULL"

  virtual ~Instruction_liste ();
  // Destructeur de la classe.

};

#endif
