#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

typedef struct {float x; float y ; float z ;} VECT ;

float * t ;
VECT * t_norm ;
int taille ;
float hauteur_coeff = 1.85 ; // de combien est divis̩e la hauteur ajout̩e lors d'une subdivision
float hauteur_coeff_mod = 1 ; // de combien est multipli̩ le coeff lors d'une subdivision 
float hauteur_init = 0.3 ; // hauteur de base
int fenetre ; // nom de la fenetre graphique


void hauteurs(int,int,int,float,float) ;
void aux_hauteur(int,int,int,int,float);
void afficher_tableau (void) ;
void normales (void) ;
void prod_vect(float x1, float y1, float z1, float x2, float y2, float z2, float * a, float * b, float * c);

void clavier(unsigned char key, int x, int y);
void clavier_spec(int key, int x, int y);
void souris_passive (int,int);
void souris_drag (int,int);
void draw(void);
void reshape(int width, int height);
void initGL(void);

int mode_gestion_camera = 0 ;
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


float clip_rapport ;
double light_rot =0 ;
double model_rot_h =0 ;
double model_rot_v =0 ;
float light_pos [4] = {1,0,0,0} ;
float mont_spec [4] = {0,0,0,1};
float mont_diff [4] = {1,1,1,1};
float mont_amb [4] = {0,0,0,1};
float eau_spec [4] = {1,1,1,1};
float eau_diff [4] = {1,1,1,1};
float eau_amb [4] = {1,1,1,1};
float light1_diff[4] = {1,1,1,1};
float light1_spec[4] = {1,1,1,1};
float light1_amb[4] = {0.05,0.05,0.05,1};
float lune_emission [4]= {0.6,0.5,0.3,1};
float lumierenulle [4] = {0,0,0,1};
int light_rot_on = 1 ;
void lumiere(void) ;
float lune_rot = 56 ;
int lune_rot_on =1 ;

void autour(void);
int environnement_on =0;

float fog_density = 0;
void init_nuages(void);
void nuage(void);
int nuage_nbspheres = 400;
float nuage_coords [400][3] ;
float nuage_rayons [400];
int nuage_on = 1;

int couleurs_continues = 1 ;
int normales_moyennes = 1 ;
void mont_couleur(float z);
void vagues_sans_normales_moyennes(void);
void point_vague_sans_normale(float,float);

int souris_x ;
int souris_y ;

float clip_left = -0.1 ;
float clip_right = 0.1 ;
float clip_bottom = -0.1 ;
float clip_top = 0.1 ;
float clip_near = 0.1 ;
float clip_far = 10 ;


//vagues
int nb_ondes = 10 ; // au plus 10
float vagues_phase [10] ;
float vagues_k [20] ;
float vagues_vitesse [10] ;
float vagues_amplitude [10] ;
void init_vagues (void);
void point_vague (float a,float b);

void init_cam (void) ;
// taille du paysage : toujours 1*1


int main( int argc, char *argv[] )
{
  int i,it ;
  printf("Nombre d'it̩rations :");
  scanf("%d",&it);
  taille = pow(2,it)+1 ;
  t=(float *) malloc(taille*taille*sizeof(float)) ;
  t_norm=(VECT *) malloc(taille*taille*sizeof(VECT)) ;
  t[0]=0;
  t[taille-1]=0;
  t[taille*(taille-1)]=0;
  t[taille*taille-1]=hauteur_init;
  srand(time(0));
  hauteurs(it,0,0,hauteur_init,hauteur_coeff);
  normales();
  init_vagues();
  init_nuages();
  //afficher_tableau();
  glutInit(&argc, argv );
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(1000, 600);
  fenetre = glutCreateWindow("Montagnes");
  glutFullScreen();
  init_cam ();
  initGL() ;
  glutReshapeFunc(reshape);
  glutDisplayFunc(draw);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(clavier_spec);
  glutPassiveMotionFunc(souris_passive); 
  glutMotionFunc(souris_drag);
  glutMainLoop();
  return 1 ;
}

