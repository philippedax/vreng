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
#ifndef URL_HPP
#define URL_HPP


/**
 * Url class
 */
class Url {

 public:

  enum {
    URLBAD,
    URLHTTP,
    URLFILE
  };

  static int parser(char *url, char *host, char *scheme, char *path);
  ///< Parse url.

  static void abs(const char *oldurl, char *newurl);
  ///< Change relative url to absolute.

  static bool check(const char *url);
  ///< Check url integity */

  static void url2file(const char *url, char *filename);
  ///< Changes url name to filename.

  static void file2url(const char *filename, char *url);
  ///< Change filename to urlname.

  static int curl(const char *url, char *filename, const char opts[] = (char *)"cache");
  ///< Launchs curl on this url.
};

#endif
