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
#include "board.hpp"
#include "world.hpp"	// current
#include "wb.hpp"	// start
#include "gui.hpp"	// ::g.gui.setToBoard
#include "user.hpp"	// localuser
#include "solid.hpp"	// getDlist


const OClass Board::oclass(BOARD_TYPE, "Board", Board::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Board::creator(char *l)
{
  return new Board(l);
}

void Board::parser(char *l)
{
  defaults();
  l = tokenize(l);
  l = parse()->parseAttributes(l, this);
}

void Board::defaults()
{
  state = INACTIVE;
  drawing = false;
  dlist = 0;
  ::g.gui.setToBoard(this);
}

Board::Board(char *l)
{
  parser(l);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);
}

bool Board::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

/* object intersect: projection */
bool Board::whenIntersect(WObject *pcur, WObject *pold)
{
  projectPosition(pcur, pold);
  return true;
}

/** Accessor */
bool Board::isDrawing() const
{
  return drawing;
}

/* React to a user click on our surface */
void Board::click(V3 d)
{
#if 1 //dax
  // get two principal vectors and a normal one
  V3 v, w, n;
  getSurfVecs(pos, &v, &w, &n);

  // check whether the click comes from the right side of the surface
  //float sp = n.v[0]*d.v[0] + n.v[1]*d.v[1] + n.v[2]*d.v[2];
  //if (sp < 0) { error("Board::click: bad side!"); }

  // eye position
  V3 e = setV3(localuser->pos.x, localuser->pos.y, localuser->pos.z + localuser->height/2 - 0.10);
  // object's center coordinates
  V3 c = setV3(pos.x, pos.y, pos.z);

  // determine (x,y) coord. relativly to our surface
  float det = detV3(d, v, w);
  float varx = calcV3(c, e, w, d) / det;
  float vary = calcV3(c, e, d, v) / det;
  int x = (int) (varx);
  int y = (int) (vary);
  //echo("click: x=%d y=%d d=%.2f %.2f v=%.2f %.2f w=%.2f %.2f n=%.2f %.2f e=%.2f %.2f", x, y, d.v[0],d.v[1],v.v[0],v.v[1],w.v[0],w.v[1],n.v[0],n.v[1],e.v[0],e.v[1]);
#endif
}

void Board::wb_cb(Board *board, void *d, time_t s, time_t u)
{
  Wb::quit();
  Wb::start(World::current()->getChan());
  board->state = Board::OPEN;
}

/* Displays the Board's 2D object list */
void Board::render()
{
  if (dlist) {		// note: dlist is always null (not used) !!!
    GLint texid = getSolid()->getTexid();
    glPushMatrix();
    glLoadName(dlist);
    float gl_mat[16];
    M4 posmat;
    getPosition(posmat);
    M4toV16(&posmat, gl_mat);
    glMultMatrixf(gl_mat);

    glCallList(dlist);

    glPopMatrix();
  }
}

/* note: effect the board disappear */
void Board::draw()
{
  enableBehavior(SPECIFIC_RENDER);
  dlist = getSolid()->getDlist();
  /* TODO */
}

/* note: effect the board reappear */
void Board::leave()
{
  disableBehavior(SPECIFIC_RENDER);
  dlist = 0;
  ::g.gui.setToBoard(NULL);
}

void Board::import()
{
  if (state == INACTIVE) return;
  state |= IMPORT;
  notice("Click in the whiteboard window");
  /* TODO */
}

void Board::draw_cb(Board *board, void *d, time_t s, time_t u) { board->draw(); }

void Board::leave_cb(Board *board, void *d, time_t s, time_t u) { board->leave(); }

void Board::import_cb(Board *board, void *d, time_t s, time_t u) { board->import(); }

void Board::cancel_cb(Board *board, void *d, time_t s, time_t u) { Wb::quit(); }

void Board::quit()
{
  Wb::quit();
  savePersistency();
}

void Board::funcs()
{
  setActionFunc(BOARD_TYPE, 0, _Action draw_cb, "Enter draw");
  setActionFunc(BOARD_TYPE, 1, _Action leave_cb, "Leave draw");
  setActionFunc(BOARD_TYPE, 2, _Action wb_cb, "Call WB");
  setActionFunc(BOARD_TYPE, 3, _Action cancel_cb, "Exit WB");
  setActionFunc(BOARD_TYPE, 4, _Action import_cb, "Import");
  setActionFunc(BOARD_TYPE, 5, _Action moveObject, "Move");
}
