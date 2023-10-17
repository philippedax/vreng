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
#include "url.hpp"
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
    trace(DBG_HTTP, "relative url: %s", url);
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
