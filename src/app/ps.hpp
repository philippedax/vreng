//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
// 
// Copyright (C) 1997-2012 Philippe Dax
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
#ifndef PS_HPP
#define PS_HPP


/**
 * Ps class
 */
class Ps {
 public:

  static void init(int toolid);
  ///< Init PS viewver

  static void view(const char *url);
  ///< Start PS viewver

  static void print(const char *url);
  ///< Launch PS printer

  static void quit();
  ///< Quit PS viewver

 private:

  static int toolid;

  static void launch(const char *tool, const char *file);
  static void launcha2ps(const char *tool, const char *file);

};


#endif
