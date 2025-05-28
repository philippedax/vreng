//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
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
// movie.cpp
//
// Movie haandling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "movie.hpp"
#include "matvec.hpp"   // V3 M4
#include "texture.hpp"	// Texture
#include "cache.hpp"	// download
#include "file.hpp"	// open, close
#include "endian.hpp"	// bigEndian
#include "user.hpp"	// localuser
#include "pref.hpp"	// quality
#include "format.hpp"	// Format
#include "spot.hpp"	// new Spot
#include "avi.hpp"	// Avi
#include "theme.hpp"	// theme.playvideo

#include <mpeg.h>	// /usr/local/include


const OClass Movie::oclass(MOVIE_TYPE, "Movie", Movie::creator);

const float Movie::FPS = 10.;		// max number of frames decoded per rendering
const float Movie::DEF_PROJ = 10;	// distance between projector and screen


/** Creator from file */
Object * Movie::creator(char *l)
{
  return new Movie(l);
}

/** Default values */
void Movie::defaults()
{
  rate = FPS;
  dist = DEF_PROJ;
  anim = false;
}

/** Parser */
void Movie::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if      (! stringcmp(l, "url"))  l = parseUrl(l, name.url);
    else if (! stringcmp(l, "dist")) l = parseFloat(l, &dist, "dist");
    else if (! stringcmp(l, "rate")) l = parseFloat(l, &rate, "rate");
    else if (! stringcmp(l, "anim")) l = parseBool(l, &anim, "anim");
    else {
      parse()->errorAtLine(l);
      l = parse()->nextToken();
      break;
    }
  }
  end_while_parse(l);
}

/** Constructor */
Movie::Movie(char *l)
{
  state = INACTIVE;
  texid = -1;
  frame = 0;
  begin = false;
  videobuf = NULL;
  texframe = NULL;
  mpg = NULL;
  avi = NULL;
  file = NULL;
  spot = NULL;

  line = new char[strlen(l)];
  strcpy(line, l);
  parser(l);

  mobileObject(0);

  vidfmt = Format::getPlayerByUrl(name.url);

  if (anim) {
    play();
  }
}

/** Opens mpg */
void Movie::open_mpg()
{
  if (mpg) return;		// an instance is already running

  char *filempg = new char[MAXHOSTNAMELEN];
  file = new File();

#if 0 //test
  cache = new Cache();

  Cache::path(name.url, filempg);
  echo("filempg: %s", filempg);
  if (! (fp = cache->open(name.url, NULL))) {
    error("can't open cache %s %p", name.url, fp);
    delete[] filempg;
    //delete cache;
  }
#else
  if (! Cache::download(name.url, filempg)) {	// download Mpeg file
    error("can't download %s", filempg);
    delete[] filempg;
    delete file;
    return;
  }
  if (! (fp = file->open(filempg, "r"))) {
    error("can't open filempg");
    delete[] filempg;
    file->close();
    delete file;
    return;
  }
#endif

  mpg = new ImageDesc[1];

  //SetMPEGOption(MPEG_DITHER, FULL_COLOR_DITHER); //ORDERED_DITHER);
  if (OpenMPEG(fp, mpg)) {
    width = mpg->Width;
    height = mpg->Height;
    fps = mpg->PictureRate;
    videobuf = new uint8_t[mpg->Size];
    //echo("mpg: w=%d h=%d f=%.3f", width, height, fps);
  }
  else {
    error("can't OpenMPEG %s", name.url);
    delete[] mpg;
    mpg = NULL;
    file->close();
    delete file;
    return;
  }
}

/** Opens avi */
void Movie::open_avi()
{
  if (avi) return;		// an instance is already running

  avi = new Avi(name.url);	// downloads avi file

  int ret = avi->read_header();
  if (ret) {
    error("avi: no header err=%d", ret);
    delete avi;
    avi = NULL;
    return;
  }
  fp = avi->getFile();
  avi->getInfos(&width, &height, &fps);
  videobuf = new uint8_t[4 * width * height];
  //echo("avi: w=%d h=%d f=%.3f", width, height, fps);
}

