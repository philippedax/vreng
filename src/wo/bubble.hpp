//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#ifndef BUBBLE_HPP
#define BUBBLE_HPP

#include "text.hpp"
struct TexFont;

#define BUBBLE_TYPE	66
#define BUBBLE_NAME	"Bubble"

/**
 * Bubble class
 */
class Bubble: public WObject {
private:

  Text *bubtext;	///< message inside bubble
  char *text;		///< text to display
  bool face;		///< face recto=0 verso=1
  float scale;		///< scale
  Pos postext;		///< text position

public:
  /* properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number.
  };

  enum {
    BUBBLERECTO,	// recto
    BUBBLEVERSO		// verso
  };
  static const float BUBBLETTL;		///< bubble time to live
  static const float BUBBLESCALE;	///< bubble scale

  static const OClass oclass;	///< class variable.

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Bubble(class User *user, char *string, const float *color, bool face);	///< Constructor.

  virtual void updateTime(time_t s, time_t us, float *lasting);
  /**< Updates lasting time. */

  virtual void changePosition(float lasting);
  /**< Imposed movement. */

  virtual bool updateToNetwork(const Pos &oldpos);
  /**< Publishes to network. */

  virtual void setPosition();
  /**< Sets position above the head. */

  virtual void quit();
  /**< Quits bubble. */

private:
  virtual void defaults();
  /**< Sets default values. */

  virtual void makeSolid();
  /**< Builds transparent bubble. */

  virtual void behavior();
  /**< Sets behavior. */

};

#endif
