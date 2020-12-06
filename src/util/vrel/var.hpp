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

#ifndef VAR_H
#define VAR_H

#include "vrel.hpp"
#include "data.hpp"

/**
 * class Variable
 */
class Variable : public Data
{
 public:

  // Nom de la variable
  char nom[30];
  
  // Constructeur.
  Variable () {; }
  Variable (char name[30]) { strcpy(nom, name); }

   // Destructeur
  virtual ~Variable () { /*delete[] nom;*/ } 

  // Retourne la Data associee au nom de variable
  Data * get_data(); 

  // Retourne la valeur de la data associee au nom de variable
  float get_float();

  // Methodes inutiles
  void set_float (float)   { } 
  void set_float (int)     { }
  Data* plus (Data *)      { return NULL; }
  Data* moins (Data *)     { return NULL; }
  Data* mult (Data *)      { return NULL; }
  Data* div (Data *)       { return NULL; }
  Data* mod (Data *)       { return NULL; }
  Data* abs ()             { return NULL; }
  Data* oppose ()          { return NULL; }
  Data* EgalEgal (Data *)  { return NULL; }
  Data* Sup (Data *)       { return NULL; }
  Data* Inf (Data *)       { return NULL; }
  Data* Supegal (Data *)   { return NULL; }
  Data* Infegal (Data *)   { return NULL; }
  Data* Different (Data *) { return NULL; }
  Data* And (Data* ptr)    { return NULL; }
  Data* Or (Data* ptr)     { return NULL; }
  Data* Not ()             { return NULL; }

};

#endif