/** Inits texture */
void Movie::init_tex()
{
  GLint param = (::g.pref.quality3D) ? GL_LINEAR : GL_NEAREST;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // computes texsiz which must be a power of 2
  int i, power = 0;
  texsiz = MAX(width, height);
  while ((texsiz = texsiz/2) > 0) {
    power++;
  }
  for (i=0, texsiz = 1; i <= power; i++) {
    texsiz *= 2;
  }

  // allocates texture pixmap
  texframe = new GLubyte[3 * texsiz * texsiz];
  frame = 0;
  begin = true;

  // gets texid
  texid = Texture::getIdByObject(this);		// works if texture exists
  if (! texid) {
    texid = Texture::open(name.url);
  }
}

/** inits */
void Movie::inits()
{
  switch (vidfmt) {
    case PLAYER_MPG:
      open_mpg();
      break;
    case PLAYER_AVI:
      open_avi();
      break;
    default: return;
  }
  init_tex();

  // creates a spot
  spot = new Spot(this, (void *)Spot::SPOT_PENTAGON, 0L, 0L);
}

/** Plays a mpeg frame */
void Movie::play_mpg()
{
  if (! mpg) return;

  uint8_t r, g, b;
  if (Endian::bigEndian())
    r = 2, g = 1, b = 0; // BGR
  else
    r = 0, g = 1, b = 2; // RGB

  // get a frame from the mpg video stream
  if (GetMPEGFrame(reinterpret_cast<char *> (videobuf)) == 0) {	// end of mpg video
    if (state == LOOP) {
      RewindMPEG(fp, mpg);	// rewind mpg video
      begin = true;
      return;
    }
    CloseMPEG();
    delete[] mpg;
    mpg = NULL;
    file->close();
    delete file;
    state = INACTIVE;
    begin = true;
    if (spot) {
      spot->deleted = true;
      delete spot;
      spot = NULL;
    }
    return;
  }

  // mpg frame : build pixmap texture
  int wof = (texsiz - width) / 2;
  int hof = (texsiz - height) / 2;
  //echo("f=%d s=%d w=%d h=%d", frame, texsiz, width, height);
  if (mpg->Colormap) {	// case of Colormap Index
    for (int h=0; h < height; h++) {
      for (int w=0; w < width; w++) {
        int v = videobuf[width * h + w];
        ColormapEntry *color = &mpg->Colormap[v];
        int t = 3 * (texsiz * (h + hof) + w + wof);	// texframe index
        texframe[t+0] = color->red % 255;	
        texframe[t+1] = color->green % 255;	
        texframe[t+2] = color->blue % 255;
      }
    }
  }
  else {
    for (int h=0; h < height; h++) {
      for (int w=0; w < width; w++) {
        int v = 4 * (width * h + w);		// videobuf index
        int t = 3 * (texsiz * (h + hof) + w + wof);	// texframe index
        texframe[t+0] = videobuf[v+r];
        texframe[t+1] = videobuf[v+g];
        texframe[t+2] = videobuf[v+b];
      }
    }
  }
}

/** Plays an avi frame */
void Movie::play_avi()
{
  int len;
  uint8_t r, g, b;

  if (Endian::bigEndian())
    r = 2, g = 1, b = 0; // BGR
  else
    r = 0, g = 1, b = 2; // RGB

  // get a frame from the avi video stream
  if ( avi->read_data(videobuf, width * height * 4, &len) == 0 ) {	// end of avi video
    //echo("avi: f=%d s=%d l=%d", frame, width*height*4, len);
    file->close();
    delete file;
    delete avi;
    avi = NULL;
    begin = true;
    state = INACTIVE;
    if (spot) {
      spot->deleted = true;
      delete spot;
      spot = NULL;
    }
    return;
  }

  // avi frame : build pixmap texture : doesn't work !!!
  int wof = (texsiz - width) / 2;
  int hof = (texsiz - height) / 2;
  //echo("avi: f=%d s=%d w=%d h=%d", frame, texsiz, width, height);
  for (int h=0; h < height; h++) {
    for (int w=0; w < width; w++) {
      int v = 4 * (width * h + w);		// videobuf index
      int t = 4 * (texsiz * (h+hof) + w+wof);	// texframe index
      texframe[t+0] = videobuf[v+r];
      texframe[t+1] = videobuf[v+g];
      texframe[t+2] = videobuf[v+b];
      //texframe[t+3] = videobuf[v+3];
    }
  }
}

