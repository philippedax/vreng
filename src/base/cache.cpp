//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
// 
// Copyright (C) 1997-2023 Philippe Dax
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
#include "cache.hpp"
#include "file.hpp"	// open, close
#include "url.hpp"	// abs
#include "http.hpp"	// read_buf
#include "env.hpp"	// ::g.env.cache
#include "pref.hpp"	// ::g.pref.refresh
#include "str.hpp"	// stringcmp
#include "texture.hpp"	// Texture
#include "wget.hpp"	// start
#include "stat.hpp"	// new_cache, del_cache

#include <string>


/* Constructor */
Cache::Cache()
{
  new_cache++;
  filein = NULL;
  fileout = NULL;
}

/* Destructor */
Cache::~Cache()
{
  del_cache++;
}

/* Fills cachepath from url */
int Cache::setCachePath(const char *url, char *cachepath)
{
  if (! url) return 0;
  const char *pfile = strrchr(url, '/');	// only the file

  if (pfile == NULL) {
    *cachepath = '\0';
    return 0;
  }
  sprintf(cachepath, "%s/%s", ::g.env.cache(), ++pfile);
  //echo("cachepath: %s", cachepath);
  return 1;
}

void Cache::getCacheName(const char *url, std::string& cachefile)
{
  static char cachepath[PATH_LEN];
  const char *pfile = strrchr(url, '/');

  sprintf(cachepath, "%s/%s", ::g.env.cache(), ++pfile);
  cachefile = cachepath;
}

char * Cache::getFilePath(const char *url)
{
  static char filepath[PATH_LEN];
  const char *pfile = strrchr(url, '/');

  sprintf(filepath, "%s", ++pfile);
  return filepath;
}

/* Opens an url and writes it into the cache and returns the file opened */
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
  if ((fpr = filein->open(cachepath, "r")) == NULL) {
    fileout = new File();	// not in the cache, write it
    if ((fpw = fileout->open(cachepath, "w")) == NULL) {
      error("openCache: can't create %s", cachepath);
      delete[] cachepath;
      delete fileout;
      return NULL;
    }

    // writes the file into the cache
http_reread:
    //
    // before check if the head of downloading file is valid
    //
    http->read_buf(buf, 14);	// read head to check if correct
    if (strncmp(buf, "<!DOCTYPE HTML", 14) == 0) {
      // Httpd-err occured (404)
      fileout->close();
      delete fileout;
      unlink(cachepath);
      progression('-');	// '-' as failed
      return NULL;
    }
    fwrite(buf, 14, 1, fpw);

    //
    // reads and writes the remaining
    //
    while (! http->heof()) {
      http->read_buf(buf, 1);
      fwrite(buf, 1, 1, fpw);
    }
    fflush(fpw);
    fileout->close();
    delete fileout;

    //
    // reopens the file for reading
    //
    filein = new File();
    if ((fpr = filein->open(cachepath, "r")) == NULL) {
      error("openCache: can't open %s", cachepath);
      return NULL;
    }

    //
    // verify the integrity of the new file created
    //
    struct stat bufstat;
    if (stat(cachepath, &bufstat) == 0) {
      if (bufstat.st_size == 0) {
        error("openCache: %s is empty", cachepath);
        unlink(cachepath);
        progression('-');	// '-' as failed
        goto http_reread;
      }
      else {
        progression('h');	// 'h' as http
      }
    }
  }
  else {	// found in the cache
    progression('c');	// 'c' as cache
  }

  // ready for reading
  delete[] cachepath;
  return fpr;  // file is now opened
}

/* Closes cache */
void Cache::close()
{
  if (filein) {
    filein->close();
    delete filein;
  }
}

#if 0 //notused
/* Opens an url and writes it into the cache and returns the file opened */
// static
FILE * Cache::openCache(const char *url, Http *http)
{
  if (! http) return NULL;

  char *cachepath = new char[PATH_LEN];
  memset(cachepath, 0, PATH_LEN);
  if (! setCachePath(url, cachepath)) return NULL;

  FILE *fp = NULL;
  if ((fp = File::openFile(cachepath, "r")) == NULL) {	// not in the cache, write it
    if ((fp = File::openFile(cachepath, "w")) == NULL) {
      error("openCache: can't create %s", cachepath);
      delete[] cachepath;
      return NULL;
    }

    // writes the file into the cache FIXED!!!
    char buf[4];
    while (! http->heof()) {
      http->read_buf(buf, 4);
      fwrite(buf, 4, 1, fp);
    }
    fflush(fp);
    File::closeFile(fp);

    // open the file for reading
    if ((fp = File::openFile(cachepath, "r")) == NULL) {
      error("openCache: can't open %s", cachepath);
      return NULL;
    }

    // verify
    struct stat bufstat;
    if (stat(cachepath, &bufstat) == 0) {
      if (bufstat.st_size == 0) {
        //error("openCache: %s is empty", cachepath);
        unlink(cachepath);
        progression('-');	// '-' as failed
      }
      else {
        progression('h');	// 'h' as http
      }
    }
  }
  else {	// found in the cache
    progression('c');	// 'c' as cache
  }

  // ready for reading
  delete[] cachepath;
  return fp;  // file is now opened
}

/* Closes cache - static */
void Cache::closeCache(FILE *fp)
{
  if (fp) {
    File::closeFile(fp);
  }
}
#endif //notused

/* Checks if file is in the cache */
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

char * Cache::getFilename(void *_tex)
{
  static char filepath[PATH_LEN] = {0};
  Texture *tex = (Texture *) _tex;

  setCachePath(tex->url, filepath);
  return filepath;
}

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
    trace(DBG_TOOL, "download: download %s in %s", url, filename);
  }
#if HAVE_CURL
  return Url::curl(url, filename, arg);
#else
  return Wget::start(url, filename, arg);
#endif
}

int Cache::check(const char *url)
{
  return download(url, NULL, "check");
}
