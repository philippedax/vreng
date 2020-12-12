//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "plane.hpp"
#include "user.hpp"	// USER_TYPE
#include "ball.hpp"	// BALL_TYPE
#include "thing.hpp"	// THING_TYPE
#include "step.hpp"	// STEP_TYPE
#include "guide.hpp"	// GUIDE_TYPE


const OClass Plane::oclass(PLANE_TYPE, "Plane", Plane::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Plane::creator(char *l)
{
  return new Plane(l);
}

void Plane::parser(char *l)
{
  char *p = strstr(l, "zp=");
  if (p) {
    char *q = strrchr(p, '"');
    *++q = '\0';
    tex = new char[128];
    strcpy(tex, p);
  }
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

void Plane::makeSolid()
{
  V3 dim, rel;
  uint8_t nbsec;
  char s[BUFSIZ*2], materials[BUFSIZ];
  GLfloat dif[4], amb[4], spe[4], emi[4], a;
  GLint shi[1];

  getDim(dim);
  // decomposer le solid d'origine en plusieurs petits solides
  // pour faire de multiples bounding boxes gerables aux collisions
  if (dim.v[2] < 0.2) return; // seuil de 20 cm en hauteur

  getMaterials(dif, amb, spe, emi, shi, &a);
  //TODO: getTextures()
  sprintf(materials, "dif=\"%.2f %.2f %.2f\" amb=\"%.2f %.2f %.2f\" spe=\"%.2f %.2f %.2f\" emi=\"%.2f %.2f %.2f\" shi=\"%d\" a=\"%.2f\" %s ", dif[0], dif[1], dif[2], amb[0], amb[1], amb[2], spe[0], spe[1], spe[2], emi[0], emi[1], emi[2], shi[0], a, (tex != NULL) ? tex : "");
  nbsec = (uint8_t) ceil(dim.v[2] / 0.2);
  float xsec = dim.v[0] / nbsec;
  float ysec = dim.v[1] / nbsec;
  float zsec = dim.v[2] / nbsec;
  //error("dim=%.2f %.2f %.2f", dim.v[0],dim.v[1],dim.v[2]);
  //error("nbsec=%d zsec=%.2f %.2f %.2f", nbsec, xsec, ysec, zsec);
  deleteSolids();

  // le premier solide est au centre
  sprintf(s, "solid dim=\"%.2f %.2f %.2f\" %s />", 2*dim.v[0], 2*dim.v[1], 2*zsec, materials);
  //error("s: %s", s);
  parse()->parseSolid(s, SEP, this);
  for (int i=1; i < nbsec; i++) {
    for (int j=0; j<2; j++) {
      // les autres solides s'ecartent progressivement du centre en negatif et positif
      int k = 2*j-1; // k=-1, k=1
      rel.v[0] = k * (2*i - 1) * (xsec/2) * cos(pos.az);
      rel.v[1] = k * (2*i - 1) * (ysec/2) * sin(pos.az);
      rel.v[2] = k * (2*i - 1) * zsec / 2;
      if (fabsf(dim.v[0]) < fabsf(dim.v[1])) {
        sprintf(s, "solid rel=\"%.2f %.2f %.2f 0 0\" dim=\"%.2f %.2f %.2f\" %s />", rel.v[0], rel.v[1], rel.v[2], 2*dim.v[0], 2*dim.v[1], 2*zsec, materials);
      }
      else {
        sprintf(s, "solid rel=\"%.2f %.2f %.2f 0 0\" dim=\"%.2f %.2f %.2f\" %s />", rel.v[0], rel.v[1], rel.v[2], 2*dim.v[0], 2*dim.v[1], 2*zsec, materials);
      }
      //error("s%d: rel=\"%.2f %.2f %.2f 0 0\" dim=\"%.2f %.2f %.2f\"", i, rel.v[0], rel.v[1], rel.v[2], 2*dim.v[0], 2*dim.v[1], 2*zsec);
      parse()->parseSolid(s, SEP, this);
    }
  }
}

Plane::Plane(char *l)
{
  tex = NULL;
  parser(l);

  if (pos.ax) {	// inclinated
    //enableBehavior(COLLIDE_ONCE);
    makeSolid();
  }
  enableBehavior(PERSISTENT);
  setRenderPrior(PRIOR_MEDIUM);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

/** Intersection with an object */
bool Plane::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {

  case USER_TYPE:
  case THING_TYPE:
    if (pos.ax > 0.01) {	// inclinated plane
      pcur->pos.z += Step::JUMP; // * Sin(pos.ax);   // up
      pcur->updatePositionAndGrid(pold);
    }
    projectPosition(pcur, pold);
    break;

  case BALL_TYPE:
    {
#if 1 //cpax FIXME
    V3 normal;
    computeNormal(pold, &normal);
    bounceTrajectory(pold, &normal);
    pold->setLasting(0);
    pold->disablePermanentMovement();
    pold->copyPosAndBB(pcur->pos);
#else
    projectPosition(pcur, pold);
#endif
    }
    break;

  case STEP_TYPE:
  case GUIDE_TYPE:
    return false;

  default:
    pold->copyPosAndBB(pcur->pos);
  }
  return true;
}

void Plane::quit()
{
  oid = 0;
  if (tex) delete[] tex;
  tex = NULL;
  savePersistency();
}

void Plane::funcs()
{
  getPropertyFunc(PLANE_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(PLANE_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(PLANE_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(PLANE_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(PLANE_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(PLANE_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(PLANE_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(PLANE_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(PLANE_TYPE, 0, WO_ACTION moveObject, "Move");
}
