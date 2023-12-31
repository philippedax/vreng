//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
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
#include "vreng.hpp"
#include "vicinity.hpp"
#include "matvec.hpp"   // V3 M4
#include "solid.hpp"	// Solid, object()
#include "wobject.hpp"	// WO
#include "user.hpp"	// localuser
#include "render.hpp"	// getSolidList
#include "pref.hpp"	// width3D

#include <map>
#include <vector>

#define _DEBUG_TOPO_ 0


Vicinity::Vicinity()
{
  solidLst = ::g.render.getSolidList();
  obj = (WO *) localuser;
  objName = obj->getInstance();
  setSize(user);
  userDist = DIST_INCL;
  objSize = userSize;

  vector<Vicin> vicinList(solidLst.size());

  listSize = 0;
  for (list<Solid*>::iterator il = solidLst.begin(); il != solidLst.end() ; ++il) {
    if (uselessType((*il)->object())) continue;
    vicinList[listSize].size = computeSize((*il)->object());
    vicinList[listSize].dist = computeDist((*il)->object(), obj);
    vicinList[listSize].object = (*il)->object();
    listSize++;
  }
  vicinList.resize(listSize);
}

Vicinity::Vicinity(string objectName)
{
  // On instancie l'objet
  objName = strdup(objectName.c_str());
  solidLst = ::g.render.getSolidList();

  // On cherche l'objet dans la liste
  obj = NULL;
  obj->getObjectByName(objName.c_str());
  //echo("J'ai trouve l'objet %s dans la liste des objets",obj->getInstance());

  setSize(localuser);
  userDist = computeDist(user, obj);
  objSize = computeSize(obj);

  vector<Vicin> vicinList(solidLst.size());

  listSize = 0;
  for (list<Solid*>::iterator il = solidLst.begin(); il != solidLst.end() ; ++il) {
    if (uselessType((*il)->object())) continue;
    vicinList[listSize].size = computeSize((*il)->object());
    vicinList[listSize].dist = computeDist((*il)->object(), obj);
    vicinList[listSize].object = (*il)->object();
    listSize++;
  }
  vicinList.resize(listSize);
  //free((char *)objName);
}

// Evaluate size of the avatar
void Vicinity::setSize(WO *user)
{
  if      ((user->pos.bbs.v[0] > 10) && (user->pos.bbs.v[1] > 4) && (user->pos.bbs.v[2] > 4))
    userSize = SIZE_HUGE;
  else if ((user->pos.bbs.v[0] > 5) && (user->pos.bbs.v[1] > 2) && (user->pos.bbs.v[2] > 2))
    userSize = SIZE_LARGE;
  else if ((user->pos.bbs.v[0] < 1) && (user->pos.bbs.v[1] < 0.5) && (user->pos.bbs.v[2] < 0.5))
    userSize = SIZE_SMALL;
  else if ((user->pos.bbs.v[0] < 0.5) && (user->pos.bbs.v[1] < 0.2) && (user->pos.bbs.v[2] < 0.2))
    userSize = SIZE_TINY;
  else
    userSize = SIZE_NORMAL;
}

Vicinity::~Vicinity()
{
  if (vicinList) delete vicinList;
  vicinList = NULL;
}

