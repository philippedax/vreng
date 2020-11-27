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
#include "wobject.hpp"
#include "world.hpp"
#include "http.hpp"	// httpOpen httpRead
#include "url.hpp"	// setCacheName
#include "user.hpp"	// USER_TYPE
#include "clock.hpp"	// Clock
#include "bgcolor.hpp"	// Bgcolor
#include "icon.hpp"	// ICON_TYPE
#include "ball.hpp"	// BALL_NAME
#include "thing.hpp"	// THING_NAME
#include "mirage.hpp"	// MIRAGE_NAME
#include "vjc.hpp"	// Vjc
#include "url.hpp"	// abs
#include "cache.hpp"	// file2url
#include "universe.hpp"	// Universe
#include "mysql.hpp"	// VRSql
#include "netobj.hpp"	// NetObject
#include "solid.hpp"	// ~Solid
#include "gui.hpp"	// ::g.gui
#include "scene.hpp"	// GLSection
#include "app.hpp"	// quitTools
#include "channel.hpp"	// join
#include "vre.hpp"	// sandbox world
#include "wind.hpp"	// Wind
#include "bubble.hpp"	// Bubble
#include "env.hpp"	// icons
#include "pref.hpp"	// url
#include "olist.hpp"	// OList
#include "grid.hpp"	// Grid
#include "axis.hpp"	// Axis
#include "prof.hpp"	// new_world
#include "entry.hpp"	// Entry
#include "halo.hpp"	// Halo
#include "hat.hpp"	// Hat
#include "dress.hpp"	// Dress
#include "wings.hpp"	// Wings
#include "file.hpp"	// OpenFile

#include <list>
using namespace std;


const uint8_t World::WORLD_LEN = 32;

/* max space reachable, even values */
const uint8_t World::GRIDX = 4;  // 30
const uint8_t World::GRIDY = 4;  // 20
const uint8_t World::GRIDZ = 2;  // 6
const float   World::DISTX = 2.;
const float   World::DISTY = 2.;
const float   World::DISTZ = 2.;

#if 1 //dax
#define STATIC_GRID
#endif
#ifdef STATIC_GRID
#undef DYNAMIC_GRID
class OList* World::gridList[GRIDX][GRIDY][GRIDZ];
#else
#define DYNAMIC_GRID
#endif

World* World::worldList = NULL;


/* World constructor */
World::World()
{
  new_world++;
  guip = NULL;
  group = 0;
  ssrc = 0;
  state = 0;
  num = 0;
  namecnt = 0;
  islinked = false;
  persistent = true;
  prev = NULL;
  next = NULL;
  url = NULL;
  name = NULL;
  chan = NULL;
  bbcenter = newV3(0, 0, 0);
  bbsize = newV3(0, 0, 0);
  bbmin = newV3(0, 0, 0);
  bbmax = newV3(0, 0, 0);
  bbslice = newV3(DISTX, DISTY, DISTZ);
  grid = NULL;

  // interaction with general objects
  universe = Universe::current();
  bgcolor = NULL;
  clock = NULL;
  ground = 0;
  vjc = NULL;
  wind = NULL;

  if (universe->worldcnt == 0) {
    universe->worldcnt++;
    return;		// manager case, not in list
  }
  num = universe->worldcnt++;
  trace(DBG_WO, "World: this=%p num=%d", this, num);

  addToList();
}

/* Adds world into world list */
void World::addToList()
{
  if (!worldList)	// first world encountered
    next = prev = NULL;
  else if (worldList != this) {
    next = worldList;
    worldList->prev = this;
    prev = NULL;
  }
  worldList = this;
}

/* Gets current world */
World * World::current()
{
  return worldList;	// head of the worlds list
}

struct GuiItem* World::getGui() const
{
  return guip;
}

bool World::isGui() const
{
  return (guip) ? true : false;
}

void World::resetGui()
{
  guip = NULL;
}

/* Sets local world name */
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
  name = new char[end-begin + 1];
  memcpy(name, begin, end-begin);
  name[end-begin] = '\0';
  trace(DBG_WO, "setName: %s", name);
}

/** Gets current world name */
const char* World::getName() const
{
  return name;
}

