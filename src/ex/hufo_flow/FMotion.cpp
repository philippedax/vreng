//-------------------------------------------------------------------
//	File:		FMotion.cpp
//    Note: Credits for the 2d warping field fly to
//          Hugo Elias: http://freespace.virgin.net/hugo.elias
//          I've transposed to 3d.
//-------------------------------------------------------------------
#include "stdafx.h"
#include "FMotion.h"

#define TILE 0.5f
//#define NBTILE 8
#define NBTILEZ 8
#define NBTILEY 4
#define NBTILEX 8

#define WARPFACT (TILE*5.0f)
#define AFFFACT ((TILE/16.0f)/WARPFACT)
//#define WARPSCL  1
//#define WARPSTEP 2
#define SPRINGFACT 0.01f
//#define FIRESPEED 1
#define DYNFACT 0.998f
#define MAXLENGTH (WARPFACT*4)

#define ANGFACT 1
//#define ANGRND 0.03f
//#define ANG0 0.08f
#define ANGRND 0.16f
#define ANG0 0.16f

#define SHFT 8

//#define Float2Int(f) ((int)(f))

//#define FLOATTOINTCONST (((65536.0*65536.0*16)+(65536.0*0.5))*65536.0)
//inline int f2int(double f)
//{ f+=FLOATTOINTCONST; return ((*((int*)&f)))-0x80000000; }

#define FLOATTOINTCONST2 (((65536.0*16)))
inline int f2int2(float f)
{ f+=FLOATTOINTCONST2; return ((*((int*)&f))&0x007fffff)-0x00400000; }
#define FLOATTOINTCONST (((1.5*65536*256)))
inline int f2int(float f)
{ f+=FLOATTOINTCONST; return ((*((int*)&f))&0x007fffff)-0x00400000; }
#define Float2Int(f) (f2int(f))
#define Float2Int2(f) (f2int2(f))

typedef float real;
/*real norms[3][3] =
{{0     ,TILE            ,2*TILE            ,
    TILE,TILE*1.414213562,  TILE*2.236067977,
  2*TILE,TILE*2.236067977,2*TILE*1.414213562}
,{TILE  ,TILE            ,2*TILE            ,
    TILE,TILE*1.414213562,  TILE*2.236067977,
  2*TILE,TILE*2.236067977,2*TILE*1.414213562}
};*/

#define M_PI ((real)PI)
#define SQRT_2 1.414213562f
#define SQRT_3 1.732050808f
#define SQRT_5 2.236067977f
#define SQRT_6 2.449489743f

#define D0     0
#define D1     1
#define D2     2
#define D1_1   SQRT_2
#define D2_1   SQRT_5
#define D2_2   2*SQRT_2
#define D1_1_1 SQRT_3
#define D2_1_1 SQRT_6
#define D2_2_1 3
#define D2_2_2 3*SQRT_2

real norms[3][3][3] =
{{TILE*D0    ,TILE*D1    ,TILE*D2    ,
  TILE*D1    ,TILE*D1_1  ,TILE*D2_1  ,
	TILE*D2    ,TILE*D2_1  ,TILE*D2_2  }
,{TILE*D1    ,TILE*D1_1  ,TILE*D2_1  ,
  TILE*D1_1  ,TILE*D1_1_1,TILE*D2_1_1,
	TILE*D2_1  ,TILE*D2_1_1,TILE*D2_2_1}
,{TILE*D2    ,TILE*D2_1  ,TILE*D2_2  ,
  TILE*D2_1  ,TILE*D2_1_1,TILE*D2_2_1,
	TILE*D2_2  ,TILE*D2_2_1,TILE*D2_2_2}
};

real distvar[3]={-TILE,0,TILE};

struct PARTICLE
{
	//real x,y,z,xv,yv,zv,c;
	SVector3D p,v;
} offset[NBTILEZ][NBTILEY][NBTILEZ]; //offset[NBTILEY+1][NBTILEX+1];

real deplfact[1<<SHFT];