Vicinity::Dist Vicinity::computeDist(WO *obj1, WO *obj2)
{
  float posobj1[6];
  float posobj2[6];
  float mindist[3];
  int inclusion[3];

  posobj1[0] = obj1->pos.x;
  posobj1[1] = obj1->pos.y;
  posobj1[2] = obj1->pos.z;
  posobj1[3] = obj1->pos.bbs.v[0];
  posobj1[4] = obj1->pos.bbs.v[1];
  posobj1[5] = obj1->pos.bbs.v[2];
  posobj2[0] = obj2->pos.x;
  posobj2[1] = obj2->pos.y;
  posobj2[2] = obj2->pos.z;
  posobj2[3] = obj2->pos.bbs.v[0];
  posobj2[4] = obj2->pos.bbs.v[1];
  posobj2[5] = obj2->pos.bbs.v[2];

  float obj1A[3], obj1B[3], obj2A[3], obj2B[3];

  for (int i=0; i<3; i++) {
    obj1A[i] = (posobj1[i]-posobj1[i+3]);
    obj1B[i] = (posobj1[i]+posobj1[i+3]);
    obj2A[i] = (posobj2[i]-posobj2[i+3]);
    obj2B[i] = (posobj2[i]+posobj2[i+3]);
  }

  // calcul de la distance suivant les 3 axes
  for (int i=0; i<3; i++) {
    mindist[i] = MIN(ABSF(obj2B[i]-obj1A[i]), ABSF(obj2A[i]-obj1B[i]));
  }

  // calcul des inclusions d'objets
  for (int i=0; i<3; i++) {
    //pour diminuer les tests on inverse en cas de besoin les 2 objets
    if (obj2A[i] < obj1A[i]) {
      for (int i=0; i<3; i++) {
        float objtmp;
	objtmp = obj2A[i];
	obj2A[i] = obj1A[i];
	obj1A[i] = objtmp;
	objtmp = obj2B[i];
	obj2B[i] = obj1B[i];
	obj1B[i] = objtmp;
      }
    }
    if ((obj1A[i] < obj2A[i]) && (obj2A[i] < obj1B[i])) {
      if (obj2B[i] < obj1B[i]) inclusion[i] = DIST_INCL;
      else inclusion[i] = DIST_INTER;
    } else {
      if (mindist[i] < 0.5) inclusion[i] = DIST_STUCK;
      else if (mindist[i] > 15) inclusion[i] = DIST_FAR;
      else inclusion[i] = DIST_NEAR;
    }
  }
  if ((inclusion[0] == DIST_INCL) && (inclusion[1] == DIST_INCL) && (inclusion[2] == DIST_INCL))
    return DIST_INCL;
  if ((inclusion[0] == DIST_FAR) || (inclusion[1] == DIST_FAR) || (inclusion[2] == DIST_FAR))
    return DIST_FAR;
  if ((inclusion[0] == DIST_NEAR) || (inclusion[1] == DIST_NEAR) || (inclusion[2] == DIST_NEAR))
    return DIST_NEAR;
  if ((inclusion[0] == DIST_STUCK) || (inclusion[1] == DIST_STUCK) || (inclusion[2] == DIST_STUCK))
    return DIST_STUCK;
  if ((inclusion[0] == DIST_INTER) || (inclusion[1] == DIST_INTER) || (inclusion[2] == DIST_INTER))
    return DIST_INTER;
  error("computeDist: default distance value");
  return DIST_INTER;
}

Vicinity::Size Vicinity::computeSize(WO *obj)
{
  Size size = SIZE_NORMAL;

  if ((obj->pos.bbs.v[0] > (10*localuser->pos.bbs.v[0]))
     && (obj->pos.bbs.v[1] > (4*localuser->pos.bbs.v[1]))
     && (obj->pos.bbs.v[2] > (4*localuser->pos.bbs.v[2]))) {
    size = SIZE_HUGE;
  } else if ((obj->pos.bbs.v[0] > (5*localuser->pos.bbs.v[0]))
	   && (obj->pos.bbs.v[1] > (2*localuser->pos.bbs.v[1]))
	   && (obj->pos.bbs.v[2] > (2*localuser->pos.bbs.v[2]))) {
    size = SIZE_LARGE;
  } else if ((obj->pos.bbs.v[0] < (1*localuser->pos.bbs.v[0]))
	   && (obj->pos.bbs.v[1] < (0.5*localuser->pos.bbs.v[1]))
	   && (obj->pos.bbs.v[2] < (0.5*localuser->pos.bbs.v[2]))) {
    size = SIZE_SMALL;
  } else if ((obj->pos.bbs.v[0] < (0.5*localuser->pos.bbs.v[0]))
	   && (obj->pos.bbs.v[1] < (0.2*localuser->pos.bbs.v[1]))
	   && (obj->pos.bbs.v[2] < (0.2*localuser->pos.bbs.v[2]))) {
    size = SIZE_TINY;
  } else {
    size = SIZE_NORMAL;
  }
  return size;
}

