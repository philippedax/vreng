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
#include "wall.hpp"
#include "walls.hpp"
#include "ground.hpp"
#include "gate.hpp"
#include "step.hpp"
#include "web.hpp"
#include "mirage.hpp"
#include "button.hpp"
#include "door.hpp"
#include "doc.hpp"
#include "host.hpp"
#include "board.hpp"
#include "cauldron.hpp"
#include "aoi.hpp"
#include "class.hpp"
#include "error.hpp"
#include "envclass.hpp"


// Construire un tableau de classes 
Env_classe::Env_classe()
{
  nb_classes = 0;
  strcpy(nom[nb_classes], "wall");     classe[nb_classes++] = new Wall();
  strcpy(nom[nb_classes], "walls");    classe[nb_classes++] = new Walls();
  strcpy(nom[nb_classes], "ground");   classe[nb_classes++] = new Ground();
  strcpy(nom[nb_classes], "step");     classe[nb_classes++] = new Step();
  strcpy(nom[nb_classes], "gate");     classe[nb_classes++] = new Gate();
  strcpy(nom[nb_classes], "web");      classe[nb_classes++] = new Web();
  strcpy(nom[nb_classes], "mirage");   classe[nb_classes++] = new Mirage();
  strcpy(nom[nb_classes], "button");   classe[nb_classes++] = new Button();
  strcpy(nom[nb_classes], "door");     classe[nb_classes++] = new Door();
  strcpy(nom[nb_classes], "host");     classe[nb_classes++] = new Host();
  strcpy(nom[nb_classes], "board");    classe[nb_classes++] = new Board();
  strcpy(nom[nb_classes], "doc");      classe[nb_classes++] = new Doc();
  strcpy(nom[nb_classes], "cauldron"); classe[nb_classes++] = new Cauldron();
  strcpy(nom[nb_classes], "aoi");      classe[nb_classes++] = new Aoi();
}

// Ajouter une classe au tableau des classes 
void Env_classe::addClasse(char nom_classe[50], Vrel * classe1)
{
  // test si le nom de la classe n'est pas deja utilise 
  for (int i=0; i<nb_classes; i++) {
    if (! strcmp(nom_classe, nom[i]))
      Erreur("Class already declared");
  }

  strcpy(nom[nb_classes], nom_classe);
  classe[nb_classes]=classe1;
  nb_classes++;
  if (nb_classes > MAX_CLASSES)
    Erreur("Number of classes limited to 256");
}
