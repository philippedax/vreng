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

/******* classe des Alternatives ******/

#include "vrel.hpp"
#include "alternative.hpp"
#include "instlist.hpp"
#include "test.hpp"


// Générateur de if... then... else
Alternative::Alternative (Data * condition, Instruction_liste * cons1, 
			       Instruction_liste * cons2)
{
  kind = 0;
  ligne = yylineno;
  strcpy (file, vrelfile);

  si = condition;
  alors = cons1;
  sinon = cons2;
}

// Générateur de if... then...
Alternative::Alternative (Data * condition, Instruction_liste * cons)
{
  kind = 0;
  ligne = yylineno;
  strcpy (file, vrelfile);

  si = condition;
  alors = cons;
  sinon = NULL;
}

// Exécute le if... then... else
void Alternative::exec ()
{
  bool test;
  Test * cond;

  cond = (Test *) si->get_data();
  test = cond->booleen;

  if(test)
    alors->exec();
  else
    if (sinon != NULL)
      sinon->exec();
}

// Destruteur.
Alternative::~Alternative ()
{
  delete si;
  delete alors;
  delete sinon;
}
