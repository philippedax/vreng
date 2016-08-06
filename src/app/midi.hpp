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
#ifndef MIDI_HPP
#define MIDI_HPP


/**
 * Midi class
 */
class Midi {
 public:

  static void init(int toolid);
  ///< Init Midi tool

  static void start(const char *url);
  ///< Start Midi tool

  static void stop();
  ///< Stop Midi tool

  static void pause();
  ///< Suspend Midi tool

  static void cont();
  ///< Resume Midi tool

 private:

  static int toolid;

  static void launch(const char *tool, const char *url);
  ///< Launch Midi tool

};


#endif
