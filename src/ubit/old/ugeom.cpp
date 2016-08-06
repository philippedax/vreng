/************************************************************************
 *
 *  ugeom.cpp: Geometry
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
 *
 * ***********************************************************************
 * COPYRIGHT NOTICE :
 * THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
 * IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
 * EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
 * SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
 * ***********************************************************************/

/* derived from : Polygon.java, Line2D.java, Rectangle2D.java
 * Copyright (C) 1999, 2002, 2004, 2005 Free Software Foundation, Inc. 
 * Copyright 2004 Sun Microsystems, Inc. All rights reserved.
 * authors Jim Graham, Aaron M. Renn (arenn@urbanophile.com), Eric Blake (ebb9@email.byu.edu)
 *
 * This file is part of GNU Classpath.
 * 
 * GNU Classpath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Classpath is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Classpath; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 * 
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 * 
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module. An independent module is a module which is not derived from
 * or based on this library. If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version. 
 */
#include <ubit/ubit_features.h>
#include <iostream>
#include <cfloat>
#include <cmath>
#include <ubit/ubit_features.h>
#include <ubit/ugeom.hpp>
#include <ubit/ugraph.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

ostream& operator<<(ostream& s, const UPoint& p) {
  return (s << p.x << " " << p.y);
}
  
istream& operator>>(istream& s, UPoint& p) {
  return (s >> p.x >> p.y);
}

ostream& operator<<(ostream& s, const UDimension& d) {
  return (s << d.width << " " << d.height);
}
  
istream& operator>>(istream& s, UDimension& d) {
  return (s >> d.width >> d.height);
}

// =========================================================== [Elc] ===========

void ULine::draw(const UGraph& g) const {
  g.drawLine(p1.x, p1.y, p2.x, p2.y);
}
//void ULine::fill(const UGraph& g) const {} in header

URect ULine::getBounds() const {
  float x, y, w, h;
  if (p1.x < p2.x) {
    x = p1.x;
    w = p2.x - p1.x;
  } 
  else {
    x = p2.x;
    w = p1.x - p2.x;
  }
  
  if (p1.y < p2.y) {
    y = p1.y;
    h = p2.y - p1.y;
  } 
  else {
    y = p2.y;
    h = p1.y - p2.y;
  }
  return URect(x, y, w, h);
}

/* pas de sens
void ULine::setFrame(const URect& r) {
  p1.x = r.x;
  p1.y = r.y;
  p2.x = r.x + r.width;
  p2.y = r.y + r.height;
}
*/
  
bool ULine::intersects(const URect& r) const {
  return r.intersectsLine(*this);
}

bool ULine::intersectsLine(const ULine& l) const {
  return linesIntersect(l.p1, l.p2, p1, p2);
}

bool ULine::linesIntersect(const UPoint& a, const UPoint& b, const UPoint& c, const UPoint& d) {
  return ((relativeCCW(a, b, c) * relativeCCW(a, b, d) <= 0)
          && (relativeCCW(c, d, a) * relativeCCW(c, d, b) <= 0));
}
 
int ULine::relativeCCW(const UPoint& p) const {
  return relativeCCW(p1, p2, p);
}

int ULine::relativeCCW(UPoint p1, UPoint p2, UPoint p) {
  p2.x -= p1.x;
  p2.y -= p1.y;
  p.x -= p1.x;
  p.y -= p1.y;
  float ccw = p.x * p2.y - p.y * p2.x;
  if (ccw == 0.0) {
    // The point is colinear, classify based on which side of
    // the segment the point falls on. We can calculate a
    // relative value using the projection of PX,PY onto the
    // segment - a negative value indicates the point projects
    // outside of the segment in the direction of the particular
    // endpoint used as the origin for the projection.
    ccw = p.x * p2.x + p.y * p2.y;
    if (ccw > 0.0) {
      // Reverse the projection to be relative to the original p2.x,p2.y
      // p2.x and p2.y are simply negated.
      // PX and PY need to have (p2.x - p1.x) or (p2.y - p1.y) subtracted
      // from them (based on the original values)
      // Since we really want to get a positive answer when the
      // point is "beyond (p2.x,p2.y)", then we want to calculate
      // the inverse anyway - thus we leave p2.x & p2.y negated.
      p.x -= p2.x;
      p.y -= p2.y;
      ccw = p.x * p2.x + p.y * p2.y;
      if (ccw < 0.0) ccw = 0.0;
    }
  }
  return (ccw < 0.0) ? -1 : ((ccw > 0.0) ? 1 : 0);
}

