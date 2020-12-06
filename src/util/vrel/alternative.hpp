/******************************************************************************
 *                               VREL COMPONENTS                              *
 *                                                                            *
 *                           Copyright (C) 2000                               *
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

#ifndef ALTERNATIVE_H
#define ALTERNATIVE_H

#include "inst.hpp"
#include "instlist.hpp"
#include "data.hpp"


/**
 * class Alternative
 */
class Alternative : public Instruction
{
 private:
  
  Data * si;  // Data de type Test.
  Instruction_liste * alors;
  Instruction_liste * sinon;

 public:

  Alternative (Data *, Instruction_liste *);
  Alternative (Data *, Instruction_liste *, Instruction_liste *);
  // Constructeurs de la classe. Attribue a "si", "alors" et "sinon" les
  // adresses des arguments du "if ... then ... else".

  void exec ();
  // Execute le test puis la liste d'instructions pertiente en fonction du
  // resultat du test. 

  virtual ~Alternative();	///< Destructeur
};

#endif
