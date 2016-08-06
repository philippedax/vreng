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

/****** affectations de variables *****/

#include "vrel.hpp"
#include "affectation.hpp"
#include "envvar.hpp"
#include "envobject.hpp"
#include "var.hpp"
#include "data.hpp"


// Constructeur.
Affectation::Affectation (Variable * var_affect, Data * valeur_affect)
{
  ligne = yylineno;
  kind = 0;
  strcpy (file, vrelfile);

  var_affectee = var_affect;
  valeur_affectee = valeur_affect;
}

// Constructeur dans le cas d'une variable d'objet.
Affectation::Affectation (char objet1[30], char variable1[30],
			  Data * valeur_affect)
{
  ligne = yylineno;
  kind = 1;
  strcpy (file, vrelfile);

  strcpy(objet, objet1);
  strcpy(variable, variable1);
  valeur_affectee = valeur_affect;
}

// Execution.
void Affectation::exec ()
{
  extern Env_var *varlocal, *varglobal, *attribut;
  int resultat = 1;
  char nomvar[30];

  // Cas d'une variable normale 
  // (pour les variables d'objet le traitement est différent)
  if (kind == 0) {
    strcpy(nomvar, var_affectee->nom);  

    if ( (varlocal->affectation(nomvar, valeur_affectee)) == 0 )
      if ( (attribut->affectation(nomvar, valeur_affectee)) == 0 )
	resultat = varglobal->affectation(nomvar , valeur_affectee);  

    if (resultat == 0)
      erreur("Impossible de faire l'affectation, la variable n'existe pas");
  }
}

// Destructeur.
Affectation::~Affectation ()
{
  if (objet) delete objet;
  if (variable) delete variable;
  if (valeur_affectee) delete valeur_affectee;
  if (var_affectee) delete var_affectee;
}
