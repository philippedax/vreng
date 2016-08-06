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
#ifndef WEB_HPP
#define WEB_HPP

#include "wobject.hpp"

#define WEB_TYPE	11
#define WEB_NAME	"Web"


/**
 * Web class
 */
class Web: public WObject {

public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS	///< last item = properties number.
  };

  static const OClass oclass;   ///< class variable.

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Web(char *l);		///< Constructor.

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  virtual void updateTime(time_t sec, time_t usec, float *lasting);
  virtual void changePosition(float lasting);

  virtual bool whenIntersect(WObject *pcur, WObject *pold);

  virtual void quit();
  /**< Quits. */

private:
  static const float TTL;
  static const float ASPEED;
  static const uint16_t MAXLEN; ///< max lenght of text.
  static const float GLYPHSIZ;

  float aspeed;		///< angular speed.
  float afront;		///< normal angle.
  float aback;		///< back angle.
  float acurr;		///< current still angle.
  float atemp;		///< current mobile angle.
  char *legend;		///< legend text.
  class Text *text;	///< Text instance.

  virtual void parser(char *l);
  /**< Parses file line. */

  virtual void parselegend(char *l);
  /**< Parses legend field. */

  virtual void defaults();
  /**< Sets default values. */

  virtual void pivot();
  /** Pivots of PI. */

  // GUI callbacks.
  static void open_cb(Web *o, void *d, time_t s, time_t u);
  static void pivot_cb(Web *o, void *d, time_t s, time_t u);
};

#endif
