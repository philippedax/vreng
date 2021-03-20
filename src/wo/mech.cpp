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
/**
* program	: glutmech V1.1
* author	: Simon Parkinson-Bates.
* E-mail	: sapb@yallara.cs.rmit.edu.au
* Copyright Simon Parkinson-Bates.
* "source if freely avaliable to anyone to copy as long as they
*  acknowledge me in their work."
**/

#include "vreng.hpp"
#include "mech.hpp"
#include "draw.hpp"	// Draw


const OClass Mech::oclass(MECH_TYPE, "Mech", Mech::creator);

// local
/* motion variables */
static int leg = 0;
static int shoulder1 = 0, shoulder2 = 0, shoulder3 = 0, shoulder4 = 0;
static int lat1 = 20, lat2 = 20;
static int elbow1 = 0, elbow2 = 0;
static int pivot = 0;
static int tilt = 10;
static int ankle1 = 0, ankle2 = 0;
static int heel1 = 0, heel2 = 0;
static int hip11 = 0, hip12 = 10, hip21 = 0, hip22 = 10;
static int fire = 0;
static float elevation = 0, mframe = 3;
static GLint dlist = -1;

/* material definitions */
static GLfloat mat_specular[] = {.628281, .555802, .366065, 1};
static GLfloat mat_ambient[] = {.24725, .1995, .0745, 1};
static GLfloat mat_diffuse[] = {.75164, .60648, .22648, 1};
static GLfloat mat_shininess[] = {128 * .4};
static GLfloat mat_specular2[] = {.508273, .508273, .508373, 1};
static GLfloat mat_ambient2[] = {.19225, .19225, .19225, 1};
static GLfloat mat_diffuse2[] = {.50754, .50754, .50754, 1};
static GLfloat mat_shininess2[] = {128 * .6};
static GLfloat mat_specular3[] = {.296648, .296648, .296648, 1};
static GLfloat mat_ambient3[] = {.25, .20725, .20725, 1};
static GLfloat mat_diffuse3[] = {1, .829, .829, 1};
static GLfloat mat_shininess3[] = {128 * .088};
static GLfloat mat_specular4[] = {.633, .727811, .633, 1};
static GLfloat mat_ambient4[] = {.0215, .1745, .0215, 1};
static GLfloat mat_diffuse4[] = {.07568, .61424, .07568, 1};
static GLfloat mat_shininess4[] = {128 * .6};


void Mech::setMaterial(GLfloat spec[], GLfloat amb[], GLfloat diff[], GLfloat shin[])
{
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
  glMaterialfv(GL_FRONT, GL_SHININESS, shin);
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
}

void Mech::drawTorso()
{
  glNewList(dlist + TORSO, GL_COMPILE);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   Draw::box(1, 3, 1);
   glTranslatef(.75, 0, 0);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   Draw::box(.5, 2, .6);
   glTranslatef(-1.5, 0, 0);
   Draw::box(.5, 2, .6);
   glTranslatef(.75, 0, 0);
  glEndList();
}

void Mech::drawHip()
{
  glNewList(dlist + HIP, GL_COMPILE);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   Draw::octagon(.7, .5, 0);
   for (int i=0; i<2; i++) {
     if (i)
       glScalef(-1, 1, 1);
     glTranslatef(1, 0, 0);
     setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
     Draw::sphere(.2, 16, 16, 0);
     glTranslatef(-1, 0, 0);
   }
   glScalef(-1, 1, 1);
  glEndList();
}

void Mech::drawShoulder()
{
  glNewList(dlist + SHOULDER, GL_COMPILE);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   Draw::box(1, .5, .5);
   glTranslatef(.9, 0, 0);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   Draw::sphere(.6, 16, 16, 0);
   glTranslatef(-.9, 0, 0);
  glEndList();
}

