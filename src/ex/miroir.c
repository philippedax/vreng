/* Source: http://www.opengl.org/resources/faq/technical/mirror.c */

#include "GL/glut.h"
#include <stdlib.h>
#include <stdio.h>

#define USE_STENCIL 1

#define TIMER 33

static float xrot=0;
static float miroirX=2;

// Cette fonction affiche la scène et ne fait rien de spécial pour le miroir
static void dessineScene( GLenum ordre )
{
   // positionner la lumière
   GLfloat pos[4] = { 1.8, 5, 1.8, 1 };
   glLightfv( GL_LIGHT1, GL_POSITION, pos );

   glPushAttrib( GL_ENABLE_BIT );

   // ne pas afficher les faces dont les normales pointent vers l'arrière
   // (en supposant que les faces sont affichées dans l'"ordre")
   glEnable( GL_CULL_FACE );
   glCullFace( GL_BACK );
   glFrontFace( ordre );

   // les quatre murs
   glBegin( GL_QUADS );
   glColor3f( 1, 1, .5 );
   glNormal3f( 1, 0, 0 );
   glVertex3f( -3,  3,  3 );
   glVertex3f( -3, -3,  3 );
   glVertex3f( -3, -3, -3 );
   glVertex3f( -3,  3, -3 );
   glNormal3f( 0, 0, 1 );
   glVertex3f( -3,  3, -3 );
   glVertex3f( -3, -3, -3 );
   glVertex3f(  3, -3, -3 );
   glVertex3f(  3,  3, -3 );
   glNormal3f( -1, 0, 0 );
   glVertex3f(  3,  3, -3 );
   glVertex3f(  3, -3, -3 );
   glVertex3f(  3, -3,  3 );
   glVertex3f(  3,  3,  3 );
   glNormal3f( 0, 0, -1 );
   glVertex3f(  3,  3,  3 );
   glVertex3f(  3, -3,  3 );
   glVertex3f( -3, -3,  3 );
   glVertex3f( -3,  3,  3 );
   // le sol
   glColor3f( .5, .5, 1 );
   glNormal3f( 0, -1, 0 );
   glVertex3f(  3, -3,   3 );
   glVertex3f(  3, -3,  -3 );
   glVertex3f( -3, -3,  -3 );
   glVertex3f( -3, -3,   3 );
   glEnd( );
   glDisable( GL_CULL_FACE );

   // les cylindres
   static GLUquadric *quad = NULL;
   if ( quad == NULL ) quad = gluNewQuadric(  );

   // un cylindre au milieu de la pièce
   glColor3f( .5, .5, 1 );
   glPushMatrix( );
   glRotatef( 5*xrot, 1, 0, 0 );
   glTranslatef( 1, 0, -2 );
   gluCylinder( quad, .25, .25, 4, 18, 8 );
   glPushMatrix(  );
   glTranslatef( 0, 0, 3 );
   gluDisk( quad, 0, .25, 18, 1 );
   glPopMatrix(  );
   glPushMatrix(  );
   glScalef( 1, 1, -1 );
   gluDisk( quad, 0, .25, 18, 1 );
   glPopMatrix( );
   glPopMatrix( );

   // un autre cylindre entre le miroir et le mur
   glColor3f( 1, .5, .5 );
   glPushMatrix( );
   glRotatef( -2*xrot, 1, 0, 0 );
   glTranslatef( -2.5, 0, -2 );
   gluCylinder( quad, .25, .25, 4, 18, 8 );
   glPushMatrix(  );
   glTranslatef( 0, 0, 3 );
   gluDisk( quad, 0, .25, 18, 1 );
   glPopMatrix(  );
   glPushMatrix(  );
   glScalef( 1, 1, -1 );
   gluDisk( quad, 0, .25, 18, 1 );
   glPopMatrix( );
   glPopMatrix( );

#if 1
   // le teapot
   glColor3f( .8, .75, .7 );
   glPushMatrix(  );
   glTranslatef( -.5, -1.3, -1 );
   glRotatef( -90, 0, 1, 0 );
   glutSolidTeapot( .8 );
   glPopMatrix(  );
#endif

   glPopAttrib( );
}

// Cette fonction affiche le miroir
static void dessineMiroir( void )
{
   glBegin( GL_QUADS );
   glVertex3f( -miroirX,  1,  2 );
   glVertex3f( -miroirX, -1,  2 );
   glVertex3f( -miroirX, -1, -2 );
   glVertex3f( -miroirX,  1, -2 );
   glEnd(  );
}

