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
#ifndef VRENG_HPP
#define VRENG_HPP

//using namespace std;

#include "sysdep.hpp"	// system includes
#include "macros.hpp"	// builtin defines
#include "defaults.hpp"	// DEF_*
#include "trigo.hpp"	// my_trigo functions
#include "log.hpp"	// notice, warning, fatal, trace
#include "matvec.hpp"	// matrices M4, vectors V3


class Global {
public:
  Global();
  ~Global() {}

  char *version;        ///< VREng version
  char *universe;       ///< universe httpd server url
  char *server;         ///< server httpd
  char *url;            ///< initial world url
  char *channel;        ///< initial Multicast channel
  char *user;           ///< user name
  char *skinf;          ///< front face url
  char *skinb;          ///< back face url

  uint32_t debug;
  uint32_t options;

  // ::g instances
  class Times& times;
  class Env& env;
  class Pref& pref;
  class Render& render;
  class Solid& solid;
  struct Theme& theme; ///< the theme used to paramaterize the GUI.
  class Gui& gui;

  int start(int argc, char *argv[]);

  void startCB();
  
  static void quitVreng(int status);

  static void printStats();

  void initSignals();
};

extern Global g;

#endif
