//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
// Telecom-Paristech (Ecole Nationale Superieure des Telecommunications)
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
#include "icon.hpp"
#include "user.hpp"	// localuser
#include "move.hpp"	// GRAVITY
#include "wall.hpp"	// Wall
#include "format.hpp"	// icons
#include "cache.hpp"	// file download
#include "texture.hpp"	// Texture
#include "netobj.hpp"	// NetObject
#include "payload.hpp"	// Payload
#include "ps.hpp"	// start
#include "pdf.hpp"	// start
#include "mp3.hpp"	// start
#include "midi.hpp"	// start
#include "browser.hpp"	// start
#include "office.hpp"	// start
#include "env.hpp"	// icons
#include "timer.hpp"	// getRate
#include "file.hpp"	// openFile
#include "universe.hpp"	// current


const OClass Icon::oclass(ICON_TYPE, "Icon", NULL, Icon::replicator);

const float Icon::DEPTH = 0.03;
const float Icon::LSPEED = 1.0;
const float Icon::ZSPEED = 1.0;
const float Icon::ASPEED = M_PI_4;
const float Icon::TTL = 1.0;
const float Icon::WIDTH = 0.32;
const float Icon::HEIGHT = 0.32;

// local
static const char *COLOR = "1 1 1";	// white


void Icon::updateTime(time_t sec, time_t usec, float *lasting)
{
  updateLasting(sec, usec, lasting);
}

bool Icon::isMoving()
{
  if (taken) return true;
  return testMoving();
}

/* sollicited movements */
void Icon::changePosition(float lasting)
{
  if (! taken) {
    float ratio = MAX(::g.times.getRate() / 40., 1);
    pos.x  += lasting * move.lspeed.v[0] * ratio;
    pos.y  += lasting * move.lspeed.v[1] * ratio;
    pos.z  += lasting * move.lspeed.v[2] * ratio;
    pos.az += lasting * move.aspeed.v[0] * ratio;
    pos.ax += lasting * move.aspeed.v[2] * ratio;
  }
  else {
    float off = 0.4;
    pos.x = localuser->pos.x + off * Cos(localuser->pos.az);
    pos.y = localuser->pos.y + off * Sin(localuser->pos.az);
    pos.z = localuser->pos.z + 0.6;
    pos.az = localuser->pos.az + M_PI_2;
  }
}

/** permanent movements */
void Icon::changePermanent(float lasting)
{
  pos.z -= lasting * GRAVITY / 2;
}

/** creation: this method is invisible: called by GUI::putIcon */
void Icon::create(User *pu, void *d, time_t s, time_t u)
{
  new Icon(pu, d);
}

void Icon::defaults()
{
  lspeed = LSPEED;
  zspeed = ZSPEED;
  aspeed = ASPEED;
  ttl = TTL;
  remove = false;
}

/** document's geometry */
void Icon::makeSolid()
{
  char s[256];

  sprintf(s, "solid dim=\"%f %f %f\" dif=\"%s\" xn=\"%s\" />",
          WIDTH, DEPTH, HEIGHT, COLOR, tex);
  parse()->parseSolid(s, SEP, this);
}

void Icon::parser(char *l)
{
    l = tokenize(vref);
    begin_while_parse(l) {
      l = parse()->parseAttributes(l, this);
      if (!l) break;
      if (!stringcmp(l, "owner=")) l = parse()->parseString(l, names.owner, "owner");
    }
    end_while_parse(l);
}

char * Icon::getParam(char *ptok)
{
  char *p, *p2;

  p = strchr(ptok, '=');
  ptok++;
  if ((p = strchr(p, '"'))) {
    ptok = ++p;
    if ((p2 = strchr(p, '"'))) *p2 = 0;
  }
  else {
    if ((p2 = strchr(ptok, '>'))) *p2 = 0;
  }
  return ptok;
}

