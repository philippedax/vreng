//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
// 
// Copyright (C) 1997-2012 Philippe Dax
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
// mp3.hpp
//---------------------------------------------------------------------------
#ifndef MP3_HPP
#define MP3_HPP


/**
 * Mp3 class
 */
class Mp3 {
 public:
  static void init(int toolid);
  ///< Init MP3 tool

  static void start(const char *url);
  ///< Start MP3 tool

  static void stop();
  ///< Stop MP3 tool

  static void pause();
  ///< Suspend MP3 tool

  static void cont();
  ///< Resume MP3 tool

 private:
  static int toolid;

  static void launch(const char *tool, const char *url);
  ///< Launch MP3 tool
};


#endif
