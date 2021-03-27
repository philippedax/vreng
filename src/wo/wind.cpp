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
#include "wind.hpp"
#include "world.hpp"	// current
#include "universe.hpp"	// current


const OClass Wind::oclass(WIND_TYPE, "Wind", Wind::creator);

Wind * Wind::wind = NULL;	// singleton


/* creation from a file */
WObject * Wind::creator(char *l)
{
  return new Wind(l);
}

void Wind::defaults()
{
  speed = 0;
  orient = 0;
}

void Wind::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "speed"))  l = parse()->parseUInt16(l, &speed, "speed");
    else if (!stringcmp(l, "orient")) l = parse()->parseFloat(l, &orient, "orient");
  }
  end_while_parse(l);
}

void * Wind::getHttp(void * arg)
{
  char url[URL_LEN];

  sprintf(url, "http://%s/%s/cgi/wind.cgi", Universe::current()->server, Universe::current()->urlpfx);

  char cmd[128];
  *cmd ='\0';
#if HAVE_CURL
  sprintf(cmd, "IFS=' '; curl -L -s %s", url);
#else
#if HAVE_WGET
  sprintf(cmd, "IFS=' '; wget -q -nv -O - %s", url);
#endif
#endif
  FILE *pp;
  if ((pp = popen(cmd, "r")) == NULL) {
    perror("popen wind");
    return NULL;
  }

  int orientation=0, speed=0;
  char buf[128];
  fgets(buf, sizeof(buf), pp);
  if (isdigit(*buf)) {
    orientation = -atoi(buf);
    char *p = strrchr(buf, ' ');
    if (p) speed = atoi(++p);
  }
  pclose(pp);

  Wind *wind = current();
  wind->orient = DEG2RAD(orientation);
  wind->speed = speed;
  notice("wind: orientation=%d (%.2frd) speed=%dkm/h", orientation, wind->orient, speed);
  pthread_exit(NULL);
  return NULL;
}

void sigwind(int s)
{
  pthread_exit(NULL);
}

Wind::Wind(char *l)
{
  parser(l);

  World *world = World::current();
  world->wind = this;	//FIXME: world->setWind(this);

  initObject(INVISIBLE);
  wind = this;

#if HAVE_LIBPTHREAD
  int r = pthread_create(&tid, NULL, getHttp, (void *) NULL);
  if (r)
    perror("wind: pthread_create");
  //dax pthread_join(tid, NULL);
#else
  if (fork() == 0) {
    getHttp((void *) NULL);
    exit(0);
  }
#endif
}

Wind * Wind::current()
{
  return wind;
}

uint16_t Wind::getSpeed() const
{
  return speed;
}

float Wind::getOrient() const
{
  return orient;
}

void Wind::quit()
{
  wind = NULL;
}

void Wind::funcs() {}
