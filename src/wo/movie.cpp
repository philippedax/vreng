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
#include "theme.hpp"	// theme.playvideo
#include "avi.hpp"	// Avi
#if HAVE_LIBMPEG
#include <mpeg.h>	// /usr/local/include
#else
#include "mpeg.h"	// local: src/bundle/mpeg_lib
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
  texid = -1;
  rate = FPS;
  pixmap = NULL;
  pixtex = NULL;
  avi = NULL;
  video = 0;
  filempeg = NULL;
  imgmpeg = NULL;
  colormap = NULL;
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
  setRenderPrior(PRIOR_MEDIUM);
}

void Movie::init()
{
  initMobileObject(0);
}

Movie::Movie(char *l)
{
  parser(l);
  behavior();
  init();
}

int Movie::inits()
{
  texid = Texture::getIdByObject(this);		// bad texid returned FIXME!
  trace(DBG_WO, "texid=%d (%s)", texid, Texture::getUrlById(texid));

  video = Format::getPlayerByUrl(names.url);
  switch (video) {
    case PLAYER_MPG: return mpegInit();
    case PLAYER_AVI: return aviInit();
    default: return 0;
  }
}

void mpegAbort(int sig)
{
  CloseMPEG();
}

int Movie::aviInit()
{
  if (avi) return 0;

  avi = new Avi(names.url);	// downloads avi file

  int r = avi->read_header();
  if (r) {
    error("avi: no header err=%d", r);
    delete avi;
    avi = NULL;
    return r;
  }
  fp = avi->getFile();
  avi->getInfos(&width, &height, &fps);
  trace(DBG_WO, "avi: w=%d h=%d f=%.3f", width, height, fps);
  pixmap = new uint8_t[3 * width * height];
  begin = true;
  frame = 0;

  /* compute texsz */
  int i, power = 0;
  texsz = MAX(width, height);
  while ((texsz = texsz/2) > 0) power++;
  for (i=0, texsz=1; i <= power; i++) texsz *= 2;
  //error("avi: texsz=%d", texsz);

  /* get memory for tex pixmap */
  pixtex = new GLubyte[3 * texsz * texsz];

  return r;
}

int Movie::mpegInit()
{
  /* download Mpeg file */
  filempeg = new char[MAXHOSTNAMELEN];
  if (Cache::download(names.url, filempeg) == 0) {
    error("can't download %s", filempeg);
    delete[] filempeg;
    return 0;
  }
  if ((fp = File::openFile(filempeg, "r")) == NULL) {
    error("can't open mpeg");
    delete[] filempeg;
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
  ColormapEntry *colormap = imgmpeg->Colormap;
  pixmap = new uint8_t[imgmpeg->Size];

  /* compute texsz */
  int i, power = 0;
  texsz = MAX(width, height);
  while ((texsz = texsz/2) > 0) power++;
  for (i=0, texsz=1; i <= power; i++)
    texsz *= 2;

  /* get memory for tex pixmap */
  pixtex = new GLubyte[3 * texsz * texsz];

  frame = 0;
  begin = true;
  signal(SIGABRT, mpegAbort);

  return 1;
}

void Movie::changePermanent(float lasting)
{
  static int n = 0;

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
  //error("frame = %d inter = %d", frame, inter);

  for (int f=0; f < (frame - inter); f++) {
    /* get video frame or not */
    if (f >= (int) fps) break;

    switch (video) {

    case PLAYER_AVI:
      {
        if (avi) {
          int r, l;

          r = avi->read_data(pixmap, width * height * 3, &l); // get Avi frame
          error("n=%d l=%d", n, l);
          if (r == 0) {
            error("end of avi video");
            File::closeFile(fp);
            state = INACTIVE;
            fp = NULL;
            delete avi;
            avi = NULL;
            begin = true;
            return;
          }
          n++;
          int wof = (texsz - width) / 2;
          int hof = (texsz - height) / 2;
          for (int h=0; h < height; h++) {
            for (int w=0; w < width; w++) {
              int u = 3 * (texsz * (h+hof) + (w+wof));
              int v = 3 * (width * h + w);
              if (File::littleEndian()) {
                pixtex[u+0] = pixmap[v+0];
                pixtex[u+1] = pixmap[v+1];
                pixtex[u+2] = pixmap[v+2];
              }
              else { // RGB = BGR
                pixtex[u+0] = pixmap[v+2];
                pixtex[u+1] = pixmap[v+1];
                pixtex[u+2] = pixmap[v+0];
              }
            }
          }
        }
        else
          return;
      }
      break;

    case PLAYER_MPG:
      {
        if (GetMPEGFrame((char *)pixmap) == false) {	// get mpeg frame
          if (state == LOOP) {
            RewindMPEG(fp, imgmpeg);
            begin = true;
            return;
          }
          CloseMPEG();
          state = INACTIVE;
          return;
        }
        n++;
        int wof = (texsz - width) / 2;
        int hof = (texsz - height) / 2;
        trace(DBG_WO, "frame=%d n=%d id=%d sz=%d w=%d h=%d", frame,n,texid,texsz,width,height);

        if (colormap) {	// case of Colormap Index
          for (int h=0; h < height; h++) {
            for (int w=0; w < width; w++) {
              int index = pixmap[h * width + w];
              ColormapEntry *color = &colormap[index];
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
                pixtex[u+0] = pixmap[v+0];
                pixtex[u+1] = pixmap[v+1];
                pixtex[u+2] = pixmap[v+2];
              }
              else { // RGB = BGR
                pixtex[u+0] = pixmap[v+2];
                pixtex[u+1] = pixmap[v+1];
                pixtex[u+2] = pixmap[v+0];
              }
            }
          }
        }
      } //end mpeg
      break;
    default: break;
    } //end switch
  } //end for

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
    if (! movie->inits()) return;
  }
}

void Movie::stop(Movie *movie, void *d, time_t s, time_t u)
{
  if (movie->state != Movie::INACTIVE) {
    movie->state = Movie::INACTIVE;
    if (movie->video == PLAYER_MPG)
      CloseMPEG();
    if (movie->filempeg) delete[] movie->filempeg;
    File::closeFile(movie->fp);
  }
  movie->disablePermanentMovement();
  if (movie->imgmpeg) delete[] movie->imgmpeg;
  movie->imgmpeg = NULL;
  if (movie->pixmap) delete[] movie->pixmap;
  movie->pixmap = NULL;
  if (movie->pixtex) delete[] movie->pixtex;
  movie->pixtex = NULL;
  movie->fp = NULL;
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
    if (movie->video == PLAYER_MPG)
      RewindMPEG(movie->fp, movie->imgmpeg);
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
    if (! movie->inits()) return;
  }
}

bool Movie::whenIntersect(WObject *pcur, WObject *pold)
{
  pold->copyPositionAndBB(pcur);
  return true;
}

void Movie::quit()
{
  stop(this, NULL, 0L, 0L);
  savePersistency();
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