bool FMotionInit()
{
	int i;
	for (i=0;i<(1<<SHFT);++i)
		deplfact[i]=i/((real)(1<<SHFT));
	int x,y,z;
 	PARTICLE *p;
	//initialise gelly
	real t, t1, t2, t3, tt, tt1, tt2, tt3, ttt, ttt1, ttt2, ttt3, xf, yf, zf;
	real f, f1, f2, f3, ff, ff1, ff2, ff3, fff, fff1, fff2, fff3;
	ttt =rnd2(2*M_PI); f =rnd2(ANGRND)+ANG0; ff =rnd2(ANGRND)+ANG0; fff =rnd2(ANGRND)+ANG0;
	ttt1=rnd2(2*M_PI); f1=rnd2(ANGRND)+ANG0; ff1=rnd2(ANGRND)+ANG0; fff1=rnd2(ANGRND)+ANG0;
	ttt2=rnd2(2*M_PI); f2=rnd2(ANGRND)+ANG0; ff2=rnd2(ANGRND)+ANG0; fff2=rnd2(ANGRND)+ANG0;
	ttt3=rnd2(2*M_PI); f3=rnd2(ANGRND)+ANG0; ff3=rnd2(ANGRND)+ANG0; fff3=rnd2(ANGRND)+ANG0;
	p=&offset[0][0][0];
	for (z=0; z</*=*/NBTILEZ; z++)
	{ zf = real(z);
		tt=ttt; tt1=ttt1; tt2=ttt3; tt3=ttt3;
		ttt +=fff; ttt1+=fff1; ttt2+=fff2; ttt3+=fff3;
		for (y=0; y</*=*/NBTILEY; y++)
		{ yf = real(y);
			t=tt; t1=tt1; t2=tt3; t3=tt3;
			tt +=ff; tt1+=ff1; tt2+=ff2; tt3+=ff3;
			for (x=0; x</*=*/NBTILEX; x++,p++)
			{ xf = real(x);
				t +=f; t1+=f1; t2+=f2; t3+=f3;
				p->p.x  = (real)(WARPFACT*(sin(ANGFACT*(t2-xf))+cos(ANGFACT*(t2+yf))+sin(ANGFACT*(t -yf))-cos(ANGFACT*(t +xf))));//+x*TILE);
				p->p.y  = (real)(WARPFACT*(sin(ANGFACT*(t1-xf))+cos(ANGFACT*(t3+yf))+sin(ANGFACT*(t3-yf))-cos(ANGFACT*(t1+xf))));//+y*TILE);
				p->p.z  = (real)(WARPFACT*(sin(ANGFACT*(t -xf))+cos(ANGFACT*(t2+yf))+sin(ANGFACT*(t -yf))-cos(ANGFACT*(t2+xf))));//+z*TILE);
				//p->p.Rand();
				//p->p.Scale(WARPFACT/p->p.Length());
				p->v.Zero();
				//p->xv = 0; p->yv = 0; p->zv = 0;
				//p->c  = abs(rand()) >> 7;
			}
		}
	}
	return r_ok;
}

void FMotionQuit()
{
}

void FMotionAnimate(float dt)
{
// Handles the wobbling gel, which is used to give the WarpMap it's shape
	int x,y,z,xi,yi,zi,xh,xl,yh,yl,zh,zl;
	//real xv,yv,zv,Xspring,Yspring,Zspring,ext,length,norm;
	SVector3D t,spring; real norm;
	PARTICLE *p,*p2;
	real ft=SPRINGFACT*dt;
	p=&offset[0][0][0];
for (z=0; z<NBTILEZ; z++)
{ zh=z+1; zl=z-1; //if (zl<0)	zl=0; if (zh>tz)	zh=tz;
	for (y=0; y<NBTILEY; y++)
	{ yh=y+1; yl=y-1; //if (yl<0)	yl=0; if (yh>ty)	yh=ty;
		for (x=0; x<NBTILEX; x++,p++)
		{ xh=x+1; xl=x-1; //if (xl<0)	xl=0; if (xh>tx)	xh=tx;
			for (zi=zl; zi<=zh; zi++)
			{ for (yi=yl; yi<=yh; yi++)
			{ for (xi=xl; xi<=xh; xi++)
				{ if ((xi != x) || (yi != y) || (zi!=z))
					{ norm = norms[abs(zi-z)][abs(yi-y)][abs(xi-x)];
						p2=&offset[zi&(NBTILEZ-1)][yi&(NBTILEY-1)][xi&(NBTILEX-1)];
						/*
						Xspring = p2->x - p->x+distvar[xi-x+1];
						Yspring = p2->y - p->y+distvar[yi-y+1];
						Zspring = p2->z - p->z+distvar[zi-z+1];
						length = (real)sqrt(Xspring*Xspring + Yspring*Yspring + Zspring*Zspring);
						Xspring *= (norm-length) * sf;
						Yspring *= (norm-length) * sf;
						Zspring *= (norm-length) * sf;
						p2->xv += Xspring;
						p2->yv += Yspring;
						p2->zv += Zspring;
						*/
						spring=p2->p; spring-=p->p;
						spring.x+=distvar[xi-x+1];
						spring.y+=distvar[yi-y+1];
						spring.z+=distvar[zi-z+1];
						spring*=(norm-spring.Length())*ft;
						p2->v+=spring;
			} } }
			}
		}
	}
}
	p=&offset[0][0][0];
	ft=(real)pow(DYNFACT,dt);
for (z=0; z<NBTILEZ; z++)
{	for (y=0; y<NBTILEY; y++)
	{ for (x=0; x<NBTILEX; x++, p++)
		{ //p->x += p->xv; p->y += p->yv; p->z += p->zv;
			//p->xv*=DYNFACT; p->yv *= DYNFACT; p->zv *= DYNFACT;
			p->p+=p->v*dt;
			if (p->p.Length2()>(MAXLENGTH*MAXLENGTH))
			{
				p->v.Zero();
			}
			else
				p->v*=ft;
	} }
}
}

