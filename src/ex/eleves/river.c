#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>


int taille;
int width;
int depth;

void clavier(unsigned char key, int x, int y);
void render(void);
void reshape(int w, int h);
void initGL(void);

float cam_pos_x ;
float cam_pos_y ;
float cam_pos_z ;
float cam_dir_x ;
float cam_dir_y ;
float cam_dir_z ;
float cam_up_x ;
float cam_up_y ;
float cam_up_z ;
double cam_rot_h ;
double cam_rot_v ;
void init_cam (void) ;
float fog_density = 0;
float rapport ;
float clip_rapport ;
float clip_left = -0.1 ;
float clip_right = 0.1 ;
float clip_bottom = -0.1 ;
float clip_top = 0.1 ;
float clip_near = 0.1 ;
float clip_far = 10 ;
double model_rot_h =0 ;
double model_rot_v =0 ;

//vagues
int nb_ondes = 10; // au plus 10
float vagues_phase[10];
float vagues_k[20];
float vagues_vitesse[10];
float vagues_ampl[10];
float eau_spec [4] = {1,1,1,1};
float eau_diff [4] = {1,1,1,1};
float eau_amb [4] = {1,1,1,1};
float eau_emission [4] = {0,0.1,0.2,1};

void inits(void);
void point_vague(float a, float b);


int main( int argc, char *argv[] )
{
  int i,it ;

  printf("Nombre d'iterations:");
  scanf("%d",&it);
  taille = it*it;
  width = 2*it;
  depth = it/2;
  srand(time(0));
  inits();
  glutInit(&argc, argv );
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(1000, 600);
  glutCreateWindow("River");
  init_cam();
  initGL() ;
  glutReshapeFunc(reshape);
  glutDisplayFunc(render);
  glutKeyboardFunc(clavier);
  glutMainLoop();
  return 0;
}

void inits(void)
{
  int i;
  float coeff_k = 1.;
  for (i=0; i<nb_ondes ; i++) {
    vagues_k[2*i] = (float) (rand()%100)*coeff_k ;
    vagues_k[2*i+1] = (float) (rand()%100)*coeff_k;
    vagues_phase[i] = (float) (rand()%10);
    vagues_vitesse[i] = (float) (rand()%100)/300;
    vagues_ampl[i] = 0.005* (float) (rand()%100)/100 ;
  }
}

void render(void)
{
  int i,j ;
  float a,b,c ;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
  glTranslated(-cam_pos_x,-cam_pos_y,-cam_pos_z);
  glRotated(model_rot_v,1,0,0);
  glRotated(model_rot_h,0,0,1);
  glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,100);
  glTranslated(-0.5,-0.5,0); 
  //glScalef(width, depth, 1);

  glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,30);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,eau_spec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,eau_diff);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,eau_amb);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,eau_emission);
  glColor4f(0.2,0.4,0.7,0.9);

  for (a=0; a<width ; a++) {
    for (b=0; b<depth ; b++) {
      glBegin(GL_QUADS);
      point_vague(a,b);
      point_vague(a+1,b);
      point_vague(a+1,b+1);
      point_vague(a,b+1);
      glEnd();
    }
  }
  for(i=0; i<nb_ondes ; i++)
    vagues_phase[i] = vagues_phase[i] + vagues_vitesse[i];
  glutSwapBuffers();
  glutPostRedisplay();
}

void point_vague(float a, float b)
{
  float x=0,y=0,z=0;
  float vx,vy;
  int i;
  for (i=0; i<nb_ondes ; i++) {
    vx = vagues_k[2*i];
    vy = vagues_k[2*i+1];
    x += vx*taille*sin(a*vx+b*vy + vagues_phase[i])*vagues_ampl[i];
    y += vy*taille*sin(a*vx+b*vy + vagues_phase[i])*vagues_ampl[i];
  }
  glNormal3f(x,y,1);
  for (i=0; i<nb_ondes ; i++) {
    vx = vagues_k[2*i];
    vy = vagues_k[2*i+1];
    z += cos(a*vx+b*vy + vagues_phase[i])*vagues_ampl[i];
  }
  glVertex3d(a/taille, b/taille, z);
}


void reshape (int w, int h)
{
  rapport = (float)w/(float)h;
  clip_left = -0.1*rapport*clip_rapport ;
  clip_right = 0.1*rapport*clip_rapport ;
  clip_bottom = -0.1*clip_rapport ;
  clip_top = 0.1*clip_rapport ;
  clip_near = 0.1 ;
  clip_far = 10 ;
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(clip_left,clip_right,clip_bottom,clip_top,clip_near,clip_far);
}

void clavier(unsigned char key, int x, int y)
{
  float pas_pos = 0.03 ;
  float pas_rot = 2 ;
  int parami ;
  float * paramfv ;
  int do_reshape = 0;
      switch(key) {
         case 'q' : cam_pos_x=cam_pos_x-pas_pos ;break;
         case 'd' : cam_pos_x=cam_pos_x+pas_pos ;break;
         case 'z' : cam_pos_z=cam_pos_z-pas_pos ;break ;
         case 's' : cam_pos_z=cam_pos_z+pas_pos ;break ;
         case 'a' : cam_pos_y=cam_pos_y+pas_pos ;break;
         case 'w' : cam_pos_y=cam_pos_y-pas_pos ;break;
         case 'm' : model_rot_h=model_rot_h+pas_rot ;break;
         case 'k' : model_rot_h=model_rot_h-pas_rot ;break;
         case 'o' : model_rot_v=model_rot_v-pas_rot ;break ;
         case 'l' : model_rot_v=model_rot_v+pas_rot ;break ;
         case 'r' : init_cam() ; do_reshape = 1; break ;
         case 'y' : clip_rapport=2*clip_rapport ; do_reshape=1 ;break ;
         case 't' : clip_rapport=clip_rapport/2 ; do_reshape=1 ;break ;
         case 'g' : fog_density = 0.1+fog_density ; break;
         case 'h' : fog_density = fog_density-0.1 ; break;
         case 'x' : exit(0);
      }
  glFogf(GL_FOG_DENSITY,fog_density);
  if (do_reshape) {
    clip_left = -0.1*rapport*clip_rapport ;
    clip_right = 0.1*rapport*clip_rapport ;
    clip_bottom = -0.1*clip_rapport ;
    clip_top = 0.1*clip_rapport ;
    clip_near = 0.1 ;
    clip_far = 100 ;
    glGetFloatv(GL_VIEWPORT,paramfv);
    reshape(paramfv[2],paramfv[3]);
  }
}

void init_cam (void)
{
  clip_rapport = 0.75 ;
  cam_pos_x = 0 ;
  cam_pos_y = 0.3 ;
  cam_pos_z = 1.3 ;
  model_rot_h = 45;
  model_rot_v = -76 ;
  cam_rot_h = 0 ;
  cam_rot_v = 0 ;
}

int gamb[4] = {0,0,0,1};
void initGL(void)
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquationEXT(GL_FUNC_ADD_EXT);
  //glLightModeliv(GL_LIGHT_MODEL_AMBIENT,gamb);
  glEnable(GL_FOG);
  glFogf(GL_FOG_MODE,GL_EXP2);//GL_LINEAR,GL_EXP,GL_EXP2
  glFogf(GL_FOG_DENSITY,fog_density);
  float fog [4] = {1,1,1,1};
  glFogfv(GL_FOG_COLOR,fog);
}
