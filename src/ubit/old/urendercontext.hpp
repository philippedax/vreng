/* ***********************************************************************
 *
 *  URenderContext.hpp: generic rendering context
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#ifndef _urendercontext_hpp_
#define	_urendercontext_hpp_ 1
#include <ubit/uwinImpl.hpp>
namespace ubit {

class URenderContext {
public: 
  virtual bool isGlcontext() const = 0;
  virtual UGlcontext* toGlcontext() = 0;
  virtual const UGlcontext* toGlcontext() const = 0;
  
  virtual bool isSharedWith(const URenderContext*) const = 0;
  ///< true if the same internal OpenGL reendering context is shared.
  
  UDisp* getDisp() const {return disp;}
  UHardwinImpl* getDest() const {return dest;}
  
  virtual void setDest(UHardwinImpl* dest, double xoffset, double yoffset) = 0;
  /* changes the destination drawable and the corresponding coordinate system.
   * Note that this function does not only change the destination but also the coordinate
   * system according to the destination's size. This is especially important when GL 
   * is used for rendering because the GL origin is the bottom left point of the drawable
   * while the Ubit origin is the top left point (as for most GUI toolkits).
   * Hence:
   * - when GL is used, Y coordinates are converted to drawable_height - Y 
   *   before calling drawing functions
   * - besides, setDest() sets the 'logical offset' to 0,0 (@see setOffset())
   */
   
  virtual void setOffset(double x, double y) = 0;
  /* changes the 'logical offset'.
   * this
   * The 'logical offset' if applied to X,Y coordinates before calling drawing functions.
   * This offset is 'logical' in that sense that it is related to the Ubit coordinate
   * system, where the origin is the top left point of the drawable.
   always related to the Ubit 
   */
  
  virtual void setPaintMode(UGraph&) = 0;
  /* sets this context in paint mode (the default mode).
   * paint mode is the default (and the only mode available when GL is used)
   * the foreground color is initialized according to UGraph values.
   */
  
  virtual void setXORMode(UGraph&, const UColor& backcolor) = 0;
  /* sets this context in XOR mode (not available with GL).
   * XOR mode is not available when GL is used
   * the foreground color is initialized according to UGraph values and the
   * background color according to the second argument.
   */
  
  virtual void set3Dmode(bool state) = 0;
  /// impl.
  
  virtual void setClip(double x, double y, double width, double height) = 0;  
  /// impl.
  
  const URect& getClip() const {return clip;}
  
  virtual void setColor(UGraph&, const UColor&) = 0;
  virtual void setBackground(UGraph&, const UColor&) = 0;
  virtual void setFont(UGraph&, const UFontDesc&) = 0;
  virtual void setWidth(UGraph&, double) = 0;

  virtual void makeCurrent() const = 0;
  /* makes this context the current rendering context.
   * calls glMakeCurrent() if this context is a UGlcontext
   */
  
  virtual void swapBuffers() = 0;
  /* swaps the front and back buffers of the current destination drawable.
   * calls glSwapBuffers() if this context is a UGlcontext
   */
  
  virtual void flush() = 0;
  /* flushes all drawing request.
   * calls glFlush() when GL is used.
   */
  
  virtual void drawArc(double x, double y, double w, double h, 
                       double start, double ext, bool filled) const = 0;

  virtual void drawIma(const UGraph&, const UIma&, double x, double y, 
                       double scale) const = 0;

  virtual void drawLine(double x1, double y1, double x2, double y2) const = 0;  

  virtual void drawPolygon(const float* points, int card, int polytype) const = 0;
  virtual void drawPolygon(const std::vector<UPoint>& points, int polytype) const = 0;
  
  virtual void drawRect(double x, double y, double w, double h, bool filled) const = 0;
  virtual void drawRoundRect(double x, double y, double w, double h, 
                             double arc_w, double arc_h, bool filled) const = 0;

  virtual void drawString(const UHardFont*, const char* str, int str_len, 
                          double x, double y) const = 0;
  
  virtual void copyArea(double x, double y, double w, double h, double delta_x, double delta_y,
                        bool generate_refresh_events_when_obscured) const = 0;

protected:
  URenderContext(UDisp* d) : disp(d), dest(null), xwin(0), ywin(0), own_dest(false) {}  
  virtual ~URenderContext() {if (own_dest) delete dest;}

  friend class UGraph;
  friend class UDisp;
  UDisp* disp;
  UHardwinImpl* dest;    // where graphics are drawn (can be a Window or a Pixmap)
  URect clip;
  double xwin, ywin; // offset from 'dest' origin
  bool own_dest;     // true if 'dest' is a pixmap that must thus be destroyed with this GC
};

}
#endif
