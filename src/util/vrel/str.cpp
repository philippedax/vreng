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

/*** classe des chaines de caractere ***/

#include "vrel.hpp"
#include "str.hpp"


// Constructeur
Chaine::Chaine(const char * name)
{
printf("chaine: name=%s\n", name);
  strcpy(file, vrelfile);
  strcpy(nom, name);
  ligne = yylineno;
}

// Destructeur
Chaine::~Chaine()
{
}

// Cette fonction renvoie systematiquement une erreur.
float Chaine::get_float() 
{
  printf("Error: file %s, line %d\nError of type: string %s is not a digit\n", file, ligne, nom);
  exit(1);
}
