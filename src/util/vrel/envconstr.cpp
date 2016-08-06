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

/*****   listes de constructeurs  *****/

#include "constructor.hpp"
#include "datalist.hpp"
#include "error.hpp"
#include "envconstr.hpp"


// Construire un tableau de constructeurs 
Env_constr::Env_constr(void)
{
  nb_constr = 0;
}

// Ajouter un constructeur au tableau de constructeurs 
void Env_constr::addConstr(Constructeur * constr1)
{
  int i;
  int nb_arg = constr1->nb_arguments ;

  // test si le constructeur n'est pas déjà utilisé 
  for (i=0; i < nb_constr; i++) 
    if (nb_arg == nb[i]) Erreur("Constructor already declared");

  nb[nb_constr] = nb_arg;
  constr[nb_constr] = constr1;
  nb_constr++;
  if (nb_constr > 255) Erreur("Number of constructeurs limited to 256");
}

// Méthode d'exécution 
void Env_constr::exec(Data_liste * arg)
{
  int i = 0;
  while (nb[i] != arg->nb_data)
    {
      if (i+1>= nb_constr) Erreur("Constructor not declared");
      i++; 
    }
  constr[i]->exec(arg);
}
