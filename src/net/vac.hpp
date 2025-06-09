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
// vac.hpp
//---------------------------------------------------------------------------
#ifndef VAC_HPP
#define VAC_HPP


/**
 * Vac class
 */
class Vac {
 private:
  char url[URL_LEN];		///< world url
  char channel[CHAN_LEN];	///< mapped channel
  bool connected;		///x true if connect established
  int sdvac;			///< file descriptor
  Vac *next;			///< next entry

  bool connectVacs();
  /**< Establishes a connection with the Vacs server */

  static void * connectThread(void *);
  /**< Establishes a connection with the Vacs server */

  void setConnected();

 public:
  Vac();		///< Constructor
  ~Vac() {};		///< Destructor

  bool getList();
  /**< Gets the whole cache and put it into an internal list */

  bool resolveWorldUrl(const char *_url, char *chanstr);
  /**< Resolves an Url and retrieves a channel string */

  bool getChannel(const char *_url, char *chanstr);
  /**< Gets a channel by its url from the list */

  bool getUrlAndChannel(const char *name, char *_url, char *chanstr);
  /**< Gets an url and its channel by its world name from the list */

  static Vac * init();
  /**< Init vac */

  static  Vac * current();
  /**< Returns vac instance */
};

#endif
