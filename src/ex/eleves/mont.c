#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

typedef struct {float x; float y; float z;} tVect;

float *vertices;
tVect *norms;

int it = 4;
float coeff_div = 1.85;	// de combien est divisee la hauteur ajoutee lors d'une subdivision
float coeff_mul = 1.;	// de combien est multiplie le coeff lors d'une subdivision 
float height = 0.3;	// hauteur de base
int couleurs_continues = 1;
int normales_moyennes = 1;
int size;

float cam_pos_x;
float cam_pos_y;
float cam_pos_z;
float cam_dir_x;
float cam_dir_y;
float cam_dir_z;
float cam_up_x;
float cam_up_y;
float cam_up_z;
double cam_rot_h;
double cam_rot_v;

float clip_left = -0.1;
float clip_right = 0.1;
float clip_bottom = -0.1;
float clip_top = 0.1;
float clip_near = 0.1;
float clip_far = 10;
float clip_rapport;
float rapport;

double light_rot =0;
double model_rot_h =0;
double model_rot_v =0;
float light_pos [4] = {1,0,0,0};
float mont_spec [4] = {0,0,0,1};
float mont_diff [4] = {1,1,1,1};
float mont_amb [4] = {0,0,0,1};
float gamb[4] = {0,0,0,1};
float light1_diff[4] = {1,1,1,1};
float light1_spec[4] = {1,1,1,1};
float light1_amb[4] = {0.05,0.05,0.05,1};
float lumierenulle [4] = {0,0,0,1};
float fog_density = 0;

void hauteurs(int,int,int,float,float);
void aux_hauteur(int,int,int,int,float);
void normales(void);
void prod_vect(float x1, float y1, float z1, float x2, float y2, float z2, float *a, float *b, float *c);
void init_cam(void);
void lumiere(void);
void couleur(float z);
void montagne(void);
void clavier(unsigned char key, int x, int y);
void scene(void);
void reshape(int w, int h);
void initGL(void);


int main(int argc, char *argv[])
{
  size = (it * it) + 1;
  vertices = (float *) malloc(size*size*sizeof(float));
  norms = (tVect *) malloc(size*size*sizeof(tVect));
  //vertices[size*(size-1)] = 0;
  //vertices[size*size-1] = height;
  srand(time(0));
  hauteurs(it, 0, 0, height, coeff_div);
  normales();
  init_cam();
  glutInit(&argc, argv);
  initGL();
  return 1;
}

void hauteurs(int iter, int x, int y, float h, float coeff)
{
  if (iter == 0) return;

  int width = pow(2, iter);
  if (y == 0) aux_hauteur(x, y, x+width, y, h);
  if (x == 0) aux_hauteur(x, y, x, y+width, h);
  aux_hauteur(x+width, y, x+width, y+width, h);
  aux_hauteur(x, y+width, x+width, y+width, h);
  vertices[(x+width/2)*size + y+width/2] = (vertices[x*size+y]+vertices[(x+width)*size+y]+vertices[x*size+y+width]+vertices[(x+width)*size+y+width])/4;
  float newcoeff = coeff * coeff_mul;
  hauteurs(iter-1, x, y, h/coeff, newcoeff);
  hauteurs(iter-1, x+width/2, y, h/coeff, newcoeff);
  hauteurs(iter-1, x, y+width/2, h/coeff, newcoeff);
  hauteurs(iter-1, x+width/2, y+width/2, h/coeff, newcoeff);
}

void aux_hauteur(int x1, int y1, int x2, int y2, float h)
{
  vertices[(x1+x2)/2*size + (y1+y2)/2] = (vertices[x1*size +y1] + vertices[x2*size +y2])/2 + h*((rand()%21)-10)/10;
}

void normales(void)
{
  int i,j;
  for (i=0; i<size-1 ; i++) {
    for (j=0; j<size-1 ; j++) {
      float a,b,c;
      prod_vect(2./size, 0, vertices[(i+1)*size+j]-vertices[(i-1)*size+j], 0, 2./size, vertices[i*size+j+1]-vertices[i*size+j-1], &a, &b, &c);
      norms[i*size+j].x = a;
      norms[i*size+j].y = b;
      norms[i*size+j].z = c;
    }
  }
}

