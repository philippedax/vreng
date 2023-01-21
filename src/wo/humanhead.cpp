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
#include "humanhead.hpp"
#include "format.hpp"
#include "user.hpp"	// localuser
#include "model.hpp"	// Model


const OClass Humanhead::oclass(HUMANHEAD_TYPE, "Humanhead", Humanhead::creator);

//local
static Humanhead *phead = NULL;
static uint16_t oid = 0;

struct sHumanhead {
  uint8_t head_id;
  const char head_str[16];
};

static struct sHumanhead heads[] = {
  {Humanhead::MALE, "male"},
  {Humanhead::FEMALE, "female"},
  {Humanhead::NONE, ""},
};


WObject * Humanhead::creator(char *l)
{
  return new Humanhead(l);
}

void Humanhead::defaults()
{
  model_e = MALE;
  scale = 1;
  color[0] = 1.; color[1] = .75; color[2] = .7;
}

void Humanhead::parser(char *l)
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
      if      (! stringcmp(str, "male"))   { model_e = MALE; }
      else if (! stringcmp(str, "female")) { model_e = FEMALE; }
    }
  }
  end_while_parse(l);
}

/* solid geometry */
void Humanhead::makeSolid()
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

uint8_t Humanhead::getModel(const char *name)
{
  if (name) {
    for (struct sHumanhead *pheads = heads ; pheads; pheads++) {
      if (! strcmp(name, pheads->head_str)) {
        return pheads->head_id;
      }
    }
  }
  return NONE;
}

void Humanhead::behavior()
{
  enableBehavior(SPECIFIC_RENDER);
  enableBehavior(DYNAMIC);
  enableBehavior(NO_BBABLE);

  initMobileObject(0);
  enablePermanentMovement();	// follows user
  createPermanentNetObject(PROPS, ++oid);
}

/* Sets an unique name */
void Humanhead::setName(const char *modelname)
{
  sprintf(names.given,"%s&%s", modelname, localuser->getInstance());
  updateNames();
}

void Humanhead::setName()
{
  setName(typeName());
}

/* special initializations */
void Humanhead::inits()
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
Humanhead::Humanhead(char *l)
{
  dx = dy = dz = 0;
  dax = day = daz = 0;
  taken = true;
  visible = true;
  model_t = 0;
  strcpy(modelname, "male");
  parser(l);
  behavior();
  inits();
  phead = this;
}

/* Regenerated by world via VRSql */
Humanhead::Humanhead(User *user, void *d, time_t s, time_t u)
{
  char *str = (char *) d;       // name transmitted
  char *p;
  if (!str) return;

  strcpy(names.given, str);
  strcpy(names.type, typeName());     // need names.type for VRSql
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
Humanhead::Humanhead(User *user, const char *url, const float *skin)
{
  if (! url) return;

  dx = dy = dz = 0;
  dax = day = daz = 0;
  taken = true;
  visible = true;
  model_t = 0;
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

void Humanhead::changePermanent(float lasting)
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

void Humanhead::visibility(bool flag)
{
  visible = flag;
}

void Humanhead::render()
{
  if (visible)
    model->render(pos, color);
}

void Humanhead::quit()
{
  phead = NULL;
  oid = 0;
}

/* Creation: this method is invisible: called by the World */
void Humanhead::recreate_cb(User *user, void *d, time_t s, time_t u)
{
  new Humanhead(user, d, s, u);
}

void Humanhead::funcs()
{
  getPropertyFunc(HUMANHEAD_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(HUMANHEAD_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(HUMANHEAD_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(HUMANHEAD_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(HUMANHEAD_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(HUMANHEAD_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(HUMANHEAD_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(HUMANHEAD_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(HUMANHEAD_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(HUMANHEAD_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(HUMANHEAD_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(HUMANHEAD_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(HUMANHEAD_TYPE, RECREATE, _Action recreate_cb, "");
}