/* called by GUI */
Icon::Icon(User *user, void *d)
{
  char *infos = (char *) d;

  ifile = NULL;
  ofile = NULL;
  char *action = NULL;
  vref = NULL;
  char icon[URL_LEN];
  *icon = 0;

  /* parameters transmission */
  for (char *pt = strtok(infos, "&"); pt ; pt = strtok(NULL, "&")) {
    if (! stringcmp(pt, "<url=")) {
      pt = getParam(pt);
      strcpy(names.url, pt);
      taken = true;
    }
    else if (! stringcmp(pt, "<file=")) {
      pt = getParam(pt);
      ifile = strdup(pt);
    }
    else if (! stringcmp(pt, "<ofile=")) {
      pt = getParam(pt);
      strcpy(names.url, pt);
    }
    else if (! stringcmp(pt, "<name=")) {
      pt = getParam(pt);
      strcpy(names.named, pt);
    }
    else if (! stringcmp(pt, "<icon=")) {
      pt = getParam(pt);
      strcpy(icon, pt);
    }
    else if (! stringcmp(pt, "<action=")) {
      pt = getParam(pt);
      action = strdup(pt);
    }
    else if (! stringcmp(pt, "<vref=")) {
      pt = strchr(pt, '=');
      pt++;
      char *p2;
      if ((p2 = strchr(pt, '>'))) *p2 = 0;
      vref = strdup(pt);
    }
  }

  if (vref) {	// local saved document exists
    parser(vref);

    // get the last loaded texture
    int texid = Texture::getIdByUrl(names.url);
    Texture *texlast = Texture::getEntryById(texid);
    if (! texlast) {
      tex = new char[sizeof(ICO_DEF) + 1];
      strcpy(tex, ICO_DEF);	// default texture
    }
    else {
      tex = new char[strlen(texlast->url) + 1];
      strcpy(tex, texlast->url);
    }
    taken = false;
  }
  else {	// new document named interactively by hand
    /* position */
    float off = 0.4;		// 40cm in front of avatar
    pos.x = user->pos.x + off * Cos(user->pos.az);
    pos.y = user->pos.y + off * Sin(user->pos.az);
    pos.z = user->pos.z + 0.6;	// visible by avatar's eyes
    pos.az = user->pos.az + M_PI_2;

    /* texture */
    if (*icon) {
      tex = new char[strlen(icon) + 1];
      strcpy(tex, icon);
    }
    else {
      // default binding icon to document
      char ext[8] = "";
      memset(ext, 0, sizeof(ext));
      if (Format::getExt(names.url, ext)) {
        tex = new char[URL_LEN];
        Format::getImgByExt(ext, tex);
      }
      else {
        tex = new char[sizeof(ICO_DEF) + 1];
        strcpy(tex, ICO_DEF);
      }
    }

    if (ifile) {	// private local document
      if (*names.url) {
        // public url given by user
        Cache::download(ifile, ofile, "inout");
      }
      else {
        // build local ofile in ~/public_html/vreng/
        ofile = new char[URL_LEN];
        sprintf(ofile, "%s/public_html", getenv("HOME"));
        if (access(ofile, R_OK|W_OK|X_OK) == 0) {
          strcat(ofile, "/vreng/");
          if (access(ofile, R_OK|W_OK|X_OK) == -1)
            mkdir(ofile, 0755);
          strcat(ofile, ifile);
          FILE *fin, *fout;
          if ((fin = File::openFile(ifile, "r")) && (fout = File::openFile(ofile, "w"))) {
            char buf[2];
            while (fread(buf, 1, 1, fin))
              fwrite(buf, 1, 1, fout);
            File::closeFile(fin);
            File::closeFile(fout);
            chmod(ofile, 0644);

            //FIXME: define local http_server
            //dax sprintf(names.url, "http://%s/~%s/vreng/%s", DEF_HTTP_SERVER, getenv("USER"), ifile);
            sprintf(names.url, "http://%s%s/vreng/%s", Universe::current()->server, Universe::current()->urlpfx, ifile);
          }
          else {
            error("can't open %s or %s: %s (%d)", ifile, ofile, strerror(errno), errno);
            free(ifile); ifile = NULL;
            delete[] ofile;
          }
        }
        else {
          error("can't access %s", ofile);
          free(ifile); ifile = NULL;
          delete[] ofile;
        }
      }
    }
    makeSolid();
  }

  // local creation
  defaults();

  enableBehavior(REMOVABLE);
  enableBehavior(NO_ELEMENTARY_MOVE);
  setRenderPrior(RENDER_HIGH);

  initializeMobileObject(1);
  ttl = (taken) ? MAXFLOAT : 0;
  initImposedMovement(ttl);
  disablePermanentMovement();

  // network creation
  createVolatileNetObject(PROPS);

  // document's owner
  setOwner();

  trace(DBG_WO, "Icon: url=%s icon=%s name=%s owner=%s", urlName(), tex, getInstance(), ownerName());

  if (action) {
    if      (! stringcmp(action, "pin")) pin(this, NULL, 0L, 0L);
    else if (! stringcmp(action, "push")) push(this, NULL, 0L, 0L);
    else if (! stringcmp(action, "carry")) carry(this, NULL, 0L, 0L);
  }
}

