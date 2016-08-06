//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#include "vreng.hpp"
#include "browser.hpp"
#include "app.hpp"
#include "url.hpp"	// abs


#if MACOSX
int Browser::toolid = SAFARI_TOOL;
#else
int Browser::toolid = FIREFOX_TOOL;
#endif

// local
static pid_t pid = -1;
static int newwindow = 0;


void Browser::init(int _toolid)
{
  toolid = _toolid;
}

void Browser::launchmozilla(const char *browser, const char *url)
{
  char openurl[URL_LEN + 20];

  if (! newwindow)
    sprintf(openurl, "openURL(%s, new-window)", url);
  else
    sprintf(openurl, "openURL(%s)", url);
  newwindow++;
  pid_t pid = fork();
  switch (pid) {
  case -1:
    error("%s %s", e_fork, browser);
    break;
  case 0:
#if MACOSX
    execlp("open", "open", url, (void*)NULL);
#else
    execlp(browser, browser, "-remote", openurl, (void*)NULL);
    execlp("netscape", "netscape", "-remote", openurl, (void*)NULL);
#endif
    error("%s %s", e_exec, browser);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Browser::launchsafari(const char *browser, const char *url)
{
  pid_t pid = fork();
  switch (pid) {
  case -1:
    error("%s %s", e_fork, browser);
    break;
  case 0:
#if MACOSX
    execlp("open", "open", "-a", SAFARI_PATH, url, (void*)NULL);
#endif
    error("%s %s", e_exec, browser);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Browser::launchchrome(const char *browser, const char *url)
{
  pid_t pid = fork();
  switch (pid) {
  case -1:
    error("%s %s", e_fork, browser);
    break;
  case 0:
#if MACOSX
    execlp("open", "open", "-a", CHROME_PATH, "--args", "--homepage", url, (void*)NULL);
#else
    execlp(browser, browser, url, (void*)NULL);
#endif
    error("%s %s", e_exec, browser);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Browser::launchnetscape(const char *browser, const char *url)
{
#ifdef LAUNCHNETSCAPE
  if (pid < 0) {
    switch (pid = fork()) {
    case -1:
      error("%s %s", e_fork, browser);
      break;
    case 0:
#if MACOSX
      execlp("open", "open", url, (void*)NULL);
#else
      execlp(browser, browser, url, (void*)NULL);
#endif
      error("%s %s", e_exec, browser);
      signal(SIGCHLD, SIG_IGN);
      exit(1);
    default:
      break;
    }
  }
  else {
#endif
    char openurl[URL_LEN];

    if (! newwindow)
      sprintf(openurl, "openURL(%s, new-window)", url);
    else
      sprintf(openurl, "openURL(%s)", url);
    newwindow++;
    pid_t pid = fork();
    switch (pid) {
    case -1:
      error("%s %s", e_fork, browser);
      break;
    case 0:
#if MACOSX
      execlp("open", "open", url, (void*)NULL);
#else
      execlp(browser, browser, "-remote", openurl, (void*)NULL);
#endif
      error("%s %s", e_exec, browser);
      signal(SIGCHLD, SIG_IGN);
      exit(1);
    default:
      break;
    }
#ifdef LAUNCHNETSCAPE
  }
#endif
}

void Browser::start(const char *_url, bool rewrite)
{
  char url[URL_LEN];

  if (rewrite) Url::abs(_url, url);
  else strcpy(url, _url);

  switch (toolid) {
  case FIREFOX_TOOL:   launchmozilla("firefox", url); break;
  case SAFARI_TOOL:    launchsafari("safari", url); break;
  case CHROME_TOOL:    launchchrome("chrome", url); break;
  case OPERA_TOOL:     launchmozilla("opera", url); break;
  case IEXPLORER_TOOL: launchmozilla("iexplorer", url); break;
  case MOZILLA_TOOL:   launchmozilla("mozilla", url); break;
  case NETSCAPE_TOOL:  launchnetscape("netscape", url); break;
  default:             error("%s browser", e_tool);
  }
}

void Browser::quit()
{
  if (pid > 0) {
    kill(pid, SIGKILL);
    pid = -1;
  }
}
