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
// universe.hpp
//---------------------------------------------------------------------------
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

class User;

#define	MANAGER_NAME	"manager"


/**
 * Universe class
 */
class Universe {
 private:
  char *server;		///< http server name
  char *url;		///< url
  char *urlpfx;		///< url prefix
  class Wheel *wheel;	///< wheel instance
  pthread_t wheel_tid;  ///< wheel thread

  static void sigWheel(int sig);
  /**< Intercepts signal SIGTERM */

  static void * runWheel(void *arg);
  /**< Runs the wheel progression indicator */

 public:
  char *grpstr;		///< group str
  uint16_t port;	///< port
  uint8_t ttl;		///< scope
  User *localuser;	///< current player (init by World !)
  uint8_t worldcnt;	///< world counter

  Universe();		///< Constructor

  virtual ~Universe();	///< Destructor

  static Universe * current();	///< pointer

  static void init();    ///< create the "world_manager"

  void startWheel();
  /**< Starts the wheel progression indicator */

  void stopWheel();
  /**< Stops the wheel progression indicator */
};

extern class World *world_manager;

#endif
