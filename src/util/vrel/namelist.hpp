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

#ifndef NAMELIST_H
#define NAMELIST_H

/**
 * class of lists of Nom
 */
class Nom_liste
{
 public:

  int nb_nom;

  // Tableau de pointeurs sur des char * : la liste est limitee à 20 noms.
  char nom[20][30]; 

  Nom_liste();
  void ajouter (char *); // Ajoute un nom a la liste
  virtual ~Nom_liste();
};

#endif
