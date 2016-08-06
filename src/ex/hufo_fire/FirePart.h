
// double to float conv
#pragma warning(disable:4244)

#ifdef _DEBUG
#include <assert.h>
#endif

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

inline Float operator*(const SVector3D &a,const SVector3D &b)
{
	return a.DotProduct(b);
}
inline SVector3D operator*(const SVector3D &a,Float b)
{
	return SVector3D(a.x*b,a.y*b,a.z*b);
}
inline SVector3D &operator*=(SVector3D &a,Float b)
{
	a.Scale(b);
	return a;
}
/*inline SVector3D operator+(const SVector3D &a,const SVector3D &b)
{
	return SVector3D(a.x+b.x,a.y+b.y,a.z+b.z);
}*/
inline SVector3D operator^(const SVector3D &a,const SVector3D &b)
{
	SVector3D c; c.CrossProduct(a,b);
	return c;
}

class SMatrix3D
{
public:
	SVector3D x,y,z;
	SMatrix3D() :x(1,0,0),y(0,1,0),z(0,0,1) {}
	void FromAngle(float ax,float ay,float az)
	{
		Float cx=cos(ax); Float sx=sin(ax);
		Float cy=cos(ay); Float sy=sin(ay);
		Float cz=cos(az); Float sz=sin(az);
		// Calcul de la matrice de transformation 3D
		x.x=cy*cz; x.y=cx*sz*cy+sx*sy; x.z=sx*sz*cy-cx*sy;
		y.x=-sz;	 y.y=cx*cz;	         y.z=sx*cz;
		z.x=cz*sy; z.y=cx*sz*sy-sx*cy; z.z=sx*sz*sy+cx*cy;
	}
	void Apply(const SVector3D &v,SVector3D *out)
	{
		out->x=v*x;
		out->y=v*y;
		out->z=v*z;
	}
	void RApply(SVector3D const &v,SVector3D *out)
	{
		out->x=v.x*x.x+v.y*y.x+v.z*z.x;
		out->y=v.x*x.y+v.y*y.y+v.z*z.y;
		out->z=v.x*x.z+v.y*y.z+v.z*z.z;
	}
	void Apply(SVector3D *v)
	{
		SVector3D i(*v);
		Apply(i,v);
	}
	void RApply(SVector3D *v)
	{
		SVector3D i(*v);
		RApply(i,v);
	}
};

#define NBPARTMAX 1024
#define PARTLIFE 3.0
#define PARTINTERV 0.01
#define FIRESIZE 1.0f
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
struct Quad
{
	Vertex v[4];
};

Quad FGround;

#define NCYL 3
struct WCyl
{
	SVector3D p;
	SVector3D d;
	float r;
} FCyl[NCYL]; // wood cylinders

#define S2_2 0.7071f
float Circle[9][2]=
{{ 1.0f, 0.0f}
,{ S2_2, S2_2}
,{ 0.0f, 1.0f}
,{-S2_2, S2_2}
,{-1.0f, 0.0f}
,{-S2_2,-S2_2}
,{ 0.0f,-1.0f}
,{ S2_2,-S2_2}
,{ 1.0f, 0.0f}
};

SVector3D FireSrc(0.0f,5.0f,-1.5f);
SVector3D FireDS1(2.0f,0.0f,0.0f);
SVector3D FireDS2(0.0f,2.0f,0.0f);
SVector3D FireDir(0.0f,0.0f,0.25f);
SVector3D FPartA(0.0f,0.0f,1.0f);
#define NCYLPART 4
struct GLCyl
{
	Vertex vTriFan1[10];
	Vertex vCyl[NCYLPART+1][9];
	Vertex vTriFan2[10];
} glCyl[NCYL]; // cylinders aff data

