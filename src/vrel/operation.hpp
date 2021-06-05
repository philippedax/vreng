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

#ifndef OPERATION_HPP
#define OPERATION_HPP

#include "vrel.hpp"
#include "data.hpp"

/**
 * metaclass of operations
 */
class Operation : public Data
{
 public:

  // Numero de ligne fournit par bison et qui sert aux messages d'erreur.
  int ligne;

  // Methode d'execution de l'operation.
  virtual Data* get_data () = 0;

  // Methode d'erreur.
  void erreur (char * texte) {
    printf ("Erreur : ligne %d \n%s \n", ligne, texte);
    exit(1);
  }

  // Retourne le resultat.
  float get_float ()  { return (get_data()->get_float());}

  // Methodes inutiles.
  void set_float (float) { } 
  void set_float (int)   { }
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
  Data* And (Data *)       { return NULL; }
  Data* Or (Data *)        { return NULL; }
  Data* Not ()         { return NULL; }

  // Destructeur.
  virtual ~Operation() { };
  
};

#endif
