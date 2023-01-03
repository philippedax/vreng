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
#ifndef SNOW_HPP
#define SNOW_HPP

#include "wobject.hpp"
#include "particle.hpp"

#define SNOW_TYPE	65
#define SNOW_NAME	"Snow"


/**
 * Snow class
 */
class Snow: public Particle {

public:
  enum {
    START,
    STOP,
    NONE
  };

  Snow(class Cloud *o, void *d, time_t s, time_t u);
  /**< constructor: called by cloud */

  static void funcs();	///< init funclist

  void changePermanent(float lasting);
  /**< Equations to move permanetly */

private:
  class Cloud *pcloud;

  void defaults();
  /**< Sets default values */

  void makeSolid();

  // Callbacks
  static void start_cb(class Cloud *cloud, void *d, time_t s, time_t u);
  static void stop_cb(class Cloud *cloud, void *d, time_t s, time_t u);

};

#endif
