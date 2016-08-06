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
 *****************************************************************************/

/** classe des listes d'instructions **/

#include "vrel.hpp"
#include "instlist.hpp"
#include "inst.hpp"
#include "affectation.hpp"
#include "new.hpp"
#include "envobject.hpp"

extern Env_objets * pere;


// Constructeur de Instruction_liste.
Instruction_liste::Instruction_liste ()
{
  actuelle = NULL;
  suivante = NULL;
}

// Ajout d'une instruction à la liste.
void Instruction_liste::addInstruction (Instruction * a_ajouter)
{
  ligne = yylineno;
  
  if (actuelle == NULL) {

    // Si l'instruction est une affectation sur une variable d'un objet :
    if (a_ajouter->kind == 1) {
      int i = 0;
      char objet[30], variable[30];
      Affectation * ajout;
 
      ajout = (Affectation *) a_ajouter;
      strcpy(objet, ajout->objet);
      strcpy(variable, ajout->variable);
      
      if (pere->nb_objets == 0)
	erreur("L'objet n'existe pas");

      while (strcmp(pere->nom[i], objet) != 0 ) {
	i++;
	if (i > pere->nb_objets)
	  erreur("L'objet n'existe pas");
      }
      pere->objet[i]->affectation(variable, ajout->valeur_affectee);
    }

    // Si l'instruction est une création avec affectation :
    if (a_ajouter->kind == 2) {
      Creation * ajout;
      ajout = (Creation *) a_ajouter;
      pere->addObjet(ajout->nameobj, ajout);
    }
    
    // Dans tous les cas :
    actuelle = a_ajouter;
    suivante = new Instruction_liste();
  }
  
  else
    suivante->addInstruction(a_ajouter);
}

// Propagation de l'exécution
void Instruction_liste::exec ()
{
  if (actuelle != NULL) {
    actuelle->exec();
    suivante->exec();
  }
}

// Destructeur.
Instruction_liste::~Instruction_liste ()
{
  if (actuelle) delete actuelle;
  if (suivante) delete suivante;
}
