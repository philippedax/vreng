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
#include "vreng.hpp"
#include "openal.hpp"


// class member initialization
Openal * Openal::openal = NULL;


Openal::Openal()
{
  openal = NULL;
}

Openal * Openal::current()
{
  return openal;
}

void Openal::quit()
{
  openal = NULL;
#if HAVE_OPENAL
  ALCcontext *context = alcGetCurrentContext();
  ALCdevice *device  = alcGetContextsDevice(context);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
#endif
}

bool Openal::init()
{
#if HAVE_OPENAL
  // Ouverture du device
  ALCdevice *device = alcOpenDevice(NULL);
  if (!device) return false;
  // Création du contexte
  ALCcontext *context = alcCreateContext(device, NULL);
  if (!context) return false;
  // Activation du contexte
  if (!alcMakeContextCurrent(context)) return false;

  return true;
#else
  return false;
#endif
}

void Openal::getDevices(std::vector<std::string>& devices)
{
#if HAVE_OPENAL
  devices.clear();
  const ALchar* deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
  if (deviceList) {
    while (strlen(deviceList) > 0) {
      devices.push_back(deviceList);
      deviceList += strlen(deviceList) + 1;
    }
  }
#endif
}

#if HAVE_OPENAL
ALuint Openal::load(const char *filename)
{
  SF_INFO fileinfos;
  SNDFILE *file = sf_open(filename, SFM_READ, &fileinfos);
  if (!file) return 0;

  ALsizei nbsamples  = static_cast<ALsizei>(fileinfos.channels *fileinfos.frames);
  ALsizei samplerate = static_cast<ALsizei>(fileinfos.samplerate);
  std::vector<ALshort> samples(nbsamples);
  if (sf_read_short(file, &samples[0], nbsamples) < nbsamples)
    return 0;
  sf_close(file);

  ALenum format;
  switch (fileinfos.channels) {
  case 1 : format = AL_FORMAT_MONO16; break;
  case 2 : format = AL_FORMAT_STEREO16; break;
  default : return 0;
  }

  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, &samples[0], nbsamples * sizeof(ALushort), samplerate);
  if (alGetError() != AL_NO_ERROR)
    return 0;
  return buffer;
}
#endif

