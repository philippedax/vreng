//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#include "ogl.hpp"


/** Copies 3D buffer into a RGB pixmap and returns this pixmap */
uint8_t * Ogl::copyPixels(GLint width, GLint height, GLenum mode)
{
  // Alloc pixel bytes
  int nbbytes = 3 * width * height;
  uint8_t *pixogl = new uint8_t[nbbytes];
  uint8_t *pixmap = new uint8_t[nbbytes];

  // Copy from OpenGL
  glReadBuffer(mode);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixogl);

  /* Swapping lines in the buffer
   * OpenGL writes from lower line going up
   * but ijl works reverse from up to down
   */
  for (int j=0; j < height; j++) { // lines
    for (int i=0; i < width; i++) { // columns
      pixmap[((j*width)+i)*3+0] = pixogl[((height-1-j)*width+i)*3+0];
      pixmap[((j*width)+i)*3+1] = pixogl[((height-1-j)*width+i)*3+1];
      pixmap[((j*width)+i)*3+2] = pixogl[((height-1-j)*width+i)*3+2];
    }
  }

  delete[] pixogl;
  return pixmap;
}

/** Check if an OpenGL extension is supported */
bool Ogl::isGLextension(const char *ext)
{
  const GLubyte *extentions = NULL;
  const GLubyte *beginext;
  GLubyte *currext, *endext;

  if (*ext == '\0') {
    return false;
  }
  if ((currext = (GLubyte *) strchr(ext, ' '))) {
    return false;
  }

  extentions = glGetString(GL_EXTENSIONS);
  beginext = extentions;
  for (;;) {
    currext = (GLubyte *) strstr((const char *) beginext, ext);
    if (! currext) {
      break;
    }
    endext = currext + strlen(ext);
    if (currext == beginext || *(currext - 1) == ' ') {
      if (*endext == ' ' || *endext == '\0') {
        return true;	// found
      }
    }
    beginext = endext;
  }
  return false;
}

/** Prints OpenGL infos */
void Ogl::infosGL()
{
  echo("GL_VERSION: %s", const_cast<unsigned char *>(glGetString(GL_VERSION)));
#if HAVE_LIBGLU
  echo("GLU_VERSION: %s", const_cast<unsigned char *>(gluGetString(GLU_VERSION)));
#endif
  echo("GL_RENDERER: %s", const_cast<unsigned char *>(glGetString(GL_RENDERER)));
  echo("GL_VENDOR: %s", const_cast<unsigned char *>(glGetString(GL_VENDOR)));

  GLint bits, lights, texsize;
  glGetIntegerv(GL_DEPTH_BITS, &bits);
  echo("GL_DEPTH_BITS: %d", bits);
  glGetIntegerv(GL_STENCIL_BITS, &bits);
  echo("GL_STENCIL_BITS: %d", bits);
  glGetIntegerv(GL_MAX_LIGHTS, &lights);
  echo("GL_MAX_LIGHTS: %d", lights);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texsize);
  echo("GL_MAX_TEXTURE_SIZE: %d", texsize);

  echo("GL_EXTENSIONS: %s", const_cast<unsigned char *>(glGetString(GL_EXTENSIONS)));
#if HAVE_LIBGLU
  echo("GLU_EXTENSIONS: %s", const_cast<unsigned char *>(gluGetString(GLU_EXTENSIONS)));
#endif
}

void Ogl::printGLMatrix()
{
  GLfloat matrix[16];

  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
  printf("Matrix:\n");
  for (int a=0; a<16; a++) {
    if (a % 4 == 0) printf("\n");
    printf("%.2f\t", *(matrix+ (a*4)%16 + a/4));
  }
  printf("\n");
}

void Ogl::printGLError()
{
  GLenum glerror = glGetError();
  if (glerror != GL_NO_ERROR) {
    error("glerror: %s", gluErrorString(glerror));
  }
}
