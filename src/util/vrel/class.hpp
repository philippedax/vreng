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

#ifndef CLASS_HPP
#define CLASS_HPP

#include "envvar.hpp"
#include "datalist.hpp"
#include "error.hpp"

/**
 * class of classes
 */
class Vrel 
{
 public:

  Env_var * var;	///< Liste des variables de classe

  virtual void exec(Data_liste *) 
    { Erreur("Erreur Interne - Classe Virtuelle"); };

  virtual ~Vrel() {};
};

#endif
