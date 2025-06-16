//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
// world.cpp
//
// World management
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "world.hpp"
#include "http.hpp"	// httpOpen httpRead
#include "user.hpp"	// USER_TYPE
#include "cache.hpp"	// path
#include "url.hpp"	// file2url
#include "universe.hpp"	// Universe
#include "gui.hpp"	// clearInfoBar
#include "netobj.hpp"	// NetObj
#include "channel.hpp"	// join
#include "solid.hpp"	// ~Solid
#include "gui.hpp"	// ::g.gui
#include "env.hpp"	// icons
#include "pref.hpp"	// url
#include "olist.hpp"	// OList
#include "file.hpp"	// open, close
#include "entry.hpp"	// Entry
#include "clock.hpp"	// Clock
#include "stat.hpp"	// new_world
#include "color.hpp"	// Color
#include "bgcolor.hpp"	// Bgcolor
#include "icon.hpp"	// ICON_TYPE
#include "ball.hpp"	// BALL_NAME
#include "thing.hpp"	// THING_NAME
#include "mirage.hpp"	// MIRAGE_NAME
#include "dress.hpp"	// DRESS_TYPE
#include "vjc.hpp"	// Vjc
#include "sql.hpp"	// VSql
#include "tool.hpp"	// quitTools
#include "sandbox.hpp"	// sandbox world
#include "wind.hpp"	// Wind
#include "bubble.hpp"	// Bubble
#include "axis.hpp"	// Axis
#include "grid.hpp"	// toggleGrid2d
#include "report.hpp"	// report

#include <list>


// local

/* max space reachable, even values */
const uint8_t World::GRIDX = 4;  // 30
const uint8_t World::GRIDY = 4;  // 20
const uint8_t World::GRIDZ = 2;  // 6
const float   World::DISTX = 2.;
const float   World::DISTY = 2.;
const float   World::DISTZ = 2.;
const uint8_t World::WORLD_LEN = 32;

class OList* World::gridArray[GRIDX][GRIDY][GRIDZ];

World* World::worldcurr = NULL;


/** World constructor */
World::World()
{
  new_world++;
  state = TOLOAD;
  num = 0;
  group = 0;
  linked = false;
  persistent = true;
  name = NULL;
  chan = NULL;
  grid = NULL;
  guip = NULL;
  bbcent = setV3(0, 0, 0);
  bbsize = setV3(0, 0, 0);
  bbmin = setV3(0, 0, 0);
  bbmax = setV3(0, 0, 0);
  bbslice = setV3(DISTX, DISTY, DISTZ);

  // interaction with general objects
  universe = Universe::current();
  bgcolor = NULL;
  clock = NULL;
  vjc = NULL;
  wind = NULL;
  ground = 0;

  if (universe->worldcnt == 0) {
    universe->worldcnt++;
    return;		// manager case, not in list
  }
  num = universe->worldcnt++;
  trace1(DBG_WO, "World: num=%d", num);

  // Adds world into world list
  if (! worldcurr) {	// first world encountered
    next = prev = NULL;
  }
  else if (worldcurr != this) {
    next = worldcurr;
    worldcurr->prev = this;
    prev = NULL;
  }
  worldcurr = this;
}

/** Gets current world */
World * World::current()
{
  return worldcurr;	// head of the worlds list
}

/** Sets local world name */
void World::setName(const char *urlOrName)
{
  // Find the begining of the last path component
  const char* begin = strrchr(urlOrName, '/');
  begin = (begin) ? begin+1 : urlOrName;

  // Find end of name by suppressing extention
  const char* end = strrchr(begin, '.');
  if (! end) end = begin + strlen(begin);

  // Copy the name
  if (name) delete name;
  uint8_t len = MIN(WORLD_LEN, end-begin);
  name = new char[len];
  memcpy(name, begin, len);
  name[len] = '\0';
}

