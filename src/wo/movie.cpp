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
  }
  end_while_parse(l);
}

Movie::Movie(char *l)
{
  state = INACTIVE;
  texid = -1;
  frame = 0;
  vidbuf = NULL;
  pixtex = NULL;
  mpeg = NULL;
  avi = NULL;
  begin = false;

  parser(l);

  initMobileObject(0);
}

int Movie::inits()
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

  video = Format::getPlayerByUrl(names.url);
  switch (video) {
    case PLAYER_MPG: mpegInit(); break;
    case PLAYER_AVI: aviInit(); break;
    default: return 0;
  }

  // texsz must be a power of 2
  int i, power = 0;
  texsz = MAX(width, height);
  while ((texsz = texsz/2) > 0) {
    power++;
  }
  for (i=0, texsz=1; i <= power; i++) {
    texsz *= 2;
  }

  pixtex = new GLubyte[3 * texsz * texsz];
  frame = 0;
  begin = true;
  return 1;
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
  static struct timeval tstart;

  if (state == INACTIVE || state == PAUSE) return;

  if (begin) {
    gettimeofday(&tstart, NULL);
    begin = false;
  }

  uint16_t inter = frame;
  struct timeval tnow;
  gettimeofday(&tnow, NULL);
  frame = (int) floor(((((float) (tnow.tv_sec - tstart.tv_sec) * 1000.) +
	                ((float) (tnow.tv_usec - tstart.tv_usec) / 1000.)
                     ) / 1000.) * (float) rate);
  //error("frame = %d inter = %d", frame, inter);

  uint8_t r,g,b;
  if (File::littleEndian())
    r = 0, g = 1, b = 2; // RGB
  else
    r = 2, g = 1, b = 0; // BGR

  for (int f=0; f < (frame-inter); f++) {
    /* get video frame or not */
    if (f >= (int) fps) {
      break;
    }
    switch (video) {
    case PLAYER_AVI:
      {
        int ret, retlen;

        ret = avi->read_data(vidbuf, width * height * 4, &retlen); // get Avi frame
        //error("f=%d s=%d l=%d", frame, width*height*4, retlen);
        if (ret == 0) {	// end of avi video
          File::closeFile(fp);
          state = INACTIVE;
          delete avi;
          avi = NULL;
          begin = true;
          return;
        }

        // build pixmap texture
        int wof = (texsz - width) / 2;
        int hof = (texsz - height) / 2;
        error("f=%d id=%d s=%d w=%d h=%d", frame, texid, texsz, width, height);
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int u = 3 * (texsz * (h+hof) + (w+wof));	// pixtex index
            int v = 4 * (width * h + w);		// vidbuf index
            //error("w,h: %d,%d u,v: %d,%d", w,h,u,v);
            pixtex[u+0] = vidbuf[v+r];
            pixtex[u+1] = vidbuf[v+g];
            pixtex[u+2] = vidbuf[v+b];
          }
        }
      }
      break;
    case PLAYER_MPG:
      if (GetMPEGFrame((char *)vidbuf) == false) {	// get mpeg frame
        if (state == LOOP) {
          RewindMPEG(fp, mpeg);
          begin = true;
          return;
        }
        CloseMPEG();
        state = INACTIVE;
        begin = true;
        return;
      }

      // build pixmap texture
      int wof = (texsz - width) / 2;
      int hof = (texsz - height) / 2;
      //error("f=%d id=%d s=%d w=%d h=%d", frame, texid, texsz, width, height);
      if (mpeg->Colormap) {	// case of Colormap Index
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int idx = vidbuf[width * h + w];
            ColormapEntry *color = &mpeg->Colormap[idx];
            int u = 3 * (texsz * (h + hof) + w + wof);
            pixtex[u+0] = color->red % 255;	
            pixtex[u+1] = color->green % 255;	
            pixtex[u+2] = color->blue % 255;
          }
        }
      }
      else {
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int u = 3 * (texsz * (h + hof) + w + wof);	// pixtex index
            int v = 4 * (width * h + w);		// vidbuf index
            pixtex[u+0] = vidbuf[v+r];
            pixtex[u+1] = vidbuf[v+g];
            pixtex[u+2] = vidbuf[v+b];
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
  glTexImage2D(GL_TEXTURE_2D, 0, 3, texsz, texsz, 0, GL_RGB, GL_UNSIGNED_BYTE, pixtex);
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
    if (movie->video == PLAYER_MPG) {
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
  if (movie->pixtex) delete[] movie->pixtex;
  movie->pixtex = NULL;
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
    if (movie->video == PLAYER_MPG) {
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
