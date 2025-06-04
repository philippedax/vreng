//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// cloud.cpp
//
// Cloud handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "cloud.hpp"
#include "draw.hpp"	// sphere
#include "world.hpp"	// current
#include "wind.hpp"	// getOrient, getSpeed
#include "snow.hpp"	// SNOW_TYPE
#include "rain.hpp"	// RAIN_TYPE


const OClass Cloud::oclass(CLOUD_TYPE, "Cloud", Cloud::creator);
const uint16_t Cloud::NUMBER = 30; // 100
const float Cloud::SPEED = 0.025;	///< 0.025 m/s
const float Cloud::FAR = 50;		///< clip far


Object * Cloud::creator(char *l)
{
  return new Cloud(l);
}

void Cloud::defaults()
{
  number = NUMBER;
  speed = SPEED;
  state = ACTIVE;
  snowing = false;
  raining = false;

  for (int i=0; i<4; i++) color[i] = .5;  // grey
}

void Cloud::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "number")) l = parseUInt16(l, &number, "number");
    else if (! stringcmp(l, "speed"))  l = parseFloat(l, &speed, "speed");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

void Cloud::build()
{
  srand(time(NULL));

  origpos.x = pos.x;
  origpos.y = pos.y;
  origpos.z = pos.z;
  for (int n=0; n < number; n++) {
    obloids[n].x = pos.x + static_cast<float>(4*(2*drand48()-1));  // +- 4
    obloids[n].y = pos.y + static_cast<float>(5*(2*drand48()-1));  // +- 5
    obloids[n].z = pos.z + static_cast<float>(2*(2*drand48()-1));  // +- 2
    obloids[n].r = static_cast<float>((rand()%1 + 2));             // radius = 2..3
  }
}

void Cloud::rebuild()
{
  pos.x = origpos.x;
  pos.y = origpos.y;
  for (int n=0; n < number; n++) {
    obloids[n].x = pos.x + static_cast<float>(4*(2*drand48()-1));  // +- 4
    obloids[n].y = pos.y + static_cast<float>(5*(2*drand48()-1));  // +- 5
  }
}

void Cloud::geometry()
{
  char s[128];
  float minx, miny, minz, maxx, maxy, maxz;
  minx = miny = minz = maxx = maxy = maxz = 0;

  for (int n=0; n < number; n++) {
    minx = MIN(minx, obloids[n].x);
    miny = MIN(miny, obloids[n].y);
    minz = MIN(minz, obloids[n].z);
    maxx = MAX(maxx, obloids[n].x);
    maxy = MAX(maxy, obloids[n].y);
    maxz = MAX(maxz, obloids[n].z);
  }
  sprintf(s, "geom shape=\"bbox\" dim=\"%f %f %f\" />", (maxx-minx)/2, (maxy-miny)/2, (maxz-minz) *.5/2);
  parseSolid(s);
}

void Cloud::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(PERMANENT_MOVEMENT);	// follows wind
  enableBehavior(SPECIFIC_RENDER);
}

void Cloud::inits()
{
  mobileObject(0);

  obloids = new tObloid[number];

  build();
  geometry();
  for (int n=0; n < number; n++) draw(n);

  // wind values
  Wind *wind = World::current()->wind;
  if (wind) {  // get orientation and speed of the wind from meteo server
    pos.az = wind->getOrient() - M_2PI/360;
    uint8_t windspeed = wind->getSpeed();
    speed = ((static_cast<float>(windspeed * 1000.)/3600.));      // km/h -> m/s
    speed /= 1000;	// slower
    //echo("orient=%.2frd %ddg, speed=%.2fm/s %dkm/h", pos.az, RAD2DEG(pos.az), speed*1000, windspeed);
  }
}

Cloud::Cloud(char *l)
{
  parser(l);
  behaviors();
  inits();
}

