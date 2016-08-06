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

#ifndef INST_H
#define INST_H

#include "vrel.hpp"

/**
 * metaclass of instructions
 *
 * Cette classe est en fait le prototype dont hériteront toutes les classes
 * d'instructions que nous alons voir ci-dessous. Les méthodes "exec()" seront
 * redéfinies dans chaque cas, et des méthodes propres à chaque classe seront
 * ajoutées.
 * L'attribut ligne n'apparait que dans l'objet "Instruction" car toutes les
 * erreurs qui peuvent apparaitre au niveau de la syntaxe seront prises en
 * charge par bison, le numéro de ligne compris. L'attribut "ligne" ici défini
 * permettra donc la localisation des erreurs d'exécution d'une instruction.
 */
class Instruction
{
 public:

  // Numéro de ligne fournit par bison et qui sert aux messages d'erreur.
  int ligne;

  // Fichier ou se trouve l'instruction.
  char file[100];

  // Type de l'instruction.
  int kind;

  // Méthode d'exécution de l'instruction.
  virtual void exec () = 0;

  // Méthode d'erreur.
  void erreur (const char * texte) {
    printf ("Error: file %s, line %d: %s\n", file, ligne, texte);
    exit(1);
  }
  
  // Destructeur.
  virtual ~Instruction() {};
  
};

#endif
