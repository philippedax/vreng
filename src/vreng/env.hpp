//---------------------------------------------------------------------------
// VREng ( Reality Engine)	http://vreng.enst.fr/
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
#ifndef ENV_HPP
#define ENV_HPP


/**
 * Env class
 */
class Env {
public:
  Env();		///< constructor
  
  char * home();
  char * cwd();
  char * dir();
  char * prefs();
  char * cache();
  char * passwd();
  char * icons();
  char * menu();
  char * worldmarks();
  char * sysname();
  char * relname();
  char * machname();
  
  void listCache();
  void cleanCacheByExt(const char *ext);
  void cleanCacheByTime(time_t cachetime);
  
private:
  char homedir[PATH_LEN];
  char vrengdir[PATH_LEN];
  char vrengcwd[PATH_LEN];
  char vrengprefs[PATH_LEN];
  char vrengmenu[PATH_LEN];
  char vrengicons[PATH_LEN];
  char vrengworldmarks[PATH_LEN];
  char vrengpasswd[PATH_LEN];
  char vrengcache[PATH_LEN];
  
  char systemname[9];
  char releasename[9];
  char machinename[32];
  
  void sysinfo();
  void init();
};


#endif
