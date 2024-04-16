//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// openal.hpp
//---------------------------------------------------------------------------
#ifndef OPENAL_HPP
#define OPENAL_HPP


#if HAVE_OPENAL 
#include <AL/al.h>
#include <AL/alc.h>
#endif
#if HAVE_SNDFILE
#include <sndfile.h>
#endif
#include <string>
#include <vector>
#include <list>

/**
 * Openal class
 */
class Openal {
private:
  static Openal *openal;	///< The singelton instance of this class

public:
#if HAVE_OPENAL 
  ALCdevice *device;
  ALCcontext *context;

  ALuint load(const char * filename);
#endif

  Openal();		///< Constructor from world

  static Openal * current();
  /**< Returns the current openal */

  static bool init();	///< Init
  static void getDevices(std::vector<std::string>& devices);
  static void quit();	///< Quits
};

#endif
