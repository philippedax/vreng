//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://www.vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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

#include "vreng.hpp"
#include "gui.hpp"	// new Gui
#include "env.hpp"	// new Env
#include "pref.hpp"	// new Pref
#include "theme.hpp"	// new Theme
#include "timer.hpp"	// new Timer
#include "render.hpp"	// new Render
#include "solid.hpp"	// new Solid
#include "universe.hpp"	// Universe::init
#include "world.hpp"	// World::init
#include "vac.hpp"	// Vac::init
#include "channel.hpp"	// Channel::init
#include "http.hpp"	// HttpT::init
#include "vrsql.hpp"	// VRSql::init
#include "stat.hpp"	// printStats
#include "aiinit.hpp"	// initOcaml
#include "openal.hpp"	// Openal::init


// global
Vreng g;	// variable that refers to various modules

// local
jmp_buf sigctx;


Vreng::Vreng() :	// beware: don't change order !!!
  timer(*new Timer),
  env(*new Env),
  pref(*new Pref),
  render(*new Render),
  solid(*new Solid),
  theme(*new Theme),
  gui(*new Gui)
{
} 

int main(int argc, char *argv[])
{
  return g.start(argc, argv);
}

int Vreng::start(int argc, char *argv[])
{
  pref.init(argc, argv, g.env.prefs());	// Options & Preferences initialization

  UAppli::conf.soft_menus = true;	// beware: only in single window mode
 
  UAppli* appli = new UAppli(argc, argv);
  if (! appli) {
    fatal("Vreng can't be launched");
  }

  theme.init();
  gui.createWidgets();
  gui.showWidgets();

  return appli->start();
}

/*
 * initCB() is implicitely called by Scene::init(),
 * but postponed so that the window can appear immediately
 */
void Vreng::initCB()
{
  timer.init.start();	// starts init timer
  initLimits();		// Change rlimit
  initTrigo();		// Trigo
  initSignals();	// Signals initialization
  Universe::init();	// World manager initialisation
  Http::init();		// Simultaneous http connections initialization
  Channel::init();	// Network initialization
  Vac::init();	    	// Vac cache initialization
  VRSql::init();	// VRSql initialization
#if HAVE_OCAML
  initOcaml();		// Ocaml runtime initialization
#endif
  Openal::init();	// Openal initialization
  World::init(::g.url); // takes a significant amount of time to launch
  timer.init.stop();	// stops init timer
}

void Vreng::quit(int sig)
{
  static int inquit = 0;

  if (sig > 0) {
    cerr << "Got signal " << sig << ", aborting VREng (pid=" << getpid() << ")" << endl;
    signal(sig, SIG_DFL);
    longjmp(sigctx, sig);
  }
  if (inquit > 0) {
    trace(DBG_FORCE, "quit: inquit=%d sig=%d", inquit, sig);
    if (inquit > 2) {
      exit(sig);  // force exit
    }
  }
  inquit++;

  stats();

  // close modules properly
  ::g.render.quit();
  World::current()->quit();

  // quit the application (and close the main window)
  UAppli::quit(sig);
}

void Vreng::stats()
{
  if (::g.pref.stats == false)  return;
  printStats(stderr);

}

static void reapchild(int sig)
{
  int status = 0;

#if HAVE_WAITPID
  while (waitpid(-1, &status, WNOHANG) > 0) ;
#else
  while (wait(&status) != -1) ;
#endif
}

void Vreng::initSignals()
{
  signal(SIGTRAP,quit);
  signal(SIGFPE, quit);
  signal(SIGBUS, quit);
  signal(SIGSEGV,quit);
  signal(SIGCHLD, reapchild);
  signal(SIGUSR1, SIG_IGN);
  signal(SIGUSR2, SIG_IGN);
  trace(DBG_INIT, "Signals initialized");
}

// increases resource limits
void Vreng::initLimits()
{
#if HAVE_SETRLIMIT
  struct rlimit rl;
  getrlimit(RLIMIT_NOFILE, &rl);
  rl.rlim_cur = 4096;
  setrlimit(RLIMIT_NOFILE, &rl);
  //echo("nb. ofile: %d/%d", rl.rlim_cur, rl.rlim_max);
#endif
}