//-------------------------------------------------------------------
//	File:		YourCode.cpp
//	Comments:	Write your visual effect here.
//-------------------------------------------------------------------
#include "stdafx.h"
//#include "Texture.h"

//----------------------------------------
// Usefull debugger thingy.
void DebugOut(const char *szFormat, ...);
//----------------------------------------

// Change your App Name.
char szAppName[] = "Hufo's little fire";

// Timer info.  Very useful for keeping animated stuff in sync on different computers.
extern float fTime, fDeltaTime;

// Current window dimensions.
extern GLsizei g_w, g_h;
#ifndef NOEXTENSION
extern HWND g_hwnd;
#endif
#include "FirePart.h"

#ifndef NOEXTENSION
bool Splash()
{
	return MessageBox(NULL,
		" \"  \" : rotate/stop camera\n"
		" ENTER : stop fire animation\n"
		" ESC : put water on the fire\n"
		"\n"
		"Enjoy it :-)        Hufo / N.A.A.\n"
		,szAppName
		,MB_OKCANCEL | MB_ICONWARNING) == IDOK;
	return True;
}
#endif
// Use the demonstration texture.
//CTexture gTexExample;
float x0; // window aspect ratio
#define XSTD (4.0/3.0)
float TFire; // fire time
float TVit;

float CylShad[9]={0.2f,0.6f,1.0f,0.6f,0.2f,0.2f,0.2f,0.2f,0.2f};
struct RGBColor { float r,g,b; };
#define SetRGBColor(c) glColor3f((c).r,(c).g,(c).b)
RGBColor CylBColor={0.43f,0.25f,0.10f};
RGBColor CylColor[9];

// Startup Stuff.
bool Init()	// Called right after the window is created, and OpenGL is initialized.
{
	// Reset the matrix to something we know.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float x = (float)g_w/(float)g_h; // Correct the viewing ratio of the window in the X axis.
	if (x>XSTD)
		gluOrtho2D( -x, x, -1, 1 );	// Reset to a 2D screen space.
	else
		gluOrtho2D( -XSTD, XSTD, -XSTD/x, XSTD/x );	// Reset to a 2D screen space.
	x0=x;
	// Reset model view matrix stack.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

//	gTexExample.LoadJPG( "Demo.jpg" );
//	glEnable( GL_TEXTURE_2D );
//	gTexExample.Use(); // Tells OpenGL to use this texture.

	glCullFace(GL_FRONT); // reject fliped faces
	glDepthFunc(GL_LESS);

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
//	glEnable(GL_BLEND);

	for (int i=0;i<9;++i)
	{	
		CylColor[i].r=CylBColor.r*CylShad[i];
		CylColor[i].g=CylBColor.g*CylShad[i];
		CylColor[i].b=CylBColor.b*CylShad[i];
	}

	TFire=0.0;
	TVit=1.0;
	FireInit(); // initialise fire
	// If everything went well, then return true.
	// Returning false will cause the program to quit right away.
	return true;
}

void Shutdown() // Called right after the window is destroyed.
{
}

#ifndef NOEXTENSION
void UserKey(int k)
{
	switch (k)
	{
		case VK_RETURN:
			FireAnim=!FireAnim;
			break;
		case VK_SPACE:
			FireRotate=!FireRotate;
			break;
		case VK_ESCAPE:
//			PostMessage(g_hwnd,WM_CLOSE,0,0);
			FireStop=True;
			break;
	}
}
#endif

#define fSQRT_3_2 0.8660254038f
void AffParticle(float ex,float ey,float dx,float dy,float a)
{
	glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f,1.0f,0.0f,a);
		//glColor4f(a,a,0.0f,1.0f);
		glVertex2f(ex,ey);
		glColor4f(1.0f,0.0f,0.0f,0.0f);
		//glColor4f(a,0.0f,0.0f,0.0f);
		glVertex2f(ex-dx,ey);
		glVertex2f(ex-0.5f*dx,ey+fSQRT_3_2*dy);
		glVertex2f(ex+0.5f*dx,ey+fSQRT_3_2*dy);
		glVertex2f(ex+1.0f*dx,ey);
		glVertex2f(ex+0.5f*dx,ey-fSQRT_3_2*dy);
		glVertex2f(ex-0.5f*dx,ey-fSQRT_3_2*dy);
		glVertex2f(ex-1.0f*dx,ey);
	glEnd();
}

