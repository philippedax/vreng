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
#include "gate.hpp"
#include "world.hpp"	// enter
#include "universe.hpp"	// Universe
#include "move.hpp"	// gotoFront
#include "user.hpp"	// USER_TYPE, localuser
#include "ball.hpp"	// BALL_TYPE
#include "dart.hpp"	// DART_TYPE
#include "bullet.hpp"	// BULLET_TYPE
#include "icon.hpp"	// ICON_TYPE
#include "channel.hpp"	// join
#include "vac.hpp"	// resolveWorldUrl
#include "sound.hpp"	// playSound
#include "audio.hpp"	// start
#include "entry.hpp"	// set


const OClass Gate::oclass(GATE_TYPE, "Gate", Gate::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Gate::creator(char *l)
{
  return new Gate(l);
}

void Gate::defaults()
{
  link = false;
  automatic = false;
  flagentry = false;
  clearV3(entry);
}

void Gate::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url"))     l = parseUrl(l, names.url);
    else if (! stringcmp(l, "world"))   l = parseString(l, names.url, "world");
    else if (! stringcmp(l, "channel")) l = parseString(l, chan, "channel");
    else if (! stringcmp(l, "mode")) {
      char modestr[6];
      l = parseString(l, modestr, "mode");
      if      (! stringcmp(modestr, "link")) link = true;
      else if (! stringcmp(modestr, "auto")) automatic = true;
    }
    else if (! stringcmp(l, "entry")) {
      l = parseVector3fv(l, &entry, "entry");
      flagentry = true;
      trace(DBG_WO, "gate: entry=%.1f %.1f %.1f", entry.v[0], entry.v[1], entry.v[2]);
    }
  }
  end_while_parse(l);
}

void Gate::behaviors()
{
  enableBehavior(COLLIDE_ONCE);
}

void Gate::inits()
{
  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

/** Created fron vre file */
Gate::Gate(char *l)
{
  parser(l);
  behaviors();
  inits();
}

/** Created by user (Gui) */
Gate::Gate(WObject *user, char *geom)
{
  defaults();

  // takes position from user position
  pos.x = user->pos.x + 0.5;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  parseSolid(geom);

  behaviors();
  inits();
  updatePosition();
}

bool Gate::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/* action: enter */
void Gate::enter()
{
  /* saves url because World::quit frees gate */
  char *new_url = strdup(names.url);

  if (link) {	// without channel
    World::current()->quit();
    World::enter(new_url, NULL, World::NEW);
    World::current()->linked();	// linked world
    return;
  }

  // with channel
  if (! strcmp(names.url, ::g.url)) {
    sprintf(chan, "%s/%u/%d",
            Universe::current()->grpstr, Universe::current()->port, Channel::currentTtl());
    //echo("initial channel: %s", chan);
  }
#if 1 //dax
#define USE_VACS 0
#else
#define USE_VACS 1
#endif
#if USE_VACS
  // call here the VACS (VREng Address Cache Server) to get the channel string
  Vac *vac = Vac::current();
  if (! vac->getChannel(names.url, chan)) {
    // this url is not in the cache, we need to ask to the vacs to resolve it
    if (vac->resolveWorldUrl(names.url, chan)) {
      echo("enter: resolveWorldUrl url=%s channel=%s", names.url, chan);
    }
    else {
      //echo("enter: resolveWorldUrl failed from Vac: url=%s", names.url);
      if (! *chan) {	// no given channel
        strcpy(chan, DEF_VRE_CHANNEL);	// forced to the default
      }
    }
    trace(DBG_IPMC, "enter: getChannel=%s url=%s", chan, new_url);
  }
#endif //USE_VACS

  char *new_chan = strdup(chan);

  World::current()->quit();		// quit the current world
  delete Channel::current();		// delete Channel
  Sound::playSound(GATESND);

  World::enter(new_url, new_chan, World::NEW);	// enter in this world

  Channel::join(new_chan);
  trace(DBG_IPMC, "enter: join channel=%s url=%s", new_chan, new_url);

  if (audioactive) Audio::start(new_chan);
  if (new_chan)
    free(new_chan);
  if (new_url)
    free(new_url);
}

/* when an intersection occurs */
bool Gate::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {

  case USER_TYPE:
    if (automatic) {
      if (pcur != localuser) {
	echo("user was just pushed out of this world");
        return true;
      }
      else {
        cntcol = 0;
        if (flagentry) {
          flagentry = false;
          pcur->updatePositionAndGrid(pcur->pos);
        }
        enter();
      }
    }
    else {
      if (cntcol < 10) {
        pold->copyPositionAndBB(pcur);
        echo("you are near the gate %s in=%d", getInstance(), cntcol);
        cntcol++;
      }
      else if (cntcol < 20) {
        pold->copyPositionAndBB(pcur);
        echo("if you insist you'll enter in %s in=%d", getInstance(), cntcol);
        cntcol++;
      }
      else {
        enter();	// enter by force
        cntcol = 0;
      }
    }
    break;

  case BULLET_TYPE:
  case DART_TYPE:
    echo("%s:%s hits %s:%s", pcur->names.type, pcur->getInstance(), names.type, getInstance());
    pold->copyPositionAndBB(pcur);
    pcur->toDelete();
    break;

  case BALL_TYPE:
    projectPosition(pcur, pold);
    break;

  case ICON_TYPE:
    // stick the icon on the wall
    doAction(ICON_TYPE, Icon::STICK, this, pcur, 0, 0);
    pold->copyPosAndBB(pcur->pos);
    break;

  default:	// other objects
    pold->copyPositionAndBB(pcur);
  }

  return true;
}

bool Gate::whenIntersectOut(WObject *pcur, WObject *pold)
{
  if (pcur->type == USER_TYPE) {
    cntcol = 0;
    return true;
  }
  return false;
}

void Gate::quit()
{
  clearObjectBar();
  oid = 0;
  savePersistency();
}

void Gate::enter_cb(Gate *gate, void *d, time_t s, time_t u)
{
  gate->enter();
}

void Gate::funcs()
{
  getPropertyFunc(GATE_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(GATE_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(GATE_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(GATE_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(GATE_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(GATE_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(GATE_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(GATE_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(GATE_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(GATE_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(GATE_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(GATE_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(GATE_TYPE, 0, _Action enter_cb, "Enter");
  setActionFunc(GATE_TYPE, 1, _Action gotoFront, "Approach");
  //setActionFunc(GATE_TYPE, 2, _Action moveObject, "Move");
}
