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
#include "head.hpp"
#include "format.hpp"
#include "user.hpp"	// localuser
#include "model.hpp"	// Model


const OClass Head::oclass(HEAD_TYPE, "Head", Head::creator);

//local
static Head *phead = NULL;
static uint16_t oid = 0;

struct sHead {
  uint8_t head_id;
  const char head_str[16];
};

static struct sHead heads[] = {
  {Head::MALE, "male"},
  {Head::FEMALE, "female"},
  {Head::NONE, ""},
};


WObject * Head::creator(char *l)
{
  return new Head(l);
}

void Head::defaults()
{
  ttl = MAXFLOAT;
  dx = dy = dz = 0;
  dax = day = daz = 0;
  taken = true;
  scale = 1;
  visible = true;
  model_e = MALE;
  model_t = 0;
  color[0] = 1.; color[1] = .75; color[2] = .7;
}

void Head::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "url="))   l = parse()->parseUrl(l, names.url);
    else if (!stringcmp(l, "scale=")) l = parse()->parseFloat(l, &scale, "scale");
    else if (!stringcmp(l, "color=")) l = parse()->parseVector3f(l, color, "color");
    else if (!stringcmp(l, "model=")) {
      char str[16];
      l = parse()->parseString(l, str, "model");
      if      (!stringcmp(str, "male")) { model_e = MALE; }
      else if (!stringcmp(str, "female")) { model_e = FEMALE; }
    }
  }
  end_while_parse(l);
}

/* solid geometry */
void Head::makeSolid()
{
  char s[256];
  switch (model_e) {
    case MALE:
    case FEMALE:
      sprintf(s,"solid shape=\"bbox\" dim=\"%f %f %f\" />",.15,.15,.2); break;
    default: return;
  }
  parse()->parseSolid(s, SEP, this);
}

uint8_t Head::getModel(const char *name)
{
  if (name) {
    for (struct sHead *pheads = heads ; pheads; pheads++)
      if (! strcmp(name, pheads->head_str)) return pheads->head_id;
  }
  return NONE;
}

void Head::behavior()
{
  enableBehavior(SPECIFIC_RENDER);
  enableBehavior(DYNAMIC);
  enableBehavior(NO_BBABLE);

  initMobileObject(0);
  enablePermanentMovement();	// follows user
  createPermanentNetObject(PROPS, ++oid);
}

/* Sets an unique name */
void Head::setName(const char *modelname)
{
  sprintf(names.given,"%s&%s", modelname, localuser->getInstance());
  updateNames();
}

void Head::setName()
{
  setName(typeName());
}

/* special initializations */
void Head::inits()
{
  model_t = Format::getModelByUrl(names.url);
  switch (model_t) {
    case MODEL_LWO: scale = LWO_SCALE; break;
    case MODEL_3DS: scale = _3DS_SCALE; break;
  }

  model = new Model(localuser, names.url, scale);
  switch (model_e) {
    case MALE:
    case FEMALE:
      dz -= .10; break;
  }

  // sets position near the avatar
  pos.x = localuser->pos.x + dx;
  pos.y = localuser->pos.y + dy;
  pos.z = localuser->pos.z + dz;
  pos.ax = localuser->pos.ax + dax;
  pos.ay = localuser->pos.ay + day;
  pos.az = localuser->pos.az + daz;
  updatePosition();
}

/* Creation from xml file */
Head::Head(char *l)
{
  model_e = Head::MALE;
  strcpy(modelname, "male");
  parser(l);
  behavior();
  inits();
  phead = this;
}

/* Regenerated by world via MySql */
Head::Head(User *user, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // name transmitted
  char *p;
  if (!str) return;

  strcpy(names.given, str);
  strcpy(names.type, typeName());     // need names.type for MySql
  p = strchr(str, '&');
  *p = '\0';
  strcpy(modelname, str);

  phead = this;
  defaults();
  model_e = getModel(modelname);
  makeSolid();
  setName(modelname);
  setOwner();
  behavior();
  inits();
}

/* Launched by user via prefs */
Head::Head(User *user, const char *url, const float *skin)
{
  if (! url) return;

  strcpy(names.url, url);
  sprintf(modelname, "head");
  defaults();
  for (int i=0; i<3; i++) color[i] = skin[i];
  makeSolid();
  setName(modelname);
  setOwner();
  behavior();
  inits();
  phead = this;
}

void Head::changePermanent(float lasting)
{
  if (! taken) return;

  pos.x = localuser->pos.x + dx;
  pos.y = localuser->pos.y + dy;
  switch (model_t) {
  case MODEL_LWO: pos.z = localuser->pos.z + (localuser->height / 2) - .17; break;
  case MODEL_OFF: pos.z = localuser->pos.z - (localuser->height / 2) - .03; break;
  case MODEL_3DS: pos.z = localuser->pos.z + (localuser->height / 2) - .15; break;
  case MODEL_OBJ: pos.z = localuser->pos.z - (localuser->height / 2) - .10; break;
  default: pos.z = localuser->pos.z - (localuser->height / 2) - .10; break;
  }
  switch (model_t) {
  case MODEL_LWO: /* dax = M_PI_2; daz = -M_PI_2;*/ break;
  case MODEL_OFF: daz = M_PI_2; break;
  case MODEL_3DS: dax = M_PI_2; break;
  case MODEL_OBJ: daz = M_PI; break;
  }
  pos.az = localuser->pos.az + daz;
  pos.ax = localuser->pos.ax + dax;
  pos.ay = localuser->pos.ay + day;
  //error("head: %.2f %.2f %.2f %.2f %.2f %.2f", pos.x,pos.y,pos.z,pos.ax,pos.ay,pos.az);
  updatePosition();
}

void Head::visibility(bool flag)
{
  visible = flag;
}

void Head::render()
{
  if (visible)
    model->render(pos, color);
}

void Head::quit()
{
  phead = NULL;
  oid = 0;
}

/* Creation: this method is invisible: called by the World */
void Head::recreate_cb(User *user, void *d, time_t s, time_t u)
{
  new Head(user, d, s, u);
}

void Head::funcs()
{
  getPropertyFunc(HEAD_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(HEAD_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(HEAD_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(HEAD_TYPE, PROPAX, WO_PAYLOAD get_ax);
  getPropertyFunc(HEAD_TYPE, PROPAY, WO_PAYLOAD get_ay);
  getPropertyFunc(HEAD_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(HEAD_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(HEAD_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(HEAD_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(HEAD_TYPE, PROPAX, WO_PAYLOAD put_ax);
  putPropertyFunc(HEAD_TYPE, PROPAY, WO_PAYLOAD put_ay);
  putPropertyFunc(HEAD_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  setActionFunc(HEAD_TYPE, RECREATE, WO_ACTION recreate_cb, "");
}
