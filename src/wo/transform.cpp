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
#include "transform.hpp"


const OClass Transform::oclass(TRANSFORM_TYPE, "Transform", Transform::creator);

void Transform::funcs() {}


WObject * Transform::creator(char *l)
{
  return new Transform(l);
}

void Transform::addList(uint8_t op, GLfloat x=0, GLfloat y=0, GLfloat z=0, GLfloat a=0)
{
  opl->op = op;
  opl->x = x;
  opl->y = y;
  opl->z = z;
  opl->a = a;
  opl->next = NULL;

  opl = new sOplist[1];
}

Transform::Transform(char *l)
{
  char opstr[16];
  opl = opList = new sOplist[1];;

  l = tokenize(l);
  while (l) {
    if (! stringcmp(l, "push")) {		// <local>
      l = parseString(l, opstr);
      addList(PUSH);
    }
    else if (! stringcmp(l, "pop")) {		// </local>
      l = parseString(l, opstr);
      addList(POP);
    }
    else if (! stringcmp(l, "rot") || !stringcmp(l, "rotation")) {
      l = parseRotation(l, rot);
      addList(ROT, rot.x, rot.y, rot.z, rot.az);
    }
    else if (! stringcmp(l, "trans") || !stringcmp(l, "translation")) {
      l = parseTranslation(l, trans);
      addList(TRANS, trans.x, trans.y, trans.z);
    }
    else if (! stringcmp(l, "scale")) {
      l = parseTranslation(l, scale);
      addList(SCALE, scale.x, scale.y, scale.z);
    }
  }

  enableBehavior(SPECIFIC_RENDER);
  initObject(INVISIBLE);

  char s[128];
  sprintf(s, "solid shape=\"none\" />");
  parseSolid(s);
}

void Transform::render()
{
  for (opl = opList; opl ; opl = opl->next) {
    switch (opl->op) {
    case PUSH:  glPushMatrix();
                break;
    case POP:   glPopMatrix();
                break;
    case ROT:   glRotatef(opl->a, opl->x, opl->y, opl->z);
                break;
    case TRANS: glTranslatef(opl->x, opl->y, opl->z);
                break;
    case SCALE: glScalef(opl->x, opl->y, opl->z);
                break;
    }
  }
}

void Transform::quit()
{
  for (opl = opList; opl ; ) {
    struct sOplist *tmp = opl;
    opl = opl->next;
    delete[] tmp;
  }
}
