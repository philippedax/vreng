//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
/* Copyright (c) Mark J. Kilgard, 1997. */
//--------------------------------------//
#include "vreng.hpp"
#include "flare.hpp"
#include "texture.hpp"	// open
#include "img.hpp"	// Img
#include "wobject.hpp"	// Pos
#include "user.hpp"	// localuser


// local
const GLfloat Flare::red[3] = {1, 0, 0};
const GLfloat Flare::green[3] = {0, 1, 0};
const GLfloat Flare::blue[3] = {0, 0, 1};

uint8_t Flare::MAX_SHINE = 10;
uint8_t Flare::MAX_FLARE = 6;
uint8_t Flare::DEF_FLARES = 12;


void Flare::defaults()
{
  num_flares = DEF_FLARES;
  scale = 1;
  num_shine = 0;
}

/* default constructor */
Flare::Flare()
{
  defaults();
  inits();
}

/* constructor with parameters */
Flare::Flare(uint8_t number, float _scale, GLfloat *color)
{
  defaults();
  inits();
  scale = _scale;
  num_flares = number % DEF_FLARES;
  if (color) setColor(color);
}

/* destructor */
Flare::~Flare()
{
  if (shineTex) delete[] shineTex;
  if (flareTex) delete[] flareTex;
}

tFlare Flare::set(int type, float loc, float _scale, const GLfloat *col, float colscale)
{
  tFlare flare;

  flare.type = type;
  flare.loc = loc;
  flare.scale = _scale;
  flare.color[0] = col[0] * colscale;
  flare.color[1] = col[1] * colscale;
  flare.color[2] = col[2] * colscale;
  return flare;
}

void Flare::inits()
{
  // Shines (-1)
  flares[0] = set(-1, 0 /*1*/, 3, red, 1);
  flares[1] = set(-1, 1, 2., green, 1);
  flares[2] = set(-1, 1, 2.5, blue, 1);
  // Flares
  flares[3] = set(2, 1.3, .4, red, 0.6);
  flares[4] = set(3, 1.0, 1., red, 0.4);
  flares[5] = set(1, 0.5, 2., red, 0.3);
  flares[6] = set(3, 0.2, .5, red, 0.3);
  flares[7] = set(0, 0.0, .4, red, 0.3);
  flares[8] = set(5, -0.25, .7, red, 0.5);
  flares[9] = set(5, -0.4, .2, red, 0.6);
  flares[10] = set(5, -0.6, .4, red, 0.4);
  flares[11] = set(5, -1.0, 0.03, red, 0.2);

  // Textures (shines & flares)
  loadTextures();
}

void Flare::setNumber(uint8_t number)
{
  num_flares = number;
}

void Flare::setScale(float _scale)
{
  scale = _scale;
}

void Flare::setColor(GLfloat *color)
{
  for (int i=0; i < num_flares ; i++) {
    flares[i].color[0] = color[0];
    flares[i].color[1] = color[1];
    flares[i].color[2] = color[2];
  }
}

void Flare::render()
{
  static float delta = 0;
  float from[3] = {0, 0, 0};

  render(from, delta);
  delta += 0.5;
}

void Flare::render(float *from)
{
  static float delta = 0;

  render(from, delta);
}

void Flare::render(Pos &pos)
{
  static float delta = 0;

  float from[3] = {0, 0, 0};
  from[0] = pos.x;
  from[1] = pos.y;
  from[2] = pos.z;

  render(from, delta);
}

void Flare::render(float delta)
{
  float from[3] = {0, 0, 0};

  render(from, delta);
}

