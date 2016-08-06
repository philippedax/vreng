
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

/*#define NBNOISEVAL 1024
#define NOISEDD 0.9;
#define NOISESCALE 2.0
float NoiseX[NBNOISEVAL+1];
float NoiseY[NBNOISEVAL+1];
float NoiseZ[NBNOISEVAL+1];
void NoiseCalc(float *t,int n,float d)
{
	t[n>>1]=(t[0]+t[n])*0.5+hrnd(2.0f*d)-d;
	if (n>1)
	{
		d*=(float)NOISEDD;
		NoiseCalc(t,n>>1,d);
		NoiseCalc(t+(n>>1),(n+1)>>1,d);
	}
}
//#include <time.h>
void NoiseInit()
{
	//srand(time(NULL));
	NoiseX[0]=NoiseX[NBNOISEVAL]=0.0;
	NoiseCalc(NoiseX,NBNOISEVAL,1.0);
	NoiseY[0]=NoiseY[NBNOISEVAL]=0.0;
	NoiseCalc(NoiseY,NBNOISEVAL,1.0);
	NoiseZ[0]=NoiseZ[NBNOISEVAL]=0.0;
	NoiseCalc(NoiseZ,NBNOISEVAL,1.0);
}

void Noise(SVector3D const &p,SVector3D *n)
{
	double d;
	float f; int i;
	f=modf(p.x*NOISESCALE,&d); i=((int)d)&(NBNOISEVAL-1);
	n->x=(1.0f-f)*NoiseX[i]+f*NoiseX[i+1];
	f=modf(p.y*NOISESCALE,&d); i=((int)d)&(NBNOISEVAL-1);
	n->y=(1.0f-f)*NoiseX[i]+f*NoiseY[i+1];
	f=modf(p.z*NOISESCALE,&d); i=((int)d)&(NBNOISEVAL-1);
	n->z=(1.0f-f)*NoiseX[i]+f*NoiseZ[i+1];
}
SVector3D Noise(SVector3D const &p)
{
	double d;
	float f; int i;
	SVector3D n;
	f=modf(p.x*NOISESCALE,&d); i=((int)d+123)&(NBNOISEVAL-1);
	n.x=(1.0f-f)*NoiseX[i]+f*NoiseX[i+1];
	f=modf(p.y*NOISESCALE,&d); i=((int)d+123)&(NBNOISEVAL-1);
	n.y=(1.0f-f)*NoiseX[i]+f*NoiseY[i+1];
	f=modf(p.z*NOISESCALE,&d); i=((int)d+123)&(NBNOISEVAL-1);
	n.z=(1.0f-f)*NoiseX[i]+f*NoiseZ[i+1];
	return n;
}*/


#define NBPARTMAX 2048
#define PARTLIFE 2.0
#define PARTINTERV (PARTLIFE/NBPARTMAX)
//0.1
#define FIRESIZE 0.3f
#define FIREDS 0.7
#define FIREALPHA 0.35f
#define FIREDA 0.2
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

SVector3D FireSrc(0.0f,5.0f,-2.0f);
SVector3D FireDS1(2.0f,0.0f,0.0f);
SVector3D FireDS2(0.0f,2.0f,0.0f);
SVector3D FireDS3(0.0f,0.0f,2.0f);
//SVector3D FireDir(0.0f,0.0f,2.5f);
SVector3D FireDir(0.0f,2.0f,5.0f);
SVector3D FPartA(0.0f,0.0f,1.0f);

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
	int i,n;
	Particle *p=TblP; //+1;
	n=0;
	float ds=pow(FIREDS,dt);
	float da=pow(FIREDA,dt);
	float f;
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
				//p->v+=FPartA*dt;
				//GetFMotion(p->p,a); p->v=a+FPartA;
				//p->p+=p->v*dt;
				p->p+=FireDir*dt;
				//p->p.z+=dt;
				if (BugMode)
					FMotionWarp2(p->p,dt);
				else
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
			p->p=FireSrc+FireDS1*nrnd(0.25)+FireDS2*nrnd(0.25)+FireDS3*nrnd(0.25);
			//do { f=nrnd(0.2); } while (abs(f)>0.45);
			//p->p=FireSrc+FCyl[hrnd(NCYL)].d*f
			if (BugMode)
				FMotionWarp2(p->p,(t-LastPartTime));
			else
				FMotionWarp(p->p,(t-LastPartTime));
			//p->p+=(FireDir+FPartA*((t-LastPartTime)*0.5))*(t-LastPartTime);
			//p->p=FireSrc; p->p.x+=nrnd(0.1); p->p.y+=nrnd(0.1); p->p.z= nrnd(0.1);
			//p->v=FireDir;+FPartA*(t-LastPartTime);
			p->t=PARTLIFE+LastPartTime-t;
			p->a=FIREALPHA*pow(FIREDA,(t-LastPartTime));
			p->s.x=FIRESIZE; p->s.y=FIRESIZE; p->s.z=FIRESIZE;
			//p->s.x=0.67*FIRESIZE+0.4*FIRESIZE*sin(PI*(p->t)/PARTLIFE);
			//p->s.z=0.05*FIRESIZE+0.768*FIRESIZE*(1.0-p->t/PARTLIFE);
			static int nump=0;
			if (!(nump&7)) { p->s*=8.0; p->a*=0.5; }
			p->s*=nrnd(0.2);
			nump++;
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