#define Point2STX(p) ( (Float2Int((p.x)*(TILE*(1<<SHFT)))+((NBTILEX/2)<<SHFT)) & ((NBTILEX<<SHFT)-1) )
#define Point2STY(p) ( (Float2Int((p.y)*(TILE*(1<<SHFT)))+((NBTILEY/2)<<SHFT)) & ((NBTILEY<<SHFT)-1) )
#define Point2STZ(p) ( (Float2Int((p.z)*(TILE*(1<<SHFT)))) & ((NBTILEZ<<SHFT)-1) )

void FMotionWarp(SVector3D &p,float dt)
{
	int sx,sy,sz,x2,y2,z2;
	sx=Point2STX(p);
	sy=Point2STY(p);
	sz=Point2STZ(p);

	real fx,fy,fz;
	fx=deplfact[sx&((1<<SHFT)-1)];
	fy=deplfact[sy&((1<<SHFT)-1)];
	fz=deplfact[sz&((1<<SHFT)-1)];
	sx>>=SHFT; x2=(sx+1)&(NBTILEX-1);
	sy>>=SHFT; y2=(sy+1)&(NBTILEY-1);
	sz>>=SHFT; z2=(sz+1)&(NBTILEZ-1);
	/*static SVector3D v0,v1,v2,v3,v4,v5,v;
	v0.Lerp(offset[sz][sy][sx].p,offset[sz][sy][sx+1].p,fx);
	v1.Lerp(offset[sz][y2][sx].p,offset[sz][y2][sx+1].p,fx);
	v2.Lerp(offset[z2][sy][sx].p,offset[z2][sy][sx+1].p,fx);
	v3.Lerp(offset[z2][y2][sx].p,offset[z2][y2][sx+1].p,fx);
	v4.Lerp(v0,v1,fy); v5.Lerp(v2,v3,fy);
	v.Lerp(v4,v5,fz);*/

	static SVector3D v0,v1,v2,v3,v4,v5,v;
	v0.Lerp(offset[sz][sy][sx].p,offset[sz][sy][sx+1].p,fx);
	v1.Lerp(offset[z2][sy][sx].p,offset[z2][sy][sx+1].p,fx);
	v.Lerp(v0,v1,fz);

	//SVector3D v=offset[sz>>SHFT][sy>>SHFT][sx>>SHFT].p;

	v*=dt;
	p+=v;
	//p.x+=t->x*dt;
	//p.y+=t->y*dt;
	//p.z+=t->z*dt;
}


#define Point2STX2(p) ( (Float2Int2((p.x)*(TILE*(1<<SHFT)))+((NBTILEX/2)<<SHFT)) & ((NBTILEX<<SHFT)-1) )
#define Point2STY2(p) ( (Float2Int2((p.y)*(TILE*(1<<SHFT)))+((NBTILEY/2)<<SHFT)) & ((NBTILEY<<SHFT)-1) )
#define Point2STZ2(p) ( (Float2Int2((p.z)*(TILE*(1<<SHFT)))) & ((NBTILEZ<<SHFT)-1) )

