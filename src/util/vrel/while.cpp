/******************************************************************************
 *                               VREL COMPONENTS                              *
 *                                                                            *
 *                           Copyright (C) 2000                               *
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

/****** classe des boucles While ******/

#include "vrel.hpp"
#include "while.hpp"
#include "test.hpp"
#include "instlist.hpp"


// Constructeur.
Boucle_while::Boucle_while (Data * tantque, Instruction_liste * cons)
{
  ligne = yylineno;
  strcpy (file, vrelfile);
  kind = 0;
  tant_que = tantque;
  alors = cons;
}

// Exécute un while sur les arguments.
void Boucle_while::exec () 
{
  Test * condition;
  bool test;

  condition = (Test *) tant_que->get_data();
  test = condition->booleen;

  if (test) {
    alors->exec (); 
    exec();
  }
}

// Destructeur.
Boucle_while::~Boucle_while() 
{
  delete tant_que;
  delete alors;
}
