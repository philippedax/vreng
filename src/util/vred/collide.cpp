#include "vred.hpp"
#include "bbox.hpp"
#include "bsphere.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "group.hpp"


// _depth(x) _width(y) _height(z)
const double epsilon = 1E-5;

#define testHB(Y)   temp=y[j]-y[i]; \
		    if (fabs(temp)>epsilon) { \
     			temp=(Y-y[i])/temp; \
			if (temp>=0&&temp<=1) { \
			    temp=x[i]+temp*(x[j]-x[i]); \
				if (fabs(temp)<w) { \
				    return 1; \
				} \
				} \
			    }

#define testDG(X) 	temp=x[j]-x[i]; \
					if (fabs(temp)>epsilon) { \
						temp=(X-x[i])/temp; \
						if (temp>=0&&temp<=1) { \
							temp=y[i]+temp*(y[j]-y[i]); \
							if (fabs(temp)<d) { \
  							    return 1; \
							} \
						} \
					}

int Bbox::collide(const Bbox& box) const
{
    //separes trivialement en z ?
    if (box.height/2+box.center[3]<=center[3]-height/2) return 0;
    if (height/2+center[3]<=box.center[3]-box.height/2) return 0;

    //transformation   
    double x[4];
    double y[4];
	int mask[4];
	double _x,_y;

	double cosalpha=COS(alpha);
	double sinalpha=SIN(alpha);
	double cosalphaD_=COS(box.alpha)*(box.depth/2.0);
	double sinalphaD_=SIN(box.alpha)*(box.depth/2.0);
	double cosalphaW_=COS(box.alpha)*(box.width/2.0);
	double sinalphaW_=SIN(box.alpha)*(box.width/2.0);

	_x=-cosalphaD_+sinalphaW_+box.center[0]-center[0];
	_y=-sinalphaD_-cosalphaW_+box.center[1]-center[1];
	x[0]=_x*cosalpha-_y*sinalpha; y[0]=_x*sinalpha+_y*cosalpha;

	_x=-cosalphaD_-sinalphaW_+box.center[0]-center[0];
	_y=-sinalphaD_+cosalphaW_+box.center[1]-center[1];
	x[1]=_x*cosalpha-_y*sinalpha; y[1]=_x*sinalpha+_y*cosalpha;

	_x=+cosalphaD_-sinalphaW_+box.center[0]-center[0];
	_y=+sinalphaD_+cosalphaW_+box.center[1]-center[1];
	x[2]=_x*cosalpha-_y*sinalpha; y[2]=_x*sinalpha+_y*cosalpha;

	_x=+cosalphaD_+sinalphaW_+box.center[0]-center[0];
	_y=+sinalphaD_-cosalphaW_+box.center[1]-center[1];
	x[3]=_x*cosalpha-_y*sinalpha; y[3]=_x*sinalpha+_y*cosalpha;

	//calcul du masque

	double d=depth/2.0,w=width/2.0;
	int i;

	for(i=0;i<4;i++) {

		mask[i]=0;
		if (x[i]>d) mask[i]|=1;
		else if (x[i]<-d) mask[i]|=2;
		if (y[i]<-w) mask[i]|=4;
		else if (y[i]>w) mask[i]|=8;
				
#ifdef VERBOSE
		printf("   Point %d (%g,%g) : mask=%d\n",i,x[i],y[i],mask[i]);
#endif
		if (mask[i]==0) {
#ifdef VERBOSE
			printf("   Point à l'intérieur !\n");
#endif
			return 1; 
		}
	}

	//Calcul des intersections

	for (i=0;i<4;i++) {

		int j=(i+1)&3;
#ifdef VERBOSE
		printf("\n   Ligne %d->%d: ",i,j);
#endif
		
		if ((mask[i]&mask[j])!=0) {
#ifdef VERBOSE
			printf("trivialement invisible");
#endif
			continue;
		}

		double temp;
		
		switch (mask[i])
		{
			case 1: testDG(d); break;
			case 2:	testDG(-d); break;
			case 4: testHB(-w); break;
			case 8: testHB(w); break;
			case 5: testDG(d); testHB(-w); break;
			case 6: testDG(-d); testHB(-w); break;
			case 9: testDG(d); testHB(w); break;
			case 10:testDG(-d); testHB(w); break;
		}
		
	}

    //Pas de problemes
#ifdef VERBOSE
    printf("\n");
#endif
    return 0;
}