void FMotionWarp2(SVector3D &p,float dt)
{
	int sx,sy,sz,x2,y2,z2;
	sx=Point2STX2(p);
	sy=Point2STY2(p);
	sz=Point2STZ2(p);

	real fx,fy,fz;
	fx=deplfact[sx&((1<<SHFT)-1)];
	fy=deplfact[sy&((1<<SHFT)-1)];
	fz=deplfact[sz&((1<<SHFT)-1)];
	sx>>=SHFT; x2=(sx+1)&(NBTILEX-1);
	sy>>=SHFT; y2=(sy+1)&(NBTILEY-1);
	sz>>=SHFT; z2=(sz+1)&(NBTILEZ-1);
	/*static SVector3D v0,v1,v2,v3,v4,v5,v;
	v0.Lerp(offset[sz][sy][sx].p,offset[sz][sy][sx+1].p,fx);
	v1.Lerp(offset[sz][y2][sx].p,offset[sz][y2][sx+1].p,fx);
	v2.Lerp(offset[z2][sy][sx].p,offset[z2][sy][sx+1].p,fx);
	v3.Lerp(offset[z2][y2][sx].p,offset[z2][y2][sx+1].p,fx);
	v4.Lerp(v0,v1,fy); v5.Lerp(v2,v3,fy);
	v.Lerp(v4,v5,fz);*/

	static SVector3D v0,v1,v2,v3,v4,v5,v;
	v0.Lerp(offset[sz][sy][sx].p,offset[sz][sy][sx+1].p,fx);
	v1.Lerp(offset[z2][sy][sx].p,offset[z2][sy][sx+1].p,fx);
	v.Lerp(v0,v1,fz);

	//SVector3D v=offset[sz>>SHFT][sy>>SHFT][sx>>SHFT].p;

	v*=dt;
	p+=v;
	//p.x+=t->x*dt;
	//p.y+=t->y*dt;
	//p.z+=t->z*dt;
}

extern Float FireFoc;
extern SMatrix3D FireM;
extern SVector3D FireO;
extern SVector3D FireSrc;

#define FireFocX FireFoc
#define FireFocY FireFoc
#define HVCtrX 0.0f
#define HVCtrY 0.0f
#define ProjEX(p) (HVCtrX+FireFocX*(p).x/(p).y)
#define ProjEY(p) (HVCtrY+FireFocY*(p).z/(p).y)
#ifdef NOZBUFFER
#define ProjEZ(ez,p)
#else
#define ProjEZ(ez,p) (ez)=-((p).y*(1.0f/20.0f)-1.0f)
#endif

void AffFMotion()
{
	int x,y,z;
 	PARTICLE *p;
	real xf, yf, zf;
	real ex,ey,ez;
	SVector3D p1,p2,o;
	p=&offset[0][0][0];
	glBegin(GL_LINES);
	o=FireSrc; o.x-=TILE*NBTILEX/2; o.y-=TILE*NBTILEY/2;
	//o.Zero();
	FireM.Apply(&o);
	o+=FireO;
	for (z=0; z<NBTILEZ; z++)
	{ zf = z*TILE;
		for (y=0; y<NBTILEY; y++)
		{ yf = y*TILE;
			for (x=0; x<NBTILEX; x++,p++)
			{ xf = x*TILE;
				p1.x=xf; p1.y=yf; p1.z=zf;
				//p2.x=xf+p->x*AFFFACT; p2.y=yf+p->y*AFFFACT; p2.z=zf+p->z*AFFFACT;
				p2=p1+p->p*AFFFACT;
				FireM.Apply(&p1); p1+=o;
				FireM.Apply(&p2); p2+=o;
				if (p1.y>0 && p2.y>0)
				{
					ex=ProjEX(p1);
					ey=ProjEY(p1);
					ProjEZ(ez,p1);
#ifdef NOZBUFFER
					glVertex2f(ex,ey);
#else
					glVertex3f(ex,ey,ez);
#endif
					ex=ProjEX(p2);
					ey=ProjEY(p2);
					ProjEZ(ez,p2);
#ifdef NOZBUFFER
					glVertex2f(ex,ey);
#else
					glVertex3f(ex,ey,ez);
#endif
				}
				//p->c  = abs(rand()) >> 7;
			}
		}
	}
	glEnd();
}
