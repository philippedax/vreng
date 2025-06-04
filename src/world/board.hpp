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
// board.hpp
//---------------------------------------------------------------------------
#ifndef BOARD_HPP
#define BOARD_HPP

#include "object.hpp"

#define BOARD_TYPE	18
#define BOARD_NAME	"Board"

/**
 * Board class
 */
class Board: public Object {
 public:
  // properties
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number
  };
  // states
  enum {
    INACTIVE,
    OPEN,
    IMPORT
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;};

  static void funcs();	///< init funclist

  Board(char *l);	///< Constructor
  ~Board();		///< Destructor

  static Object * (creator)(char *l);
  /**< Creates from fileline */

  bool intersect(Object *pcur, Object *pold);
  /**< Intersection with an other object */

  bool publish(const Pos &oldpos);
  /**< Publishes new position */

  void render();
  /**< Renders the drawing */

  bool isDrawing() const;
  /**< Accessor: is drawing ? */

  void click(V3 dir);
  /**< React to a user click on our surface */

 private:
  GLint dlist;		///< display-list
  bool drawing;

  void parser(char *l);
  /**< Parses vre file. */

  void defaults();
  /**< Sets default values. */

  void inits();
  /**< Sets initializations. */

  void draw();
  void import();
  void leave();

  // GUI callbacks
  static void wb_cb(Board *o, void *d, time_t s, time_t u);
  static void draw_cb(Board *o, void *d, time_t s, time_t u);
  static void leave_cb(Board *o, void *d, time_t s, time_t u);
  static void import_cb(Board *o, void *d, time_t s, time_t u);
  static void cancel_cb(Board *o, void *d, time_t s, time_t u);
};

#endif
