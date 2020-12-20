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

#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "vrel.hpp"
#include "data.hpp"

/**
 * metaclass of logical comparaisons 
 */
class Comparaison : public Data
{
 public:

  Data * op_gauche,* op_droite;	///< Operandes gauche et droite
  
  int ligne;			///< line number supplied by bison
  
  virtual Data* get_data () = 0; ///< Methode d'execution de la comparaison
    
  void erreur (char * texte) {	///< Methode d'erreur
    printf ("Erreur : ligne %d \n %s \n", ligne, texte);
    exit(1);
  }

  virtual ~Comparaison() {	///< Destructeur
    delete op_gauche;
    delete op_droite;
  }

  // Methodes inutiles.
  void set_float (float) { } 
  void set_float (int)   { }
  float get_float ()     { return 0.0; }
  Data* plus (Data *)    { return NULL; }
  Data* moins (Data *)   { return NULL; }
  Data* mult (Data *)    { return NULL; }
  Data* div (Data *)     { return NULL; }
  Data* mod (Data *)     { return NULL; }
  Data* abs ()           { return NULL; }
  Data* oppose ()        { return NULL; }
  Data* EgalEgal (Data *){ return NULL; }
  Data* Sup (Data *)     { return NULL; }
  Data* Inf (Data *)     { return NULL; }
  Data* Supegal (Data *) { return NULL; }
  Data* Infegal (Data *) { return NULL; }
  Data* Different(Data *){ return NULL; }
  Data* And (Data *)     { return NULL; }
  Data* Or (Data *)      { return NULL; }
  Data* Not ()           { return NULL; }

};

#endif
