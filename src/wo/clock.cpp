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
#include "clock.hpp"


const OClass Clock::oclass(CLOCK_TYPE, "Clock", Clock::creator);

// local
static uint16_t oid = 1;

void Clock::funcs() {}


void Clock::initTime()
{
  time_t t = time(0);
  struct tm *pdate = localtime(&t);
  sec = pdate->tm_sec;
  min = pdate->tm_min;
  hour = pdate->tm_hour;
  sec_last = min_last = hour_last = 255;

  enablePermanentMovement();
  changePermanent(0.);
}

void Clock::updateTime(time_t s, time_t us, float *lasting)
{
  struct tm *pdate = localtime(&s);

  sec = pdate->tm_sec;
  min = pdate->tm_min;
  hour = pdate->tm_hour;
  yday = pdate->tm_yday;
}

void Clock::changePermanent(float lasting)
{
  switch (needle % 3) {
    case SECOND:	// secondes
      pos.ax = ((float) sec * M_PI / 30) + M_PI_2;
      //trace(DBG_WO, "sec=%d %f obj=%d", sec, pos.ax, needle);
      break;
    case MINUTE:	// minutes
      if (min != min_last) {
        pos.ax = ((float) min * M_PI / 30) + M_PI_2;
        min_last = min;
      }
      break;
    case HOUR:	// hours
      if (hour != hour_last) {
        pos.ax = ((float) hour * M_PI / 6) +
                 ((float) min * M_PI / 360) + M_PI_2;
        //trace(DBG_WO, "hour=%d %f", hour % 12, pos.ax);
        hour_last = hour;
      }
      break;
  }
  sec_last = sec;

  // Can be used with changing Bgcolor
  daytime = 12 - 4 * Cos(360 * (float) yday/365.);
  risetime = 13 - daytime/2;
  falltime = 13 + daytime/2;
}

/** creation from a file */
WObject * Clock::creator(char *l)
{
  return new Clock(l);
}

void Clock::parser(char *l)
{
  haveneedle = false;

  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if (!stringcmp(l, "needle")) {
      char needlestr[8] = "";
      l = parse()->parseString(l, needlestr, "needle");
      haveneedle = true;
      switch (*needlestr) {
        case 's': needle = SECOND; break;
        case 'm': needle = MINUTE; break;
        case 'h': needle = HOUR; break;
        default: haveneedle = false;
      }
    }
  }
  end_while_parse(l);
}

/** External clock */
Clock::Clock(char *l)
{
  parser(l);

  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(COLLIDE_NEVER);
  setRenderPrior(RENDER_HIGH);

  initializeMobileObject(0);

  if (! haveneedle) needle = oid;
  initTime();
  trace(DBG_WO, "Clock: needle=%d name=%s", needle, getInstance());
  oid++;
}

/** Internal clock */
Clock::Clock()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(UNVISIBLE);
  enableBehavior(COLLIDE_NEVER);
  setRenderPrior(RENDER_HIGH);

  initializeMobileObject(0);

  needle = SECOND;
  initTime();
}

void Clock::quit()
{
  oid = 0;
}