/* Check wether this url has been already loaded - static */
World * World::worldByUrl(const char *url)
{
  if (! url) return NULL;	// sandbox world

  char urla[URL_LEN];
  Url::abs(url, urla);

  int loop = 0;
  for (World *w = worldList; w && (loop <100) ; w = w->next, loop++) {
    if ((! strcmp(w->url, url)) || (! strcmp(w->url, urla)))
      return w;	// world found
    if (w == w->next) {
      //error("getWorldByUrl: %s", w->url);
      break;	//FIXME: bug inside the list
    }
  }
  return NULL;	// world not found
}

World * World::worldByGroup(uint32_t group)
{
  int loop = 0;
  for (World *w = worldList; w && (loop <100) ; w = w->next, loop++)
    if (w->group == group)
      return w;	// world found
  return NULL;
}

World * World::worldBySsrc(uint32_t ssrc)
{
  int loop = 0;
  for (World *w = worldList; w && (loop <100) ; w = w->next, loop++)
    if (w->ssrc == ssrc)
      return w;	// world found
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
  manager->chan = new char[strlen(chan_str) + 1];
  Channel::joinManager(manager->chan, chan_str);
}

const char * World::getManagerChan() //static
{
  return manager->chan;
}

/* Sets the channel name */
bool World::setChan(const char *chan_str)
{
  if (! chan_str) {
    //error("setChan: chan_str NULL");
    return false;
  }
  if (*chan_str == '\0') {
    //error("setChan: chan_str EMPTY");
    return false;
  }
  if (strlen(chan_str) >= CHAN_LEN) {
    error("setChan: chan_str too long = %s", chan_str);
    return false;
  }

  chan = new char[strlen(chan_str) + 1];
  memset(chan, 0, strlen(chan_str)+1);
  strncpy(chan, chan_str, strlen(chan_str));
  return true;
}

/* Sets the channel name and Joins the new channel */
void World::setChanAndJoin(char *chan_str)
{
  if (setChan(chan_str)) Channel::join(chan_str);
}

/* Gets the current channel string */
const char* World::getChan() const	//FIXME (char *chan)
{
  return chan;
}

User* World::localUser() const
{
  return localuser;
}

Bgcolor* World::backgroundColor() const
{
  return bgcolor;
}

void World::setPersistent(bool _persistent)
{
  persistent = _persistent;
}

const char* World::getUrl() const
{
  return url;
}

void World::setUrl(const char* _url)
{
  if (url) delete[] url;
  url = new char[strlen(_url) + 1];
  strcpy(url, _url);
}

uint16_t World::number() const
{
  return num;
}

void World::setGround(float level)
{
  ground = level;
}

float World::getGround() const
{
  return ground;
}

