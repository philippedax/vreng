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

#ifndef PROGRAM_H
#define PROGRAM_H

#include "instlist.hpp"
#include "declvar.hpp"
#include "decllist.hpp"
#include "envvar.hpp"

extern Env_var* varglobal ;
// Pointeur sur la liste des variables globales.

extern Env_var* varlocal ;
// Pointeur sur la liste des variables locales.

/**
 * class Programme
 */
class Programme
{
 private:
  
  Instruction_liste * instruc_main ; 
  // Pointeur sur la liste des instructions du main.

  char fichierimport[100][100] ;
  // Tableau des fichiers importes.

  int nbfichiers ;
  // Nombre de fichiers importes.

 public:

  Programme() ;   // Constructeur de la classe.
  virtual ~Programme() ; // Destructeur de la classe.
  
  void import (char *) ; 
  // Passage a un autre fichier.  L'opération inverse se fait dans la fonction
  // yywrap définie dans le fichier vrel.flex.
  
  void adddeclarvar (Decl_var *) ;  
  // Ajout d'une variable à la liste des variables globales.

  void addmain (Decl_var_liste *, Instruction_liste *) ; 
  // Ajout du main.
  
  void exec () ;   
  // Execution du programme : appel récursif de la méthode "exec()" de tous 
  // les objets du main.

};

#endif
