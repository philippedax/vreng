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

/****** classe des listes de Noms *****/

#include "vrel.hpp"
#include "namelist.hpp"


// Constructeur.
Nom_liste::Nom_liste () 
{
  nb_nom = 0;
}

// Ajout d'un nom.
void Nom_liste::ajouter (char *name)
{
  strcpy(nom[nb_nom], name);
  nb_nom ++;
}

// Destructeur.
Nom_liste::~Nom_liste ()
{
}
