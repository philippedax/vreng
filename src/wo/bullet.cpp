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
const float Bullet::RADIUS = 0.03;	// 3 cm

// local
static const char *COLOR = "1 0 0";	// red


void Bullet::defaults()
{
  lspeed = LSPEED;
}

/** solid geometry */
void Bullet::makeSolid()
{
  char s[128];

  sprintf(s,"solid shape=\"sphere\" r=\"%f\" emi=\"%s\" />", RADIUS, COLOR);
  parse()->parseSolid(s, SEP, this);
}

Bullet::Bullet(WObject *pu, void *d, time_t s, time_t u)
{
  defaults();
  makeSolid();

  /* position */
  pos.x = pu->pos.x;
  pos.y = pu->pos.y;
  pos.z = pu->pos.z + 0.6 * localuser->height/2;
  pos.az = pu->pos.az;

  enableBehavior(COLLIDE_ONCE);
  initMobileObject(TTL);

  /* action */
  move.lspeed.v[0] = lspeed * cos(pu->pos.az);
  move.lspeed.v[1] = lspeed * sin(pu->pos.az);
  initImposedMovement(TTL);

  /* network creation */
  createVolatileNetObject(PROPS);
  noh->declareObjCreation();

  Sound::playSound(DRIPSND);
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
  replicateVolatileNetObject(PROPS, _noid);
  noh->getAllProperties(pp);
  copyNoid(_noid);

  defaults();
  makeSolid();
  initMobileObject(0);

  Sound::playSound(DRIPSND);
}

void Bullet::get_hit(Bullet *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("c", &(pcur->hit));

  if (pcur->hit == 1) {
    pcur->hit = 0;
    notice("%s hits me", pcur->getInstance());
    Sound::playSound(OUILLESND);
  }
  pcur->updatePositionAndGrid(oldpos);
}

void Bullet::funcs()
{
  getPropertyFunc(BULLET_TYPE, PROPHNAME, WO_PAYLOAD get_hname);
  getPropertyFunc(BULLET_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(BULLET_TYPE, PROPHIT, WO_PAYLOAD get_hit);

  putPropertyFunc(BULLET_TYPE, PROPHNAME, WO_PAYLOAD put_hname);
  putPropertyFunc(BULLET_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(BULLET_TYPE, PROPHIT, WO_PAYLOAD User::bulletPutHit); // user

  setActionFunc(BULLET_TYPE, CREATE, WO_ACTION create, "");
}
