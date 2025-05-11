//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// netprop.cpp
//
// Network Properties handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "netprop.hpp"
#include "objects.hpp"	// getPropertiesnumber
#include "stat.hpp"	// new_netprop


NetProp::NetProp()
{
  min_assume_delay = DEF_REFRESH_TIMEOUT * 2;
  max_assume_delay = DEF_REFRESH_TIMEOUT * 5;
  new_netprop++;
}

/** Initializes responsibilities */
void NetProp::setResponsible(bool _responsible)
{
  responsible = _responsible;
}

NetProp::~NetProp()
{
  del_netprop++;
}

/** Returns the number of properties of this type */
uint8_t NetProp::getProperties(uint8_t _type_id)
{
  return getPropertiesnumber(_type_id);
}

/** Sets the number of properties of this type */
void NetProp::setProperties(uint8_t _type_id, uint8_t _nbprop)
{
  setPropertiesnumber(_type_id, _nbprop);
}

/** Adds d seconds to the date */
void NetProp::addToDate(struct timeval *t, double d)
{
  float f = floor(d);

  t->tv_sec += (time_t) f;
  t->tv_usec += (time_t) (d-f)*1e6;
  while (t->tv_usec >= 1000000) {
    t->tv_usec -= 1000000;
    t->tv_sec++;
  }
}

/**
 * Computes a new date for the assume_at of the property
 * and sets the last_seen at "now"
 */
void NetProp::newDate()
{
  struct timeval now;
  gettimeofday(&now, NULL);

  last_seen = now;
  assume_at = now;

  double delay = min_assume_delay + rand()/(RAND_MAX+1.0)*(max_assume_delay - min_assume_delay);
  addToDate(&assume_at, delay);
}
