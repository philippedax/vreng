/*************************************************************************
 *
 *  ugraph.hpp: Graphics Layer
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

#ifndef _ugraph_hpp_
#define	_ugraph_hpp_ 1
#include <vector>
#include <ubit/udefs.hpp>
#include <ubit/ucolor.hpp>
#include <ubit/ugeom.hpp>
namespace ubit {
  
  
struct UGraphAttributes {
  UGraphAttributes() : color_rgba(0u,0u,0u,255u), bgcolor_rgba(255u,255u,255u,255u) {}
  short font_styles;
  float width;
  unsigned long color, bgcolor;      // for 2D_GRAPHICS
  URgba color_rgba, bgcolor_rgba;    // for GL
  enum {OPAQUE, UNIFORM_BLEND, PSEUDO_BLEND, TRUE_BLEND} blend_mode;
  bool in_xor_mode;
  bool in_3d_mode;      // true when displaying inside a 3Dcanvas
};

  
/** class for drawing on widgets.
  * Widgets inherit from UBox. A box can manage one or several views. UGraph makes
  * it possible to draw graphic primitives on these views.
  *
  * @see also: UBox, UView.
 */
class UGraph : public UGraphAttributes {
public:
  /** for client GL Graphics.
   */
  class Glpaint {
  public:
    Glpaint(UPaintEvent&, bool _push_attrib);
    Glpaint(UView*, bool _push_attrib);
    virtual ~Glpaint();
    virtual void begin(UView*, bool _push_attrib);
    virtual void end();
  protected:
    static int count;
    int no;
    bool push_attrib;
    UHardwinImpl* hardwin;
  };
  
  
  UGraph(UPaintEvent&) throw (UError);
  /**< constructor for drawing in a UOn::paint callback method.
   * Example:
   * <pre>
   *   void MyObj::paintCB(UPaintEvent& e) {
    *       UGraph g(e);
    *       g.setColor(UColor::red);  // predefined red
    *       g.drawRect(10, 10, 50, 50);
    *       g.setColor(UColor(0, 0, 255, 128));  // semi-transparent blue
    *       g.drawLine(10, 10, 10+50, 10+50);
    *   }
    *
    *   int main() {
    *       MyObj* obj = new MyObj();
    *       UBox* box = new UBox();
    *       box->add(UOn::paint / ucall(obj, &MyObj::paintCB));
    *       .....
    *   }
   * </pre>
   * The last line adds a paint callback to the box. ucall(...) is a callback
   * object that will fire the paintCB() instance method of MyObj each time the box
   * is repainted. paintCB() could also have arguments or be a static function:
   * @see UCall for more details.
   *
   * The paintCB() method has a UPaintEvent& parameter that can be passed to 
   * the UGraph constructor. This will initialize the UGraph object with appropriate
   * attributes and the redraw clip corresponding to this paint event.
   *
   * Note that UGraph objects should always be created in this way, so that they 
   * are implicitely destroyed when the function returns. UGraph objects may not
   * be copied nor stored for future use.
   *
   * font, colors, and other attributes are undefined when the UGraph object
   * is created, they must be specified by using the appropriate functions
   * (such as UGraph::setFont(), setColor(), etc.
   *
   * Ubit objects that derive from UBox can have multiple UView(s). The callback
   * function is then fired once for each view (and the UPaintEvent parameter
   * is set accordingly for each view)
   *
   * Note that this constr. raises an exception if the corresponding UView is null
   */
  
  virtual ~UGraph();
    
  UBox* getBox() const;
  /**< returns the widget where this UGraph can draw on.
    * this function is a shortcut for: getView()->getBox().
    * @see: getView().
    */
  
  UView* getView() const {return boxview;}
  /**< returns the view of the widget where this UGraph can draw on.
    * widgets inherit from UBox. A box can manage one or several views.
    * view->getBox() returns the box that owns this view (@see UView::getBox()).
    */
  
  UDisp* getDisp() const {return disp;}
  ///< returns the display that is related to this UGraph.

  URect getClip() const {URect r; getClip(r); return r;}
  ///< returns the drawing zone relatively to the origin of getView().
  