int Vicinity::compDist(const void *t1, const void *t2)
{
  Vicin s1 = *((Vicin*) t1);
  Vicin s2 = *((Vicin*) t2);

  // increasing order
  if (s1.dist > s2.dist) return 1;
  if (s1.dist < s2.dist) return -1;
  if (s1.size > s2.size) return -1;
  if (s1.size < s2.size) return 1;
  return 0;
}

void Vicinity::sortDist()
{
  qsort((void *)vicinList, listSize, sizeof(vicinList[0]), compDist);
}

int Vicinity::compInterest(const void *t1, const void *t2)
{
  Vicin s1 = *((Vicin*) t1);
  Vicin s2 = *((Vicin*) t2);
  int size1 = ((int) s1.size);
  int size2 = ((int) s2.size);
  int dist1 = (int) s1.dist;
  int dist2 = (int) s2.dist;

  // qd un objet est loin on en tient pas compte
  if ((dist1 == 4) && (dist2 < 4)) return 1;
  if ((dist2 == 4) && (dist1 < 4)) return -1;

  // si la taille de l'objet est plus grande ou si la distance est plus proche
  if ((size1 > size2) && (dist1 < dist2)) return -1;
  if ((size1 < size2) && (dist1 > dist2)) return 1;

  // un objet grand et pres est plus important qu'un objet petit et loin
  if ((size1 > size2) && (dist1 > dist2)) {
    int distu1 = ((int) computeDist(localuser, s1.object));
    int distu2 = ((int) computeDist(localuser, s2.object));

    // si un objet est grand loin size1 dist1 et un autre petit pres size2 dist2
    // on regarde la distance de leur centre a l'utilisateur.
    if (distu1 > distu2) return 1;
    else                 return -1;
  }
  if ((size1 < size2) && (dist1 < dist2)) {
    int distu1 = ((int) computeDist(localuser, s1.object));
    int distu2 = ((int) computeDist(localuser, s2.object));
    if (distu1 > distu2) return 1;
    else                 return -1;
  }
  // si un objet est de taille proche de l'utilisateur il est plus important
  // TODO: trouver une formule magique pour rendre encore plus pertinent le test
  return 0;
}

void Vicinity::sortInterest()
{
  qsort((void *)vicinList, listSize, sizeof(Vicin), compInterest);
}

WO* Vicinity::searchProximityObject(char** typeObj, int nb)
{
  sortDist();

  WO* o = NULL;
  for (int i=0; i<listSize && vicinList[i].dist <= DIST_NEAR; i++) {
    for (int j=0; j<nb ; j++) {
      if (!strcasecmp((vicinList[i].object)->typeName(), typeObj[j])) {
	o = vicinList[i].object;
	return o;
      }
    }
  }
  return NULL;
}

void Vicinity::describeTopo(Vicin vicin)
{
  string msg;

  switch (vicin.dist) {
  case DIST_INCL:  msg = "includes"; break;
  case DIST_INTER: msg = "intersects"; break;
  case DIST_STUCK: msg = "is stuck to"; break;
  case DIST_NEAR:  msg = "is near of"; break;
  case DIST_FAR:   msg = "is very far of"; break;
  }
  echo("object %s %s %s", objName.c_str(), msg.c_str(), (vicin.object)->getInstance());
}

void Vicinity::analScene()
{ 
  // test: que vois-je de cristal-pyramid ?
  // on analyse le monde par rapport a un objet

  //sortInterest();
  sortDist();
  analTopo();
  analVisual(25);
  analVicinity(); 
  //actionList();
  return;
}

void Vicinity::analTopo()
{
  int toread = 3;

#if _DEBUG_TOPO_
  for (int i=0; i < listSize; i++) {
    echo(" object (%s) size :%d dist:%d",
	  (vicinList[i].object)->getInstance(),
	  vicinList[i].size,
	  vicinList[i].dist);
  }
#endif
  if (listSize < toread) toread = listSize;

  for (int i=0; i < toread; i++) {
    describeTopo(vicinList[i]);
  }
}

bool Vicinity::uselessType(WO *obj)
{
  if (! obj) return true;

  if (!strcasecmp(obj->typeName(), "ground")) return true;
  if (!strcasecmp(obj->typeName(), "wall")) return true;
  if (!strcasecmp(obj->typeName(), "user")) return true;

  return false;
}