/** Checks whether this url has been already loaded - static */
World * World::getWorld(const char *url)
{
  if (! url) return NULL;	// sandbox world

  char urla[URL_LEN] = {0};
  Url::abs(url, urla);

  for (World *w = worldcurr; w ; w = w->next) {
    if ((! strcmp(w->url, url)) || (! strcmp(w->url, urla))) {
      return w;			// world found
    }
  }
  return NULL;			// world not found
}

void World::joinManager(const char *chan)
{
  world_manager->chan = new char[strlen(chan) + 1];
  Channel::joinManager(world_manager->chan, chan);
}

/** Sets the channel name */
bool World::setChan(const char *chanstr)
{
  if (! chanstr) {
    return false;
  }
  if (strlen(chanstr) >= CHAN_LEN) {
    return false;
  }
  if (*chanstr == '\0') {
    chan = new char[strlen(::g.channel) + 1];
    memset(chan, 0, strlen(::g.channel)+1);
    strncpy(chan, ::g.channel, strlen(::g.channel));
  }
  else {
    chan = new char[strlen(chanstr) + 1];
    memset(chan, 0, strlen(chanstr)+1);
    strncpy(chan, chanstr, strlen(chanstr));
  }
  return true;
}

/** Sets the channel name and Joins the new channel */
void World::joinChan(char *chan)
{
  if (setChan(chan)) {
    Channel::join(chan);
  }
}

/** Sets the world's url */
void World::setUrl(const char* _url)
{
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
}

/** Sets the ground's level */
void World::setGround(float level)
{
  ground = level;
}

void World::setGroup(uint32_t _group)
{
  group = _group;
}

/**
 * Computes the World (Simulation) :
 * called by scene.cpp
 */
void World::compute(time_t sec, time_t usec)
{
  switch (state) {
  case LOADING:
    error("compute: no end of vre file encountered");
  case LOADED:
    trace1(DBG_WO, "compute: world loaded");
    if (localuser) {
      localuser->move.perm_sec = sec;
      localuser->move.perm_usec = usec;
    }
    // computes world's bounding box
    for (std::vector<Object*>::iterator it = stillList.begin(); it != stillList.end(); ++it) {
      for (int i=0; i<3 ; i++) {
        bbmin.v[i] = MIN(bbmin.v[i], (*it)->pos.bbc.v[i] - (*it)->pos.bbs.v[i]);
        bbmax.v[i] = MAX(bbmax.v[i], (*it)->pos.bbc.v[i] + (*it)->pos.bbs.v[i]);
      }
    }
    for (std::list<Object*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
      for (int i=0; i<3 ; i++) {
        bbmin.v[i] = MIN(bbmin.v[i], (*it)->pos.bbc.v[i] - (*it)->pos.bbs.v[i]);
        bbmax.v[i] = MAX(bbmax.v[i], (*it)->pos.bbc.v[i] + (*it)->pos.bbs.v[i]);
      }
    }
    for (int i=0; i<3 ; i++) {
      bbcent.v[i] = (bbmax.v[i] + bbmin.v[i]);
      bbsize.v[i] = (bbmax.v[i] - bbmin.v[i]);
    }
    Axis::axis()->init();
    Grid::grid()->init(bbsize.v[0], bbsize.v[1], bbsize.v[2]);
    state = SIMULATION;
  case SIMULATION:				// do movements
    // user movements
    if (localuser) {
      localuser->userMovements(sec, usec);	// localuser movement
    }
    // removes objects scheduled to be deleted
    deleteObjects();
    // objects movements with imposed or permanent movements
    for (std::list<Object*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
      (*it)->imposedMovements(sec, usec);	// object with imposed movement
      (*it)->permanentMovements(sec, usec);	// object with permanent movement
    }
  }
  return;
}

