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

/***** multiplication de variables ****/

#include "vrel.hpp"

#include "mul.hpp"
#include "data.hpp"


// Constructeur.
Multiplication::Multiplication (Data * opgauche, Data * opdroite)
{
  ligne = yylineno;
  op_gauche = opgauche;
  op_droite = opdroite;
}

// Exécution.
Data* Multiplication::get_data ()
{
  Data *opg, *opd;
  Data *resultat;

  opg = op_gauche->get_data();
  opd = op_droite->get_data();
  
  resultat = opg->mult(opd);
  if (resultat == NULL) {
    cout << "Error at line " << ligne << " : args are not digits\n";
    exit(1);
  }
  return resultat;
}

// Destructeur.
Multiplication::~Multiplication ()
{
  delete op_gauche;
  delete op_droite;
}