void hauteurs (int niveau, int x, int y, float h, float coeff)
{
  if (niveau==0)
    return ;
  int largeur = pow(2,niveau) ;
  if (y==0) aux_hauteur(x,y,x+largeur,y,h);
  if (x==0) aux_hauteur(x,y,x,y+largeur,h);
  aux_hauteur(x+largeur,y,x+largeur,y+largeur,h);
  aux_hauteur(x,y+largeur,x+largeur,y+largeur,h);
  t[(x+largeur/2)*taille + y+largeur/2] = (t[x*taille+y]+t[(x+largeur)*taille+y]+t[x*taille+y+largeur]+t[(x+largeur)*taille+y+largeur])/4 ;
  //aux_hauteur(x,y,x+largeur,y+largeur,h);
  float newcoeff = coeff * hauteur_coeff_mod ;
  hauteurs(niveau-1,x,y,h/coeff,newcoeff);
  hauteurs(niveau-1,x+largeur/2,y,h/coeff,newcoeff);
  hauteurs(niveau-1,x,y+largeur/2,h/coeff,newcoeff);
  hauteurs(niveau-1,x+largeur/2,y+largeur/2,h/coeff,newcoeff);
}

void aux_hauteur (int x1,int y1,int x2,int y2, float h)
{
  t[(x1+x2)/2 * taille + (y1+y2)/2] = (t[x1*taille +y1] + t[x2*taille +y2])/2 + h*((rand()%21)-10)/10 ;
}


void normales(void)
{
  int i,j ;
  float a,b,c ;
  for(i=1;i<taille-1;i++)
    for(j=1;j<taille-1;j++)
      {
        prod_vect(2.0/taille,0,t[(i+1)*taille+j]-t[(i-1)*taille+j],0,2.0/taille,t[i*taille+j+1]-t[i*taille+j-1],&a,&b,&c);
        t_norm[i*taille+j].x=a;
        t_norm[i*taille+j].y=b;
        t_norm[i*taille+j].z=c;
      }
}

void afficher_tableau ()
{
  int i,j;
  for(i=0;i<taille;i++)
  {
    for(j=0;j<taille;j++)
      printf("  %5.3f",t[taille*i+j]);
  printf("\n");
  }
}


float rapport ;

void reshape (int width, int height)
{
  rapport = (float)width/(float)height ;
  clip_left = -0.1*rapport*clip_rapport ;
  clip_right = 0.1*rapport*clip_rapport ;
  clip_bottom = -0.1*clip_rapport ;
  clip_top = 0.1*clip_rapport ;
  clip_near = 0.1 ;
  clip_far = 10 ;
  glViewport(0,0,width,height);
//*****  gestion de la projection *****//
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(clip_left,clip_right,clip_bottom,clip_top,clip_near,clip_far);
//***** fin gestion projection *****//
  //gluPerspective(45,rapport,0.1,100);
  
}







void draw(void)
{
  int i,j ;
  float a,b,c ;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//d̩but description de la sc̬ne
  glMatrixMode(GL_MODELVIEW);
//  if (mode_gestion_camera==0)
//  {
    glLoadIdentity() ;
    glTranslated(-cam_pos_x,-cam_pos_y,-cam_pos_z);
    glRotated(model_rot_v,1,0,0);
    glRotated(model_rot_h,0,0,1);
/*  }
  if (mode_gestion_camera==1)
  {
    glPopMatrix();
    glPushMatrix();
  }*/
// param̬tres mat̩riaux globaux
  glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,100);
//Gestion lumi̬re
  lumiere();
  if (nuage_on) nuage();
/*lune
  float lune_diff [4] = {1,1,0,1};
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,lumierenulle);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,lumierenulle);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,lune_diff);
  glPushMatrix();
  glRotated(lune_rot,-1,-1,1);
    glTranslated(1,-0.3,0);
    glColor3f(1,0.8,0.4);
    glutSolidSphere(0.1,20,20);
  glPopMatrix();
  if (lune_rot_on) lune_rot = lune_rot + 0.000053568 * taille * taille ;*/
