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
#include "error.hpp"
#include "datalist.hpp"
#include "earth.hpp"


// Constructeur de la classe Earth 
Earth::Earth()
{
  var = new Environnement_var;
  var->declaration("tex","0");
  options_app();
}

// Méthode exec de la classe Earth
//  env1 = liste des arguments passés lors de l'appel de classe 
void Earth::exec(Data_liste *env1)
{
  if (env1->nb_data != 4) Erreur("Earth needs 4 arguments");
  fprintf(yyout, "<mirage pos=\"%.2f %.2f %.2f\" mode=\"earth\"> <solid shape=\"sphere\" r=\"%.2f\"", 
	  env1->donnee[0]->get_float(), 
	  env1->donnee[1]->get_float(),
	  env1->donnee[2]->get_float(),
	  env1->donnee[3]->get_float());
  if (var->eval("tex")->nom != "0") 
    fprintf(yyout, " tx=\"%s\"",var->eval("tex")->nom);
  ecrire_app();
  fprintf(yyout, " /></mirage>\n");
}