uint32_t World::getSsrc() const
{
  return ssrc;
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

bool World::isLinked() const
{
  return islinked;
}

void World::linked()
{
  islinked = true;
}

bool World::isPersistent() const
{
  return persistent;
}

/**
 * Computes the World (Simulation) : The Heart of Vreng
 * called by gui/scene.cpp
 */
void World::compute(time_t sec, time_t usec)
{
  uint16_t dimx, dimy, dimz;

  trace(DBG_WO, "compute:");
  switch (getState()) {
      
  case LOADING:
    //error("compute: no end encountered");
    return;

  case LOADED:
    trace(DBG_WO, "compute loaded:");
    if (localuser) {
      localuser->move.perm_sec = sec;
      localuser->move.perm_usec = usec;
    }

    //
    // computes world's bb
    //
    for (list<WObject*>::iterator o = stillList.begin(); o != stillList.end(); ++o) {
      if (! (*o)->isValid()) continue;
      if (! (*o)->bbBehavior() || (*o)->isBehavior(COLLIDE_NEVER)) continue;
      for (int i=0; i<3 ; i++) {
        bbmin.v[i] = MIN(bbmin.v[i], (*o)->pos.bbcenter.v[i] - (*o)->pos.bbsize.v[i]);
        bbmax.v[i] = MAX(bbmax.v[i], (*o)->pos.bbcenter.v[i] + (*o)->pos.bbsize.v[i]);
      }
    }
    for (list<WObject*>::iterator o = mobileList.begin(); o != mobileList.end(); ++o) {
      if (! (*o)->isValid()) continue;
      if (! (*o)->bbBehavior() || (*o)->isBehavior(COLLIDE_NEVER) || (*o)->type == USER_TYPE) continue;
      for (int i=0; i<3 ; i++) {
        bbmin.v[i] = MIN(bbmin.v[i], (*o)->pos.bbcenter.v[i] - (*o)->pos.bbsize.v[i]);
        bbmax.v[i] = MAX(bbmax.v[i], (*o)->pos.bbcenter.v[i] + (*o)->pos.bbsize.v[i]);
      }
      if (bbmax.v[0] > 1000 || bbmax.v[1] >1000 || bbmax.v[2] > 1000)
        error("mobil: %d %s bbmin=%.1f,%.1f,%.1f bbmax=%.1f,%.1f,%.1f", (*o)->type, (*o)->getInstance(), bbmin.v[0], bbmin.v[1], bbmin.v[2], bbmax.v[0], bbmax.v[1], bbmax.v[2]);
    }
    for (int i=0; i<3 ; i++) {
      bbcenter.v[i] = (bbmax.v[i] + bbmin.v[i]);
      bbsize.v[i]   = (bbmax.v[i] - bbmin.v[i]);
    }
    notice("size=%.1f,%.1f,%.1f center=%.1f,%.1f,%.1f", bbsize.v[0], bbsize.v[1], bbsize.v[2], bbcenter.v[0], bbcenter.v[1], bbcenter.v[2]);

    OList::clearIspointedFlag(mobileList);

    // compute Grid dimensions
    dimx = (int) (bbsize.v[0] / DISTX);
    dimy = (int) (bbsize.v[1] / DISTY);
    dimz = (int) (bbsize.v[2] / DISTZ);
    dimx = (dimx % 2) ? dimx + 1 : dimx;
    dimy = (dimy % 2) ? dimy + 1 : dimy;
    dimz = (dimz % 2) ? dimz + 1 : dimz;
    dimx = MIN(64, dimx);
    dimy = MIN(64, dimy);
    dimz = MIN(16, dimz);
    //notice("dim: %d,%d,%d", dimx, dimy, dimz);
    //dimgrid[Ø] = dimx;
    //dimgrid[1] = dimy;
    //dimgrid[2] = dimz;
    // free the current grid, reinitialize another one with new dimensions
    //World::current->freeGrid();
    //World::current->initGrid(dimgrid, bbslice);

    Grid::grid()->init(dimx, dimy, dimz);
    Axis::axis()->init();
    setState(SIMULATION);
    return;

  case SIMULATION:
    trace(DBG_WO, "compute simulation:");
    //
    // user motions
    //
    if (! localuser) return;
    if (! localuser->isValid()) return;
    if (::g.pref.dbgtrace) error("user mov: %s-%s", localuser->typeName(), localuser->named());
    localuser->userMovement(sec, usec);

    //
    //  removes objects scheduled to be deleted
    //
    deleteObjects();

    //
    // objects with imposed and permanent motions
    //
    {
      int i=0;
      for (list<WObject*>::iterator o = mobileList.begin(); o != mobileList.end(); ++o, i++) {
        if (! (*o)->isValid()) {
          error("bad type=0: %d", i);
          mobileList.remove(*o);
          continue;
        }
        if ((*o)->type > OBJECTSNUMBER) {
          error("bad type out of range: %d t=%d", i, (*o)->type);
          mobileList.remove(*o);
          continue;
        }
        if ((*o)->mode > 2) {
          error("bad mode: %d m=%d", i, (*o)->mode);
          mobileList.remove(*o);
          continue;
        }
        if (! (*o)->num) {
          error("num null: %d", i);
          mobileList.remove(*o);
          continue;
        }
        if (::g.pref.dbgtrace) error("obj: %s-%s", (*o)->typeName(), (*o)->getInstance());
        (*o)->imposedMovement(sec, usec);
        (*o)->permanentMovement(sec, usec);
      }
    }
    default: return;
  }
}

/* Switch channels */
// virtual private
bool World::call(World *wpred)
{
  trace(DBG_WO, "call: leave chan=%s", wpred->chan);
  if (wpred->islinked) {
    enter(url, NULL, OLD);
    setChan(wpred->chan);
  }
  else {
    trace(DBG_IPMC, "call: leave chan=%s", wpred->chan);
    if (Channel::current()) delete Channel::current();	// leave current channel

    enter(url, NULL, OLD);

    char groupstr[GROUP_LEN];
    Channel::getGroup(chan, groupstr);
    group = inet_addr(groupstr);

    trace(DBG_IPMC, "call: join chan=%s", chan);
    if (Channel::join(chan) == 0) {	// join previous channel
      trace(DBG_IPMC, "call: can't join chan=%s", chan);
      return false;
    }
    setChan(chan);

    ::g.gui.updateWorld(this, NEW);	// nofify the gui
  }
  return true;
}

/* Go to the previous World */
// static
World * World::goBack()
{
  World *world = worldList;
  World *worldback = worldList->next;

  if (! worldback) return NULL;
  trace(DBG_WO, "goBack: ");

  world->quit();	// quit current world first

  World *wp;
  int loop = 0;
  for (wp = worldback; wp->next && (loop <100); wp = wp->next, loop++)
    if (wp == wp->next) break;

  wp->next = world;
  world->next = NULL;
  worldback->prev = NULL;
  world->prev = worldback;
  worldList = worldback;
  //debug dumpworldlist("back");

  if (worldback->call(world)) return worldList;
  return NULL;
}

/* Go to the next World */
// static
World * World::goForward()
{
  World *world = worldList;
  World *worldforw;

  if (! worldList->next) return NULL;
  trace(DBG_WO, "goForward: ");

  world->quit();	// quit current world first

  World *wp;
  int loop = 0;
  for (wp = world; (worldforw = wp->next)->next && (loop <100); wp = wp->next, loop++) ;
  worldforw->next = world;
  worldforw->prev = NULL;
  world->prev = worldforw;
  wp->next = NULL;
  worldList = worldforw;
  //debug dumpworldlist("forw");

  if (worldforw->call(world)) return worldList;
  return NULL;
}

/* Exchange Worlds in the list */
// static
World * World::swap(World *w)
{
  if (worldList == w) return worldList;

  trace(DBG_WO, "swap: ");
  if (w->prev)
    w->prev->next = worldList;	// 1
  if (w->next)
    w->next->prev = worldList;	// 2
  if (worldList->next)
    worldList->next->prev = w;	// 3
  worldList->prev = w->prev;	// 4
  World *tmp = worldList->next;
  worldList->next = w->next;	// 5
  w->next = tmp;		// 6
  w->prev = NULL;		// 7
  worldList = w;		// 8
  return worldList;
}

//
// GRID
//

void World::initGrid()
{ 
  dimgrid[0] = GRIDX;
  dimgrid[1] = GRIDY;
  dimgrid[2] = GRIDZ;
  bbslice.v[0] = DISTX;
  bbslice.v[1] = DISTY;
  bbslice.v[2] = DISTZ;
  localGrid();
        
#ifdef DYNAMIC_GRID 
  grid = allocGrid();
#endif
  clearGrid();
}
                
void World::initGrid(const uint8_t _dim[3], const V3 &sl)
{
  for (int i=0; i<3 ; i++) {
    dimgrid[i] = _dim[i];
    bbslice.v[i] = sl.v[i];
  }
  localGrid();

#ifdef DYNAMIC_GRID
  grid = allocGrid();
#endif
  clearGrid();
}

OList **** World::allocGrid()
{
#ifdef DYNAMIC_GRID
  grid = new OList***[dimgrid[0]];
  for (int x=0; x < dimgrid[0] ; x++)
    grid[x] = new OList**[dimgrid[1]];
  for (int x=0; x < dimgrid[0] ; x++)
    for (int y=0; y < dimgrid[1] ; y++)
      grid[x][y] = new OList*[dimgrid[2]];
  return grid;
#else
  return NULL;
#endif
}
  
/** clear all pointers in the grid */
void World::clearGrid()
{
  for (int x=0; x < dimgrid[0]; x++)
    for (int y=0; y < dimgrid[1]; y++)
      for (int z=0; z < dimgrid[2]; z++)
#ifdef DYNAMIC_GRID
        if (grid) grid[x][y][z] = NULL;
#else
        gridList[x][y][z] = NULL;
#endif
}

/** free all the grid (static) */
void World::freeGrid()
{
  for (int x=0; x < dimgrid[0]; x++)
    for (int y=0; y < dimgrid[1]; y++)
      for (int z=0; z < dimgrid[2]; z++) {
#ifdef DYNAMIC_GRID
        if (grid) {
          if (grid[x][y][z]) grid[x][y][z]->remove();
          grid[x][y][z] = NULL;
        }
#else
        if (gridList[x][y][z]) gridList[x][y][z]->remove();
        gridList[x][y][z] = NULL;
#endif
      }
#ifdef DYNAMIC_GRID
  for (int x=0; x < dimgrid[0]; x++)
    for (int y=0; y < dimgrid[1]; y++)
      if (grid[x][y]) delete[] grid[x][y];
  for (int x=0; x < dimgrid[0]; x++)
    if (grid[x]) delete[] grid[x];
  if (grid) delete[] grid;
  grid = NULL;
#endif
}

/* Check and load my proper icons - static */
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
            if (stat(di->d_name, &bufstat) == 0 &&
                S_ISREG(bufstat.st_mode)) {
              // open the icon and read it
              FILE *fp;
              if ((fp = File::openFile(di->d_name, "r")) == NULL) {
                error("can't open %s/%s/%s", ::g.env.icons(), getName(), di->d_name);
                continue;
              }
              char vref[BUFSIZ], infos[BUFSIZ *2], urlvre[URL_LEN];
              fgets(vref, sizeof(vref), fp);
              File::closeFile(fp);
              Cache::file2url(di->d_name, urlvre);
              // create the icon
              sprintf(infos, "<url=\"%s\">&<vref=%s>", urlvre, vref);
              trace(DBG_WO, "load-icon: %s", infos);
              if (isAction(ICON_TYPE, Icon::CREATE))
                doAction(ICON_TYPE, Icon::CREATE, localUser(), infos, 0, 0);
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

/* Check wether other objects are persistents */
void World::checkPersistObjects()
{
#if HAVE_MYSQL
  VRSql *psql = VRSql::getVRSql();     // first take the VRSql handle;
  if (psql) {
    int nitem;
    char pat[256], qname[256];

    nitem = psql->getCount(BALL_NAME, getName());	// balls in MySql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", getName());
      if (psql->getName(BALL_NAME, pat, i, qname) >= 0) {
        char *p = strchr(qname, '@');
        if (p) {
          *p = '\0';
          doAction(BALL_TYPE, Ball::RECREATE, (WObject*)this, (void*)qname,0,0);
        }
      }
    }
    nitem = psql->getCount(THING_NAME, getName());	// things in MySql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", getName());
      if (psql->getName(THING_NAME, pat, i, qname) >= 0) {
        if (! stringcmp(qname, THING_NAME)) {
          char *p = strchr(qname, '@');
          if (p) {
            *p = '\0';
            doAction(THING_TYPE, Thing::RECREATE, (WObject*)this, (void*)qname,0,0);
          }
        }
      }
    }
    nitem = psql->getCount(MIRAGE_NAME, getName());	// mirages in MySql
    for (int i=0; i < nitem; i++) {
      sprintf(pat, "@%s", getName());
      if (psql->getName(MIRAGE_NAME, pat, i, qname) >= 0) {
        if (! stringcmp(qname, MIRAGE_NAME)) {
          char *p = strchr(qname, '@');
          if (p) {
            *p = '\0';
            doAction(MIRAGE_TYPE, Mirage::RECREATE, (WObject*)this, (void*)qname,0,0);
          }
        }
      }
    }
  }
#endif
}