/** Calls a world - private */
bool World::call(World *world)
{
  if (world->linked) {
    enter(url, NULL, OLD_WORLD);
    setChan(world->chan);
    return true;
  }
  trace1(DBG_IPMC, "call: leave chan=%s", world->chan);
  if (Channel::current()) {
    delete Channel::current();		// leave current channel
  }

  enter(url, NULL, OLD_WORLD);		// enter in this world

  char grp[GROUP_LEN];
  Channel::getGroup(chan, grp);
  group = inet_addr(grp);

  trace1(DBG_IPMC, "call: join chan=%s", chan);
  if (Channel::join(chan) == 0) {	// join previous channel
    trace1(DBG_IPMC, "call: can't join chan=%s", chan);
    return false;			// fail
  }
  setChan(chan);
  ::g.gui.updateWorld(this, NEW_WORLD);	// nofify the gui
  return true;				// success
}

/** Go to the previous world - static */
World * World::goPrev()
{
  World *worldback = worldcurr->next;
  if (! worldback) return NULL;	// no prev world
  if (::g.pref.trace) echo("curr: %s", worldcurr->name);
  if (::g.pref.trace) echo("back: %s", worldback->name);

  World *world = worldcurr;
  //if (world->quit() != NULL)
    world->quit();		// quit current world : FIXME segfault !!!
  //else
  //  return world;

  World *w;
  for (w = worldback; w->next ; w = w->next) {
    if (w == w->next) {
      break;			// found
    }
  }
  w->next = world;
  world->next = NULL;
  worldback->prev = NULL;
  world->prev = worldback;
  worldcurr = worldback;
  if (worldback->call(world)) {
    return worldcurr;
  }
  return world;
}

/** Go to the next world - static */
World * World::goNext()
{
  if (! worldcurr->next) return NULL;	// no forward world

  World *world = worldcurr;
  world->quit();			// quit current world first
  //return world;

  World *w;
  World *worldnext;
  for (w = world; (worldnext = w->next)->next; w = w->next) {
    ;
  }
  worldnext->next = world;
  worldnext->prev = NULL;
  world->prev = worldnext;
  w->next = NULL;
  worldcurr = worldnext;
  if (worldnext->call(world)) {
    return worldcurr;
  }
  return world;
}

/** Exchanges worlds in the list - static */
World * World::swap(World *world)
{
  if (worldcurr == world) return worldcurr;	// same world

  if (world->prev)
    world->prev->next = worldcurr;	// 1
  if (world->next)
    world->next->prev = worldcurr;	// 2
  if (worldcurr->next)
    worldcurr->next->prev = world;	// 3
  worldcurr->prev = world->prev;	// 4
  World *wtmp = worldcurr->next;
  worldcurr->next = world->next;	// 5
  world->next = wtmp;			// 6
  world->prev = NULL;			// 7
  worldcurr = world;			// 8
  return worldcurr;
}

/** Reads a world - static */
void World::reader(void *_url, Http *http)
{
  char *url = static_cast<char *> (_url);
  if (! http) {
    error("can't download %s, check access to the remote http server", url);
  }

  int len = 0;
  char *cachename = new char[PATH_LEN];
  char buf[1024];
  File *fileout = new File();
  File *filein = new File();
  FILE *fpcache = NULL;
  struct stat bufstat;

  Parse *parser = new Parse();		// creates the parser instance

  *cachename = 0;
  Cache::path(url, cachename);
  if (stat(cachename, &bufstat) < 0) {	// is not in the cache
    if (! (fpcache = fileout->open(cachename, "w"))) {
      error("worldReader: can't create file %s from url %s", cachename, url);
    }

    // download the vre file from the httpd server
httpread:
    while ((len = http->httpRead(buf, sizeof(buf))) > 0) {
      if (fpcache) {
        fwrite(buf, 1, len, fpcache);	// save into the cache
      }
      if (parser->parseVreFile(buf, len) <= 0) {
        break;				// eof
      }
    }
    fileout->close();
    delete fileout;

#if 0 //HAVE_LIBXML2
    Xml::dtdValidation(cachename);	// check the DTD
#endif //HAVE_LIBXML2
  }
  else {        			// cachename exists in the cache
    if (! (fpcache = filein->open(cachename, "r"))) {
      goto httpread;			// if can't open download it by http
    }
    while ((len = fread(buf, 1, sizeof(buf), fpcache)) > 0) {
      if (parser->parseVreFile(buf, len) <= 0) {
        break;				// eof
      }
    }
  }
  if (fpcache) {
    filein->close();
    delete filein;
  }
  delete[] cachename;
  return;
}

