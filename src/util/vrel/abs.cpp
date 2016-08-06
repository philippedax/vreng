/******************************************************************************
 *                                                                            *
 *                               VREL COMPONENTS                              *
 *                                                                            *
 *                           Copyright (C) 2000                               *
 *                                                                            *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin      *
 *                                                                            *
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
 *                                                                            *
 *****************************************************************************/

/******** valeur absolue de datas *******/

#include "vrel.hpp"

#include "abs.hpp"
#include "data.hpp"


// Constructeur
Valabsolue::Valabsolue (Data * operande)
{
  ligne = yylineno;
  op_erande = operande;
}

// Exécution.
Data* Valabsolue::get_data ()
{
  Data *op;
  Data *resultat;

  op = op_erande->get_data();
  
  resultat = op->abs();
  if (resultat == NULL) {
    cout << "Erreur  à la ligne " << ligne << " : dans la valeur absolue : l'opérande n'est pas un nombre. \n";
    exit(1);
  }
  return resultat;
}

// Destructeur
Valabsolue::~Valabsolue()
{
  delete op_erande;
}
