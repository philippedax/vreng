/* springgl.c   spring and mass  command line  s for single buffer */

#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265
#endif

static int winWidth = 600;
static int winHeight = 400;
static int xm, ym, bm;    // mouse position
static int xc = 300;      // disc/rectangle location
static int yc = 275;
static int xp;            // last mouse location during drag
static int yp;
static int tracking = 0;  // tracking mouse motion
static int loose = 0;     // loose to move
static double x = 0.0;    // integrated position
static double vx = 0.0;   // integrated velocity
static int double_k1 = 0; // yellow buttons
static int double_k2 = 0;
static int disc = 0;
static int double_buffer = 1;
static GLUquadricObj *obj;

int in_rect(int x, int y, int w, int h)
{
  return xm>x && xm<x+w && (400-ym)>y && (400-ym)<y+h;
}

void mouseMotion(int x, int y)
{
  xm = x;
  ym = y;
  bm = 1;  
  if(tracking)
  {
    /* printf("mouse motion x=%d, y=%d \n", x, y); */
    xc = xc + (xm-xp); // mouse moved (new-old)
    xp = xm;
    yp = ym;
    if(xc<96)xc=96;
    if(xc>504)xc=504;
  }
  glutPostRedisplay();  
}

void mouseButton(int button, int state, int x, int y)
{
  xm = x;
  ym = y;
  bm = 1;  

  if(state==GLUT_DOWN)
  {
    /* printf("mouse down x=%d, y=%d, track=%d \n", x, y, tracking); */
    if(in_rect(xc-75, yc-75, 150, 150))
    {
      tracking = 1;
      loose = 0;
      xp = xm;
      yp = ym;
    }
    if(in_rect(220,150,160,50)) disc = 1-disc;
    if(in_rect(40,150,150,40)) double_k1 = 1-double_k1;
    if(in_rect(410,150,150,40)) double_k2 = 1-double_k2;
  }
  if(state==GLUT_UP)
  {
    /* printf("mouse up x=%d, y=%d, track=%d \n", x, y, tracking); */
    tracking = 0;
    loose = 1;
  }
  glutPostRedisplay();
}

void physicsComputation(void)
{
  double ax, f1, f2, f;
  double d1, d2, xold;
  double dt = 0.1;
  double mass = 1.0;
  double k = 1.0;     // spring constant
    
  if(!loose)
  {
    x = 0.0;
    vx = 0.0;
    ax = 0.0;
    return;
  }

  // Get spring forces
  d1 = (xc-20)/280.0;
  d2 = (580-xc)/280.0;
    
  f1 = k * d1;
  if(double_k1) f1 = f1*2.0;

  f2 = k * d2;
  if(double_k2) f2 = f2*2.0;

  f  = f1 - f2;      // d2 bigger, force to left, smaller x
    
  xold = x;
  ax = f / mass;     // f = ma  or  a = f / m
  vx = vx + ax * dt; // v = integral a dt
  x = x + vx * dt;   // x = integral v dt
  xc = xc + (int)((xold - x)*35.0);
  if(xc<96) { xc=96; vx=0.0; }
  if(xc>504) { xc=504; vx=0.0; }
  glutPostRedisplay();
}

void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  winWidth = w;
  winHeight = h;
  glutPostRedisplay();
}

static void printstring(int x, int y, char *string)
{
   int len, i;

   glRasterPos2i(x, y);
   len = (int) strlen(string);
   for (i = 0; i < len; i++)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
} /* end printstring */

