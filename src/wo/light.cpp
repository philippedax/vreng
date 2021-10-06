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
#include "vreng.hpp"
#include "light.hpp"


const OClass Light::oclass(LIGHT_TYPE, "Light", Light::creator);


/* creation from a file */
WObject * Light::creator(char *l)
{
  return new Light(l);
}

void Light::defaults()
{
  fog = 0;
  for (int i=0; i<3; i++) color[i] = 1;	// white
}

void Light::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "fog="))   l = parse()->parseFloat(l, &fog, "fog");
    else if (! stringcmp(l, "color=")) l = parse()->parseVector3f(l, color, "color");
  }
  end_while_parse(l);
}

Light::Light()
{
  enableBehavior(UNVISIBLE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  initObject(INVISIBLE);
}

Light::Light(char *l)
{
  parser(l);

  initObject(INVISIBLE);
  addToList(lightList);

  if (!pos.x && !pos.y && !pos.z && !pos.az && !pos.ax)
    islight = false;
  else
    islight = true;

  if (islight) {
    GLfloat light_diffuse[] = {1, 1, 1, 1};
    GLfloat light_ambient[] = {0.6, 0.6, 0.6, 1};
    GLfloat light_specular[] = {0.8, 0.8, 0.8, 1};
    GLfloat light_spot_direction[3];
    GLfloat light_spot_cutoff[1];

    light_ambient[0] = 0.1;
    light_ambient[1] = 0.1;
    light_ambient[2] = 0.1;
    light_ambient[3] = 1;
    light_position[0] = pos.x;
    light_position[1] = pos.y;
    light_position[2] = pos.z;
    light_position[3] = pos.az;		// w = 0 (dir) | 1 (pos)
    if (light_position[3] == 0) {	// directionnel
      if (pos.ax > 0 && pos.ax <= 90) {
        light_spot_cutoff[0] = pos.ax;	// 0 < spot < 90
        light_mode = DIR;
      }
      else light_mode = POS;
    }
    else light_mode = POS; // positionnel

    // reset orientation
    pos.az = 0;
    pos.ax = 0;

    glEnable(GL_LIGHT2);

    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

    if (light_mode == DIR) {	// spot
      light_spot_direction[0] = light_position[0];
      light_spot_direction[1] = light_position[1];
      light_spot_direction[2] = light_position[2];
      glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_spot_direction);
      glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, light_spot_cutoff);
      //glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);
    }
    else
      glLightfv(GL_LIGHT2, GL_POSITION, light_position);
  }
  if (fog) {
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_DENSITY, fog);
    glFogfv(GL_FOG_COLOR, color);
  }
}

/* Called by renderer if in lightList */
void Light::lights()
{
  //printf("\n*** light:");
  for (list<WObject*>::iterator il = lightList.begin(); il != lightList.end() ; ++il) {
    (*il)->lighting();
    //printf(" %s", (*il)->typeName());
  }
  //printf("\n");
}

void Light::lighting()
{
  if (islight) {
    glPushAttrib(GL_LIGHTING_BIT);
    if (light_mode == POS) {
      glPushMatrix();
      glLoadIdentity();
      glLightfv(GL_LIGHT2, GL_POSITION, light_position);
      glPopMatrix();
    }
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glPopAttrib();
  }
  if (fog) glEnable(GL_FOG);
}

void Light::quit()
{
  glDisable(GL_FOG);
  glDisable(GL_LIGHT2);
}

void Light::funcs() {}
