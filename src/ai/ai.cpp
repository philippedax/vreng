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
#include "wobject.hpp"
#include "gui.hpp"	// getClicked
#include "world.hpp"	// current()
#include "cache.hpp"	// setCachePath
#include "user.hpp"	// pos
#include "move.hpp"	// gotoXYZ
#include "render.hpp"	// highlight
#include "vicinity.hpp"	// analScene
#include "xml.hpp"	// selectProximity


/** Initialization */
void initOcaml()
{
  char *Argv[2];

  Argv[0] = "";
  Argv[1] = NULL;
#if HAVE_OCAML
  caml_startup(Argv);
#endif
}

#if HAVE_OCAML

/** fonctions de transformations de requetes */
value recherche_Typegen(value ttype, value actiondemande)
{
  char *typechercher = const_cast<char *>(String_val(ttype));
  char *action = const_cast<char *>(String_val(actiondemande));

  uint8_t oclick = 0;
  float oclicked[7];

  char *foundpos = new char[100];
  sprintf(foundpos, "N/A");
  //echo("Here");
  int nbelem = 0;
  WO** listObj = g.render.getDrawedObjects(&nbelem);
  //echo("Here %s >> %s >> %d", listObj[1]->typeName(), typechercher, nbelem);

  int found = 0;
  int foundelem = 0;

  for (int i=1; i < nbelem; i++) {
    echo("Here %d %s", i, listObj[i]->typeName());

    if (! strcasecmp(listObj[i]->typeName(), typechercher) && listObj[i]->isVisible()) {
      //echo("found a type !");
      if (found == 0) {
	foundelem = i;
	found = 1;
      } else {
	found = -1;
	break;
      }
    }
  }

  if (found == 1) {
    echo("Here: I execute the action on the object");
    listObj[foundelem]->runAction(action);
  }
  if (found == -1 || found == 0) {
    g.gui.getClicked(&oclick, oclicked);
    if (oclicked[0] != MAXFLOAT && oclick == 0) {
      //echo("I analyze your selection on %s", (g.gui.getSelectedObject())->name); 
      g.gui.getSelectedObject()->runAction(action);
      g.gui.initClicked();
      return Val_int(0);
    }
  }
  if (found == -1) {
    echo("there are several elements of this type, please show");
    return Val_int(1);
  } else if (found == 0) {
    echo("I don't see anything, please show");
    return Val_int(1);
  }

  //XXX demande designation.
  return Val_int(0);
}

