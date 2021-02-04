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
  static const float SCALE;		///< default scale.
  static const uint16_t MAXLEN;		///< max lenght of text.

  char *text;		///< text string.
  class Txf *txf;	///< font pointer.
  GLfloat scale;	///< scale to apply.
  GLfloat color[4];	///< color of text.
  GLfloat shiftx;	///< shift pos.x to apply.
  GLfloat shifty;	///< shift pos.y to apply.
  GLfloat shiftz;	///< shift pos.z to apply.
  GLfloat shiftaz;	///< shift pos.az to apply.
  GLfloat shiftax;	///< shift pos.ax to apply.
  uint8_t verso;	///< flag text verso.
  bool side;		///< bubble text side.
  bool loaded;		///< flag texture established.
  GLint dlists[2];	///< bubble dlists

public:
  static const float GLYPHSIZ;		///< lenght of a glyph.

  static const OClass oclass;	///< class variable.

  virtual const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist.

  Text(char *l);	///< Constructor.
  Text(const char *t, struct Pos &pos, float _scale, float *_color);	///< Constructor for legend.
  Text() {}		///< Constructor needed for bubble class.
  Text(class User *user, char *text, bool side);	///< Constructor.

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  virtual void render();
  /**< Special rendering for text. */

  virtual void setShifts(float x, float y, float z, float az, float ax);
  /**< Sets positional shifts. */

  virtual void quit();
  /**< Quits text. */

protected:
  virtual bool loadFont();
  /**< Loads font */

private:
  virtual void parser(char *l);
  /**< Parses file line.*/

  virtual void defaults();
  /**< Sets defaults. */

  virtual void makeSolid();
  /**< Makes a solid. */

  virtual void behavior();
  /**< Sets behaviors. */

  virtual void inits();
  /**< Makes initializations. */
};

#endif
