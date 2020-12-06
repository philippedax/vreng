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

#ifndef ENVOBJECT_H
#define ENVOBJECT_H

#include "new.hpp"

/**
 * class Env_objets
 */
class Env_objets
{
 public:

  int nb_objets;           ///< nombre d'objets dans le tableau des objets
  char nom[255][30];       ///< tableau des noms d'objets
  Creation * objet[255];   ///< tableau des pointeurs sur les objets

  Env_objets();
  virtual ~Env_objets() {};

  void addObjet(char *, Creation *);
  /**< Methode qui ajoute un objet au tableau des objets */

  void affectation(char *, char *, Data *);
  /**< Methode qui affecte une valeur à une variable x de l'objet y */
};

#endif
