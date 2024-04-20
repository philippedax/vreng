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
// aoi.hpp
//---------------------------------------------------------------------------
#ifndef AOI_HPP
#define AOI_HPP

#include "wobject.hpp"

#define AOI_TYPE	52
#define AOI_NAME	"Aoi"

#define AOI_VISIBLE	1	// if hard debugging time


/**
 * Aoi class
 * Area of Interest
 */
class Aoi: public WO {
 private:
  char chan[CHAN_LEN];	///< aoi mcast channel string

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Aoi(char *l);		///< Constructor

  static WO * (creator)(char *l);
  /**< Creates from fileline */

  void aoiEnter();
  /**<
   * Performs action to be done while entering a new Area of Interest:
   *  - leaving previous one with aoiQuit() call
   *  - initializing the network on new AoI's mcast chan_str
   *  - initializing the only mobile object we know for now in
   *      this AoI: local user's avatar
   *
   * Unique caller is generalIntersect() (col.cc)
   */

  static void aoiQuit();
  /**<
   * Performs actions to be done while leaving an Area of Interest:
   *  - Remove all mobile object from network (leaving only static ones)
   *  - Shutdown the network
   *
   * Unique caller is aoiEnter() ... (see code aoi.cc)
   */

 private:
  void parser(char *l);
  /**< Parses */

  void defaults();
  /**< Sets defauts values */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */
};

extern Aoi *currentAoi;

#endif
