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
#include "pref.hpp"
#include "format.hpp"	// Format
#include "theme.hpp"	// theme.playvideo
#include "avi.hpp"	// Avi
#if HAVE_LIBMPEG
#include <mpeg.h>	// /usr/local/include
#else
#include "mpeg.h"	// local: src/extra/mpeg_lib
#endif


const OClass Movie::oclass(MOVIE_TYPE, "Movie", Movie::creator);

const float Movie::FPS = 10.;	// max number of frames decoded per rendering


WObject * Movie::creator(char *l)
{
  return new Movie(l);
}

void Movie::defaults()
{
  state = INACTIVE;
  begin = false;
  fp = NULL;
  texid = 0;
  rate = FPS;
  pixmap = NULL;
  pixtex = NULL;
  avi = NULL;
  video = 0;
  cachempeg = NULL;
  imgmpeg = NULL;
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

void Movie::behavior()
{
  enableBehavior(PERSISTENT);
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_ONCE);
  setRenderPrior(RENDER_NORMAL);
}

void Movie::init()
{
  initializeMobileObject(0);
}

Movie::Movie(char *l)
{
  parser(l);
  behavior();
  init();
}

int Movie::inits()
{
  //Texture::listTextures();
  texid = Texture::getIdByObject(this);
  trace(DBG_FORCE, "texid=%d (%s)", texid, Texture::getUrlById(texid));

  video = Format::getPlayerByUrl(names.url);
  if (video == PLAYER_AVI) return aviInit();
  else return mpegInit();
}

int Movie::aviInit()
{
  if (avi) return 0;

  avi = new Avi(names.url);

  int r = avi->read_header();
  if (r) {
    error("avi: error=%d", r);
    delete avi;
    avi = NULL;
    return r;
  }
  fp = avi->getInfos(&width, &height, &fps);
  //error("avi: w=%d h=%d f=%.3f", width, height, fps);
  pixmap = new uint8_t[3 * width * height];
  begin = true;
  frame = 0;

  /* compute texsize */
  int i, power = 0;
  texsize = MAX(width, height);
  while ((texsize = texsize/2) > 0) power++;
  for (i=0, texsize=1; i <= power; i++) texsize *= 2;
  //error("texsize=%d", texsize);

  /* get memory for tex pixmap */
  pixtex = new GLubyte[3 * texsize * texsize];

  return r;
}

void mpegAbort(int sig)
{
  CloseMPEG();
}

int Movie::mpegInit()
{
  /* download Mpeg file */
  cachempeg = new char[MAXHOSTNAMELEN];
  if (Cache::download(names.url, cachempeg) == 0) {
    error("can't download %s", cachempeg);
    delete[] cachempeg;
    return 0;
  }
  if ((fp = File::openFile(cachempeg, "r")) == NULL) {
    error("can't open mpeg");
    delete[] cachempeg;
    return 0;
  }

  /* init Mpeg */
  imgmpeg = new ImageDesc[1];

  SetMPEGOption(MPEG_DITHER, FULL_COLOR_DITHER); //ORDERED_DITHER);
  OpenMPEG(fp, imgmpeg);

  /* get Mpeg infos */
  width = imgmpeg->Width;
  height = imgmpeg->Height;
  fps = imgmpeg->PictureRate;
  pixmap = new uint8_t[imgmpeg->Size];

  /* compute texsize */
  int i, power = 0;
  texsize = MAX(width, height);
  while ((texsize = texsize/2) > 0) power++;
  for (i=0, texsize=1; i <= power; i++) texsize *= 2;
  //error("mpg: size=%d w=%d h=%d texsize=%d f=%.1f", imgmpeg->Size,width,height,texsize,fps);

  /* get memory for tex pixmap */
  pixtex = new GLubyte[3 * texsize * texsize];

  frame = 0;
  begin = true;
  signal(SIGABRT, mpegAbort);
  return 1;
}

