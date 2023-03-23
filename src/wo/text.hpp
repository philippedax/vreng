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
#ifndef TEXT_HPP
#define TEXT_HPP

#include "wobject.hpp"

#define TEXT_TYPE	39
#define TEXT_NAME	"Text"

/**
 * Text class
 */
class Text: public WObject {

protected:
  static const float RATIO_SCALE;	///< default scale ratio.
  static const uint16_t MAXLEN;		///< max lenght of text.

  char *textstr;	///< text string.
  class Txf *txf;	///< font pointer.
  GLfloat scale;	///< scale to apply.
  float color[4];	///< color of text.
  GLfloat shiftx;	///< shift pos.x to apply.
  GLfloat shifty;	///< shift pos.y to apply.
  GLfloat shiftz;	///< shift pos.z to apply.
  GLfloat shiftaz;	///< shift pos.az to apply.
  GLfloat shiftax;	///< shift pos.ax to apply.
  uint8_t verso;	///< flag text recto/verso.
  bool havefont;	///< flag texture txf established.

public:
  static const float GLYPHSIZ;		///< lenght of a glyph.

  static const OClass oclass;	///< class variable.

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Text(char *l);
  ///< Constructor for vre file.

  Text(const char *t, struct Pos &pos, float _scale, const float *_color);
  ///< Constructor for legend.

  Text(const char *t, struct Pos &pos, float _scale, const float *_color, bool _face);
  ///< Constructor for bubble.

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  void changePosition(float lasting);
  /**< Follows localuser */

  void expire();
  /**< toDelete. */

  void render();
  /**< Special rendering for text. */

  void setPos(float x, float y, float z, float az, float ax);
  /**< Sets positional shifts. */

  float lenText(const char *text);
  ///< Returns lenght en glyphs.

  void quit();
  /**< Quits text. */

protected:
  bool loadFont();
  /**< Loads font */

private:
  void parser(char *l);
  /**< Parses file line.*/

  void defaults();
  /**< Sets defaults. */

  void makeSolid();
  /**< Makes a solid. */

  void behavior();
  /**< Sets behaviors. */

  void inits();
  /**< Makes initializations. */
};

#endif
