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
#ifndef BOOK_HPP
#define BOOK_HPP

#define BOOK_TYPE	43
#define BOOK_NAME	"Book"

#include "wobject.hpp"

class Http;
class Sheet;

/**
 * Book class
 */
class Book: public WObject {
  friend class Sheet;

protected:
  class Sheet *right;	///< rightside sheet
  class Sheet *left;	///< leftside sheet
  class Sheet *inter;	///< volatile sheet

  float aright;		///< right angle when book is opened
  float aleft;		///< left  angle when book is opened
  float aspeed;		///< angular speed
  float vspeed;		///< to increase speed when sheet is turning
  float thick;	///< sheet thickness
  float width;		///< sheet width
  float height;		///< sheet height
  V3 size;		///< sheet size

  uint8_t nbs;		///< number of sheets
  uint8_t num;		///< current sheet number
  int32_t fwd;		///< sheets to turn for p+10
  int32_t bwd;		///< sheets to turn for p-10

  char url[64];		///< sheet url
  char **tex;		///< texture table
  char **html;		///< html table

public:
  bool near;		///< near or not

  static const float ASPEED;
  static const float ARIGHT;
  static const float ALEFT;
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
    PULL,
    OPEN,
    CLOSE,
    NEXT,
    PREV,
    FWD,
    REW,
    LOOKL,
    LOOKR,
    MOVE
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
    VOLATILE
  };

  static const OClass oclass;	///< class variable

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Book(char *l);	///< Constructor

  static WObject * (creator)(char *l);
  /**< Creates from file */

  virtual void changePosition(float lasting);
  /**< When changing position */

  virtual void updateTime(time_t, time_t, float *);
  /**< Updates delays */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes to network */

  virtual bool whenIntersect(WObject *pcur, WObject *pold);
  /**< When an intersect occurs */

  virtual void quit();
  /**< Quits properly */

  virtual void forward(void *d, time_t s, time_t u);
  /**< Next sheet */

  virtual void backward(void *d, time_t s, time_t u);
  /**< Prev sheet */

  virtual void setPos(char *s, float x, float y, float z, float az, float ax);
  /**< Sheet position */

  virtual void setDim(char *s, float x, float y, float z);
  /**< Sheet dimension */

  virtual void setCov(char *s, char *yn, char *yp, char *xp, char *xn, char *zp);
  /**< Sheet cover texture */

  virtual void setCov(char *s, char *yn, char *yp);
  /**< Sheet cover texture */

  virtual void setTex(char *s, char *yn, char *xn, char *yp, char *xp, char *zp);
  /**< Sheet texture */

  virtual void setTex(char *s, char *yn, char *xn);
  /**< Sheet texture */

  virtual void createSheet(char *s, uint8_t t, uint8_t side);
  ///< Create sheet

private:
  virtual void parser(char *l);
  /**< Parses */

  virtual void defaults();
  /**< sets default values */

  virtual const char * getUrl() const;

  void turnPrev(Sheet *sheet, void *d, time_t s, time_t u);
  void turnNext(Sheet *sheet, void *d, time_t s, time_t u);
  void pushPrev(Sheet *sheet, float dist);
  void pushNext(Sheet *sheet, float dist);
  void pullPrev(Sheet *sheet);
  void pullNext(Sheet *sheet);
  void cancelSheet(Sheet *sheet);

  void open(Book *book, void *d, time_t s, time_t u);
  void close(Book *book, void *d, time_t s, time_t u);
  void approach();
  void nextSheet(Book *book, void *d, time_t s, time_t u);
  void prevSheet(Book *book, void *d, time_t s, time_t u);
  void forwardSheet(Book *book, void *d, time_t s, time_t u);
  void backwardSheet(Book *book, void *d, time_t s, time_t u);

  // GUI callbacks
  static void open_cb(Book *book, void *d, time_t s, time_t u);
  static void close_cb(Book *book, void *d, time_t s, time_t u);
  static void approach_cb(Book *book, void *d, time_t s, time_t u);
  static void nextSheet_cb(Book *book, void *d, time_t s, time_t u);
  static void prevSheet_cb(Book *book, void *d, time_t s, time_t u);
  static void forwardSheet_cb(Book *book, void *d, time_t s, time_t u);
  static void backwardSheet_cb(Book *book, void *d, time_t s, time_t u);
  static void lookLeft_cb(Book *book, void *d, time_t s, time_t u);
  static void lookRight_cb(Book *book, void *d, time_t s, time_t u);

  static void httpReader(void*, Http*);
};

#endif