void display(void)
{
  double x=0.0, xx=0.0;
  double y=0.0, yy=0.0;
  double xn,yn,xs;
  double a;
  double b;
  double t=0.0;
  double f1;
  double f2;
  int i;
  
  a = 1.0/(4.0*M_PI);
  b = 2.0*a;

  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0, 0.0, 0.0);
  printstring(20, 380, "drag and drop disc to see motion");
  printstring(20, 80, "Basic physics:"); /* 20, 310 */
  printstring(20, 60, "f = k x  the force from a spring is proportional to distance stretched times spring constant, k");
  printstring(20,40, "f = m a  the force on a body of mass, m, is proportional to the acceleration, a");
  printstring(20,20, "acceleration a = dv / dt    velocity v = dx / dt");

  glColor3f(0.0, 0.0, 0.0); /* box */
  glLineWidth(1.0);
  glBegin(GL_LINE_LOOP); 
    glVertex2f(20, 200);
    glVertex2f(20, 360);
    glVertex2f(580,360);
    glVertex2f(580,200);
  glEnd();

  glColor3f(1.0, 0.0, 0.0); /* mass */
  if(disc)
  {
    glTranslatef(xc, yc, 0);
    gluDisk(obj, 75, 75, 20, 20);
    glTranslatef(-xc, -yc, 0);
  }
  if(!disc)
  {
    glLineWidth(1.0);
    glBegin(GL_LINE_LOOP); 
      glVertex2f(xc-74, yc-74);
      glVertex2f(xc-74, yc+74);
      glVertex2f(xc+74, yc+74);
      glVertex2f(xc+74, yc-74);
    glEnd();
  }

  glColor3f(0.0, 0.0, 1.0); /* spring ends */
  glPointSize(5.0);
  glBegin(GL_POINTS); 
    glVertex2f(xc, yc);
    glVertex2f(20, 275);
    glVertex2f(580,275);
  glEnd();

  glColor3f(0.0, 1.0, 1.0); /* button */
  glBegin(GL_POLYGON);
    glVertex2f(40, 150);
    glVertex2f(40, 190);
    glVertex2f(190,190);
    glVertex2f(190,150);
  glEnd();
  glColor3f(0.0, 0.0, 0.0);
  if(!double_k1) printstring(45, 170, "double left spring k");
  if(double_k1)  printstring(45, 170, "halve left spring");
  printstring(45, 155, "note frequency change");

  glColor3f(0.0, 1.0, 1.0); /* button */
  glBegin(GL_POLYGON);
    glVertex2f(410, 150);
    glVertex2f(410, 190);
    glVertex2f(560,190);
    glVertex2f(560,150);
  glEnd();
  glColor3f(0.0, 0.0, 0.0);
  if(!double_k2) printstring(415, 170, "double right spring k");
  if(double_k2)  printstring(415, 170, "halve right spring");
  printstring(415, 155, "note frequency change");

  glColor3f(0.0, 1.0, 1.0); /* button */
  glBegin(GL_POLYGON);
    glVertex2f(220, 150);
    glVertex2f(220, 190);
    glVertex2f(380,190);
    glVertex2f(380,150);
  glEnd();
  glColor3f(0.0, 0.0, 0.0);
  if(disc)  printstring(225, 170, "Change disc to rectangle");
  if(!disc) printstring(225, 170, "Change rectangle to disc");

  glColor3f(0.0, 0.0, 1.0);
  f1 = (double)(xc-20)/280.0;  // factors < 0.5 or > 1.5 not very visible
  f2 = (double)(580-xc)/280.0;

  t = 0.0;    
  y = 0.0;
  x = 0.0;
  if(double_k1) glLineWidth(2.0);
  if(!double_k1) glLineWidth(1.0);
  while(t<32.0*M_PI) // heuristic
  {
    t = t+0.1; // 0.05 flicker
    yn = -(b - b*cos(t));
    xn = f1*a*t + b*sin(t);
    glBegin(GL_LINES); 
      glVertex2f(20+(int)(x *35.0), 275+(int)(y *200.0));
      glVertex2f(20+(int)(xn*35.0), 275+(int)(yn*200.0));
    glEnd();
    x = xn;
    y = yn;
  }    

  t = 0.0;
  x = 0.0;
  y = 0.0;
  if(double_k2) glLineWidth(2.0);
  if(!double_k2) glLineWidth(1.0);
  while(t<32.0*M_PI) // heuristic
  {
    t = t+0.1; // 0.05 flicker
    yn = -(b - b*cos(t));
    xn = f2*a*t +  b*sin(t);
    glBegin(GL_LINES); 
      glVertex2f(xc+(int)(x *35.0), 275+(int)(y *200.0));
      glVertex2f(xc+(int)(xn*35.0), 275+(int)(yn*200.0));
    glEnd();
    x = xn;
    y = yn;
  }    
  
  glFlush();  
  if(double_buffer) glutSwapBuffers(); /* only for double */
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  if(argc>1) if(argv[1][0]=='s') double_buffer = 0;
  if(double_buffer)  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  if(!double_buffer) glutInitDisplayMode(GLUT_RGB); /* no double buffer */
  glutInitWindowSize(winWidth, winHeight);
  glutInitWindowPosition(100,100); 
  glutCreateWindow(argv[0]);
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMotion);
  glutIdleFunc(physicsComputation);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLfloat)winWidth, 0.0, (GLfloat)winHeight);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(1.0, 1.0, 1.0, 0.0);
  obj = gluNewQuadric(); /* used for disc */
  gluQuadricDrawStyle(obj, GLU_LINE);
  glutMainLoop();
  return 0;
}

