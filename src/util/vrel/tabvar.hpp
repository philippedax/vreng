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

#ifndef TABVAR_H
#define TABVAR_H

#include "envvar.hpp"

/**
 * class Tabvar
 */
class Tabvar
{
 private:
  
  int profondeur;

  Env_var * tabenv [20];
  // Tableau de stockage des environnements de variables.

 public:
  
  Tabvar(); // Constructeur de la classe.
  virtual ~Tabvar(); // Destructeur de la classe.
  
  void sauver (Env_var *);
  // Sauvegarde un environnement sur la pile.
  
  Env_var * charger ();
  // Retire l'environnement au dessus de la pile.
  
};

#endif
