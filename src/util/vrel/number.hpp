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

#ifndef NOMBRE_H
#define NOMBRE_H

#include "vrel.hpp"
#include "data.hpp"
#include "test.hpp"
#include "operation.hpp"

/**
 * class Nombre
 */
class Nombre : public Data
{
  float valeur;  // Tout en reel.

 public:

  // Constructeur dans le cas d'un entier.
  Nombre(int value)   { valeur = (float)value; }
  // Constructeur dans le cas d'un reel.
  Nombre(float value) { valeur = value; }

  // Destructeur
  virtual ~Nombre() {}

  // Retourne la valeur.
  float get_float() { return valeur; }

  // Se retourne lui-meme
  Nombre* get_data () { return this; } 

  // Affectation (entier ou reel).
  void set_float (float value) { valeur = value; }
  void set_float (int value)   { valeur = (float)value; }

  // Operations algébriques.
  Nombre* plus (Data *);
  Nombre* moins (Data *);
  Nombre* mult (Data *);
  Nombre* div (Data *);
  Nombre* mod (Data *); // les reels sont convertis en entier
  Nombre* abs ();
  Nombre* oppose ();

  // Comparaisons : renvoient un booleen.
  Test* EgalEgal (Data *);
  Test* Sup (Data *);
  Test* Inf (Data *);
  Test* Supegal (Data *);
  Test* Infegal (Data *);
  Test* Different (Data *);

  // Méthodes inutiles.
  Data* And (Data *) { return NULL; }
  Data* Or (Data *)  { return NULL; }
  Data* Not ()      { return NULL; }

};

#endif
