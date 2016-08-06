
// double to float conv
#pragma warning(disable:4244)

#ifdef _DEBUG
#include <assert.h>
#endif

#include "FMotion.h"

#define HVCtrX 0.0
#define HVCtrY 0.0
Float FireFoc=2.0f;
#define FireFocX FireFoc
#define FireFocY FireFoc

#define ProjEX(p) (HVCtrX+FireFocX*(p).x/(p).y)
#define ProjEY(p) (HVCtrY+FireFocY*(p).z/(p).y)
#ifdef NOZBUFFER
#define ProjEZ(ez,p)
#else
#define ProjEZ(ez,p) (ez)=-((p).y*(float)(1.0/20.0)-1.0)
#endif

inline int hrnd (int n)
{	return (n*rand() )/RAND_MAX;       }
inline double hrnd (double n)
{	return (n*rand() )/RAND_MAX;       }
inline double abs(double n)
{ return (n<0)?-n:n; }
inline double abs(float n)
{ return (n<0)?-n:n; }

double nrnd(double d) // normal distribution randomizer
{ double r,v1,v2;
	do {
		v1=2.0*hrnd(1.0)-1.0;
		v2=2.0*hrnd(1.0)-1.0;
		r=v1*v1+v2*v2;
	} while (r >= 1.0 || r == 0.0);
	r=sqrt(-2.0*log(r)/r);
	//gset=v1*fac;
	//iset=v2*fac;
	return v1*d*r;
}


#define NBPARTMAX 2048
#define PARTLIFE 2.0
#define PARTINTERV (PARTLIFE/NBPARTMAX)
#define FIRESIZE 1.0f
#define FIREDS 0.7
#define FIREALPHA 0.15f
#define FIREDA 0.4
#define FIRECYLR 0.2f
struct Particle
{
	SVector3D p; // position
	SVector3D v; // dp/dt
	SVector3D s; // size
	float a; // alpha
	float t; // time to death
	float ex,ey,ez; // screen pos
	float dx,dy; // screen size
} TblP[NBPARTMAX];
int np;
float LastPartTime;
Bool FireAnim,FireRotate,FireRecalc;
Bool FireStop;
float FireRot;
float FireAng;
struct Vertex
{
	float u,v;
	float ex,ey,ez;
};

SVector3D FireSrc(0.0f,25.0f,-13.0f);
SVector3D FireDS1(6.0f,0.0f,0.0f);
SVector3D FireDS2(0.0f,2.0f,0.0f);
SVector3D FireDir(0.0f,0.0f,10.0f);
SVector3D FPartA(0.0f,0.0f,4.0f);

SMatrix3D FireM;
SVector3D FireO;

void FireInit()
{
	//NoiseInit();
	np=0;
	LastPartTime=0.0;
	FireAnim=True; FireRotate=False; FireRot=PI/5.0; FireAng=0.0;
	FireStop=False; FireRecalc=True;
	FMotionInit();
/*	np=1;
	TblP[0].p=FireSrc;
	TblP[0].v.Zero();
	TblP[0].a=1.0;;
	TblP[0].s.x=2.0;
	TblP[0].s.y=2.0;
	TblP[0].s.z=2.0;
	TblP[0].t=-1;*/
}
void CalcFire(float t,float dt)
{
	int n;
	Particle *p=TblP; //+1;
	n=0;
	float ds=pow(FIREDS,dt);
	float da=pow(FIREDA,dt);
	//float f;
	SVector3D a;
	if (FireAnim)
	{
		FMotionAnimate(dt);
		while (n<np)
		{
			if ((p->t-=dt)<=0)
			{ // kill it
				*p=TblP[--np];
			}
			else
			{ // animate it
				//a=p->p; a.z-=5*t;
				//a=FPartA; //+Noise(a)*(p->p.z-FireSrc.z)*1.0; //a.x*=4.0f; a.y*=4.0f;
				p->v+=FPartA*dt;
				//GetFMotion(p->p,a); p->v=a+FPartA;
				p->p+=p->v*dt;
				//p->p+=FireDir*dt;
				//p->p.z+=dt;
				FMotionWarp(p->p,dt);
				//p->s*=ds;
				//p->s.x=0.67*FIRESIZE+0.4*FIRESIZE*sin(PI*(p->t)/PARTLIFE);
				//p->s.z=0.05*FIRESIZE+0.768*FIRESIZE*(1.0-p->t/PARTLIFE);
				p->a*=da;
				++n;
				++p;
			}
		}
		if (!FireStop)
		while (np<NBPARTMAX && t-LastPartTime>=PARTINTERV)
		{
			LastPartTime+=(float)PARTINTERV;
			p=TblP+(np++);
			p->p=FireSrc+FireDS1*nrnd(0.25)+FireDS2*nrnd(0.25);
			p->v=FireDir;
			FMotionWarp(p->p,(t-LastPartTime));
			p->t=PARTLIFE+LastPartTime-t;
			float size=FIRESIZE*(0.5f+nrnd(0.5));
			float alpha=FIREALPHA*(float)pow(size/FIRESIZE,0.5f);
			p->s.x=size; p->s.y=size; p->s.z=size;
			p->a=alpha*pow(FIREDA,(t-LastPartTime));
			p->s*=0.5f+nrnd(0.5);
		}
	}
	else LastPartTime+=dt;
	n=0; p=TblP;
	SVector3D v;
	if (FireRotate) { FireAng+=FireRot*dt; FireRecalc=True; }
	FireM.FromAngle(0,0,FireAng);
	FireO.Zero();
	FireM.Apply(FireSrc,&FireO);
	FireO=FireSrc-FireO;
	while (n<np)
	{
		FireM.Apply(p->p,&v);
		v+=FireO;
		if (v.y>1.0f)
		{
			p->ex=ProjEX(v); p->ey=ProjEY(v); ProjEZ(p->ez,v);
			p->dx=FireFocX*p->s.x/v.y;
			p->dy=FireFocY*p->s.z/v.y;
		}
		else p->dx=0;
		++n;
		++p;
	}
}
