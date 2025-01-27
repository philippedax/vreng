//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
// bubble.hpp
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
class Bubble: public Object {
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

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Bubble(class User *user, char *string, const float *color, bool face);	///< Constructor.

  void updateTime(time_t s, time_t us, float *lasting);
  /**< Updates lasting time. */

  void changePosition(float lasting);
  /**< Imposed movement. */

  bool publish(const Pos &oldpos);
  /**< Publishes to network. */

  void setPosition();
  /**< Sets position above the head. */

  void quit();
  /**< Quits bubble. */

 private:
  void defaults();
  /**< Sets default values. */

  void geometry();
  /**< Builds transparent bubble. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

};

#endif