/** General World Initialization - static */
void World::init(const char *url)
{
  //
  // Creates initial world (Rendezvous)
  //
  World *world = new World();

  world->state = LOADING;
  world->setUrl(url);
  world->setName(url);
  world->joinChan(::g.channel);		// join initial channel
  Channel::getGroup(world->chan, Universe::current()->grpstr);
  Universe::current()->port = Channel::getPort(world->chan);

  //report(world->name);
  world->guip = ::g.gui.addWorld(world, NEW_WORLD);
  world->initGrid();
  world->clearObjects();
  Object::initNames();
  initFunc();				// init funcs (objects.cpp)

  if (::g.pref.keep == false) {
    ::g.env.cleanCacheByExt("vre");	// remove *.vre in the cache
  }

  world->clock = new Clock();
  world->bgcolor = new Bgcolor();

  //
  // Creates localuser first
  //
  User *user = new User();
  world->user = user;
  Universe::current()->localuser = user;	// keep user in this universe

  //
  // Download initial world (Rendezvous.vre by default)
  //
  world->universe->startWheel();
  Http::httpOpen(url, reader, (void *) (url), 0);
  world->universe->stopWheel();
  endprogression();

  // check whether icons are locally presents
  world->checkIcons();
  // check whether other objects are persistents
  world->checkPersist();

  if (! ::g.pref.gravity) ::g.gui.pauseAvatar();

  world->state = LOADED;
  trace1(DBG_INIT, "World %s initialized", world->name);

  Entry *entry = new Entry();
  entry->query(user);
  localuser->setPosition();

  // attach bubble welcome text to localuser
  char welcome[32];
  sprintf(welcome, "Hi! I am %s", ::g.user);
  user->bubble = new Bubble(user, welcome, Color::red, Bubble::BUBBLEVERSO);
}

/** Quits the current World */
void World::quit()
{
  //trace1(DBG_WO, "quit %s", name);
  state = STOPPED;

  //
  // Deletes objects
  //

  // invisible objects
  for (std::vector<Object*>::iterator it = invisList.begin(); it != invisList.end(); ++it) {
    if ((*it)->deleted) continue;
    delete *it;
  }
  invisList.clear();

  // still objects
  for (std::vector<Object*>::iterator it = stillList.begin(); it != stillList.end(); ++it) {
    if ((*it)->deleted) continue;
    delete *it;
  }
  stillList.clear();

  // mobile objects
  for (std::list<Object*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    if ((*it) == localuser) continue;	// FIX segfault
    if ((*it)->deleted) continue;
    delete *it;
  }
  mobileList.clear();

  // fluid objects
  for (std::vector<Object*>::iterator it = fluidList.begin(); it != fluidList.end(); ++it) {
    if ((*it)->deleted) continue;
    delete *it;
  }
  fluidList.clear();

  // cloth objects
  for (std::vector<Object*>::iterator it = clothList.begin(); it != clothList.end(); ++it) {
    if ((*it)->deleted) continue;
    delete *it;				// sometimes segfault FIXME!!!
  }
  clothList.clear();

  // Update GUI
  if (guip) {
    ::g.gui.updateWorld(this, OLD_WORLD);
  }
  ::g.gui.showNavigator();		// force navig mode

  if (localuser && localuser->netop) {
    localuser->netop->declareDeletion();
    localuser->netop->deleteFromList();	// publishes I leave
  }

  // reset user position
  if (localuser) localuser->resetPosition();
  if (linked) return;

  Tool::quitTools();			// quits all tools
}

