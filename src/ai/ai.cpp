//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
// ai.cpp
//
// AI management
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "ai.hpp"
#include "object.hpp"	// Object
#include "gui.hpp"	// getClicked
#include "world.hpp"	// current()
#include "cache.hpp"	// setCachePath
#include "user.hpp"	// pos
#include "move.hpp"	// goXYZ
#include "render.hpp"	// highlight
#include "vicinity.hpp"	// analScene
#include "xml.hpp"	// selectProximity


/** Initialization */
void initOcaml()
{
  char *args[2];

  args[0] = NULL;
  args[1] = NULL;
#if HAVE_OCAML
  caml_startup(args);
#endif
}

#if HAVE_OCAML

/** Fonctions de transformations de requetes */
value recherche_Typegen(value _type, value _action)
{
  char *type = const_cast<char *> (String_val(_type));
  char *action = const_cast<char *> (String_val(_action));

  uint8_t clicks = 0;
  float clicked[7];

  char *foundpos = new char[100];
  sprintf(foundpos, "N/A");
  int nbobj = 0;
  Object** objects = g.render.getDrawedObjects(&nbobj);
  //echo("Here %s >> %s >> %d", objects[1]->typeName(), type, nbobj);

  int found = 0;
  int foundobj = 0;

  for (int i=1; i < nbobj; i++) {
    echo("Here %d %s", i, objects[i]->typeName());
    if (! strcasecmp(objects[i]->typeName(), type) && objects[i]->isVisible()) {
      //echo("found a type!");
      if (found == 0) {
	foundobj = i;
	found = 1;
      } else {
	found = -1;
	break;
      }
    }
  }
  if (found == 1) {
    echo("Here: execute the action on the object");
    objects[foundobj]->runAction(action);
  }
  if (found == -1 || found == 0) {
    g.gui.getClicked(&clicks, clicked);
    if (clicked[0] != MAXFLOAT && clicks == 0) {
      //echo("analyze your selection on %s", (g.gui.getSelectedObject())->name); 
      g.gui.getSelectedObject()->runAction(action);
      g.gui.initClicked();
      return Val_int(0);
    }
  }
  if (found == -1) {
    echo("there are several elements of this type, please show");
    return Val_int(1);
  } else if (found == 0) {
    echo("don't see anything, please show");
    return Val_int(1);
  }
  // demande designation.
  return Val_int(0);
}

value recherche_Type(value _type)
{
  value ret;
  char *type = const_cast<char *> (String_val(_type));
  uint8_t clicks = 0;
  float clicked[7];

  char *foundpos = new char[100];
  sprintf(foundpos, "N/A");

  int nbobj = 0;
  Object** objects = g.render.getDrawedObjects(&nbobj);

  int found = 0;
  for (int i=0; i < nbobj; i++) {
    if (! strcasecmp(objects[i]->typeName(), type) && objects[i]->isVisible()) {
      if (found) {
	found = 0;
	break;
      } else {
	found = i;
      }
    }
  }
  if (found == 0) {
    //echo("found a type by click");
    g.gui.getClicked(&clicks, clicked);
    if (clicked[0] != MAXFLOAT && clicks == 0) {
      sprintf(foundpos,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
	      clicked[0], clicked[1], clicked[2], clicked[3], clicked[4], clicked[5], clicked[6]);
      g.gui.initClicked();

      // enlever les highlights des objets de meme type
      g.render.highlight(type, false);
    }
    else {
      // mettre en highlight les objets de meme type
      g.render.highlight(type, true);
    }
  }
  else {
    //echo("found a type auto");
    sprintf(foundpos,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
	    objects[found]->pos.x,
	    objects[found]->pos.y,
	    objects[found]->pos.z,
	    objects[found]->pos.az,
	    objects[found]->pos.bbs.v[0],
	    objects[found]->pos.bbs.v[1],
	    objects[found]->pos.bbs.v[2]);
  }
  ret = copy_string(foundpos);
  delete[] foundpos;
  return ret;
}

value recherche_Objet(value mot)
{
  value ret;
  char *foundpos = new char[100];
  sprintf(foundpos, "N/A");
#if HAVE_LIBXML2
  char *val = const_cast<char *> (String_val(mot));
  char filename[64];
  Cache::setCachePath(World::current()->url, filename);
  Xml::selectXpathExpr(filename, "//*/@name", val, foundpos);
#endif

  ret = copy_string(foundpos);
  delete[] foundpos;
  return ret;
}

