/*
 * wheel asynchronous progress indicator
 *
 * Philippe Dax - Mars 2009
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>


static void tooth(GLfloat r1, GLfloat r2)
{
  GLfloat w = (r2 - r1) / 2.5;

  glShadeModel(GL_FLAT);
  glNormal3f(0.0, 0.0, 1.0);

  /* draw front side of one tooth */
  glBegin(GL_QUADS);
  glVertex3f(r1, - w/2, 0);
  glVertex3f(r2, - w/2, 0);
  glVertex3f(r2, + w/2, 0);
  glVertex3f(r1, + w/2, 0);
  glEnd();
}

static GLint wheel;
static GLfloat angle = 0.0;

static void render(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
    //glScalef(0.25, 0.25, 0.25);
    //glRotatef(angle, 0.0, 0.0, 1.0);
    glRotatef(+2.0 * angle - 25.0, 0.0, 0.0, 1.0);
    glCallList(wheel);
  glPopMatrix();

  glutSwapBuffers();

static void idle(void)
{
  angle -= 1.5;
  glutPostRedisplay();
}

static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

static void init(int argc, char *argv[])
{
  static GLfloat color[4] = {0.0, 0.0, 0.0, 1.0};
  static GLfloat bg[4] = {0.2, 0.2, 1.0, 1.0};
  GLint i;

  glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glClearColor(bg[0], bg[1], bg[2], bg[3]);

  wheel = glGenLists(1);
  glNewList(wheel, GL_COMPILE);

  int teeth = 12;
  float r1 = 1;
  float r2 = 2;
  for (i=0; i<teeth ; i++) {
    color[0] = color[1] = color[2] = 0; // noir
    color[3] = (float) (i)/teeth;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glPushMatrix();
     float a = -i * 360 / teeth;
     glRotatef(a, 0, 0, 1);
     tooth(r1, r2);
    glPopMatrix();
  }
  glEndList();

  //glEnable(GL_NORMALIZE);
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  glutInitWindowPosition(0, 0);
  glutInitWindowSize(300, 300);
  glutCreateWindow("wheel");
  init(argc, argv);

  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);

  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
