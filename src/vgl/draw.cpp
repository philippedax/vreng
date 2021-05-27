//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2012 Philippe Dax
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
#include "draw.hpp"
#include "render.hpp"	// render
#include "color.hpp"	// Color


/* Draws vertex */
void Draw::vertex3fv(const GLfloat *v)
{
  ::g.render.setBB(v);		//update bbin & bbmax
  glVertex3f(v[0], v[1], v[2]);
}

void Draw::vertex3f(GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat v[3];

  v[0] = x; v[1] = y; v[2] = z;
  vertex3fv(v);
}

/* Draws point */
void Draw::point(GLfloat x, GLfloat y, GLfloat z)
{
  glBegin(GL_POINTS);
   glVertex3f(x, y, z);
  glEnd();
}

/* Draws line */
void Draw::line(GLfloat l, GLfloat w = 0)
{
  glEnable(GL_LINE_SMOOTH);  // anti aliasing
  if (!w) w = 1;
  glLineWidth(w);
  glBegin(GL_LINES);
   glVertex2f(0, 0);
   glVertex2f(0, l);
  glEnd();
  glDisable(GL_LINE_SMOOTH);
}

/* Draws rectangle */
void Draw::rect(GLfloat w, GLfloat h, uint8_t style, GLfloat rtxs, GLfloat rtxt)
{
  if ((rtxs > 1) || (rtxt > 1)) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  switch (style) {
    case STYLE_FILL:   glBegin(GL_QUADS); break;
    case STYLE_LINES:  glBegin(GL_LINE_LOOP); break;
    case STYLE_POINTS: glBegin(GL_POINTS); break;
  }
   glTexCoord2f(0, 0);
   glVertex3f(-w, -h, 0);
   glTexCoord2f(rtxs, 0);
   glVertex3f(w, -h, 0);
   glTexCoord2f(rtxs, rtxt);
   glVertex3f(w, h, 0);
   glTexCoord2f(0, rtxt);
   glVertex3f(-w, h, 0);
  glEnd();
}

/* Draws triangle */
void Draw::triangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
  glBegin(GL_POLYGON);
   glVertex3f(x1, y1, z1);
   glVertex3f(x2, y2, z2);
   glVertex3f(x3, y3, z3);
  glEnd();
}

/* Draws ellipse */
void Draw::ellipse(GLfloat xr, GLfloat yr, uint8_t style)
{
  switch (style) {
  case STYLE_FILL:
    glBegin(GL_TRIANGLE_FAN);
     glVertex2f(0, 0);
     break;
  case STYLE_LINES:
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
     break;
  case STYLE_POINTS:
    glPointSize(1);
    glBegin(GL_POINTS);
     break;
  }
   for (int i=360; i>=0; i -=5)
     glVertex2f(cos(DEG2RAD(i))*xr, sin(DEG2RAD(i))*yr);
  glEnd();
}

/*
 * Draws a box (taken from Mesa)
 */