  void getClip(URect&) const;
  ///< returns the drawing zone relatively to the origin of getView().

  void setClip(const URect&);
  ///< changes the drawing zone relatively to the origin of getView().

  // ==== Attributes ===========================================================

  void setPaintMode();
  /**< sets the drawing mode in Paint mode.
    * "Paint mode" is the default "normal" drawing mode. In Paint Mode, drawing
    * primitives overwrites pixels with the paint color, which must set by calling
    * setColor() and can have an (optional) alpha channel. 
    * @see: setXORMode(), setColor(), setBackground().
    */
    
  void setXORMode(const UColor& background_color);
  /** sets the drawing mode in XOR mode.
    * In "XOR mode", drawing primitives alternates the paint and the background 
    * colors, which must be set by calling setColor() and setBackground().
    * NOTES: 
    * - !!! the current OpenGL implementation does NOT support XOR mode !!!
    * - alpha channel cannot be used in in XOR mode
    * @see: setPaintMode(), setColor(), setBackground().
    */
  
  bool isXORMode() const {return in_xor_mode;}
  /**< returns the drawing mode.
    * see setXORMode()
    */
  
  void setColor(const UColor&);
  /**< changes the paint color (which can have an alpha component).
    * The paint color is used by all drawing primitives. Example:
    * <pre>
    *   void paintCB(UPaintEvent& e) {
    *       UGraph g(e);
    *       g.setColor(UColor::red);  // predefined red
    *       g.drawRect(10, 10, 50, 50);
    *       g.setColor(UColor(0, 0, 255, 128));  // semi-transparent blue
    *       g.drawLine(10, 10, 10+50, 10+50);
    *   }
    * </pre>
    * @see also: setPaintMode(), setXORMode(), setBackground().
    */
    
  void setColor(const UColor&, float alpha);
  /**< changes the paint color (forces this alpha component, in range [0,1]).
   * @see setColor(const UColor&).
   */
  
  void setColor(const UColor&, unsigned int alpha);
  /**< changes the paint color (forces this alpha component, in range [0,255]).
   * @see setColor(const UColor&).
    */
    
  void setBackground(const UColor&);
  /**< changes the background color.
   */
  
  void setFont(const UFont&);
  /**< changes the current Font. 
    */
  
  void setFont(const UFontDesc&);
  /**< changes the current Font. 
    */
  
  void getFontMetrics(const UFont&, UFontMetrics&) const;
  /**< return the corresponding Font Metrics. 
    */
    
  void setWidth(float);
  /**< changes the line width (when drawing lines, etc).
    * in X11 mode the default value is 0, this will draw 1 pixel width lines in an optimized way
    */ 
  
  float getWidth() const;
  /**< returns the current line width.
    * @see: setWidth()
    */
       
  void flush() const;
  ///< flushes all drawing requests.

  // ==== Drawing routines =====================================================
     
  void draw(const UShape&) const;
  ///< draws a shape.
  
  void fill(const UShape&) const;
  ///< fills a shape.

  void drawArc(double x, double y, double width, double height, 
               double start_angle, double ext_angle) const;
  /**< draws a circular or elliptical arc.
    * 'x,y,width,height' are the top/left position and size of the enclosing rectangle
    * 'start_ang' is the beginning angle and 'ext_ang' the extent of the arc relative
    *  to 'start_ang'. Angles are in degrees.
    */
  
  void fillArc(double x, double y, double width, double height, 
               double start_angle, double ext_angle) const;
  ///< fills an arc of a circle (or a complete circle); same arguments as drawArc().
  
  void drawEllipse(double x, double y, double width, double height) const; 
  ///< draws an ellipse or a circle that fits in this rectangle.
        
  void fillEllipse(double x, double y, double width, double height) const; 
  ///< fills an ellipse or a circle that fits in this rectangle.
  
  void drawIma(const UIma&, double x, double y, double scale = 1.) const;
  ///< draws this image with this scale.

  void drawLine(double x1, double y1, double x2, double y2) const;
  ///< draws a line.
  
  void drawLine(const UPoint& p1, const UPoint& p2) const;
  ///< draws a line.
  