int Vicinity::compVisual(const void *t1, const void *t2)
{
  VisualPos s1 = *((VisualPos*) t1);
  VisualPos s2 = *((VisualPos*) t2);

  // decreasing order
  if (s1.nbPixels > s2.nbPixels) return -1;
  if (s1.nbPixels < s2.nbPixels) return 1;
  return 0;
}

void Vicinity::sortVisual()
{
  qsort((void *)visualList, visualListSize, sizeof(VisualPos), compVisual);
}

/**
 * Creer une structure des elements visibles pour l'utilisateur
 * tel que : la proportion occupee par l'objet, son emplacement sur l'ecran.
 * Tout a partir de l'ecran de visualisation de l'utilisateur
 */
void Vicinity::analVisual(int details)
{
  int drawed = 0;

  visualListSize = 0;
  ::g.render.getDrawedObjects(&drawed);

  if (drawed < 1) drawed = 5000;

  visualList = new VisualPos[drawed];

  for (int i=0; i < drawed; i++) {
    visualList[i].object = NULL;
    visualList[i].nbPixels = 0;
    visualList[i].xmin = -1;
    visualList[i].xmax = -1;
    visualList[i].ymin = -1;
    visualList[i].ymax = -1;
  }

  //TODO : super lourd mais tous ces calculs sont precalcules sur
  //les nouvelles cartes graphiques et c'est pour faire des tests
  //c pas encore l'algo final c'est pour avoir des valeurs fines
  //pour le second algo rapide qui me donnera des donnees approx

  for (int i=0; i < ::g.pref.width3D; i += details) {
    for (int j=0; j < ::g.pref.height3D; j += details) {
      bool transp = true;
      int depth = 0;

      // au bout de 5 niveau de transparence on peut supposer l'objet opaque
      while (transp || depth > 4) {
	// gestion de transparence
        uint16_t n = ::g.render.bufferSelection(i, j, depth);
        WO* po = WO::byNum(n);
	depth++;
	
	if (po && po->getSolid()->isOpaque()) transp = false;
        else if (!po) {
	  transp = false;
	  break;  //end while
	}
	if (uselessType(po))
          continue;

	// 3 boucles for je sais c pas bien ! mais j'y suis oblige
	bool found = false;
	for (int k=0; k < visualListSize; k++) {
	  if (po && visualList[k].object && visualList[k].object == po) {
	    visualList[k].nbPixels++;
	    found = true;
	    visualList[k].xmax = i;
	    visualList[k].ymax = j;
	    break;
	  }
	}
	if (!found && po) {
	  visualList[visualListSize].object = po;
	  visualList[visualListSize].nbPixels++;
	  visualList[visualListSize].xmin = i;
	  visualList[visualListSize].xmax = i;
	  visualList[visualListSize].ymin = j;
	  visualList[visualListSize].ymax = j;
	  visualListSize++;
	}
      } //end while
    }
    fprintf(stderr, "| %d ", i);
  } //fin des 3 for
  sortVisual();
  describeVisual();

  if (visualList) delete[] visualList;
  visualList = NULL;
}

void Vicinity::describeVisual()
{
  V3 coord = ::g.render.getVisiblePosition(obj);
  //echo("coord de objet %s, x:%.2f y:%.2f", objName.c_str(), coord.v[0], coord.v[1]);

  char* msg = NULL;
  char msgprecis[100];

  if (coord.v[0] < 0)                                 msg = strdup("est a gauche");
  else if (coord.v[0] > (::g.pref.width3D))           msg = strdup("est a droite");
  else if (coord.v[0] < (::g.pref.width3D/3.0))       msg = strdup("est sur la gauche");
  else if (coord.v[0] > ((::g.pref.width3D*2.0)/3.0)) msg = strdup("est sur la droite");
  else                                                msg = strdup("est devant"); 
  float deltaVisual = 0.0;
  if (coord.v[0] < ((visualList[0].xmin+visualList[0].xmax)/2.0-deltaVisual)) {
    sprintf(msgprecis, "est a gauche de %s", (visualList[0].object)->getInstance());
  }
  else if (coord.v[0] < ((visualList[0].xmin+visualList[0].xmax)/2.0+deltaVisual)) {
    sprintf(msgprecis, "est a droite de %s", (visualList[0].object)->getInstance());
  }
  else {
    sprintf(msgprecis, "est pres de %s", (visualList[0].object)->getInstance());
  }
  echo("-> %s %s %s ! ok ?", objName.c_str(), msg, msgprecis);
  free(msg);
}

