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

/****** classe des listes de Data *****/

#include "vrel.hpp"

#include "datalist.hpp"
#include "data.hpp"


// Constructeur.
Data_liste::Data_liste () 
{
  donnee = new Data*[20];
  if (donnee == NULL) {
    cout << "Error: can't alloc Data_list\n";
    exit(1);
  } 
  nb_data = 0;
  return;
}

// Ajout d'une data.
void Data_liste::ajouter (Data* data)
{
  donnee[nb_data] = data;
  nb_data ++;
}

// Destructeur.
Data_liste::~Data_liste ()
{
  delete[] donnee;
}
