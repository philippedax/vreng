//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#include "vreng.hpp"
#include "movie.hpp"
#include "texture.hpp"	// current
#include "cache.hpp"	// download
#include "file.hpp"	// openFile
#include "pref.hpp"	// quality
#include "format.hpp"	// Format
#if HAVE_LIBMPEG
#include <mpeg.h>	// /usr/local/include
#else
#include "mpeg.h"	// local: src/bundle/mpeg_lib
#endif
#include "avi.hpp"	// Avi
#include "theme.hpp"	// theme.playvideo


const OClass Movie::oclass(MOVIE_TYPE, "Movie", Movie::creator);

const float Movie::FPS = 10.;	// max number of frames decoded per rendering


WObject * Movie::creator(char *l)
{
  return new Movie(l);
}

void Movie::defaults()
{
  rate = FPS;
  anim = false;
}

void Movie::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (! stringcmp(l, "url"))  l = parse()->parseUrl(l, names.url);
    else if (! stringcmp(l, "rate")) l = parse()->parseFloat(l, &rate, "rate");
    else if (! stringcmp(l, "anim")) l = parse()->parseBool(l, &anim, "anim");
  }
  end_while_parse(l);
}

Movie::Movie(char *l)
{
  state = INACTIVE;
  texid = -1;
  frame = 0;
  vidbuf = NULL;
  texmap = NULL;
  mpeg = NULL;
  avi = NULL;
  begin = false;

  parser(l);

  initMobileObject(0);

  if (anim) {
    loop(this, 0, 0, 0);
  }
}

void Movie::inits()
{
#if 0 //dax
  //dax Texture *tex = new Texture(null);
  texid = Texture::current();
#else
  texid = Texture::getIdByObject(this);		// works if texture exists
  if (! texid) {
    texid = Texture::open(names.url);
  }
  trace(DBG_WO, "texid=%d (%s)", texid, Texture::getUrlById(texid));
#endif
  //error("texid : %d", texid);

  vidfmt = Format::getPlayerByUrl(names.url);
  switch (vidfmt) {
    case PLAYER_MPG: mpegInit(); break;
    case PLAYER_AVI: aviInit(); break;
    default: return;
  }

  // texsiz must be a power of 2
  int i, power = 0;
  texsiz = MAX(width, height);
  while ((texsiz = texsiz/2) > 0) {
    power++;
  }
  for (i=0, texsiz = 1; i <= power; i++) {
    texsiz *= 2;
  }

  texmap = new GLubyte[3 * texsiz * texsiz];
  frame = 0;
  begin = true;
}

void Movie::mpegInit()
{
  char *filempeg = new char[MAXHOSTNAMELEN];

  if (Cache::download(names.url, filempeg) == 0) {	// download Mpeg file
    error("can't download %s", filempeg);
    delete[] filempeg;
    return;
  }
  if ((fp = File::openFile(filempeg, "r")) == NULL) {
    error("can't open mpeg");
    delete[] filempeg;
    return;
  }

  mpeg = new ImageDesc[1];

  SetMPEGOption(MPEG_DITHER, FULL_COLOR_DITHER); //ORDERED_DITHER);
  OpenMPEG(fp, mpeg);

  width = mpeg->Width;
  height = mpeg->Height;
  fps = mpeg->PictureRate;
  vidbuf = new uint8_t[mpeg->Size];
  trace(DBG_WO, "mpeg: w=%d h=%d f=%.3f", width, height, fps);
}

void Movie::aviInit()
{
  if (avi) return;		// an instance is already running

  avi = new Avi(names.url);	// downloads avi file

  int ret = avi->read_header();
  if (ret) {
    error("avi: no header err=%d", ret);
    delete avi;
    avi = NULL;
    return;
  }
  fp = avi->getFile();
  avi->getInfos(&width, &height, &fps);
  vidbuf = new uint8_t[4 * width * height];
  trace(DBG_WO, "avi: w=%d h=%d f=%.3f", width, height, fps);
}