void Flare::render(float *from, float delta)
{
  float at[3], light[3], view_dir[3], tmp[3], light_dir[3], pos[3], dx[3], dy[3], center[3], axis[3], sx[3], sy[3], dot;
  float near = 1.;

  from[0] = localuser->pos.x;
  from[1] = localuser->pos.y;
  from[2] = localuser->pos.z;
  light[0] = sin(delta * 0.73) + 2.;
  light[1] = sin(delta * 0.678) * 0.5 + 2.;
  light[2] = sin(delta * 0.895) * 0.5 + 2.;
  //error("flare: %.2f %.2f %.2f", light[0],light[1],light[2]);

  vdiff(view_dir, at, from);
  vnorm(view_dir);			// view_dir = normalize(at-from)
  vscale(tmp, view_dir, near);
  vadd(center, from, tmp);		// center = from + near * view_dir
  vdiff(light_dir, light, from);
  vnorm(light_dir);			// light_dir = normalize(light-from)
  dot = vdot(light_dir, view_dir);
  vscale(tmp, light_dir, near/dot);
  vadd(light, from, light_dir);		// light = from + dot(light,view_dir)*near*light_dir
  vdiff(axis, light, center);
  vcopy(dx, axis);			// axis = light - center
  vnorm(dx);				// dx = normalize(axis)
  vcross(dy, dx, view_dir);		// dy = cross(dx,view_dir)

  glPushMatrix();
  glDisable(GL_LIGHTING);
  //dax glDisable(GL_DITHER);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  //dax glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (int i=0; i < num_flares; i++) {
    vscale(sx, dx, flares[i].scale * scale);
    vscale(sy, dy, flares[i].scale * scale);

    glColor3fv(flares[i].color);

    // bind textures
    if (flares[i].type < 0) {	// shines
      glBindTexture(GL_TEXTURE_2D, shineTex[num_shine]);
      num_shine = (num_shine + 1) % MAX_SHINE;	// [0..9]
    }
    else {	// flares
      glBindTexture(GL_TEXTURE_2D, flareTex[flares[i].type]);
    }

    vscale(tmp, axis, flares[i].loc);
    vadd(pos, center, tmp);	// pos = center + flares[i].loc * axis

    glBegin(GL_QUADS);
     glTexCoord2f(0, 0); vadd(tmp, pos, sx);  vadd(tmp, tmp, sy); glVertex3fv(tmp);  // pos+sx+sy
     glTexCoord2f(1, 0); vdiff(tmp, pos, sx); vadd(tmp, tmp, sy); glVertex3fv(tmp);  // pos-sx+sy
     glTexCoord2f(1, 1); vdiff(tmp, pos, sx); vdiff(tmp, tmp, sy); glVertex3fv(tmp); // pos-sx-sy
     glTexCoord2f(0, 1); vadd(tmp, pos, sx);  vdiff(tmp, tmp, sy); glVertex3fv(tmp); // pos+sx-sy
    glEnd();
  }
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  //dax glEnable(GL_DITHER);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void Flare::setTexture(GLuint texid)
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  Texture *tex = Texture::getTexById(texid);
  if (tex && tex->img) {
    uint16_t w = tex->img->width;
    uint16_t h = tex->img->height;
    uint8_t *pix = tex->img->pixmap;
    glTexImage2D(GL_TEXTURE_2D, 0, 1, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pix);
  }
}

void Flare::loadTextures()
{
  char url[URL_LEN];

  shineTex = new GLuint[MAX_SHINE];
  flareTex = new GLuint[MAX_FLARE];

  for (int i=0; i<MAX_SHINE; i++) {
    sprintf(url, "/sgi/shine%d.bw", i);
    shineTex[i] = Texture::open(url);
    setTexture(shineTex[i]);
  }
  for (int i=0; i<MAX_FLARE; i++) {
    sprintf(url, "/sgi/flare%d.bw", i+1);
    flareTex[i] = Texture::open(url);
    setTexture(flareTex[i]);
  }
}

void Flare::vnorm(float *vec)
{
  float len = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
  if (len == 0) return;
  vec[0] /= len; vec[1] /= len; vec[2] /= len;
}

float Flare::vdot(float *a, float *b)
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void Flare::vdiff(float *dst, float *a, float *b)
{
  dst[0] = a[0] - b[0];
  dst[1] = a[1] - b[1];
  dst[2] = a[2] - b[2];
}

void Flare::vadd(float *dst, float *a, float *b)
{
  dst[0] = a[0] + b[0];
  dst[1] = a[1] + b[1];
  dst[2] = a[2] + b[2];
}

void Flare::vcopy(float *dst, float *src)
{
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
}

void Flare::vscale(float *dst, float *src, float scale)
{
  dst[0] = src[0] * scale;
  dst[1] = src[1] * scale;
  dst[2] = src[2] * scale;
}

void Flare::vcross(float *cross, const float *v1, const float *v2)
{
  float tmp[3];

  tmp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  tmp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  tmp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
  vcopy(cross, tmp);
}
