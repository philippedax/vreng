//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// button.cpp
//
// Button handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "button.hpp"
#include "netobj.hpp"   // netop
#include "payload.hpp"	// Payload


const OClass Button::oclass(BUTTON_TYPE, "Button", Button::creator);

const uint8_t Button::USENAME_MAX = 8;
const uint8_t Button::ACTION_LEN = 16;

// local
static uint16_t oid = 0;


Object * Button::creator(char *l)
{
  return new Button(l);
}

void Button::defaults()
{
  num_action0 = num_action1 = 0;
  use_names = new char[OBJNAME_LEN * USENAME_MAX];
}

void Button::parser(char *l)
{
  char str_action0[ACTION_LEN];
  char str_action1[ACTION_LEN];
  memset(str_action0, 0, sizeof(str_action0));
  memset(str_action1, 0, sizeof(str_action1));

  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (!stringcmp(l, "use")) {
      l = parseQuotedString(l, use_names, "use");	// refers object to use
    }
    else if (!stringcmp(l, "state"))   l = parseUInt8(l, &pos.st, "state");
    else if (!stringcmp(l, "method0")) l = parseString(l, str_action0, "method0");
    else if (!stringcmp(l, "method1")) l = parseString(l, str_action1, "method1");
  }
  end_while_parse(l);

  char *p, *pname = new char[OBJNAME_LEN * USENAME_MAX];
  strcpy(pname, use_names);
  p = strchr(pname, ' ');
  if (p) *p = '\0';

  Object *puse = getObject(pname);
  if (puse) {
    num_action0 = indexAction(puse->type, str_action0);
    if (*str_action1)
      num_action1 = indexAction(puse->type, str_action1);
    if (num_action1 == 0 || num_action1 == 255)
      num_action1 = num_action0;
  }
  delete[] pname;
}

void Button::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
}

void Button::inits()
{
  mobileObject(0);
  createNetObj(PROPS, ++oid);
}

Button::Button(char *l)
{
  parser(l);
  behaviors();
  inits();
}

/* Updates towards the network */
bool Button::updateToNetwork(const Pos &oldpos)
{
  bool change = false;

  if (pos.st != oldpos.st) {
    netop->declareDelta(PROPSTATE);
    change = true;
  }
  return change;
}

/** Action commut - static */
void Button::commut(Button *button, void *d, time_t s, time_t us)
{
  int idxaction = (button->pos.st == 0) ? button->num_action0 : button->num_action1;
  button->pos.st = 1 - button->pos.st;

  Object *psel = NULL;

  char *puse = strtok(button->use_names, " \t");
  while (puse) {
    if ((psel = button->getObject(puse))) {
      if ((idxaction <= OBJECTSNUMBER) && isAction(psel->type, idxaction))
        doAction(psel->type, idxaction, psel, d, s, us);
      else
        echo("commut: object named %s has an unknown action name %d", puse, idxaction);
    }
    else {
      echo("button: no object named %s", puse);
    }
    puse = strtok(NULL, " \t");	// next name
  }
}

void Button::get_bstate(Button *button, Payload *pp)
{
  Pos oldpos = button->pos;

  pp->getPayload("d", &(button->pos.st));
  button->updatePositionAndGrid(oldpos);
}

void Button::put_bstate(Button *button, Payload *pp)
{
  pp->putPayload("d", button->pos.st);
}

void Button::quit()
{
  delete[] use_names;
  use_names = NULL;
  oid = 0;
}

void Button::funcs()
{
  getPropFunc(BUTTON_TYPE, PROPSTATE, _Payload get_bstate);
  putPropFunc(BUTTON_TYPE, PROPSTATE, _Payload put_bstate);

  setActionFunc(BUTTON_TYPE, 0, _Action commut, "Push");
}
