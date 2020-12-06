/****************************************************************************
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 ****************************************************************************/

#ifndef EXTENDS_H
#define EXTENDS_H

#include "class.hpp"
#include "instlist.hpp"
#include "datalist.hpp"

/**
 * class Class_extends
 *
 * permet de modifier uniquement les var de classe !!!
 */

class Class_extends : public Vrel
{
 private:

  char heritage[30];               // classe mere d'une class_extends 
  Instruction_liste * instr;       // liste d'instruction d'une class_extends

 public:
  
  Class_extends(char *, char *, Instruction_liste *);
  /**<
   * - 1. Remplir les champs heritage et instr
   * - 2. Déclarer les variables de la classe mere dans var
   */

  virtual ~Class_extends() {};

  void exec(Data_liste *);
  /**<
   * - 1. Exécuter les instructions de la classe 
   * - 2. Affecter var dans les variables de la classe mere
   * - 3. Exécuter la classe mere
   */
};

#endif