/* ==================================================== ===== ======= */
/* ==================================================== ===== ======= */
 
void URect::draw(const UGraph& g) const {
  g.drawRect(x, y, width, height);
}  

void URect::fill(const UGraph& g) const {
  g.fillRect(x, y, width, height);
} 

void URect::setRect(float _x, float _y, float _w, float _h) {
  x = _x, y = _y, width = _w, height = _h;
}

void URect::setRect(const UPoint& p1, const UPoint& p2) {
  x = min(p1.x, p2.x);
  y = min(p1.y, p2.y);
  width  = max(p1.x, p2.x) - x;
  height = max(p1.y, p2.y) - y;
}

bool URect::contains(const UPoint& p) const {
  if (isEmpty()) return false;
  return (p.x >= x && p.y >= y 
          && p.x < (float)x + width && p.y < (float)y + height); 
}

bool URect::contains(const URect& r) const {
  if (isEmpty() || r.width <= 0 || r.height <= 0) return false;
  return (r.x >= x && r.y >= y 
          && (float)r.x + r.width  < (float)x + width 
          && (float)r.y + r.height < (float)y + height); 
}

bool URect::intersects(const URect& r) const {
  if (isEmpty() || r.width <= 0 || r.height <= 0) return false;
  return (r.x + (float)r.width > x && r.y + (float)r.height > y 
          && r.x < x + (float)width && r.y < y + (float)height);
}

// effectue l'intersection de *this avec clip2
// *this est mis a jour (contient l'intersection), clip2 ne change pas
// renvoie:
// * false si pas d'intersection (region inchangee)
// * true sinon si region est incluse dans clip (ie. clipping inutile)

bool URect::doIntersection(const URect& clip2) {
  bool stat = false;
  
  // *** en x
  
  if (clip2.x < x) {
    if (clip2.x + clip2.width < x) {
      width = height = 0;	// securite !
      return false;// no inter
    }
    
    // fait: width = UMIN(x + width, clip2.x + clip2.width) - x;
    
    else if (x + width <= clip2.x + clip2.width) {
      //region incluse horizontalement dans clip (clip inutile)
      stat = true;
    }
    else {
      width = clip2.x + clip2.width -x;
      // inter simple
      stat = true;
    }
  }
  
  else {    // (x <= clip2.x)
    if (x + width <= clip2.x) {
      width = height = 0;	// securite !
      return false;
    }
    else if (x + width <= clip2.x + clip2.width) {
      width = x + width - clip2.x;
      x = clip2.x;		// !att a l'ordre!
      stat = true;
    }
    else {
      width = clip2.width;
      x = clip2.x;
      // clip inclus dans region (renvoie 1: clip necessaire)
      stat = true;
    }
  }
  
  // *** en y
  
  if (clip2.y < y) {
    if (clip2.y + clip2.height < y) {
      width = height = 0;	// securite !
      return false;// no inter
    }
    
    // fait: height = UMIN(y + height, clip2.y + clip2.height) - y;
    
    else if (y + height <= clip2.y + clip2.height) {
      // region incluse horizontalement
      // *ET* verticalement dans clip (clip inutile)
      stat = true;
    }
    else {
      height = clip2.y + clip2.height -y;
      // inter simple
      stat = true;
    }
  }
  
  else {    // (y <= clip2.y)
    if (y + height <= clip2.y) {
      width = height = 0;	// securite !
      return false;// no inter
    }
    
    // height = UMIN(y + height, clip2.y + clip2.height) - clip2.y;
    else if (y + height <= clip2.y + clip2.height) {
      height = y + height - clip2.y;
      y = clip2.y;		// !att a l'ordre!
      stat = true;
    }
    else {
      height = clip2.height;
      y = clip2.y;
      // clip inclus dans region (renvoie 1: clip necessaire)
      stat = true;
    }
  }
  
  // renvoyer false si vide!
  // ceci exclut les "regions ponctuelles" sans epaisseur
  if (width <= 0 || height <= 0) return false;
  else return stat;
}

