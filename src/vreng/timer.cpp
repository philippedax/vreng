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
#include "timer.hpp"
#include "pref.hpp"	// maxfps & frame_delay
#include "gui.hpp"	// getCycles


ProfileTime::ProfileTime()
{
  memset(&start_time, 0, sizeof(start_time));
  memset(&stop_time, 0, sizeof(stop_time));
  cumul_time = 0;
  diff_time = 0;
}

void ProfileTime::start()
{
  gettimeofday(&start_time, NULL);
}

/* stop time & cumul time, return last diff */
double ProfileTime::stop()
{
  gettimeofday(&stop_time, NULL);
  double d = Times::diffDates(start_time, stop_time);
  diff_time = d;
  cumul_time += d;
  return d;
}

/* returns last diff */
double ProfileTime::diff()
{
  return diff_time;
}

/** Return float time in milliseconds */
double Times::fTime()
{
  struct timeval t;
  gettimeofday(&t, NULL);

  return (((double) t.tv_sec) * 1e3) + (((double) t.tv_usec) * 1e-3);
}

/** Computes t2-t1 in seconds */
double Times::diffDates(struct timeval t1, struct timeval t2)
{
  return ((double) t2.tv_sec - (double) t1.tv_sec) +
  ((double) t2.tv_usec - (double) t1.tv_usec) * 1e-6;
}

float Times::getRate()
{
  double time_cycles = simul.cumul_time + render.cumul_time + idle.cumul_time;
  if (time_cycles == 0)
    return 5.0;	// 5 sec
  else return
    (float) (::g.gui.getCycles() / time_cycles);
}

bool Times::isRate(uint16_t rate)
{
  int ratio = int(getRate() / rate);
  return ((ratio <= 1) || ((ratio > 1) && (::g.gui.getCycles() % ratio) == 1));
}

/* Returns the number of seconds that have elapsed since
   the previous call to this function. */
float Times::delta()
{
  static bool first = true;
  static timeval lasttime;
  timeval nowtime;

  if (first) {
    gettimeofday(&lasttime, NULL);
    first = false;
  }
  gettimeofday(&nowtime, NULL);

  double difftime = Times::diffDates(lasttime, nowtime);
  lasttime = nowtime;
  return (float)difftime;
}

#if 0 //notused
void idleTime()
{
  startTime(&ptime_idle);
  float rate = getRate();
  if (rate > ::g.pref.maxfps && ::g.pref.cpulimit) {
    struct timeval to;
    to.tv_sec = 0;
    to.tv_usec = ::g.pref.frame_delay;     // 20ms -> 50 fps
    select(0, 0, 0, 0, &to);
  }
  stopTime(&ptime_idle);
}
#endif