/* world reader - static */
void World::httpReader(void *url, Http *http)
{
  char *vreurl = (char *) url;
  char vrefile[BUFSIZ];
  Cache::setCacheName(vreurl, vrefile);
  trace(DBG_WO, "httpReader %s:", vreurl);

  if (! http) error("can't download %s, check access to the remote http server",vreurl);

  Parse *parser = new Parse();	// create the parser instance

  FILE *vrefp;
  int vrelen = 0;
  struct stat vrestat;
  char vrebuf[BUFSIZ];

  if (stat(vrefile, &vrestat) < 0) {    // file is not in the cache
    if ((vrefp = File::openFile(vrefile, "w")) == NULL)
      error("can't create file %s", vrefile);

    // download the vre file from the httpd server
httpread:
    while ((vrelen = http->httpRead(vrebuf, sizeof(vrebuf))) > 0) {
      if (vrefp)
        fwrite(vrebuf, 1, vrelen, vrefp);      // save into the cache
      if (parser->parseVreLines(vrebuf, vrelen) <= 0)   // parsing error
        break;
    }

#if 0 //HAVE_LIBXML2
    Xml::dtdValidation(vrefile);        // check the DTD
#endif //HAVE_LIBXML2
  }
  else {        // file exists in the cache
    if ((vrefp = File::openFile(vrefile, "r")) == NULL) goto httpread;    // download it
    while ((vrelen = fread(vrebuf, 1, sizeof(vrebuf), vrefp)) > 0) {
      if (parser->parseVreLines(vrebuf, vrelen) <= 0)   // parsing error
        break;
    }
  }
  parser->numline = 0;
  if (vrefp) File::closeFile(vrefp);

  trace(DBG_WO, "httpReader: %s downloaded", vreurl);
  return;
}

