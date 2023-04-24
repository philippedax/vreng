//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#include "bullet.hpp"
#include "user.hpp"	// localuser
#include "netobj.hpp"	// NetObject
#include "payload.hpp"	// Payload
#include "sound.hpp"	// playSound


const OClass Bullet::oclass(BULLET_TYPE, "Bullet", NULL, Bullet::replicator);
const float Bullet::LSPEED = 5.0;	// 5 m/s
const float Bullet::TTL = 3.0;		// 3 sec
const float Bullet::RADIUS = 0.01;	// 1 cm

// local
static const char *COLOR = "1 0 0";	// red


void Bullet::defaults()
{
  lspeed = LSPEED;
}

/** solid geometry */
void Bullet::geometry()
{
  char s[128];

  sprintf(s,"solid shape=\"sphere\" r=\"%f\" emi=\"%s\" />", RADIUS, COLOR);
  parseSolid(s);
}

void Bullet::behaviors()
{
  enableBehavior(COLLIDE_ONCE);
}

void Bullet::inits()
{
  initMobileObject(TTL);

  /* network creation */
  netop = createVolatileNetObject(PROPS);
  netop->declareObjCreation();

  /* position */
  pos.x = localuser->pos.x;
  pos.y = localuser->pos.y;
  pos.z = localuser->pos.z + 0.6 * localuser->height/2;
  pos.az = localuser->pos.az;

  /* action */
  move.lspeed.v[0] = lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = lspeed * sin(localuser->pos.az);
  initImposedMovement(TTL);

  Sound::playSound(DRIPSND);
}

Bullet::Bullet(WObject *pu, void *d, time_t s, time_t u)
{
  defaults();
  geometry();
  behaviors();
  inits();
}

/* Creation: this method is invisible, called by user */
void Bullet::create(User *user, void *d, time_t s, time_t u)
{
  new Bullet(user, d, s, u);
}

/* Replication from the network */
WObject * Bullet::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Bullet(type_id, noid, pp);
}

Bullet::Bullet(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);
  netop = replicateNetObject(PROPS, _noid);
  netop->getAllProperties(pp);

  defaults();
  geometry();
  behaviors();
}

void Bullet::get_hit(Bullet *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("c", &(pcur->hit));

  if (pcur->hit == 1) {
    pcur->hit = 0;
    echo("%s hits me", pcur->getInstance());
    Sound::playSound(OUILLESND);
  }
  pcur->updatePositionAndGrid(oldpos);
}

void Bullet::funcs()
{
  getPropertyFunc(BULLET_TYPE, PROPHNAME, _Payload get_hname);
  getPropertyFunc(BULLET_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(BULLET_TYPE, PROPHIT, _Payload get_hit);

  putPropertyFunc(BULLET_TYPE, PROPHNAME, _Payload put_hname);
  putPropertyFunc(BULLET_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(BULLET_TYPE, PROPHIT, _Payload User::bulletPutHit); // user

  setActionFunc(BULLET_TYPE, CREATE, _Action create, "");
}