void scene(void)
{
  int i,j;
  float a,b,c;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-cam_pos_x, -cam_pos_y, -cam_pos_z);
    glRotated(model_rot_v, 1,0,0);
    glRotated(model_rot_h, 0,0,1);
  glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);
  lumiere();

  //debut montagne
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mont_spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lumierenulle);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mont_amb);
  glTranslated(-.5, -.5, 0); 
  montagne();
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lumierenulle);
  if (normales_moyennes) {
    glBegin(GL_QUADS);
    for (i=0; i<size-1 ; i++) {
      for (j=0; j<size-1 ; j++) {
        couleur(vertices[i*size+j]);
        a = norms[i*size+j].x;
        b = norms[i*size+j].y;
        c = norms[i*size+j].z;
        glNormal3f(a, b, c);
        glVertex3f((float)i/size, (float)j/size, vertices[i*size+j]);
        if (couleurs_continues) couleur(vertices[(i+1)*size+j]);
        a = norms[(i+1)*size+j].x;
        b = norms[(i+1)*size+j].y;
        c = norms[(i+1)*size+j].z;
        glNormal3f(a, b, c);
        glVertex3f((float)(i+1)/size, (float)j/size, vertices[(i+1)*size+j]);
        if (couleurs_continues) couleur(vertices[(i+1)*size+j+1]);
        a = norms[(i+1)*size+j+1].x;
        b = norms[(i+1)*size+j+1].y;
        c = norms[(i+1)*size+j+1].z;
        glNormal3f(a, b, c);
        glVertex3f((float)(i+1)/size, (float)(j+1)/size, vertices[(i+1)*size+j+1]);
        if (couleurs_continues) couleur(vertices[i*size+j+1]);
        a = norms[i*size+j+1].x;
        b = norms[i*size+j+1].y;
        c = norms[i*size+j+1].z;
        glNormal3f(a, b, c);
        glVertex3f((float)i/size, (float)(j+1)/size, vertices[i*size+j+1]);
      }
    }
    glEnd();      
  }
  else {
    glBegin(GL_QUADS);
    for (i=0; i< size-1 ; i++) {
      for (j=0; j<size-1 ; j++) {
        prod_vect(1.0/size, 0, vertices[(i+1)*size+j]-vertices[i*size+j], 0, 1.0/size, vertices[i*size+j+1]-vertices[i*size+j], &a, &b, &c);
	glNormal3f(a, b, c);
        couleur(vertices[i*size+j]);
        glVertex3f((float)i/size, (float)j/size, vertices[i*size+j]);
        if (couleurs_continues)
          couleur(vertices[(i+1)*size+j]);
        glVertex3f((float)(i+1)/size, (float)j/size, vertices[(i+1)*size+j]);
        if (couleurs_continues)
          couleur(vertices[(i+1)*size+j+1]);
        glVertex3f((float)(i+1)/size, (float)(j+1)/size, vertices[(i+1)*size+j+1]);
        if (couleurs_continues)
          couleur(vertices[i*size+j+1]);
        glVertex3f((float)i/size, (float)(j+1)/size, vertices[i*size+j+1]);
      }
    }	 
    glEnd();      
  }	 
  glutSwapBuffers();
  glutPostRedisplay();
}

void lumiere(void)
{
  double a = sin(3.141592653589793238462643832795*(light_rot/180));
  light1_diff [1] = 0.5 + a/2;
  light1_diff [2] = 0.3 + a;
  light1_spec [1] = a;
  light1_spec [2] = a;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light1_diff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light1_spec);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light1_amb);
  light1_diff[1] = light1_diff[1]*light1_diff[1];
  light1_diff[2] = light1_diff[2]*light1_diff[2];
  glPushMatrix();
  glRotated(light_rot, -1,-1,0);
    glPushMatrix();
    glTranslated(2,0,0);
      glColor3f(1,1,0.8);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, light1_diff);
      glutSolidSphere(0.1,20,20);
    glPopMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glPopMatrix();

  light_rot += 0.00005 * size*size;
}

void couleur(float z)
{
  float r,g,b;
  if (z>-0.01) {
    r = 0.0009/(0.001+z*z)+z*z/0.05; 
    g = 0.4 +(z+0.1)*(z-0.12)*(z-0.15)*130;
    b = 0.1 + z*z*(z-0.1)/0.018;
  }
  else {
    r = 0.1;
    g = 0.2;
    b = 0.5;
  }
  glColor4d(r,g,b,1);
}

