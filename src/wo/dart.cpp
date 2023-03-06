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
#include "dart.hpp"
#include "user.hpp"	// USER_TYPE
#include "netobj.hpp"	// NetObject
#include "payload.hpp"	// Payload
#include "sound.hpp"	// playSound


const OClass Dart::oclass(DART_TYPE, "Dart", NULL, Dart::replicator);
const float Dart::LSPEED = 5.;		// 5 m/s
const float Dart::TTL = 3.;		// 3 sec

// local
static const char *DIM = "0.50 .01 .01";// 50 cm
static const char *COLOR = "0 0 0";	// black


void Dart::defaults()
{
  lspeed = LSPEED;
}

/** solid geometry */
void Dart::makeSolid()
{
  char s[256];

  sprintf(s,"solid shape=\"box\" dim=\"%s\" dif=\"%s\" />", DIM, COLOR);
  parseSolid(s);
}

Dart::Dart(WObject *user, void *d, time_t s, time_t u)
{
  defaults();
  makeSolid();

  /* position */
  pos.x = user->pos.x;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.6 * localuser->height/2;
  pos.az = user->pos.az;

  enableBehavior(COLLIDE_ONCE);
  initMobileObject(TTL);

  /* action */
  move.lspeed.v[0] = lspeed * cos(user->pos.az);
  move.lspeed.v[1] = lspeed * sin(user->pos.az);
  initImposedMovement(TTL);

  /* network creation */
  netop = createVolatileNetObject(PROPS);
  netop->declareObjCreation();

  Sound::playSound(SHOOTSND);
}

/* Creation: this method is invisible, called by user */
void Dart::create(User *user, void *d, time_t s, time_t u)
{
  new Dart(user, d, s, u);
}

/* Replication from the network */
WObject * Dart::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Dart(type_id, noid, pp);
}

Dart::Dart(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);
  netop = replicateNetObject(PROPS, _noid);
  netop->getAllProperties(pp);

  makeSolid();

  defaults();
  enableBehavior(COLLIDE_ONCE);
  initMobileObject(0);

  Sound::playSound(SHOOTSND);
}

void Dart::get_hit(Dart *pcur, Payload *pp)
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

void Dart::funcs()
{
  getPropertyFunc(DART_TYPE, PROPHNAME, _Payload get_hname);
  getPropertyFunc(DART_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(DART_TYPE, PROPHIT, _Payload get_hit);

  putPropertyFunc(DART_TYPE, PROPHNAME, _Payload put_hname);
  putPropertyFunc(DART_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(DART_TYPE, PROPHIT, _Payload User::dartPutHit);

  setActionFunc(DART_TYPE, CREATE, _Action create, "");
}
