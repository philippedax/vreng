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
#include "ball.hpp"
#include "user.hpp"	// User
#include "vrsql.hpp"	// deleteRow
#include "cauldron.hpp"	// Cauldron
#include "step.hpp"	// STEP_TYPE
#include "move.hpp"	// GRAVITY
#include "netobj.hpp"	// NetObject
#include "timer.hpp"	// rate


#define ratio() MAX(::g.timer.rate() / 20., 1)

const OClass Ball::oclass(BALL_TYPE, "Ball", Ball::creator, Ball::replicator);

const float Ball::LSPEED = 5.0;		// 5 m/s
const float Ball::ZSPEED = 3.0;		// 3 m/s
const float Ball::ASPEED = -1.5;	// 1.5 rd/s
const float Ball::ORIGZ = 1.0;		// 1 m
const float Ball::SHIFT = 1.0;		// 1 m
const float Ball::RADIUS = 0.1;		// 10 cm
const float Ball::DELTAZ = 0.02;        // 2 cm
const float Ball::TTL = 3.0;		// 3 s

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Ball::creator(char *l)
{
  return new Ball(l);
}

void Ball::defaults()
{
  lspeed = LSPEED;
  zspeed = ZSPEED;
  aspeed = ASPEED; // * (float)drand48();
  gravity = GRAVITY / 2;
  ttl = TTL;
  taken = false;
}

/** Sets name */
void Ball::setName()
{
  oid++;
  sprintf(names.given, "%s-%s.%d", BALL_NAME, localuser->getInstance(), oid);
}

/** Solid geometry */
void Ball::makeSolid()
{
  char s[128];

  sprintf(s, "solid shape=\"sphere\" r=\"%f\" tx=\"%s\" />", RADIUS, DEF_URL_BALL);
  parse()->parseSolid(s, SEP, this);
}

/** Parse vre fileline */
void Ball::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
  }
  end_while_parse(l);
}

/** Create from fileline */
Ball::Ball(char *l)
{
  parser(l);

  //dax1 enableBehavior(PERSISTENT);

  initMobileObject(1);
  createVolatileNetObject(PROPS);
}

/** Created by the cauldron */
Ball::Ball(WObject *ball, void *d, time_t s, time_t u)
{
  defaults();
  setName();
  makeSolid();

  /* random position */
  srand((uint32_t) time(NULL));
  pos.x = ball->pos.x + (float)drand48() * 2 -1;
  pos.y = ball->pos.y + (float)drand48() * 2 -1;
  pos.z = ball->pos.z + ORIGZ;
  origz = pos.z;	// see ground

  //dax1 enableBehavior(PERSISTENT);	//dax1
  initMobileObject(TTL);
  enablePermanentMovement(); // apply gravity

  createVolatileNetObject(PROPS);
}

/** Recreated by the world (persistency) */
Ball::Ball(World *world, void *d, time_t s, time_t u)
{
  char *nam = (char *) d;	// name of the ball
  if (!nam)  return;

  char *p = nam;
  while (*p && !isdigit(*p)) p++;
  oid = atoi(p);
  strcpy(names.given, nam);
  trace(DBG_SQL, "recreate: %s oid=%d", names.given, oid);

  defaults();
  makeSolid();

  //dax1 enableBehavior(PERSISTENT);
  initMobileObject(TTL);
  enablePermanentMovement();

  createVolatileNetObject(PROPS);
}

/** Created by the user */
Ball::Ball(WObject *user, char *solid)
{
  defaults();
  setName();
  parse()->parseSolid(solid, SEP, this);

  /* position in front of user */
  pos.x = user->pos.x + 0.4;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  updatePosition();

  //dax1 enableBehavior(PERSISTENT);
  initMobileObject(TTL);

  createVolatileNetObject(PROPS);
}

/** Replication from the network */
WObject * Ball::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Ball(type_id, noid, pp);
}

Ball::Ball(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);

  replicateVolatileNetObject(PROPS, _noid);
  noh->getAllProperties(pp);
  copyNoid(_noid);

  makeSolid();
  defaults();
  initMobileObject(0);
}

void Ball::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

bool Ball::isMoving()
{
  if (taken)
    return true;
  return testMoving();
}

/** Imposed movements */
void Ball::changePosition(float lasting)
{
  if (! taken) {
    pos.x  += lasting * move.lspeed.v[0] * ratio();
    pos.y  += lasting * move.lspeed.v[1] * ratio();
    pos.z  += lasting * move.lspeed.v[2] * ratio();
    pos.az += lasting * move.aspeed.v[0] * ratio();
    pos.ax += lasting * move.aspeed.v[2] * ratio();

    // reduce speed
    move.lspeed.v[0] /= 1.02;
    move.lspeed.v[1] /= 1.02;
    move.lspeed.v[2] /= 1.02;
    move.aspeed.v[0] /= 1.02;
    move.aspeed.v[2] /= 1.02;
  }
  else { // ball is taken by user
    float off = 0.4; //(pos.x > localuser->pos.x) ? 0.4 : -0.4;
    pos.x = localuser->pos.x + off * cos(localuser->pos.az);
    pos.y = localuser->pos.y + off * sin(localuser->pos.az);
    pos.z = localuser->pos.z + 0.5;
  }
}

