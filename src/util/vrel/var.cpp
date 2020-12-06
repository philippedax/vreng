/****************************************************************************
 *                                                                          *
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *                                                                          *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
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
 *                                                                          *
 ****************************************************************************/

/*********** classe variables *********/

#include "vrel.hpp"
#include "var.hpp"
#include "envvar.hpp"
#include "data.hpp"

extern Env_var *varglobal, *varlocal, *attribut;


// Retourne la data correspondante.
Data* Variable::get_data ()
{
  Data *expression; 

  if ((expression = varlocal->eval(nom)) == NULL)
    if ((expression = attribut->eval(nom)) == NULL)
      expression = varglobal->eval(nom);

  if (expression == NULL) {
    printf ("Error: variable %s has not been declared\n", nom);
    exit(1);
  }
  return expression;
}

// Retourne la valeur de la data associee au nom de variable.
float Variable::get_float()
{
  Data *expression = this->get_data();

  return expression->get_float();
}
