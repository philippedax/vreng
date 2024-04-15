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
// model.cpp
//
// Model handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "model.hpp"
#include "move.hpp"	// gotoFront
#include "format.hpp"	// getModelByMime
#include "music.hpp"	// MUSIC_TYPE
#include "texture.hpp"	// Texture::open
#include "user.hpp"	// localuser
#include "lwo.hpp"      // Lwo
#include "3ds.hpp"      // 3ds
#include "ase.hpp"      // Ase
#include "obj.hpp"      // Obj
#include "md2.hpp"      // Md2
#include "dxf.hpp"      // Dxf
#include "off.hpp"      // Off
#include "x3d.hpp"      // X3d
#include "bvh.hpp"      // Bvh
#include "man.hpp"      // Man


const OClass Model::oclass(MODEL_TYPE, "Model", Model::creator);

const float Model::DEF_SCALE = 1;       ///< 1

// local
static uint16_t oid = 0;


/** Creates model class */
WO * Model::creator(char *l)
{
  return new Model(l);
}

/** Sets defaults before parsing */
void Model::defaults()
{
  scale = DEF_SCALE;
  state = INACTIVE;
  texid = 0;
  taken = false;
  color[0] = 1; color[1] = .75; color[2] = .7;
}

/** Constructor : creation from vre file */
Model::Model(char *l)
 :
 texurl(NULL),
 bvhurl(NULL),
 sndurl(NULL),
 lwo(NULL),
 ds3(NULL),
 ase(NULL),
 obj(NULL),
 md2(NULL),
 dxf(NULL),
 off(NULL),
 x3d(NULL),
 bvh(NULL),
 man(NULL)
{
  parser(l);
  geometry();
  behaviors();
  loader();
  scaler();
  drawer();

  initMobileObject(1);
}

/** Sets solid geometry */
void Model::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%f %f %f\" />", .3, .16, 1.7);
  parseSolid(s);
}

/** Sets name */
void Model::setName()
{
  sprintf(name.given, "%s-%s.%d", MODEL_NAME, localuser->objectName(), getNum());
}

/** Parse a model */
void Model::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url="))   l = parseUrl(l, name.url);
    else if (! stringcmp(l, "scale=")) l = parseFloat(l, &scale, "scale");
    else if (! stringcmp(l, "color=")) l = parseVector3f(l, color, "color");
    else if (! stringcmp(l, "bvh=")) {
      bvhurl = new char[URL_LEN];
      l = parseString(l, bvhurl, "bvh");
      bvh = new Bvh(bvhurl);
    }
    else if (! stringcmp(l, "sound=")) {
      sndurl = new char[URL_LEN];
      l = parseString(l, sndurl, "sound");
    }
    else if (! stringcmp(l, "tex=")) {
      texurl = new char[URL_LEN];
      l = parseString(l, texurl, "tex");
      texid = Texture::open(texurl);
    }
  }
  end_while_parse(l);
}

/** Sets behaviors */
void Model::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);
}

/** Creation from Gui addobj */
Model::Model(WO *user, char *url, float _scale)
{
  defaults();
  taken = true;
  geometry();
  behaviors();
  enableBehavior(DYNAMIC);

  setName();
  setOwner();
  //dax1 name.url = new char[URL_LEN];
  strcpy(name.url, url);
  loader();
  scale = _scale;
  scaler();
  drawer();

  /* position = in front of user */
  pos.x = localuser->pos.x + 1;
  pos.y = localuser->pos.y;
  pos.z = localuser->pos.z;
  updatePosition();

  initMobileObject(1);
}

/** Loads a model */
void Model::loader()
{
  if ((name.url && *name.url == 0) || (name.url && ! strcmp(name.url, "man"))) {
    model_t = MODEL_MAN;	// hugly hack!!!
    disableBehavior(SPECIFIC_RENDER);
  }
  else {	// normal url
    model_t = Format::getModelByUrl(name.url);
  }

  switch (model_t) {
  case MODEL_LWO: lwo = new Lwo(name.url); break;
  case MODEL_3DS: ds3 = new _3ds(name.url); break;
  case MODEL_ASE: ase = new Ase(name.url); break;
  case MODEL_OBJ: obj = new Obj(name.url); break;
  case MODEL_MD2: md2 = new Md2(name.url); break;
  case MODEL_DXF: dxf = new Dxf(name.url); break;
  case MODEL_OFF: off = new Off(name.url); break;
  case MODEL_X3D: x3d = new X3d(name.url); break;
  case MODEL_MAN: man = new Man(); break;
  default: error("Model: unknown type=%d url=%s", model_t, name.url); return;
  }
}

