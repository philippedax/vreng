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
// book.hpp
//---------------------------------------------------------------------------
#ifndef BOOK_HPP
#define BOOK_HPP

#define BOOK_TYPE	43
#define BOOK_NAME	"Book"
#define DEF_URL_PAPER	"/jpg/paper.jpg"	// paper for book

#include "object.hpp"


/**
 * Book class
 */
class Book: public Object {
  friend class Sheet;

 protected:
  class Sheet *right;	///< rightside sheet
  class Sheet *left;	///< leftside sheet
  class Sheet *temp;	///< volatile sheet

  float aright;		///< right angle when book is opened
  float aleft;		///< left  angle when book is opened
  float aspeed;		///< angular speed
  float vspeed;		///< to increase speed when sheet is turning
  float thick;		///< sheet thickness
  float width;		///< sheet width
  float height;		///< sheet height
  V3 size;		///< sheet size

  uint8_t nb;		///< number of sheets
  uint8_t current;	///< current sheet number
  int32_t fwd;		///< sheets to turn for p+10
  int32_t bwd;		///< sheets to turn for p-10

  char url[64];		///< sheet url
  char **tex;		///< texture table
  char **html;		///< html table

 public:
  static const float ARIGHT;
  static const float ALEFT;
  static const float ASPEED;
  static const float LSPEED;

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
    OPEN,
    CLOSE,
    NEXT,
    PREV,
    FWD,
    REW,
    PULL,
    LOOKL,
    LOOKR
  };

  /* states */
  enum {
    OPENED = 2,
    CLOSED_R,
    CLOSED_L
  };

  /* parts */
  enum {
    RIGHT,
    LEFT,
    TEMP
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Book(char *l);	///< Constructor

  static Object * (creator)(char *l);
  /**< Creates from a file */

  void changePosition(float lasting);
  /**< When changing position */

  void timing(time_t, time_t, float *);
  /**< Updates delays */

  bool publish(const Pos &oldpos);
  /**< Publishes to network */

  bool whenIntersect(Object *pcur, Object *pold);
  /**< When an intersection occurs */

  void quit();
  /**< Quits properly */

 private:
  void forward(void *d, time_t s, time_t u);
  /**< Next sheet */

  void backward(void *d, time_t s, time_t u);
  /**< Prev sheet */

  void setPos(char *s, float x, float y, float z, float az, float ax);
  /**< Sheet position */

  void setDim(char *s, float x, float y, float z);
  /**< Sheet dimension */

  //void setCov(char *s, char *yn, char *yp, char *xp, char *xn, char *zp);
  void setCov(char *s, char *tx);
  /**< Sheet cover texture */

  void setCov(char *s, char *yn, char *yp);
  /**< Sheet cover texture */

  void setTex(char *s, char *yn, char *xn, char *yp, char *xp, char *zp);
  /**< Sheet texture */

  void setTex(char *s, char *yn, char *xn);
  /**< Sheet texture */

  void createSheet(char *s, uint8_t heap);
  ///< Create sheet

  void parser(char *l);
  /**< Parses */

  void behaviors();
  /**< sets behaviors. */

  void inits();
  /**< sets initializations. */

  void turnPrev(class Sheet *sheet, void *d, time_t s, time_t u);
  void turnNext(class Sheet *sheet, void *d, time_t s, time_t u);
  void pushPrev(class Sheet *sheet, float dist);
  void pushNext(class Sheet *sheet, float dist);
  void pullPrev(class Sheet *sheet);
  void pullNext(class Sheet *sheet);
  void delPage(class Sheet *sheet);

  void open(Book *book, void *d, time_t s, time_t u);
  void close(Book *book, void *d, time_t s, time_t u);
  void approach();
  void nextPage(Book *book, void *d, time_t s, time_t u);
  void prevPage(Book *book, void *d, time_t s, time_t u);
  void forwardPage(Book *book, void *d, time_t s, time_t u);
  void backwardPage(Book *book, void *d, time_t s, time_t u);

  // GUI callbacks
  static void open_cb(Book *book, void *d, time_t s, time_t u);
  static void close_cb(Book *book, void *d, time_t s, time_t u);
  static void approach_cb(Book *book, void *d, time_t s, time_t u);
  static void nextPage_cb(Book *book, void *d, time_t s, time_t u);
  static void prevPage_cb(Book *book, void *d, time_t s, time_t u);
  static void forwardPage_cb(Book *book, void *d, time_t s, time_t u);
  static void backwardPage_cb(Book *book, void *d, time_t s, time_t u);
  static void lookLeft_cb(Book *book, void *d, time_t s, time_t u);
  static void lookRight_cb(Book *book, void *d, time_t s, time_t u);

  static void reader(void*, class Http*);
};

#endif
