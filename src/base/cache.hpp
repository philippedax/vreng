//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2011 Philippe Dax
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
// cache.hpp
//---------------------------------------------------------------------------
#ifndef CACHE_HPP
#define CACHE_HPP


#include "file.hpp"	///< File
#include <string>


/**
 * Cache class
 */
class Cache : public File {
 private:
  File *filein;		///< filein instance
  File *fileout;	///< fileout instance

 public:
  Cache();
  virtual ~Cache();

  char * getFilename(void *tex);
  ///< Gets file name.

  static int setCachePath(const char *url, char *cachepath);
  ///< Sets cache path from url.

  FILE * open(const char *url, class Http *http);
  ///< Saves file into cache and opens it.

  void close();
  ///< Closes file into cache.

  static bool inCache(const char *url);
  ///< Checks whether url is cached.

  static void getCacheName(const char *url, std::string& cachename);
  ///< Fills absolute file name (string).

  static char * getFilePath(const char *url);
  ///< Gets relative file name.

  static int download(const char *url, char *filename, const char opts[] = (char *)"cache");
  ///< Launchs curl or wget on this url.

  static int check(const char *url);
  ///< Checks url existence.
};

#endif
