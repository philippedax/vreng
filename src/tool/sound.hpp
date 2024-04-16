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
// sound.hpp
//---------------------------------------------------------------------------
#ifndef SOUND_HPP
#define SOUND_HPP

#include "sounds.hpp"


/**
 * Sound class
 */
class Sound {

public:
  Sound() {}
  ~Sound() {}

  static void playSound(const char *snd);
  ///< Play this encrypted base64 sound

private:
  static int opendev();
  ///< Open audio device

  static void * sound_thread(void *snd);
  ///< Thread
};


#endif
