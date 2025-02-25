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
// hat.cpp
//
// Hat handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "hat.hpp"
#include "user.hpp"	// User


const OClass Hat::oclass(HAT_TYPE, "Hat", Hat::creator);

//local
static Hat *phat = NULL;
static uint16_t oid = 0;

struct sHat {
  uint8_t hat_id;
  const char hat_str[16];
};

static struct sHat hats[] = {
  {Hat::TOPHAT,  "tophat"},
  {Hat::BOWLER,  "bowler"},
  {Hat::CHINESE, "chinese"},
  {Hat::REDHAT,  "redhat"},
  {Hat::COUNTRY, "country"},
  {Hat::CLOWN,   "clown"},
  {Hat::NOHAT,   ""},
};

Object * Hat::creator(char *l)
{ 
  return new Hat(l);
}

void Hat::defaults()
{
  article = Cloth::HAT;
  ttl = MAXFLOAT;
  dx = 0;
  dy = 0;
  dz = -0.1;		// above the head
  dax = 0;
  day = 0;
  daz = 0;
  model = NOHAT;
}

/** solid geometry */
void Hat::geometry()
{
  char s[256];

  switch (model) {

  case Hat::TOPHAT:
    sprintf(s,"solid shape=\"cone\" ri=\"%f\" re=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.08,.15,"black");
    parseSolid(s);
    sprintf(s,"<solid shape=\"disk\" rel=\"0 0 .15 0 0\" ri=\"0\" re=\"%f\" dif=\"%s\" />",.08,"black");
    parseSolid(s);
    sprintf(s,"<solid shape=\"disk\" rel=\"0 0 0 0 0\" ri=\"%f\" re=\"%f\" dif=\"%s\" />",.08,.15,"black");
    parseSolid(s);
    break;

  case Hat::BOWLER:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.05,.12,"green");
    parseSolid(s);
    sprintf(s,"solid shape=\"torus\" rel=\"0 0 0 0 0\" r=\"%f\" rt=\"%f\" dif=\"%s\" />",.08,.02,"darkgreen");
    parseSolid(s);
    break;

  case Hat::CHINESE:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.2,.0,.15,"yellow");
    parseSolid(s);
    break;

  case Hat::REDHAT:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.05,.10,"red");
    parseSolid(s);
    sprintf(s,"solid shape=\"disk\" rel=\"0 0 0 0 0\" ri=\"%f\" re=\"%f\" dif=\"%s\" />",.08,.12,"red");
    parseSolid(s);
    break;

  case Hat::COUNTRY:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" style=\"1\" />",.08,.04,.12,"wheat");
    parseSolid(s);
    sprintf(s,"solid shape=\"torus\" r=\"%f\" rt=\"%f\" dif=\"%s\" style=\"1\" />",.08,.02,"wheat");
    parseSolid(s);
    break;

  case Hat::CLOWN:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.0,.30,"white");
    parseSolid(s);
    break;

  default:
    break;
  }
}

void Hat::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(TAKABLE);
  if (taken) {
    enableBehavior(DYNAMIC);
    enableBehavior(TRANSCIENT);
  }
}

/** sets position near the avatar */
void Hat::inits()
{
  clothObject(ttl);
  if (taken)
    permanentMovement();  // follows user

  if (! taken)  return;
  if (localuser) dz += localuser->height / 2;

  // save original position
  ox = pos.x; oy = pos.y; oz = pos.z;
  oax = pos.ax; oay = pos.ay; oaz = pos.az;

  // set new position
  if (localuser) {
    pos.x = localuser->pos.x + dx;
    pos.y = localuser->pos.y + dy;
    pos.z = localuser->pos.z + dz;
    pos.az = localuser->pos.az + daz;
  }
  pos.ax = dax;
  pos.ay = day;

  updatePosition();
}


uint8_t Hat::getModel(const char *name)
{
  if (name) {
    struct sHat *phats = hats;

    for ( ; phats; phats++) {
      if (! strcmp(name, phats->hat_str))
        return phats->hat_id;
    }
  }
  return NOHAT;
}