void Movie::changePermanent(float lasting)
{
  if (state == INACTIVE || state == PAUSE) return;

  if (begin) {
    gettimeofday(&start, NULL);
    begin = false;
  }

  uint16_t inter = frame;
  struct timeval t;
  gettimeofday(&t, NULL);
  frame = (int) floor(((((float) (t.tv_sec - start.tv_sec) * 1000.) +
	                ((float) (t.tv_usec - start.tv_usec) / 1000.)
                     ) / 1000.) * (float) rate);

  for (int f=0; f < frame-inter; f++) {
    /* get Mpeg frame or not */
    if (f >= (int) fps) break;

    if (video == PLAYER_AVI) {
      /* get Avi frame */
      if (avi) {
        int r, l;
        static int n = 0;

        r = avi->read_data(pixmap, width * height * 3, &l);
        //error("n=%d l=%d", n, l);
        if (r == 0) {
          //error("end of avi video");
          File::closeFile(fp);
          fp = NULL;
          delete avi;
          avi = NULL;
          begin = true;
          state = INACTIVE;
          return;
        }
        n++;
        int woff = (texsize - width) / 2;
        int hoff = (texsize - height) / 2;
        for (int j=0; j < height; j++) {
          for (int i=0; i < width; i++) {
            int u = 3 * (texsize * (j+hoff) + (i+woff));
            int v = 3 * (width * j + i);
            if (File::littleEndian()) {
              pixtex[u+0] = pixmap[v+0];
              pixtex[u+1] = pixmap[v+1];
              pixtex[u+2] = pixmap[v+2];
            }
            else {
              pixtex[u+0] = pixmap[v+2]; // RGB = ABG
              pixtex[u+1] = pixmap[v+1];
              pixtex[u+2] = pixmap[v+0];
            }
          }
        }
      }
    }
    else if (video == PLAYER_MPG) {
      if (GetMPEGFrame((char *)pixmap) == false) {
        /* The End */
        if (state == LOOP) {
          RewindMPEG(fp, imgmpeg);
          begin = true;
          return;
        }
        error("frames=%d", ++frame);
        CloseMPEG();
        state = INACTIVE;
        return;
      }
      int woff = (texsize - width) / 2;
      int hoff = (texsize - height) / 2;
      trace(DBG_WO, "frame=%d f=%d texid=%d size=%d w=%d h=%d woff=%d hoff=%d", frame, f, texid, texsize, width, height, woff, hoff);

      ColormapEntry *colorMap = imgmpeg->Colormap;
      if (colorMap) {	// case of Colormap Index
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int cmapindex = pixmap[h * width + w];
            ColormapEntry *color = &colorMap[cmapindex];
            int u = 3 * ((h+hoff) * texsize + (w+hoff));
            pixtex[u+0] = color->red % 255;	
            pixtex[u+1] = color->green % 255;	
            pixtex[u+2] = color->blue % 255;
          }
        }
      }
      else {
        for (int h=0; h < height; h++) {
          for (int w=0; w < width; w++) {
            int u = 3 * (texsize * (h+hoff) + (w+woff));
            int v = 4 * (width * h + w);
            if (File::littleEndian()) {
              pixtex[u+0] = pixmap[v+0];
              pixtex[u+1] = pixmap[v+1];
              pixtex[u+2] = pixmap[v+2];
            }
            else {
              pixtex[u+0] = pixmap[v+2]; // RGB = ABG
              pixtex[u+1] = pixmap[v+1];
              pixtex[u+2] = pixmap[v+0];
            }
          }
        }
      }
    }
  }

  // bind the frame texid
  GLint param = (::g.pref.quality3D) ? GL_LINEAR : GL_NEAREST;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, texsize, texsize, 0, GL_RGB, GL_UNSIGNED_BYTE, pixtex);
}

void Movie::play(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state == Movie::INACTIVE) {
    movie->state = Movie::PLAYING;
    movie->enablePermanentMovement();	// for get frames
    if (! movie->inits()) return;
  }
}

void Movie::stop(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state != Movie::INACTIVE) {
    movie->state = Movie::INACTIVE;
    if (movie->video == PLAYER_MPG) CloseMPEG();
    if (movie->cachempeg) delete[] movie->cachempeg;
    File::closeFile(movie->fp);
  }
  movie->disablePermanentMovement();
  if (movie->imgmpeg) delete[] movie->imgmpeg; movie->imgmpeg = NULL;
  if (movie->pixmap) delete[] movie->pixmap; movie->pixmap = NULL;
  if (movie->pixtex) delete[] movie->pixtex; movie->pixtex = NULL;
  movie->fp = NULL;
}

void Movie::pause(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state == Movie::PLAYING || movie->state == Movie::LOOP) movie->state = Movie::PAUSE;
  else if (movie->state == Movie::PAUSE) movie->state = Movie::PLAYING;
}

void Movie::rewind(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state != Movie::PLAYING && movie->state != Movie::LOOP && movie->fp != NULL) {
    if (movie->video == PLAYER_MPG) RewindMPEG(movie->fp, movie->imgmpeg);
    movie->frame = 0;
    movie->begin = true;
    movie->state = Movie::PLAYING;
  }
}

void Movie::loop(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state != Movie::INACTIVE) movie->state = Movie::LOOP;
}

bool Movie::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Movie::quit()
{
  stop(this, NULL, 0L, 0L);
  flushMySqlPosition();
}

void Movie::funcs()
{
#if 0
  setActionFunc(MOVIE_TYPE, 0, WO_ACTION play, (const char *) uitem(ulabel(g.theme.Playvideo)));
#else
  setActionFunc(MOVIE_TYPE, 0, WO_ACTION play, "Play");
#endif
  setActionFunc(MOVIE_TYPE, 1, WO_ACTION stop, "Stop");
  setActionFunc(MOVIE_TYPE, 2, WO_ACTION pause, "Pause");
  setActionFunc(MOVIE_TYPE, 3, WO_ACTION loop, "Loop");
  setActionFunc(MOVIE_TYPE, 4, WO_ACTION rewind, "Rewind");
  setActionFunc(MOVIE_TYPE, 5, WO_ACTION moveObject, "Move");
}
