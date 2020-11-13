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
World *manager = NULL;


Universe::Universe()
{
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
    //error("server=%s", server);
    
    ++p;
    urlpfx = new char[1];
    ptmp = strrchr(p, '/');
    if (ptmp) *ptmp = '\0';
    sprintf(urlpfx, "/%s", p);
  }

  group = new char[GROUP_LEN + 1];
  Channel::getGroup(DEF_MANAGER_CHANNEL, group);

  worldList = NULL;
  worldcnt = 0;
  localuser = NULL;
  ttl = Channel::getTtl(::g.channel);
  port = VRENG_MPORT_BASE;
  version = VRE_VERSION;
  prop = 0;
  wheel = NULL;

  wheel = new Wheel();
  trace(DBG_INIT,"Universe: url=%s server=%s pfx=%s", url, server, urlpfx);
  notice("Universe: url=%s server=%s pfx=%s", url, server, urlpfx);
}

Universe::~Universe()
{
  delete server;
  delete url;
  delete urlpfx;
  delete worldList;
  delete group;
  delete localuser;
  if (wheel) delete wheel;
}

Universe* Universe::current()
{
  static Universe* sDefaultUniverse = NULL;

  if (sDefaultUniverse == NULL)
    sDefaultUniverse = new Universe();
  return sDefaultUniverse;
}

void Universe::getUrl(char *_url)
{
  strncpy(_url, url, URL_LEN);
}

/** Initialization from vreng.cc - static */
void Universe::init()
{
  manager = new World();
  manager->setManagerChanAndJoin(DEF_MANAGER_CHANNEL); // join manager chan
  manager->setName(MANAGER_NAME);
}

static bool progress = false;

void Universe::sigWheel(int sig)
{
  progression('!');
  progress = false;
}

void Universe::startWheel()
{
  //dax GLSection gls(::g.gui.getScene());
#if HAVE_LIBPTHREAD
  signal(SIGTERM, sigWheel);
  pthread_create(&wheel_tid, NULL, runWheel, (void *) NULL);
#endif
}

void * Universe::runWheel(void * arg)
{
  progress = true;
  progression('[');
  //CRASH ubit GLSection gls(::g.gui.getScene());
#if 0 //dax
  while (progress) {
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
  progress = false;
}
