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

/********** classe Programme ***********/

#include "vrel.hpp"

#include "program.hpp"
#include "instlist.hpp"
#include "declvar.hpp"
#include "decllist.hpp"
#include "envvar.hpp"
#include "envobject.hpp"


extern Env_objets * pere;


// Constructeur de la classe.
Programme::Programme () 
{
  instruc_main = NULL;
  strcpy(fichierimport[0], vrelfile);
  nbfichiers = 1;
}

// Destructeur de la classe.
Programme::~Programme ()
{
  delete instruc_main;
  delete[] fichierimport;
}

// Passage a un autre fichier. 
void Programme::import (char *file)
{
  extern FILE * yyin;
  extern FILE * tabyyin[];

  int test = 0;
  int i = 0;

  // On verifie que le fichier n'a pas déjà été parcouru.
  while ( (i<nbfichiers) && (test == 0) ) {
    if (strcmp(file , fichierimport[i]) == 0)
      test = 1;
    i++;
  }

  // Si le fichier n'a pas déjà été parcouru :
  if (test == 0) {
	
    // Mémorisation de la position.
    tabyyin[comptfichier]       = yyin;
    tablinenumber[comptfichier] = yylineno;
    strcpy (tabvrelfile[comptfichier], vrelfile);
    comptfichier ++;
    
    // Ouverture du nouveau fichier.
    yyin = fopen(file, "r");
    if (yyin  == NULL) {
      perror(file); 
      exit(1); 
    }
    strcpy(fichierimport[nbfichiers], file);
    nbfichiers++;

    if (nbfichiers >= 100) {
      printf("Error: too much imported files\n");
      exit(1);
    }
    
    // Réinitialisation.
    strcpy(vrelfile, file);
    yylineno = 1;
  }
}

// Ajout d'une variable à la liste des variables globales.
void Programme::adddeclarvar (Decl_var* declarvar)
{
  varglobal->declaration (declarvar->nomvar);
  if(declarvar->donnee != NULL)
    varglobal->affectation (declarvar->nomvar, declarvar->donnee);
}

//  Ajout de la liste des instructions du main.
void Programme::addmain ( Decl_var_liste * varlocmain,
			  Instruction_liste * instrucmain )
{
  int line;
  
  line = yylineno;

  if (instruc_main == NULL) {
    varlocal = varlocmain->varloc;
    instruc_main = instrucmain;
  }
  else {
    printf ("Error: %s, line %d\n2 main in this file\n", vrelfile, line);
    exit(1);
  }
}

// Execution du programme
void Programme::exec ()
{
  if (instruc_main == NULL) {
    printf("Error: no main found in this file\n");
    exit(1);
  }
  
  instruc_main->exec();
}
