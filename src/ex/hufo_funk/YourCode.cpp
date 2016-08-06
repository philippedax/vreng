//-------------------------------------------------------------------
//	File:		YourCode.cpp
//	Comments:	Write your visual effect here.
//-------------------------------------------------------------------
#include "stdafx.h"
//#include "Texture.h"
#include "CObject.h"
#include "CHair.h"

//int NBOBJ=4;
#define NBOBJMAX 10
#define USELIST 1
//#define USESTRIP
//#define OBJSIZE 2000
#define OBJSIZE 150

#define DATAREP 

//----------------------------------------
// Usefull debugger thingy.
void DebugOut(const char *szFormat, ...);
//----------------------------------------

// Change your App Name.
char szAppName[] = "Hufo's Funky Lines";

// Timer info.  Very useful for keeping animated stuff in sync on different computers.
extern float fTime, fDeltaTime;

// Current window dimensions.
extern GLsizei g_w, g_h;
extern HWND g_hwnd;
extern bool bFullScreen;
// Use the demonstration texture.
//CTexture gTexExample;

CObject obj;

CHair hair;

CPoint3D pos;
CPoint3D pos0;
CPoint3D pos1;

bool preInit()
{
	//Test(obj.LoadLWO(DATAREP "fig22.lwo"));
	//Test(obj.LoadLWO("h:\data\fig22.lwo"));
	//Test(obj.LoadLWO(DATAREP "tete1.lwo"));
	Test(obj.LoadLWO(DATAREP "tete2.lwo"));
	//obj.Flip();
	CSurface *s_hair=obj.FindSurface("hair");
	if (s_hair==NULL)
		return r_err;
	hair.Init(&obj,s_hair);

	CSurface *s_skin=obj.FindSurface("skin");
	if (s_skin!=NULL)
	{
		*(CMaterial*)s_hair=*(CMaterial*)s_skin;
	}

//#ifdef USESTRIP
//	Test(obj.GetStrip(DATAREP "fig22_Default.objf"));
//#endif
/*
	switch(MessageBox(NULL,
		" DO YOU WANT TO GO FULLSCREEN (same res/colors as desktop)\n"
		,szAppName
		,MB_YESNOCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2
		))
	{
		case IDYES:
			bFullScreen=true;
			break;
		case IDNO:
			bFullScreen=false;
			break;
		case IDCANCEL:
			return r_err;
	}
*/
	bFullScreen=false;
	return r_ok;
}
/*
float ObjTr[NBOBJMAX][3];

void SetNbObj(int n)
{
	NBOBJ=min(NBOBJMAX,n);
	if (NBOBJ==1)
	{
		ObjTr[0][0]=0.0f;
		ObjTr[0][1]=0.0f;
		ObjTr[0][2]=0.0f;
	}
	else
	for (int i=0;i<NBOBJ;++i)
	{
		ObjTr[i][0]=(float)((OBJSIZE/2)*cos(i*2*PI/NBOBJ));
		ObjTr[i][1]=(float)((OBJSIZE/2)*sin(i*2*PI/NBOBJ));
		ObjTr[i][2]=0.0f;
	}
}
*/
#ifdef USELIST
int ObjList;
bool ObjChanged;
#endif
#ifdef USESTRIP
bool ObjStrip;
#endif
int ClearBuf;
// Startup Stuff.
bool Init()	// Called right after the window is created, and OpenGL is initialized.
{
	//SetNbObj(NBOBJ);
	// Reset the matrix to something we know.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float x = (float)g_w/(float)g_h; // Correct the viewing ratio of the window in the X axis.
	//gluOrtho2D( -x, x, -1, 1 );	// Reset to a 2D screen space.

	// Reset model view matrix stack.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

//	gTexExample.LoadJPG( "Demo.jpg" );
//	glEnable( GL_TEXTURE_2D );
//	gTexExample.Use(); // Tells OpenGL to use this texture.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective( 110, x, OBJSIZE/20.0f, 2*OBJSIZE); //mfFarZ );
//	gluLookAt(	vPos.x, vPos.y, vPos.z,
//				vDir.x, vDir.y, vDir.z,
//				vUp.x, vUp.y, vUp.z);

	// Reset model view matrix stack.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//glCullFace(GL_FRONT); // reject fliped faces
	glCullFace(GL_BACK); // reject fliped faces

	glDepthFunc(GL_LESS);
	if (obj.sf[0].flag&CMaterial::Blend)
	{
		glDisable(GL_DEPTH_TEST);
		ClearBuf=GL_COLOR_BUFFER_BIT;
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
		ClearBuf=GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	}

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	//Test(obj.LoadLWO("h:\\data\\fig22.lwo"));
#ifdef USELIST
	ObjChanged=true; // rebuild list
	ObjList=glGenLists(1);
	if (!ObjList) return HErrorN("glList creation error");
#endif
#ifdef USESTRIP
	ObjStrip=true;
#endif
	// If everything went well, then return true.
	// Returning false will cause the program to quit right away.
	return r_ok;
}