//d̩but montagnes
  glTranslated(-0.5,-0.5,0); 

  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mont_spec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,lumierenulle);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mont_amb);
  autour();
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,lumierenulle);
  if (normales_moyennes)
  {
    for(i=0;i<taille-1;i++)
      for(j=0;j<taille-1;j++)
      {
        glBegin(GL_QUADS);
        mont_couleur(t[i*taille+j]);
        a=t_norm[i*taille+j].x; b=t_norm[i*taille+j].y; c=t_norm[i*taille+j].z;  glNormal3f(a,b,c);
        glVertex3f((float)i/taille,(float)j/taille,t[i*taille+j]);
        if (couleurs_continues) mont_couleur(t[(i+1)*taille+j]);
        a=t_norm[(i+1)*taille+j].x; b=t_norm[(i+1)*taille+j].y; c=t_norm[(i+1)*taille+j].z;  glNormal3f(a,b,c);
        glVertex3f((float)(i+1)/taille,(float)j/taille,t[(i+1)*taille+j]);
        if (couleurs_continues) mont_couleur(t[(i+1)*taille+j+1]);
        a=t_norm[(i+1)*taille+j+1].x; b=t_norm[(i+1)*taille+j+1].y; c=t_norm[(i+1)*taille+j+1].z;  glNormal3f(a,b,c);
        glVertex3f((float)(i+1)/taille,(float)(j+1)/taille,t[(i+1)*taille+j+1]);
        if (couleurs_continues) mont_couleur(t[i*taille+j+1]);
        a=t_norm[i*taille+j+1].x; b=t_norm[i*taille+j+1].y; c=t_norm[i*taille+j+1].z;  glNormal3f(a,b,c);
        glVertex3f((float)i/taille,(float)(j+1)/taille,t[i*taille+j+1]);
        glEnd();      
      }
  }
  else
  {
    for(i=0;i<taille-1;i++)
      for(j=0;j<taille-1;j++)
      {
        prod_vect(1.0/taille,0,t[(i+1)*taille+j]-t[i*taille+j],0,1.0/taille,t[i*taille+j+1]-t[i*taille+j],&a,&b,&c);
	glNormal3f(a,b,c);
	glBegin(GL_QUADS);
        mont_couleur(t[i*taille+j]);
        glVertex3f((float)i/taille,(float)j/taille,t[i*taille+j]);
        if (couleurs_continues) mont_couleur(t[(i+1)*taille+j]);
        glVertex3f((float)(i+1)/taille,(float)j/taille,t[(i+1)*taille+j]);
        if (couleurs_continues) mont_couleur(t[(i+1)*taille+j+1]);
        glVertex3f((float)(i+1)/taille,(float)(j+1)/taille,t[(i+1)*taille+j+1]);
        if (couleurs_continues) mont_couleur(t[i*taille+j+1]);
        glVertex3f((float)i/taille,(float)(j+1)/taille,t[i*taille+j+1]);
        glEnd();      
      }
  }	 
// eau
  float eau_emission [4] = {0,0.1,0.2,1};
  glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,30);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,eau_spec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,eau_diff);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,eau_amb);
  //glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,eau_emission);
  glColor4f(0.2,0.4,0.7,0.9);
/*  float vagues_amplitude = 0.005 ;
  float vx = vagues_k[0] ;
  float vy = vagues_k[1] ;
  float x,y,z ;*/
  if (normales_moyennes)
    {
    for(a=0.0;a<taille-1;a++)
      {
      for(b=0.0;b<taille-1;b++)
        {
        glBegin(GL_QUADS);
        point_vague(a,b);
        point_vague(a+1,b);
        point_vague(a+1,b+1);
        point_vague(a,b+1);
        glEnd();
        }
      }
    }
  else
    {
    vagues_sans_normales_moyennes();
    }  
  for(i=0;i<nb_ondes;i++)
    vagues_phase[i] = vagues_phase[i] + vagues_vitesse[i] ;
  glutSwapBuffers();
  glutPostRedisplay();
}


