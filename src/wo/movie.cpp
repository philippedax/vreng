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
#include "movie.hpp"
#include "texture.hpp"	// getCurrent
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
  pixvid = NULL;
  pixtex = NULL;
  mpeg = NULL;
  avi = NULL;
  begin = false;

  parser(l);

  initMobileObject(0);
}

int Movie::inits()
{
  texid = Texture::getIdByObject(this);		// bad texid returned FIXME!
  if (! texid)
    texid = Texture::open(names.url);
  //dax texid = Texture::open(names.url);
  //error("texid : %d", texid);
  trace(DBG_WO, "texid=%d (%s)", texid, Texture::getUrlById(texid));

  video = Format::getPlayerByUrl(names.url);
  switch (video) {
    case PLAYER_MPG: mpegInit(); break;
    case PLAYER_AVI: aviInit(); break;
    default: return 0;
  }

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
  pixvid = new uint8_t[mpeg->Size];
}

void Movie::aviInit()
{
  if (avi) return;		// an instance is already running

  avi = new Avi(names.url);	// downloads avi file

  int r = avi->read_header();
  if (r) {
    error("avi: no header err=%d", r);
    delete avi;
    avi = NULL;
    return;
  }
  fp = avi->getFile();
  avi->getInfos(&width, &height, &fps);
  trace(DBG_WO, "avi: w=%d h=%d f=%.3f", width, height, fps);
  pixvid = new uint8_t[3 * width * height];
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

  for (int f=0; f < (frame-inter); f++) {
    /* get video frame or not */
    if (f >= (int) fps) {
      break;
    }
    switch (video) {
    case PLAYER_AVI:
      {
        int r, l;

        r = avi->read_data(pixvid, width * height * 3, &l); // get Avi frame
        //error("l=%d", l);
        if (r == 0) {
          error("end of avi video");
          File::closeFile(fp);
          state = INACTIVE;
          delete avi;
          avi = NULL;
          frame = 0;
          begin = true;
          return;
        }
        int wof = (texsz - width) / 2;
        int hof = (texsz - height) / 2;
        trace(DBG_FORCE, "frame=%d id=%d sz=%d w=%d h=%d", frame, texid, texsz, width, height);
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int u = 3 * (texsz * (h+hof) + (w+wof));
            int v = 3 * (width * h + w);
            if (File::littleEndian()) {
              pixtex[u+0] = pixvid[v+0];
              pixtex[u+1] = pixvid[v+1];
              pixtex[u+2] = pixvid[v+2];
            }
            else { // RGB = BGR
              pixtex[u+0] = pixvid[v+2];
              pixtex[u+1] = pixvid[v+1];
              pixtex[u+2] = pixvid[v+0];
            }
          }
        }
      }
      break;
    case PLAYER_MPG:
      {
        if (GetMPEGFrame((char *)pixvid) == false) {	// get mpeg frame
          if (state == LOOP) {
            RewindMPEG(fp, mpeg);
            frame = 0;
            begin = true;
            return;
          }
          CloseMPEG();
          state = INACTIVE;
          frame = 0;
          begin = true;
          return;
        }
        int wof = (texsz - width) / 2;
        int hof = (texsz - height) / 2;
        trace(DBG_WO, "frame=%d id=%d sz=%d w=%d h=%d", frame, texid, texsz, width, height);

        if (mpeg->Colormap) {	// case of Colormap Index
          for (int h=0; h < height; h++) {
            for (int w=0; w < width; w++) {
              int index = pixvid[h * width + w];
              ColormapEntry *color = &mpeg->Colormap[index];
              int u = 3 * ((h+hof) * texsz + (w+hof));
              pixtex[u+0] = color->red % 255;	
              pixtex[u+1] = color->green % 255;	
              pixtex[u+2] = color->blue % 255;
            }
          }
        }
        else {
          for (int h=0; h < height; h++) {
            for (int w=0; w < width; w++) {
              int u = 3 * (texsz * (h+hof) + (w+wof));
              int v = 4 * (width * h + w);
              if (File::littleEndian()) {
                pixtex[u+0] = pixvid[v+0];
                pixtex[u+1] = pixvid[v+1];
                pixtex[u+2] = pixvid[v+2];
              }
              else { // RGB = BGR
                pixtex[u+0] = pixvid[v+2];
                pixtex[u+1] = pixvid[v+1];
                pixtex[u+2] = pixvid[v+0];
              }
            }
          }
        }
      } //end mpeg
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
  if (movie->pixvid) delete[] movie->pixvid;
  movie->pixvid = NULL;
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
}