/* Replication from the network */
WObject * Icon::replicator(uint8_t type_id, Noid noid, Payload *pp)
{
  return new Icon(type_id, noid, pp);
}

Icon::Icon(uint8_t type_id, Noid _noid, Payload *pp)
{
  setType(type_id);
  replicateVolatileNetObject(PROPS, _noid);
  noh->getAllProperties(pp);
  copyNoid(_noid);

  parse()->parseName(names.named, names.named);
  makeSolid();
  defaults();
  initializeMobileObject(0);
  taken = false;
}

/** Updates to the network */
bool Icon::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Icon::whenIntersect(WObject *pcur, WObject *pold)
{
  switch (pcur->type) {
  case WALL_TYPE:
    pos.x += pcur->pos.x - pold->pos.x;
    pos.y += pcur->pos.y - pold->pos.y;
    pos.z += pcur->pos.z - pold->pos.z;
    pos.az += pcur->pos.az - pold->pos.az;
    pos.ax += pcur->pos.ax - pold->pos.ax;
    updatePositionAndGrid(pold);
    break;
  case USER_TYPE:
    projectPosition(pcur, pold);
    break;
  case ICON_TYPE:
    pcur->pos.x += 0.1;
    pcur->pos.y += 0.1;
    pcur->updatePositionAndGrid(pold);
    break;
  default:
    pold->copyPositionAndBB(pcur);
  }
  return true;
}

/* this method is invisible: called by Wall::whenIntersect */
void Icon::stick(Wall *pwall, void *_picon, time_t s, time_t u)
{
  Icon *picon = (Icon *) _picon;

  picon->pos.az = pwall->pos.az;	// same angle than obstacle

  GLfloat iconcolor[3] = {1, 0, 0};	// red
  picon->setFlashy(iconcolor);		// flash the icon
  GLfloat wallcolor[3] = {1, 1, 0};	// yellow
  pwall->setFlashy(wallcolor);		// flash the wall

  //dax picon->updatePositionAndGrid(picon->pos);
  picon->updatePosition();
  picon->enableBehavior(COLLIDE_ONCE);
  picon->taken = false;
}

void Icon::pin(Icon *icon, void *d, time_t s, time_t u)
{
  clearV3(icon->move.lspeed);
  clearV3(icon->move.aspeed);
  icon->move.lspeed.v[0] = icon->lspeed * 4 * Cos(localuser->pos.az);
  icon->move.lspeed.v[1] = icon->lspeed * 4 * Sin(localuser->pos.az);
  icon->ttl = Icon::TTL * 10;
  icon->initImposedMovement(icon->ttl);
  icon->disablePermanentMovement();
  icon->enableBehavior(COLLIDE_ONCE);
  icon->taken = false;
}

void Icon::push(Icon *icon, void *d, time_t s, time_t u)
{
  clearV3(icon->move.lspeed);
  clearV3(icon->move.aspeed);
  icon->move.lspeed.v[0] = icon->lspeed * Cos(localuser->pos.az);
  icon->move.lspeed.v[1] = icon->lspeed * Sin(localuser->pos.az);
  icon->ttl = Icon::TTL * 4;
  icon->initImposedMovement(icon->ttl);
  icon->disablePermanentMovement();
  icon->enableBehavior(COLLIDE_ONCE);
  icon->taken = false;
}

