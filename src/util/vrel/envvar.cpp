/****************************************************************************
 *                                                                          *
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *                                                                          *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
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
 *                                                                          *
 ****************************************************************************/

/****** classe Env_var ******/

#include "vrel.hpp"
#include "envvar.hpp"
#include "data.hpp"
#include "number.hpp"
#include "str.hpp"


// Constructeur: alloue la place memoire pour les tableaux.
Env_var::Env_var ()
{
  // Initialisation du tableau de Data.
  donnee = new Data*[256];
  if (donnee == NULL) {
    cout << "can't alloc Data\n";
    exit (1);
  }
  for (int i=0; i < 256; i++) donnee[i] = NULL;
  
  count = 1;  // Attention : le tableau part de 1.
}

// Fonction de parcours.
int Env_var::parcours (const char *nom_var)
{
  int i = 1;
  while ((strcmp(nom[i], nom_var) != 0) && i < 256) i++;
  if (i == 256)
    return 0; // return 0 si la variable n'a pas ete trouvee dans le tableau
  else
    return i; // return son indice
}

// Fonction de declaration d'une variable.
void Env_var::declaration (const char *nom_var)
{
  extern Env_var * varglobal;

  if (varglobal->parcours(nom_var) != 0) {
    printf("envar: error: %s, line %d\n", vrelfile, yylineno);
    printf("\t this variable %s has been declared as global\n", nom_var);
    exit(1);
  }

  if (parcours(nom_var) == 0) {
    strcpy(nom[count], nom_var);
    donnee[count] = NULL;
    count ++;
  }
  else {
    printf("envar: error: %s, line %d\n", vrelfile, yylineno);
    printf("\t this variable %s has been declared twice\n", nom_var);
    exit(1);
  }
}

// Fonction de declaration d'un float.
void Env_var::declaration (const char *nom, float flo1)
{
  declaration(nom);
  Nombre *nb = new Nombre(flo1);
  affectation(nom, nb);
}

// Fonction de declaration d'une chaine de caracteres.
void Env_var::declaration (const char *nom, const char *str)
{
  declaration(nom);
  Chaine *ch = new Chaine(str);
  affectation(nom, ch);
}

// Fonction d'affectation.
int Env_var::affectation (const char *nom_var, Data* valeur)
{
  int ind;

  if ((ind = parcours(nom_var)) != 0)
    donnee[ind] = valeur->get_data();
  return ind;  // On renvoie 0 si l'affectation n'a pas eu lieu
}

// Fonction d'affectation sans evaluation (ie la data peut etre un pointeur 
// sur un objet de type Variable).
int Env_var::affectation2 (const char *nom_var, Data* valeur)
{
  int ind = 0;

  if ((ind = parcours(nom_var)) != 0)
    donnee[ind] = valeur;
  return ind;  // On renvoie 0 si l'affectation n'a pas eu lieu
}

// Fonction qui renvoie la valeur d'une variable.
Data* Env_var::eval (const char *nom_var)
{
  int ind = parcours(nom_var);

  if (ind == 0)
    return NULL;
  else if (donnee[ind] == NULL) {
    printf ("envar: error:\ncan't evaluate this variable %s because not affected\n", nom_var);    
    exit(1);
  }
  return donnee[ind];   
}

// Destructeur.
Env_var::~Env_var()
{
  if (donnee) delete[] donnee;
  count = 0;
}
