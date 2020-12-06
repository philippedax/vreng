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

#ifndef WHILE_H
#define WHILE_H

#include "inst.hpp"
#include "data.hpp"
#include "instlist.hpp"
#include "envobject.hpp"

/**
 * class Boucle_While
 */
class Boucle_while : public Instruction
{
 private:

  Data * tant_que;
  // Ce pointeur indique l'objet "Test" qui est argument du "while".
  
  Instruction_liste * alors;
  // Il s'agit de la liste d'instructions executee tant que la condition est
  // remplie.

 public:

  Boucle_while (Data *, Instruction_liste *);
  /**<
   * Constructeur de la classe. Attribue à "tant_que" et "alors" les adresses
   * des arguments du "while".
   */
  
  void exec ();
  /**<
   * Verifie qu'on ne risque pas d'avoir une boucle infinie et exécute un
   * "while" sur le test "tant_que". "alors" est exécuté par appel de sa
   * méthode "exec" si la condition est remplie.
   */

  virtual ~Boucle_while();	///< Destructeur

};

#endif
