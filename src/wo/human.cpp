//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
//-------------------------------------------------------------------
// Fichier d'origine en VRML
// Auteurs de l'avatar initial : Thierry BLANDET - Magalie GROSS
//-------------------------------------------------------------------
#include "vreng.hpp"
#include "human.hpp"
#include "wobject.hpp"
#include "user.hpp"	// localuser
#include "pref.hpp"	// Pref

#include "man.data"	// vgl/man.data : vertices, texcoord, normals, triangles
// 462 Verticies
// 526 Texture Coordinates
// 775 Normals
// 940 Triangles


const OClass Human::oclass(HUMAN_TYPE, "Human", Human::creator);

static struct _tMaterial materials[] = {
  { {0,0,0}, {0.5,1.0,0.5}, {0,0,0}, {0,0,0}, 1, 20, -1 }, //bust green
  { {0,0,0}, {0.2,0.4,1.0}, {0,0,0}, {0,0,0}, 1, 20, -1 }, //legs blue
  { {0,0,0}, {0.1,0.1,0.1}, {0,0,0}, {0,0,0}, 1, 20, -1 }, //feet brown
  { {0,0,0}, {1.0,0.7,0.8}, {0,0,0}, {0,0,0}, 1, 20, -1 }, //head grey1
  { {0,0,0}, {1.0,0.8,0.8}, {0,0,0}, {0,0,0}, 1, 20, -1 }, //hands grey2
  { {0,0,0}, {1.0,0.1,0.1}, {0,0,0}, {0,0,0}, 1, 20, -1 }, //bust red
  { {0,0,0}, {1.0,1.0,0.1}, {0,0,0}, {0,0,0}, 1, 20, -1 }  //hairs yellow
};

static _tBody manpart[] = {
 {MAN_FOREARM_R,    0, 16, 4},	// forearm_r
 {MAN_FOREARM_L,   16, 16, 4},	// forearm_l
 {MAN_THIGH_R,     32, 16, 1},	// thigh_r
 {MAN_THIGH_L,     48, 16, 1},	// thigh_l
 {MAN_ELBOW_R,     64, 36, 4},	// elbow_r
 {MAN_ARM_R,      100, 16, 5},	// arm_r
 {MAN_ARM_L,      116, 16, 5},	// arm_l
 {MAN_ELBOW_L,    132, 36, 4},	// elbow_l
 {MAN_SHOULDER_R, 168, 36, 5},	// shoulder_r
 {MAN_HIP_R,      204, 36, 1},	// hip_r
 {MAN_HIP_L,      240, 36, 1},	// hip_l
 {MAN_SHOULDER_L, 276, 36, 5},	// shoulder_l
 {MAN_FOOT_R,     312, 56, 2},	// foot_r
 {MAN_FOOT_L,     368, 56, 2},	// foot_l
 {MAN_ANKLE_R,    424, 36, 4},	// ankle_r
 {MAN_ANKLE_L,    460, 36, 4},	// ankle_l
 {MAN_SHIN_R,     496, 16, 1},	// shin_r
 {MAN_SHIN_L,     512, 16, 1},	// shin_l
 {MAN_KNEE_R,     528, 36, 1},	// knee_r
 {MAN_KNEE_L,     564, 36, 1},	// knee_l
 {MAN_BELT,       600, 36, 2},	// belt
 {MAN_PELVIC,     636, 16, 1},	// pelvic
 {MAN_HAIRS,      652, 32, 6},	// hairs
 {MAN_HEAD,       684, 32, 4},	// head
 {MAN_CHEST,      716, 48, 5},	// chest
 {MAN_NECK,       764, 36, 4},	// neck
 {MAN_HAND_R,     800, 34, 4},	// hand_r
 {MAN_HAND_L,     834, 34, 4},	// hand_l
 {MAN_WRIST_R,    868, 36, 4},	// wrist_r
 {MAN_WRIST_L,    904, 36, 4}	// wrist_l
};


/* Creation from a file */
WObject * Human::creator(char *l)
{ 
  return new Human(l);
} 

/* Constructor */
Human::Human()
{
  Pref* pref = &::g.pref;

  if (pref->my_widthstr)        width = atof(pref->my_widthstr);
  if (pref->my_depthstr)        depth = atof(pref->my_depthstr);
  if (pref->my_heightstr)       height = atof(pref->my_heightstr);
  usercontrol = true;
  behaviors();
  geometry();
  dlist = -1;
}

