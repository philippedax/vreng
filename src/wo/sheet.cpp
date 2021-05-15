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
#include "sheet.hpp"
#include "book.hpp"	// Book
#include "mp3.hpp"	// start
#include "solid.hpp"	// getRelativeBB


const OClass Sheet::oclass(SHEET_TYPE, "Sheet", NULL);

const float Sheet::LSPEED = 1.0;
const float Sheet::ASPEED = 1.0;

// local
static uint16_t oid = 0;


void Sheet::parser(char *l)
{
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

Sheet::Sheet(Book* _book, char* l, uint8_t _side)
{
  //error("l=%s s=%d", l,_side);
  parser(l);

  /* adapt sheet position from book position */
  book = _book;
  aright = book->aright;
  aleft  = book->aleft;
  state = _side;
  aspeed = book->vspeed * ASPEED;
  lspeed = LSPEED;
  center.v[0] = pos.x;	// hinge
  center.v[1] = pos.y;
  center.v[2] = pos.z;

  getDimBB(size);
  pos.x = center.v[0] + (size.v[0] * cos(pos.az));
  pos.y = center.v[1] + (size.v[0] * sin(pos.az));
  //pos.z = center.v[2];
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  //error("cen: %.2f %.2f %.2f", center.v[0],center.v[1],center.v[2]);
  //error("siz: %.2f %.2f %.2f", size.v[0],size.v[1],size.v[2]);
  //error("pos: %.2f %.2f %.2f %.2f", pos.x,pos.y,pos.z,pos.az);

  enableBehavior(COLLIDE_NEVER);

  initMobileObject(1);
  createPermanentNetObject(PROPS, ++oid);
}

void Sheet::changePosition(float lasting)
{
  pos.az += lasting * move.aspeed.v[0];

  center.v[0] += lasting * move.lspeed.v[0];
  center.v[1] += lasting * move.lspeed.v[1];
  center.v[2] += lasting * move.lspeed.v[2];

  // pour x et y, on utilise l'attribut center pour garder une trace du passe
  // (sinon, pb car x et y sont remis par defaut au centre de la box)
  pos.x = center.v[0] + size.v[0] * cos(pos.az);
  pos.y = center.v[1] + size.v[0] * sin(pos.az);
  //pos.z = center.v[2];
  //error("%.2f %.2f %.2f %.2f dt=%.2f", pos.x,pos.y,pos.z,pos.az,lasting);
}

void Sheet::updateTime(time_t s, time_t us, float *lasting)
{
  if (move.aspeed.v[0] == aspeed) // turning right
    move.ttl = MIN(ABSF(deltaAngle(pos.az, aright)/move.aspeed.v[0]), move.ttl);
  else if (move.aspeed.v[0] == -aspeed) // turning left
    move.ttl = MIN(ABSF(deltaAngle(pos.az, aleft)/move.aspeed.v[0]), move.ttl);

  if (! updateLasting(s, us, lasting)) {
    if (book->fwd > 0) book->forward(NULL, 0, 0);
    book->fwd--;
    if (book->bwd > 0) book->backward(NULL, 0, 0);
    book->bwd--;
  }
}

bool Sheet::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Sheet::whenIntersect(WObject *pcur, WObject *pold)
{
  // if (state == RIGHT)
    projectPosition(pcur, pold);
  return true;
}

/**
 * Called by Book by a doAction()
 *
 * side indicates on which side (right/left) of book the sheet is attached
 * heap indicates on which heap is the sheet.
 */
void Sheet::sheetCreate(Book* book, char* l, uint8_t side, int heap)
{
  switch (heap) {
  case Book::RIGHT:    book->right= new Sheet(book, l, side); break;
  case Book::LEFT:     book->left= new Sheet(book, l, side); break;
  case Book::VOLATILE: book->inter = new Sheet(book, l, side); break;
  }
}

void Sheet::turnNext()
{
  float ttl = ABSF(deltaAngle(aright, aleft) / aspeed);
  // if (state == LEFT) return;
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.aspeed.v[0] = -aspeed;
  initImposedMovement(ttl);
  Mp3::start(DEF_URL_NOISE);
  //state = LEFT;
}

void Sheet::turnPrev()
{
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  // if (state == RIGHT) return;
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.aspeed.v[0] = aspeed;
  initImposedMovement(ttl);
  Mp3::start(DEF_URL_NOISE);
  //state = RIGHT;
}

void Sheet::pullNext()
{
  // if (state == LEFT) return;
  getDimBB(size);
  float ttl = ABSF(deltaAngle(pos.az, aleft) / aspeed);
  float dx = -size.v[1] * (cos(aleft) + cos(aright));
  float dy =  size.v[1] * (sin(aleft) + sin(aright));
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.lspeed.v[0] = dx / ttl;
  move.lspeed.v[1] = dy / ttl;
  move.aspeed.v[0] = -aspeed;
  initImposedMovement(ttl);
  //state = LEFT;
}

void Sheet::pullPrev()
{
  // if (state == RIGHT) return;
  getDimBB(size);
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  float dx =  size.v[1] * (cos(aleft) + cos(aright));
  float dy = -size.v[1] * (sin(aleft) + sin(aright));
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  move.lspeed.v[0] = dx / ttl;
  move.lspeed.v[1] = dy / ttl;
  move.aspeed.v[0] = aspeed;
  initImposedMovement(ttl);
  //state = RIGHT;
}

/* pousser un tas gauche (pendant que le tas droit tourne) */
void Sheet::pushNext(float dist)
{
  //if (state == RIGHT) return;
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  move.lspeed.v[0] =  dist * sin(aleft) / ttl;
  move.lspeed.v[1] = -dist * cos(aleft) / ttl;
  initImposedMovement(ttl);
}

/* pousser un tas droit (pendant que le tas gauche tourne) */
void Sheet::pushPrev(float dist)
{
  //if (state == LEFT) return;
  clearV3(move.lspeed);
  clearV3(move.aspeed);
  float ttl = ABSF(deltaAngle(aleft, aright) / aspeed);
  move.lspeed.v[0] = -dist * sin(aright) / ttl;
  move.lspeed.v[1] =  dist * cos(aright) / ttl;
  initImposedMovement(ttl);
}

void Sheet::destroy()
{
  toDelete();	// delete Wobject
}

void Sheet::quit()
{
  oid = 0;
}

void Sheet::funcs()
{
  getPropertyFunc(SHEET_TYPE, PROPXY, WO_PAYLOAD get_xy);
  getPropertyFunc(SHEET_TYPE, PROPZ, WO_PAYLOAD get_z);
  getPropertyFunc(SHEET_TYPE, PROPAZ, WO_PAYLOAD get_az);
  getPropertyFunc(SHEET_TYPE, PROPHNAME, WO_PAYLOAD get_hname);

  putPropertyFunc(SHEET_TYPE, PROPXY, WO_PAYLOAD put_xy);
  putPropertyFunc(SHEET_TYPE, PROPZ, WO_PAYLOAD put_z);
  putPropertyFunc(SHEET_TYPE, PROPAZ, WO_PAYLOAD put_az);
  putPropertyFunc(SHEET_TYPE, PROPHNAME, WO_PAYLOAD put_hname);

  // invisible method, called by Book
  setActionFunc(SHEET_TYPE, CREATE, WO_ACTION sheetCreate, "");
}