void Movie::changePermanent(float lasting)
{
  if (state == INACTIVE || state == PAUSE) return;

  static struct timeval tstart;
  struct timeval tcurr;
  uint8_t r, g, b;

  if (File::littleEndian())
    r = 0, g = 1, b = 2; // RGB
  else
    r = 2, g = 1, b = 0; // BGR

  if (begin) {
    gettimeofday(&tstart, NULL);
    begin = false;
  }
  uint16_t finter = frame;	// previous frame

  gettimeofday(&tcurr, NULL);
  frame = (uint16_t) floor(((((float) (tcurr.tv_sec - tstart.tv_sec) * 1000.) +
	                   ((float) (tcurr.tv_usec - tstart.tv_usec) / 1000.)
                           ) / 1000.) * (float) rate);	// current frame

  for (uint16_t fdelta=0; fdelta < (frame-finter); fdelta++) {
    if (fdelta >= (uint16_t) fps) {
      break;	// ignore this frame
    }
    switch (vidfmt) {
    case PLAYER_AVI:
      {
        // get a frame from the avi video stream
        int ret, retlen;
        ret = avi->read_data(vidbuf, width * height * 4, &retlen);
        //error("f=%d s=%d l=%d", frame, width*height*4, retlen);
        if (ret == 0) {	// end of avi video
          File::closeFile(fp);
          state = INACTIVE;
          delete avi;
          avi = NULL;
          begin = true;
          return;
        }
        // build pixmap texture : doesn't work !!!
        int wof = (texsiz - width) / 2;
        int hof = (texsiz - height) / 2;
        wof = hof = 0; //dax
        //error("f=%d s=%d w=%d h=%d", frame, texsiz, width, height);
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int v = 4 * (width * h + w);		// vidbuf index
            int t = 3 * (texsiz * (h+hof) + w + wof);	// texmap index
            //error("w,h: %d,%d t,v: %d,%d", w,h,t,v);
            texmap[t+0] = vidbuf[v+r];
            texmap[t+1] = vidbuf[v+g];
            texmap[t+2] = vidbuf[v+b];
          }
        }
      }
      break;
    case PLAYER_MPG:
      // get a frame from the mpeg video stream
      if (GetMPEGFrame((char *)vidbuf) == false) { // end of mpeg video
        if (state == LOOP) {
          RewindMPEG(fp, mpeg);	// rewind mpeg video
          begin = true;
          return;
        }
        CloseMPEG();
        state = INACTIVE;
        begin = true;
        return;
      }
      // build pixmap texture
      int wof = (texsiz - width) / 2;
      int hof = (texsiz - height) / 2;
      //error("f=%d s=%d w=%d h=%d", frame, texsiz, width, height);
      if (mpeg->Colormap) {	// case of Colormap Index
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int v = vidbuf[width * h + w];
            ColormapEntry *color = &mpeg->Colormap[v];
            int t = 3 * (texsiz * (h + hof) + w + wof);	// texmap index
            texmap[t+0] = color->red % 255;	
            texmap[t+1] = color->green % 255;	
            texmap[t+2] = color->blue % 255;
          }
        }
      }
      else {
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int v = 4 * (width * h + w);		// vidbuf index
            int t = 3 * (texsiz * (h + hof) + w + wof);	// texmap index
            texmap[t+0] = vidbuf[v+r];
            texmap[t+1] = vidbuf[v+g];
            texmap[t+2] = vidbuf[v+b];
          }
        }
      }
      break;
    } //end switch
  } //end for frame

  // bind the frame texid
  GLint param = (::g.pref.quality3D) ? GL_LINEAR : GL_NEAREST;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, texsiz, texsiz, 0, GL_RGB, GL_UNSIGNED_BYTE, texmap);
}

void Movie::play(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state == Movie::INACTIVE) {
    movie->state = Movie::PLAYING;
    movie->enablePermanentMovement();	// for get frames
    movie->inits();
  }
}

void Movie::stop(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state != Movie::INACTIVE) {
    movie->state = Movie::INACTIVE;
    if (movie->vidfmt == PLAYER_MPG) {
      CloseMPEG();
      File::closeFile(movie->fp);
    }
  }
  movie->disablePermanentMovement();

  if (movie->mpeg) delete[] movie->mpeg;
  movie->mpeg = NULL;
  //if (movie->avi) delete[] movie->avi;
  //movie->avi = NULL;
  if (movie->vidbuf) delete[] movie->vidbuf;
  movie->vidbuf = NULL;
  if (movie->texmap) delete[] movie->texmap;
  movie->texmap = NULL;
}

void Movie::pause(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state == Movie::PLAYING || movie->state == Movie::LOOP)
    movie->state = Movie::PAUSE;
  else if (movie->state == Movie::PAUSE)
    movie->state = Movie::PLAYING;
}

void Movie::rewind(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state != Movie::PLAYING && movie->state != Movie::LOOP && movie->fp != NULL) {
    if (movie->vidfmt == PLAYER_MPG) {
      RewindMPEG(movie->fp, movie->mpeg);
    }
    movie->frame = 0;
    movie->begin = true;
    movie->state = Movie::PLAYING;
  }
}

void Movie::loop(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state == Movie::INACTIVE) {
    movie->state = Movie::LOOP;
    movie->enablePermanentMovement();	// for get frames
    movie->inits();
  }
}

void Movie::debug(Movie *movie, void *d, time_t s, time_t u)
{
  Texture::listTextures();
}

void Movie::funcs()
{
#if 0
  setActionFunc(MOVIE_TYPE, 0, _Action play, (const char *) uitem(ulabel(g.theme.Playvideo)));
#else
  setActionFunc(MOVIE_TYPE, 0, _Action play, "Play");
#endif
  setActionFunc(MOVIE_TYPE, 1, _Action stop, "Stop");
  setActionFunc(MOVIE_TYPE, 2, _Action pause, "Pause");
  setActionFunc(MOVIE_TYPE, 3, _Action loop, "Loop");
  setActionFunc(MOVIE_TYPE, 4, _Action rewind, "Rewind");
  setActionFunc(MOVIE_TYPE, 5, _Action moveObject, "Move");
  setActionFunc(MOVIE_TYPE, 6, _Action debug, "Debug");
}