void lumiere(void)
{
  double a = sin(3.141592653589793238462643832795*(light_rot/180)) ;
  light1_diff [1] = 0.5 + a/2;
  light1_diff [2] = 0.3 + a;
  light1_spec [1] = a;
  light1_spec [2] = a;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light1_diff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light1_spec);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light1_amb);
  light1_diff[1]=light1_diff[1]*light1_diff[1];
  light1_diff[2]=light1_diff[2]*light1_diff[2];
  glPushMatrix();
  glRotated(light_rot,-1,-1,0);
    glPushMatrix();
    glTranslated(2,0,0);
      glColor3f(1,1,0.8);
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,light1_diff);
      glutSolidSphere(0.1,20,20);
    glPopMatrix();
    glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
  glPopMatrix();
  if (light_rot_on) light_rot=light_rot+0.00005 * taille * taille ;

}


void mont_couleur(float z)
{
  float r,g,b ;
  if (z>-0.01) {
    r = 0.0009/(0.001+z*z)+z*z/0.05 ; 
    g = 0.4 +(z+0.1)*(z-0.12)*(z-0.15)*130 ;
    b = 0.1 + z*z*(z-0.1)/0.018;
  }
  else {
  r = 0.1 ;
  g = 0.2 ;
  b = 0.5 ;
  }
  glColor4d(r,g,b,1);
}

void point_vague (float a,float b)
{
  float x=0,y=0,z=0 ;
  float vx,vy ;
  int i ;
  for(i=0;i<nb_ondes;i++)
  {
    vx = vagues_k[2*i];
    vy = vagues_k[2*i+1];
    x = x + vx*taille*sin(a*vx+b*vy + vagues_phase[i])*vagues_amplitude[i];
    y = y + vy*taille*sin(a*vx+b*vy + vagues_phase[i])*vagues_amplitude[i];
  }
  glNormal3f(x,y,1);
  for(i=0;i<nb_ondes;i++)
  {
    vx = vagues_k[2*i];
    vy = vagues_k[2*i+1];
    z = z + cos(a*vx+b*vy + vagues_phase[i])*vagues_amplitude[i];
  }
  glVertex3d(a/taille,b/taille,z);
}

void point_vague_sans_normale (float a, float b)
{
  int i ;
  float z=0, vx, vy;
  for(i=0;i<nb_ondes;i++)
  {
    vx = vagues_k[2*i];
    vy = vagues_k[2*i+1];
    z = z + cos(a*vx+b*vy + vagues_phase[i])*vagues_amplitude[i];
  }
  glVertex3d(a/taille,b/taille,z);
}


void vagues_sans_normales_moyennes(void)
{
  float x=0,y=0 ;
  float vx,vy,a,b ;
  int i ;
    for(a=0.0;a<taille-1;a++)
      {
      for(b=0.0;b<taille-1;b++)
        {
        for(i=0;i<nb_ondes;i++)
	  {
  	  vx = vagues_k[2*i];
	  vy = vagues_k[2*i+1];
	  x = x + vx*taille*sin(a*vx+b*vy + vagues_phase[i])*vagues_amplitude[i];
	  y = y + vy*taille*sin(a*vx+b*vy + vagues_phase[i])*vagues_amplitude[i];
	  }
	glNormal3f(x,y,1);
	glBegin(GL_QUADS);
        point_vague_sans_normale(a,b);
        point_vague_sans_normale(a+1,b);
        point_vague_sans_normale(a+1,b+1);
        point_vague_sans_normale(a,b+1);
        glEnd();
        }
      }
    
}


void orage(void)
{
  
}


void eclair(void)
{
  
}


