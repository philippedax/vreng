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

/********** instruction new **********/

#include "vrel.hpp"

#include "new.hpp"
#include "data.hpp"
#include "datalist.hpp"
#include "var.hpp"
#include "envvar.hpp"
#include "envclass.hpp"

extern Env_classe * mere;  // Tableau des classes.


// Methode : constructeur de la classe New 
// Arguments : nom1 = nom de la classe appelee
//             env1 = valeurs des parametres d'appel 
Creation::Creation(char nom1[30], Data_liste * arg1)
{
  char nomvar[64];

  ligne = yylineno;
  kind = 0;
  strcpy(file, vrelfile);

  // Champ nom.
  strcpy(nom, nom1);

  // Verifie l'existence de la classe.
  int i;
  for (i=0; strcmp(mere->nom[i], nom) != 0; i++) { 
    if (i+1 >= mere->nb_classes) erreur("Classe non déclarée");
  }

  // Champ arg.
  arg = arg1;
  // Champ var.
  var = new Env_var();

  // Copier les var de classes de la classe associee.
  for (int j=1; j < mere->classe[i]->var->count; j++) {
    for (int k=0; k<30 ; k++) {
      nomvar[k] = mere->classe[i]->var->nom[j][k];
    }
    var->declaration(nomvar);
    if (mere->classe[i]->var->donnee[j] != NULL)
      var->affectation(nomvar, mere->classe[i]->var->donnee[j]);
  }
}

// Methode : constructeur de la classe New 
// Arguments : nom1 = nom de l'objet
//             nom2 = nom de la classe appelee
//             env1 = valeurs des parametres d'appel 
Creation::Creation(Variable * var1, char nom2[30], Data_liste * arg1)
{
  char nomvar[64];

  kind = 2; // Cas special.
  ligne = yylineno;
  strcpy(file, vrelfile);

  // Champ nameobj.
  strcpy(nameobj, var1->nom);
  // Champ nom. 
  strcpy(nom,nom2);

  // Verifie l'existence de la classe 
  int i;
  for (i=0; strcmp(mere->nom[i], nom) != 0; i++) { 
    if (i+1>= mere->nb_classes) erreur("Classe non déclarée");
  }

  // Champ arg. 
  arg = arg1;
  // Champ var. 
  var = new Env_var;

  // Copier les var de classes de la classe associee
  for (int j=1; j < mere->classe[i]->var->count; j++) {
    for (int k=0; k<30 ; k++) {
      nomvar[k] = mere->classe[i]->var->nom[j][k];
    }
    var->declaration(nomvar);
    if (mere->classe[i]->var->donnee[j] != NULL)
      affectation(nomvar, mere->classe[i]->var->donnee[j]);
  }
}

// Affecte une valeur a une variable de classe 
void Creation::affectation(char nom[30], Data * flo1)
{
  if (var->affectation2(nom,flo1) == 0) 
    erreur("La variable n'est pas declaree");
}

// Methode d'execution de l'instruction new 
void Creation::exec()
{
  char nomvar[64];
  
  int i = 0;
  while (strcmp (mere->nom[i], nom) != 0) i++;

  // Transmettre les variables de classes
  for (int j=1; j < var->count; j++) {
    strcpy (nomvar, var->nom[j]);
    if (var->donnee[j] != NULL)
      mere->classe[i]->var->affectation(nomvar, var->donnee[j]);
  }

  // Executer la classe
  mere->classe[i]->exec(arg);
}

// Destructeur.
Creation::~Creation ()
{
  if (nom) delete[] nom;
  if (arg) delete arg;
  if (var) delete var;
  if (nameobj) delete[] nameobj;
}
