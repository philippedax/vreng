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

class Cache {

 public:

  static int curl(const char *url, char *filename, const char opts[] = (char *)"cache");
  ///< Launchs curl on this url.

  static int download(const char *url, char *filename, const char opts[] = (char *)"cache");
  ///< Launchs curl or wget on this url.

  static int check(const char *url);
  ///< Checks url existence.

  static int setCacheName(const char *url, char *cachefile);
  ///< Changes url name to cache filename.

  static FILE * openCache(const char *url, Http *http);
  ///< Saves file into cache and opens it.

  static bool inCache(const char *url);
  ///< Checks weither url is cached.

  static void file(const char *url, char *filename);
  ///< Changes url name to filename.

  static void file2url(const char *filename, char *url);
  ///< Change filename to urlname.

  static void getCacheName(const char *url, std::string& cachename);
  ///< Gets absolute file name.

  static char * getFilePath(const char *url);
  ///< Gets relative file name.

};

#endif
