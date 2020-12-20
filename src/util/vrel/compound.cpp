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

#include "vrel.hpp"
#include "envvar.hpp"
#include "datalist.hpp"
#include "decllist.hpp"
#include "envclass.hpp"
#include "class.hpp"
#include "compound.hpp"
#include "constructor.hpp"
#include "envconstr.hpp"


extern Env_var * attribut; // pointeur sur les attributs de la classe courante.
extern Env_classe * mere;  // tableau des classes


// Declaration d'une compound ... 
Compound::Compound(char nom1[30], Decl_var_liste * env, Env_constr * construc)
{ 
  Env_var * envvar;

  // champ nom de classe 
  mere->addClasse(nom1,this);
  // champ attribut
  var = env->varloc;
  // champs de definition 
  constr = construc;

  // On verifie qu'aucune variable locale de constructeur n'a le meme nom qu'un attribut.
  for (int i=0; i<construc->nb_constr; i++) {
    envvar = construc->constr[i]->decl;

    for (int j=1; j<envvar->count; j++) {
      int result = var->parcours(envvar->nom[j]);
      if (result != 0) {
        printf("Error: line %d\n", yylineno);
        printf("compound: variable %s has already been declared as attribute of class %s\n", 
  	      envvar->nom[j], nom1);
        //dax exit(1);
      }
    }
  }
}

// Methode d'execution de compound 
void Compound::exec(Data_liste * env1)
{
  attribut = var;
  constr->exec(env1);
  attribut = new Env_var();
}
