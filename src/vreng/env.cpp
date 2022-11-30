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
#include "vreng.hpp"
#include "env.hpp"


Env::Env()
{
  strcpy(systemname, "unknown");
  strcpy(releasename, "unknown");
  strcpy(machinename, "unknown");

  // Set some process attributes
#if HAVE_NICE
  nice(10);
#endif
#if HAVE_FPSETMASK
  fpsetmask(0L);                // What is it for ? (BD)
#endif
#if HAVE_SETLOCALE
  setlocale(LC_ALL, "C");       // Unix traditionnal
#endif
  
  sysinfo();
  init();
}

const char * Env::home() const
{
  return homedir;
}

const char * Env::login() const
{
  return logname;
}

const char * Env::cwd() const
{
  return vrengcwd;
}

const char * Env::dir() const
{
  return vrengdir;
}

const char * Env::cache() const
{
  return vrengcache;
}

const char * Env::prefs() const
{
  return vrengprefs;
}

const char * Env::icons() const
{
  return vrengicons;
}

const char * Env::menu() const
{
  return vrengmenu;
}

const char * Env::worldmarks() const
{
  return vrengworldmarks;
}

const char * Env::passwd() const
{
  return vrengpasswd;
}

const char * Env::sysname() const
{
  return systemname;
}

const char * Env::relname() const
{
  return releasename;
}

const char * Env::machname() const
{
  return machinename;
}

void Env::sysinfo()
{
#if HAVE_UNAME
  struct utsname myutsname;

  uname(&myutsname);
  strcpy(systemname, myutsname.sysname);
  strcpy(releasename, myutsname.release);
  strcpy(machinename, myutsname.machine);
  for (int i=0; i < sizeof(machinename); i++) {
    if (machinename[i] == ' ') {
      machinename[i] = '_';
    }
  }
#endif
}

void Env::init()
{
  char pathenvdir[PATH_LEN+8];
  char pathweb[PATH_LEN+8];
  char pathhtdocs[PATH_LEN+8];
  char pathdata[PATH_LEN+8];
  char pathprefs[PATH_LEN+16];
  char pathmenu[PATH_LEN+16];
  char pathicons[PATH_LEN+16];
  char pathworldmarks[PATH_LEN+32];
  char pathcache[PATH_LEN+32];
  char pathpasswd[PATH_LEN+32];
  struct stat bufstat;
  char *home = NULL;
  char *loguser = NULL;

  // $CWD
  if (getcwd(vrengcwd, sizeof(vrengcwd)) == NULL) {
    return;
  }
  strcpy(homedir, vrengcwd);

  // $HOME
  if ((home = getenv("HOME"))) {
    strcpy(homedir, home);
  }

  // $LOGNAME
  if ((loguser = getenv("LOGNAME"))) {
    strcpy(logname, loguser);
  }

  if (! strcmp(DEF_HTTP_SERVER, "localhost") && strlen(DEF_URL_PFX) > 0) { // if local
    sprintf(pathdata, "%s/htdocs", vrengcwd);	// htdocs location
#if MACOSX
    sprintf(pathweb, "%s/Sites", homedir);
#else
    sprintf(pathweb, "%s/public_html", homedir);
#endif
    if (stat(pathweb, &bufstat) < 0) {
      error("sites does not exist: %s", pathweb);
      // $HOME/public_html or $HOME/Sites
      mkdir(pathweb, 0755);
      sprintf(pathhtdocs, "%s/vreng", pathweb);
      if (stat(pathhtdocs, &bufstat) < 0) {
        int r;
        r = symlink(pathdata, pathhtdocs);
        if (r == 0) {
          error("create link htdocs: %s", pathhtdocs);
        }
        else {
          error("can't create link htdocs: %s (%d)", pathhtdocs, r);
        }
      }
    }
    else {
      sprintf(pathhtdocs, "%s/vreng", pathweb);
      if (stat(pathhtdocs, &bufstat) < 0) {
        int r;
        r = symlink(pathdata, pathhtdocs);
        if (r == 0) {
          error("create link htdocs: %s", pathhtdocs);
        }
        else {
          error("can't create link htdocs: %s (%d)", pathhtdocs, r);
        }
      }
    }
  }

  // $HOME/.vreng
  sprintf(pathenvdir, "%s/.vreng", homedir);
  if (stat(pathenvdir, &bufstat) < 0) {
    mkdir(pathenvdir, 0700);
  }
  strcpy(vrengdir, pathenvdir);
  chdir(pathenvdir);

  // $HOME/.vreng/prefs
  sprintf(pathprefs, "%s/prefs", pathenvdir);
  strcpy(vrengprefs, pathprefs);

  // $HOME/.vreng/cache
  sprintf(pathcache, "%s/cache", pathenvdir);
  if (stat(pathcache, &bufstat) < 0) {
    mkdir(pathcache, 0700);
  }
  strcpy(vrengcache, pathcache);

  // $HOME/.vreng/icons
  sprintf(pathicons, "%s/icons", pathenvdir);
  if (stat(pathicons, &bufstat) < 0) {
    mkdir(pathicons, 0700);
  }
  strcpy(vrengicons, pathicons);

  // $HOME/.vreng/menu
  sprintf(pathmenu, "%s/menu", pathenvdir);
  strcpy(vrengmenu, pathmenu);

  // $HOME/.vreng/worldmarks
  sprintf(pathworldmarks, "%s/worldmarks", pathenvdir);
  strcpy(vrengworldmarks, pathworldmarks);

  // $HOME/.vreng/vncpasswd
  sprintf(pathpasswd, "%s/vncpasswd", pathenvdir);
  strcpy(vrengpasswd, pathpasswd);

  chdir(vrengcwd);
}

void Env::listCache()
{
  char cmd[128];

  chdir(vrengcache);
  sprintf(cmd, "IFS=' '; /bin/ls -l");
  system(cmd);
}

void Env::cleanCacheByExt(const char *ext)
{
  chdir(vrengcache);
#if HAVE_READDIR
  DIR *dirp = opendir(".");
  if (dirp) {
    struct dirent *dp;
    struct stat bufstat;

    for (dp = readdir(dirp); dp; dp = readdir(dirp)) {
      if (stat(dp->d_name, &bufstat) == 0) {
        if (strstr(dp->d_name, ext)) {
          char *p = strrchr(dp->d_name, '.');
          p++;
          if (! strcmp(p, ext)) {
            unlink(dp->d_name);
          }
        }
      }
    }
    closedir(dirp);
  }
#else
  char cmd[128];
  sprintf(cmd, "IFS=' '; /bin/rm -f *.%s", ext);
  system(cmd);
#endif
  chdir(vrengcwd);
}

void Env::cleanCacheByTime(time_t cachetime)
{
  time_t now;

  time(&now);
  chdir(vrengcache);
#if HAVE_READDIR
  DIR *dirp = opendir(".");
  if (dirp) {
    struct dirent *dp;
    struct stat bufstat;

    for (dp = readdir(dirp); dp; dp = readdir(dirp)) {
      if (stat(dp->d_name, &bufstat) == 0) {
        if (S_ISREG(bufstat.st_mode)) {
          if (now - bufstat.st_mtime > cachetime) {
            unlink(dp->d_name);
          }
        }
      }
    }
    closedir(dirp);
  }
#else
  char cmd[128];
  sprintf(cmd, "IFS=' '; /bin/rm -f *");
  system(cmd);
#endif
  chdir(vrengcwd);
}
