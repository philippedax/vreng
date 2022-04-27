//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "universe.hpp"
#include "world.hpp"	// World
#include "wobject.hpp"	// WObject
#include "channel.hpp"	// getTtl
#include "user.hpp"	// User
#include "pref.hpp"	// url
#include "wheel.hpp"	// Wheel
#include "scene.hpp"	// GLSection
#include "prof.hpp"	// new_world


// global
World *world_manager = NULL;


Universe::Universe()
{
  localuser = NULL;
  port = DEF_VRENG_PORT;
  version = VRE_VERSION;
  worldcnt = 0;

  url = new char[strlen(::g.url) + 1];
  strcpy(url, ::g.url);

  if (! ::g.pref.new_universe) { // by default
    server = new char[sizeof(DEF_HTTP_SERVER) + 1];
    strcpy(server, DEF_HTTP_SERVER);
    urlpfx = new char[sizeof(DEF_URL_PFX) + 1];
    strcpy(urlpfx, DEF_URL_PFX);
  }
  else {  // universe given by -u
    char tmp[64];
    char *p, *ptmp, *pserv;
    server = new char[strlen(::g.universe) + 1];
    strcpy(tmp, ::g.universe);
    p = strchr(tmp, '/');
    p++;
    ptmp = strchr(p, '/');
    pserv = ++p;
    p = strchr(p, '/');
    *p = '\0';
    strcpy(server, pserv);
  
    ++p;
    urlpfx = new char[1];
    ptmp = strrchr(p, '/');
    if (ptmp) *ptmp = '\0';
    sprintf(urlpfx, "/%s", p);
  }
  group = new char[GROUP_LEN + 1];
  Channel::getGroup(DEF_MANAGER_CHANNEL, group);
  ttl = Channel::getTtl(::g.channel);
  wheel = new Wheel();
  trace(DBG_INIT,"Universe: universe=%s server=%s pfx=%s", ::g.universe, server, urlpfx);
  notice("universe=%s server=%s pfx=%s", ::g.universe, server, urlpfx);
}

Universe::~Universe()
{
  if (server) delete server;
  if (url) delete url;
  if (urlpfx) delete urlpfx;
  if (group) delete group;
  if (wheel) delete wheel;
}

Universe* Universe::current()
{
  static Universe* defUniverse = NULL;

  if (defUniverse == NULL) {
    defUniverse = new Universe();
  }
  return defUniverse;
}

void Universe::getUrl(char *_url)
{
  strncpy(_url, url, URL_LEN);
}

/** Initialization from vreng.cc - static */
void Universe::init()
{
  world_manager = new World();
  world_manager->setManagerChanAndJoin(DEF_MANAGER_CHANNEL); // join world_manager chan
  world_manager->setName(MANAGER_NAME);
}

static bool univ_progress = false;

void Universe::sigWheel(int sig)
{
  progression('!');
  univ_progress = false;
}

void Universe::startWheel()
{
  signal(SIGTERM, sigWheel);
  pthread_create(&wheel_tid, NULL, runWheel, (void *) NULL);
}

void * Universe::runWheel(void * arg)
{
  univ_progress = true;
  progression('[');
  //CRASH ubit GLSection gls(::g.gui.scene());
#if 0 //dax
  while (univ_progress) {
    //progression('.');
    //::g.render.wheel();	//CRASH
    if (new_world > 1) Wheel::current()->render();	//CRASH machine
    //signal(SIGTERM, SIG_IGN);
    usleep(100000/12);		// a tour of dial in 1 sec
    //signal(SIGTERM, sigWheel);
  }
#endif
  return NULL;
}

void Universe::stopWheel()
{
  progression(']');
#if HAVE_LIBPTHREAD
  pthread_kill(wheel_tid, SIGTERM);
  signal(SIGTERM, SIG_IGN);
#endif
  univ_progress = false;
}
