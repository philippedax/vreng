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
#include "clip.hpp"
#include "world.hpp"	// current
#include "channel.hpp"	// getGroup
#include "cache.hpp"	// download
#include "audio.hpp"	// start
#include "video.hpp"	// start
#include "mpeg.hpp"	// start


const OClass Clip::oclass(CLIP_TYPE, "Clip", Clip::creator);

// local
static const char * RTP_URL = "http://avods.enst.fr/";
static const char * PLAY_CGI = "rtpplay.cgi";
static const char * STOP_CGI = "rtpstop.cgi";
static const char * PAUSE_CGI = "rtppause.cgi";
static const char * CONT_CGI = "rtpcont.cgi";


WO * Clip::creator(char *l)
{
  return new Clip(l);
}

void Clip::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (!stringcmp(l, "url")) l = parseUrl(l, names.url);
  }
  end_while_parse(l);
}

Clip::Clip(char *l)
{
  parser(l);

  enableBehavior(COLLIDE_ONCE);

  initMobileObject(0);

  char *purl, *ptmpurl;

  purl = strrchr(names.url, '.');
  purl++;
  if (!stringcmp(purl, "mpg") || !stringcmp(purl, "mpeg")) fmt = FMT_MPEG;
  else if (!stringcmp(purl, "video")) {
    fmt = FMT_RTPAV;
    strcpy(url2, names.url);
    ptmpurl = strrchr(url2, '.');
    *(++ptmpurl) = '\0';
    strcpy(ptmpurl, "audio");
  }
  else if (!stringcmp(purl, "audio")) {
    fmt = FMT_RTPAV;
    strcpy(url2, names.url);
    ptmpurl = strrchr(names.url, '.');
    *(++ptmpurl) = '\0';
    strcpy(ptmpurl, "video");
  }
  else
    error("Clip: unknown media format");
  state = RTP_INACTIVE;
}

bool Clip::whenIntersect(WO *pcur, WO *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void postCgi(const char *cmd)
{
  Cache::download(cmd, NULL);
}

void Clip::startRtp()
{
  if (state == RTP_ACTIVE) return;  // busy

  char *pfile, *end;
  pfile = strrchr(names.url, '/');
  pfile++;
  end = strchr(pfile, '.');
  *end = '\0';

  char group[GROUP_LEN];
  Channel::getGroup(World::current()->getChan(), group);

  char cmd[URL_LEN];
  sprintf(cmd, "%s%s?%s&%s&%d&%d", RTP_URL, PLAY_CGI,
          pfile,
          group,
          Channel::getPort(World::current()->getChan()) + 2,
          Channel::currentTtl());
  postCgi(cmd);
  state = RTP_ACTIVE;
}

void Clip::stopRtp()
{
  if (state == RTP_INACTIVE) return;  // nothing

  char group[GROUP_LEN];
  const char* channel = World::current()->getChan();
  Channel::getGroup(channel, group);

  char cmd[URL_LEN];
  sprintf(cmd, "%s%s?%s/%d/%d&%s/%d/%d", RTP_URL, STOP_CGI,
          group,
          Channel::getPort(channel) + 2,
          Channel::currentTtl(),
          group,
          Channel::getPort(channel) + 4,
          Channel::currentTtl()
         );
  postCgi(cmd);
  state = RTP_INACTIVE;
}

void Clip::pauseRtp()
{
  if (state == RTP_INACTIVE) return;

  const char* channel = World::current()->getChan();
  char group[GROUP_LEN];
  Channel::getGroup(channel, group);

  char cmd[URL_LEN];
  sprintf(cmd, "%s%s?%s/%d/%d&%s/%d/%d", RTP_URL, PAUSE_CGI,
          group,
          Channel::getPort(channel) + 2,
          Channel::currentTtl(),
          group,
          Channel::getPort(channel) + 4,
          Channel::currentTtl()
         );
  postCgi(cmd);
}

void Clip::contRtp()
{
  if (state == RTP_INACTIVE) return;

  const char* channel = World::current()->getChan();
  char group[GROUP_LEN];
  Channel::getGroup(channel, group);

  char cmd[URL_LEN];
  sprintf(cmd, "%s%s?%s/%d/%d&%s/%d/%d", RTP_URL, CONT_CGI,
          group,
          Channel::getPort(channel) + 2,
          Channel::currentTtl(),
          group,
          Channel::getPort(channel) + 4,
          Channel::currentTtl()
         );
  postCgi(cmd);
}

void Clip::play(Clip *clip, void *d, time_t s, time_t u)
{
  switch (clip->fmt) {
  case Clip::FMT_MPEG:
    Mpeg::start(clip->names.url);
    break;
  case Clip::FMT_RTPAV:
    Video::start(World::current()->getChan());
    Audio::start(World::current()->getChan());
    clip->startRtp();
    break;
  default:
    break;
  }
}

void Clip::stop(Clip *clip, void *d, time_t s, time_t u)
{
  switch (clip->fmt) {
  case Clip::FMT_MPEG:
    Mpeg::stop();
    break;
  case Clip::FMT_RTPAV:
    Video::quit();
    Audio::quit();
    clip->stopRtp();
    break;
  default:
    break;
  }
}

void Clip::pause(Clip *clip, void *d, time_t s, time_t u)
{
  switch (clip->fmt) {
  case Clip::FMT_MPEG:
    Mpeg::pause();
    break;
  case Clip::FMT_RTPAV:
    clip->pauseRtp();
    break;
  default:
    break;
  }
}

void Clip::cont(Clip *clip, void *d, time_t s, time_t u)
{
  switch (clip->fmt) {
  case Clip::FMT_MPEG:
    Mpeg::cont();
    break;
  case Clip::FMT_RTPAV:
    clip->contRtp();
    break;
  default:
    break;
  }
}

void Clip::quit()
{
  state = RTP_INACTIVE;
  stopRtp();
  Mpeg::stop();
}

void Clip::funcs()
{
  setActionFunc(CLIP_TYPE, 0, _Action play, "Play");
  setActionFunc(CLIP_TYPE, 1, _Action stop, "Stop");
  setActionFunc(CLIP_TYPE, 2, _Action pause, "Pause");
  setActionFunc(CLIP_TYPE, 3, _Action cont, "Continue");
  setActionFunc(CLIP_TYPE, 4, _Action moveObject, "Move");
}
