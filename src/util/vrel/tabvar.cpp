/****************************************************************************
 *                                                                          *
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *                                                                          *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
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
 *                                                                          *
 ****************************************************************************/

/*********** classe Tabvar ************/

#include "vrel.hpp"

#include "tabvar.hpp"
#include "envvar.hpp"


// Constructeur de la classe.
Tabvar::Tabvar()
{
  for (int i=0 ; i<20 ; i++)
    tabenv[i] = NULL;

  profondeur = 0;
}

// Destructeur de la classe.
Tabvar::~Tabvar()
{
  delete[] tabenv;
}

// Sauvegarde un environnement sur la pile.
void Tabvar::sauver (Env_var * varlocales)
{
  if (profondeur > 19) {
    printf("Error: classes imbrication too depth\n");
    exit(1);
  }

  tabenv[profondeur] = varlocales;
  profondeur ++;
}

// Retire l'environnement au dessus de la pile.
Env_var * Tabvar::charger ()
{
  Env_var * tabvar;

  profondeur --;
  tabvar = tabenv[profondeur];

  return tabvar;
}