/** Enters in a new World - static */
World * World::enter(const char *url, const char *chan, bool isnew)
{
  //trace1(DBG_FORCE, "world enter");

  // clears everything
  ::g.gui.clearInfoBar();
  Object::initNames();
  current()->clearObjects();
  current()->initGrid();
  World *world = NULL;

  // checks whether this world is already in memory
  if (getWorld(url) && isnew) {
    world = getWorld(url);	// existing world
    worldcurr = swap(world);
    if (::g.pref.trace) echo("enter: %s", world->name);
    ::g.gui.updateWorld(world, NEW_WORLD);
  }
  else if (isnew) {
    // new world must be initialized
    World *newworld = new World();

    if (! url) {		// sandbox world without url
      ::g.gui.updateWorld(newworld, NEW_WORLD);
    }
    else if (isprint(*url)) {
      newworld->url = new char[strlen(url) + 1];
      strcpy(newworld->url, url);
      newworld->setName(newworld->url);
    }
    if (chan) {			// not a world link
      newworld->setChan(chan);
    }
    newworld->guip = ::g.gui.addWorld(world, NEW_WORLD);
    world = newworld;
  }
  else {			// world already exists
    world = current();
    ::g.gui.updateWorld(world, OLD_WORLD);
  }

  // default bgcolor
  world->bgcolor = new Bgcolor();

  //
  // downloads the vre description file of the new world
  //
  world->state = LOADING;	// need to download
  if (url) {			// world to download
    //trace1(DBG_FORCE, "enter: download %s", url);
    world->universe->startWheel();
    if (Http::httpOpen(url, reader, (void *) (url), 0) < 0) {
      error("enter: bad download: %s", url);
      return NULL;
    }
    world->universe->stopWheel();
    endprogression();
    localuser->enableGravity();
    Grid::grid()->reset();
    Axis::axis()->reset();
  }
  else {			// world sandbox
    // sandbox world
    World *sandbox = world;
    sandbox->setName("sandbox");

    Parse *parser = Parse::getParse();
    parser->parseVreFile(sandbox_vre, sizeof(sandbox_vre));
    sandbox->linked = true;

    float gridcolor[4] = { 0,1,0,.5 };	// green grid
    Grid::grid()->toggleGrid2d();
    Grid::grid()->setColor(gridcolor);
    Grid::grid()->setWidthIncr(16);
    Grid::grid()->setDepthIncr(16);
    Axis::axis()->toggle();
  }
  localuser->inits();

  // default entry
  new Entry();

  // attachs bubble hello text to localuser
  char hello[32];
  sprintf(hello, "Hello! I am %s", ::g.user);
  localuser->bubble = new Bubble(localuser, hello, Color::black, Bubble::BUBBLEVERSO);

  // checks whether icons are locally presents
  world->checkIcons();
  // check whether other objects are persistents by VSql
  world->checkPersist();

  // creates clock
  world->clock = new Clock();	// internal clock

  //trace1(DBG_FORCE, "enter: %s loaded", world->name);
  world->state = LOADED;	// downloaded
  return world;
}

/** Deletes all objects dropped in the deleteList - static */
void World::deleteObjects()
{
  for (std::vector<Object*>::iterator it = deleteList.begin(); it != deleteList.end(); ++it) {
    if (! (*it)->isBehavior(COLLIDE_NEVER)) {
      (*it)->delGrid();
    }
    if ((*it)->removed) {
      mobileList.remove(*it);
      if (::g.pref.trace) echo("delete object: %s", (*it)->objectName());
      if ((*it)->deleted) continue;
      if ( strcmp((*it)->typeName(), "Dart") && strcmp((*it)->typeName(), "Bullet") ) // Hack! FIXME!
        delete (*it);	//segfault FIXME!
    }
  }
  deleteList.clear();
}