void Mech::drawUpperArm()
{
  glNewList(dlist + UPPER_ARM, GL_COMPILE);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   Draw::box(1, 1, 2);
   glTranslatef(0, -.95, 0);
   glRotatef(90, 1, 0, 0);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   Draw::cylinder(.4, .4, 1.5, 16, 10, 0);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   glRotatef(-90, 1, 0, 0);
   glTranslatef(-.4, -1.85, 0);
   glRotatef(90, 0, 1, 0);
   for (int i=0; i<2; i++) {
     if (i)
       Draw::cylinder(.5, .5, .8, 16, 10, 0);
     else
       Draw::cylinder(.2, .2, .8, 16, 10, 0);
   }
   for (int i=0; i<2; i++) {
     if (i)
       glScalef(-1, 1, 1);
     if (i)
       glTranslatef(0, 0, .8);
     Draw::disk(.2, .5, 16, 10, 0);
     if (i)
       glTranslatef(0, 0, -.8);
   }
   glScalef(-1, 1, 1);
   glRotatef(-90, 0, 1, 0);
   glTranslatef(.4, 2.9, 0);
  glEndList();
}

void Mech::drawVulcanGun()
{
  glNewList(dlist + VULCAN, GL_COMPILE);
  setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
  Draw::cylinder(.5, .5, .5, 16, 10, 0);
  glTranslatef(0, 0, .5);
  Draw::disk(0, .5, 16, 10, 0);

  for (int i=0; i<5; i++) {
    glRotatef(72, 0, 0, 1);
    glTranslatef(0, .3, 0);
    Draw::cylinder(.15, .15, 2, 16, 10, 0);
    Draw::cylinder(.06, .06, 2, 16, 10, 0);
    glTranslatef(0, 0, 2);
    Draw::disk(.1, .15, 16, 10, 0);
    Draw::cylinder(.1, .1, .1, 16, 5, 0);
    glTranslatef(0, 0, .1);
    Draw::disk(.06, .1, 16, 5, 0);
    glTranslatef(0, -.3, -2.1);
  }
  glEndList();
}

void Mech::drawForeArm()
{
  glNewList(dlist + FOREARM, GL_COMPILE);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   for (int i=0; i<5; i++) {
     glTranslatef(0, -.1, -.15);
     Draw::box(.6, .2, .8);
     glTranslatef(0, .1, -.15);
     Draw::box(.4, .1, .6);
   }
   glTranslatef(0, 0, 2.45);
   Draw::box(1, 2, 1);
   glTranslatef(0, 0, -1);
  glEndList();
}

void Mech::drawUpperLeg()
{
  glNewList(dlist + UPPER_LEG, GL_COMPILE);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   glTranslatef(0, -1, 0);
   Draw::box(.4, .7, 1);
   glTranslatef(0, -.65, 0);
   for (int i=0; i<5; i++) {
     Draw::box(1.2, 1.2, .3);
     glTranslatef(0, -.2, 0);
     Draw::box(1, 1, .1);
     glTranslatef(0, -.2, 0);
   }
   glTranslatef(0, -.15, -.4);
   Draw::box(2, 2, .5);
   glTranslatef(0, -.3, -.2);
   glRotatef(90, 1, 0, 0);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   Draw::cylinder(.6, .6, 3, 16, 10, 0);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   glRotatef(-90, 1, 0, 0);
   glTranslatef(0, -1.5, 1);
   Draw::box(1.5, .5, 3);
   glTranslatef(0, -1.75, -.8);
   Draw::box(2, 2, .5);
   glTranslatef(0, -.9, -.85);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   Draw::cylinder(.8, .8, 1.8, 16, 10, 0);
   for (int i=0; i<2; i++) {
     if (i)
       glScalef(-1, 1, 1);
     if (i)
       glTranslatef(0, 0, 1.8);
     Draw::disk(0, .8, 16, 10, 0);
     if (i)
       glTranslatef(0, 0, -1.8);
   }
   glScalef(-1, 1, 1);
  glEndList();
}

void Mech::drawFoot()
{
  glNewList(dlist + FOOT, GL_COMPILE);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   glRotatef(90, 1, 0, 0);
   Draw::octagon(1.5, .6, 0);
   glRotatef(-90, 1, 0, 0);
  glEndList();
}

