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

/********** opposee d'une data ********/

#include "vrel.hpp"
#include "opp.hpp"
#include "data.hpp"


// Constructeur
Opposition::Opposition (Data* operande)
{
  ligne = yylineno;
  op_erande = operande;
}

// Execution.
Data* Opposition::get_data ()
{
  Data *op = op_erande->get_data();
  Data *resultat = op->oppose();

  if (resultat == NULL) {
    cout << "Erreur a la ligne " << ligne << " : dans la valeur absolue : l'operande n'est pas un nombre.\n";
    exit(1);
  }
  return resultat;
}

// Destructeur.
Opposition::~Opposition ()
{
  delete op_erande;
}
