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
// env.hpp
//---------------------------------------------------------------------------
#ifndef ENV_HPP
#define ENV_HPP


/**
 * Env class
 */
class Env {
 public:
  Env();		///< constructor
  virtual ~Env();	///< destructor
  
  const char * home() const;
  const char * cwd() const;
  const char * dir() const;
  const char * prefs() const;
  const char * stats() const;
  const char * cache() const;
  const char * login() const;
  const char * passwd() const;
  const char * icons() const;
  const char * menu() const;
  const char * worldmarks() const;
  const char * sysname() const;
  const char * relname() const;
  const char * machname() const;
  
  void listCache();
  void fillCache();
  void cleanCacheByExt(const char *ext);
  void cleanCacheByTime(time_t cachetime);
  
 private:
  char *homedir;
  char *vrengcwd;
  char *vrengdir;
  char *logname;
  char *systemname;
  char *releasename;
  char *machinename;
  char *pathhtdocs;

  char vrengcache[PATH_LEN];
  char vrengprefs[PATH_LEN];
  char vrengstats[PATH_LEN];
  char vrengicons[PATH_LEN];
  char vrengworldmarks[PATH_LEN];
  char vrengpasswd[PATH_LEN];
  char vrengmenu[PATH_LEN];
  
  void init();
};


#endif
