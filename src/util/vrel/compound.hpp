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

#ifndef COMPOUND_H
#define COMPOUND_H

#include "decllist.hpp"
#include "class.hpp"
#include "datalist.hpp"
#include "envconstr.hpp"
#include "envvar.hpp"
#include "constructor.hpp"

/**
 * class Compound 
 */
class Compound : public Vrel
{
 private:

  Env_constr * constr;	///< list of constructors d'une compound

 public:

  Compound(char *, Decl_var_liste *, Env_constr *);
  /**< Constructeur d'une Compound */

  virtual ~Compound() {};

  void exec(Data_liste *);
  /**<
   * - 1. Sauver les variables de classe (et des superclasses ?)
   * - 2. Lancer l'exécution du constructeur
   * - 3. Charger l'environnement courant
   */
};

#endif
