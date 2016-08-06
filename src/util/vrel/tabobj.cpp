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

/*********** classe Tabobj ************/

#include "vrel.hpp"

#include "tabobj.hpp"
#include "envobject.hpp"


// Constructeur de la classe.
Tabobj::Tabobj()
{
  for (int i=0 ; i<20 ; i++)
    envobj[i] = NULL;
  
  profondeur = 0;
}

// Destructeur de la classe.
Tabobj::~Tabobj()
{
  delete[] envobj;
}

// Sauvegarde un environnement sur la pile.
void Tabobj::sauver (Env_objets * objlocaux)
{
  if (profondeur > 19) {
    printf ("Error:\nclasses imbrication too depth\n");
    exit(1);
  }

  envobj[profondeur] = objlocaux;
  profondeur ++;
}

// Retire l'environnement au dessus de la pile.
Env_objets * Tabobj::charger ()
{
  Env_objets * tabobj;

  profondeur --;
  tabobj = envobj[profondeur];

  return tabobj;
}