/** Scales a model */
void Model::scaler()
{
  if (scale == 1) {
    switch (model_t) {
    case MODEL_LWO: if (lwo) scale = lwo->getScale(); break;
    case MODEL_3DS: if (ds3) scale = ds3->getScale(); break;
    case MODEL_ASE: if (ase) scale = ase->getScale(); break;
    case MODEL_OBJ: if (obj) scale = obj->getScale(); break;
    case MODEL_MD2: if (md2) scale = md2->getScale(); break;
    case MODEL_DXF: if (dxf) scale = dxf->getScale(); break;
    case MODEL_OFF: break;
    case MODEL_X3D: break;
    case MODEL_MAN: break;
    }
  }
  else {
    switch (model_t) {
    case MODEL_LWO: if (lwo) lwo->setScale(scale); break;
    case MODEL_3DS: if (ds3) ds3->setScale(scale); break;
    case MODEL_ASE: if (ase) ase->setScale(scale); break;
    case MODEL_OBJ: if (obj) obj->setScale(scale); break;
    case MODEL_MD2: if (md2) md2->setScale(scale); break;
    case MODEL_DXF: if (dxf) dxf->setScale(scale); break;
    case MODEL_OFF: if (off) off->setScale(scale); break;
    case MODEL_X3D: break;
    case MODEL_MAN: break;
    }
  }
}

/** Draws a model into a displaylist */
void Model::drawer()
{
  switch (model_t) {
  case MODEL_LWO:    if (lwo) lwo->displaylist(); break;
  case MODEL_3DS: /* if (ds3) ds3->displaylist(); */ break;
  case MODEL_ASE: /* if (ase) ase->displaylist(); */ break;
  case MODEL_OBJ: /* if (obj) obj->displaylist(); */ break;
  case MODEL_MD2:    if (md2) md2->displaylist(1, texid); break;
  case MODEL_DXF:    if (dxf) dxf->displaylist(); break;
  case MODEL_OFF:    if (off) off->displaylist(); break;
  case MODEL_MAN:    if (man) man->displaylist(); break;
  case MODEL_X3D: break;
  }
}

/** Is model moving */
bool Model::isMoving()
{
  if (taken) return true;
  return testMoving();
}

/** Permanent movement of a model */
void Model::changePermanent(float lasting)
{
  pos.x = localuser->pos.x;
  pos.y = localuser->pos.y;
  pos.z = localuser->pos.z;
  pos.az = localuser->pos.az;
}

/** Renders position of a model */
void Model::posRender()
{
  if (texid) {
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texid);
  }
  else {
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
  }
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  glRotatef(RAD2DEG(pos.az), 0,0,1);
  glRotatef(RAD2DEG(pos.ax) + 90, 1,0,0);
  glRotatef(RAD2DEG(pos.ay), 0,1,0);
}

/** Renders end */
void Model::endRender()
{
  if (texid)
    glDisable(GL_LIGHTING);
  glPopMatrix();
}

/** Renders a model */
void Model::render()
{
  switch (model_t) {
  case MODEL_LWO: if (lwo) { posRender(); lwo->render();      endRender(); } break;
  case MODEL_3DS: if (ds3) { posRender(); ds3->render(color); endRender(); } break;
  case MODEL_ASE: if (ase) { posRender(); ase->render();      endRender(); } break;
  case MODEL_OBJ: if (obj) { posRender(); obj->render(color); endRender(); } break;
  case MODEL_DXF: if (dxf) { posRender(); dxf->render();      endRender(); } break;
  case MODEL_OFF: if (off) { posRender(); off->render(color); endRender(); } break;
  case MODEL_MD2: if (md2) {              md2->render(pos);                 } break;
  case MODEL_MAN: if (man) {                                                } break;
  case MODEL_X3D: break;
  }
}

/** Quits a model */
void Model::quit()
{
  switch (model_t) {
  case MODEL_LWO: if (lwo) delete lwo; lwo = NULL; break;
  case MODEL_3DS: if (ds3) delete ds3; ds3 = NULL; break;
  case MODEL_ASE: if (ase) delete ase; ase = NULL; break;
  case MODEL_OBJ: if (obj) delete obj; obj = NULL; break;
  case MODEL_MD2: if (md2) delete md2; md2 = NULL; break;
  case MODEL_DXF: if (dxf) delete dxf; dxf = NULL; break;
  case MODEL_OFF: if (off) delete off; off = NULL; break;
  case MODEL_X3D: if (x3d) delete x3d; x3d = NULL; break;
  }
  if (texurl) delete[] texurl;
  if (sndurl) delete[] sndurl;
  if (bvhurl) delete[] bvhurl;
  oid = 0;
}

void Model::sound(Model *model, void *d, time_t s, time_t u)
{
  if (model->sndurl) {
    d = (void *) model->sndurl;
    doAction(MUSIC_TYPE, Music::PLAY, model, d, s, u);
  }
}

void Model::funcs()
{
  setActionFunc(MODEL_TYPE, 0, _Action moveObject, "Move");
  setActionFunc(MODEL_TYPE, 1, _Action sound, "Sound");
}
