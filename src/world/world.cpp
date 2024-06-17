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
#include "wobject.hpp"	// WO
#include "http.hpp"	// httpOpen httpRead
#include "user.hpp"	// USER_TYPE
#include "cache.hpp"	// setCachePath
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
using namespace std;


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

World* World::worldList = NULL;


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

  addList();
}

/** Adds world into world list */
void World::addList()
{
  if (!worldList) {	// first world encountered
    next = prev = NULL;
  }
  else if (worldList != this) {
    next = worldList;
    worldList->prev = this;
    prev = NULL;
  }
  worldList = this;
  //dumpworldList("debug");
}

/** Gets current world */
World * World::current()
{
  return worldList;	// head of the worlds list
}

/** Gets the gui pointer */
struct GuiItem* World::getGui() const
{
  return guip;
}

/** Is gui valid ? */
bool World::isGui() const
{
  return (guip) ? true : false;
}

void World::resetGui()
{
  guip = NULL;
}

/** Sets local world name */
void World::setName(const char *urlOrName)
{
  // Find the begining of the last path component
  const char* begin = strrchr(urlOrName, '/');
  begin = (begin != NULL) ? begin+1 : urlOrName;

  // Find end of name by suppressing extention
  const char* end = strrchr(begin, '.');
  if (end == NULL) end = begin + strlen(begin);

  // Copy the name
  if (name) delete name;
  uint8_t len = MIN(WORLD_LEN, end-begin);
  name = new char[len];
  memcpy(name, begin, len);
  name[len] = '\0';
}

/** Gets current world name */
const char* World::getName() const
{
  return name;
}

/** Checks whether this url has been already loaded - static */
World * World::find(const char *url)
{
  if (! url) return NULL;	// sandbox world

  char urla[URL_LEN] = {0};
  Url::abs(url, urla);

  for (World *w = worldList; w ; w = w->next) {
    if ((! strcmp(w->url, url)) || (! strcmp(w->url, urla))) {
      return w;	// world found
    }
    if (w == w->next) {
      break;	//FIXME: bug inside the list
    }
  }
  return NULL;	// world not found
}

/** Gets a world by its group number */
World * World::find(uint32_t group)
{
  for (World *w = worldList; w ; w = w->next) {
    if (w->group == group) {
      return w;	// world found
    }
  }
  return NULL;
}

uint8_t World::getState() const
{
  return state;
}

void World::setState(int _state)
{
  state = _state;
}

bool World::isDead() const
{
  return (state == STOPPED);
}

void World::setManagerChanAndJoin(const char *chan_str)
{
  world_manager->chan = new char[strlen(chan_str) + 1];
  Channel::joinManager(world_manager->chan, chan_str);
}

/** Sets the channel name */
bool World::setChan(const char *chan_str)
{
  if (! chan_str) {
    return false;
  }
  if (strlen(chan_str) >= CHAN_LEN) {
    return false;
  }

  if (*chan_str == '\0') {
    chan = new char[strlen(::g.channel) + 1];
    memset(chan, 0, strlen(::g.channel)+1);
    strncpy(chan, ::g.channel, strlen(::g.channel));
  }
  else {
    chan = new char[strlen(chan_str) + 1];
    memset(chan, 0, strlen(chan_str)+1);
    strncpy(chan, chan_str, strlen(chan_str));
  }
  return true;
}

/** Sets the channel name and Joins the new channel */
void World::setChanAndJoin(char *chan_str)
{
  if (setChan(chan_str)) {
    Channel::join(chan_str);
  }
}

/** Gets the current channel string */
const char* World::getChan() const	//FIXME (char *chan)
{
  return chan;
}

/** Gets localuser */
User* World::localUser() const
{
  return localuser;
}

/** Gets background color */
Bgcolor* World::backgroundColor() const
{
  return bgcolor;
}