/* General World Initialization - static */
void World::init(const char *vreurl)
{
  trace(DBG_INIT, "initial url = %s", vreurl);

  //
  // Create initial world
  //
  World *world = new World();

  world->setState(TOLOAD);
  world->setChanAndJoin(::g.channel);      // join initial channel
  world->setName(Universe::current()->url);
  Channel::getGroup(world->getChan(), Universe::current()->group);
  Universe::current()->port = Channel::getPort(world->getChan());

  world->initGrid();
  initLists();
  initNames();
  initGeneralFuncList();

  if (::g.pref.keep == false)
    ::g.env.cleanCacheByExt(".vre");	// remove *.vre in the cache

  world->setUrl(vreurl);
  world->setName(vreurl);
  world->guip = ::g.gui.addWorld(world, NEW);
  trace(DBG_INIT, "initWO: worldname = %s", world->getName());

  world->clock = new Clock();
  world->bgcolor = new Bgcolor();

  //
  // Create local user first
  //
  trace(DBG_WO, "createLocaluser: ");
  User *user = new User();
  world->user = user;
  Universe::current()->localuser = user;	// keep user in this universe

  //
  // Download initial world (Rendezvous.vre)
  //
  trace(DBG_WO, "dowload initial world: ");
  world->setState(LOADING);
  world->universe->startWheel();
  Http::httpOpen(world->getUrl(), httpReader, (void *)vreurl, 0);
  world->universe->stopWheel();
  endprogression();

  // Attach bubble text to localuser
  char text[32];
  float red[] = {1,0,0};
  sprintf(text, "Hi! I am %s", user->getInstance());
  user->bubble = new Bubble(user, text, red, Bubble::BUBBLEBACK);

  // check if icons are locally presents
  world->checkIcons();

  // check wether other objects are persistents by MySql
  world->checkPersistObjects();

  //not used
  //declareJoinWorldToManager(world->getUrl(), getChan(), user->getInstance());

  if (! ::g.pref.gravity) ::g.gui.pauseUser();

  world->setState(LOADED);
  trace(DBG_INIT, "World initialized");

  Entry *entry = new Entry();
  entry->query(user);
  //dax localuser->setPosition();
}

