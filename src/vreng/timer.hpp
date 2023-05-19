//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef TIMER_HPP
#define TIMER_HPP

/**
 * ProfileTime class
 */
class ProfileTime {
public:

  ProfileTime();
  
  void start();
  double stop();
  double diff();

  struct timeval start_time;
  struct timeval stop_time;
  double cumul_time;
  double diff_time;
};


/**
 * Timer class
 */
class Timer {
public:

  ProfileTime init, net, simul, render, idle, capture, sql, object, image;

  float rate();
  /**< Returns actual rate */

  bool isRate(uint16_t rate);

  static double fTime();
  /**< Returns float time in milliseconds */

  static double diffDates(struct timeval t1, struct timeval t2);
  /**< Computes t2-t1 in seconds */

  static float delta();
  /**< Returns the number of seconds that have elapsed
       since the previous call to this function. */
};

#endif