void Draw::box(GLfloat width, GLfloat depth, GLfloat height, const int textures[], GLfloat rtx[6][2], uint8_t style)
{
  GLfloat x1 = -width/2;
  GLfloat x2 = width/2;
  GLfloat y1 = -depth/2;
  GLfloat y2 = depth/2;
  GLfloat z1 = -height/2;
  GLfloat z2 = height/2;

  static const GLfloat normals[6][3] = {
    { -1,  0,  0 },
    {  0,  1,  0 },
    {  1,  0,  0 },
    {  0, -1,  0 },
    {  0,  0,  1 },
    {  0,  0, -1 }
  };
  static const int faces[6][4] = {
    { 0, 1, 2, 3 },
    { 3, 2, 6, 7 },
    { 7, 6, 5, 4 },
    { 4, 5, 1, 0 },
    { 5, 6, 2, 1 },
    { 7, 4, 0, 3 }
  };
  // Repeat Texture: GLfloat rtx[6][2] is an array of GLfloat containing
  // nb. repeat for each texture's s-axis and t-axis
  GLfloat texcoords[6][4][2] = {
   { {0        ,rtx[0][1]},
     {0        ,0}        ,
     {rtx[0][0],0}        ,
     {rtx[0][0],rtx[0][1]} },
   { {rtx[1][0],rtx[1][1]},
     {rtx[1][0],0}        ,
     {0        ,0}        ,
     {0        ,rtx[1][1]} },
   { {0        ,rtx[2][1]},
     {0        ,0}        ,
     {rtx[2][0],0}        ,
     {rtx[2][0],rtx[2][1]} },
   { {rtx[3][0],rtx[3][1]},
     {rtx[3][0],0}        ,
     {0        ,0}        ,
     {0        ,rtx[3][1]} },
   { {0        ,0}        ,
     {0        ,rtx[4][1]},
     {rtx[4][0],rtx[4][1]},
     {rtx[4][0],0}         },
   { {0        ,0}        ,
     {0        ,rtx[5][1]},
     {rtx[5][0],rtx[5][1]},
     {rtx[5][0],0}         }
  };

  GLfloat v[8][3], tmp;
  if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
  if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }
  if (z1 > z2) { tmp = z1; z1 = z2; z2 = tmp; }

  v[0][0] = v[1][0] = v[2][0] = v[3][0] = x1;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = x2;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = y1;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = y2;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = z1;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = z2;

  int drawtype;
  switch (style) {
    case STYLE_LINES:  drawtype = GL_LINE_LOOP; break;
    case STYLE_POINTS: drawtype = GL_POINTS; break;
    default:           drawtype = GL_QUADS; break;
  }
   for (int i=0; i<6; i++) {
     if (textures[i] > 0) {
       glEnable(GL_TEXTURE_2D);
       glBindTexture(GL_TEXTURE_2D, textures[i]);
       if ((rtx[i][0] > 1) || (rtx[i][1] > 1)) {
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       }
     }
     glBegin(drawtype);
     for (int j=0; j<4; j++) {
       if (textures[i] > 0)
         glTexCoord2fv(&texcoords[i][j][0]);
       glNormal3fv(&normals[i][0]);
       vertex3fv(&v[faces[i][j]][0]);
     }
    glEnd();

    if (textures[i] > 0)
      glDisable(GL_TEXTURE_2D);
  }
}

void Draw::box(GLfloat width, GLfloat depth, GLfloat height)
{
  GLfloat x = width/2, y = height/2, z = depth/2;
  int j=0;

  for (int i=0; i<4; i++) {
    glRotatef(90, 0, 0, 1);
    if (j) {
      glBegin(GL_QUADS);
       glNormal3f(-1, 0, 0);
       glVertex3f(-x, y, z);
       glVertex3f(-x, -y, z);
       glVertex3f(-x, -y, -z);
       glVertex3f(-x, y, -z);
      glEnd();
      glBegin(GL_TRIANGLES);
       glNormal3f(0, 0, 1);
       glVertex3f(0, 0, z);
       glVertex3f(-x, y, z);
       glVertex3f(-x, -y, z);
       glNormal3f(0, 0, -1);
       glVertex3f(0, 0, -z);
       glVertex3f(-x, -y, -z);
       glVertex3f(-x, y, -z);
      glEnd();
      j = 0;
    } else {
      glBegin(GL_QUADS);
       glNormal3f(-1, 0, 0);
       glVertex3f(-y, x, z);
       glVertex3f(-y, -x, z);
       glVertex3f(-y, -x, -z);
       glVertex3f(-y, x, -z);
      glEnd();
      glBegin(GL_TRIANGLES);
       glNormal3f(0, 0, 1);
       glVertex3f(0, 0, z);
       glVertex3f(-y, x, z);
       glVertex3f(-y, -x, z);
       glNormal3f(0, 0, -1);
       glVertex3f(0, 0, -z);
       glVertex3f(-y, -x, -z);
       glVertex3f(-y, x, -z);
      glEnd();
      j = 1;
    }
  }
}

/*
 * Draws a bounding box
 */
void Draw::bbox(GLfloat width, GLfloat depth, GLfloat height)
{
  glLineWidth(2);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0x3333);
  glColor3f(1, 0, 1);		// magenta

  Draw::box(width, depth, height);

  glDisable(GL_LINE_STIPPLE);
}

