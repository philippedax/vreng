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
#include "universe.hpp"	// universe
#include "move.hpp"	// gotoFront
#include "user.hpp"	// USER_TYPE, localuser
#include "ball.hpp"	// BALL_TYPE
#include "dart.hpp"	// DART_TYPE
#include "bullet.hpp"	// BULLET_TYPE
#include "guide.hpp"	// GUIDE_TYPE
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
  autoEnter = false;
  link = false;
  flagEntry = false;
  memset(entry, 0, sizeof(entry));
}

void Gate::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "url"))     l = parse()->parseUrl(l, names.url);
    else if (! stringcmp(l, "world"))   l = parse()->parseString(l, names.url, "world");
    else if (! stringcmp(l, "channel")) l = parse()->parseString(l, chan, "channel");
    else if (! stringcmp(l, "mode")) {
      char modestr[6];
      l = parse()->parseString(l, modestr, "mode");
      if      (! stringcmp(modestr, "link")) link = true;
      else if (! stringcmp(modestr, "auto")) autoEnter = true;
    }
    else if (! stringcmp(l, "entry")) {
      l = parse()->parseVector3f(l, entry, "entry");
      flagEntry = true;
      trace(DBG_WO, "gate: entry=%.2f,%.2f,%.2f", entry[0], entry[1], entry[2]);
    }
  }
  end_while_parse(l);
}

void Gate::behavior()
{
  enableBehavior(PERSISTENT);
  enableBehavior(COLLIDE_ONCE);
  setRenderPrior(RENDER_NORMAL);

  initializeMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

Gate::Gate(char *l)
{
  parser(l);
  behavior();
}

Gate::Gate(WObject *user, char *geom)
{
  defaults();

  // user position
  pos.x = user->pos.x + 0.5;
  pos.y = user->pos.y;
  pos.z = user->pos.z + 0.5;
  parse()->parseSolid(geom, SEP, this);

  behavior();
  updatePosition();
}

bool Gate::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

void Gate::enter()
{
  /* save url and channel, because World::quit frees gate */
  char *new_url = strdup(names.url);
  if (!new_url) { error("enter: can't strdup url=%s", new_url); return; }

  if (link) {	// without channel
    World::current()->quit();
    World::enter(new_url, NULL, World::NEW);
    World::current()->linked();	// linked world
  }
  else {		// with channel
    if (strcmp(names.url, Universe::current()->url) == 0) {
      sprintf(chan, "%s/%u/%d",
              Universe::current()->group, Universe::current()->port, Channel::currentTtl());
      trace(DBG_IPMC, "initial channel = %s", chan);
    }

    //
    // call here the VACS (VREng Address Cache Server) to get the channel string
    //
#if 0 //dax
    Vac *vac = Vac::current();
    if (! vac->getChannel(names.url, chan)) {
      // this url is not in the cache, we need to ask to the vacs to resolve it
      if (vac->resolveWorldUrl(names.url, chan)) {
        trace(DBG_FORCE, "enter: resolveWorldUrl url=%s channel=%s", names.url, chan);
      }
      else {
        warning("enter: warning resolveWorldUrl failed from Vac: url=%s", names.url);
        if (! *chan)
          strcpy(chan, DEF_VRE_CHANNEL);  // no given channel, forced to the default
      }
    }
#endif //dax
    trace(DBG_IPMC, "enter: getChannel=%s url=%s", chan, new_url);
    char *new_chan_str = strdup(chan);

    World::current()->quit();		// quit the current world
    delete Channel::current();		// delete Channel
#if 0 //no sound
    Sound::playSound(GATESND);
#endif
    World::enter(new_url, new_chan_str, World::NEW);

    char *chanstr = strdup(new_chan_str);
    Channel::join(chanstr);
    free(chanstr);
    trace(DBG_IPMC, "enter: join channel=%s url=%s", new_chan_str, new_url);

    //TODO declareJoinWorldToManager(new_url, new_chan_str, worlds->plocaluser->getInstance());

    if (audioactive) Audio::start(new_chan_str);
    free(new_chan_str);
  }
  free(new_url);
}

/* when an intersection occurs */
bool Gate::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case USER_TYPE:
    if (autoEnter) {
      if (pcur != localuser) {
	notice("Another user was just pushed out of this world"); return true;
      }
      else {
        collideCnt = 0;
        if (flagEntry) {
          flagEntry = false;
          pcur->updatePositionAndGrid(pcur->pos);
        }
        enter();
      }
    }
    else {
      if (collideCnt < 20) {
        pold->copyPositionAndBB(pcur);
        notice("Warning! You are near the gate %s in=%d", getInstance(), collideCnt);
        collideCnt++;
      }
      else if (collideCnt < 40) {
        pold->copyPositionAndBB(pcur);
        notice("Warning! If you insist you'll enter in %s in=%d", getInstance(), collideCnt);
        collideCnt++;
      }
      else {
        enter();
        collideCnt = 0;
      }
    }
    break;
  case BULLET_TYPE:
  case DART_TYPE:
    notice("%s:%s hits %s:%s", pcur->names.type, pcur->getInstance(), names.type, getInstance());
    pold->copyPositionAndBB(pcur);
    pcur->toDelete();
    break;
  case BALL_TYPE:
    projectPosition(pcur, pold);
    break;
  case GUIDE_TYPE:
    return false;
  default:	// other objects
    pold->copyPositionAndBB(pcur);
  }
  return true;
}

bool Gate::whenIntersectOut(WObject *pcur, WObject *pold)
{
  if (pcur->type == USER_TYPE) {
    collideCnt = 0;
    return true;
  }
  return false;
}

void Gate::quit()
{
  clearObjectBar();
  oid = 0;
  flushMySqlPosition();
}

void Gate::enter_cb(Gate *gate, void *d, time_t s, time_t u)
{
  gate->enter();
}

void Gate::funcs()
{
  getPropertyFunc(GATE_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(GATE_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(GATE_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(GATE_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(GATE_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(GATE_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(GATE_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(GATE_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(GATE_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(GATE_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(GATE_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(GATE_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(GATE_TYPE, 0, WO_ACTION enter_cb, "Enter");
  setActionFunc(GATE_TYPE, 1, WO_ACTION gotoFront, "Approach");
  setActionFunc(GATE_TYPE, 2, WO_ACTION moveObject, "Move");
}
