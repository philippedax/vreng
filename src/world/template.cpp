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
// template.cpp
//
// Template example
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "template.hpp"
#include "matvec.hpp"   // V3 M4
#include "move.hpp"     // gotoFront


const OClass Template::oclass(TEMPLATE_TYPE, "Template", Template::creator);


/* Creation from a file */
Object * Template::creator(char *l)
{
  return new Template(l);
}

void Template::defaults()
{
}

/* Parser */
void Template::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
  }
  end_while_parse(l);
}

/* Behavior */
void Template::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  updatePosition();
}

/* Specific inits */
void Template::inits()
{
}

/* Constructor */
Template::Template(char *l)
{
  parser(l);
  behaviors();
  inits();
}

void Template::buildScreen()
{
  V3 dim;
  getDim(dim);
}

/* Computes something at each loop */
void Template::changePermanent(float lasting)
{
}

/* Renders at each loop */
void Template::render()
{
  updatePosition();

  // push
  glPushMatrix();
  glEnable(GL_CULL_FACE);

  // render

  // pop
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void Template::quit()
{
}

void Template::action1(Template *template, void *d, time_t s, time_t u)
{
}

void Template::action2(Template *template, void *d, time_t s, time_t u)
{
}

void Template::funcs()
{
  setActionFunc(TEMPLATE_TYPE, 0, _Action action1, "action1");
  setActionFunc(TEMPLATE_TYPE, 1, _Action action2, "action2");
  setActionFunc(TEMPLATE_TYPE, 2, _Action gotoFront, "Approach");
}
