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
#include "env.hpp"	// cache
#include "wget.hpp"	// start
#include "http.hpp"	// getChar
#include "pref.hpp"	// ::g.pref.refresh
#include "str.hpp"	// stringcmp


int Cache::setCacheName(const char *url, char *cachefile)
{
  if (! url) return 0;
  const char *pfile = strrchr(url, '/');

  if (pfile == NULL) {
    *cachefile = '\0';
    return 0;
  }
  sprintf(cachefile, "%s/%s", ::g.env.cache(), ++pfile);
  return 1;
}

void Cache::getFileName(const char *url, std::string& cachefile)
{
  static char temp[BUFSIZ];
  const char *pfile = strrchr(url, '/');

  sprintf(temp, "%s/%s", ::g.env.cache(), ++pfile);
  cachefile = temp;
}

char * Cache::getFileName(const char *url)
{
  static char filename[BUFSIZ];
  const char *pfile = strrchr(url, '/');

  sprintf(filename, "%s",  ++pfile);
  return filename;
}

FILE * Cache::openCache(const char *url, Http *http)
{
  char filename[PATH_LEN];
  if (! setCacheName(url, filename)) return NULL;
  //error("url=%s http=%p", url, http);

  FILE *f;
  if ((f = File::openFile(filename, "r")) == NULL) {
    if ((f = File::openFile(filename, "w")) == NULL) {
      error("httpReader: can't create %s", filename);
      return NULL;
    }

    if (! http) return NULL;

    int c;
    while ((c = http->getChar()) >= 0) putc(c, f);
    File::closeFile(f);
    progression('c');
    f = File::openFile(filename, "r");
  }
  if (::g.pref.refresh) {  // force reload
    int c;
    progression('h');
    while ((c = http->getChar()) >= 0) putc(c, f);
    File::closeFile(f);
    progression('c');
    f = File::openFile(filename, "r");
  }

  return f;  // file is opened
}

bool Cache::inCache(const char *url)
{
  char filename[PATH_LEN];
  if (! setCacheName(url, filename)) return false;

  FILE *f;
  if ((f = File::openFile(filename, "r")) != NULL) {
    File::closeFile(f);
    return true;
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

int Cache::curl(const char *url, char *filename, const char opts[])
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
      if (stringcmp(opts, "anon") == 0) {
        curl_easy_setopt(hcurl, CURLOPT_USERPWD, "ftp:vreng@");
      }
      if (stringcmp(opts, "inout") == 0) {
        curl_easy_setopt(hcurl, CURLOPT_READFUNCTION, NULL);
      }
    }
    else if (stringcmp(opts, "check") == 0) {
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

int Cache::download(const char *_url, char *filename, const char opts[])
{
  char url[URL_LEN];
  //unused bool cached = false;

  Url::abs(_url, url);
  
  if (filename && (! strcmp(opts, "cache"))) {        // use the cache
    //unused cached = true;
    if (! Cache::setCacheName(_url, filename)) return 0;
  
    struct stat bufstat;
    if (stat(filename, &bufstat) == 0) {
      trace(DBG_TOOL, "download: %s found in cache", filename);
      if (bufstat.st_size != 0) return 1;  // file yet in the cache
      unlink(filename); // remove empty file
    }
    trace(DBG_TOOL, "download: download %s in %s", url, filename);
  }

#if HAVE_CURL
  return curl(url, filename, opts);
#else
  return Wget::start(url, filename, opts);
#endif
}

int Cache::check(const char *url)
{
  return download(url, NULL, "check");
}
