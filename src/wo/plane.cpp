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
#include "step.hpp"	// JUMP


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
  l = parseAttributes(l);
}

void Plane::makeSolid()
{
  V3 dim, rel;
  uint8_t sections;
  char s[BUFSIZ], materials[128];
  GLfloat dif[4], amb[4], spe[4], emi[4], a;
  GLint shi[1];

  getDimBB(dim);
  // decomposer le solid d'origine en plusieurs petits solides
  // pour faire de multiples bounding boxes gerables aux collisions
  if (dim.v[2] < 0.2) return; // seuil de 20 cm en hauteur

  getMaterials(dif, amb, spe, emi, shi, &a);
  //TODO: getTextures()
  sprintf(materials, "dif=\"%.2f %.2f %.2f\" amb=\"%.2f %.2f %.2f\" spe=\"%.2f %.2f %.2f\" emi=\"%.2f %.2f %.2f\" shi=\"%d\" a=\"%.2f\" %s ", dif[0], dif[1], dif[2], amb[0], amb[1], amb[2], spe[0], spe[1], spe[2], emi[0], emi[1], emi[2], shi[0], a, (tex != NULL) ? tex : "");
  sections = (uint8_t) ceil(dim.v[2] / 0.2);
  float xsec = dim.v[0] / sections;
  float ysec = dim.v[1] / sections;
  float zsec = dim.v[2] / sections;

  // le premier solide est au centre
  sprintf(s, "solid dim=\"%.2f %.2f %.2f\" %s />", 2*dim.v[0], 2*dim.v[1], 2*zsec, materials);
  parseSolid(s);
  for (int i=1; i < sections; i++) {
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
      parseSolid(s);
    }
  }
}

Plane::Plane(char *l)
{
  tex = NULL;
  parser(l);

  if (pos.ax) {	// inclinated
    makeSolid();
  }

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

/** Intersection with an object */
bool Plane::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    if (pos.ax > 0.01) {	// inclinated plane
      pcur->pos.z += Step::JUMP; // * sin(pos.ax);   // up
      pcur->updatePositionAndGrid(pold);
    }
    projectPosition(pcur, pold);
    break;
  case BALL_TYPE:
    {
    V3 normal;
    computeNormal(pold, &normal);
    bounceTrajectory(pold, &normal);
    pold->setLasting(0);
    pold->disablePermanentMovement();
    pold->copyPosAndBB(pcur->pos);
    }
    break;
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
  getPropertyFunc(PLANE_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(PLANE_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(PLANE_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(PLANE_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(PLANE_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(PLANE_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(PLANE_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(PLANE_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(PLANE_TYPE, 0, _Action moveObject, "Move");
}
