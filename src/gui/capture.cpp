//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       http://vreng.enst.fr/
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
/*
 *  capture.cpp : captures for the VREng GUI
 *  VREng Project
 *  Author: Philippe Dax - Telecom-ParisTech Paris
 */
#include "vreng.hpp"
#include "widgets.hpp"
#include "capture.hpp"
#include "pref.hpp"
#include "sound.hpp"	// playSound
#include "world.hpp"
#include "ogl.hpp"	// copyPixels
#include "img.hpp"	// saveJPG
#include "render.hpp"	// sharedRender
#include "timer.hpp"
#include "file.hpp"	// openFile
#if WANT_GL2PS
# include "gl2ps.h"	// GL2PS_*
#endif

Capture::Capture() 
: is_capturing(false), capture_no(0) {
}

/** capture JPEG (only 3D) */
void Capture::captureGl2jpg(const char *filename)
{
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLint w = vp[2] -2;	// I don't know why -2, but the result is correct
  GLint h = vp[3];

  uint8_t *pixels = Ogl::copyPixels(w, h, GL_FRONT);

  // save in a jpeg file
  int jpegquality = 75;	// range 1 to 100
  Img::saveJPG(filename, w, h, jpegquality, pixels);

  if (pixels) delete[] pixels;
  notice("capture %dx%d done in file %s", w, h, filename);
}

/** capture PNG (only 3D) */
void Capture::captureGl2png(const char *filename)
{
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLint w = vp[2] -2;	// I don't know why -2, but the result is correct
  GLint h = vp[3];

  // save in a png file
  Img::savePNG(filename, w, h);

  notice("capture %dx%d done in file %s", w, h, filename);
}

/** capture XWD (whole window) */
void Capture::captureXwd(const char *ext)
{
  char cmd[BUFSIZ];

  const char *name = World::current()->getName();
  sprintf(cmd, "IFS=' '; xwd -name vreng >/tmp/%s.xwd; convert /tmp/%s.xwd %s.%s; rm -f /tmp/%s.xwd", name, name, name, ext, name);
  notice("capture done in file %s.%s", name, ext);
  system(cmd);
}

#if WANT_GL2PS
/** capture and save 3D area using gl2ps API */
void Capture::captureGl2ps(const char *ext)
{
  FILE *fp;
  GLint buffsize = 0, state = GL2PS_OVERFLOW;
  GLuint form;
  char name[World::WORLD_LEN];

  if      (!strcmp(ext, "ps"))  form = GL2PS_PS;
  else if (!strcmp(ext, "eps")) form = GL2PS_EPS;
  else if (!strcmp(ext, "pdf")) form = GL2PS_PDF;
  else if (!strcmp(ext, "svg")) form = GL2PS_SVG;
  else return;

  sprintf(name, "%s.%s", World::current()->getName(), ext);
  if ((fp = File::openFile(name, "wb")) == 0) { perror("open"); return; }

  for (int i=1; state == GL2PS_OVERFLOW; i++) {
    buffsize += 512*512;
    gl2psBeginPage(name, PACKAGE_STRING, NULL, form,
                   GL2PS_SIMPLE_SORT,	// GL2PS_BSP_SORT,
                   GL2PS_SILENT |
                   GL2PS_BEST_ROOT |
                   GL2PS_OCCLUSION_CULL |
                   GL2PS_NO_BLENDING |
                   GL2PS_DRAW_BACKGROUND |
                   GL2PS_USE_CURRENT_VIEWPORT |
                   GL2PS_SIMPLE_LINE_OFFSET,
                   GL_RGBA, 0, NULL, 0, 0, 0, buffsize, fp, name);
    Render::sharedRender()->rendering();
    state = gl2psEndPage();
  }
  File::closeFile(fp);
  notice("capture done in file %s", name);
}

/** callback capture PS */
void Capture::captureImagePS()
{
  captureGl2ps("ps");
}

/** callback capture EPS */
void Capture::captureImageEPS()
{
  captureGl2ps("eps");
}

/** callback capture PDF */
void Capture::captureImagePDF()
{
  captureGl2ps("pdf");
}

/** callback capture SVG */
void Capture::writeSVGImage()
{
  captureGl2ps("svg");
}
#endif

/** callback capture JPG */
void Capture::writeJPGImage()
{
  char filename[World::WORLD_LEN + 4];

  sprintf(filename, "%s.jpg", World::current()->getName());
  g.render.setFlash();	// flash effect
  Sound::playSound(CAMERASND);  // sound effect
  captureGl2jpg(filename);
}

/** callback capture GIF */
void Capture::writeGIFImage()
{
  Sound::playSound(CAMERASND);
  captureXwd("gif");
}

/** callback capture PNG */
void Capture::writePNGImage()
{
  char filename[World::WORLD_LEN + 4];

  Sound::playSound(CAMERASND);
  sprintf(filename, "%s.png", World::current()->getName());
  captureGl2png(filename);
}

// start capture video 
void Capture::startVideo()
{
  is_capturing = true;
  capture_no =0;
  notice("sequence capture starts");
  ::g.times.capture.start();
}

// stop capture video
void Capture::stopVideo()
{
  char jpegbase[16];
  char swfname[World::WORLD_LEN + 4];  // video flash (swf)
  char cmd[BUFSIZ];
  
  int rate = int(ceil( float(MAX_CAPTURE_COUNT) / (time_t) ::g.times.capture.stop()));
  
  notice("sequence capture stops at %d images, rate=%d", capture_no + 1, rate);
  sprintf(jpegbase, "/tmp/vreng-");
  sprintf(swfname, "%s.swf", World::current()->getName());
  sprintf(cmd, "IFS=' '; jpeg2swf -v -q 50 -r %d -o %s %s*.jpg; rm -f %s*.jpg", rate, swfname, jpegbase, jpegbase);
  system(cmd);
  notice("file %s created", swfname);
  is_capturing = false;
  capture_no =0;
}

// capture video 
void Capture::writeVideoFrame()
{
  char filename[64];

  sprintf(filename, "/tmp/vreng-%03d.jpg", capture_no);
  captureGl2jpg(filename);
  capture_no++;
  if (capture_no >= MAX_CAPTURE_COUNT) stopVideo();
}