void Draw::pyramid(GLfloat _width, GLfloat height, uint8_t style)
{
  GLfloat width = _width/2;

  if (style == STYLE_FILL) glBegin(GL_TRIANGLE_FAN);
  else                     glBegin(GL_LINE_LOOP);
   glVertex3f(0, 0, height);
   glVertex3f(-width, -width, 0);
   glVertex3f(+width, -width, 0);
   glVertex3f(+width, +width, 0);
   glVertex3f(-width, +width, 0);
   glVertex3f(-width, -width, 0);
  glEnd();

  if (style == STYLE_FILL) glBegin(GL_QUADS);
  else                     glBegin(GL_LINE_LOOP);
   glVertex3f(-width, -width, 0);
   glVertex3f(+width, -width, 0);
   glVertex3f(+width, +width, 0);
   glVertex3f(-width, +width, 0);
  glEnd();
}

void Draw::octagon(GLfloat width, GLfloat height, uint8_t style)
{
  GLfloat x = width * sin(M_PI_4), y = width/2., z = height/2., c;

  c = x + y;
  for (int j=0; j<8; j++) {
    glTranslatef(-c, 0, 0);
    if (style != STYLE_FILL) glBegin(GL_LINE_LOOP);
    else                     glBegin(GL_QUADS);
     glNormal3f(-1, 0, 0);
     glVertex3f(0, -y, z);
     glVertex3f(0, y, z);
     glVertex3f(0, y, -z);
     glVertex3f(0, -y, -z);
    glEnd();
    glTranslatef(c, 0, 0);
    if (style == STYLE_FILL) {
      glBegin(GL_TRIANGLES);
       glNormal3f(0, 0, 1);
       glVertex3f(0, 0, z);
       glVertex3f(-c, -y, z);
       glVertex3f(-c, y, z);
       glNormal3f(0, 0, -1);
       glVertex3f(0, 0, -z);
       glVertex3f(-c, y, -z);
       glVertex3f(-c, -y, -z);
      glEnd();
    }
    glRotatef(45, 0, 0, 1);
  }
}

/*
 * Torus drawing (taken from Mesa)
 */
void Draw::torus(GLfloat rc, uint8_t numc, GLfloat rt, uint8_t numt, uint8_t style)
{
  for (int i=0; i < numc; i++) {
    float s, t, x, y, z;

    if (style == STYLE_FILL) glBegin(GL_QUAD_STRIP);
    else                     glBegin(GL_LINE_STRIP);
     for (int j=0; j <= numt; j++) {
       for (int k=1; k >= 0; k--) {
         s = (i + k) % numc + 0.5;
         t = j % numt;
         x = cos(t*M_2PI/numt) * cos(s*M_2PI/numc);
         y = sin(t*M_2PI/numt) * cos(s*M_2PI/numc);
         z = sin(s*M_2PI/numc);
         glNormal3f(x, y, z);

         x = (rt + rc * cos(s*M_2PI/numc)) * cos(t*M_2PI/numt);
         y = (rt + rc * cos(s*M_2PI/numc)) * sin(t*M_2PI/numt);
         z = rc * sin(s*M_2PI/numc);
         vertex3f(x, y, z);
       }
     }
    glEnd();
  }
}

/*
 * Sphere (taken from Mesa)
 */
