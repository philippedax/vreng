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
// ball.cpp
//
// Ball handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "ball.hpp"
#include "matvec.hpp"   // V3 M4
#include "user.hpp"	// User
#include "sql.hpp"	// deleteRow
#include "cauldron.hpp"	// Cauldron
#include "step.hpp"	// STEP_TYPE
#include "move.hpp"	// GRAVITY
#include "timer.hpp"	// rate
#include "netobj.hpp"	// netop


#define ratio() MAX(::g.timer.rate() / 20., 1)

const OClass Ball::oclass(BALL_TYPE, "Ball", Ball::creator, Ball::replicator);

const float Ball::LSPEED = 5.0;		// 5 m/s
const float Ball::ZSPEED = 3.0;		// 3 m/s
const float Ball::ASPEED = -1.5;	// 1.5 rd/s
const float Ball::SHIFT = 1.0;		// 1 m
const float Ball::RADIUS = 0.1;		// 10 cm
const float Ball::DELTAZ = 0.02;        // 2 cm
const float Ball::TTL = 3.0;		// 3 s

// local
static uint16_t oid = 0;


/** creation from a file */
Object * Ball::creator(char *l)
{
  return new Ball(l);
}

void Ball::defaults()
{
  lspeed = LSPEED;
  zspeed = ZSPEED;
  aspeed = ASPEED;
  gravity = GRAVITY / 2;
  ttl = TTL;
  taken = false;
}

/** Sets name */
void Ball::setName()
{
  oid++;
  if (! name.given)
    name.given = new char[OBJNAME_LEN];
  sprintf(name.given, "%s-%s%d", BALL_NAME, ::g.user, oid);
}

/** Solid geometry */
void Ball::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"sphere\" r=\"%f\" tx=\"%s\" />", RADIUS, DEF_URL_BALL);
  if (! geomsolid) {
    geomsolid = new char[strlen(s)];
    strcpy(geomsolid, s);
  }
  parseSolid(s);
}

/** Parse vre fileline */
void Ball::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
  }
  end_while_parse(l);
}

void Ball::behaviors()
{
  enableBehavior(PERSISTENT);
}

void Ball::inits()
{
  mobileObject(TTL);
  permanentMovement();	// follow gravity force
  createNetObj(PROPS);
}

/** Creation from fileline */
Ball::Ball(char *l)
{
  parser(l);
  behaviors();
  inits();
}

/** Creation by the cauldron */
Ball::Ball(Object *cauldron, void *d, time_t s, time_t u)
{
  defaults();
  setName();
  geometry();
  behaviors();
  inits();

  // random position
  srand(time(NULL));
  pos.x = cauldron->pos.x + static_cast<float>(drand48()) * 2 -1;
  pos.y = cauldron->pos.y + static_cast<float>(drand48()) * 2 -1;
  pos.z = cauldron->pos.z + 1;	// + 1m
}

/** Recreated by the world (persistency) */
Ball::Ball(World *world, void *d, time_t s, time_t u)
{
  char *nam = static_cast<char *>(d);	// name of the ball
  if (! nam)  return;

  char *p = nam;
  while (*p && !isdigit(*p)) p++;
  oid = atoi(p);
  strcpy(name.given, nam);
  echo("recreate: %s oid=%d", name.given, oid);

  defaults();
  geometry();
  behaviors();
  inits();
}

/** Creation by the user - via addobj (GUI) */
Ball::Ball(Object *user, char *solid)
{
  defaults();
  behaviors();
  setName();
  inits();
  parseSolid(solid);

  // position in front of user
  pos.x = user->pos.x + 0.4;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  updatePosition();
}

/** Replication from the network */
Object * Ball::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Ball(type_id, noid, pp);
}

/** Creation from the network */
Ball::Ball(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);

  netop = replicate(PROPS, _noid);
  netop->getAllProperties(pp);

  defaults();
  geometry();
  behaviors();
  mobileObject(0);
}

/** Update time lasting */
void Ball::timing(time_t sec, time_t usec, float *dt)
{
  lasting(sec, usec, dt);
}

/** Checks is moving */
bool Ball::isMoving()
{
  if (taken)
    return true;
  return testMoving();
}

/** Imposed movements */
void Ball::imposed(float dt)
{
  if (! taken) {
    pos.x  += dt * move.lspeed.v[0] * ratio();
    pos.y  += dt * move.lspeed.v[1] * ratio();
    pos.z  += dt * move.lspeed.v[2] * ratio();
    pos.az += dt * move.aspeed.v[0] * ratio();
    pos.ax += dt * move.aspeed.v[2] * ratio();

    // reduce speed
    move.lspeed.v[0] /= 1.03;
    move.lspeed.v[1] /= 1.03;
    move.lspeed.v[2] /= 1.03;
    move.aspeed.v[0] /= 1.02;
    //move.aspeed.v[2] /= 1.02;
  }
  else { // ball is taken by user
    float off = 0.4; //(pos.x > localuser->pos.x) ? 0.4 : -0.4;
    pos.x = localuser->pos.x + off * cos(localuser->pos.az);
    pos.y = localuser->pos.y + off * sin(localuser->pos.az);
    pos.z = localuser->pos.z + 0.5;
  }
}

