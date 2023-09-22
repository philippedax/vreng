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
#include "str.hpp"
#include "envvar.hpp"
#include "box.hpp"
#include "data.hpp"


// Methode d'initialisation de la classe Box 
void Box::options_box()
{
  var = new Env_var;
  // Textures
  var->declaration("tex_xp", "");
  var->declaration("tex_xn", ""); 
  var->declaration("tex_yp", "");
  var->declaration("tex_yn", ""); 
  var->declaration("tex_zp", "");
  var->declaration("tex_zn", "");

  options_app();
} 

// Methode d'ecriture des parametres optionnels 
void Box::ecrire_box()
{
  // Textures
  if (strcmp(var->eval("tex_xp")->nom, ""))
    fprintf(yyout, " xp=\"%s\"", var->eval("tex_xp")->nom);
  if (strcmp(var->eval("tex_xn")->nom, "")) 
    fprintf(yyout, " xn=\"%s\"", var->eval("tex_xn")->nom);
  if (strcmp(var->eval("tex_yp")->nom, "")) 
    fprintf(yyout, " yp=\"%s\"", var->eval("tex_yp")->nom);
  if (strcmp(var->eval("tex_yn")->nom, "")) 
    fprintf(yyout, " yn=\"%s\"", var->eval("tex_yn")->nom);
  if (strcmp(var->eval("tex_zp")->nom, "")) 
    fprintf(yyout, " zp=\"%s\"", var->eval("tex_zp")->nom);
  if (strcmp(var->eval("tex_zn")->nom, "")) 
    fprintf(yyout, " zn=\"%s\"", var->eval("tex_zn")->nom);

  ecrire_app();
}
