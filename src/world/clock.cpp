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
// clock.cpp
//
// Clock handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "clock.hpp"


const OClass Clock::oclass(CLOCK_TYPE, "Clock", Clock::creator);

// local
static uint16_t oid = 1;

void Clock::funcs() {}


/** External clock (from vre file) */
Clock::Clock(char *l)
{
  parser(l);

  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(PERMANENT_MOVEMENT);

  mobileObject(0);

  if (! haveneedle) needle = oid;
  inits();
  oid++;
}

/** Internal clock */
Clock::Clock()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(NO_BBABLE);
  enableBehavior(UNSELECTABLE);
  enableBehavior(UNVISIBLE);
  enableBehavior(PERMANENT_MOVEMENT);

  mobileObject(0);

  needle = SECOND;
  inits();
}

/** Inits time */
void Clock::inits()
{
  time_t t = time(0);
  struct tm *ptime = localtime(&t);
  sec = ptime->tm_sec;
  min = ptime->tm_min;
  hour = ptime->tm_hour;
  sec_last = min_last = hour_last = 255;

  permanent(60.);
}

void Clock::parser(char *l)
{
  haveneedle = false;

  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (! stringcmp(l, "needle")) {
      char needlestr[8] = "";
      l = parseString(l, needlestr, "needle");
      haveneedle = true;
      switch (*needlestr) {
        case 's': needle = SECOND; break;
        case 'm': needle = MINUTE; break;
        case 'h': needle = HOUR;   break;
        default: haveneedle = false;
      }
    }
  }
  end_while_parse(l);
}

void Clock::permanent(float lasting)
{
  time_t t = time(0);
  struct tm *ptime = localtime(&t);

  sec = ptime->tm_sec;
  min = ptime->tm_min;
  hour = ptime->tm_hour;
  yday = ptime->tm_yday;

  switch (needle % 3) {
    case SECOND:	// secondes
      pos.ax = (static_cast<float>(sec * M_PI / 30)) + M_PI_2;
      break;
    case MINUTE:	// minutes
      if (min != min_last) {
        pos.ax = (static_cast<float>(min * M_PI / 30)) + M_PI_2;
        min_last = min;
      }
      break;
    case HOUR:	// hours
      if (hour != hour_last) {
        pos.ax = (static_cast<float>(hour * M_PI / 6)) +
                 (static_cast<float>(min * M_PI / 360)) + M_PI_2;
        hour_last = hour;
      }
      break;
  }
  sec_last = sec;

  // Can be used with changing Bgcolor
  daytime = 12 - 4 * Cos(360 * static_cast<float>(yday/365.));
  risetime = 13 - daytime/2;
  falltime = 13 + daytime/2;
}

/** creation from a file */
Object * Clock::creator(char *l)
{
  return new Clock(l);
}

Clock::~Clock()
{
  oid = 0;
}