void Draw::sphere(GLfloat r, uint8_t slices, uint8_t stacks, uint8_t style)
{
  GLfloat rho, drho, theta, dtheta;
  GLfloat x, y, z;
  GLfloat s, t, ds, dt;

  drho = M_PI / (GLfloat) stacks;
  dtheta = M_2PI / (GLfloat) slices;

  // draw +Z end as a triangle fan
  glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.5, 0);
   vertex3f(0, 0, r);
   for (int j=0; j <= slices; j++) {
     theta = (j==slices) ? 0 : j*dtheta;
     x = -sin(theta) * sin(drho);
     y = cos(theta) * sin(drho);
     z = cos(drho);
     glNormal3f(x, y, z);
     vertex3f(x*r, y*r, z*r);
   }
  glEnd();

  ds = 1. / slices;
  dt = 1. / stacks;
  t = 1;  // because loop now runs from 0

  // draw intermediate stacks as quad strips
  for (int i=0; i < stacks; i++) {
     rho = i * drho;
     glBegin(GL_QUAD_STRIP);
      s = 0;
      for (int j=0; j <= slices; j++) {
        theta = (j==slices) ? 0 : j*dtheta;
        x = -sin(theta) * sin(rho);
        y = cos(theta) * sin(rho);
        z = cos(rho);
        glNormal3f(x, y, z);
        glTexCoord2f(s, 1-t);
        vertex3f(x*r, y*r, z*r);
        x = -sin(theta) * sin(rho+drho);
        y = cos(theta) * sin(rho+drho);
        z = cos(rho+drho);
        glNormal3f(x, y, z);
        glTexCoord2f(s, 1-(t-dt));
        s += ds;
        vertex3f(x*r, y*r, z*r);
     }
    glEnd();
    t -= dt;
  }
  // draw -Z end as a triangle fan
  glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0.5, 1);
   vertex3f(0, 0, -r);
   rho = M_PI - drho;
   s = 1;
   t = dt;
   for (int j=slices; j >= 0; j--) {
     theta = (j==slices) ? 0 : j*dtheta;
     x = -sin(theta) * sin(rho);
     y = cos(theta) * sin(rho);
     z = cos(rho);
     glNormal3f(x, y, z);
     glTexCoord2f(s, 1-t);
     s -= ds;
     vertex3f(x*r, y*r, z*r);
   }
  glEnd();
}

/*
 * Cylinder (taken from Mesa, Brian Paul)
 */
void Draw::cylinder(GLfloat baseR, GLfloat topR, GLfloat height, uint8_t slices, uint8_t stacks, uint8_t style)
{
  GLfloat da, r, dr, dz;
  GLfloat z, nz;
  GLfloat ds = 1. / slices;
  GLfloat dt = 1. / stacks;
  GLfloat t = 0;

  da = M_2PI / slices;
  dr = (topR-baseR) / stacks;
  dz = height / stacks;
  nz = (baseR-topR) / height;  // Z component of normal vectors

  switch (style) {
  case STYLE_FILL:
    {
      z = 0;
      r = baseR;
      for (int j=0; j < stacks; j++) {
        GLfloat s = 0;
        glBegin(GL_QUAD_STRIP);
        for (int i=0; i <= slices; i++) {
          GLfloat x, y;
          if (i == slices) {
            x = sin(0.0);
            y = cos(0.0);
          }
          else {
            x = sin(i*da);
            y = cos(i*da);
          }
          glNormal3f(x, y, nz);
          glTexCoord2f(s, t);
          vertex3f(x*r, y*r, z);
          glNormal3f(x, y, nz);
          glTexCoord2f(s, t+dt);
          vertex3f(x*(r+dr), y*(r+dr), z+dz);
          s += ds;
        }
        glEnd();
        r += dr;
        t += dt;
        z += dz;
      }
    }
    break;
  case STYLE_LINES:
    {
      z = 0;
      r = baseR;
      for (int j=0; j <= stacks; j++) {
        glBegin(GL_LINE_LOOP);
        for (int i=0; i < slices; i++) {
          GLfloat x = cos(i*da);
          GLfloat y = sin(i*da);
          glNormal3f(x, y, nz);
          glVertex3f(x*r, y*r, z);
        }
        glEnd();
        z += dz;
        r += dr;
      }
      /* draw length lines */
      glBegin(GL_LINES);
      for (int i=0;i<slices;i++) {
        GLfloat x = cos(i*da);
        GLfloat y = sin(i*da);
        glNormal3f(x, y, nz);
        glVertex3f(x*baseR, y*baseR, 0);
        glVertex3f(x*topR, y*topR, height);
      }
      glEnd();
    }
    break;
  case STYLE_POINTS:
    {
      glBegin(GL_POINTS);
      for (int i=0; i < slices; i++) {
        GLfloat x = cos(i*da);
        GLfloat y = sin(i*da);
        glNormal3f(x, y, nz);
        z = 0;
        r = baseR;
        for (int j=0; j <= stacks; j++) {
          glVertex3f(x*r, y*r, z);
          z += dz;
          r += dr;
        }
      }
      glEnd();
    }
    break;
  }
}