/** Parses from file */
void Hat::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);	// <solid ... />
    if (!l) break;
    if (! stringcmp(l, "model=")) {
      l = parseString(l, modelname, "model");
      //echo("hat: %s", modelname);
      if      (! stringcmp(modelname, "tophat"))   model = TOPHAT;
      else if (! stringcmp(modelname, "bowler"))   model = BOWLER;
      else if (! stringcmp(modelname, "chinese"))  model = CHINESE;
      else if (! stringcmp(modelname, "redhat"))   model = REDHAT;
      else if (! stringcmp(modelname, "country"))  model = COUNTRY;
      else if (! stringcmp(modelname, "clown"))    model = CLOWN;
    }
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

/** Created from file */
Hat::Hat(char *l)
{
  taken = false;
  model = NOHAT;
  parser(l);
  behaviors();
  geometry();
  inits();
}

/** Regenerated by world via VSql */
Hat::Hat(User *user, void *d, time_t s, time_t u)
{
  char *str = static_cast<char *>(d);       // name transmitted
  if (!str) return;

  strcpy(name.given, str);
  strcpy(name.type, typeName());
  char *p = strchr(str, '&');
  *p = '\0';
  strcpy(modelname, str);

  phat = this;
  defaults();
  taken = true;
  //model = getModel(modelname);
  setOwner();
  getPersist();
  geometry();
  behaviors();
  inits();
}

void Hat::quit()
{
  phat = NULL;
  oid = 0;
  savePersist();
}

/** wear */
void Hat::wear()
{
  if (taken) takeoff();

  defaults();
  taken = true;
  setOwner();
  behaviors();
  inits();
  setPersist();
  createNetObj(PROPS);
  addWearList();
}

/** takeoff */
void Hat::takeoff()
{
  if (! taken) return;

  taken = false;
  restorePosition();	// restore original position
  delPersist();
  delWearList();
}

/** Deletion */
void Hat::destroy()
{
  taken = false;
  delPersist();
  delWearList();
  toDelete();
  phat = NULL;
}

/** wear: indirectly called by user */
void Hat::wear_cb(Hat *hat, void *d, time_t s, time_t u)
{
  hat->wear();
}

/** takeoff: indirectly called by user */
void Hat::takeoff_cb(Hat *hat, void *d, time_t s, time_t u)
{
  hat->takeoff();
}

/** Deletion: this method is invisible, directly called by user */
void Hat::destroy_cb(User *user, void *d, time_t s, time_t u)
{
  if (phat) phat->destroy();
}

/** Creation: this method is invisible: called by the World */
void Hat::recreate_cb(User *user, void *d, time_t s, time_t u)
{   
  new Hat(user, d, s, u);
}

void Hat::funcs()
{
  getPropFunc(HAT_TYPE, PROPXY, _Payload get_xy);
  getPropFunc(HAT_TYPE, PROPZ, _Payload get_z);
  getPropFunc(HAT_TYPE, PROPAZ, _Payload get_az);
  getPropFunc(HAT_TYPE, PROPAX, _Payload get_ax);
  getPropFunc(HAT_TYPE, PROPAY, _Payload get_ay);
  getPropFunc(HAT_TYPE, PROPHNAME, _Payload get_hname);

  putPropFunc(HAT_TYPE, PROPXY, _Payload put_xy);
  putPropFunc(HAT_TYPE, PROPZ, _Payload put_z); 
  putPropFunc(HAT_TYPE, PROPAZ, _Payload put_az);
  putPropFunc(HAT_TYPE, PROPAX, _Payload put_ax);
  putPropFunc(HAT_TYPE, PROPAY, _Payload put_ay);
  putPropFunc(HAT_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(HAT_TYPE, WEAR, _Action wear_cb, "Wear");
  setActionFunc(HAT_TYPE, TAKEOFF, _Action takeoff_cb, "Takeoff");
  setActionFunc(HAT_TYPE, RECREATE, _Action recreate_cb, "");
  //dax setActionFunc(HAT_TYPE, DESTROY, _Action destroy_cb, "");
}
