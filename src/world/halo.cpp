//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// halo.cpp
//
// Halo handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "halo.hpp"
#include "user.hpp"	// User


const OClass Halo::oclass(HALO_TYPE, "Halo", Halo::creator);

GLfloat Halo::RI = 0.06;
GLfloat Halo::RE = 0.09;

//local
static uint16_t oid = 0;


Object * Halo::creator(char *l)
{ 
  return new Halo(l);
}

void Halo::defaults()
{
  article = Cloth::HALO;
  ttl = MAXFLOAT;
  dx = 0;
  dy = 0;
  dz = 0.05;		// 5 cm above
  dax = M_PI_4 / 2;	// oblique PI/8
  day = 0;
  daz = -M_PI_2;	// behind

  color[0] = .94;
  color[1] = .90;
  color[2] = 0;
  sprintf(strcolor, "%.2f %.2f %.2f", color[0], color[1], color[2]);
}

/* solid geometry */
void Halo::geometry()
{
  char s[128];

  sprintf(s, "geom shape=\"disk\" ri=\"%f\" re=\"%f\" dif=\"%s\" />", RI, RE, strcolor);
  parseSolid(s);
}

void Halo::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(TAKABLE);
  if (taken) {
    enableBehavior(DYNAMIC);
    enableBehavior(TRANSCIENT);
  }
}

/* sets position near the avatar */
void Halo::inits()
{
  clothObject(ttl);
  if (taken)
    permanentMovement();  // follows user

  if (! taken)  return;
  if (localuser) dz += localuser->height / 2;

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


/* Created from file */
void Halo::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);	// <geom ... />
    if (!l) break;
  }
  end_while_parse(l);
}

/* Created from file */
Halo::Halo(char *l)
{
  parser(l);
  taken = false;
  behaviors();
  inits();
}

/* Regenerated by world via VSql */
Halo::Halo(User *user, void *d, time_t s, time_t u)
{
  char *str = static_cast<char *>(d);       // name transmitted
  if (!str)  return;

  strcpy(name.given, str);
  strcpy(name.type, typeName());     // need name.type for VSql

  defaults();
  taken = true;
  setName();
  setOwner();
  getPersist();
  geometry();
  behaviors();
  inits();
}

void Halo::quit()
{
  oid = 0;
  savePersist();
}

/* wear */
void Halo::wear()
{
  taken = true;
  defaults();
  setName();
  setOwner();
  setPersist();
  behaviors();
  inits();
  createNetObj(PROPS);
  addWearList();
}

/* takeoff */
void Halo::takeoff()
{
  // restore original position
  restorePosition();
  delPersist();
  delWearList();
  taken = false;
}

/* wear: indirectly called by user button */
void Halo::wear_cb(Halo *halo, void *d, time_t s, time_t u)
{
  halo->wear();
}

/* takeoff: indirectly called by user button */
void Halo::takeoff_cb(Halo *halo, void *d, time_t s, time_t u)
{
  halo->takeoff();
}

/* Creation: this method is invisible: called by the World */
void Halo::recreate_cb(User *pu, void *d, time_t s, time_t u)
{
  new Halo(pu, d, s, u);
}

void Halo::funcs()
{
  getPropFunc(HALO_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(HALO_TYPE, PROPZ, _Payload get_z);
  getPropFunc(HALO_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(HALO_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(HALO_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(HALO_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(HALO_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(HALO_TYPE, PROPZ, _Payload put_z);
  putPropFunc(HALO_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(HALO_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(HALO_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(HALO_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(HALO_TYPE, WEAR, _Action wear_cb, "Wear");
  setActionFunc(HALO_TYPE, TAKEOFF, _Action takeoff_cb, "Takeoff");

  setActionFunc(HALO_TYPE, RECREATE, _Action recreate_cb, "");
}
