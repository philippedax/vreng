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
// sheet.cpp
//
// Sheet handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "sheet.hpp"
#include "book.hpp"	// Book
#include "matvec.hpp"   // V3
#include "mp3.hpp"	// start


const OClass Sheet::oclass(SHEET_TYPE, "Sheet", NULL);

const float Sheet::LSPEED = 1.0;
const float Sheet::ASPEED = 1.0;

// local
static uint16_t oid = 0;


void Sheet::parser(char *l)
{
  l = tokenize(l);
  l = parseAttributes(l);
}

Sheet::Sheet(Book* _book, char* l)
{
  parser(l);

  /* adapt sheet position from book position */
  book = _book;
  aright = book->aright;
  aleft  = book->aleft;
  aspeed = book->vspeed * ASPEED;
  lspeed = LSPEED;
  center.v[0] = pos.x;	// hinge
  center.v[1] = pos.y;
  center.v[2] = pos.z;

  getDim(size);
  pos.x = center.v[0] + (size.v[0] * cos(pos.az));
  pos.y = center.v[1] + (size.v[0] * sin(pos.az));
  clearV3(move.lspeed);
  clearV3(move.aspeed);

  enableBehavior(COLLIDE_NEVER);

  mobileObject(1);
}

void Sheet::imposed(float dt)
{
  pos.az += dt * move.aspeed.v[0];

  center.v[0] += dt * move.lspeed.v[0];
  center.v[1] += dt * move.lspeed.v[1];
  center.v[2] += dt * move.lspeed.v[2];

  // pour x et y, on utilise l'attribut center pour garder une trace du passe
  // (sinon, pb car x et y sont remis par defaut au centre de la box)
  pos.x = center.v[0] + size.v[0] * cos(pos.az);
  pos.y = center.v[1] + size.v[0] * sin(pos.az);
}

void Sheet::timing(time_t s, time_t us, float *dt)
{
  if (move.aspeed.v[0] == aspeed)	// turning right
    move.ttl = MIN(ABSF(deltaAngle(pos.az, aright)/move.aspeed.v[0]), move.ttl);
  else if (move.aspeed.v[0] == -aspeed) // turning left
    move.ttl = MIN(ABSF(deltaAngle(pos.az, aleft)/move.aspeed.v[0]), move.ttl);

  if (! lasting(s, us, dt)) {
    if (book->fwd > 0) book->forward(NULL, 0, 0);
    book->fwd--;
    if (book->bwd > 0) book->backward(NULL, 0, 0);
    book->bwd--;
  }
}

bool Sheet::intersect(Object *pcur, Object *pold)
{
  projectPosition(pcur, pold);
  return true;
}

/**
 * Called by Book by a doAction()
 *
 * side indicates on which side (right/left) of book the sheet is attached
 * heap indicates on which heap is the sheet.
 */
void Sheet::create(Book* book, char* l, int heap)
{
  switch (heap) {
  case Book::RIGHT: book->right = new Sheet(book, l); break;
  case Book::LEFT:  book->left  = new Sheet(book, l); break;
  case Book::TEMP:  book->temp  = new Sheet(book, l); break;
  }
}

void Sheet::turnNext()
{
  float ttl = ABSF(deltaAngle(aright, aleft) / aspeed);

  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.aspeed.v[0] = -aspeed;
  imposedMovement(ttl);
  Mp3::start(DEF_URL_NOISE);
}

void Sheet::turnPrev()
{
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);

  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.aspeed.v[0] = aspeed;
  imposedMovement(ttl);
  Mp3::start(DEF_URL_NOISE);
}

void Sheet::pullNext()
{
  getDim(size);
  float ttl = ABSF(deltaAngle(pos.az, aleft) / aspeed);
  float dx = -size.v[1] * (cos(aleft) + cos(aright));
  float dy =  size.v[1] * (sin(aleft) + sin(aright));
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.lspeed.v[0] = dx / ttl;
  move.lspeed.v[1] = dy / ttl;
  move.aspeed.v[0] = -aspeed;
  imposedMovement(ttl);
}

void Sheet::pullPrev()
{
  getDim(size);
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  float dx =  size.v[1] * (cos(aleft) + cos(aright));
  float dy = -size.v[1] * (sin(aleft) + sin(aright));
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.lspeed.v[0] = dx / ttl;
  move.lspeed.v[1] = dy / ttl;
  move.aspeed.v[0] = aspeed;
  imposedMovement(ttl);
}

/* pousser un tas gauche (pendant que le tas droit tourne) */
void Sheet::pushNext(float dist)
{
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  move.lspeed.v[0] =  dist * sin(aleft) / ttl;
  move.lspeed.v[1] = -dist * cos(aleft) / ttl;
  imposedMovement(ttl);
}

/* pousser un tas droit (pendant que le tas gauche tourne) */
void Sheet::pushPrev(float dist)
{
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  move.lspeed.v[0] = -dist * sin(aright) / ttl;
  move.lspeed.v[1] =  dist * cos(aright) / ttl;
  imposedMovement(ttl);
}

/** Quits - destructor */
Sheet::~Sheet()
{
  oid = 0;
}

void Sheet::funcs()
{
  // invisible method, called by Book
  setActionFunc(SHEET_TYPE, CREATE, _Action create, "");
}
