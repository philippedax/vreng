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
// bgcolor.cpp
//
// Background Color
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "bgcolor.hpp"
#include "world.hpp"	// World


const OClass Bgcolor::oclass(BGCOLOR_TYPE, "Bgcolor", Bgcolor::creator);

// static class member initialization
Bgcolor * Bgcolor::bgcolor = NULL;

void Bgcolor::funcs() {}


Object * Bgcolor::creator(char *l)
{
  return new Bgcolor(l);
}

void Bgcolor::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    if (!stringcmp(l, "color=")) {
      l = parseVector3f(l, color, "color");
    }
  }
  end_while_parse(l);
}

void Bgcolor::behaviors()
{
  initObject(INVISIBLE);
}

void Bgcolor::inits()
{
  /* we use (x,y,z,az) to match (r,g,b,a) */
  Bgcolor *wcolor = World::current()->bgcolor;
  wcolor->color[0] = color[0];
  wcolor->color[1] = color[1];
  wcolor->color[2] = color[2];
  wcolor->color[3] = 1;
}

Bgcolor::Bgcolor(char *l)
{
  bgcolor = this;
  parser(l);
  behaviors();
  inits();
}

/** Bgcolor by default, build by the world it self */
Bgcolor::Bgcolor()
{
  black();
}

void Bgcolor::black()
{
  color[0] = color[1] = color[2] = 0; color[3] = 1;
}

/** Quits - destructor */
//Bgcolor::~Bgcolor()
void Bgcolor::quit()
{
  black();
  bgcolor = NULL;
}
