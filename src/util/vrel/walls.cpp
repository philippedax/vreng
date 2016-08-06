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
#include "datalist.hpp"
#include "error.hpp"
#include "walls.hpp"


// Méthode exec de la classe Walls
//  env1 = liste des arguments passés lors de l'appel de classe 
void Walls::exec(Data_liste * env1)
{
  if (env1->nb_data != 1) Erreur("Walls needs 1 argument");
  fprintf(yyout, "<walls url=\"%s\"></walls>", env1->donnee[0]->nom);
  fprintf(yyout, "\n");
}