/** Binds frame tex */
void Movie::bind_frame()
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texid);
  switch (vidfmt) {
  case PLAYER_MPG:
    glTexImage2D(GL_TEXTURE_2D, 0, 3, texsiz, texsiz, 0, GL_RGB, GL_UNSIGNED_BYTE, texframe);
    break;
  case PLAYER_AVI:
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, texframe);
    break;
  }
}

/** Gets stream video frames */
void Movie::permanent(float lasting)
{
  if (state == INACTIVE || state == PAUSE) return;

  static struct timeval tstart;
  if (begin) {
    gettimeofday(&tstart, NULL);
    begin = false;
  }
  uint16_t finter = frame;	// previous frame

  // current frame
  struct timeval tcurr;
  gettimeofday(&tcurr, NULL);
  frame = (uint16_t) floor(((((float)((tcurr.tv_sec - tstart.tv_sec) * 1000.)) +
	                     ((float)((tcurr.tv_usec - tstart.tv_usec) / 1000.)) / 1000.)
                           ) * (float)(rate));
  for (uint16_t fdelta=0; fdelta < (frame-finter); fdelta++) {
    if (fdelta >= (uint16_t) fps) {
      return;	// ignore this frame
    }
    switch (vidfmt) {
    case PLAYER_MPG:
      play_mpg();
      break;
    case PLAYER_AVI:
      play_avi();
      break;
    }
  }
  bind_frame();
}

/** Quits */
#if 0 //dax
void Movie::quit()
{
  static int q = 0;
  q++;
  echo("quit: %d", q);
}
#endif

/*
 * Actions
 */

void Movie::play()
{
  if (state == INACTIVE) {
    state = PLAYING;
    permanentMovement();	// to get frames
    inits();
  }
}

void Movie::stop()
{
  if (state == INACTIVE) {
    return;	// nothing to stop
  }
  state = INACTIVE;
  switch (vidfmt) {
    case PLAYER_MPG:
      CloseMPEG();
      file->close();
      delete file;
      if (mpg) delete[] mpg;
      mpg = NULL;
      break;
    case PLAYER_AVI:
      if (avi) delete avi;
      avi = NULL;
      delete file;
      break;
  }

  stopPermanent();

  if (videobuf) delete[] videobuf;
  videobuf = NULL;
  if (texframe) delete[] texframe;
  texframe = NULL;
}

/** Pause  / Continue */
void Movie::pause()
{
  if (state == PLAYING || state == LOOP) {
    state = PAUSE;
  }
  else if (state == PAUSE) {
    state = PLAYING;	// leaves pause and continues
  }
}

/** Replay one time */
void Movie::rewind()
{
  if (state != PLAYING && state != LOOP && fp) {
    if (vidfmt == PLAYER_MPG) {
      RewindMPEG(fp, mpg);
      frame = 0;
      begin = true;
      state = PLAYING;
    }
  }
}

/** Play continuesly */
void Movie::loop()
{
  if (state == INACTIVE) {
    if (vidfmt == PLAYER_MPG) {
      state = LOOP;
      permanentMovement();	// to get frames
      inits();
    }
  }
}

/** callbacks actions */

void Movie::play_cb(Movie *movie, void *d, time_t s, time_t u)
{
  movie->play();
}

void Movie::stop_cb(Movie *movie, void *d, time_t s, time_t u)
{
  movie->stop();
}

void Movie::pause_cb(Movie *movie, void *d, time_t s, time_t u)
{
  movie->pause();
}

void Movie::rewind_cb(Movie *movie, void *d, time_t s, time_t u)
{
  movie->rewind();
}

void Movie::loop_cb(Movie *movie, void *d, time_t s, time_t u)
{
  movie->loop();
}

void Movie::funcs()
{
  setActionFunc(MOVIE_TYPE, 0, _Action play_cb, "Play");
  setActionFunc(MOVIE_TYPE, 1, _Action stop_cb, "Stop");
  setActionFunc(MOVIE_TYPE, 2, _Action pause_cb, "Pause");	// side effect if pending
  setActionFunc(MOVIE_TYPE, 3, _Action loop_cb, "Loop");
  //setActionFunc(MOVIE_TYPE, 4, _Action rewind_cb, "Rewind");	// crash in mpeglib
}
