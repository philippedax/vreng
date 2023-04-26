//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "cloth.hpp"
#include "user.hpp"	// localuser
#include "vrsql.hpp"	// VRSql


const OClass Cloth::oclass(CLOTH_TYPE, "Cloth", Cloth::creator);

//local
static uint16_t oid = 0;

#include <list>
using namespace std;

list<WObject*> Cloth::wearList;		// list of objects weared by local user


WObject * Cloth::creator(char *l)
{
  return new Cloth(l);
}

void Cloth::defaults()
{
  ttl = MAXFLOAT;
  dx = 0;
  dy = 0;
  dz = 0;
  dax = 0;
  day = 0;
  daz = 0;
  taken = false;
  hat = false;
  halo = false;
  skirt = false;
  wings = false;
  article = NONE;
  for (int i=0; i<3 ; i++) color[i] = 0;
}

void Cloth::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "article=")) {
      char str[16];
      l = parseString(l, str, "article");
      if      (! stringcmp(str, "halo"))  { halo = true;  article = HALO; }
      else if (! stringcmp(str, "hat"))   { hat = true;   article = HAT; }
      else if (! stringcmp(str, "dress")) { skirt = true; article = DRESS; }
      else if (! stringcmp(str, "wings")) { wings = true; article = WINGS; }
    }
    else if (! stringcmp(l, "color=")) l = parseString(l, color, "color");
  }
  end_while_parse(l);
}

/* solid geometry */
void Cloth::geometry()
{
  char s[128];

  switch (article) {
  case HALO:
    sprintf(s,"solid shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%s\" />", .09, .12, color);
    break;
  case HAT:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />", .08, .08, .10, color);
    parseSolid(s);
    sprintf(s,"solid shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%s\" />", .08, .15, color);
  case DRESS:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />", .10, .30, -.50, color);
    break;
  case WINGS:
    sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" />", 1.2, .02, 1.2);
  default:
    return;
  }
  parseSolid(s);
}

void Cloth::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(TAKABLE);
  if (taken) {
    enableBehavior(DYNAMIC);
    enableBehavior(TRANSCIENT);
  }
}

/* sets position near the avatar */
void Cloth::inits()
{
  initMobileObject(ttl);
  if (taken)
    enablePermanentMovement();	// follows user

  if (! taken)  return;
  switch (article) {
  case HALO:
  case HAT:
    if (localuser) dz += localuser->height / 2;
    break;
  }

  // save original position
  ox = pos.x; oy = pos.y; oz = pos.z;
  oax = pos.ax; oay = pos.ay; oaz = pos.az;

  // set new position
  if (localuser) {
    pos.x = localuser->pos.x + dx;
    pos.y = localuser->pos.y + dy;
    pos.z = localuser->pos.z + dz;
    pos.az = localuser->pos.az + daz;
  }
  pos.ax = dax;
  pos.ay = day;

  updatePosition();
}

/* Sets an unique name */
void Cloth::setName(const char *modelname)
{
  if (localuser)
    sprintf(names.given,"%s&%s", modelname, localuser->getInstance());
  updateNames();
}

void Cloth::setName()
{
  setName(typeName());
}

void Cloth::setPersist()
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();
  if (psql && givenName()) {
    psql->insertRow(this);
    psql->updatePos(this);
    psql->updateOwner(this);
    //geom psql->updateGeom(this, geom);
  }
#endif
}

void Cloth::getPersist()
{
#if VRSQL
  if (! psql) psql = VRSql::getVRSql();
  if (psql && givenName()) {
    psql->getOwner(this);
    psql->getPos(this);
    psql->getGeom(this);
  }
#endif
}

void Cloth::delPersist()
{
#if VRSQL
  if (psql && givenName())  psql->deleteRow(this, names.given);
#endif
}

/* Creation from xml file */
Cloth::Cloth(char *l)
{
  parser(l);
  behaviors();
  inits();
}

void Cloth::changePermanent(float lasting)
{
  if (taken) {
    pos.x = localuser->pos.x + dx;
    pos.y = localuser->pos.y + dy;
    pos.z = localuser->pos.z + dz;
    pos.az = localuser->pos.az + daz;
    updatePosition();
  }
}

void Cloth::addWearList()
{
  wearList.push_back(this);
}

void Cloth::delWearList()
{
  for (list<WObject*>::iterator it = wearList.begin(); it != wearList.end(); it++) {
    if (*it == this) {
      wearList.pop_back();	// wearList.erase(it);
    }
  }
}

void Cloth::restorePosition()
{
  pos.x = ox;
  pos.y = oy;
  pos.z = oz;
  pos.ax = oax;
  pos.ay = oay;
  pos.az = oaz;
  updatePosition();
  behaviors();
}

//void Cloth::quit()
//{
//  //dax oid = 0;
//}

void Cloth::funcs()
{}
