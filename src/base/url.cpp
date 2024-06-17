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
// url.cpp
//
// URL operations
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "url.hpp"
#include "file.hpp"	// open, close
#include "str.hpp"	// stringcmp


/* URL parsing
 *
 * Parses url
 * Returns: host, scheme, path
 */
int Url::parser(char *url, char *host, char *scheme, char *path)
{
  int urltype;

  /* parse scheme */
  if (! url || ! strlen(url)) {
    *scheme = '\0';
    error("bad url: %s", url);
    return URLBAD;
  }
  if (! stringcmp(url, "http://")) {
    urltype = URLHTTP;
    strcpy(scheme, "http");
    url += 7;
  } else if (! stringcmp(url, "file:")) {
    urltype = URLFILE;
    strcpy(scheme, "file");
    url += 5;
  }
  else {	// incomplete url
    trace1(DBG_HTTP, "relative url: %s", url);
    strcpy(scheme, "http");
    strcpy(host, ::g.server);
    if (*url != '/') {
      // relative url
      if (! isprint(*url)) {
        error("bad url: %02x%02x%02x host=%s", url[0], url[1], url[2], host);
        return URLBAD;
      }
      sprintf(path, "%s%s", ::g.urlpfx, url);
      //echo("rel path: %s", path);
    }
    else {	// /path
      if (! stringcmp(url, "http://") || ! stringcmp(url, "/~")) {
        // full qualified url
        sprintf(path, "%s", url);
        //echo("abs path: %s", path);
      }
      else {
        // add url prefix before url
        sprintf(path, "/%s%s", ::g.urlpfx, url);
        //echo("pfx path: %s", path);
      }
    }
    return URLHTTP;
  }

  /* then parse host's name */
  if (urltype != URLFILE) {
    while ((*url != ':') && (*url != '/') && (*url != '\0')) {
      *host++ = *url;
      url++;
    }
    *host = '\0';
  }

  /* then parse pathname */
  if (*url == '\0')
    strcpy(path, "/");
  else // file
    strcpy(path, url);

  return urltype;
}

void Url::abs(const char *relurl, char *absurl)
{
  if ((! stringcmp(relurl, "http://")) || (! stringcmp(relurl, "ftp://"))) {
    strncpy(absurl, relurl, URL_LEN);
  }
  else {
    sprintf(absurl, "http://%s/%s%s", ::g.server, ::g.urlpfx, relurl);
  }
  if (strlen(absurl) >= URL_LEN) {
    absurl[URL_LEN -1] = '\0';
    error("url length too big %s", absurl);
  }
  //echo("absurl: %s", absurl);
}

bool Url::check(const char *url)
{   
  if (! url) {      
    error("url NULL");
    return false;
  } 
  if (! isprint(*url)) {
    error("url not printable");
    for (int i=0; i<16; i++) {
      fprintf(stderr, "%02x ", url[i]);
    }
    return false;
  }
  return true;
}


/* (specific icon) */
void Url::url2file(const char *_url, char *filename)
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

void Url::file2url(const char *filename, char *url)
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

/** Gets a file by an url */
int Url::curl(const char *url, char *filename, const char arg[])
{
#if HAVE_LIBCURL
  CURL *hcurl;
  CURLcode res;
  File *file = NULL;
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
      if ((fp = file->open(filename, "wb")) == NULL) {
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
    if (fp) {
      file->close();
      delete file;
    }
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