void Mech::drawLowerLeg()
{
  setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
  for (float k=0; k<2; k++) {
    for (float l=0; l<2; l++) {
      glPushMatrix();
      glTranslatef(k, 0, l);
      setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
      Draw::box(1, 1, .5);
      glTranslatef(0, -.45, 0);
      setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
      Draw::sphere(.2, 16, 16, 0);
      if (leg)
        glRotatef((GLfloat) heel1, 1, 0, 0);
      else
        glRotatef((GLfloat) heel2, 1, 0, 0);
      glTranslatef(0, -1.7, 0);
      setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
      Draw::box(.25, .25, 3);
      glTranslatef(0, -1.7, 0);
      setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
      Draw::sphere(.2, 16, 16, 0);
      if (leg)
        glRotatef((GLfloat) - heel1, 1, 0, 0);
      else
        glRotatef((GLfloat) - heel2, 1, 0, 0);
      glTranslatef(0, -.45, 0);
      setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
      Draw::box(1, 1, .5);
      if (!k && !l) {
        int j;

        glTranslatef(-.4, -.8, .5);
        if (leg)
          glRotatef((GLfloat) ankle1, 1, 0, 0);
        else
          glRotatef((GLfloat) ankle2, 1, 0, 0);
        glRotatef(90, 0, 1, 0);
        Draw::cylinder(.8, .8, 1.8, 16, 10, 0);
        for (j=0; j<2; j++) {
          if (j) {
            glScalef(-1, 1, 1);
            glTranslatef(0, 0, 1.8);
          }
          Draw::disk(0, .8, 16, 10, 0);
          if (j)
            glTranslatef(0, 0, -1.8);
        }
        glScalef(-1, 1, 1);
        glRotatef(-90, 0, 1, 0);
        glTranslatef(.95, -.8, 0);
        glCallList(dlist + FOOT);
      }
      glPopMatrix();
    }
  }
}

void Mech::drawRocketPod()
{
  glNewList(dlist + ROCKET, GL_COMPILE);
   setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
   glScalef(.4, .4, .4);
   // Neck1
   glRotatef(45, 0, 0, 1);
   glTranslatef(1, 0, 0);
   Draw::box(2, 3, .5);
   // Neck2
   glTranslatef(1, 0, 0);
   glRotatef(45, 0, 0, 1);
   glTranslatef(.5, 0, 0);
   Draw::box(1.2, 3, .5);
   // Head
   glTranslatef(2.1, 0, 0);
   glRotatef(-90, 0, 0, 1);
   setMaterial(mat_specular, mat_ambient, mat_diffuse, mat_shininess);
   Draw::box(2, 4, 3);
   glTranslatef(-.5, -1, 1.3);
   // Eyes
   for (int i=0; i<2; i++) {
     int k = 0;
     for (int j=2; j<3; j++) {
       glTranslatef(i, j, .6);
       setMaterial(mat_specular3, mat_ambient3, mat_diffuse3, mat_shininess3);
       Draw::cylinder(.4, .4, .3, 16, 10, 0);
       glTranslatef(0, 0, .3);
       setMaterial(mat_specular4, mat_ambient4, mat_diffuse4, mat_shininess4);
       Draw::cylinder(.4, 0, .5, 16, 10, 0);
       k++;
       glTranslatef(-i, -j, -.9);
     }
   }
  glEndList();
}