int Bsphere::collide(const Bsphere& sph) const
{
  Vect val=center;
  val-=sph.center;
  if (val.norm()>=radius*radius+sph.radius*sph.radius+2*sph.radius*radius) return 0;
  else return 1;
}

void Bsphere::calcGroup(int n,Solid** table)
{
  Vect temp,bordi,bordj;

  if (n==0) {
    radius=0.0;
    center=Vect::null;
    return;
  }
  
  //pour le cas n=1 et en cas d'objets identiques
  radius=table[0]->myBoundingSphere.radius;
  center=table[0]->myBoundingSphere.center;
  
  for(int i=0;i<n;i++) {
    
    for(int j=i+1;j<n;j++) {
      
      temp=table[i]->myBoundingSphere.center;
      temp-=table[j]->myBoundingSphere.center;
      
      double d=(sqrt(temp.norm())+table[i]->myBoundingSphere.radius+table[j]->myBoundingSphere.radius)/2.0;
      if (d>radius) {
	radius=d;
	temp*=1/sqrt(temp.norm());
	
	bordi=temp;
	bordi*=table[i]->myBoundingSphere.radius;
	bordi+=table[i]->myBoundingSphere.center;
	
	bordj=temp;
	bordj*=-table[j]->myBoundingSphere.radius;
	bordj+=table[j]->myBoundingSphere.center;
	
	center=bordi;
	center+=bordj;
	center*=0.5;
      }
      
    }   
    
  }
  
#ifdef VERBOSE
  printf("calcGroup: center=<%g %g %g> radius=%g\n",center[0],center[1],center[2],radius);	
#endif
}

// _depth(x) _width(y) _height(z)

#define min(a,b)      temp=(b); if ((a)>temp) (a)=temp;
#define max(a,b)      temp=(b); if ((a)<temp) (a)=temp;
#define minmax(a,b,c) temp=(c); if ((a)>temp) (a)=temp; else if ((b)<temp) (b)=temp;

void Bbox::calcGroup(int n,Solid** table)
{
  alpha=0.0;
  
  if (n==0) {
    depth=height=width=alpha=0.0;
    center=Vect::null;
    return;
  }

  double temp;
  double xmin=table[0]->myBoundingBox.center[0],xmax=table[0]->myBoundingBox.center[0];
  double ymin=table[0]->myBoundingBox.center[1],ymax=table[0]->myBoundingBox.center[1];
  double zmin=table[0]->myBoundingBox.center[2],zmax=table[0]->myBoundingBox.center[2];
  
  for(int i=0;i<n;i++) {

    min(zmin,table[i]->myBoundingBox.center[2]-table[i]->myBoundingBox.height/2.0);
    max(zmax,table[i]->myBoundingBox.center[2]+table[i]->myBoundingBox.height/2.0);

    double cosalphaD_=COS(table[i]->myBoundingBox.alpha)*(table[i]->myBoundingBox.depth/2.0);
    double sinalphaD_=SIN(table[i]->myBoundingBox.alpha)*(table[i]->myBoundingBox.depth/2.0);
    double cosalphaW_=COS(table[i]->myBoundingBox.alpha)*(table[i]->myBoundingBox.width/2.0);
    double sinalphaW_=SIN(table[i]->myBoundingBox.alpha)*(table[i]->myBoundingBox.width/2.0);

    minmax(xmin,xmax,-cosalphaD_+sinalphaW_+table[i]->myBoundingBox.center[0]);
    minmax(ymin,ymax,-sinalphaD_-cosalphaW_+table[i]->myBoundingBox.center[1]);

    minmax(xmin,xmax,-cosalphaD_-sinalphaW_+table[i]->myBoundingBox.center[0]);
    minmax(ymin,ymax,-sinalphaD_+cosalphaW_+table[i]->myBoundingBox.center[1]);

    minmax(xmin,xmax,+cosalphaD_-sinalphaW_+table[i]->myBoundingBox.center[0]);
    minmax(ymin,ymax,+sinalphaD_+cosalphaW_+table[i]->myBoundingBox.center[1]);

    minmax(xmin,xmax,+cosalphaD_+sinalphaW_+table[i]->myBoundingBox.center[0]);
    minmax(ymin,ymax,+sinalphaD_-cosalphaW_+table[i]->myBoundingBox.center[1]);

  }

  center[0]=(xmin+xmax)/2.0;
  center[1]=(ymin+ymax)/2.0;
  center[2]=(zmin+zmax)/2.0;

  depth=xmax-xmin;
  width=ymax-ymin;
  height=zmax-zmin;

  //  printf("   xmin=%g, xmax=%g, ymin=%g, ymax=%g, zmin=%g, zmax=%g\n",xmin,xmax,ymin,ymax,zmin,zmax);
#ifdef VERBOSE
  printf("calcGroup: center=<%g %g %g>, depth=%g, width=%g, height=%g, alpha=%g\n",center[0],center[1],center[2],depth, width, height, alpha);  
#endif
}

