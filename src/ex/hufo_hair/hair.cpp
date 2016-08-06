//-------------------------------------------------------------------
//  Created:  13/06/00 19:30:PM
//  Author:    Allard Jeremie ( Hufo / N.A.A. )
//-------------------------------------------------------------------
#include "hair.hpp"

#define LENGTH    HAIRLENGTH
#define LENGTH2   (4.0f*LENGTH)
#define MAXLENGTH (1.2f*LENGTH)
#define MAXLENGTH2 (1.1f*LENGTH)
#define GRAVITY   0.03f
#define STIFFNESS 0.2f
#define STIFF2    0.007f
#define FROT      0.1f
#define MAXL  (MAXLENGTH*MAXLENGTH)
#define MAXL2 (MAXLENGTH2)


void Line::Init(Point3D p0, Point3D dp)
{
  n=dp; n.Normalize();
  p0-=dp;
  for (int i=0;i<HAIRSIZE;i++) {
    pt[i].p=p0;
    p0+=dp;
    dp.x+=rnd(2*HAIRRANDOM)-HAIRRANDOM;
    dp.y+=rnd(2*HAIRRANDOM)-HAIRRANDOM;
    dp.z+=rnd(2*HAIRRANDOM)-HAIRRANDOM;
  }
}

void Hair::Init(Object *_o, Surface *_s)
{
  o=_o; s=_s;
  m=*s;
  m.flag&=~Material::Lighting;
  for (int i=0;i<NBHAIR;i++) {
    int f=rnd(s->nbf);
    TriFace *fc=s->fc+f;
    float a=rnd(1.0f);
    float b=rnd(1.0f);
    if (a+b>1.0f) {
      a=1.0f-a;
      b=1.0f-b;
    }
    Point3D pos=fc->v[0]->p + a*(fc->v[1]->p - fc->v[0]->p) + b*(fc->v[2]->p - fc->v[0]->p);
    Point3D dir=fc->v[0]->n + a*(fc->v[1]->n - fc->v[0]->n) + b*(fc->v[2]->n - fc->v[0]->n);
    dir.Scale(HAIRLENGTH/dir.Length());
    ln[i].Init(pos,dir);
  }
  Color4f col=m.diffuse;
  for (int i=0;i<HAIRNCOLOR;i++) {
    c[i]=col*((i+HAIRNCOLOR/2)*1.0f/HAIRNCOLOR);
  }
}

void Node::Put(int pdata)
{
  glVertex3f(p.x,p.y,p.z);
}

void Line::Aff(int pdata)
{
  //if (pdata&PUT_NORMAL)
    //glNormal3f(n.x,n.y,n.z);
/*
  glBegin(GL_LINE_STRIP);
  Node *p=pt+1;
  int n=HAIRSIZE-1;
  do {
    p->Put(pdata);
    ++p;
  } while (--n);
  glEnd();
*/
  Node *p=pt+1;
  int n=HAIRSIZE-1;
  glInterleavedArrays(GL_V3F,sizeof(Node),p);
  glDrawArrays(GL_LINE_STRIP,0,n);
}

void Hair::Aff()
{
  int pdata=m.Put();
  Line *l=ln;
  int n=NBHAIR;
  do {
    //glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,c[((n-1)*HAIRNCOLOR)/NBHAIR].t);
    glColor4fv(c[((n-1)*HAIRNCOLOR)/NBHAIR].t);
    l->Aff(pdata);
    ++l;
  } while (--n);
}

void Hair::Update(float dt, Point3D Depl)
{
  Point3D v,v2;
  float l,f1,f2;
  Line *pl=ln;
  int i=NBHAIR;
  do {
    int p=HAIRSIZE-2;
    Node *pp=pl->pt+2;
    do {
      pp->p+=Depl;
      v=pp[-1].p-pp[0].p;
      l=v.Length2();
      if (l>MAXL) {
        v*=(1.0f-MAXL2/(float)sqrt(l));
        pp->p+=v;
        pp->v*=1.0f-FROT;
      }
      else {
        f1=STIFFNESS*(1.0f-(LENGTH*LENGTH)/l);
        v2=pp[-2].p-pp[0].p;
        l=v2.Length2();
        f2=0.4f*STIFF2*(1.0f-(LENGTH2*LENGTH2)/l);
        pp->v=pp->v*(1.0f-FROT)+v*f1+v2*f2; //+GForce;
      }
      pp->v.z+=GRAVITY;
      ++pp;
    } while (--p);

    p=HAIRSIZE-2;
    pp=pl->pt+2;
    do {
      pp->p+=pp->v;
      ++pp;
    } while (--p);
    ++pl;
  } while (--i);
}
