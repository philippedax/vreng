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

/************* classe Faux ************/

#include "test.hpp"
#include "false.hpp"
#include "true.hpp"


// And : Renvoie Faux
Test* Faux::And (Data* ptr) 
{
  return this;
}

// Or : Renvoie Vrai si ptr est vrai et Faux si ptr est Faux.
Data* Faux::Or (Data* ptr)
{
  return ptr;
}

// Not : Renvoie Vrai.
Test* Faux::Not()
{
  return new Vrai();
}
