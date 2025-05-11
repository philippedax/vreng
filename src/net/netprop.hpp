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
// netprop.hpp
//---------------------------------------------------------------------------
#ifndef NETPROP_HPP
#define NETPROP_HPP


/**
 * NetProp class
 */
class NetProp {
 public:
  bool responsible;		///< flag responsibility
  int16_t version;		///< version number
  struct timeval last_seen;	///< last seen date
  struct timeval assume_at;	///< assume at date
  float min_assume_delay;	///< min in secs
  float max_assume_delay;	///< max in secs

  NetProp();			///< Constructor
  virtual ~NetProp();		///< Destructor

  void newDate();
  /**<
   * Computes a new date for the assume_at of the property
   * and sets the last_seen at "now"
   */

  void setResponsible(bool flag);

  static uint8_t getProperties(uint8_t _type_id);
  static void setProperties(uint8_t _type_id, uint8_t _nbprop);
  static void addToDate(struct timeval *t, double d);
};

#endif
