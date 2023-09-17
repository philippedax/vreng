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
//
// A part of this code comes from Lesson11 at NeHe written by Bosco in 1999.
// http://nehe.gamedev.net/
//
#include "vreng.hpp"
#include "flag.hpp"
#include "world.hpp"	// current
#include "wind.hpp"	// getOrient
#include "texture.hpp"	// Texture::current


const OClass Flag::oclass(FLAG_TYPE, "Flag", Flag::creator);


/* creation from a file */
WObject * Flag::creator(char *l)
{
  return new Flag(l);
}

void Flag::parser(char *l)
{
  l = tokenize(l);
  l = parseAttributes(l);
}

void Flag::behaviors()
{
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);
}

void Flag::inits()
{
  initMobileObject(0);

  width = pos.bbs.v[0];
  height = pos.bbs.v[2];
  texid = Texture::current();
  wiggle = 0;

  Wind *wind = World::current()->wind;
  if (wind) {  // get orientation and speed of the wind from meteo server
    zrot  = RAD2DEG(wind->getOrient()) + 180;
    uint8_t speed = wind->getSpeed();
    force = MAX(speed / 20, 1);  // [1..5]
  }
  else {
    zrot  = RAD2DEG(pos.az);
    force = 1;
  }
  //echo("w=%.2f h=%.2f r=%.2f f=%d", width, height, zrot, force);

  for (int x=0; x < DIM_FLAG + 1; x++) {
    for (int y=0; y < DIM_FLAG + 1; y++) {
      mesh[x][y][0] = (GLfloat) (x - DIM_FLAG/2) / 5.;
      mesh[x][y][1] = (GLfloat) (y - DIM_FLAG/2) / 5.;
      mesh[x][y][2] = (GLfloat) (sin(((x * (180./DIM_FLAG))/180.) * M_PI));
    }
  }
}

Flag::Flag(char *l)
{
  parser(l);
  behaviors();
  inits();
}

void Flag::changePermanent(float dt)
{
  if (wiggle == 2) {
    for (int y=0; y < DIM_FLAG; y++) {
      GLfloat w = mesh[0][y][2];
      for (int x=0; x < DIM_FLAG; x++) {
        mesh[x][y][2] = mesh[x+1][y][2];	// next wave to the right
      }
      mesh[DIM_FLAG][y][2] = w;
    }
    wiggle = 0;
  }
  wiggle++;
}

void Flag::draw()
{
  glBegin(GL_QUADS);
  for (int x=0; x < DIM_FLAG; x++) {
    for (int y=0; y < DIM_FLAG; y++) {
      GLfloat fx  = (GLfloat) (x)   / DIM_FLAG;
      GLfloat fy  = (GLfloat) (y)   / DIM_FLAG;
      GLfloat fxb = (GLfloat) (x+1) / DIM_FLAG;
      GLfloat fyb = (GLfloat) (y+1) / DIM_FLAG;

      glTexCoord2f(fx, fy);
      glVertex3f(mesh[x][y][0], mesh[x][y][1], mesh[x][y][2]);
      glTexCoord2f(fx, fyb);
      glVertex3f(mesh[x][y+1][0], mesh[x][y+1][1], mesh[x][y+1][2]);
      glTexCoord2f(fxb, fyb);
      glVertex3f(mesh[x+1][y+1][0], mesh[x+1][y+1][1], mesh[x+1][y+1][2]);
      glTexCoord2f(fxb, fy);
      glVertex3f(mesh[x+1][y][0], mesh[x+1][y][1], mesh[x+1][y][2]);
    }
  }
  glEnd();
}

GLvoid Flag::render()
{
  glPushMatrix();
  glDisable(GL_CULL_FACE);

  if (texid) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }

  glTranslatef(pos.x+((width/2)*cos(DEG2RAD(zrot))), pos.y+((width/2)*sin(DEG2RAD(zrot))), pos.z);
  glRotatef(RAD2DEG(pos.ax) + 90, 1,0,0);
  glRotatef(180, 0,1,0);
  glScalef(0.1*width, 0.1*height, 0.05*height);

  draw();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glPopMatrix();
}

void Flag::funcs()
{}