void Bbox::set(const Vect& _center, double d, double w, double h, double a)
{
  center=_center;
  depth=d;
  width=w;
  height=h;
  alpha=a;
}

void Bsphere::set(const Vect& _center, double r)
{
  center=_center;
  radius=r;
}

void Camera::updateBB(int recalc)
{
#ifdef VERBOSE
  printf("Camera::updateBB()\n");
#endif
  myBoundingBox.setCenter(getCenter());
  myBoundingSphere.setCenter(getCenter());
}

void Sphere::updateBB(int recalc)
{
#ifdef VERBOSE
  printf("Sphere::updateBB()\n");
#endif
  double radius=getSize()[0];
  myBoundingBox.set(getCenter(),2.0*radius,2.0*radius,2.0*radius,0.0);
  myBoundingSphere.set(getCenter(),radius);

  if (recalc!=1) {
    father->updateBB(2);
  }

  myBoundingBox.print();
  myBoundingSphere.print();
}

void Box::updateBB(int recalc)
{
#ifdef VERBOSE
  printf("Box::updateBB()\n");
#endif
  myBoundingBox.set(getCenter(),getSize()[0],getSize()[1],getSize()[2],getOrient()[2]);
  myBoundingSphere.set(getCenter(),sqrt(getSize().norm())/2.0);

  if (recalc!=1) {
    father->updateBB(2);
  }

  myBoundingBox.print();
  myBoundingSphere.print();
}

void Group::updateBB(int recalc)
{
  #ifdef VERBOSE
  printf("Group::updateBB() : card=%d\n",card);
  #endif

  if (recalc!=2) {
    for (int i = 0; i < card; ++i) {
      group[i]->updateBB(1);
    }
  }

  myBoundingSphere.calcGroup(card,group);
  myBoundingBox.calcGroup(card,group);

  myBoundingBox.print();
  myBoundingSphere.print();
}

int Bbox::inner(Vect pt) const
{
  if (pt[3]<center[3]-height/2.0||pt[3]>center[3]+height/2.0) return 0;

#ifdef VERBOSE
  printf("Point a tester:");
  pt.print();
  printf("\n");
#endif

  double x1=pt[0]-center[0];
  double y1=pt[1]-center[1];

#ifdef VERBOSE
  printf("pt1 %g %g\n",x1,y1);
#endif
  double _cos=COS(-alpha);
  double _sin=SIN(-alpha);

  double x2=_cos*x1-_sin*y1;
  double y2=_sin*x1+_cos*y1;

#ifdef VERBOSE
  printf("pt2 %g %g\n",x2,y2);
#endif

  if (x2<-depth/2.0||x2>depth/2.0) return 0;
  if (y2<-width/2.0||y2>width/2.0) return 0;

  return 1;
}