/** Gets the world's url */
const char* World::getUrl() const
{
  return url;
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

float World::getGround() const
{
  return ground;
}

bool World::isLinked() const
{
  return linked;
}

void World::setLinked()
{
  linked = true;
}

void World::setSsrc(uint32_t _ssrc)
{
  ssrc = _ssrc;
}

uint32_t World::getGroupAdr() const
{
  return group;
}

void World::setGroupAdr(uint32_t _group)
{
  group = _group;
}

/**
 * Computes the World (Simulation) :
 * called by scene.cpp
 */
void World::compute(time_t sec, time_t usec)
{
  switch (state = getState()) {

  case LOADING:
    error("compute: no end of vre file encountered");
    return;

  case LOADED:
    trace1(DBG_WO, "compute: world loaded");
    if (localuser) {
      localuser->move.perm_sec = sec;
      localuser->move.perm_usec = usec;
    }

    //
    // computes world's bounding box
    //
    for (vector<WO*>::iterator it = stillList.begin(); it != stillList.end(); ++it) {
      for (int i=0; i<3 ; i++) {
        bbmin.v[i] = MIN(bbmin.v[i], (*it)->pos.bbc.v[i] - (*it)->pos.bbs.v[i]);
        bbmax.v[i] = MAX(bbmax.v[i], (*it)->pos.bbc.v[i] + (*it)->pos.bbs.v[i]);
      }
    }
    for (list<WO*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
      for (int i=0; i<3 ; i++) {
        bbmin.v[i] = MIN(bbmin.v[i], (*it)->pos.bbc.v[i] - (*it)->pos.bbs.v[i]);
        bbmax.v[i] = MAX(bbmax.v[i], (*it)->pos.bbc.v[i] + (*it)->pos.bbs.v[i]);
      }
    }
    for (int i=0; i<3 ; i++) {
      bbcent.v[i] = (bbmax.v[i] + bbmin.v[i]);
      bbsize.v[i] = (bbmax.v[i] - bbmin.v[i]);
    }
    //echo("bbs=%.1f,%.1f,%.1f bbc=%.1f,%.1f,%.1f", bbsize.v[0], bbsize.v[1], bbsize.v[2], bbcent.v[0], bbcent.v[1], bbcent.v[2]);

    //dax OList::clearIspointed(mobileList);
    setState(SIMULATION);
    Axis::axis()->init();
    Grid::grid()->init(bbsize.v[0], bbsize.v[1], bbsize.v[2]);
    return;

  case SIMULATION:	// do movements
    //
    // user movement
    //
    if (localuser) {
      localuser->userMovement(sec, usec);	// localuser movement
    }

    //
    // Removes objects scheduled to be deleted
    //
    deleteObjects();

    //
    // objects movement with imposed or permanent movements
    //
    for (list<WO*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
      (*it)->imposedMovement(sec, usec);	// object with imposed movement
      (*it)->permanentMovement(sec, usec);	// object with permanent movement
    }

    break;

  default: return;
  }
}

/** Calls a world - private */
bool World::call(World *w)
{
  if (w->linked) {
    enter(url, NULL, OLD);
    setChan(w->chan);
  }
  else {
    trace1(DBG_IPMC, "call: leave chan=%s", w->chan);
    if (Channel::current()) {
      delete Channel::current();	// leave current channel
    }

    enter(url, NULL, OLD);		// enter in this world

    char grpstr[GROUP_LEN];
    Channel::getGroup(chan, grpstr);
    group = inet_addr(grpstr);

    trace1(DBG_IPMC, "call: join chan=%s", chan);
    if (Channel::join(chan) == 0) {	// join previous channel
      trace1(DBG_IPMC, "call: can't join chan=%s", chan);
      return false;	// fail
    }
    setChan(chan);
    ::g.gui.updateWorld(this, NEW);	// nofify the gui
  }
  return true;		// success
}

/** Go to the previous world - static */
World * World::goPrev()
{
  World *worldback = worldList->next;
  if (! worldback) return NULL;	// no prev world

  World *world = worldList;
  world->quit();		// quit current world first

  World *wp;
  for (wp = worldback; wp->next ; wp = wp->next) {
    if (wp == wp->next) {
      break;	// found
    }
  }
  wp->next = world;
  world->next = NULL;
  worldback->prev = NULL;
  world->prev = worldback;
  worldList = worldback;

  if (worldback->call(world)) {
    return worldList;
  }
  return NULL;
}

/** Go to the next world - static */
World * World::goNext()
{
  if (! worldList->next) return NULL;	// no forward world

  World *world = worldList;
  world->quit();	// quit current world first

  World *wp;
  World *worldnext;
  for (wp = world; (worldnext = wp->next)->next; wp = wp->next) {
    ;
  }
  worldnext->next = world;
  worldnext->prev = NULL;
  world->prev = worldnext;
  wp->next = NULL;
  worldList = worldnext;

  if (worldnext->call(world)) {
    return worldList;
  }
  return NULL;
}

/** Exchanges worlds in the list - static */
World * World::swap(World *w)
{
  if (worldList == w) return worldList;	// same world

  if (w->prev)
    w->prev->next = worldList;	// 1
  if (w->next)
    w->next->prev = worldList;	// 2
  if (worldList->next)
    worldList->next->prev = w;	// 3
  worldList->prev = w->prev;	// 4
  World *wtmp = worldList->next;
  worldList->next = w->next;	// 5
  w->next = wtmp;		// 6
  w->prev = NULL;		// 7
  worldList = w;		// 8

  return worldList;
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
  clearGrid();
}

/** Clears all pointers in the grid */
void World::clearGrid()
{
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
    // find if current world is there
    for (struct dirent *dw = readdir(dirw); dw; dw = readdir(dirw)) {
      struct stat bufstat;
      if (stat(dw->d_name, &bufstat) == 0 &&
          S_ISDIR(bufstat.st_mode) &&
          ! strcmp(dw->d_name, getName())) {
        chdir(dw->d_name);
        DIR *diri = opendir(".");
        if (diri) {
          // find icons in this world
          for (struct dirent *di = readdir(diri); di; di = readdir(diri)) {
            if (stat(di->d_name, &bufstat) == 0 && S_ISREG(bufstat.st_mode)) {
              // open the icon and read it
              FILE *fp;
              File *file = new File();
              if ((fp = file->open(di->d_name, "r")) == NULL) {
                error("can't open %s/%s/%s", ::g.env.icons(), getName(), di->d_name);
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
                doAction(ICON_TYPE, Icon::CREATE, localUser(), infos, 0, 0);
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
    nitem = vsql->getCount(BALL_NAME, getName());	// balls in VSql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", getName());
      if (vsql->getName(BALL_NAME, pat, i, name) >= 0) {
        char *p = strchr(name, '@');
        if (p) {
          *p = '\0';
          doAction(BALL_TYPE, Ball::RECREATE, (WO*)this, (void*)name,0,0);
        }
      }
    }
    // check things
    nitem = vsql->getCount(THING_NAME, getName());	// things in VSql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", getName());
      if (vsql->getName(THING_NAME, pat, i, name) >= 0) {
        char *p = strchr(name, '@');
        if (p) {
          *p = '\0';
          doAction(THING_TYPE, Thing::RECREATE, (WO*)this, (void*)name,0,0);
        }
      }
    }
    // check mirages
    nitem = vsql->getCount(MIRAGE_NAME, getName());	// mirages in VSql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", getName());
      if (vsql->getName(MIRAGE_NAME, pat, i, name) >= 0) {
        char *p = strchr(name, '@');
        if (p) {
          *p = '\0';
          doAction(MIRAGE_TYPE, Mirage::RECREATE, (WO*)this, (void*)name,0,0);
        }
      }
    }
  }
}

/** Reads a world - static */
void World::reader(void *_url, Http *http)
{
  char *url = static_cast<char *>(_url);
  //char *url = World::current()->url;	// maybe url is corrupted HACK!!!
  //echo("worldReader: %s %s", _url, url);
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

  Parse *parser = new Parse();	// creates the parser instance

  *cachename = 0;
  Cache::setCachePath(url, cachename);
  if (stat(cachename, &bufstat) < 0) {	// is not in the cache
    if ((fpcache = fileout->open(cachename, "w")) == NULL) {
      error("worldReader: can't create file %s from url %s", cachename, url);
    }

    // download the vre file from the httpd server
httpread:
    while ((len = http->httpRead(buf, sizeof(buf))) > 0) {
      if (fpcache) {
        fwrite(buf, 1, len, fpcache);	// save into the cache
      }
      if (parser->parseVreFile(buf, len) <= 0) {  // eof
        break;
      }
    }
    fileout->close();
    delete fileout;

#if 0 //HAVE_LIBXML2
    Xml::dtdValidation(cachename);        // check the DTD
#endif //HAVE_LIBXML2
  }
  else {        // cachename exists in the cache
    if ((fpcache = filein->open(cachename, "r")) == NULL) {
      goto httpread;		// if can't open download it by http
    }
    while ((len = fread(buf, 1, sizeof(buf), fpcache)) > 0) {
      if (parser->parseVreFile(buf, len) <= 0) {
        break;			// eof
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

  world->setState(LOADING);
  world->setUrl(url);
  world->setName(url);
  world->setChanAndJoin(::g.channel);      // join initial channel
  Channel::getGroup(world->getChan(), Universe::current()->grpstr);
  Universe::current()->port = Channel::getPort(world->getChan());

  //report(world->getName());
  world->guip = ::g.gui.addWorld(world, NEW);
  world->initGrid();
  clearLists();
  WO::initNames();
  initFunc();		// init funcs (objects.cpp)

  if (::g.pref.keep == false) {
    ::g.env.cleanCacheByExt("vre");	// remove *.vre in the cache
  }

  world->clock = new Clock();
  world->bgcolor = new Bgcolor();

  //
  // Create local user first
  //
  User *user = new User();
  world->user = user;
  Universe::current()->localuser = user;	// keep user in this universe

  //
  // Download initial world (Rendezvous.vre by default)
  //
  //world->universe->startWheel();
  Http::httpOpen(url, reader, (void *)url, 0);
  //world->universe->stopWheel();
  endprogression();

  // check whether icons are locally presents
  world->checkIcons();
  // check whether other objects are persistents
  world->checkPersist();

  if (! ::g.pref.gravity) ::g.gui.pauseAvatar();

  world->setState(LOADED);
  trace1(DBG_INIT, "World %s initialized", world->getName());

  Entry *entry = new Entry();
  entry->query(user);
  localuser->setPosition();

  // Attach bubble welcome text to localuser
  char welcome[32];
  sprintf(welcome, "Hi! I am %s", ::g.user);
  user->bubble = new Bubble(user, welcome, Color::red, Bubble::BUBBLEVERSO);
}

/** Quits the current World */
void World::quit()
{
  trace1(DBG_WO, "quit %s", getName());
  state = STOPPED;

  //
  // Quits and deletes objects
  //

  // invisible objects
  for (vector<WO*>::iterator it = invisList.begin(); it != invisList.end(); ++it) {
    if ((*it)->deleted) continue;
    (*it)->quit();
    delete *it;
  }
  invisList.clear();

  // still objects
  for (vector<WO*>::iterator it = stillList.begin(); it != stillList.end(); ++it) {
    if ((*it)->deleted) continue;
    if (! (*it)->isValid()) continue;
    if (::g.pref.dbgtrace) echo("del: %s", (*it)->objectName());
    if (! strlen((*it)->objectName())) continue;	// avoid segfault
    (*it)->quit();	// sometimes segfault FIXME!!!
    delete *it;
  }
  stillList.clear();

  // mobile objects
  for (list<WO*>::iterator it = mobileList.begin(); it != mobileList.end(); ++it) {
    if (::g.pref.dbgtrace) echo("del: %s", (*it)->objectName());
    if ( (*it) == localuser /*|| (*it)->isBehavior(TRANSCIENT)*/ ) continue;  // FIX segfault
    //dax if ((*it)->type == DRESS_TYPE) continue;	// avoid segfault
    if ((*it)->deleted) continue;
    if (! (*it)->isValid()) continue;
    if (! strlen((*it)->objectName())) continue;
    (*it)->quit();
    delete *it;
  }
  mobileList.clear();

  // fluid objects
  for (vector<WO*>::iterator it = fluidList.begin(); it != fluidList.end(); ++it) {
    if ((*it)->deleted) continue;
    (*it)->quit();
    delete *it;
  }
  fluidList.clear();

  // cloth objects
  for (vector<WO*>::iterator it = clothList.begin(); it != clothList.end(); ++it) {
    if ((*it)->deleted) continue;
    (*it)->quit();
    delete *it;		// sometimes segfault FIXME!!!
  }
  clothList.clear();

  // Update GUI
  if (guip) {
    ::g.gui.updateWorld(this, OLD);
  }
  ::g.gui.showNavigator();	// force navig mode

  if (localuser && localuser->netop) {
    localuser->netop->declareDeletion();	// publishes I leave
    localuser->netop->deleteFromList();
  }

  // reset user position
  if (localuser) localuser->resetPosition();
  if (linked) return;

  //dax WO::resetObjectsNumber();
  Tool::quitTools();		// quits all tools
}

/** Enters in a new World - static */
World * World::enter(const char *url, const char *chanstr, bool isnew)
{
  trace1(DBG_WO, "world enter");

  // cleanup
  clearLists();
  ::g.gui.clearInfoBar();
  WO::initNames();
  current()->initGrid();

  World *world = NULL;

  // check whether this world is already in memory
  if (find(url) && isnew) {
    world = find(url);	// existing world
    worldList = swap(world);
    if (::g.pref.dbgtrace) echo("enter: world=%s (%d)", world->name, isnew);
    if (world->guip) {
      ::g.gui.updateWorld(world, NEW);
    }
  }
  else if (isnew) {
    //
    // new world must to be initialized
    //
    World *newworld = new World();

    if (! url) {	// sandbox world
      if (newworld->guip) {
        ::g.gui.updateWorld(newworld, NEW);
      }
    }
    else if (isprint(*url)) {
      newworld->url = new char[strlen(url) + 1];
      strcpy(newworld->url, url);
      newworld->setName(newworld->url);
    }
    else {
      return NULL;	// bad world url
    }
    
    if (chanstr) {	// not a world link
      newworld->setChan(chanstr);
    }
    newworld->guip = ::g.gui.addWorld(world, NEW);
    world = newworld;
  }
  else {	// world already exists
    world = current();
    if (world->guip) {
      ::g.gui.updateWorld(world, OLD);
    }
  }

  // default bgcolor
  world->bgcolor = new Bgcolor();

  /////////////////////////////////////////////////////
  //
  // Download the vre description file of the new world
  //
  world->setState(LOADING);	// need to download
  if (url) {
    //
    // world to downloaded
    //
    trace1(DBG_WO, "enter: downloading world url=%s", url);
    //world->universe->startWheel();
    if (Http::httpOpen(url, reader, (void *)url, 0) < 0) {
      error("enter: bad download: url=%s", url);
      return NULL;
    }
    //world->universe->stopWheel();
    endprogression();
    localuser->enableGravity();
    Grid::grid()->reset();
    Axis::axis()->reset();
  }
  else {	// world sandbox
    //
    // sandbox world
    //
    //echo("enter: world sandbox");
    World *sandbox = world;

    sandbox->setName("sandbox");

    Parse *parser = Parse::getParse();
    parser->parseVreFile(sandbox_vre, sizeof(sandbox_vre));
    sandbox->linked = true;

    float gridcolor[4] = { 0, 1, 0, .5 };	// green grid
    Grid::grid()->toggleGrid2d();
    Grid::grid()->setColor(gridcolor);
    Grid::grid()->setWidthIncr(16);
    Grid::grid()->setDepthIncr(16);
    Axis::axis()->toggle();
  }
  localuser->inits();

  // default entry
  new Entry();

  // Attach bubble hello text to localuser
  char hello[32];
  sprintf(hello, "Hello! I am %s", ::g.user);
  localuser->bubble = new Bubble(localuser, hello, Color::black, Bubble::BUBBLEVERSO);

  // check whether icons are locally presents
  world->checkIcons();
  // check whether other objects are persistents by VSql
  world->checkPersist();

  // create clock
  world->clock = new Clock();	// internal clock

  trace1(DBG_WO, "enter: world %s loaded: ", world->name);
  world->setState(LOADED);// downloaded

  return world;
}

/** Deletes all objects dropped in the deleteList - static */
void World::deleteObjects()
{
  for (vector<WO*>::iterator it = deleteList.begin(); it != deleteList.end(); ++it) {
    if (! (*it)->isBehavior(COLLIDE_NEVER)) {
      (*it)->delFromGrid();
    }
    if ((*it)->removed) {
      //dax objectList.remove(*it);
      //dax stillList.remove(*it);
      mobileList.remove(*it);
      //dax8 echo("delete object: %s", (*it)->objectName());
      if ((*it)->deleted) continue;
      if ( strcmp((*it)->typeName(), "Dart") && strcmp((*it)->typeName(), "Bullet") ) //dax8 Hack! FIXME!
        delete (*it);	//segfault FIXME!
    }
  }
  deleteList.clear();
}

/** Clears all lists */
void World::clearLists()
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

/** Lists world list - debug */
void World::dumpworldList(const char *note)
{
  int i=0;
  printf("%s: ", note);
  for (World *wp = worldList; wp && i<32; wp = wp->next, i++) {
    printf("%s -> ", wp->name);
    if (wp == wp->next) {
      printf("loop\n");
      return;
    }
  }
  if (i==32) printf("LOOP\n");
  else printf("\n");
}
