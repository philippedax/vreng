/******************************************************************************
 *                              VREL COMPONENTS                               *
 *                                                                            *
 *                            Copyright (C) 2000                              *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin      *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 ******************************************************************************/

#ifndef LOGICAL_H
#define LOGICAL_H

#include "vrel.hpp"
#include "data.hpp"

/**
 * metaclass of logical operations
 */
class Oplogique : public Data
{
 public:

  // Numéro de ligne fournit à bison et qui sert aux messages d'erreur.
  int ligne;

  // Méthode d'exécution de l'operation logique.
  virtual Data* get_data () = 0;
  
  // Méthode d'erreur.
  void erreur (const char * texte) {
    printf ("Error: line %d: %s\n", ligne, texte);
    exit(1);
  }

  // Méthodes inutiles.
  void set_float (float)  { } 
  void set_float (int)    { }
  float get_float ()  {return 0.0;}
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
  Data* And (Data* ptr)   { return NULL; }
  Data* Or (Data* ptr)    { return NULL; }
  Data* Not ()        { return NULL; }

  // Destructeur.
  virtual ~Oplogique() { };
  
};

#endif
