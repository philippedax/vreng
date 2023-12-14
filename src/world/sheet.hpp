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
#ifndef SHEET_HPP
#define SHEET_HPP

#include "wobject.hpp"

#define SHEET_TYPE	44
#define SHEET_NAME	"Sheet"

#define DEF_URL_NOISE	"/mp3/sheet.mp3"	// noise for sheet

class Book;

/**
 * Sheet class
 */
class Sheet: public WO {
  friend class Book;

 protected:
  class Book* book;	///< book pointer
  float aright;		///< right angle
  float aleft;		///< left angle
  float aspeed;		///< angular speed
  float lspeed;		///< linear speed
  V3 center;		///< center
  V3 size;		///< size

 public:
  static const float LSPEED;
  static const float ASPEED;

  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS
  };

  /* actions */
  enum {
    CREATE,
    DESTROY,
    TURN_RIGHT,
    PULL,
    ROTATE
  };

  /* states */
  enum {
    RIGHT,
    LEFT
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Sheet(class Book* _book, char* l, uint8_t _heap);
  /**< Constructor called by Book */
#if 0
  Sheet(uint8_t type_id, Noid _noid, Payload *pp);
  /**< Constructor replicator */

  static WO * replicator(uint8_t type_id, Noid noid, Payload *pp);
#endif

  void changePosition(float lasting);
  void updateTime(time_t s, time_t us, float *lasting);
  //bool publish(const Pos &oldpos);
  bool whenIntersect(WO *pcur, WO *pold);
  void quit();

  void turnPrev();
  void turnNext();
  void pullPrev();
  void pullNext();
  void pushPrev(float dist);
  void pushNext(float dist);
  void destroy();

 private:
  void parser(char *l);
  /**< Parses */

  static void sheetCreate(Book* book, char* l, uint8_t side, int heap);
};

#endif