/* renders the mech */
void Mech::display()
{
  glPushMatrix();
  {
   glScalef(.5, .5, .5);
   glPushMatrix();
   {
    glTranslatef(0, -.75, 0);
    glRotatef((GLfloat) tilt, 1, 0, 0);
    glRotatef(90, 1, 0, 0);
    glCallList(dlist + HIP);
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, .75, 0);

    // torso
    glPushMatrix();
    {
     glRotatef((GLfloat) pivot, 0, 1, 0);
     glPushMatrix();
     {
      glCallList(dlist + TORSO);
     }
     glPopMatrix();
     glPushMatrix();
     {
      glTranslatef(.5, .5, 0);
      glCallList(dlist + ROCKET);
     }
     glPopMatrix();

     // shouders, upperarms, forearms, vulcan
     for (int i=0; i<2; i++) {
       glPushMatrix();
       {
        if (i)
          glScalef(-1, 1, 1);
        glTranslatef(1.5, 0, 0);
        glCallList(dlist + SHOULDER);
        glTranslatef(.9, 0, 0);
        if (i) {
          glRotatef((GLfloat) lat1, 0, 0, 1);
          glRotatef((GLfloat) shoulder1, 1, 0, 0);
          glRotatef((GLfloat) shoulder3, 0, 1, 0);
         } else {
           glRotatef((GLfloat) lat2, 0, 0, 1);
           glRotatef((GLfloat) shoulder2, 1, 0, 0);
           glRotatef((GLfloat) shoulder4, 0, 1, 0);
         }
         glTranslatef(0, -1.4, 0);
         glCallList(dlist + UPPER_ARM);
         glTranslatef(0, -2.9, 0);
         if (i)
           glRotatef((GLfloat) elbow1, 1, 0, 0);
         else
           glRotatef((GLfloat) elbow2, 1, 0, 0);
         glTranslatef(0, -.9, -.2);
         glCallList(dlist + FOREARM);
         glPushMatrix();
         {
          glTranslatef(0, 0, 2);
          glRotatef((GLfloat) fire, 0, 0, 1);
          glCallList(dlist + VULCAN);
         }
         glPopMatrix();
       }
       glPopMatrix();
     } //for
    }
    glPopMatrix();
   }
   glPopMatrix();

   // legs
   for (int j=0; j<2; j++) {
     glPushMatrix();
     {
      if (j) {
        glScalef(-.5, .5, .5);
        leg = 1;
      } else {
        glScalef(.5, .5, .5);
        leg = 0;
      }
      glTranslatef(2, -1.5, 0);
      if (j) {
        glRotatef((GLfloat) hip11, 1, 0, 0);
        glRotatef((GLfloat) hip12, 0, 0, 1);
      } else {
        glRotatef((GLfloat) hip21, 1, 0, 0);
        glRotatef((GLfloat) hip22, 0, 0, 1);
      }
      glTranslatef(0, .3, 0);
      glPushMatrix();
      {
       glCallList(dlist + UPPER_LEG);
      }
      glPopMatrix();
      glTranslatef(0, -8.3, -.4);
      if (j)
        glRotatef((GLfloat) - hip12, 0, 0, 1);
      else
        glRotatef((GLfloat) - hip22, 0, 0, 1);
      glTranslatef(-.5, -.85, -.5);
      drawLowerLeg();		// no display-list, strange !
     }
     glPopMatrix();
   } //for
  }
  glPopMatrix();
}

void Mech::draw()
{
#if 0 //FIXME segfault
  if (dlist != -1) glDeleteLists(dlist, BODY_PARTS);
#endif
  dlist = glGenLists(BODY_PARTS);
  setMaterial(mat_specular2, mat_ambient2, mat_diffuse2, mat_shininess2);
  drawTorso();
  drawHip();
  drawShoulder();
  drawRocketPod();
  drawUpperArm();
  drawForeArm();
  drawUpperLeg();
  drawFoot();
  drawVulcanGun();
}

/* system of equations handling permanent motion */
void Mech::changePermanent(float lasting)
{
  float angle;
  static int step;

  if (!anim) return;

  if (step == 0 || step == 2) {
    if (mframe >= 0 && mframe <= 21) {
      if (mframe == 0)
        mframe = 3;
      angle = (180 / M_PI) * (acos(((cos((M_PI / 180) * mframe) * 2.043) + 1.1625) / 3.2059));
      if (mframe > 0)
        elevation = -(3.2055 - (cos((M_PI / 180) * angle) * 3.2055));
      else
        elevation = 0;

      if (step == 0) {
        hip11 = (int) -(mframe * 1.7);
        if (1.7 * mframe > 15)
          heel1 = (int) (mframe * 1.7);
        heel2 = 0;
        ankle1 = (int) (mframe * 1.7);
        if (mframe > 0)
          hip21 = (int) angle;
        else
          hip21 = 0;
        ankle2 = -hip21;
        shoulder1 = (int) (mframe * 1.5);
        shoulder2 = (int) -(mframe * 1.5);
        elbow1 = (int) mframe;
        elbow2 = (int) -mframe;
      } else {
        hip21 = (int) -(mframe * 1.7);
        if (1.7 * mframe > 15)
          heel2 = (int) (mframe * 1.7);
        heel1 = 0;
        ankle2 = (int) (mframe * 1.7);
        if (mframe > 0)
          hip11 = (int) angle;
        else
          hip11 = 0;
        ankle1 = -hip11;
        shoulder1 = (int) -(mframe * 1.5);
        shoulder2 = (int) (mframe * 1.5);
        elbow1 = (int) -mframe;
        elbow2 = (int) mframe;
      }
      if (mframe == 21)
        step++;
      if (mframe < 21)
        mframe = mframe + 3;
    }
  }
  if (step == 1 || step == 3) {
    /* for(x=21; x>=0; x=x-3){ */
    if (mframe <= 21 && mframe >= 0) {
      angle = (180 / M_PI) * (acos(((cos((M_PI / 180) * mframe) * 2.043) + 1.1625) / 3.2029));
      if (mframe > 0)
        elevation = -(3.2055 - (cos((M_PI / 180) * angle) * 3.2055));
      else
        elevation = 0;
      if (step == 1) {
        elbow2 = hip11 = (int) -mframe;
        elbow1 = heel1 = (int) mframe;
        heel2 = 15;
        ankle1 = (int) mframe;
        if (mframe > 0)
          hip21 = (int) angle;
        else
          hip21 = 0;
        ankle2 = -hip21;
        shoulder1 = (int) (1.5 * mframe);
        shoulder2 = (int) -(mframe * 1.5);
      } else {
        elbow1 = hip21 = (int) -mframe;
        elbow2 = heel2 = (int) mframe;
        heel1 = 15;
        ankle2 = (int) mframe;
        if (mframe > 0)
          hip11 = (int) angle;
        else
          hip11 = 0;
        ankle1 = -hip11;
        shoulder1 = (int) -(mframe * 1.5);
        shoulder2 = (int) (mframe * 1.5);
      }
      if (mframe == 0)
        step++;
      if (mframe > 0)
        mframe = mframe - 3;
    }
  }
  if (step == 4)
    step = 0;
}