// Cette fonction affiche le miroir
static void dessineSceneReflechie( void )
{
   glPushMatrix(  );

   // a) faire une translation pour amener le plan de réflexion à la position du miroir
   glTranslatef( -miroirX, 0, 0 );

   // b) le miroir est dans le plan YZ; faire une réflexion par -1 en X
   glScalef( -1, 1, 1 );

#if 1
   // c) mettre un plan de clipping à la position du miroir afin d'éliminer
   // les réflexions des objets qui sont à l'arrière du miroir
   const GLdouble equation[] = { 1, 0, 0, 0 };
   glClipPlane( GL_CLIP_PLANE0, equation );
   glEnable( GL_CLIP_PLANE0 );
#endif

   // d) faire la translation inverse
   glTranslatef( miroirX, 0, 0 );

   // e) tracer la scène
   dessineScene( GL_CW );
   glDisable( GL_CLIP_PLANE0 );

   glPopMatrix(  );
}

static void display( void )
{
   // Donner la position de la caméra
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity(  );
   gluLookAt( 0, 1, 7,  -1, 0, 0,  0, 1, 0 );
   // et faire une rotation: tourner la scène par rapport à la caméra
   glRotatef( -.4*xrot, 0, 1, 0 );

#if USE_STENCIL

   // 0) Effacer l'écran: les couleurs, les valeurs de Z et le stencil
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

   // 1) Remplir le stencil avec le miroir (on veut des 1 pour tous les
   // pixels du miroir, 0 partout ailleurs), mais on ne veut rien tracer à
   // l'écran pour le moment.  On s'organise donc pour que le test du
   // stencil ne passe jamais tout en remplissant le stencil de 1.
   glEnable( GL_STENCIL_TEST );
   glStencilFunc( GL_NEVER, 1, 1 );
   glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
   dessineMiroir( );

   // 2) Maintenant que le stencil est rempli de 1 à la position du miroir,
   // on trace la scène réfléchie.  On fait en sorte que seulement la région
   // du stencil avec des 1 soit tracée (c'est-à-dire seulement la région du miroir)
   glStencilFunc( GL_EQUAL, 1, 1 );
   glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
   dessineSceneReflechie(  );
   glDisable( GL_STENCIL_TEST );

#else

   // 0) Effacer l'écran: les couleurs et les valeurs de Z
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // 1) Tracer d'abord la scène réfléchie
   dessineSceneReflechie(  );

   // 2) Mettre tous les z de la scène réfléchie à la profondeur maximale (=1)
   glClear( GL_DEPTH_BUFFER_BIT );

#endif

   // 3) Tracer maintenant le miroir comme une vitre presque complètement
   // transparente. Ça mettra les bonnes valeurs de z pour le miroir.
   glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
   glColor4f( 0, 0, 0, .1 );
   dessineMiroir( );
   glPopAttrib( );

   // 4) Tracer la scène normalement
   dessineScene( GL_CCW );

   // Et c'est tout!
   glutSwapBuffers( );
   // y'a une erreur ?
   {
      int err = glGetError( );
      if( err != GL_NO_ERROR)
         printf( "Error %x\n", err );
   }
}

static void reshape( int w, int h )
{
   glViewport( 0, 0, w, h );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity(  );
   gluPerspective( 50, (float)w/(float)h, 1, 20 );
}

static void cbMainMenu( int value )
{
   if( value == 99 ) exit( 0 );
}

static void timer( int value )
{
   xrot += 1;
   if( xrot > 360 ) xrot -= 360;

   glutPostRedisplay(  );
   glutTimerFunc( TIMER, timer, 0 );
}

static void init( void )
{
   GLfloat blanc[4] = { 1, 1, 1, 1 };

   // la lumière
   glEnable( GL_LIGHTING );
   glEnable( GL_LIGHT1 );
   glLightfv( GL_LIGHT1, GL_DIFFUSE, blanc );
   glLightfv( GL_LIGHT1, GL_SPECULAR, blanc );
   // ambient et diffuse suivront glColor
   glEnable( GL_COLOR_MATERIAL );
   glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
   glMaterialfv( GL_FRONT, GL_SPECULAR, blanc );
   glMaterialf( GL_FRONT, GL_SHININESS, 20 );

   glDisable( GL_DITHER );
   glEnable( GL_DEPTH_TEST );

   glutDisplayFunc( display );
   glutReshapeFunc( reshape );
   glutTimerFunc( TIMER, timer, 0 );

   glutCreateMenu( cbMainMenu );
   glutAddMenuEntry( "Quit", 99 );
   glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int main( int argc, char** argv )
{
   glutInit( &argc,argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL );
   glutInitWindowSize( 300, 250 );
   glutInitWindowPosition( 0, 0 );
   glutCreateWindow( "miroir" );

   init(  );

   glutMainLoop(  );
   return( 0 );
}
