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

#ifndef FALSE_HPP
#define FALSE_HPP

#include "test.hpp"

/**
 * class Faux
 */
class Faux : public Test
{
 public:

  Faux () { booleen = false ; }	///< Constructeur.
  Test* And (Data *);		///< And: Renvoie Faux
  Data* Or (Data *);		///< Or: Renvoie Vrai si ptr est vrai sinon Faux
  Test* Not ();			///< Not: Renvoie Vrai.
};

#endif
