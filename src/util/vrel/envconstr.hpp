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

#ifndef ENVCONSTR_H
#define ENVCONSTR_H

#include "constructor.hpp"
#include "datalist.hpp"

/**
 * list of constructors
 */
class Env_constr
{
public:

  int nb_constr;   // nombre de constructeurs dans le tableau des constructeurs
  int nb[50]; // tableau des nombres d'arguments des constructeurs
  Constructeur * constr[50];   // tableau des pointeurs sur les constructeurs

  Env_constr();
  virtual ~Env_constr() {};

  void addConstr(Constructeur *);
  // Méthode qui ajoute un constructeur au tableau des constructeurs

  void exec(Data_liste *);
  /**<
   * - 1. Vérifier l'existence du constructeur
   * - 2. Executer le constructeur
   */
};

#endif
