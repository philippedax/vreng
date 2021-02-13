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

#include <ubit/ubit.hpp>
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
#include "http.hpp"	// HttpThread::init
#include "vrsql.hpp"	// VRSql::init
#include "stat.hpp"	// statNetwork
#include "prof.hpp"	// statMemory
#include "aiinit.hpp"	// initOcaml
#include "openal.hpp"	// Openal::init

#include <setjmp.h>	// jmp_buf
#include <sys/resource.h>	// rlimit

using namespace ubit;

// global variable that refers to the various modules
Global g;

jmp_buf sigctx;


Global::Global() :
	debug(0),
	options(0),
	timer(*new Timer),
	env(*new Env),
	pref(*new Pref),
	render(*new Render),
	solid(*new Solid),
	theme(*new Theme),
	gui(*new Gui) {} 

int main(int argc, char *argv[])
{
  return g.start(argc, argv);
}

int Global::start(int argc, char *argv[])
{
  timer.init.start();
  pref.init(argc, argv, g.env.prefs());	// Options & Preferences initialization

  // UAppli::conf.setDepthBuffer(16);   // utile ???  
  UAppli::conf.soft_menus = true;	// attention: seulement en mode single window
  
  UAppli* appli = new UAppli(argc, argv);
  if (!appli) fatal("Vreng can't be launched");

  theme.setTheme(0);

  gui.createWidgets();
  gui.showWidgets();

  // A REVOIR if (pref.infogl) Ogl::infosGL();
  // startCB() is implicitely called, but postponed so that the window
  // can appear immediately
#if HAVE_OCAML
  initOcaml(argv);		// Ocaml runtime initialization
#endif
  return appli->start();
}

void Global::startCB()
{
  initLimits();		// Change rlimit
  initTrigo();		// Trigo
  initSignals();	// Signals initialization
  HttpThread::init();	// Simultaneous http connections initialization
  Channel::init();	// Network initialization
  Universe::init();	// World manager initialisation
  Vac::init();	    	// Vac cache initialization
#if HAVE_SQLITE | HAVE_MYSQL
  VRSql::init();	// MySql initialization
#endif
#if HAVE_OPENAL
  Openal::init();	// Openal initialization
#endif
  // NB: this function takes a significant amount of time to launch
  World::init(Universe::current()->url);
  timer.init.stop();
}

void Global::quitVreng(int signum)
{
  static int inquit = 0;

  if (signum > 0) {
    cerr << "Got signal " << signum <<
    ", aborting VREng (pid=" << getpid() << " ppid=" << getppid() << ")" << endl;
    signal(signum, SIG_DFL);
    longjmp(sigctx, signum);
  }
  if (inquit > 0) {
    trace(DBG_FORCE, "quit: inquit=%d sig=%d", inquit, signum);
    if (inquit > 3) exit(signum);  // force exit without quitting properly
    else inquit++;
  }
  inquit++;

  printStats();

  // close modules properly
  ::g.render.quit();
  if (World::current()) World::current()->quit();
  delete Channel::current();
  delete Channel::getManager();

  // quit the application (and close the main window)
  UAppli::quit(signum);
}

void Global::printStats()
{
  if (::g.pref.stats == false)
    return;
  statLog();
  statTimings();
  statNetwork();
  Render::stat();
  statMemory();
  statIO();
}

extern int my_wait(pid_t);
static void reapchild(int sig)
{
  my_wait(-1);
}

void Global::initSignals()
{
  signal(SIGTRAP,quitVreng);
  signal(SIGFPE, quitVreng);
  signal(SIGBUS, quitVreng);
  signal(SIGSEGV,quitVreng);
  signal(SIGCHLD, reapchild);
  signal(SIGUSR1, SIG_IGN);
  signal(SIGUSR2, SIG_IGN);
  trace(DBG_INIT, "Signals initialized");
#if 0
  longjmp(sigctx, 0);
#endif
}

// increases resource limits
void Global::initLimits()
{
#if HAVE_SETRLIMIT
  struct rlimit rl;
  getrlimit(RLIMIT_NOFILE, &rl);
  //printf("default nofile: %d/%d\n", rl.rlim_cur, rl.rlim_max);
  rl.rlim_cur = 4096;
  setrlimit(RLIMIT_NOFILE, &rl);
  //printf("new nofile: %d/%d\n", rl.rlim_cur, rl.rlim_max);
#endif
}
