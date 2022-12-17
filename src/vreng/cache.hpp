//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#ifndef CACHE_HPP
#define CACHE_HPP

#include <string>

/**
 * Cache class
 */

class Http;
class File;

class Cache {

 private:
  File *file;	///< file instance

 public:
  Cache() { file = NULL; };
  virtual ~Cache() {};

  static int setCachePath(const char *url, char *cachepath);
  ///< Sets cache path from url.

  FILE * open(const char *url, Http *http);
  ///< Saves file into cache and opens it.

  static FILE * openCache(const char *url, Http *http);
  ///< Saves file into cache and opens it.

  void close();
  ///< Closes file into cache.

  static void closeCache(FILE *fp);
  ///< Closes file into cache.

  static bool inCache(const char *url);
  ///< Checks whether url is cached.

  static void url2file(const char *url, char *filename);
  ///< Changes url name to filename.

  static void file2url(const char *filename, char *url);
  ///< Change filename to urlname.

  static void getCacheName(const char *url, std::string& cachename);
  ///< Gets absolute file name.

  static char * getFilePath(const char *url);
  ///< Gets relative file name.

  static int curl(const char *url, char *filename, const char opts[] = (char *)"cache");
  ///< Launchs curl on this url.

  static int download(const char *url, char *filename, const char opts[] = (char *)"cache");
  ///< Launchs curl or wget on this url.

  static int check(const char *url);
  ///< Checks url existence.

};

#endif