/* creation from a file */
WObject * Mech::creator(char *l)
{
  return new Mech(l);
}

void Mech::defaults()
{
  anim = walking = 0;
}

void Mech::parser(char *l)
{
  defaults();
  l = tokenize(l);
  begin_while_parse(l) {
    l = parse()->parseAttributes(l, this);
    if (!l) break;
    if      (!stringcmp(l, "anim")) l = parse()->parseUInt8(l, &anim, "anim");
    else if (!stringcmp(l, "walk")) l = parse()->parseUInt8(l, &walking, "walk");
  }
  end_while_parse(l);
}

void Mech::makeSolid()
{
  char s[256];
  sprintf(s,"solid shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />", .5, .5, 2.);
  parse()->parseSolid(s, SEP, this);
}

Mech::Mech(char *l)
{
  parser(l);
  makeSolid();

  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
  enablePermanentMovement();	// anim

  rotz = (float) RAD2DEG(pos.az);
  roty = (float) RAD2DEG(pos.ax);
  rotx = (float) 90;

  draw();
}

void Mech::render()
{
  static float mech_radian = 3*M_PI/2;	// radian
  static float mech_step   = 72;	// steps per tours
  static float mech_radius = 7;		// space unit

  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  glScalef(.5, .5, .5);
  glRotatef(rotx, 1, 0, 0);
  glRotatef(roty, 0, 1, 0);
  glRotatef(rotz, 0, 0, 1);

  if (walking) {
    mech_radian -= M_2PI / mech_step;
    if (mech_radian <= 0)
      mech_radian = M_2PI;
    float dx =  mech_radius * cos(mech_radian);
    float dz = -mech_radius * sin(mech_radian);
    glTranslatef(dx, 0, dz);
    glRotatef((float) RAD2DEG(mech_radian), 0, 1, 0);
  }

  display();
  glPopMatrix();
}

void Mech::quit()
{
  savePersistency();
}

void Mech::still(Mech *mech, void *d, time_t s, time_t u)
{ mech->anim = 0; mech->walking = 0; }

void Mech::animate(Mech *mech, void *d, time_t s, time_t u)
{ mech->anim = 1; }

void Mech::walk(Mech *mech, void *d, time_t s, time_t u)
{ mech->walking = 1; }

void Mech::funcs()
{
  setActionFunc(MECH_TYPE, 0, WO_ACTION still, "Still");
  setActionFunc(MECH_TYPE, 1, WO_ACTION animate, "Anim");
  setActionFunc(MECH_TYPE, 2, WO_ACTION walk, "Walk");
  setActionFunc(MECH_TYPE, 3, WO_ACTION moveObject, "Move");
}
