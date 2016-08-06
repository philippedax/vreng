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

/***** constructeurs des compound *****/

#include "namelist.hpp"
#include "datalist.hpp"
#include "instlist.hpp"
#include "decllist.hpp"
#include "envvar.hpp"
#include "tabvar.hpp"
#include "tabobj.hpp"
#include "constructor.hpp"


extern Tabvar * tableauvar;
extern Env_var * varlocal;
extern Tabobj * tableauobj;
extern Env_objets * pere;


// Constructeur pour une class ... compound. 
Constructeur::Constructeur(Nom_liste * nom2, Decl_var_liste * decl1, 
			   Instruction_liste * instr1)
{
  int i;

  envobj = pere;
  pere = new Env_objets();  

  // champ nb d'arguments.
  nb_arguments = nom2->nb_nom;
  // champ des instructions.
  instr = instr1;
  // champ des variables locales au constructeur.
  decl = decl1->varloc;
  // arguments du constructeur.
  argument = new Env_var();

  for (i=0; i<nom2->nb_nom; i++) {
    decl->declaration(nom2->nom[i]);
    argument->declaration(nom2->nom[i]);
  }
  declfixe = decl;
}


// Méthode d'exécution du constructeur 
void Constructeur::exec(Data_liste * arg)
{
  int i;

  // On cree un environnement_var image qui pourra etre mofifier pendant l'exec.
  decl = new Env_var ();
  for (i=1 ; i<declfixe->count ; i++) 
    {
      decl->declaration(declfixe->nom[i]);
      if (declfixe->donnee[i] != NULL)
	decl->affectation(declfixe->nom[i], declfixe->donnee[i]);
    }
  for (i=0; i<nb_arguments; i++)
    decl->affectation(argument->nom[i+1], arg->donnee[i]);

  // On change d'environnement.
  tableauobj->sauver(pere);
  pere = envobj;
  tableauvar->sauver(varlocal);
  varlocal = decl;

  // On execute.
  instr->exec();

  // On rétablit l'environnement.
  varlocal = tableauvar->charger();
  pere = tableauobj->charger();
}
