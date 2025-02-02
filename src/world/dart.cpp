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
// dart.cpp
//
// Dart handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "dart.hpp"
#include "user.hpp"	// USER_TYPE
#include "netobj.hpp"   // netop
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
void Dart::geometry()
{
  char s[128];

  sprintf(s,"solid shape=\"box\" dim=\"%s\" dif=\"%s\" />", DIM, COLOR);
  parseSolid(s);
}

void Dart::behaviors()
{
  enableBehavior(COLLIDE_ONCE);
}

void Dart::inits()
{
  // position
  pos.x = localuser->pos.x;
  pos.y = localuser->pos.y;
  pos.z = localuser->pos.z + 0.6 * localuser->height/2;
  pos.az = localuser->pos.az;

  mobileObject(TTL);

  // action
  move.lspeed.v[0] = lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = lspeed * sin(localuser->pos.az);
  imposedMovement(TTL);

  // network creation
  netop = createNetObj(PROPS);
  netop->declareCreation();

  Sound::playSound(SHOOTSND);
}

Dart::Dart(Object *user, void *d, time_t s, time_t u)
{
  defaults();
  geometry();
  behaviors();
  inits();
}

/** Creation: this method is invisible, called by user */
void Dart::create(User *user, void *d, time_t s, time_t u)
{
  new Dart(user, d, s, u);
}

/** Replication from the network */
Object * Dart::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Dart(type_id, noid, pp);
}

Dart::Dart(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);
  netop = replicate(PROPS, _noid);
  netop->getAllProperties(pp);

  defaults();
  geometry();
  behaviors();
  mobileObject(0);
}

void Dart::get_hit(Dart *pcur, Payload *pp)
{
  Pos oldpos = pcur->pos;

  pp->getPayload("c", &(pcur->hit));

  if (pcur->hit == 1) {
    pcur->hit = 0;
    echo("%s hits me", pcur->objectName());
    Sound::playSound(OUILLESND);
  }
  pcur->updatePositionAndGrid(oldpos);
}

void Dart::funcs()
{
  getPropFunc(DART_TYPE, PROPHNAME, _Payload get_hname);
  getPropFunc(DART_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(DART_TYPE, PROPHIT, _Payload get_hit);

  putPropFunc(DART_TYPE, PROPHNAME, _Payload put_hname);
  putPropFunc(DART_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(DART_TYPE, PROPHIT, _Payload User::dartPutHit);

  setActionFunc(DART_TYPE, CREATE, _Action create, "");
}
