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
#include "gui.hpp"
#include "theme.hpp"
#include "world.hpp"	// World
#include "channel.hpp"	// initNetwork
#include "pref.hpp"	// init
#include "env.hpp"	// init
#include "vac.hpp"	// init
#include "universe.hpp"	// initManager
#include "http.hpp"	// HttpThread::init
#include "mysql.hpp"	// VRSql::init
#include "render.hpp"	// Render::quit
#include "stat.hpp"	// statNetwork
#include "prof.hpp"	// statMemory
#include "timer.hpp"	// ptime_*
#include "openal.hpp"	// Openal::init
#include "aiinit.hpp"	// initOcaml

#include "setjmp.h"	// jmp_buf
using namespace ubit;

// global variable that refers to the various modules
Global g;

jmp_buf sigctx;


Global::Global() :
	debug(0),
	options(0),
	times(*new Times),
	env(*new Env),
	pref(*new Pref),
	render(*new Render),
	theme(*new Theme),
	gui(*new Gui) {} 

int main(int argc, char *argv[])
{
  return g.start(argc, argv);
}

int Global::start(int argc, char *argv[])
{
#if 0
  if (setjmp(sigctx)) error("Restart Vreng");
#endif
  times.init.start();
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
  initTrigo();		// Trigo
  initSignals();	// Signals initialization
  HttpThread::init();	// Simultaneous http connections initialization
  Channel::init();	// Network initialization
  Universe::init();	// World manager initialisation
  Vac::init();	    	// Vac cache initialization
#if HAVE_MYSQL
  VRSql::init();	// MySql initialization
#endif
#if HAVE_OPENAL
  Openal::init();	// Openal initialization
#endif
  // NB: this function takes a significant amount of time to launch
  World::init(Universe::current()->url);
  times.init.stop();
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
  
#ifdef DMALLOC_FUNC_CHECK
  dmalloc_shutdown();
#endif
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

  statMemory();
  statIO();
}

void Global::printStats()
{
  statLog();
  statTimings();
  statNetwork();
  Render::stat();
}

extern int my_wait(pid_t);
static void reapchild(int sig)
{
  //error("child pid/ppid=%d/%d", (int)getpid(), (int)getppid());
  my_wait(-1);
}

void Global::initSignals()
{
  //DAX signal(SIGILL, quitVreng);
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

