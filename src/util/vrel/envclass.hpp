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

#ifndef ENVCLASS_H
#define ENVCLASS_H

#include "class.hpp"

#define MAX_CLASSES	255
#define CLASS_LEN	30

/**
 * class Env_classe
 */
class Env_classe
{
 public:

  int nb_classes;            	///< number of classes in the classes array
  char nom[MAX_CLASSES][CLASS_LEN];	///< tableau des noms de classes
  Vrel *classe[MAX_CLASSES];	///< tableau des pointeurs sur les classes

  Env_classe();
  virtual ~Env_classe() {};

  void addClasse(char *, Vrel *);
  /**< Methode qui ajoute une classe au tableau des classes */
};

#endif