/** Clears all lists */
void World::clearObjects()
{
  objectList.clear();
  mobileList.clear();
  stillList.clear();
  fluidList.clear();
  clothList.clear();
  invisList.clear();
  deleteList.clear();
  lightList.clear();
}

/**
 * Vicinity Grid
 */
void World::initGrid()
{ 
  dimgrid[0] = GRIDX;
  dimgrid[1] = GRIDY;
  dimgrid[2] = GRIDZ;
  bbslice.v[0] = DISTX;
  bbslice.v[1] = DISTY;
  bbslice.v[2] = DISTZ;

  localGrid();

  // Clears all pointers in the grid
  for (int x=0; x < dimgrid[0]; x++) {
    for (int y=0; y < dimgrid[1]; y++) {
      for (int z=0; z < dimgrid[2]; z++) {
        gridArray[x][y][z] = NULL;
      }
    }
  }
}

/** Checks and load my proper icons - static */
void World::checkIcons()
{
  chdir(::g.env.icons());
  DIR *dirw = opendir(".");
  if (dirw) {
    // check if current world is there
    for (struct dirent *dw = readdir(dirw); dw; dw = readdir(dirw)) {
      struct stat bufstat;
      if (stat(dw->d_name, &bufstat) == 0 &&
          S_ISDIR(bufstat.st_mode) &&
          ! strcmp(dw->d_name, name)) {
        chdir(dw->d_name);
        DIR *diri = opendir(".");
        if (diri) {
          // checks icons in this world
          for (struct dirent *di = readdir(diri); di; di = readdir(diri)) {
            if (stat(di->d_name, &bufstat) == 0 && S_ISREG(bufstat.st_mode)) {
              // open the icon and read it
              FILE *fp;
              File *file = new File();
              if (! (fp = file->open(di->d_name, "r"))) {
                error("can't open %s/%s/%s", ::g.env.icons(), name, di->d_name);
                continue;
              }
              char vref[BUFSIZ], infos[BUFSIZ *2], url[URL_LEN];
              fgets(vref, sizeof(vref), fp);
              file->close();

              // create the icon
              Url::file2url(di->d_name, url);
              sprintf(infos, "<url=\"%s\">&<vref=%s>", url, vref);
              //echo("load-icon: %s", infos);
              if (isAction(ICON_TYPE, Icon::CREATE)) {
                doAction(ICON_TYPE, Icon::CREATE, localuser, infos, 0, 0);
              }
            }
          }
          closedir(diri);
        }
        break;
      }
    }
    closedir(dirw);
  }
  chdir(::g.env.cwd());
}

/** Checks whether other objects are persistents */
void World::checkPersist()
{
  VSql *vsql = new VSql();     // first take the VSql handle;
  if (vsql) {
    int nitem = 0;
    char pat[64], name[128];

    // check balls
    nitem = vsql->getCount(BALL_NAME, name);	// balls in VSql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", name);
      if (vsql->getName(BALL_NAME, pat, i, name) >= 0) {
        char *p = strchr(name, '@');
        if (p) {
          *p = '\0';
          doAction(BALL_TYPE, Ball::RECREATE, (Object*)this, (void*)name,0,0);
        }
      }
    }
    // check things
    nitem = vsql->getCount(THING_NAME, name);	// things in VSql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", name);
      if (vsql->getName(THING_NAME, pat, i, name) >= 0) {
        char *p = strchr(name, '@');
        if (p) {
          *p = '\0';
          doAction(THING_TYPE, Thing::RECREATE, (Object*)this, (void*)name,0,0);
        }
      }
    }
    // check mirages
    nitem = vsql->getCount(MIRAGE_NAME, name);	// mirages in VSql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", name);
      if (vsql->getName(MIRAGE_NAME, pat, i, name) >= 0) {
        char *p = strchr(name, '@');
        if (p) {
          *p = '\0';
          doAction(MIRAGE_TYPE, Mirage::RECREATE, (Object*)this, (void*)name,0,0);
        }
      }
    }
  }
}