// les regions vides ne sont pas prises en compte !
void URect::doUnion(const URect& clip2) {
  if (width <= 0 || height <= 0) {
    *this = clip2;
  }
  else if (clip2.width <= 0 || clip2.height <= 0) {
    /*nop*/;
  }
  else {
    if (x > clip2.x) x = clip2.x;
    if (y > clip2.y) y = clip2.y;
    
    if (x + width < clip2.x + clip2.width)
      width = clip2.x + clip2.width - x;
    if (y + height < clip2.y + clip2.height)
      height = clip2.y + clip2.height - y;
  }
}

bool URect::intersectsLine(const ULine& l) const {
  int out1, out2;
   
  if ((out2 = outcode(l.p2)) == 0) return true;
        
  float x1 = l.p1.x;
  float y1 = l.p1.y;
  float x2 = l.p2.x;
  float y2 = l.p2.y;
  
  while ((out1 = outcode(l.p1)) != 0) {
    if ((out1 & out2) != 0) return false;
    
    if ((out1 & (OUT_LEFT | OUT_RIGHT)) != 0) {
      float x = this->x;
      if ((out1 & OUT_RIGHT) != 0) x += width;
      
      y1 = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
      x1 = x;
    } 
    else {
      float y = this->y;
      if ((out1 & OUT_BOTTOM) != 0) y += height;
    
      x1 = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
      y1 = y;
    }
  }
  return true;
}

int URect::outcode(const UPoint& p) const {
  /*
   * Note on casts to float below. If the arithmetic of
   * x+w or y+h is done in float, then some bits may be
   * lost if the binary exponents of x/y and w/h are not
   * similar. By converting to float before the addition
   * we force the addition to be carried out in float to
   * avoid rounding error in the comparison.
   */
  int out = 0;
  if (width <= 0) {
    out |= OUT_LEFT | OUT_RIGHT;
  } else if (p.x < x) {
    out |= OUT_LEFT;
  } else if (p.x > x + (float)width) {
    out |= OUT_RIGHT;
  }
  if (height <= 0) {
    out |= OUT_TOP | OUT_BOTTOM;
  } else if (p.y < y) {
    out |= OUT_TOP;
  } else if (p.y > y + (float)height) {
    out |= OUT_BOTTOM;
  }
  return out;
}

/* ==================================================== ===== ======= */
/* ==================================================== ===== ======= */

void UEllipse::draw(const UGraph& g) const {
  g.drawEllipse(x, y, width, height);
}

void UEllipse::fill(const UGraph& g) const {
  g.drawEllipse(x, y, width, height);
}  

UEllipse::UEllipse() 
: x(0), y(0), width(0), height(0) {}

UEllipse::UEllipse(float _x, float _y, float _w, float _h) 
: x(_x), y(_y), width(_w), height(_h) {}

UEllipse::UEllipse(const URect& r) 
: x(r.x), y(r.y), width(r.width), height(r.height) {}

URect UEllipse::getBounds() const {
  return URect(x,y,width,height);
}

void UEllipse::setFrame(const URect& r) {
  x = r.x, y = r.y, width = r.width, height = r.height;
}

void UEllipse::setFrame(float _x, float _y, float _w, float _h) {
  x = _x, y = _y, width = _w, height = _h;
}

bool UEllipse::contains(float xx, float yy) const {
  // Normalize the coordinates compared to the ellipse
  // having a center at 0,0 and a radius of 0.5.
  if (width <= 0.0) return false;
  float normx = (xx - x) / width - 0.5;
  
  if (height <= 0.0) return false;
  float normy = (yy - y) / height - 0.5;
  
  return (normx * normx + normy * normy) < 0.25;
}

bool UEllipse::contains(const UPoint& p) const {
  return contains(p.x, p.y);
}
  
bool UEllipse::contains(const URect& r) const {
  return (contains(r.x, r.y)
          && contains(r.x + r.width, y)
          && contains(r.x, r.y + r.height) 
          && contains(r.x + r.width, r.y + r.height));
}

