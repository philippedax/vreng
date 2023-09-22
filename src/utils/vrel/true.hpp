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

#ifndef TRUE_HPP
#define TRUE_HPP

#include "test.hpp"

/**
 * class Vrai
 */
class Vrai : public Test
{
 public:

  Vrai () { booleen = true; }	///< Constructeur

  Data* And (Data *);	///< And: Renvoie Vrai si ptr est vrai sinon Faux
  Test* Or (Data *);	///< Or: Renvoie Vrai
  Test* Not ();		///< Not: Renvoie Faux

};

#endif
