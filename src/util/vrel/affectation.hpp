/******************************************************************************
 *                               VREL COMPONENTS                              *
 *                                                                            *
 *                           Copyright (C) 2000                               *
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
 *****************************************************************************/

#ifndef AFFECTATION_H
#define AFFECTATION_H

#include "inst.hpp"
#include "var.hpp"
#include "data.hpp"


/**
 * affectation of variables
 */
class Affectation : public Instruction
{
 private:

  Variable * var_affectee;	///< Variable affectee
  
 public:

  Data * valeur_affectee;	///< Valeur d'affectation

  char objet[30], variable[30];  // Nom de l'objet et de sa variable.
  // Ces attributs ne sont utilisees que dans le cas d'une affectation 
  // de variable d'un objet.
 
  Affectation(Variable *, Data *);
  // Constructeur de la classe. Attribue respectivemnt a "var_affectee" et
  // a "valeur_affectee" les adresses de la variable dans laquelle on stocke
  // l'information et la data qui constitue cette information.

  Affectation(char *, char *,  Data *);
  // Idem pour les variables d'objet.
  
  void exec();
  // Exec fait appel a la methode de lecture et d'evaluation de data
  // et a la methode d'affectation d'une variable dans l'environnement.
  
  virtual ~Affectation();	///< Destructeur

};

#endif
