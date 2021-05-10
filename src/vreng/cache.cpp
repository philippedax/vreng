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


int Cache::setCacheName(const char *url, char *cachepath)
{
  if (! url) return 0;
  const char *pfile = strrchr(url, '/');

  if (pfile == NULL) {
    *cachepath = '\0';
    return 0;
  }
  sprintf(cachepath, "%s/%s", ::g.env.cache(), ++pfile);
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

  sprintf(filepath, "%s",  ++pfile);
  return filepath;
}

FILE * Cache::openCache(const char *url, Http *http)
{
  char filepath[PATH_LEN];
  if (! setCacheName(url, filepath)) return NULL;

  FILE *fp;
  if ((fp = File::openFile(filepath, "r")) == NULL) {
    if ((fp = File::openFile(filepath, "w")) == NULL) {
      error("httpReader: can't create %s", filepath);
      return NULL;
    }

    if (! http) return NULL;

    // writing the file into the cache
    int c;
    while ((c = http->getChar()) >= 0) {
      putc(c, fp);
    }
    File::closeFile(fp);
    progression('c');	// c as cache
  }
  if (::g.pref.refresh) {  // force reload
    int c;
    progression('h');	// h as http
    while ((c = http->getChar()) >= 0) {
      putc(c, fp);
    }
    File::closeFile(fp);
    progression('c');
  }
  // and open it for reading
  fp = File::openFile(filepath, "r");

  return fp;  // file is opened
}

bool Cache::inCache(const char *url)
{
  char filepath[PATH_LEN];
  if (! setCacheName(url, filepath)) return false;

  FILE *fp;
  struct stat bufstat;
  if ((fp = File::openFile(filepath, "r")) != NULL) {
    File::closeFile(fp);
    stat(filepath, &bufstat);
    if (bufstat.st_size != 0)
      return true;
    else
      error("file %s empty", filepath);
  }
  return false;
}

void Cache::file(const char *_url, char *filename)
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
  FILE *fp = NULL;
  size_t size;

  hcurl = curl_easy_init();
  if (hcurl) {
    curl_easy_setopt(hcurl, CURLOPT_URL, url);
    curl_easy_setopt(hcurl, CURLOPT_USERAGENT, PACKAGE_NAME "/" PACKAGE_VERSION);
    curl_easy_setopt(hcurl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(hcurl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(hcurl, CURLOPT_VERBOSE, 1);
    if (filename) {
      if ((fp = File::openFile(filename, "wb")) == NULL) {
        perror("open wb");
        return 0;
      }
      curl_easy_setopt(hcurl, CURLOPT_VERBOSE, 0);
      curl_easy_setopt(hcurl, CURLOPT_WRITEFUNCTION, NULL);
      curl_easy_setopt(hcurl, CURLOPT_WRITEDATA, fp);
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
    if (fp) File::closeFile(fp);
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
    if (! Cache::setCacheName(_url, filename)) return 0;
  
    struct stat bufstat;
    if (stat(filename, &bufstat) == 0) {
      trace(DBG_TOOL, "download: %s found in cache", filename);
      if (bufstat.st_size != 0)
        return 1;  // file yet in the cache
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