value recherche_Type(value ttype)
{
  value ret;
  char *typechercher = const_cast<char *>(String_val(ttype));
  uint8_t oclick = 0;
  float oclicked[7];

  char *foundpos = new char[100];
  sprintf(foundpos, "N/A");

  int nbelem = 0;
  WO** listObj = g.render.getDrawedObjects(&nbelem);

  int found = 0;
  for (int i=0; i < nbelem; i++) {
    if (! strcasecmp(listObj[i]->typeName(), typechercher) && listObj[i]->isVisible()) {
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
    g.gui.getClicked(&oclick, oclicked);
    if (oclicked[0] != MAXFLOAT && oclick == 0) {
      sprintf(foundpos,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
	      oclicked[0],
	      oclicked[1],
	      oclicked[2],
	      oclicked[3],
	      oclicked[4],
	      oclicked[5],
	      oclicked[6]);
      g.gui.initClicked();

      /* enlever les highlights des objets de meme type */
      g.render.highlight(typechercher, false);
    }
    else {
      /* mettre en highlight les objets de meme type */
      g.render.highlight(typechercher, true);
    }
  }
  else {
    //echo("found a type auto");
    sprintf(foundpos,"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
	    listObj[found]->pos.x,
	    listObj[found]->pos.y,
	    listObj[found]->pos.z,
	    listObj[found]->pos.az,
	    listObj[found]->pos.bbs.v[0],
	    listObj[found]->pos.bbs.v[1],
	    listObj[found]->pos.bbs.v[2]);
  }

  ret = copy_string(foundpos);
  delete[] foundpos;
  return ret;
}

value recherche_Objet(value mot)
{
  char *foundpos = new char[100];
  value ret;
  sprintf(foundpos, "N/A");

#if HAVE_LIBXML2
  char *val = const_cast<char *>(String_val(mot));
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
  int deplacement;

  float posx = static_cast<float>(Double_val(px));
  float posy = static_cast<float>(Double_val(py));
  float posz = static_cast<float>(Double_val(pz));
  float orient = static_cast<float>(Double_val(ori));
  deplacement = static_cast<int>(Int_val(depl));

  // Deplacement
  if (! deplacement) {
    localuser->pos.x = posx;
    localuser->pos.y = posy;
    localuser->pos.z = posz;
    localuser->pos.az = orient;
  }
  else {
    gotoXYZ(posx, posy, posz, orient);
  }

  return Val_int(0);
}

value recherche_Func(value mot, value act)
{
  char *val = const_cast<char *>(String_val(mot));
  char *actiontype = const_cast<char *>(String_val(act));
  int *listNumType = Vicinity::getTypeFromAction(val);
  int nbtype = listNumType[0];

  //echo("I'am here %d %s %s !", nbtype, val, actiontype);

  if (nbtype > 0) {
    char** typelist = new char*[nbtype];

    for (int k=1 ; k < 1+nbtype ; k++) {
      typelist[k-1] = Vicinity::translateNum2Type(listNumType[k]);
    }

    int nb = 0;
    WO **listObj = g.render.getVisibleObjects(typelist, nbtype, &nb);

    if (nb > 0 && strcasecmp(actiontype, "NA")) {
      //on lance la methode sur les objets visibles! faut-il la lancer sur tout?
      for (int l=0; l<nb; l++) {
	if (!strcasecmp(listObj[l]->typeName(), actiontype))
	  listObj[l]->runAction(val);
      }
    }
    else {
      if (nb == 1 && listObj[0]) {
        echo("object found, do \"%s\" action !", val);
        listObj[0]->runAction(val);
      }
      else if (nb > 1) {
        echo("ambiguity, %d objects found, be more precise", nb);
      }
      else {
        echo("no visible object for \"%s\", scan around the user", val);
        Vicinity *vicin = new Vicinity();
        int nbproximity = 0;
        WO* nearObj = vicin->searchProximityObject(typelist, nbtype);
        if (nearObj) {
	  nearObj->runAction(val);
	  echo("call the method on the nearest object");	
        }
        else if (nbproximity > 1) {
	  echo("test proxi %d", nbproximity);
        }
        else {
	  echo("no action \"%s\" possible", val);
        }
      }
    }
    if (typelist) delete[] typelist; typelist = NULL;
    if (listObj) free(listObj);
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
  char *val = const_cast<char *>(String_val(mot));
  char filename[64];
  Cache::setCachePath(World::current()->url, filename);
  res = Xml::selectProximity(filename, val, &posx, &posy, &posz, &orient);
#endif

  if (! res) {
    gotoXYZ(posx, posy, posz, orient);
  }

  return Val_int(0);
}

/** fonctions d'affichage */
void msg_info(value mot)
{
  char *val = const_cast<char *>(String_val(mot));
  echo("msgInfo::%s", val);
}

void msg_debug(value mot)
{
  char *val = const_cast<char *>(String_val(mot));
  echo("msg_debug::%s", val);
}

void viewed_objects(value mot)
{
  char *val = const_cast<char *>(String_val(mot));
  Vicinity* vicin = new Vicinity(val);
  vicin->analScene();
}

/** fonctions de lecture des requetes */
int read_request(const char *requete)
{
  if (requete && (requete[0] == '@')) {
    printf("RECUPERATION\n");
  }
  if (! strcmp(requete, "TEST")) {
    printf("TESTONS\n");
    g.render.calculateFov(0,0,0,0, const_cast<char *>("TEST.jpg"));
    return 0;
  }
  if (! strcmp(requete, "C")) {
    g.render.computeCameraProjection();
    echo("calculate user visualisation");
    return 0;
  }
  if (! strcmp(requete, "V")) {
    Vicinity::show("I look at vicinity");
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
