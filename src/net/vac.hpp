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
#ifndef VAC_HPP
#define VAC_HPP

/**
 * Vac class
 */
class Vac {

 public:
  char url[URL_LEN];		///< world url
  char channel[CHAN_LEN];	///< mapped channel
  Vac *next;			///< next entry

  Vac();		///< Constructor

  virtual ~Vac() {};	///< Destructor

  virtual bool getList();
  /**< Gets the whole cache and put it into an internal list */

  static int connectVacs();
  /**< Establishes a connection with the Vacs server */

  static int connectVac();
  /**< Establishes a connection with the Vacs server */

  static bool resolveWorldUrl(const char *_url, char *chanstr);
  /**< Resolves an Url and retrieves a channel string */

  static bool getChannel(const char *_url, char *chanstr);
  /**< Gets a channel by its url from the list */

  static bool getUrlAndChannel(const char *name, char *_url, char *chanstr);
  /**< Gets an url and its channel by its world name from the list */

  static void* init(void *arg);
  /**< Init vac */
};

#endif
