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

/************ CLASS_EXTENDS **************/

#include "vrel.hpp"
#include "instlist.hpp"
#include "envclass.hpp"
#include "error.hpp"
#include "inst.hpp"
#include "envvar.hpp"
#include "datalist.hpp"
#include "class.hpp"
#include "extends.hpp"


extern Env_var * attribut; // pointeur sur les attributs de la classe courante.
extern Env_classe * mere;  // tableau des classes.


// Declaration d'une class ... extends ... 
Class_extends::Class_extends(char nom1[30], char nom2[30], Instruction_liste * instr1)
{ 
  int i,j;
  char nomvar[30];

  // champ nom de classe 
  mere->addClasse(nom1,this);

  // champs heritage 
  strcpy(heritage, nom2);
  for (i=0; strcmp(mere->nom[i], nom2) != 0; i++) {
    if (i+1>= mere->nb_classes)
      Erreur("Parent Class not declared");
  }

  // champs de definition 
  instr = instr1;

  // champ var 
  var = new Env_var();

  // Déclarer les var de la classe mère dans var
  for (j=1; j < mere->classe[i]->var->count; j++) {
    for (int k=0; k<30 ; k++) {
      nomvar[k] = mere->classe[i]->var->nom[j][k];
    }
    //nomvar = mere->classe[i]->var->nom[j] ;
    var->declaration(nomvar);
    if( mere->classe[i]->var->donnee[j] != 0)
      var->affectation(nomvar, mere->classe[i]->var->donnee[j]) ;
  }
}

// Méthode d'exécution de class_extends 
void Class_extends::exec(Data_liste * env1)
{
  int i,j ;

  attribut = var;
  
  instr->exec();

  i = 0;
  while (strcmp (heritage, mere->nom[i]) != 0) i++;

  // Transmettre les variables de classes
  for (j = 1; j < var->count; j++) {
    mere->classe[i]->var->affectation(var->nom[j],var->donnee[j]);
  }

  // Executer la classe
  mere->classe[i]->exec(env1);

  attribut = new Env_var();
}
