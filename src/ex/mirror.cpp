//-----------------------------------------------------------------------------
// File: mirror.cpp
// Desc: sample shows a simple mirror placed on the xy plane
//       this a special case of mirror placement (greatly simplified)
//       the general method:
//           - make stencil mask of mirror pane
//           - draw inverted mirror image with stencil mask
//           - draw mirror pane to depth buffer
//           - draw normal scene without stencil
//           - blend mirror pane for effect
//
// Autumn 2000 - Ge Wang, Eric McGimpsey, Christina Hsu
//-----------------------------------------------------------------------------
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>




//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
void idleFunc( );
void displayFunc( );
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void initialize( );

void render();
void mirror( GLfloat val );



//-----------------------------------------------------------------------------
// global variables and #defines
//-----------------------------------------------------------------------------
#define INC_VAL     2.0f
#ifndef M_PI
#define M_PI        3.14159265359
#endif

// width and height of the window
GLsizei g_width = 480;
GLsizei g_height = 360;

// whether to animate
GLboolean g_rotate = GL_TRUE;

// light 0 position
GLfloat g_light0_pos[4] = { 10.0f, 5.0f, 0.0f, 1.0f };

// clipping planes
GLdouble eqn1[4] = { 0.0, 0.0, 1.0, 0.0 };

GLfloat g_inc = 0.0f;


//-----------------------------------------------------------------------------
// Name: main( )
// Desc: entry point
//-----------------------------------------------------------------------------
int main( int argc, char ** argv )
{
    // initialize GLUT
    glutInit( &argc, argv );
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH |
			 GLUT_STENCIL );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow( "Mirror Sample" );

    // set the idle function - called when idle
    glutIdleFunc( g_rotate ? idleFunc : NULL );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );

    // do our own initialization
    initialize();

    // let GLUT handle the current thread from here
    glutMainLoop();

    return 0;
}




//-----------------------------------------------------------------------------
// Name: initialize( )
// Desc: sets initial OpenGL states and initializes any application data
//-----------------------------------------------------------------------------
void initialize()
{
    // set the GL clear color - use when the color buffer is cleared
    glClearColor( 0.0f, 0.0f,0.0f, 1.0f );
    // set the shading model to 'smooth'
    glShadeModel( GL_SMOOTH );
    // enable depth
    glEnable( GL_DEPTH_TEST );
    // set the front faces of polygons
    glFrontFace( GL_CCW );
    // set fill mode
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    // set the line width
    glLineWidth( 2.0f );

    // enable lighting
    glEnable( GL_LIGHTING );
    // enable lighting for front
    glLightModeli( GL_FRONT, GL_TRUE );
    // material have diffuse and ambient lighting 
    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    // enable color
    glEnable( GL_COLOR_MATERIAL );


    // enable light 0
    glEnable( GL_LIGHT0 );
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new window size
    g_width = w; g_height = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 1.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}