/** fontions de deplacement de l'utilisateur */
value deplacement_to_Objet(value px, value py, value pz, value ori, value depl)
{
  float posx = static_cast<float> (Double_val(px));
  float posy = static_cast<float> (Double_val(py));
  float posz = static_cast<float> (Double_val(pz));
  float orient = static_cast<float> (Double_val(ori));
  int deplacement = static_cast<int> (Int_val(depl));

  // Deplacement
  if (! deplacement) {
    localuser->pos.x = posx;
    localuser->pos.y = posy;
    localuser->pos.z = posz;
    localuser->pos.az = orient;
  }
  else {
    goXYZ(posx, posy, posz, orient);
  }
  return Val_int(0);
}

value recherche_Func(value mot, value act)
{
  char *val = const_cast<char *> (String_val(mot));
  char *action = const_cast<char *> (String_val(act));
  int *listtype = Vicinity::getTypeFromAction(val);
  int nbtype = listtype[0];

  //echo("I'am here %d %s %s !", nbtype, val, action);

  if (nbtype > 0) {
    char** types = new char*[nbtype];

    for (int k=1 ; k < 1+nbtype ; k++) {
      types[k-1] = Vicinity::translateNum2Type(listtype[k]);
    }

    int nbobj = 0;
    Object **objects = g.render.getVisibleObjects(types, nbtype, &nbobj);

    if (nbobj > 0 && strcasecmp(action, "NA")) {
      //on lance la methode sur les objets visibles! faut-il la lancer sur tout?
      for (int l=0; l<nbobj; l++) {
	if (! strcasecmp(objects[l]->typeName(), action))
	  objects[l]->runAction(val);
      }
    }
    else {
      if (nbobj == 1 && objects[0]) {
        echo("object found, do \"%s\" action !", val);
        objects[0]->runAction(val);
      }
      else if (nbobj > 1) {
        echo("ambiguity, %d objects found, be more precise", nbobj);
      }
      else {
        echo("no visible object for \"%s\", scan around the user", val);
        Vicinity *vicin = new Vicinity();
        int nbprox = 0;
        Object* nearObj = vicin->searchProximityObject(types, nbtype);
        if (nearObj) {
	  nearObj->runAction(val);
	  echo("call the method on the nearest object");	
        }
        else if (nbprox > 1) {
	  echo("test proxi %d", nbprox);
        }
        else {
	  echo("no action \"%s\" possible", val);
        }
      }
    }
    if (types) delete[] types; types = NULL;
    if (objects) free(objects);
  }
  return Val_int(0);
}

value deplacement_to_Proximite(value mot, value pos)
{
  int res = 0;
  float posx = localuser->pos.x;
  float posy = localuser->pos.y;
  float posz = localuser->pos.z;
  float orient = localuser->pos.az;
#if HAVE_LIBXML2
  char *val = const_cast<char *> (String_val(mot));
  char filename[64];
  Cache::setCachePath(World::current()->url, filename);
  res = Xml::selectProximity(filename, val, &posx, &posy, &posz, &orient);
#endif

  if (! res) {
    goXYZ(posx, posy, posz, orient);
  }
  return Val_int(0);
}

/** Fonctions d'affichage */
void msg_info(value mot)
{
  char *val = const_cast<char *> (String_val(mot));
  echo("msginfo::%s", val);
}

void msg_debug(value mot)
{
  char *val = const_cast<char *> (String_val(mot));
  echo("msg_debug::%s", val);
}

void viewed_objects(value mot)
{
  char *val = const_cast<char *> (String_val(mot));
  Vicinity* vicin = new Vicinity(val);
  vicin->analScene();
}

/** Fonctions de lecture des requetes */
int read_request(const char *requete)
{
  if (requete && (requete[0] == '@')) {
    printf("RECUPERATION\n");
  }
  if (! strcmp(requete, "TEST")) {
    printf("TESTONS\n");
    g.render.calculateFov(0,0,0,0, const_cast<char *> ("TEST.jpg"));
    return 0;
  }
  if (! strcmp(requete, "C")) {
    g.render.computeCameraProjection();
    echo("calculate user visualisation");
    return 0;
  }
  if (! strcmp(requete, "V")) {
    Vicinity::show("look at vicinity");
    return 0;
  }
  if (! strcmp(requete, "L")) {
    g.render.showSolidList();
    return 0;
  }

  int temp = (int) Int_val(callback(*caml_named_value("caml_entree_analyse"), copy_string(requete)));
  return temp;
}

#endif //HAVE_OCAML
