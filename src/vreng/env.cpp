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
#ifdef DMALLOC_FUNC_CHECK
  //dmalloc_verify(0L);
#endif
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
  initDirs();
}

char * Env::cwd()
{
  return vrengcwd;
}

char * Env::dir()
{
  return vrengdir;
}

char * Env::cache()
{
  return vrengcache;
}

char * Env::prefs()
{
  return vrengprefs;
}

char * Env::icons()
{
  return vrengicons;
}

char * Env::menu()
{
  return vrengmenu;
}

char * Env::worldmarks()
{
  return vrengworldmarks;
}

char * Env::passwd()
{
  return vrengpasswd;
}

char * Env::sysname()
{
  return systemname;
}

char * Env::relname()
{
  return releasename;
}

char * Env::machname()
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
  for (uint32_t i=0; i < sizeof(machinename); i++) {
    if (machinename[i] == ' ')
      machinename[i] = '_';
  }
#endif
}

void Env::initDirs()
{
  char *home;
  char pathenvdir[PATH_LEN+8], pathprefs[PATH_LEN+16], pathmenu[PATH_LEN+16], pathicons[PATH_LEN+16], pathworldmarks[PATH_LEN+32], pathcache[PATH_LEN+32], pathpasswd[PATH_LEN+32];
  struct stat bufstat;

  if ((home = getenv("HOME")) == NULL)
    return;
  if (getcwd(vrengcwd, sizeof(vrengcwd)) == NULL)
    return;

  sprintf(pathenvdir, "%s/.vreng", home);
  if (stat(pathenvdir, &bufstat) < 0)
    mkdir(pathenvdir, 0700);
  strcpy(vrengdir, pathenvdir);
  chdir(pathenvdir);

  sprintf(pathicons, "%s/icons", pathenvdir);
  if (stat(pathicons, &bufstat) < 0)
    mkdir(pathicons, 0700);
  strcpy(vrengicons, pathicons);

  sprintf(pathprefs, "%s/prefs", pathenvdir);
  strcpy(vrengprefs, pathprefs);

  sprintf(pathmenu, "%s/menu", pathenvdir);
  strcpy(vrengmenu, pathmenu);

  sprintf(pathworldmarks, "%s/worldmarks", pathenvdir);
  strcpy(vrengworldmarks, pathworldmarks);

  sprintf(pathpasswd, "%s/vncpasswd", pathenvdir);
  strcpy(vrengpasswd, pathpasswd);

  sprintf(pathcache, "%s/cache", pathenvdir);
  if (stat(pathcache, &bufstat) < 0)
    mkdir(pathcache, 0700);
  strcpy(vrengcache, pathcache);
  chdir(vrengcwd);
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
