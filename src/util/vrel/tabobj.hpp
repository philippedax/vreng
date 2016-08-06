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

#ifndef TABOBJ_H
#define TABOBJ_H

#include "envobject.hpp"

/**
 * class Tabobj
 */
class Tabobj
{
 private:

  int profondeur;

  Env_objets * envobj [20];
  // Tableau de stockage des environnements d'objets.

 public:

  Tabobj(); // Constructeur de la classe.
  virtual ~Tabobj(); // Destructeur de la classe.

  void sauver (Env_objets *);
  // Sauvegarde un environnement sur la pile.

  Env_objets * charger ();
  // Retire l'environnement au dessus de la pile.

};

#endif
