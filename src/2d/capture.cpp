//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)       https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// capture.cpp
//
// Captures for the VREng GUI
// Author: Philippe Dax
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "capture.hpp"
#include "world.hpp"	// current
#include "ogl.hpp"	// copyPixels
#include "img.hpp"	// saveJPG
#include "render.hpp"	// sharedRender
#include "timer.hpp"	// ::g.timer
#include "file.hpp"	// open, close
#include "sound.hpp"	// playSound
#if HAVE_GL2PS
# include "gl2ps.h"	// GL2PS_*
#endif


Capture::Capture() :
 is_capturing(false),
 capture_no(0)
{
}

/** capture JPEG (only 3D scene) */
void Capture::captureGl2JPG(const char *filename)
{
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLint w = vp[2];
  GLint h = vp[3];

  uint8_t *pixels = Ogl::copyPixels(w, h, GL_FRONT);

  // save in a jpeg file
  int jpegquality = 100;	// range 1 to 100
  Img::saveJPG(filename, w, h, jpegquality, pixels);

  if (pixels) delete[] pixels;
  echo("capture %dx%d done in file %s", w, h, filename);
}

/** capture PNG (only 3D scene) */
void Capture::captureGl2PNG(const char *filename)
{
  GLint vp[4];

  glGetIntegerv(GL_VIEWPORT, vp);
  GLint w = vp[2];
  GLint h = vp[3];

  // save in a png file
  Img::savePNG(filename, w, h);

  echo("capture %dx%d done in file %s", w, h, filename);
}

/** capture XWD (whole window) */
void Capture::captureXwd(const char *ext)
{
  char cmd[128];

  const char *worldname = World::current()->name;
  sprintf(cmd, "IFS=' '; xwd -name vreng >/tmp/%s.xwd; convert /tmp/%s.xwd %s.%s; rm -f /tmp/%s.xwd", worldname, worldname, worldname, ext, worldname);
  system(cmd);
  echo("capture done in file %s.%s", worldname, ext);
}

#if HAVE_GL2PS
/** capture and save 3D area using gl2ps API */
void Capture::captureGl2PS(const char *ext)
{
  GLint bufsize = 0, state = GL2PS_OVERFLOW;
  GLuint form;
  char worldname[World::WORLD_LEN];

  if      (!strcmp(ext, "ps"))  form = GL2PS_PS;
  else if (!strcmp(ext, "eps")) form = GL2PS_EPS;
  else if (!strcmp(ext, "pdf")) form = GL2PS_PDF;
  else if (!strcmp(ext, "svg")) form = GL2PS_SVG;
  else return;

  sprintf(worldname, "%s.%s", World::current()->name, ext);

  FILE *fp;
  File *file = new File();
  if ((fp = file->open(worldname, "wb")) == 0) {
    perror("open");
    return;
  }

  for (int i=1; state == GL2PS_OVERFLOW; i++) {
    bufsize += 512*512;
    gl2psBeginPage(worldname, PACKAGE_STRING, NULL, form,
                   GL2PS_SIMPLE_SORT,	// GL2PS_BSP_SORT,
                   GL2PS_SILENT |
                   GL2PS_BEST_ROOT |
                   GL2PS_OCCLUSION_CULL |
                   GL2PS_NO_BLENDING |
                   GL2PS_DRAW_BACKGROUND |
                   GL2PS_USE_CURRENT_VIEWPORT |
                   GL2PS_SIMPLE_LINE_OFFSET,
                   GL_RGBA, 0, NULL, 0, 0, 0, bufsize, fp, worldname);
    Render::sharedRender()->rendering();
    state = gl2psEndPage();
  }
  file->close();
  delete file;
  echo("capture done in file %s", worldname);
}

/** callback capture PS */
void Capture::captureImagePS()
{
  captureGl2PS("ps");
}

/** callback capture EPS */
void Capture::captureImageEPS()
{
  captureGl2PS("eps");
}

/** callback capture PDF */
void Capture::captureImagePDF()
{
  captureGl2PS("pdf");
}

/** callback capture SVG */
void Capture::writeSVGImage()
{
  captureGl2PS("svg");
}
#endif //GL2PS


/** callback capture JPG */
void Capture::writeJPGImage()
{
  char worldname[World::WORLD_LEN + 4];

  sprintf(worldname, "%s.jpg", World::current()->name);
  ::g.render.setFlash();	// flash effect
  Sound::playSound(CAMERASND);  // sound effect
  captureGl2JPG(worldname);
}

/** callback capture Window */
void Capture::writeWINImage()
{
  Sound::playSound(CAMERASND);
  captureXwd("jpg");
}

/** callback capture PNG */
void Capture::writePNGImage()
{
  char worldname[World::WORLD_LEN + 4];

  Sound::playSound(CAMERASND);
  sprintf(worldname, "%s.png", World::current()->name);
  captureGl2PNG(worldname);
}

/** Start capture video */
void Capture::startVideo()
{
  is_capturing = true;
  capture_no = 0;
  echo("sequence capture starts");
  ::g.timer.capture.start();
}

/** Check capture video */
bool Capture::isCapturingVideo() const
{
  return is_capturing;
}

/** Stop capture video */
void Capture::stopVideo()
{
  char jpegbase[16];
  char cmd[256];
  char swfname[World::WORLD_LEN + 4];  // video flash (swf)
  
  int rate = int(ceil( float(MAX_CAPTURE_COUNT) / (time_t) ::g.timer.capture.stop()));
  
  echo("sequence capture stops at %d images, rate=%d", capture_no + 1, rate);
  sprintf(jpegbase, "/tmp/vreng-");
  sprintf(swfname, "%s.swf", World::current()->name);
  sprintf(cmd, "IFS=' '; jpeg2swf -v -q 50 -r %d -o %s %s*.jpg; rm -f %s*.jpg", rate, swfname, jpegbase, jpegbase);
  system(cmd);
  echo("file %s created", swfname);
  is_capturing = false;
  capture_no = 0;
}

/** Write video */
void Capture::writeVideo()
{
  char worldname[64];

  sprintf(worldname, "/tmp/vreng-%03d.jpg", capture_no);
  captureGl2JPG(worldname);
  capture_no++;
  if (capture_no >= MAX_CAPTURE_COUNT)
    stopVideo();
}
