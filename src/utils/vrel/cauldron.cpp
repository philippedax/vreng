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

#include "vrel.hpp"
#include "envvar.hpp"
#include "datalist.hpp"
#include "error.hpp"
#include "cauldron.hpp"


// Constructeur de la classe Cauldron 
Cauldron::Cauldron()
{
  var = new Env_var;
  options_app();
}

// Methode exec de la classe Cauldron
//  env1 = liste des arguments passes lors de l'appel de classe 
void Cauldron::exec(Data_liste *env1)
{
  if (env1->nb_data != 6) Erreur("Cauldron needs 6 arguments");
  fprintf(yyout, "<cauldron pos=\"%.2f %.2f %.2f %.2f\"> <solid shape=\"torus\" radius=\"%.2f\" radius2=\"%.2f\"", 
	  env1->donnee[0]->get_float(), 
	  env1->donnee[1]->get_float(), 
	  env1->donnee[2]->get_float(), 
	  env1->donnee[3]->get_float(), 
	  env1->donnee[4]->get_float(), 
	  env1->donnee[5]->get_float());
  ecrire_app();
  fprintf(yyout, " /></cauldron>\n");
}
