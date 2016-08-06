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
#include "error.hpp"
#include "app.hpp"


// Méthode d'initialisation de la classe App 
void App::options_app()
{
  var->declaration("diffRed",0.0);
  var->declaration("diffGreen",0.0);
  var->declaration("diffBlue",0.0);
  // Diffuse (à remplacer par un tableau de 3 valeurs si possible)
  var->declaration("ambRed",0.0);
  var->declaration("ambGreen",0.0);
  var->declaration("ambBlue",0.0);
  // ambiente (à remplacer par un tableau de 3 valeurs si possible)
  var->declaration("shinPar1",0.0);
  var->declaration("shinPar2",0.0);
  var->declaration("shinPar3",0.0);
  // shininess (à remplacer par un tableau de 3 valeurs si possible)
  var->declaration("specPar1",0.0);
  var->declaration("specPar2",0.0); 
  var->declaration("specPar3",0.0);
  // specular (à remplacer par un tableau de 3 valeurs si possible)
}

// Méthode d'écriture des paramètres optionnels 
void App::ecrire_app()
{
  int test = 0 ;

  // Diffuse
  if ((var->eval("diffRed")->get_float() != 0.0)
      ||(var->eval("diffGreen")->get_float() != 0.0)
      ||(var->eval("diffBlue")->get_float() != 0.0)) {
    fprintf(yyout," dif=\"%.2f %.2f %.2f\"",
	    var->eval("diffRed")->get_float(),
	    var->eval("diffGreen")->get_float(),
	    var->eval("diffBlue")->get_float());
    test ++ ;
  }
  
  // Ambient
  if ((var->eval("ambRed")->get_float() != 0.0)
      ||(var->eval("ambGreen")->get_float() != 0.0)
      ||(var->eval("ambBlue")->get_float() != 0.0)) {
    fprintf(yyout," amb=\"%.2f %.2f %.2f\"",
	    var->eval("ambRed")->get_float(),
	    var->eval("ambGreen")->get_float(),
	    var->eval("ambBlue")->get_float());
    test ++ ;
  }

  if (test == 2)
    Erreur("we can't use \"diffuse\" and \"ambient\" together");

  test = 0 ;

  // Shininess
  if ((var->eval("shinPar1")->get_float() != 0.0)
      ||(var->eval("shinPar2")->get_float() != 0.0)
      ||(var->eval("shinPar3")->get_float() != 0.0)) {
    fprintf(yyout," shi=\"%.2f %.2f %.2f\"",
	    var->eval("shinPar1")->get_float(),
	    var->eval("shinPar2")->get_float(),
	    var->eval("shinPar3")->get_float());
    test ++ ;
  }

  // Specular
  if ((var->eval("specPar1")->get_float() != 0.0)
      ||(var->eval("specPar2")->get_float() != 0.0)
      ||(var->eval("specPar3")->get_float() != 0.0)) {
    fprintf(yyout," spe=\"%.2f %.2f %.2f\"",
	    var->eval("specPar1")->get_float(),
	    var->eval("specPar2")->get_float(),
	    var->eval("specPar3")->get_float());
    test ++ ;
  }

  // if (test == 2)
  //   Erreur("we can't use \"shininess\" and \"specular\" together");
}