void Icon::pull(Icon *icon, void *d, time_t s, time_t u)
{
  if (! icon->taken) {
    icon->resetFlashy();
    clearV3(icon->move.lspeed);
    clearV3(icon->move.aspeed);
    icon->move.lspeed.v[0] = -icon->lspeed * Cos(localuser->pos.az);
    icon->move.lspeed.v[1] = -icon->lspeed * Sin(localuser->pos.az);
    icon->ttl = Icon::TTL;
    icon->initImposedMovement(icon->ttl);
    icon->disablePermanentMovement();
    icon->enableBehavior(COLLIDE_ONCE);
    icon->taken = false;
  }
}

void Icon::carry(Icon *icon, void *d, time_t s, time_t u)
{
  if (! icon->taken) {
    icon->resetFlashy();
    icon->ttl = MAXFLOAT;
    icon->initImposedMovement(icon->ttl);
    icon->disablePermanentMovement();
    icon->disableBehavior(COLLIDE_NEVER);
    icon->taken = true;
  }
}

void Icon::leave(Icon *icon, void *d, time_t s, time_t u)
{
  if (icon->taken) {
    icon->ttl = 0;
    icon->initImposedMovement(icon->ttl);
    icon->taken = false;
  }
}

void Icon::drop(Icon *icon, void *d, time_t s, time_t u)
{
  if (! icon->taken) {
    icon->enablePermanentMovement();
    icon->ttl = 0.;
    icon->initImposedMovement(icon->ttl);
    icon->enableBehavior(COLLIDE_ONCE);
  }
}

void Icon::open(Icon *icon, void *d, time_t s, time_t u)
{
  char *p, ext[8] = "???";
  memset(ext+3, 0, sizeof(ext)-3);

  Format::getExt(icon->names.url, ext);
  switch (Format::getPlayerByExt(ext)) {
  case PLAYER_PS:  Ps::view(icon->names.url); break;
  case PLAYER_PDF: Pdf::start(icon->names.url); break;
  case PLAYER_WGET:
    if ((p = strrchr(icon->names.url, '/')))
      Cache::download(icon->names.url, ++p, "");
    break;
  case PLAYER_OFFICE: Office::start(icon->names.url); break;
  case PLAYER_MP3:    Mp3::start(icon->names.url); break;
  case PLAYER_MIDI:   Midi::start(icon->names.url); break;
  default:            Browser::start(icon->names.url, false);  // without rewrite
  }
}

void Icon::save(Icon *icon, void *d, time_t s, time_t u)
{
  chdir(::g.env.cwd());
  char *pfile;
  if (icon->names.url[strlen(icon->names.url) - 1] == '/') {
    Cache::download(icon->names.url, NULL, "");
    trace(DBG_FORCE, "web %s saved", icon->names.url);
  }
  else if ((pfile = strrchr(icon->names.url, '/'))) {
    Cache::download(icon->names.url, ++pfile, "");
    trace(DBG_FORCE, "file %s saved", pfile);
  }
  else notice("nothing to save");
}

void Icon::turn(Icon *icon, void *d, time_t s, time_t u)
{
  clearV3(icon->move.lspeed);
  clearV3(icon->move.aspeed);
  icon->move.aspeed.v[0] = icon->aspeed * 2;
  icon->ttl = Icon::TTL;
  icon->initImposedMovement(icon->ttl);
  icon->disablePermanentMovement();
}

void Icon::destroy(Icon *icon, void *d, time_t s, time_t u)
{
  if (icon->removeFromScene()) {
    // remove file
    char ficon[URL_LEN];

    Cache::file(icon->names.url, ficon);
    chdir(::g.env.icons());
    chdir(icon->worldName());
    unlink(ficon);
    chdir(::g.env.cwd());
  }
}

void Icon::get_url(Icon *icon, Payload *pp)
{
  if (icon) {
    pp->getPayload("s", icon->names.url);
    trace(DBG_WO, "get_url: %s", icon->names.url);
  }
}

void Icon::put_url(Icon *icon, Payload *pp)
{
  if (icon) pp->putPayload("s", icon->names.url);
}

void Icon::get_tex(Icon *icon, Payload *pp)
{
  if (icon) {
    icon->tex = new char[URL_LEN];
    pp->getPayload("s", icon->tex);
    if (icon->tex) trace(DBG_WO, "get_tex: %s", icon->tex);
  }
}