void nuage(void)
{
  int i ;
  float emission [4] = {0.2,0.2,0.2,1};
  float vent = 0.005 ;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,lumierenulle);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,lumierenulle);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mont_diff);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission);
  glPushMatrix();
  glColor4f(1,1,1,1);
  for(i=0;i<nuage_nbspheres;i++)
  {
    glPushMatrix();
    glTranslatef(nuage_coords[i][0],nuage_coords[i][1],nuage_coords[i][2]);
    glutSolidSphere(nuage_rayons[i],20,20);
    nuage_coords[i][0]=nuage_coords[i][0] +0.005*(rand()%100-50)/100 +vent ;
    nuage_coords[i][1]=nuage_coords[i][1] +0.005*(rand()%100-50)/100 - vent;
//    nuage_coords[i][2]=nuage_coords[i][2] +0.005*(rand()%100-50)/100;
    glPopMatrix();
  }
  if (nuage_coords[0][0]>0.5) init_nuages();
  glPopMatrix();
}

void init_nuages(void)
{
  int i;
  float a,r;
  for(i=0;i<nuage_nbspheres;i++)
    {
    a = rand();
    r = 0.2*(rand()%100-50)/100; //+ 2*(rand()%2)-1);
    nuage_coords[i][0] = r*cos(a) - 0.5;
    nuage_coords[i][1] = r*sin(a) + 0.5;
    nuage_coords[i][2] =0.4+0.05*(rand()%100-50)/100;
    nuage_rayons[i]=0.1*(rand()%100 + 20)/nuage_nbspheres ;
    }
}