//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
    case 'Q':
    case 'q':
	exit( 1 );
	break;
    }

    // do a reshape since g_eye_y might have changed
    reshapeFunc( g_width, g_height );
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {
        // rotate
        if( state == GLUT_DOWN )
            g_inc -= INC_VAL;
        else
            g_inc += INC_VAL;
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        if( state == GLUT_DOWN )
            g_inc += INC_VAL;
        else
            g_inc -= INC_VAL;
    }
    else
        g_inc = 0.0f;

    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
    static GLfloat angle = 0.0;
    GLfloat val = .8f;
    GLint buffers = GL_NONE;

    // get the current color buffer being drawn to
    glGetIntegerv( GL_DRAW_BUFFER, &buffers );
    
    glPushMatrix( );
        // rotate the viewpoint
        glRotatef( angle += g_inc, 0.0f, 1.0f, 0.0f );

        // set the clear value
        glClearStencil( 0x0 );
	// clear the stencil buffer
        glClear( GL_STENCIL_BUFFER_BIT );
	// always pass the stencil test
	glStencilFunc(GL_ALWAYS, 0x1, 0x1 );
	// set the operation to modify the stencil buffer
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	// disable drawing to the color buffer
	glDrawBuffer( GL_NONE );
        // enable stencil
        glEnable( GL_STENCIL_TEST );

        // draw the stencil mask
	glBegin( GL_QUADS );
            mirror( val );
	glEnd();

	// reenable drawing to color buffer
	glDrawBuffer( (GLenum) buffers );
	// make stencil buffer read only
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );


	// clear the color and depth buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// draw the mirror image
	glPushMatrix();
	    // invert image about xy plane
	    glScalef( 1.0f, 1.0f, -1.0f );

            // invert the clipping plane based on the view point
            if( cos( angle * M_PI / 180.0 ) < 0.0 ) 
                eqn1[2] = -1.0;
            else
                eqn1[2] = 1.0;

	    // clip one side of the plane
            glClipPlane( GL_CLIP_PLANE0, eqn1 );
	    glEnable( GL_CLIP_PLANE0 );
	    
	    // draw only where the stencil buffer == 1
    	    glStencilFunc( GL_EQUAL, 0x1, 0x1 );
	    // draw the object
	    render();

	    // turn off clipping plane
	    glDisable( GL_CLIP_PLANE0 );
	glPopMatrix( );

	// disable the stencil buffer
	glDisable( GL_STENCIL_TEST );
	// disable drawing to the color buffer
	glDrawBuffer( GL_NONE );

	// draw the mirror pane into depth buffer -
	// to prevent object behind mirror from being render
	glBegin( GL_QUADS );
	    mirror( val );
	glEnd();

	// enable drawing to the color buffer
	glDrawBuffer( (GLenum) buffers );

	// draw the normal image, without stencil test
	glPushMatrix( );
	    render();
	glPopMatrix( );

	// draw the outline of the mirror
	glColor3f( 0.4f, 0.4f, 1.0f );
	glBegin( GL_LINE_LOOP );
	    mirror( val );
	glEnd( );

        // mirror shine
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	glDisable( GL_LIGHTING );
	
	glColor4f( 1.0f, 1.0f, 1.0f, .2f );
        glTranslatef( 0.0f, 0.0f, 0.001f * eqn1[2] );
	glBegin( GL_QUADS );
	    mirror( val );
        glEnd( );

	glDisable( GL_BLEND );
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	glEnable( GL_LIGHTING );

    glPopMatrix( );

    glFlush( );
    glutSwapBuffers( );
}





//-----------------------------------------------------------------------------
// Name: mirror( )
// Desc: draws mirror pane
//-----------------------------------------------------------------------------
void mirror( GLfloat val )
{
    glVertex3f( val, val, 0.0f );
    glVertex3f( -val, val, 0.0f );
    glVertex3f( -val, -val, 0.0f );
    glVertex3f( val, -val, 0.0f );    
}




//-----------------------------------------------------------------------------
// Name: render( )
// Desc: draws scene
//-----------------------------------------------------------------------------
void render( )
{
    glLightfv( GL_LIGHT0, GL_POSITION, g_light0_pos );

    glPushMatrix();
    glColor3f( .4f, 1.0f, .4f );
    glTranslatef( 0.0f, 0.0f, 2.5f );
    glutSolidSphere( 0.5, 12, 12 );

    glTranslatef( 0.5f, 0.0f, -0.7f );
    glColor3f( 1.0f, .4f, .4f );
    glutSolidCube( .3 );

    glTranslatef( -0.5f, 0.0f, -0.2f );
    glRotatef( -90, 1.0f, 0.0f, 0.0f );
    glColor3f( 1.0f, 1.0f, .4f );
    glutSolidCone( .3, .6, 8, 8 );
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 0.2f, 0.3f, -2.0f );
    glColor3f( .9f, .4f, .9f );
    glutWireTorus( .3f, .8f, 8, 8 );
    glPopMatrix();
}