/* constructor from xml file */
Human::Human(char *l)
{ 
  parser(l);
  usercontrol = false;
  behaviors();
  dlist = -1;
}

Human::~Human()
{}

void Human::parser(char *l)
{ 
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
  }
  end_while_parse(l);
}

void Human::behaviors()
{
  enableBehavior(NO_ELEMENTARY_MOVE);
  enableBehavior(COLLIDE_NEVER);
  enableBehavior(PERMANENT_MOVEMENT);
  enableBehavior(SPECIFIC_RENDER);

  initMobileObject(0);
}

void Human::geometry()
{
  char s[128];

  sprintf(s, "solid shape=\"bbox\" dim=\"%.2f %.2f %.2f\" />", width, depth, height);
  parseSolid(s);
}

void Human::changePermanent(float lasting)
{
  if (usercontrol && localuser) {
    // follow localuser movement
    pos.x = localuser->pos.x;
    pos.y = localuser->pos.y;
    pos.z = localuser->pos.z;
    pos.az = localuser->pos.az - M_PI_2;
    updatePosition();
  }
}

void Human::myMaterial(GLenum mode, float *rgb, float alpha)
{
  float color[4];

  color[0] = rgb[0];
  color[1] = rgb[1];
  color[2] = rgb[2];
  color[3] = alpha;
  glMaterialfv(GL_FRONT_AND_BACK, mode, color);
}

/* Sets the material properties of the 3D Object */
void Human::setMaterial(int i)
{
  float alpha = materials[i].alpha;

  myMaterial(GL_AMBIENT, materials[i].ambient, alpha);
  myMaterial(GL_DIFFUSE, materials[i].diffuse, alpha);
  myMaterial(GL_SPECULAR, materials[i].specular, alpha);
  myMaterial(GL_EMISSION, materials[i].emission, alpha);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materials[i].shininess);
}

// Draws in the display list
GLint Human::displaylist()
{
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  draw();
  glEndList();
  return dlist;
}

/* Draws man */
void Human::draw()
{
  int vc = 0;

  for (int i=0; i < (sizeof(faces)/sizeof(faces[0])); i++) {
    glBegin(GL_TRIANGLES);
    if (vc == 0) {
      setMaterial(manpart[i].mat);
      vc = manpart[i].vcount;
    }
    vc--;
    for (int j=0; j<3 ; j++) {
      int vi = faces[i][j];	//Vertex index
      int ni = faces[i][j+3];	//Normal index
      int ti = faces[i][j+6];	//Texture index
      glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
      glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);
      glTexCoord2f(texcoords[ti][0], texcoords[ti][1]);
    }
    glEnd();
  }
}

void Human::render()
{
  glPushMatrix();
   glTranslatef(pos.x, pos.y, pos.z);
   glRotatef(RAD2DEG(pos.az) + 180, 0, 0, 1);	// orientation forward
   glRotatef(RAD2DEG(pos.ax) + 90, 1, 0, 0);    // stand up
   glScalef(.9, .9, .9);

   //dax glCallList(dlist);	// draw human display list
   draw();			//dax draw directly

  glPopMatrix();
}

void Human::armShowing(int dir[3])
{
  // shoulder join point
  float posx = 0.20;	// distance a l'epaule
  float posy = 0;	// centre de l'epaule
  float posz = 1.6;	// shoulder height

  float theta, psi;

  theta = asin((sqrt(pow(dir[0]-posx,2)+pow(dir[2]+posy,2))) / (abs(dir[0]-posx)));

  if (dir[2] >= posz)
    psi = asin((sqrt(pow(dir[0]-posx,2)+pow(dir[1]-posz,2))) / (abs(dir[1]-posz)));
  else
    psi = asin((sqrt(pow(dir[0]-posx,2)+pow(dir[1]-posz,2))) / (abs(dir[0]-posx)));

  float rotx = theta;
  float roty = 0;
  float rotz = psi;

  glPushMatrix();
   glTranslatef(posx, posy, posz);
   glRotatef(rotx, 1,0,0);
   glRotatef(roty, 0,1,0);
   glRotatef(rotz, 0,0,1);

   draw();
   //handPointing();
  glPopMatrix();
}

void Human::funcs()
{
}

void Human::quit()
{
}
