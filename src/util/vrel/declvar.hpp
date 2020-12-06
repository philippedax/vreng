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

#ifndef DECLVAR_H
#define DECLVAR_H

#include "decl.hpp"
#include "data.hpp"
#include "var.hpp"

/**
 * class Decl_var
 *
 * Cette classe traite des declarations de variables.
 * Elle herite de Declaration.
 */
class Decl_var : public Declaration
{
 public:
  
  Decl_var (Variable *);  
  // Constructeur dans le cas d'une declaration simple.
  // Il affecte var.
  
  Decl_var (Variable *, Data *);
  // Constructeur dans le cas d'une declaration avec affectation.
  // Il evalue l'expression et affecte var et donnee.

  virtual ~Decl_var ();		///< Destructeur de la classe

  char nomvar[30];			///< Variable a declarer

  Data* donnee;
  // Donnee à affecter a cette variable. Vaut NULL par defaut.

};


#endif