#ifdef NOZBUFFER
#define glVertexAll(v) glVertex2f((v).ex,(v).ey)
#else
#define glVertexAll(v) glVertex3f((v).ex,(v).ey,(v).ez)
#endif
// Draw all the scene related stuff.
void Render()
{
	float x = (float)g_w/(float)g_h; // Aspect Ratio
	if (x!=x0) // Window was resized and need reinit
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (x>XSTD)
			gluOrtho2D( -x, x, -1, 1 );	// Reset to a 2D screen space.
		else
			gluOrtho2D( -XSTD, XSTD, -XSTD/x, XSTD/x );	// Reset to a 2D screen space.
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		x0=x;
	}

#ifndef NOEXTENSION
	static float secTime=fTime;
	static int nf=0;
	if (fTime-secTime>=1.0)
	{ // aff framerate
		char buf[256];
		sprintf(buf,"%s - %g fps",szAppName,nf/(fTime-secTime));
//		sprintf(buf,"%s - %g fps (%d part)",szAppName,nf/(fTime-secTime),np);
		SetWindowText(g_hwnd,buf);
		secTime=fTime;
		nf=0;
	}
#endif

	TFire+=fDeltaTime*TVit;
	CalcFire(TFire,fDeltaTime*TVit); // animate the fire

	glClear( GL_COLOR_BUFFER_BIT
#ifndef NOZBUFFER
	       | GL_DEPTH_BUFFER_BIT
#endif
	       );
	if (FireStop && np==0)
		PostMessage(g_hwnd,WM_CLOSE,0,0);

	int c,i,n;

#ifndef NOZBUFFER
	glEnable(GL_DEPTH_TEST);
#endif
	glEnable(GL_CULL_FACE);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
#ifndef NOZBUFFER
	for (c=0;c<NCYL;++c)
	{
		glBegin(GL_TRIANGLE_FAN);
			SetRGBColor(CylColor[0]);
			glVertexAll(glCyl[c].vTriFan1[0]);
			for (i=1;i<10;++i)
			{
				SetRGBColor(CylColor[i-1]);
				glVertexAll(glCyl[c].vTriFan1[i]);
			}
		glEnd();
	}
#endif
	for (n=0;n<NCYLPART;++n)
	{
		for (c=0;c<NCYL;++c)
		{
			glBegin(GL_QUAD_STRIP);
				for (i=0;i<9;++i)
				{
					SetRGBColor(CylColor[i]);
					glVertexAll(glCyl[c].vCyl[n  ][i]);
					glVertexAll(glCyl[c].vCyl[n+1][i]);
				}
			glEnd();
		}
	}
#ifdef NOZBUFFER
	for (c=0;c<NCYL;++c)
	{
		glBegin(GL_TRIANGLE_FAN);
			SetRGBColor(CylColor[0]);
			glVertexAll(glCyl[c].vTriFan2[0]);
			for (i=9;i>0;--i)
			{
				SetRGBColor(CylColor[i-1]);
				glVertexAll(glCyl[c].vTriFan2[i]);
			}
		glEnd();
	}
#endif
	glDisable(GL_CULL_FACE);
#ifndef NOZBUFFER
	glDisable(GL_DEPTH_TEST);
#endif

	glEnable(GL_BLEND);
	Particle *p=TblP;
	n=np;
	while (n)
	{
		AffParticle(p->ex,p->ey,p->dx,p->dy,p->a);
		++p;
		--n;
	}
	glDisable(GL_BLEND);
#ifndef NOEXTENSION
	++nf;
#endif
}
