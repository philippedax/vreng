/************************************************************************
 *
 *  ugeom.hpp: graphics geometry
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
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

#ifndef _ugeom_hpp_
#define	_ugeom_hpp_ 1
#include <ubit/uobject.hpp>
namespace ubit {
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** 2D Point.
   */
  class UPoint {
  public:
    float x, y;
    
    UPoint() : x(0), y(0) {}
    UPoint(float _x, float _y) : x(_x), y(_y) {}
    
    float getX() const {return x;}
    float getY() const {return y;}
    
    void set(float _x, float _y) {x = _x, y = _y;}
    void set(const UPoint& p) {*this = p;}
    
    friend std::ostream& operator<<(std::ostream&, const UPoint&);
    ///< prints the point on an output stream (eg: cout << point).
    
    friend std::istream& operator>>(std::istream&, UPoint&);
    ///< reads the point from an input stream (eg: cin >> point).  
  };
  
  std::ostream& operator<<(std::ostream&, const UPoint&);
  ///< prints the point on an output stream (eg: cout << point).
  
  std::istream& operator>>(std::istream&, UPoint&);
  ///< reads the point from an input stream (eg: cin >> point).  
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** 2D Dimension.
   */
  class UDimension {
  public:
    float width, height;
    
    UDimension() : width(0), height(0) {}
    UDimension(float _w, float _h) : width(_w), height(_h) {}
    
    float getWidth()  const {return width;}
    float getHeight() const {return height;}
    
    void set(float _w, float _h) {width = _w, height = _h;}
    void set(const UDimension& d) {*this = d;}
    
    friend std::ostream& operator<<(std::ostream&, const UDimension&);
    ///< prints the dimension on an output stream (eg: cout << dimension).
    
    friend std::istream& operator>>(std::istream&, UDimension&);
    ///< reads the dimension from an input stream (eg: cin >> dimension).    
  };
  
  std::ostream& operator<<(std::ostream&, const UDimension&);
  ///< prints the dimension on an output stream (eg: cout << dimension).
  
  std::istream& operator>>(std::istream&, UDimension&);
  ///< reads the dimension from an input stream (eg: cin >> dimension).
  
  
  // =============================================================================
  /** Abstract Base class for geometrical shapes.
   */
  class UShape : public UObject {
  public:
    UABSTRACT_CLASS(UShape)
    
    enum ShapeType {
      LINE    = 1,
      RECT    = 2,
      ELLIPSE = 3,
      ARC     = 4,
      POLYGON = 5
    };
    
    virtual int getShapeType() const  = 0;
    
    virtual URect getBounds() const = 0;
    ///< returns the bounding box of this shape.
    
    virtual bool contains(const UPoint&) const = 0; 
    ///< tests if a given point is inside the boundary of this Shape.
    
    virtual bool contains(const URect&) const = 0; 
    ///< tests if the interior of this Shape entirely contains the specified Rectangle
    
    virtual bool intersects(const URect&) const = 0; 
    ///< tests if this Shape intersects the interior of a specified Rectangle.
    
    virtual void draw(const UGraph&) const = 0;
    virtual void fill(const UGraph&) const = 0;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** 2D Line.
   */
  class ULine : public UShape {
  public:
    UCLASS(ULine)

    UPoint p1, p2;
    
    int getShapeType() const {return LINE;}
    
    ULine() {}
    ULine(const UPoint& _p1, const UPoint& _p2) : p1(_p1), p2(_p2) {}
    
    URect getBounds() const;
    UPoint getP1() const {return p1;}
    UPoint getP2() const {return p2;}
    
    virtual void setLine(const UPoint& _p1, const UPoint& _p2) {p1 = _p1; p2 = _p2;}
    virtual void setLine(const ULine& l) {*this = l;}
    
    bool contains(const UPoint&) const {return false;}    
    bool contains(const URect&) const {return false;}
    
    bool intersects(const URect&) const; 
    ///< tests if this Line intersects the interior of a specified Rectangle.
    
    bool intersectsLine(const ULine&) const;
    ///< tests if the specified line segment intersects this Line.
    
    static bool linesIntersect(const UPoint& a, const UPoint& b, const UPoint& c, const UPoint& d);
    ///< tests if the line segment [A,B] intersects the line segment from [C,D]
    
    static int relativeCCW(UPoint a, UPoint b, UPoint p);
    /**< Returns an indicator of where P lies with respect to the line segment [A,B].
     * The return value can be either 1, -1, or 0 and indicates in which direction 
     * the specified line must pivot around its first endpoint, A, in order to point 
     * at the specified point P.
     */
    
    int relativeCCW(const UPoint&) const;
    ///< Returns an indicator of where the specified Point2D lies with respect to this line segment.
    
    virtual void draw(const UGraph&) const;
    virtual void fill(const UGraph&) const {};
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** 2D Rectangle.
   */
  class URect : public UShape {
  public:
    UCLASS(URect)

    float x, y, width, height;
    
    int getShapeType() const {return RECT;}  
    
    URect() : x(0), y(0), width(0), height(0) {}
    URect(float _x, float _y, float _w, float _h) : x(_x), y(_y), width(_w), height(_h) {}
    URect(const UPoint& p1, const UPoint& p2) {setRect(p1, p2);}
    
    URect getBounds() const {return *this;}
    bool isEmpty() const {return width <= 0.0 || height <= 0.0;}
    
    void setRect(const URect& r) {*this = r;}
    void setRect(float x, float y, float width, float height);
    void setRect(const UPoint& p1, const UPoint& p2);
    void setFrame(const URect& r) {*this = r;}
    
    bool contains(const UPoint&) const;
    bool contains(const URect&) const;
    
    bool intersects(const URect&) const; 
    ///< tests if this Rectangle intersects the interior of a specified Rectangle.
    
    bool intersectsLine(const ULine&) const; 
    ///< tests if the specified line segment intersects the interior of this Rectangle.
    
    bool doIntersection(const URect&);
    /**< performs the intersection with the rectangle that is given as an argument.
     * returns false if there is no intersection (this rectangle is then left unchanged)
     */
    
    void doUnion(const URect&);
    /**< performs the union with the rectangle that is given as an argument.
     * Note: empty regions are not taken into account.
     */
    
    enum {OUT_LEFT = 1, OUT_TOP = 2, OUT_RIGHT = 4, OUT_BOTTOM = 8};
    
    int outcode(const UPoint& p) const;
    ///< determines where the specified coordinates lie with respect to this Rectangle.
    
    virtual void draw(const UGraph&) const;
    virtual void fill(const UGraph&) const;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** 2D Ellipse.
   */
  class UEllipse : public UShape {
  public:
    UCLASS(UEllipse)

    float x, y, width, height;
    
    int getShapeType() const {return ELLIPSE;}  
    
    UEllipse();
    UEllipse(const URect&);
    UEllipse(float x, float y, float weight, float height);
    
    URect getBounds() const;
    bool isEmpty() const {return width <= 0.0 || height <= 0.0;}
    
    void setFrame(const URect&);
    void setFrame(float x, float y, float width, float height);
    
    bool contains(float x, float y) const;
    bool contains(const UPoint&) const;
    bool contains(const URect&) const;
    bool intersects(const URect&) const; 
    
    virtual void draw(const UGraph&) const;
    virtual void fill(const UGraph&) const;
  };
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** 2D Polygon or Polyline.
   */
  class UPolygon : public UShape {
  public:
    UCLASS(UPolygon)

    virtual int getShapeType() const {return POLYGON;}
    
    virtual ~UPolygon();
    
    UPolygon(bool closed = true);
    ///< creates an empty polygon ('closed'=true) or polyline ('closed'=false).
    
    UPolygon(const std::vector<UPoint>& points, bool closed = true);
    ///< creates and initalizes a polygon ('closed'=true) or polyline ('closed'=false).
    
    UPolygon(const UPolygon& poly2);
    ///< creates a new polygon (or a polyline) that is a copy of 'poly2'.
    
    UPolygon& operator=(const UPolygon& poly2);
    ///< copies the content of 'poly2' to this polygon.
    
    UPolygon& addPoint(const UPoint& point);
    ///< adds a point to the polygon.
    
    UPolygon& addPoints(const std::vector<UPoint>& points);
    ///< adds these points to the polygon.
    
    UPolygon& addPoints(const float* _points, int _npoints);
    ///< adds these points to the polygon.
    
    URect getBounds() const {return bounds;}
    ///< returns the bounding box of the polygon.
    
    void translate(float dx, float dy);
    ///< translates the polygon. 
    
    void reset();
    ///< resets this Polygon object to an empty polygon.  
    
    bool contains(const UPoint&) const;
    bool contains(const URect&) const;
    bool intersects(const URect&) const;
    
    void draw(const UGraph&) const;
    void fill(const UGraph&) const;
    
  private:
    int npoints, memsize;
    bool closed;
    float* points;   // the array contains x0, y0, x1, y1, x2, y2...
    URect bounds;
    int evaluateCrossings(float x, float y, bool useYaxis, float distance) const;
    void augment(int n);
    void updateBounds(float x, float y);
  };
  
}
#endif