void Icon::put_tex(Icon *icon, Payload *pp)
{
  if (icon && icon->tex) pp->putPayload("s", icon->tex);
}

void Icon::get_gname(Icon *icon, Payload *pp)
{
  if (icon) {
    pp->getPayload("s", icon->named());
    if (icon->named()) trace(DBG_WO,"get_gname: %s", icon->named());
  }
}

void Icon::put_gname(Icon *icon, Payload *pp)
{
  if (icon && icon->getInstance()) pp->putPayload("s", icon->getInstance());
}

void Icon::quit()
{
  // Save icons for persistency
  if ((! taken) && (! remove) && *names.url) {
    char ficon[URL_LEN];

    Cache::file(names.url, ficon);

    if (chdir(::g.env.icons()) != -1 ) {
      struct stat bufstat;
      if (stat(worldName(), &bufstat) < 0) mkdir(worldName(), 0700);
      chdir(worldName());

      FILE *fp;
      if ((fp = File::openFile(ficon, "r")) != NULL) {
        File::closeFile(fp);
        unlink(ficon);
      }
      if ((fp = File::openFile(ficon, "w")) != NULL) {
        char buf[BUFSIZ];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "name=\"%s\" pos=\"%.2f %.2f %.2f %.2f %.2f\" owner=\"%s\" solid dim=\"%.2f %.2f %.2f\" dif=\"%s\" xn=\"%s\" ",
                getInstance(),
                pos.x, pos.y, pos.z, pos.az, pos.ax,
                names.owner[0] ? names.owner : "",
                WIDTH, DEPTH, HEIGHT, COLOR, tex);
        strcat(buf, "\n");
        fputs(buf, fp);
        File::closeFile(fp);
      }
      chdir(::g.env.cwd());
    }
    else error("can't chdir to %s", ::g.env.icons());
  }

  if (tex) delete[] tex;
  free(ifile); ifile = NULL;
  if (ofile) delete[] ofile;
  free(vref); vref = NULL;
}

void Icon::funcs()
{
  getPropertyFunc(ICON_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(ICON_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(ICON_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(ICON_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(ICON_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(ICON_TYPE, PROPHNAME, WO_PAYLOAD get_hname);
  getPropertyFunc(ICON_TYPE, PROPURL, WO_PAYLOAD get_url);
  getPropertyFunc(ICON_TYPE, PROPTEX, WO_PAYLOAD get_tex);
  getPropertyFunc(ICON_TYPE, PROPGNAME, WO_PAYLOAD get_gname);

  putPropertyFunc(ICON_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(ICON_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(ICON_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(ICON_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(ICON_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(ICON_TYPE, PROPHNAME, WO_PAYLOAD put_hname);
  putPropertyFunc(ICON_TYPE, PROPURL, WO_PAYLOAD put_url);
  putPropertyFunc(ICON_TYPE, PROPTEX, WO_PAYLOAD put_tex);
  putPropertyFunc(ICON_TYPE, PROPGNAME, WO_PAYLOAD put_gname);

  setActionFunc(ICON_TYPE, OPEN, WO_ACTION open, "Open");
  setActionFunc(ICON_TYPE, SAVE, WO_ACTION save, "Save");
  setActionFunc(ICON_TYPE, PIN, WO_ACTION pin, "Pin");
  setActionFunc(ICON_TYPE, LEAVE, WO_ACTION leave, "Leave");
  setActionFunc(ICON_TYPE, CARRY, WO_ACTION carry, "Carry");
  setActionFunc(ICON_TYPE, KILL, WO_ACTION destroy, "Destroy");
  setActionFunc(ICON_TYPE, PUSH, WO_ACTION push, "Push");
  setActionFunc(ICON_TYPE, PULL, WO_ACTION pull, "Pull");
  setActionFunc(ICON_TYPE, DROP, WO_ACTION drop, "Drop");
  setActionFunc(ICON_TYPE, TURN, WO_ACTION turn, "Turn");
  setActionFunc(ICON_TYPE, MOVE, WO_ACTION moveObject, "Move");
  setActionFunc(ICON_TYPE, CREATE, WO_ACTION create, "");
  setActionFunc(ICON_TYPE, STICK, WO_ACTION stick, "");
}
