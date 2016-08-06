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
#ifndef UNIVERSE_HPP
#define UNIVERSE_HPP

class World;
class User;

#define	MANAGER_NAME	"manager"
#define MAX_WORLDS	256


/**
 * Universe class
 */
class Universe {

 public:
  char *name;		///< http server name
  char *url;		///< url
  char *group;		///< group addr
  char *urlpfx;		///< url prefix
  uint16_t port;	///< port
  uint8_t ttl;		///< scope
  uint8_t version;	///< vre version
  uint8_t prop;		///< notused
  uint16_t worldcnt;	///< world counter
  World *worldList;	///< list of worlds
  User *localuser;	///< current player
  class Wheel *wheel;	///< wheel instance
#if HAVE_LIBPTHREAD
  pthread_t wheel_tid;  ///< wheel thread
#endif

  Universe();		///< Constructor
  virtual ~Universe();	///< Destructor

  static Universe * current();	///< pointer

  static void initManager();    ///< create the "manager"

  virtual void getUrl(char *_url);	///< gets url string

  virtual void startWheel();
  /**< Starts the wheel progression indicator */

  static void sigWheel(int sig);
  /**< Intercepts signal SIGTERM */

  static void * runWheel(void * arg);
  /**< Runs the wheel progression indicator */

  virtual void stopWheel();
  /**< Stops the wheel progression indicator */
};

extern World *manager;

#endif
