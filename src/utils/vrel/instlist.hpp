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

#ifndef INSTLIST_HPP
#define INSTLIST_HPP

#include "inst.hpp"

/**
 * class of lists of instructions
 */
class Instruction_liste : public Instruction
{
 public:

  Instruction * actuelle;
  // C'est "actuelle" qui pointe sur l'instruction a effectuer.

  Instruction_liste * suivante;
  // L'enchainement des instructions du programme est assure par l'enchainement
  // des objets "Instruction_liste" qui pointent vers les objets "Instruction".

  Instruction_liste ();
  // Constructeur de la classe. Initialise "actuelle" et "suivante" comme
  // pointeurs NULL.

  void addInstruction (Instruction *);
  // Methode permettant de faire pointer l'attribut "actuelle" de cet objet 
  // vers un objet "Instruction", et de creer l'objet "Instruction_liste" 
  // pointe par "suivante".

  void exec ();
  // Methode appelant la methode "exec()" de "actuelle" si "actuelle != NULL"
  // et la methode "exec" de "suivante" si "suivante != NULL"

  virtual ~Instruction_liste ();
  // Destructeur de la classe.

};

#endif