void Shutdown() // Called right after the window is destroyed.
{
#ifdef USELIST
	if (ObjList)
	{
		glDeleteLists(ObjList,1);
		ObjList=0;
	}
#endif
	obj.Free();
}

bool AffObj=false;
bool Rot=false;

float Angle=150.0f;

void UserKey(int k)
{
	switch (k)
	{
		case VK_RETURN:
			AffObj=!AffObj;
			break;
		case VK_SPACE:
			Rot=!Rot;
			break;
		case VK_ESCAPE:
			PostMessage(g_hwnd,WM_CLOSE,0,0);
			break;
	}
}

float t0=0;
float t1=0.5f;

#define RANDOMX (OBJSIZE*0.02f)
#define RANDOMY (OBJSIZE*0.02f)
#define RANDOMZ (OBJSIZE*0.02f)
#define MAXPOS (OBJSIZE/8)

// Draw all the scene related stuff.
void Render()
{
	static float secTime=fTime;
	static int nf=0;
	if (fTime-secTime>=1.0)
	{ // aff framerate
		char buf[256];
		sprintf(buf,"%s - %g fps",szAppName,nf/(fTime-secTime));
		SetWindowText(g_hwnd,buf);
		secTime=fTime;
		nf=0;
	}

	static float lastTime=fTime;

	float dt=fTime-lastTime;
	lastTime=fTime;

	if (fTime>=t1)
	{
		t1=fTime+(t1-t0);
		t0=fTime;
		pos0=pos1;
		pos1.x+=rnd(2*RANDOMX)-RANDOMX;
    if (pos1.x>MAXPOS) pos1.x=MAXPOS;
    else if (pos1.x<-MAXPOS) pos1.x=-MAXPOS;
		pos1.y+=rnd(2*RANDOMY)-RANDOMY;
    if (pos1.y>MAXPOS) pos1.y=MAXPOS;
    else if (pos1.y<-MAXPOS) pos1.y=-MAXPOS;
		pos1.z+=rnd(2*RANDOMZ)-RANDOMZ;
    if (pos1.z>MAXPOS) pos1.z=MAXPOS;
    else if (pos1.z<-MAXPOS) pos1.z=-MAXPOS;
	}
	CPoint3D p2=pos0+(pos1-pos0)*((fTime-t0)/(t1-t0));

	hair.Update(dt,pos-p2);
	pos=p2;

	++nf;

	if (Rot) Angle+=dt*20;

	glClear( ClearBuf );
	glPushMatrix();
		float light0[4]={0.0f,0.0f,1.0f,0.0f};
		glEnable(GL_NORMALIZE);
		//glTranslatef(0.0f,0.0f,-OBJSIZE/2);
		glTranslatef(0.0f,0.0f,-OBJSIZE/8);
		// Calculate light position
		glPushMatrix();
			//glRotatef( 180+fTime*180, 0.0f, 1.0f, 0.0f );
			//glRotatef( fTime*100, 1.0f, 0.0f, 0.0f );
			//glRotatef( fTime* 70, 0.0f, 0.0f, 1.0f );
			glLightfv(GL_LIGHT0,GL_POSITION,light0);
			glEnable(GL_LIGHT0);
		glPopMatrix();
		// Calculate objects position
		//glRotatef( fTime*30, 0.0f, 1.0f, 0.0f );
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f );
		//glRotatef(180.0f, 0.0f, 0.0f, 1.0f );
		//for (int i=0;i<NBOBJ;++i)
		{
			glPushMatrix();
			//glTranslatef(ObjTr[i][0],ObjTr[i][1],ObjTr[i][2]);
			//glRotatef(-fTime*80, 0.0f, 1.0f, 0.0f );
			//glRotatef(-fTime*20, 0.0f,0.0f, 1.0f);
			glRotatef(Angle, 0.0f,0.0f, 1.0f);
		glTranslatef(pos.x,pos.y,pos.z);
			if (AffObj)
			{
	#ifdef USELIST
				if (ObjChanged) // rebuild list
				{
					glNewList(ObjList,GL_COMPILE_AND_EXECUTE);
	#endif
	#ifdef USESTRIP
					if (ObjStrip)
						obj.AffStrip();
					else
	#endif
					obj.Aff();
	#ifdef USELIST
					glEndList();
					ObjChanged=false;
				}
				else
					glCallList(ObjList);
	#endif
			}
			hair.Aff();
			glPopMatrix();
		}
	glPopMatrix();
}
