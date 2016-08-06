/* ***********************************************************************
 *
 *  UGlcontext.hpp: OpenGL rendering context
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

#if UBIT_WITH_GL

#ifndef _UGlcontext_hpp_
#define	_UGlcontext_hpp_ 1
#include <ubit/ugl.hpp>
#include <ubit/urendercontext.hpp>
namespace ubit {

class UHardImaGL;

class UGlcontext : public URenderContext {
public:
#if UBIT_WITH_GLUT
  UGlcontext(UDisp*, class UHardwinGLUT*);
#else
  UGlcontext(UDisp*, UGlcontext* sharelists);
#endif
  virtual ~UGlcontext();
  
  virtual bool isGlcontext() const {return true;}
  virtual UGlcontext* toGlcontext() {return this;}
  virtual const UGlcontext* toGlcontext() const {return this;}
  virtual bool isSharedWith(const URenderContext*) const;

  virtual void setDest(UHardwinImpl* destination, double xoffset, double yoffset);
  virtual void setOffset(double x, double y);
  virtual void setPaintMode(UGraph& g);
  virtual void setXORMode(UGraph& g, const UColor& backcolor);
  virtual void set3Dmode(bool state);
  virtual void setClip(double x, double y, double width, double height);
  
  virtual void setColor(UGraph& g, const UColor&);
  virtual void setBackground(UGraph& g, const UColor&);
  virtual void setFont(UGraph& g, const UFontDesc&);
  virtual void setWidth(UGraph& g, double);

  virtual void makeCurrent() const;
  virtual void swapBuffers();
  virtual void flush();
  
  virtual void drawArc(double x, double y, double w, double h, double start, double ext, bool filled) const;
  virtual void drawIma(const UGraph&, const UIma&, double x, double y, double scale) const;
  virtual void drawLine(double x1, double y1, double x2, double y2) const; 
  virtual void drawPolygon(const float* points, int card, int polytype) const;
  virtual void drawPolygon(const std::vector<UPoint>& points, int polytype) const;
  virtual void drawRect(double x, double y, double w, double h, bool filled) const;
  virtual void drawRoundRect(double x, double y, double w, double h, 
                             double arc_w, double arc_h, bool filled) const;
  virtual void drawString(const UHardFont*, const char* str, int str_len, double x, double y) const;
  
  virtual void copyArea(double x, double y, double w, double h, 
                        double delta_x, double delta_y,
                        bool generate_refresh_events_when_obscured) const;
  ///< not available with OpenGL.
  
private:
  double win_height, cx, cy;  // offset from 'dest' origin with cy converted to lower bound
#if UBIT_WITH_X11
  friend class UDispX11;
  GLXContext glxcontext;
#elif UBIT_WITH_GLUT
  class UHardwinGLUT* hardwin;
#endif
  void drawTex(const UGraph&, const UHardImaGL*, double x, double y, double width, double height) const;
};

}
#endif
#endif