void Cloud::draw(int n)
{
  obloids[n].dlist = glGenLists(1);
  glNewList(obloids[n].dlist, GL_COMPILE);
  glPushMatrix();
  //float dscale = static_cast<float>((rand()%1 - 0.5));
  //glScalef(2 + dscale, 1.5 + dscale, .5 + dscale);
  glScalef(2, 1.5, .5);
  Draw::sphere(obloids[n].r, 16, 16, 0);
  glPopMatrix();
  glEndList();
}

void Cloud::permanent(float lasting)
{
  if (state == INACTIVE) return;

  // progression
  GLfloat dx = -speed * cos(pos.az);
  GLfloat dy = -speed * sin(pos.az);

  for (int n=0; n < number; n++) {
    obloids[n].x += dx;  // next position
    obloids[n].y += dy;
    if ((fabs(obloids[n].x) > FAR) || (fabs(obloids[n].y) > FAR)) {
      rebuild();
      return;
    }
    // compute global position
    pos.x += obloids[n].x;
    pos.y += obloids[n].y;
    pos.z += obloids[n].z;
  }
  // global position
  pos.x /= static_cast<float>(number);
  pos.y /= static_cast<float>(number);
  pos.z /= static_cast<float>(number);
  //echo("cloud: pos=%.2f %.2f %.2f", pos.x, pos.y, pos.z);
}

void Cloud::render()
{
  glPushMatrix();
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

  for (int n=0; n < number; n++) {
    glPushMatrix();
    glTranslatef(obloids[n].x, obloids[n].y, obloids[n].z);
    glCallList(obloids[n].dlist);
    glPopMatrix();
  }

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glPopMatrix();
}

Cloud::~Cloud()
{
  for (int n=0; n < number; n++) {
    if (obloids[n].dlist > 0) glDeleteLists(obloids[n].dlist, 1);
  }
  delete[] obloids; obloids = NULL;
}

void Cloud::rain(Cloud *cloud, void *d, time_t s, time_t u)
{
  if (! raining) {
    raining = true;
    if (isAction(RAIN_TYPE, Rain::START)) doAction(RAIN_TYPE, Rain::START, cloud, d, s, u);
  }
  else {
    raining = false;
    if (isAction(RAIN_TYPE, Rain::STOP)) doAction(RAIN_TYPE, Rain::STOP, cloud, d, s, u);
  }
}

void Cloud::snow(Cloud *cloud, void *d, time_t s, time_t u)
{
  if (! snowing) {
    snowing = true;
    if (isAction(SNOW_TYPE, Snow::START)) doAction(SNOW_TYPE, Snow::START, cloud, d, s, u);
  }
  else {
    snowing = false;
    if (isAction(SNOW_TYPE, Snow::STOP)) doAction(SNOW_TYPE, Snow::STOP, cloud, d, s, u);
  }
}

void Cloud::sound(Cloud *cloud, void *d, time_t s, time_t u)
{
  if (raining && isAction(RAIN_TYPE, Rain::SOUND)) doAction(RAIN_TYPE, Rain::SOUND, cloud, d, s, u);
}

void Cloud::rain_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  cloud->rain(cloud, d, s, u);
}

void Cloud::snow_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  cloud->snow(cloud, d, s, u);
}

void Cloud::sound_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  cloud->sound(cloud, d, s, u);
}

void Cloud::pause_cb(Cloud *cloud, void *d, time_t s, time_t u)
{
  if (cloud->state == ACTIVE) cloud->state = INACTIVE;
  else cloud->state = ACTIVE;
}

void Cloud::funcs()
{
  setActionFunc(CLOUD_TYPE, 0, _Action rain_cb, "Rain");
  setActionFunc(CLOUD_TYPE, 1, _Action snow_cb, "Snow");
  setActionFunc(CLOUD_TYPE, 2, _Action sound_cb, "Sound");
  setActionFunc(CLOUD_TYPE, 3, _Action pause_cb, "Pause");
}