/* Quits the current World */
void World::quit()
{
  trace(DBG_WO, "quit %s", getName());
  state = STOPPED;

  Parse *parser = Parse::getParse();
  if (parser) delete parser;

  //notused declareLeaveWorldToManager(getName(), getChan(), localUser()->getInstance());

  //
  // Quits and deletes objects
  //
  for (list<WObject*>::iterator o = invisibleList.begin(); o != invisibleList.end(); ++o) {
    if (*o && (*o)->isValid()) {
      (*o)->quit();
      delete *o;
    }
  }
  invisibleList.clear();

  for (list<WObject*>::iterator o = fluidList.begin(); o != fluidList.end(); ++o) {
    if (*o && (*o)->isValid()) {
      (*o)->quit();
      delete *o;
    }
  }
  fluidList.clear();

  for (list<WObject*>::iterator o = stillList.begin(); o != stillList.end(); ++o) {
    if (*o && (*o)->isValid()) {
      (*o)->clearObjectBar();
      (*o)->quit();
      delete *o;
    }
  }
  stillList.clear();

  for (list<WObject*>::iterator o = mobileList.begin(); o != mobileList.end(); ++o) {
    if ((*o) == localuser) continue;
    if (*o) { //FIXME segfault
      if ((*o)->isValid() && ! (*o)->isEphemeral()) {
        //FIXME segfault (*o)->clearObjectBar();
        (*o)->clearObjectBar();
        (*o)->quit();
        delete *o;
      }
    }
  }
  mobileList.clear();

  //dax current()->freeGrid();

  // Update GUI
  if (guip) ::g.gui.updateWorld(this, OLD);
  ::g.gui.showNavigator();	// force navig mode

  if (localuser && localuser->noh) {
    localuser->noh->declareDeletion();	// publishes I leave
    localuser->noh->deleteFromList();
  }

  // reset user position
  if (localuser) localuser->resetPosition();
  if (islinked) return;

  WObject::resetObjectsNumber();
  App::quitTools();
}