void FireInit()
{
	//NoiseInit();
	np=0;
	LastPartTime=0.0;
	FireAnim=True; FireRotate=False; FireRot=PI/5.0; FireAng=0.0;
	FireStop=False; FireRecalc=True;
	FCyl[0].r=FIRECYLR;
	FCyl[0].d=FireDS1*2.0f;
	FCyl[0].p=FireSrc-FCyl[0].d*0.5f; FCyl[0].p.z-=FIRECYLR;
	FCyl[1].r=FIRECYLR;
	FCyl[1].d=(FireDS1*0.85f+FireDS2*0.5f)*2.0f;
	FCyl[1].p=FireSrc-FCyl[1].d*0.5f; FCyl[1].p.z-=FIRECYLR;
	FCyl[2].r=FIRECYLR;
	FCyl[2].d=(FireDS1*-0.5f+FireDS2*0.85f)*2.0f;
	FCyl[2].p=FireSrc-FCyl[2].d*0.5f; FCyl[2].p.z-=FIRECYLR;
}
void CalcFire(float t,float dt)
{
	int i,n;
	Particle *p=TblP;
	n=0;
	float ds=pow(FIREDS,dt);
	float da=pow(FIREDA,dt);
	float f;
	SVector3D a;
	if (FireAnim)
	{
		while (n<np)
		{
			if ((p->t-=dt)<=0)
			{ // kill it
				*p=TblP[--np];
			}
			else
			{ // animate it
				//a=p->p; a.z-=5*t;
				a=FPartA; //+Noise(a)*(p->p.z-FireSrc.z)*1.0; //a.x*=4.0f; a.y*=4.0f;
				p->v+=a*dt;
				p->p+=p->v*dt;
				//p->s*=ds;
				p->s.x=0.67*FIRESIZE+0.4*FIRESIZE*sin(PI*(p->t)/PARTLIFE);
				p->s.z=0.05*FIRESIZE+0.768*FIRESIZE*(1.0-p->t/PARTLIFE);
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
			//p->p=FireSrc+FireDS1*nrnd(1.0)+FireDS2*nrnd(1.0);
			do { f=nrnd(0.2); } while (abs(f)>0.45);
			p->p=FireSrc+FCyl[hrnd(NCYL)].d*f;
			p->v=FireDir;
			p->t=LastPartTime+PARTLIFE-t;
			p->a=FIREALPHA;
			p->s.x=FIRESIZE; p->s.y=FIRESIZE; p->s.z=FIRESIZE;
			p->s.x=0.67*FIRESIZE+0.4*FIRESIZE*sin(PI*(p->t)/PARTLIFE);
			p->s.z=0.05*FIRESIZE+0.768*FIRESIZE*(1.0-p->t/PARTLIFE);
		}
	}
	else LastPartTime+=dt;
	n=0; p=TblP;
	SVector3D v;
	SMatrix3D m;
	SVector3D o;
	if (FireRotate) { FireAng+=FireRot*dt; FireRecalc=True; }
	m.FromAngle(0,0,FireAng);
	m.Apply(FireSrc,&o);
	o=FireSrc-o;
	while (n<np)
	{
		m.Apply(p->p,&v);
		v+=o;
		p->ex=ProjEX(v); p->ey=ProjEY(v); ProjEZ(p->ez,v);
		p->dx=FireFocX*p->s.x/v.y;
		p->dy=FireFocY*p->s.z/v.y;
		++n;
		++p;
	}
	SVector3D vg;
	for (n=0;n<4;++n)
	{
		vg=FireSrc; vg.y-=2*FIRECYLR;
		if ((n+1)&2) vg+=FireDS1; else vg-=FireDS1;
		if ( n   &2) vg+=FireDS2; else vg-=FireDS2;
		m.Apply(vg,&v);
		v+=o;
		FGround.v[n].ex=HVCtrX+FireFocX*v.x/v.y;
		FGround.v[n].ey=HVCtrY+FireFocY*v.z/v.y;
		FGround.v[n].ex=ProjEX(v); FGround.v[n].ey=ProjEY(v); ProjEZ(FGround.v[n].ez,v);
		FGround.v[n].u=((n+1)&2)?1.0:0.0;
		FGround.v[n].v=( n   &2)?1.0:0.0;
	}
	if (FireRecalc)
	{
		SVector3D cp,cd,cm,cn;
		for (int c=0;c<NCYL;++c)
		{
			m.Apply(FCyl[c].p,&cp); cp+=o;
			m.Apply(FCyl[c].d,&cd);
#ifdef NOZBUFFER
			if (cd.y>0.0) // back to forward sorting
#else
			if (cd.y<0.0) // forward to back sorting
#endif
			{
				cp+=cd;
				cd*=-1.0;
			}
			v.x=0.0f; v.y=0.0f; v.z=1.0f;
			cm.CrossProduct(v,cd);
			cn.CrossProduct(cd,cm);
			cm*=FIRECYLR/cm.Length();
			cn*=FIRECYLR/cn.Length();
			cd*=1.0f/NCYLPART;
			glCyl[c].vTriFan1[0].ex=ProjEX(cp);
			glCyl[c].vTriFan1[0].ey=ProjEY(cp);
			ProjEZ(glCyl[c].vTriFan1[0].ez,cp);
			for (i=0;i<9;++i)
			{
				v=cp+cm*Circle[i][0]+cn*Circle[i][1];
				glCyl[c].vTriFan1[i+1].ex=ProjEX(v);
				glCyl[c].vTriFan1[i+1].ey=ProjEY(v);
				ProjEZ(glCyl[c].vTriFan1[i+1].ez,v);
			}
			for (n=0;n<NCYLPART+1;++n)
			{
				if (n) cp+=cd;
				for (i=0;i<9;++i)
				{
					v=cp+cm*Circle[i][0]+cn*Circle[i][1];
					glCyl[c].vCyl[n][i].ex=ProjEX(v);
					glCyl[c].vCyl[n][i].ey=ProjEY(v);
					ProjEZ(glCyl[c].vCyl[n][i].ez,v);
				}
			}
			glCyl[c].vTriFan2[0].ex=ProjEX(cp);
			glCyl[c].vTriFan2[0].ey=ProjEY(cp);
			ProjEZ(glCyl[c].vTriFan2[0].ez,cp);
			for (i=0;i<9;++i)
			{
				v=cp+cm*Circle[i][0]+cn*Circle[i][1];
				glCyl[c].vTriFan2[i+1].ex=ProjEX(v);
				glCyl[c].vTriFan2[i+1].ey=ProjEY(v);
				ProjEZ(glCyl[c].vTriFan2[i+1].ez,v);
			}
		}
		FireRecalc=False;
	}
}
