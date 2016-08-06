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

/******** classe Env_objets ********/

#include "new.hpp"
#include "error.hpp"
#include "data.hpp"
#include "envobject.hpp"


// Construire un tableau d'objets 
Env_objets::Env_objets()
{
  nb_objets = 0;
}

// Ajouter un objet au tableau des objets 
void Env_objets::addObjet(char nom1[30], Creation * new1)
{
  int i;

  // test si le nom de l'objet n'est pas déjà utilisé 
 for (i=0; i<nb_objets; i++)
   if (strcmp(nom1, nom[i]) == 0) Erreur("Object already declared");

  strcpy(nom[nb_objets],nom1);
  objet[nb_objets]=new1;
  nb_objets++;
  if (nb_objets > 255) Erreur("Number of objects limited to 256");
}

// Méthode qui affecte une valeur à une variable x de l'objet y 
void Env_objets::affectation (char nom1[50], char nom2[50], Data * flo1)
{
  int i=0;
  while (strcmp (nom[i], nom1) != 0)
    {
      if (i+1 >= nb_objets) Erreur("Object not declared");
      i++; 
    }
  objet[i]->affectation (nom2, flo1);
}