/* New World initialization - static */
World * World::enter(const char *url, const char *chanstr, bool isnew)
{
  trace(DBG_WO, "world enter: ");
  // debug show
  //OList::show(mobileList, "mobile:");

  // cleanup
  initLists();
  current()->initGrid();
  initNames();

  World *world = NULL;

  // check whether this world is already in memory
  if (worldByUrl(url) != NULL && isnew) {
    world = worldByUrl(url);	// existing world
    worldList = swap(world);
    if (::g.pref.dbgtrace) error("enter: world=%s (%d)", world->name, isnew);
    if (! isprint(*world->url)) {
      error("enter: bad old url");	//FIXME
      strcpy(world->url, url);
    }
    //debug dumpworldlist("old ");
    if (world->guip) ::g.gui.updateWorld(world, NEW);
  }
  else if (isnew) { // new world must to be initialized

    World *newworld = new World();

    if (url && isprint(*url)) {
      newworld->url = new char[strlen(url) + 1];
      if (strlen(url) < URL_LEN)
        strcpy(newworld->url, url);
      else {
        strncpy(newworld->url, url, URL_LEN-1);
        newworld->url[URL_LEN] = '\0';
        warning("enter: url too long = %s", url);
      }
      newworld->setName(newworld->url);
    }
    else if (! url) {	// sandbox world
      newworld->setName("sandbox");
      worldList = swap(newworld);
      if (newworld->guip) ::g.gui.updateWorld(newworld, NEW);
    }
    else return NULL;	// bad world
    
    //debug dumpworldlist("new ");
    if (chanstr) {	// not a world link
      newworld->setChan(chanstr);
    }

    newworld->guip = ::g.gui.addWorld(world, NEW);
    world = newworld;
  }
  else { // world already exists
    trace(DBG_WO, "world=%s (%d) already exists", current()->getName(), isnew);
    world = current();
    if (world->guip) ::g.gui.updateWorld(world, OLD);
  }

  // default bgcolor
  world->bgcolor = new Bgcolor();

  /////////////////////////////////////////////////////
  //
  // Download the vre description file of the new world
  //
  world->setState(LOADING);	// to download
  if (url) {
    trace(DBG_WO, "downloading world url=%s", url);
    world->universe->startWheel();
    if (Http::httpOpen(url, httpReader, (void *)url, 0) < 0) {
      error("bad download: url=%s", url);
      return NULL;
    }
    world->universe->stopWheel();
    endprogression();
  }
  else {
    trace(DBG_WO, "world sandbox: ");
    world->setName("sandbox");
    Parse *parser = Parse::getParse();
    parser->parseVreLines(sandbox_vre, sizeof(sandbox_vre));
  }

  localuser->inits();

  // default entry
  new Entry();

  // check wether icons are locally presents
  world->checkIcons();

  // check wether other objects are persistents by MySql
  world->checkPersistObjects();

  // create clock
  world->clock = new Clock();	// internal clock

  trace(DBG_WO, "world loaded: ");
  world->setState(LOADED);// downloaded
  return world;
}

/* Deletes all objects dropped in the deleteList - static */
void World::deleteObjects()
{
  int s = deleteList.size();
  int i=0;

  for (list<WObject*>::iterator o = deleteList.begin(); i<s; ++o, i++) {
    if (*o) {
      if ((*o)->isValid() && ! (*o)->isBehavior(COLLIDE_NEVER)) (*o)->deleteFromGrid();
      mobileList.remove(*o);
      deleteList.remove(*o);
      if (*o) delete *o;	//segfault
    }
  }
}

/* clears all lists */
void World::initLists()
{
  mobileList.clear();
  invisibleList.clear();
  fluidList.clear();
  stillList.clear();
  deleteList.clear();
  lightList.clear();
}

#if 0 //debug
void dumpworldlist(const char *note)
{
  int i=0;
  printf("%s: ", note);
  for (World *wp = current(); wp && i<10; wp = wp->next, i++) {
    printf("%s -> ", wp->_name);
    if (wp == wp->next) {
      printf("loop\n");
      return;
    }
  }
  if (i==10) printf("LOOP\n");
  else printf("null\n");
}
#endif
