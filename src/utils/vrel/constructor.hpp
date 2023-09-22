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

#ifndef CONSTRUCTOR_HPP
#define CONSTRUCTOR_HPP

#include "decllist.hpp"
#include "instlist.hpp"
#include "envvar.hpp"
#include "datalist.hpp"
#include "envobject.hpp"
#include "namelist.hpp"

/**
 * class Constructeur for compound
 */
class Constructeur
{
 private:
  
  Instruction_liste * instr;       ///< liste des instructions du constructeur
  Env_var * argument;    ///< liste des arguments
  Env_objets * envobj;   ///< environnement local d'objets.
  Env_var * declfixe ;   ///< variables initiales locales.

 public:

  int nb_arguments;                ///< nb d'arguments
  Env_var * decl ;       ///< variables locales au constructeur.

  Constructeur(Nom_liste *, Decl_var_liste *, Instruction_liste *); 

  virtual ~Constructeur() {};

  void exec(Data_liste *);
  /**<
   * - 1. Affecter les variables en arguments
   * - 2. Charger les variables en arguments 
   * - 3. Executer les instructions
   * - 4. Sauver l'environnement courant
   */
};

#endif