void autour(void)
{
  int i ;
  float max=6,min=-5,top=5;
  float sol_emission [4] = {0.3,0.9,0.1,1}, ciel_emission [4] = {0.1,0.2,0.9,1};
  if(environnement_on)
  {
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,sol_emission);
   // glColor4f(0.3,0.9,0.1,1);
    glBegin(GL_QUADS);
    glVertex3f(max,0,0);
    glVertex3f(max,min,0);
    glVertex3f(min,min,0);
    glVertex3f(min,0,0);
    glVertex3f(max,1,0);
    glVertex3f(max,max,0);
    glVertex3f(min,max,0);
    glVertex3f(min,1,0);
    glVertex3f(1,0,0);
    glVertex3f(1,1,0);
    glVertex3f(max,1,0);
    glVertex3f(max,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glVertex3f(min,1,0);
    glVertex3f(min,0,0);
//    glColor4f(0.1,0.2,0.9,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ciel_emission);
    glVertex3f(max,max,top);//haut
    glVertex3f(max,min,top);
    glVertex3f(min,min,top);
    glVertex3f(min,max,top);
    glVertex3f(max,max,0);//
    glVertex3f(max,min,0);
    glVertex3f(max,min,top);
    glVertex3f(max,max,top);
    glVertex3f(min,max,0);//
    glVertex3f(max,max,0);
    glVertex3f(max,max,top);
    glVertex3f(min,max,top);
    glVertex3f(min,max,0);//
    glVertex3f(min,max,top);
    glVertex3f(min,min,top);
    glVertex3f(min,min,0);
    glVertex3f(max,min,0);//
    glVertex3f(max,min,top);
    glVertex3f(min,min,top);
    glVertex3f(min,min,0);
    glEnd();
  }
  else
  {
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,lumierenulle);
    glColor4f(0,0,0,1);
    for(i=0;i<taille-1;i++)
    {
      glBegin(GL_QUADS);
      glVertex3f((float)i/taille,0,-hauteur_init);
      glVertex3f((float)i/taille,0,t[i*taille]);
      glVertex3f((float)(i+1)/taille,0,t[(i+1)*taille]);
      glVertex3f((float)(i+1)/taille,0,-hauteur_init);
      glVertex3f((float)i/taille,1-1./taille,-hauteur_init);
      glVertex3f((float)i/taille,1-1./taille,t[i*taille+taille-1]);
      glVertex3f((float)(i+1)/taille,1-1./taille,t[(i+1)*taille+taille-1]);
      glVertex3f((float)(i+1)/taille,1-1./taille,-hauteur_init);
      glVertex3f(0,(float)i/taille,-hauteur_init);
      glVertex3f(0,(float)i/taille,t[i]);
      glVertex3f(0,(float)(i+1)/taille,t[(i+1)]);
      glVertex3f(0,(float)(i+1)/taille,-hauteur_init);
      glVertex3f(1-1./taille,(float)i/taille,-hauteur_init);
      glVertex3f(1-1./taille,(float)i/taille,t[taille*(taille-1)+i]);
      glVertex3f(1-1./taille,(float)(i+1)/taille,t[taille*(taille-1)+(i+1)]);
      glVertex3f(1-1./taille,(float)(i+1)/taille,-hauteur_init);
      glEnd();
      
    }
  }
}
/* chutes de DRAW
vagues_amplitude*(1+(rand()%11-5)/10)
tentative infructueuse pour g̩rer la profondeur de l'eau :
  float eau_niveau = 0 ;  
  for(eau_niveau=-0.001;eau_niveau>-hauteur_init;eau_niveau=eau_niveau-0.001)
  {
    a=(hauteur_init+eau_niveau)*(hauteur_init+eau_niveau)/(hauteur_init*hauteur_init);
    glColor4f(0.1*a,0.2*a,0.8*a,1.5-hauteur_init/(hauteur_init-eau_niveau));
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    glColor4f(0.1*a,0.2*a,0.8*a,1.5-hauteur_init/(hauteur_init-eau_niveau));
    glVertex3d(0,0,eau_niveau);
    glVertex3d(1,0,eau_niveau);
    glVertex3d(1,1,eau_niveau);
    glVertex3d(0,1,eau_niveau);
    glEnd();
  }


pour mettre les normales associ̩e aux faces et pas aux points :
prod_vect(1.0/taille,0,t[(i+1)*taille+j]-t[i*taille+j],0,1.0/taille,t[i*taille+j+1]-t[i*taille+j],&a,&b,&c);




      glNormal3f(vx*taille*sin(a*vx+b*vy + vagues_phase)*vagues_amplitude, vy*taille*sin(a*vx+b*vy + vagues_phase)*vagues_amplitude,1);
      glVertex3d(a/taille,b/taille,cos(a*vx+b*vy + vagues_phase)*vagues_amplitude);
      glNormal3f(vx*taille*sin((a+1)*vx+b*vy + vagues_phase)*vagues_amplitude, vy*taille*sin((a+1)*vx+b*vy + vagues_phase)*vagues_amplitude,1);
      glVertex3d((a+1)/taille,b/taille,cos((a+1)*vx+b*vy + vagues_phase)*vagues_amplitude );
      glNormal3f(vx*taille*sin((a+1)*vx+(b+1)*vy + vagues_phase)*vagues_amplitude, vy*taille*sin((a+1)*vx+(b+1)*vy + vagues_phase)*vagues_amplitude,1);
      glVertex3d((a+1)/taille,(b+1)/taille,cos((a+1)*vx+(b+1)*vy + vagues_phase)*vagues_amplitude );
      glNormal3f(vx*taille*sin(a*vx+(b+1)*vy + vagues_phase)*vagues_amplitude, vy*taille*sin(a*vx+(b+1)*vy + vagues_phase)*vagues_amplitude,1);
      glVertex3d(a/taille,(b+1)/taille,cos(a*vx+(b+1)*vy + vagues_phase)*vagues_amplitude );
*/

