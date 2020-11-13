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
#include "universe.hpp"	// Universe
#include "str.hpp"	// stringcmp


/* URL parsing
 *
 * Parses url
 * Returns: host, scheme, path
 */
int Url::parser(char *url, char *host, char *scheme, char *path)
{
  int urltype;
  char *p;
  static char prev_host[MAXHOSTNAMELEN] = "";	// first one is undefined

  /* parse scheme */
  if (! url || ! strlen(url)) {
    *scheme = '\0';
    trace(DBG_HTTP, "BAD URL: %s", url);
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
  else {	// incomplete URL
    trace(DBG_HTTP, "relative URL: %s", url);
    strcpy(scheme, "http");
    strcpy(host, Universe::current()->server);
    if (*url != '/') {
      // relative url
      if (! isprint(*url)) {
        error("Bad Url: %02x%02x%02x host=%s", url[0], url[1], url[2], host);
        return URLBAD;
      }
      sprintf(path, "%s/%s", Universe::current()->urlpfx, url);
    }
    else {	// /path
      if (! stringcmp(url, "http://") || ! stringcmp(url, "/~")) {
        // full qualified url
        sprintf(path, "%s", url);
      }
      else {
        // add url prefix before url
        sprintf(path, "%s%s", Universe::current()->urlpfx, url);
      }
    }
    return URLHTTP;
  }

  /* then parse host's name */
  if (urltype != URLFILE) {
    p = prev_host;
    while ((*url != ':') && (*url != '/') && (*url != '\0')) {
      *host++ = *url;
      *p++ = *url;	// keep current host
      url++;
    }
    *host = '\0';
    *p = '\0';
  }

  /* then parse pathname */
  if (*url == '\0')
    strcpy(path, "/");
  else // file
    strcpy(path, url);
  return urltype;
}

void Url::abs(const char *oldurl, char *newurl)
{
  if ((! stringcmp(oldurl, "http://")) || (! stringcmp(oldurl, "ftp://")))
    strncpy(newurl, oldurl, URL_LEN);
  else
    sprintf(newurl, "http://%s%s%s", Universe::current()->server, Universe::current()->urlpfx, oldurl);
  if (strlen(newurl) >= URL_LEN) {
    newurl[URL_LEN -1] = '\0';
    warning("url length too big %s", newurl);
  }
}
