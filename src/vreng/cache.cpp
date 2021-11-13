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
#include <string>
#include "cache.hpp"
#include "file.hpp"	// openFile, closeFile
#include "url.hpp"	// abs
#include "http.hpp"	// getChar
#include "env.hpp"	// ::g.env.cache
#include "pref.hpp"	// ::g.pref.refresh
#include "str.hpp"	// stringcmp
#include "wget.hpp"	// start


/* Fills cachepath from url */
int Cache::setCachePath(const char *url, char *cachepath)
{
  if (! url) return 0;
  const char *pfile = strrchr(url, '/');

  if (pfile == NULL) {
    *cachepath = '\0';
    return 0;
  }
  sprintf(cachepath, "%s/%s", ::g.env.cache(), ++pfile);
  //error("cachepath: %s", cachepath);
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

/* Opens an url and writes it into the cache and returns le file opened (fp) */
FILE * Cache::openCache(const char *url, Http *http)
{
  char cachepath[PATH_LEN] = {0};

  if (! http) return NULL;
  if (! setCachePath(url, cachepath)) return NULL;

  FILE *fpcache;
  if ((fpcache = File::openFile(cachepath, "r")) == NULL) {
    if ((fpcache = File::openFile(cachepath, "w")) == NULL) {
      error("openCache: can't create %s", cachepath);
      return NULL;
    }

    // writes the file into the cache
    int c;
    while ((c = http->getChar()) >= 0) {
      putc(c, fpcache);
    }
    File::closeFile(fpcache);
    struct stat bufstat;
    if (stat(cachepath, &bufstat) == 0) {
      if (bufstat.st_size == 0) {
        //error("openCache: %s is empty", cachepath);
        unlink(cachepath);
      }
      else {
        progression('h');	// 'h' as http
      }
    }
  }
  // and opens it for reading
  fpcache = File::openFile(cachepath, "r");

  return fpcache;  // file is opened
}

/* Checks if file is in the cache */
bool Cache::inCache(const char *url)
{
  char cachepath[PATH_LEN] = {0};

  if (! setCachePath(url, cachepath)) {
    return false;
  }

  struct stat bufstat;
  if (stat(cachepath, &bufstat) == 0) {
    if (bufstat.st_size != 0) {
      return true;	// file is in the cache
    }
    else {
      //error("inCache: file %s empty", cachepath);
      unlink(cachepath); // remove empty file
    }
  }
  return false;
}

/* (specific icon) */
void Cache::url2file(const char *_url, char *filename)
{
  int i = 0;
  char *url = strdup(_url);

  for (char *p = url; *p ; p++, i++) {
    if (*p == '/') *p = '%';
    filename[i] = *p;
  }
  filename[i] = '\0';
  free(url);
}

void Cache::file2url(const char *filename, char *url)
{
  int i = 0;
  char *fname = strdup(filename);

  for (char *p = fname; *p ; p++, i++) {
    if (*p == '%')
      *p = '/';
    url[i] = *p;
  }
  url[i] = '\0';
  free(fname);
}

int Cache::curl(const char *url, char *filename, const char arg[])
{
#if HAVE_LIBCURL
  CURL *hcurl;
  CURLcode res;
  FILE *fpcache = NULL;
  size_t size;

  hcurl = curl_easy_init();
  if (hcurl) {
    curl_easy_setopt(hcurl, CURLOPT_URL, url);
    curl_easy_setopt(hcurl, CURLOPT_USERAGENT, PACKAGE_NAME "/" PACKAGE_VERSION);
    curl_easy_setopt(hcurl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(hcurl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(hcurl, CURLOPT_VERBOSE, 1);
    if (filename) {
      if ((fpcache = File::openFile(filename, "wb")) == NULL) {
        perror("open wb");
        return 0;
      }
      curl_easy_setopt(hcurl, CURLOPT_VERBOSE, 0);
      curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, NULL);
      curl_easy_setopt(hcurl, CURLOPT_WRITEDATA, fpcache);
      if (stringcmp(arg, "anon") == 0) {
        curl_easy_setopt(hcurl, CURLOPT_USERPWD, "ftp:vreng@");
      }
      if (stringcmp(arg, "inout") == 0) {
        curl_easy_setopt(hcurl, CURLOPT_READFUNCTION, NULL);
      }
    }
    else if (stringcmp(arg, "check") == 0) {
      curl_easy_setopt(hcurl, CURLOPT_VERBOSE, 0);
      curl_easy_setopt(hcurl, CURLOPT_NOBODY, 1);
    }
    res = curl_easy_perform(hcurl);
    if (res) {
      error("curl: %s", curl_easy_strerror(res));
    }
    else {
      curl_easy_getinfo(hcurl, CURLINFO_SIZE_DOWNLOAD, &size);
    }
    if (fpcache) File::closeFile(fpcache);
    curl_easy_cleanup(hcurl);
    return 1;
  }
  else {
    error("curl: can't init");
    return 0;
  }
#else
  return 0;
#endif
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
  return curl(url, filename, arg);
#else
  return Wget::start(url, filename, arg);
#endif
}

int Cache::check(const char *url)
{
  return download(url, NULL, "check");
}
