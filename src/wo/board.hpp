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
#ifndef BOARD_HPP
#define BOARD_HPP

#include "wobject.hpp"

#define BOARD_TYPE	18
#define BOARD_NAME	"Board"

/**
 * Board class
 */
class Board: public WObject {

public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number
  };

  /* states */
  enum {
    INACTIVE,
    OPEN,
    IMPORT
  };

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;};

  static void funcs();	///< init funclist

  Board(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from fileline */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< Intersection with an other object */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes new position */

  virtual void render();
  /**< Renders the drawing */

  virtual bool isDrawing() const;
  /**< Accessor: is drawing ? */

  virtual void click(V3 dir);
  /**< React to a user click on our surface */

  virtual void quit();
  /**< Quits */

private:
  GLint dlist;
  bool drawing;

  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< Sets default values */

  virtual void draw();
  virtual void import();
  virtual void leave();

  // GUI callbacks
  static void wb_cb(Board *o, void *d, time_t s, time_t u);
  static void draw_cb(Board *o, void *d, time_t s, time_t u);
  static void leave_cb(Board *o, void *d, time_t s, time_t u);
  static void import_cb(Board *o, void *d, time_t s, time_t u);
  static void cancel_cb(Board *o, void *d, time_t s, time_t u);
};

#endif
