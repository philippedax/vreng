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
  {Hat::NONE,    ""},
};

WObject * Hat::creator(char *l)
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
  model = NONE;
}

/* solid geometry */
void Hat::makeSolid()
{
  char s[256];

  switch (model) {

  case Hat::TOPHAT:
    sprintf(s,"solid shape=\"cone\" ri=\"%f\" re=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.08,.15,"black");
    parse()->parseSolid(s, SEP, this);
    sprintf(s,"solid shape=\"disk\" rel=\"0 0 .15 0 0\" ri=\"0\" re=\"%f\" dif=\"%s\" />",.08,"black");
    parse()->parseSolid(s, SEP, this);
    sprintf(s,"solid shape=\"disk\" rel=\"0 0 0 0 0\" ri=\"%f\" re=\"%f\" dif=\"%s\" />",.08,.15,"black");
    parse()->parseSolid(s, SEP, this);
    break;

  case Hat::BOWLER:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.05,.12,"green");
    parse()->parseSolid(s, SEP, this);
    sprintf(s,"solid shape=\"torus\" rel=\"0 0 0 0 0\" r=\"%f\" rt=\"%f\" dif=\"%s\" />",.08,.02,"darkgreen");
    parse()->parseSolid(s, SEP, this);
    break;

  case Hat::CHINESE:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.2,.0,.15,"yellow");
    parse()->parseSolid(s, SEP, this);
    break;

  case Hat::REDHAT:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.08,0.5,.10,"red");
    parse()->parseSolid(s, SEP, this);
    sprintf(s,"solid shape=\"disk\" rel=\"0 0 0 0 0\" ri=\"%f\" re=\"%f\" dif=\"%s\" />",.08,.12,"red");
    parse()->parseSolid(s, SEP, this);
    break;

  case Hat::COUNTRY:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" style=\"1\" />",.08,.04,.12,"wheat");
    parse()->parseSolid(s, SEP, this);
    sprintf(s,"solid shape=\"torus\" r=\"%f\" rt=\"%f\" dif=\"%s\" style=\"1\" />",.08,.02,"wheat");
    parse()->parseSolid(s, SEP, this);
    break;

  case Hat::CLOWN:
    sprintf(s,"solid shape=\"cone\" rb=\"%f\" rt=\"%f\" h=\"%f\" dif=\"%s\" />",.08,.0,.30,"white");
    parse()->parseSolid(s, SEP, this);
    break;
  default:
    break;
  }
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
  return NONE;
}

/* Created from file */
void Hat::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);	// <solid ... />
    if (!l) break;
    if (! stringcmp(l, "model=")) {
      l = parse()->parseString(l, modelname, "model");
      if      (! stringcmp(modelname, "tophat"))   model = TOPHAT;
      else if (! stringcmp(modelname, "bowler"))   model = BOWLER;
      else if (! stringcmp(modelname, "chinese"))  model = CHINESE;
      else if (! stringcmp(modelname, "redhat"))   model = REDHAT;
      else if (! stringcmp(modelname, "country"))  model = COUNTRY;
      else if (! stringcmp(modelname, "clown"))    model = CLOWN;
    }
    else
     model = NONE;
  }
  end_while_parse(l);
}

/* Created from file */
Hat::Hat(char *l)
{
  taken = false;
  model = NONE;
  parser(l);
  behavior();
  if (model != NONE) {
    makeSolid();
  }
}

/* Regenerated by world via VRSql */
Hat::Hat(User *user, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // name transmitted
  char *p;
  if (!str) return;

  strcpy(names.given, str);
  strcpy(names.type, typeName());
  p = strchr(str, '&');
  *p = '\0';
  strcpy(modelname, str);

  phat = this;
  defaults();
  taken = true;
  model = getModel(modelname);
  makeSolid();
  setName(modelname);
  setOwner();
  getPersist();
  behavior();
  inits();
}

void Hat::quit()
{
  phat = NULL;
  oid = 0;
  savePersistency();
}

/* wear */
void Hat::wear()
{
  if (taken) takeoff();

  defaults();
  taken = true;
  model = getModel(modelname);
  setName(modelname);
  setOwner();
  behavior();
  inits();
  setPersist();
  createPermanentNetObject(PROPS, ++oid);
  addToWearList();
}

/* takeoff */
void Hat::takeoff()
{
  if (! taken) return;

  // restore original position
  restorePosition();
  delPersist();
  delFromWearList();
  taken = false;
}

/* Deletion */
void Hat::destroy()
{
  taken = false;
  delPersist();
  delFromWearList();
  toDelete();
  phat = NULL;
}

/* wear: indirectly called by user */
void Hat::wear_cb(Hat *hat, void *d, time_t s, time_t u)
{
  hat->wear();
}

/* takeoff: indirectly called by user */
void Hat::takeoff_cb(Hat *hat, void *d, time_t s, time_t u)
{
  hat->takeoff();
}

/* Deletion: this method is invisible, directly called by user */
void Hat::destroy_cb(User *user, void *d, time_t s, time_t u)
{
  if (phat) phat->destroy();
}

/* Creation: this method is invisible: called by the World */
void Hat::recreate_cb(User *user, void *d, time_t s, time_t u)
{   
  new Hat(user, d, s, u);
}

void Hat::funcs()
{
  getPropertyFunc(HAT_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(HAT_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(HAT_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(HAT_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(HAT_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(HAT_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(HAT_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(HAT_TYPE, PROPZ, WO_PAYLOAD put_z); 
  putPropertyFunc(HAT_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(HAT_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(HAT_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(HAT_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(HAT_TYPE, WEAR, WO_ACTION wear_cb, "Wear");
  setActionFunc(HAT_TYPE, TAKEOFF, WO_ACTION takeoff_cb, "Takeoff");

  setActionFunc(HAT_TYPE, RECREATE, WO_ACTION recreate_cb, "");
  setActionFunc(HAT_TYPE, DESTROY, WO_ACTION destroy_cb, "");
}
