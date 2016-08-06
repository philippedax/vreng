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

#ifndef DATALISTE_H
#define DATALISTE_H

#include "data.hpp"

/**
 * class of lists of Data
 */
class Data_liste
{
 public:

  int nb_data;

  Data** donnee;		///< Tableau de poiteurs sur des data

  Data_liste ();		///< Constructeur
  void ajouter (Data *);	///< Ajoute une data a la liste
  virtual ~Data_liste ();	///< Destructeur
};

#endif