  static const int LINE_STRIP, LINE_LOOP, FILLED;

  void drawPolygon(const std::vector<UPoint>& points, int type = LINE_LOOP) const;
  /**< draws a polyline, polygon or a filled polygon depending on 'type'.
    * type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).
    */
  
  void drawPolygon(const float* points, int npoints, int type = LINE_LOOP) const;
  /**< draws a polyline, polygon or a filled polygon depending on 'type'.
    * 'points' format is x0, y0, x1, y1, x2, y2... it must contains 2*npoints values
    * type is one of LINE_STRIP (polyline), LINE_LOOP (polygon), FILLED (filled polygon).
    */
     
  void drawPolyline(const std::vector<UPoint>& points) const {drawPolygon(points, LINE_STRIP);}
  ///< draws a polyline.
     
  void fillPolygon(const std::vector<UPoint>& points) const {drawPolygon(points, FILLED);}
  ///< fills a polygon (that must be convex).
  
  void drawRect(double x, double y, double width, double height) const;
  ///< draws a rectangle.
 
  void fillRect(double x, double y, double width, double height) const;
  ///< fills a rectangle.
    
  void drawRoundRect(double x, double y, double width, double height, 
                     double arc_w, double arc_h) const;
  /**< draws a round-cornered rectangle.
    * arc_w, arc_h are the horiz/vert diameters of the arc at the 4 corners
    */
  
  void fillRoundRect(double x, double y, double width, double height, 
                     double arc_w, double arc_h) const;
  ///< fills a round-cornered rectangle; same arguments as fillRect(), drawRoundRect().
  
  void drawString(const UStr& string, double x, double y) const;
  ///< draw a character string (the current Font is used: @see setFont()).

  void drawString(const char* string, int length, double x, double y) const;
  ///< draw a character string (the current Font is used: @see setFont()).

  void copyArea(double x, double y, double width, double height,
                double delta_x, double delta_y, 
                bool paint_events_when_obscured) const;
  /** copies a rectangular area (X11 only, does not work with OpenGL).
    *  copies a rectangular area of size (width,height) at location (x,y) 
    *  to a location shifted by delta_x, delta_y
    * <br>Notes: 
    *  - does nothing if the toolkit is using OpenGL as its graphics lib
    *  - CopyArea can't copy obscured parts of the component but will generate refresh
    *    events for repainting the missing parts if the last argument is true 
    */
    
  // === Impl. =================================================================
#ifndef NO_DOC

  UGraph(UView*) throw (UError);
  /**< constructor for drawing in a view.
   * see UGraph(UPaintEvent&) for details.
   * This constr. raises an exception if the UView is null
   */
  
  UHardwinImpl* getHardwin() const {return hardwin;}
  /* [Impl] returns the hard window where this UGraph can draw on.
   * Ubit Windows (UWin and subclasses) can either be hard (system windows) or
   * soft (simulated), in which case they are drawn on the top of a hard window.
   * getHardwin() always returns the hard window.
   */
  
  void getHardwinClip(URect&) const; 
  ///< [Impl] returns the drawing zone relatively to the current hard window.
  
  void setHardwinClip(const URect&);
  ///< [Impl] changes the drawing zone relatively to the current hard window.
  
  void set3Dmode(bool state);
  bool in3Dmode() const {return in_3d_mode;}
    
  void getWinOffset(float& x, float& y) const;  
  void setWinOffset(float x, float y) const;  
  static void setViewportOrtho(UHardwinImpl*);

private:
  UGraph(UGraph&);
  UGraph& operator=(const UGraph&);
protected:
  friend class UView;
  friend class U3DcanvasView;
  friend class UPaintEvent;
  friend class UFontMetrics;
  friend class URenderContext;
  friend class UGlcontext;
  friend class UX11context;
  UDisp* disp;         // display 
  UView* boxview;      // view where the graphics are drawn
  UHardwinImpl* hardwin;   // corresponding hard window 
  UHardFont* font;     // native font that is currently used
  URenderContext* rc;  // corresponding rendering context
  bool is_client;
#endif // NO_DOC
};

}
#endif