bool UEllipse::intersects(const URect& r) const {
  if (r.width <= 0.0 || r.height <= 0.0) return false;
        
  // Normalize the rectangular coordinates compared to the ellipse
  // having a center at 0,0 and a radius of 0.5.
  float ellw = width;
  if (ellw <= 0.0) return false;

  float normx0 = (r.x - x) / ellw - 0.5;
  float normx1 = normx0 + r.width / ellw;
  
  float ellh = height;
  if (ellh <= 0.0) return false;
  
  float normy0 = (r.y - y) / ellh - 0.5;
  float normy1 = normy0 + r.height / ellh;
  
  // find nearest x (left edge, right edge, 0.0)
  // find nearest y (top edge, bottom edge, 0.0)
  // if nearest x,y is inside circle of radius 0.5, then intersects
  float nearx, neary;

  if (normx0 > 0.0) nearx = normx0; // center to left of X extents
  else if (normx1 < 0.0) nearx = normx1; // center to right of X extents
  else nearx = 0.0;

  if (normy0 > 0.0) neary = normy0;  // center above Y extents
  else if (normy1 < 0.0) neary = normy1;// center below Y extents
  else neary = 0.0;
  
  return (nearx * nearx + neary * neary) < 0.25;
}

/* ==================================================== ===== ======= */
#if 0  // REVOIR contains() et intersects()

struct UArc : public UShape {
  float x, y, width, height, start, extent;
  
  UCLASS("uarc", UArc, new UArc)
  int getShapeType() const {return ARC;}  
  
  UArc();
  UArc(const URect&, float start, float extent, int type = 0);
  UArc(float _x, float _y, float _w, float _h, float _s, float _e, int _t);
  
  URect getBounds() const;
  bool isEmpty() const {return width <= 0.0f || height <= 0.0;}
  
  void setFrame(const URect&);
  void setArc(const URect&, float start, float extent, int closure = 0);
  void setArc(float _x, float _y, float _w, float _h, float _s, float _e, int _t);
  
  float getStart()  const {return start;}
  float getExtent() const {return extent;}
  void setStart(float angle)  {start = angle;}
  void setExtent(float angle) {extent = angle;}
  
  bool contains(const UPoint&) const;
  bool contains(const URect&) const;
  bool intersects(const URect&) const; 
  
  virtual void draw(const UGraph&) const;
  virtual void fill(const UGraph&) const;
};


UArc::UArc() 
: x(0), y(0), width(0), height(0), start(0), extent(0) {}

UArc::UArc(const URect& r, float _s, float _e, int _t) 
: x(r.x), y(r.y), width(r.width), height(r.height), start(_s), extent(_e) {}

UArc::UArc(float _x, float _y, float _w, float _h, float _s, float _e, int _t) 
: x(_x), y(_y), width(_w), height(_h), start(_s), extent(_e) {}

URect UArc::getBounds() const {
  return URect(x,y,width,height);
}

/*
void UArc::set(const UPoint& p1, const UPoint& p2, float _s, float _e, int _t) {
  URect r(p1, p2);
  x = r.x, y = r.y, width = r.width, height = r.height;
  start = _s, extent = _e;
}
*/

void UArc::setFrame(const URect& r) {
  x = r.x, y = r.y, width = r.width, height = r.height;
}

void UArc::setArc(const URect& r, float _s, float _e, int closure) {
  x = r.x, y = r.y, width = r.width, height = r.height , start = _s, extent = _e;
}

void UArc::setArc(float _x, float _y, float _w, float _h, float _s, float _e, int _t) {
  x = _x, y = _y, width = _w, height = _h; start = _s, extent = _e;
}
  
// FAUX !!!!
bool UArc::contains(const UPoint& p) const {
  return (p.x >= x && p.y >= y && p.x < x+width && p.y < y+height); 
}

bool UArc::contains(const URect& r) const {
  return (r.x >= x && r.y >= y 
          && r.x+r.width-1 < x+width && r.y+r.height-1 < y+height); 
}

void UArc::draw(const UGraph& g) const {
  g.drawArc(x, y, width, height, start, extent);
}

void UArc::fill(const UGraph& g) const {
  g.fillArc(x, y, width, height, start, extent);
}

#endif
/* ==================================================== ===== ======= */
/* ==================================================== ===== ======= */

// A big number, but not so big it can't survive a few float operations.
static const float BIG_VALUE = FLT_MAX / 10.0;