/*TODO : Fusion de l'analyse Topo + Visuel en fonction de la */
void Vicinity::analVicinity()
{
  int hits = 0;
  bool *seen = NULL;
  WO **drawedObj;

  if (obj) {
    drawedObj = ::g.render.getDrawedObjects(&hits);
    seen = new bool[hits];

    for (int i=0; i < hits ; i++) {
      if (drawedObj[i]->isSeen() && !uselessType(drawedObj[i]))
	echo("I see: i=%d name=%s", i, drawedObj[i]->getInstance());
      else
	echo("I can't see: i=%d name=%s", i, drawedObj[i]->getInstance());
    }
  }
  if (seen) delete[] seen;
}

/* Debugging Methods */
void Vicinity::show()
{
  show("Show:");
}

void Vicinity::show(const char *str)
{
  echo("%s", str);
  int i=0;

  OList *viciList = localuser->getVicinity(localuser);
  for (OList *ol = viciList; ol && ol->pobject ; ol = ol->next, i++) {
    echo("%2d: %s", i, (ol->pobject)->getInstance());
  }
}

void Vicinity::actionList()
{
  for (int i=0; i <= OBJECTSNUMBER; i++) {
    for (int j=0; j < ACTIONSNUMBER; j++) { // action functions
      if (isActionName(i, j)) {
	echo("actionList[%d,%d]= %s %s %p", j, i,
		translateNum2Type(i),
		getActionName(i, j),
		getActionMethod(i, j));
      }
    }
  }
}

int * Vicinity::getTypeFromAction(const char *actionName)
{
  int* acttype = new int[OBJECTSNUMBER+1];
  int nbtype = 1;

  for (int i=0; i <= OBJECTSNUMBER; i++) {
    for (int j=0; j < ACTIONSNUMBER; j++) {
      if ((isActionName(i, j)) && (! strcasecmp(getActionName(i, j), actionName))) {
	acttype[nbtype] = i;
	nbtype++;
      }
    }
  }
  acttype[0] = nbtype - 1;
  return acttype;
}

char* Vicinity::translateNum2Type(int num)
{
  char* ren = new char[OBJNAME_LEN];
  map<int,string> translate;

  translate[0]  = "world";
  translate[1]  = "user";
  translate[2]  = "plane";
  translate[3]  = "door";
  translate[4]  = "thing";
  translate[5]  = "ball";
  translate[6]  = "step";
  translate[7]  = "mirage";
  translate[8]  = "gate";
  translate[9]  = "wall";
  translate[10] = "ground";
  translate[11] = "web";
  translate[12] = "button";
  translate[13] = "cauldron";
  translate[14] = "missile";
  translate[15] = "bullet";
  translate[16] = "dart";
  translate[17] = "host";
  translate[18] = "board";
  translate[19] = "doc";
  translate[20] = "walls";
  translate[21] = "entry";
  translate[22] = "bgcolor";
  translate[23] = "light";
  translate[24] = "music";
  translate[25] = "movie";
  translate[26] = "clip";
  translate[27] = "clock";
  translate[28] = "download";
  translate[29] = "mirror";
  translate[30] = "stars";
  translate[31] = "particle";
  translate[32] = "water";
  translate[33] = "mech";
  translate[34] = "guy";
  translate[35] = "vnc";
  translate[36] = "vrelet";
  translate[37] = "vjc";
  translate[38] = "anim";
  translate[39] = "text";
  translate[40] = "humanoid";
  translate[41] = "carrier";
  translate[42] = "cart";
  translate[43] = "book";
  translate[44] = "sheet";
  translate[45] = "icon";
  translate[46] = "model";
  translate[47] = "flag";
  translate[48] = "guide";
  translate[49] = "area";
  translate[50] = "lift";
  translate[51] = "transform";
  translate[52] = "aoi";

  strcpy(ren, translate[num].c_str());
  return ren;
}
