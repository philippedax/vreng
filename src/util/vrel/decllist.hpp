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

#ifndef DECLLIST_H
#define DECLLIST_H

#include "declvar.hpp"
#include "envvar.hpp"

/**
 * class Decl_var_liste
 */
class Decl_var_liste : public Declaration
{
 public:

  Decl_var_liste ();		///< Constructeur de la classe
  virtual ~Decl_var_liste ();	///< Destructeur de la classe

  void adddeclarvar (Decl_var *);
  /**< Ajout d'une variable à la liste varlocal */

  Env_var * varloc;
  /**< Pointeur sur la liste des variables locales créées lors des déclarations */
 
};

#endif
