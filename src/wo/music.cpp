//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "music.hpp"
#include "au.hpp"	// start
#include "mp3.hpp"	// start
#include "wav.hpp"	// start
#include "midi.hpp"	// start


const OClass Music::oclass(MUSIC_TYPE, "Music", Music::creator);

// local
static uint16_t oid = 0;


/* creation from a file */
WObject * Music::creator(char *l)
{
  return new Music(l);
}

void Music::defaults()
{
  state = INACTIVE;
  repeat = false;
  statestr[0] = '\0';
}

void Music::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "url"))   l = parse()->parseUrl(l, names.url);
    else if (! stringcmp(l, "state")) l = parse()->parseString(l, statestr, "state");
  }
  end_while_parse(l);
}

int Music::format()
{
  char *pext = strrchr(names.url, '.');
  pext++;

  if (!stringcmp(pext, "mp3") || !stringcmp(pext, "mp2"))
    fmt = FMT_MP3;
  else if (!stringcmp(pext, "wav"))
    fmt = FMT_WAV;
  else if (!stringcmp(pext, "au"))
    fmt = FMT_AU;
  else if (!stringcmp(pext, "mid") || !stringcmp(pext, "midi"))
    fmt = FMT_MIDI;
  else
    fmt = FMT_NONE;

  return fmt;
}

Music::Music(char *l)
{
  parser(l);
  format();

  enableBehavior(COLLIDE_NEVER);

  initMobileObject(0);
  createPermanentNetObject(PROPS, ++oid);

  if ((! stringcmp(statestr, "play")) || (! stringcmp(statestr, "loop")))
    play(this, (void *) 1, 0L, 0L);
}

bool Music::updateToNetwork(const Pos &oldpos)
{
  return updatePosToNetwork(oldpos, PROPXY, PROPZ, PROPAZ, PROPAX, PROPAY);
}

bool Music::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Music::play(Music *music, void *d, time_t s, time_t u)
{
  music->repeat = (d == (void *) 1) ? true : false;

  // check if d is an url
  if (music->repeat == false && (d != (void *)NULL)) {
    strcpy(music->names.url, (char *) d);
    music->fmt = music->format();
    music->state = Music::INACTIVE;
  }

  if (music->state == Music::INACTIVE) {
    switch (music->fmt) {
    case Music::FMT_MP3:  Mp3::start(music->names.url); break;
    case Music::FMT_WAV:  Wav::start(music->names.url, music->repeat); break;
    case Music::FMT_AU:   Au::start(music->names.url); break;
    case Music::FMT_MIDI: Midi::start(music->names.url); break;
    default: music->state = Music::INACTIVE; return;
    }
    music->state = Music::PLAYING;
  }
}

void Music::stop(Music *music, void *d, time_t s, time_t u)
{
  if (music->state == Music::PLAYING || music->state == Music::LOOPING) {
    switch (music->fmt) {
    case Music::FMT_MP3:  Mp3::stop(); break;
    case Music::FMT_WAV:  Wav::stop(); break;
    case Music::FMT_AU:   Au::stop(); break;
    case Music::FMT_MIDI: Midi::stop(); break;
    }
    music->state = Music::INACTIVE;
  }
}

void Music::pause(Music *music, void *d, time_t s, time_t u)
{
  if (music->state == Music::PLAYING || music->state == Music::LOOPING) {
    switch (music->fmt) {
    case Music::FMT_MP3:  Mp3::pause(); break;
    case Music::FMT_WAV:  Wav::pause(); break;
    case Music::FMT_AU:   Au::pause(); break;
    case Music::FMT_MIDI: Midi::pause(); break;
    default: music->state = Music::INACTIVE; return;
    }
    music->state = Music::PAUSING;
  }
}

void Music::cont(Music *music, void *d, time_t s, time_t u)
{
  if (music->state == Music::PAUSING) {
    switch (music->fmt) {
    case Music::FMT_MP3:  Mp3::cont(); break;
    case Music::FMT_WAV:  Wav::cont(); break;
    case Music::FMT_AU:   Au::cont(); break;
    case Music::FMT_MIDI: Midi::cont(); break;
    default: music->state = Music::INACTIVE; return;
    }
    music->state = Music::PLAYING;
  }
}

void Music::loop(Music *music, void *d, time_t s, time_t u)
{
  if (music->state == Music::INACTIVE) {
    music->state = Music::LOOPING;
    play(music, (void *) 1, 0L, 0L);
  }
}

void Music::quit()
{
  switch (fmt) {
  case FMT_MP3:  Mp3::stop(); break;
  case FMT_WAV:  Wav::stop(); break;
  case FMT_AU:   Au::stop(); break;
  case FMT_MIDI: Midi::stop(); break;
  }
  oid = 0;
  savePersistency();
}

void Music::funcs()
{
  getPropertyFunc(MUSIC_TYPE, PROPXY, _Payload get_xy);
  getPropertyFunc(MUSIC_TYPE, PROPZ, _Payload get_z);
  getPropertyFunc(MUSIC_TYPE, PROPAZ, _Payload get_az);
  getPropertyFunc(MUSIC_TYPE, PROPAX, _Payload get_ax);
  getPropertyFunc(MUSIC_TYPE, PROPAY, _Payload get_ay);
  getPropertyFunc(MUSIC_TYPE, PROPHNAME, _Payload get_hname);

  putPropertyFunc(MUSIC_TYPE, PROPXY, _Payload put_xy);
  putPropertyFunc(MUSIC_TYPE, PROPZ, _Payload put_z);
  putPropertyFunc(MUSIC_TYPE, PROPAZ, _Payload put_az);
  putPropertyFunc(MUSIC_TYPE, PROPAX, _Payload put_ax);
  putPropertyFunc(MUSIC_TYPE, PROPAY, _Payload put_ay);
  putPropertyFunc(MUSIC_TYPE, PROPHNAME, _Payload put_hname);

  setActionFunc(MUSIC_TYPE, PLAY, _Action play, "Play");
  setActionFunc(MUSIC_TYPE, STOP, _Action stop, "Stop");
  setActionFunc(MUSIC_TYPE, PAUSE, _Action pause, "Pause");
  setActionFunc(MUSIC_TYPE, CONT, _Action cont, "Continue");
  setActionFunc(MUSIC_TYPE, LOOP, _Action loop, "Loop");
  setActionFunc(MUSIC_TYPE, MOVE, _Action moveObject, "Move");
}
