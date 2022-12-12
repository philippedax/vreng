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

#include "sysdep.hpp"	// system includes
#include "macros.hpp"	// builtin defines
#include "defaults.hpp"	// DEF_*
#include "log.hpp"	// notice, warning, fatal, trace
#include "matvec.hpp"	// matrix M4 vector V3
#include "trigo.hpp"	// my_trigo


/**
 * Vreng class
 */
class Vreng {

public:
  // global variables reachable by ::g.variable
  char *version;        ///< VREng version.
  char *universe;       ///< universe httpd server url.
  char *server;         ///< server httpd.
  char *url;            ///< initial world url.
  char *channel;        ///< initial Multicast channel.
  char *user;           ///< user name.
  char *urlpfx;         ///< urlpfx.
  char *skinf;          ///< front face url.
  char *skinb;          ///< back face url.
  uint32_t debug;	///< debug flags.
  uint32_t options;	///x command line options.

  // global methods usable by ::g.method
  class Timer& timer;	///< ::g.timer
  class Env& env;	///< ::g.env
  class Pref& pref;	///< ::g.pref
  class Render& render;	///< ::g.render
  class Solid& solid;	///< ::g.solid
  class Theme& theme;	///< ::g.theme used to paramaterize the GUI.
  class Gui& gui;	///< ::g.gui

  // Methods

  Vreng();		///< constructor.
  ~Vreng() {}		///< destructor.
  
  static void quit(int status);

  int start(int argc, char *argv[]);

  void initCB();

private:

  static void stats();

  void initSignals();

  void initLimits();
};

extern Vreng g;

#endif