/** Permanent movements */
void Ball::permanent(float dt)
{
  pos.z  -= dt * gravity;
}

/** Updates to the network */
bool Ball::publish(const Pos &oldpos)
{
  return publishPos(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/** Intersects with an other object */
bool Ball::intersect(Object *pcur, Object *pold)
{
  switch (pcur->type) {
  case BALL_TYPE:
    pcur->pos.x += (static_cast<float>(drand48()) * 2 - 1) * RADIUS / 2;
    pcur->pos.y += (static_cast<float>(drand48()) * 2 - 1) * RADIUS / 2;
    break;
  case USER_TYPE:
    if (! taken) {
      /* user pushes the ball on the ground */
      move.lspeed.v[0] = (lspeed / 4) * cos(pcur->pos.az);
      move.lspeed.v[1] = (lspeed / 4) * sin(pcur->pos.az);
      ttl = 1;	// 1 sec
      imposedMovement(ttl);
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
void Ball::wallsIntersect(Object *pold, V3 *normal)
{
  enableBehavior(COLLIDE_ONCE);
  bounceTrajectory(pold, normal);
}

/** Action push */
void Ball::push()
{
  move.lspeed.v[0] = lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = lspeed * sin(localuser->pos.az);
  move.lspeed.v[2] = 0;
  move.aspeed.v[0] = 0;
  ttl = Ball::TTL;
  ttl /= ratio();
  imposedMovement(ttl);
  taken = false;
}

/** Action pull */
void Ball::pull()
{
  move.lspeed.v[0] = -lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = -lspeed * sin(localuser->pos.az);
  move.lspeed.v[2] = 0;
  move.aspeed.v[0] = 0;
  ttl = Ball::TTL;
  ttl /= ratio();
  imposedMovement(ttl);
  taken = false;
}

/** Action shoot */
void Ball::shoot()
{
  move.lspeed.v[0] = lspeed * cos(localuser->pos.az);
  move.lspeed.v[1] = lspeed * sin(localuser->pos.az);
  move.lspeed.v[2] = zspeed;
  move.aspeed.v[0] = aspeed;
  ttl = Ball::TTL;
  ttl /= ratio();
  imposedMovement(ttl);
  taken = false;
}

/** Action up */
void Ball::up()
{
  move.lspeed.v[0] = 0;
  move.lspeed.v[1] = 0;
  move.lspeed.v[2] = zspeed / 2;
  move.aspeed.v[0] = aspeed;
  ttl = 1;	// 1 sec
  imposedMovement(ttl);
}

/** Action take */
void Ball::take()
{
  ttl = MAXFLOAT;
  imposedMovement(ttl);
  stopPermanent();
  taken = true;
}

/** Action drop */
void Ball::drop()
{
  permanentMovement();
  ttl = 0;
  imposedMovement(ttl);
  taken = false;
}

/** Action turn */
void Ball::turn()
{
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.lspeed.v[2] = 0;
  move.aspeed.v[0] = aspeed * 2;
  ttl = 1;	// 1 sec
  imposedMovement(ttl);
}

/** Action destroy */
void Ball::destroy()
{
  taken = false;
  clearObjectBar();
  toDelete();	// predelete Ball
  if (vsql) vsql->deleteRow(this);
}

void Ball::quit()
{
  savePersist();
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
  getPropFunc(BALL_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(BALL_TYPE, PROPZ, _Payload get_z);
  getPropFunc(BALL_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(BALL_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(BALL_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(BALL_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(BALL_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(BALL_TYPE, PROPZ, _Payload put_z);
  putPropFunc(BALL_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(BALL_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(BALL_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(BALL_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(BALL_TYPE, PUSH,     _Action push_cb, "Push");
  setActionFunc(BALL_TYPE, PULL,     _Action pull_cb, "Pull");
  setActionFunc(BALL_TYPE, SHOOT,    _Action shoot_cb, "Shoot");
  setActionFunc(BALL_TYPE, UP,       _Action up_cb, "Up");
  setActionFunc(BALL_TYPE, TAKE,     _Action take_cb, "Take");
  setActionFunc(BALL_TYPE, DROP,     _Action drop_cb, "Drop");
  setActionFunc(BALL_TYPE, TURN,     _Action turn_cb, "Turn");
  setActionFunc(BALL_TYPE, KILL,     _Action destroy_cb, "Destroy");
  setActionFunc(BALL_TYPE, CREATE,   _Action create_cb, "");
  setActionFunc(BALL_TYPE, RECREATE, _Action recreate_cb, "");
 }