void UPolygon::draw(const UGraph& g) const {
  if (closed) g.drawPolygon(points, npoints, UGraph::LINE_LOOP);
  else g.drawPolygon(points, npoints, UGraph::LINE_STRIP);
}  

void UPolygon::fill(const UGraph& g) const {
  g.drawPolygon(points, npoints, UGraph::FILLED);
} 
  
UPolygon::UPolygon(bool _closed)
: npoints(0), memsize(4), closed(_closed), points(new float[memsize]) {
}

UPolygon::UPolygon(const std::vector<UPoint>& _points, bool _closed)
: npoints(0), memsize(0), closed(_closed), points(null) {
  addPoints(_points);
}

UPolygon::UPolygon(const UPolygon& p2)
: npoints(0), memsize(0), closed(p2.closed), points(null) {
  addPoints(p2.points, p2.npoints);
}

UPolygon::~UPolygon() {
  delete [] points;
}

void UPolygon::reset() {
  npoints = 0, memsize = 0, closed = true;
  bounds.setRect(0,0,0,0);
  delete [] points;
}

UPolygon& UPolygon::operator=(const UPolygon& p2) {
  reset();
  closed = p2.closed;
  addPoints(p2.points, p2.npoints);
  return *this;
}

void UPolygon::augment(int n) {
  int npoints2 = npoints*2;  // the array contains x0, y0, x1, y1, x2, y2...
  int n2 = n*2;
  
  if (npoints2 + n2 > memsize) {
    memsize *= 2;
    if (memsize < n2) memsize = n2;
    
    float* new_points = new float[memsize];
    ::memcpy(new_points, points, npoints2 * sizeof(*points));
    delete [] points;
    points = new_points;
  }
}

void UPolygon::updateBounds(float x, float y) {
  if (npoints == 1) {
    bounds.x = x;
    bounds.y = y;
  }
  else {
    if (x < bounds.x) {
      bounds.width += bounds.x - x;
      bounds.x = x;
    }
    else if (x > bounds.x + bounds.width) {
      bounds.width = x - bounds.x;
    }  
    if (y < bounds.y) {
      bounds.height += bounds.y - y;
      bounds.y = y;
    }
    else if (y > bounds.y + bounds.height) {
      bounds.height = y - bounds.y;
    }
  }  
}

UPolygon& UPolygon::addPoint(const UPoint& p) {
  augment(1);
  points[npoints*2] = p.x;
  points[npoints*2 +1] = p.y;
  npoints++;
  updateBounds(p.x, p.y);
  return *this;
}

UPolygon& UPolygon::addPoints(const std::vector<UPoint>& _points) {
  augment(_points.size());
  for (unsigned int k = 0, kk = npoints*2; k < _points.size(); ++k, kk += 2) {
    points[kk] = _points[k].x;
    points[kk +1] = _points[k].y;
    updateBounds(points[kk], points[kk+1]);
  }
  npoints += _points.size();
  return *this;
}

UPolygon& UPolygon::addPoints(const float* _points, int _npoints) {
  augment(_npoints);
  unsigned int _npoints2 = _npoints*2;
  for (unsigned int k = 0, kk = npoints *2; k < _npoints2; k += 2, kk += 2) {
    points[kk] = _points[k];
    points[kk+1] = _points[k+1];
    updateBounds(points[kk], points[kk+1]);
  }
  npoints += _npoints;
  return *this;
}

/*
void UPolygon::setFrame(const URect& r) {
  points[0] = r.x;
  points[1] = r.y;
}
*/
  
void UPolygon::translate(float dx, float dy) {
  int npoints2 = npoints*2;  // the array contains x0, y0, x1, y1, x2, y2...
  for (int k = 0; k < npoints2; k += 2) { 
    points[k] += dx;
    points[k+1] += dy;
  }
  bounds.x += dx;
  bounds.y += dy;
}

/*
URect UPolygon::getBounds() const {
  if (bounds == null) {
    if (npoints == 0) return bounds = new Rectangle();
    int i = npoints - 1;
    int minx = xpoints[i];
    int maxx = minx;
    int miny = ypoints[i];
    int maxy = miny;
    
    while (--i >= 0) {
      int x = xpoints[i];
      int y = ypoints[i];
      if (x < minx) minx = x; else if (x > maxx) maxx = x;
      if (y < miny) miny = y; else if (y > maxy) maxy = y;
    }
    bounds = new Rectangle(minx, miny, maxx - minx, maxy - miny);
  }
  return bounds;
}
*/

