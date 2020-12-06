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

#ifndef NEW_H
#define NEW_H

#include "inst.hpp"
#include "datalist.hpp"
#include "data.hpp"
#include "var.hpp"
#include "envvar.hpp"

/**
 * class Creation (new)
 */
class Creation : public Instruction
{
 private:

  char nom[30];              ///< Nom de la classe appelee.
  Data_liste * arg;          ///< Liste des arguments du constructeur.
  Env_var * var;   ///< Liste des variables de classe.

 public:

  char nameobj[30];

  Creation(char *, Data_liste *);
  /**<
   * Constructeur
   * - 1. Verifier l'existence de la classe
   * - 2. Declarer les variables de classes dans var de Creation
   */

  Creation(Variable *, char *, Data_liste *);
  // Constructeur avec affectation
  
  virtual ~Creation();
  // Destructeur

  void affectation(char *, Data *);
  // Affecte une valeur à une variable de classe

  void exec();
  /**<
   * Execution 
   * - 1. Affecter var dans les variables de la classe
   * - 2. Executer la classe
   */

};

#endif
