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
// sheet.hpp
//---------------------------------------------------------------------------
#ifndef SHEET_HPP
#define SHEET_HPP

#define SHEET_TYPE	44
#define SHEET_NAME	"Sheet"
#define DEF_URL_NOISE	"/mp3/sheet.mp3"	// noise for sheet

#include "object.hpp"


/**
 * Sheet class
 */
class Sheet: public Object {
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

  Sheet(class Book* _book, char* l);
  /**< Constructor called by Book */

  void changePosition(float lasting);
  void timing(time_t s, time_t us, float *lasting);
  bool whenIntersect(Object *pcur, Object *pold);
  void quit();

  void turnPrev();
  void turnNext();
  void pullPrev();
  void pullNext();
  void pushPrev(float dist);
  void pushNext(float dist);

 private:
  void parser(char *l);
  /**< Parses */

  static void create(class Book* book, char* l, int heap);
};

#endif