/** Permanent movements */
void Ball::changePermanent(float lasting)
{
  pos.z  -= lasting * gravity;
}

/** Updates to the network */
bool Ball::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/** Intersects with an other object */
bool Ball::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case BALL_TYPE:
    pcur->pos.x += ((float) drand48() * 2 - 1) * RADIUS / 2;
    pcur->pos.y += ((float) drand48() * 2 - 1) * RADIUS / 2;
    break;
  case USER_TYPE:
    if (! taken) {
      /* user pushes the ball on the ground */
      move.lspeed.v[0] = (lspeed / 4) * cos(pcur->pos.az);
      move.lspeed.v[1] = (lspeed / 4) * sin(pcur->pos.az);
      ttl = 1;	// 1 sec
      initImposedMovement(ttl);
      pcur->updatePositionAndGrid(pold);
    }
    pold->copyPositionAndBB(pcur);
    break;
  case STEP_TYPE:
    return false;
  default:
    pold->copyPositionAndBB(pcur);
    break;
  }
  pcur->updatePositionAndGrid(pold);
  return true;
}

/** Intersects with walls */
void Ball::whenWallsIntersect(WObject *pold, V3 *normal)
{
  enableBehavior(COLLIDE_ONCE);
  bounceTrajectory(pold, normal);
}

void Ball::push()
{
  move.lspeed.v[0] = lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = lspeed * sin(localuser->pos.az);
  move.lspeed.v[2] = 0;
  move.aspeed.v[0] = 0; //aspeed;
  ttl = Ball::TTL;
  ttl /= ratio();
  initImposedMovement(ttl);
  taken = false;
}

void Ball::pull()
{
  move.lspeed.v[0] = -lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = -lspeed * sin(localuser->pos.az);
  move.lspeed.v[2] = 0;
  move.aspeed.v[0] = 0; //aspeed;
  ttl = Ball::TTL;
  ttl /= ratio();
  initImposedMovement(ttl);
  taken = false;
}

void Ball::shoot()
{
  move.lspeed.v[0] = lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = lspeed * sin(localuser->pos.az);
  move.lspeed.v[2] = zspeed;
  move.aspeed.v[0] = aspeed;
  ttl = Ball::TTL;
  ttl /= ratio();
  initImposedMovement(ttl);
  taken = false;
}

void Ball::up()
{
  move.lspeed.v[0] = 0;
  move.lspeed.v[1] = 0;
  move.lspeed.v[2] = zspeed;
  move.aspeed.v[0] = 0; //aspeed;
  ttl = 1;	// 1 sec
  initImposedMovement(ttl);
}

void Ball::take()
{
  ttl = MAXFLOAT;
  initImposedMovement(ttl);
  disablePermanentMovement();
  taken = true;
}

void Ball::drop()
{
  enablePermanentMovement();
  ttl = 0;
  initImposedMovement(ttl);
  taken = false;
}

void Ball::turn()
{
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.lspeed.v[2] = 0;
  move.aspeed.v[0] = aspeed * 2;
  ttl = 1;	// 1 sec
  initImposedMovement(ttl);
}

void Ball::destroy()
{
  taken = false;
  clearObjectBar();
  toDelete();	// delete Ball
#if VRSQL
  if (psql) psql->deleteRow(this);
#endif
}

void Ball::quit()
{
  savePersistency();
}

void Ball::push_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->push();
}

void Ball::pull_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->pull();
}

void Ball::shoot_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->shoot();
}

void Ball::up_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->up();
}

void Ball::take_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->take();
}

void Ball::drop_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->drop();
}

void Ball::turn_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->turn();
}

void Ball::destroy_cb(Ball *ball, void *d, time_t s, time_t u)
{
  ball->destroy();
}

/** Creation: this method is invisible: called by the cauldron */
void Ball::create_cb(Cauldron *o, void *d, time_t s, time_t u)
{
  new Ball(o, d, s, u);
}

/** Creation: this method is invisible: called by the World */
void Ball::recreate_cb(World *w, void *d, time_t s, time_t u)
{
  new Ball(w, d, s, u);
}

void Ball::funcs()
{
  getPropertyFunc(BALL_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(BALL_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(BALL_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(BALL_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(BALL_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(BALL_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(BALL_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(BALL_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(BALL_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(BALL_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(BALL_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(BALL_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(BALL_TYPE, PUSH,     WO_ACTION push_cb, "Push");
  setActionFunc(BALL_TYPE, PULL,     WO_ACTION pull_cb, "Pull");
  setActionFunc(BALL_TYPE, SHOOT,    WO_ACTION shoot_cb, "Shoot");
  setActionFunc(BALL_TYPE, UP,       WO_ACTION up_cb, "Up");
  setActionFunc(BALL_TYPE, TAKE,     WO_ACTION take_cb, "Take");
  setActionFunc(BALL_TYPE, DROP,     WO_ACTION drop_cb, "Drop");
  setActionFunc(BALL_TYPE, TURN,     WO_ACTION turn_cb, "Turn");
  setActionFunc(BALL_TYPE, KILL,     WO_ACTION destroy_cb, "Destroy");
  setActionFunc(BALL_TYPE, CREATE,   WO_ACTION create_cb, "");
  setActionFunc(BALL_TYPE, RECREATE, WO_ACTION recreate_cb, "");
}
