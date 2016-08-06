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

/********** Negation logique **********/

#include "vrel.hpp"

#include "not.hpp"
#include "data.hpp"


// Constructeur
Not::Not (Data * operande)
{
  ligne = yylineno;
  op_erande = operande;
}

// Exécution.
Data* Not::get_data ()
{
  Data *op;
  Data *resultat;

  op = op_erande->get_data();
  
  resultat = op->Not();
  if (resultat == NULL) {
    cout << "Error at ligne " << ligne << " : not a  boolean\n";
    exit(1);
  }
  return resultat;
}

// Destructeur
Not::~Not ()
{
  delete op_erande;
}