bool UPolygon::contains(const UPoint& p) const {
 return closed && (evaluateCrossings(p.x, p.y, false, BIG_VALUE) & 1) != 0;
}
  
/* Test if a high-precision rectangle lies completely in the shape. This is
* true if all points in the rectangle are in the shape. This implementation
* is precise.
*/
bool UPolygon::contains(const URect& r) const {
  if (!closed || !getBounds().intersects(r)) return false;
  
  // Does any edge intersect?
  if (evaluateCrossings(r.x, r.y, false, r.width) != 0 /* top */
    || evaluateCrossings(r.x, r.y + r.height, false, r.width) != 0 /* bottom */
    || evaluateCrossings(r.x + r.width, r.y, true, r.height) != 0 /* right */
    || evaluateCrossings(r.x, r.y, true, r.height) != 0) /* left */
    return false;
  
  // No intersections, is any point inside?
  if ((evaluateCrossings(r.x, r.y, false, BIG_VALUE) & 1) != 0)
    return true;
  
  return false;
}
  
bool UPolygon::intersects(const URect& r) const {
  // does any edge intersect?
  if (evaluateCrossings(r.x, r.y, false, r.width) != 0 /* top */
    || evaluateCrossings(r.x, r.y + r.height, false, r.width) != 0 /* bottom */
    || evaluateCrossings(r.x + r.width, r.y, true, r.height) != 0 /* right */
    || evaluateCrossings(r.x, r.y, true, r.height) != 0) /* left */
    return true;
  
  // no intersections, is any point inside?
  if ((evaluateCrossings(r.x, r.y, false, BIG_VALUE) & 1) != 0) return true;
  
  return false;
}

/* Helper for contains, intersects, calculates the number of intersections
* between the polygon and a line extending from the point (x, y) along
* the positive X, or Y axis, within a given interval. return the winding number.
*/
int UPolygon::evaluateCrossings(float x, float y, bool useYaxis, float distance) const {
  int XX = 0, YY = 1;
   
  if (useYaxis) {   // the roles of X and Y are inverted
    XX = 1; YY = 0;
    float swap = y; y = x; x = swap;
  }
  
  // Get a value which is small but not insignificant relative the path.
  float epsilon = 1E-7;
  UPoint p0, p1;
  int crossings = 0;

  p0.x = points[0 + XX] - x;
  p0.y = points[0 + YY] - y;
  if (p0.y == 0.0) p0.y -= epsilon;
  
  int npoints2 = npoints*2;  // the array contains x0, y0, x1, y1, x2, y2...
  for (int k = 2; k < npoints2; k += 2) {
    p1.x = points[k + XX] - x;
    p1.y = points[k + YY] - y;
    //cerr << "p0 " << p0.x << " " <<p0.y << " p1 " << p1.x << " " <<p1.y;
    //if (p0.y == 0.0) p0.y -= epsilon;
    if (p1.y == 0.0) p1.y -= epsilon;
    
    if (p0.y * p1.y < 0) {
      if (ULine::linesIntersect(p0, p1, UPoint(epsilon, 0.0), UPoint(distance, 0.0))) {
        ++crossings;
        //cerr << " HIT "<<crossings;
      }
    }
    //cerr << endl;
    p0 = p1;
  }
  
  // end segment
  
  p1.x = points[0 + XX] - x;
  p1.y = points[0 + YY] - y;
  //if (p0.y == 0.0) p0.y -= epsilon;
  if (p1.y == 0.0) p1.y -= epsilon;
  //cerr << "p0 " << p0.x << " " <<p0.y << " p1 " << p1.x << " " <<p1.y;

  if (p0.y * p1.y < 0) {
    if (ULine::linesIntersect(p0, p1, UPoint(epsilon, 0.0), UPoint(distance, 0.0))) {
      ++crossings;
      //cerr << " HIT "<<crossings;
    }
  }
  
  //cerr << endl << "-- CROSSINGS "<<crossings << endl << endl;
  return crossings;
}
  
  
}