/*
 * Disk (taken from Mesa, Brian Paul)
 */
void Draw::disk(GLfloat innerR, GLfloat outerR, uint8_t slices, uint8_t loops, uint8_t style)
{
  /* Normal vectors */
  glNormal3f(0, 0, -1);

  GLfloat da = M_2PI / slices;
  GLfloat dr = (outerR-innerR) / (GLfloat) loops;

  switch (style) {
  case STYLE_FILL:
    {
      /* texture of a gluDisk is a cut out of the texture unit square
       * x, y in [-outerR, +outerR]; s, t in [0, 1] * (linear mapping)
       */
      GLfloat dtc = 2. * outerR;
      GLfloat sa, ca;
      GLfloat r1 = innerR;
      for (int l=0; l<loops; l++) {
        GLfloat r2 = r1 + dr;
        glBegin(GL_QUAD_STRIP);
        for (int s=slices; s>=0 ; s--) {
          GLfloat a;
          if (s == slices)
            a = 0;
          else
            a = s * da;
          sa = sin(a);
          ca = cos(a);
          glTexCoord2f(0.5-sa*r2/dtc, 0.5+ca*r2/dtc);
          glVertex3f(r2*sa, r2*ca, 0);
          glTexCoord2f(0.5-sa*r1/dtc, 0.5+ca*r1/dtc);
          glVertex3f(r1*sa, r1*ca, 0);
        }
        glEnd();
        r1 = r2;
      }
    }
    break;
  case STYLE_LINES:
    {
      // draw loops
      for (int l=0; l<=loops; l++) {
        GLfloat r = innerR + l * dr;
        glBegin(GL_LINE_LOOP);
        for (int s=0; s<slices; s++) {
          GLfloat a = s * da;
          glVertex3f(r*sin(a), r*cos(a), 0);
        }
        glEnd();
      }
      // draw spokes
      for (int s=0; s<slices; s++) {
        GLfloat a = s * da;
        GLfloat x = sin(a);
        GLfloat y = cos(a);
        glBegin(GL_LINE_STRIP);
          for (int l=0; l<=loops; l++) {
            GLfloat r = innerR + l * dr;
            glVertex3f(r*x, r*y, 0);
          }
        glEnd();
      }
    }
    break;
  case STYLE_POINTS:
    {
      glBegin(GL_POINTS);
      for (int s=0; s<slices; s++) {
        GLfloat a = s * da;
        GLfloat x = sin(a);
        GLfloat y = cos(a);
        for (int l=0; l<=loops; l++) {
          GLfloat r = innerR * l * dr;
          glVertex3f(r*x, r*y, 0);
        }
      }
      glEnd();
    }
    break;
  }
}

/* Draws a line in a display-list */
void Draw::ray(GLint *ray_dl, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, const GLfloat *color, GLint stipple)
{
  *ray_dl = glGenLists(1);
  glNewList(*ray_dl, GL_COMPILE);
   glLineWidth(1);
   glEnable(GL_LINE_SMOOTH);	// anti aliasing
   if (stipple) {
     glEnable(GL_LINE_STIPPLE);
     glLineStipple(1, stipple);
   }
   glBegin(GL_LINES);
    glColor3fv(color);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
   glEnd();
   if (stipple)
     glDisable(GL_LINE_STIPPLE);
   glDisable(GL_LINE_SMOOTH);
  glEndList();
}

void Draw::ray(GLint *ray_dl, GLfloat ex, GLfloat ey, GLfloat ez, GLfloat tx, GLfloat ty, GLfloat tz)
{
  const GLfloat *color = Color::green;

  Draw::ray(ray_dl, ex, ey, ez, tx, ty, tz, color, 0);
}

void Draw::helix(GLfloat r, GLfloat length, GLfloat height, GLfloat slices, GLfloat thick, GLfloat *color)
{
  GLfloat a, h;

  glBegin(GL_LINE_STRIP);
   glColor3fv(color);
   glLineWidth(thick);
   for (a=0, h=0; h < length; a += M_PI/180, h += height)
     glVertex3f(r * cos(a*slices), r * sin(a*slices), h);
   glLineWidth(1);
  glEnd();
}