void montagne(void)
{
#if 0 //DAX
  int i,j;
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lumierenulle);
  glColor4f(0,0,0,1);
  glBegin(GL_QUADS);
  for (i=0; i<size-1 ; i++) {
    glVertex3f((float)i/size, 0, -height);
    glVertex3f((float)i/size, 0, vertices[i*size]);
    glVertex3f((float)(i+1)/size, 0, vertices[(i+1)*size]);
    glVertex3f((float)(i+1)/size, 0, -height);
    glVertex3f((float)i/size, 1-1./size, -height);
    glVertex3f((float)i/size, 1-1./size, vertices[i*size+size-1]);
    glVertex3f((float)(i+1)/size, 1-1./size, vertices[(i+1)*size+size-1]);
    glVertex3f((float)(i+1)/size, 1-1./size, -height);
    glVertex3f(0, (float)i/size, -height);
    glVertex3f(0, (float)i/size, vertices[i]);
    glVertex3f(0, (float)(i+1)/size, vertices[(i+1)]);
    glVertex3f(0, (float)(i+1)/size, -height);
    glVertex3f(1-1./size, (float)i/size, -height);
    glVertex3f(1-1./size, (float)i/size, vertices[size*(size-1)+i]);
    glVertex3f(1-1./size, (float)(i+1)/size, vertices[size*(size-1)+(i+1)]);
    glVertex3f(1-1./size, (float)(i+1)/size, -height);
  }
  glEnd();
#endif
}

void clavier(unsigned char key, int x, int y)
{
  float pas_pos = 0.03;
  float pas_rot = 2;
  int parami;
  float * paramfv;
  int do_reshape = 0;
  switch(key) {
     case 'q' : cam_pos_x=cam_pos_x-pas_pos; break;
     case 'd' : cam_pos_x=cam_pos_x+pas_pos; break;
     case 'z' : cam_pos_z=cam_pos_z-pas_pos; break;
     case 's' : cam_pos_z=cam_pos_z+pas_pos; break;
     case 'a' : cam_pos_y=cam_pos_y+pas_pos; break;
     case 'w' : cam_pos_y=cam_pos_y-pas_pos; break;
     case 'r' : init_cam(); do_reshape = 1; break;
     case 'y' : clip_rapport = 2*clip_rapport; do_reshape=1; break;
     case 't' : clip_rapport = clip_rapport/2; do_reshape=1; break;
     case 'n' : normales_moyennes = 1-normales_moyennes; break;
     case 'f' : glGetIntegerv(GL_FOG_MODE,&parami); 
                switch(parami){
		case GL_LINEAR : glFogf(GL_FOG_MODE,GL_EXP);break;
		case GL_EXP : glFogf(GL_FOG_MODE,GL_EXP2);break;
		case GL_EXP2 : glFogf(GL_FOG_MODE,GL_LINEAR);break;
		}
                break;
     case 'g' : fog_density = 0.1+fog_density; break;
     case 'h' : fog_density = fog_density-0.1; break;
     case 'x' : exit(0);
  }

  glFogf(GL_FOG_DENSITY, fog_density);
  if (do_reshape) {
    clip_left = -0.1*rapport*clip_rapport;
    clip_right = 0.1*rapport*clip_rapport;
    clip_bottom = -0.1*clip_rapport;
    clip_top = 0.1*clip_rapport;
    clip_near = 0.1;
    clip_far = 100;
    glGetFloatv(GL_VIEWPORT,paramfv);
    reshape(paramfv[2],paramfv[3]);
  }
}

void prod_vect(float x1, float y1, float z1, float x2, float y2, float z2, float * a, float * b, float * c)
{
  *a = y1*z2 - z1*y2;
  *b = z1*x2 - x1*z2;
  *c = x1*y2 - y1*x2;
}

void init_cam(void)
{
  clip_rapport = 0.75;
  cam_pos_x = 0;
  cam_pos_y = 0.3;
  cam_pos_z = 1.3;
  model_rot_h = 45;
  model_rot_v = -76;
  cam_rot_h = 0;
  cam_rot_v = 0;
}

void reshape(int w, int h)
{
  rapport = (float)w/(float)h;
  clip_left = -0.1*rapport*clip_rapport;
  clip_right = 0.1*rapport*clip_rapport;
  clip_bottom = -0.1*clip_rapport;
  clip_top = 0.1*clip_rapport;
  clip_near = 0.1;
  clip_far = 10;
  glViewport(0,0,w,h);
  // gestion de la projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(clip_left,clip_right,clip_bottom,clip_top,clip_near,clip_far);
}

void initGL(void)
{
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Montagnes");

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  //glEnable(GL_AUTO_NORMAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquationEXT(GL_FUNC_ADD_EXT);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gamb);
  glEnable(GL_FOG);
  glFogf(GL_FOG_MODE,GL_EXP2);//GL_LINEAR,GL_EXP,GL_EXP2
  glFogf(GL_FOG_DENSITY,fog_density);
  float fog [4] = {1,1,1,1};
  glFogfv(GL_FOG_COLOR,fog);

  glutReshapeFunc(reshape);
  glutDisplayFunc(scene);
  glutKeyboardFunc(clavier);
  glutMainLoop();
}