void clavier(unsigned char key, int x, int y)
{
  float pas_pos = 0.03 ;
  float pas_rot = 2 ;
  int parami ;
  float * paramfv ;
  int do_reshape = 0;
  switch (mode_gestion_camera)
  {
    case 0 :
      switch(key)
      {
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
         case 'e' : light_rot_on = 1-light_rot_on ; break;
         case 'c' : couleurs_continues = 1-couleurs_continues ; break;
         case 'n' : normales_moyennes = 1-normales_moyennes ; break;
         case 'u' : lune_rot_on = 1-lune_rot_on ; break;
         case 'f' : glGetIntegerv(GL_FOG_MODE,&parami) ; 
		    switch(parami){
			case GL_LINEAR : glFogf(GL_FOG_MODE,GL_EXP);break;
			case GL_EXP : glFogf(GL_FOG_MODE,GL_EXP2);break;
			case GL_EXP2 : glFogf(GL_FOG_MODE,GL_LINEAR);break;
			}break;
         case 'g' : fog_density = 0.1+fog_density ; break;
         case 'h' : fog_density = fog_density-0.1 ; break;
         case 'v' : environnement_on = 1-environnement_on ; break;
         case 'b' : nuage_on = 1-nuage_on ; break;
      }
      break ;
    case 1 :
      glMatrixMode(GL_MODELVIEW) ;
      glPopMatrix() ;
      switch(key)
      {
         case 'q' : glTranslated(pas_pos,0,0) ;break;
         case 'd' : glTranslated(-pas_pos,0,0) ;break;
         case 'z' : glTranslated(0,0,pas_pos) ;break ;
         case 's' : glTranslated(0,0,-pas_pos) ;break ;
         case 'a' : glTranslated(0,pas_pos,0) ;break ;
         case 'w' : glTranslated(0,-pas_pos,0) ;break ;
 
      }
      glPushMatrix();
      break ;
  }
  glFogf(GL_FOG_DENSITY,fog_density);
  if (do_reshape)
  {
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

void clavier_spec(int key, int x, int y)
{
  glMatrixMode(GL_MODELVIEW);
  switch(key)
  {
    case GLUT_KEY_F1 : glLoadIdentity() ; mode_gestion_camera = 0 ; break ;
    //case GLUT_KEY_F2 : glLoadIdentity() ; mode_gestion_camera = 1 ; break ;
  }
}

void souris_passive (int x, int y)
{
  souris_x = x ;
  souris_y = y ;
}

void souris_drag (int x, int y)
{
  float v = 0.5 ;
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glRotated((x-souris_x)*v,1,0,0) ;
  glRotated((y-souris_y)*v,0,1,0) ;
  souris_x = x ;
  souris_y = y ;
  glPushMatrix();
}

void init_vagues (void)
{
  int i ;
  float coeff_k = 1./200 ; //(float) 25000/((float)200*taille);
  for(i=0;i<nb_ondes;i++)
  {
    vagues_k [2*i] = (float)(rand()%100)*coeff_k ;
    vagues_k [2*i+1] = (float)(rand()%100)*coeff_k;
    vagues_phase [i] = (float)(rand()%10);
    vagues_vitesse [i] = (float)(rand()%100)/300;
    vagues_amplitude [i] = 0.005*(float)(rand()%100)/100 ;
  }
}

void prod_vect(float x1, float y1, float z1, float x2, float y2, float z2, float * a, float * b, float * c)
{
  *a = y1*z2 - z1*y2 ;
  *b = z1*x2 - x1*z2 ;
  *c = x1*y2 - y1*x2 ;
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

  /*cam_dir_x = 1 ;
  cam_dir_y = 1 ;
  cam_dir_z = -0.5 ;
  cam_up_x = 0 ;
  cam_up_y = 0 ; 
  cam_up_z = 1 ;*/
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
//  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquationEXT(GL_FUNC_ADD_EXT);
  glLightModeliv(GL_LIGHT_MODEL_AMBIENT,gamb);
  glEnable(GL_FOG);
  glFogf(GL_FOG_MODE,GL_EXP2);//GL_LINEAR,GL_EXP,GL_EXP2
  glFogf(GL_FOG_DENSITY,fog_density);
  float fog [4] = {1,1,1,1};
  glFogfv(GL_FOG_COLOR,fog);
}




