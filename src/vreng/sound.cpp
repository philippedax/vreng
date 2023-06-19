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
#include "sound.hpp"
#include "pref.hpp"	// ::g.pref.silent
#include "audio.hpp"	// audioactive
#include "base64.hpp"	// decodeBase64


#define DEVAUDIO	"/dev/audio"
#define DEVAUDIOCTL	"/dev/audioctl"


/* Open audio device */
int Sound::opendev()
{
  int fd = -1;

#if !defined(MACOSX)
  if ((fd = open(DEVAUDIO, 1)) < 0)
    error("can't open /dev/audio");
#endif
  return fd;
}

/* Thread which plays a sound */
void * Sound::sound_thread(void *snd)
{
  int fdaudio;
  if ((fdaudio = opendev()) < 0) return NULL;

  char *bufaudio = strdup((const char *) snd);
  int sizeaudio = decodeBase64((const char *) snd, bufaudio);

  audioactive = true;
  write(fdaudio, bufaudio, sizeaudio);
  close(fdaudio);
  audioactive = false;
  free(bufaudio);
  kill(getpid(), SIGUSR2);
#if HAVE_LIBPTHREAD
  pthread_exit(NULL);
#endif
  return NULL;
}

/* Method which plays a sound */
void Sound::playSound(const char *snd)
{
  //echo("play: busy=%d", audioactive);
  if (::g.pref.silent)  return;
  if (audioactive)  return;

#if HAVE_LIBPTHREAD
  pthread_t tid;
  pthread_create(&tid, NULL, sound_thread, (void *) snd);
#else
  if (fork() == 0) {
    sound_thread((void *) snd);
    exit(0);
  }
#endif
}
