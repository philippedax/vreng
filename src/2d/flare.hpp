//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
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
/* Copyright (c) Mark J. Kilgard, 1997. */
#ifndef FLARE_HPP
#define FLARE_HPP

typedef struct t_flare {
  int type;		// flare texture index, -1..5
  float scale;
  float loc;            // postion on axis
  GLfloat color[3];
} tFlare;

struct Pos;

/**
 * Flare class
 */
class Flare {
 private:
  static const GLfloat white[3];
  static const GLfloat red[3];
  static const GLfloat green[3];
  static const GLfloat blue[3];
  static uint8_t MAX_SHINE;
  static uint8_t MAX_FLARE;
  static uint8_t DEF_FLARES;

  tFlare flares[16];	///< array of flares
  GLuint *shineTex;	///< texid for shines
  GLuint *flareTex;	///< texid for flares
  uint8_t num_shine;	///< number of shines
  uint8_t num_flares;	///< number of flares
  float scale;		///< global scale

 public:
  Flare();		///< constructor
  Flare(uint8_t number, float _scale, GLfloat *color);

  virtual ~Flare();	///< destructor

  void setNumber(uint8_t number);
  /**< Sets number of flares */

  void setScale(float scale);
  /**< Sets new scale for flares */

  void setColor(GLfloat *color);
  /**< Sets unique color for all flares */

  void render();
  void render(struct Pos &from);
  void render(float *from);
  void render(float delta);
  void render(float *from, float delta);
  /**< Renders flare + shine */

 private:
  void defaults();
  /**< Sets default values */

  tFlare set(int type, float location, float scale, const GLfloat *color, float colorScale);
  /**< Sets each flare */

  void inits();
  /**< Initializes flares */

  void setTexture(GLuint texid);
  /**< Sets textures */

  void loadTextures();
  /**< Load needed textures */

  void vnorm(float *vec);
  float vdot(float *a, float *b);
  void vdiff(float *dst, float *a, float *b);
  void vadd(float *dst, float *a, float *b);
  void vcopy(float *dst, float *src);
  void vscale(float *dst, float *src, float scale);
  void vcross(float *cross, const float *v1, const float *v2);
};


#endif
