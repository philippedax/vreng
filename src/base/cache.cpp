//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
// 
// Copyright (C) 1997-2023 Philippe Dax
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
// cache.cpp
//
// Cache for http
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "cache.hpp"
#include "file.hpp"	// open, close
#include "url.hpp"	// abs
#include "http.hpp"	// readHttp
#include "env.hpp"	// ::g.env.cache
#include "pref.hpp"	// ::g.pref.refresh
#include "str.hpp"	// stringcmp
#include "texture.hpp"	// Texture
#include "wget.hpp"	// start
#include "stat.hpp"	// new_cache, del_cache

#include <string>


/** Constructor */
Cache::Cache()
{
  new_cache++;
  filein = NULL;
  fileout = NULL;
}

/** Destructor */
Cache::~Cache()
{
  del_cache++;
}

/** Fills cachepath from url */
int Cache::setCachePath(const char *url, char *cachepath)
{
  if (! url) return 0;
  const char *pfile = strrchr(url, '/');	// only the file

  if (! pfile) {
    *cachepath = '\0';
    return 0;
  }
  sprintf(cachepath, "%s/%s", ::g.env.cache(), ++pfile);
  return 1;
}

/** Fills cache name string */
void Cache::getCacheName(const char *url, std::string& cachestring)
{
  static char cachepath[PATH_LEN];
  const char *pfile = strrchr(url, '/');

  sprintf(cachepath, "%s/%s", ::g.env.cache(), ++pfile);
  cachestring = cachepath;
}

/** Gets file path */
char * Cache::getFilePath(const char *url)
{
  static char filepath[PATH_LEN];
  const char *pfile = strrchr(url, '/');

  sprintf(filepath, "%s", ++pfile);
  return filepath;
}

/** Opens an url and writes it into the cache and returns the FILE opened */
FILE * Cache::open(const char *url, Http *http)
{
  if (! http) return NULL;

  char *cachepath = new char[PATH_LEN];
  memset(cachepath, 0, PATH_LEN);
  if (! setCachePath(url, cachepath)) return NULL;

  FILE *fpr = NULL;
  FILE *fpw = NULL;
  char buf[16];

  filein = new File();
  if (! (fpr = filein->open(cachepath, "r"))) {
    fileout = new File();	// not in the cache, write it
    if (! (fpw = fileout->open(cachepath, "w"))) {
      error("openCache: can't create %s", cachepath);
      delete[] cachepath;
      delete fileout;
      return NULL;
    }

    // Writes the file into the cache
http_reread:
    //
    // Checks if the head of the downloaded file is valid
    //
    http->readHttp(buf, 14);	// read head to check if correct
    if (strncmp(buf, "<!DOCTYPE HTML", 14) == 0) {	// hack!!!
      // Httpd-err occured (404)
      fileout->close();
      delete fileout;
      unlink(cachepath);
      progression('-');	// '-' as failed
      return NULL;
    }
    fwrite(buf, 14, 1, fpw);

    //
    // Reads and writes the remaining
    //
    while (http->readHttp(buf, 1)) {
      fwrite(buf, 1, 1, fpw);
    }
    fflush(fpw);
    fileout->close();
    delete fileout;

    //
    // Reopens the file for reading
    //
    filein = new File();
    if (! (fpr = filein->open(cachepath, "r"))) {
      error("openCache: can't reopen %s", cachepath);
      return NULL;
    }

    //
    // Verifies the integrity of the new created file
    //
    struct stat bufstat;
    if (stat(cachepath, &bufstat) == 14) {
      if (bufstat.st_size == 0) {
        error("openCache: %s is empty", cachepath);
        unlink(cachepath);
        progression('-');	// '-' as failed
        goto http_reread;	// re-do read
      }
      else {
        progression('h');	// 'h' as http
      }
    }
  }
  else {	// found in the cache
    progression('c');		// 'c' as cache
  }

  // ready for reading as a file
  delete[] cachepath;
  return fpr;			// file is now opened
}

/** Closes cache */
void Cache::close()
{
  if (filein) {
    filein->close();
    delete filein;
  }
}

/** Checks if file is in the cache */
bool Cache::inCache(const char *url)
{
  char *cachepath = new char[PATH_LEN];
  *cachepath = 0;

  if (! setCachePath(url, cachepath)) {
    delete[] cachepath;
    return false;
  }

  struct stat bufstat;
  if (stat(cachepath, &bufstat) == 0) {
    if (bufstat.st_size != 0) {
      delete[] cachepath;
      return true;	// file is in the cache
    }
    else {	// file is empty
      unlink(cachepath); // remove empty file
    }
  }
  delete[] cachepath;
  return false;
}

/** Gets filename from an url */
char * Cache::getFilename(void *_tex)
{
  static char filepath[PATH_LEN] = {0};
  Texture *tex = static_cast<Texture *>(_tex);

  setCachePath(tex->url, filepath);
  return filepath;
}

/** Downloads in the cache a file from an url */
int Cache::download(const char *_url, char *filename, const char arg[])
{
  char url[URL_LEN];

  Url::abs(_url, url);
  
  if (filename && (! strcmp(arg, "cache"))) {        // use the cache
    if (! Cache::setCachePath(url, filename)) {
      return 0;
    }

    struct stat bufstat;
    if (stat(filename, &bufstat) == 0) {
      if (bufstat.st_size != 0) {
        return 1;	// file yet in the cache
      }
      unlink(filename); // remove empty file
    }
    trace1(DBG_TOOL, "download: download %s in %s", url, filename);
  }
#if HAVE_CURL
  return Url::curl(url, filename, arg);
#else
  return Wget::start(url, filename, arg);
#endif
}

/** Checks existence of a file */
int Cache::check(const char *url)
{
  return download(url, NULL, "check");
}
