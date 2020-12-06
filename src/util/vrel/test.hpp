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

#ifndef TEST_H
#define TEST_H

#include "vrel.hpp"
#include "data.hpp"

/**
 * class of tests
 */
class Test : public Data
{
 public:

  virtual Data* And (Data *) = 0;
  virtual Data* Or (Data *) = 0;
  virtual Data* Not () = 0;

  // Se retourne lui-meme
  Test* get_data () { return this; } 

  // Methodes inutiles.
  float get_float()   { return 0; }
  void set_float (float)  { } 
  void set_float (int)    { }
  Data* plus (Data *)     { return NULL; }
  Data* moins (Data *)    { return NULL; }
  Data* mult (Data *)     { return NULL; }
  Data* div (Data *)      { return NULL; }
  Data* mod (Data *)      { return NULL; }
  Data* abs ()            { return NULL; }
  Data* oppose ()         { return NULL; }
  Data* EgalEgal (Data *) { return NULL; }
  Data* Sup (Data *)      { return NULL; }
  Data* Inf (Data *)      { return NULL; }
  Data* Supegal (Data *)  { return NULL; }
  Data* Infegal (Data *)  { return NULL; }
  Data* Different (Data *){ return NULL; }
  
};

#endif
