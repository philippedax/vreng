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

/******* classe Decl_var *******/

#include "vrel.hpp"

#include "declvar.hpp"
#include "data.hpp"
#include "var.hpp"


// Constructeur dans le cas d'une declaration simple.
Decl_var::Decl_var (Variable* variable)
{
  strcpy(nomvar, variable->nom);
  donnee = NULL;
}

// Constructeur dans le cas d'une declaration avec affectation.
Decl_var::Decl_var (Variable* variable, Data* expression)
{
  strcpy(nomvar, variable->nom);
  donnee = expression;
}

// Destructeur
Decl_var::~Decl_var ()
{
  delete donnee;
  delete[] nomvar; 
}
