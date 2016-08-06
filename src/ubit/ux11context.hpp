#if (UBIT_WITH_X11 && WITH_2D_GRAPHICS)

#ifndef _UX11context_hpp_
#define	_UX11context_hpp_ 1
#include <ubit/urendercontext.hpp>
namespace ubit {

class UHardIma2D;
  
class UX11context : public URenderContext {
public:
  UX11context(UDisp*);
  virtual ~UX11context();

  virtual bool isGlcontext() const {return false;}
  virtual UGlcontext* toGlcontext() {return null;}
  virtual const UGlcontext* toGlcontext() const {return null;}
  virtual bool isSharedWith(const URenderContext*) const {return false;}

  virtual void setDest(UHardwinImpl* d, double x, double y) {dest=d; xwin=x; ywin=y;}
  virtual void setOffset(double x, double y) {xwin = x; ywin = y;}
  virtual void setPaintMode(UGraph&);
  virtual void setXORMode(UGraph&, const UColor& backcolor);
  virtual void set3Dmode(bool state) {}
  virtual void setClip(double x, double y, double width, double height);

  virtual void setColor(UGraph&, const UColor&);
  virtual void setBackground(UGraph&, const UColor&);
  virtual void setWidth(UGraph&, double);
  virtual void setFont(UGraph&, const UFontDesc&);

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
  virtual void copyArea(double x, double y, double w, double h, double delta_x, double delta_y,
                        bool generate_refresh_events_when_obscured) const;
protected:
  void drawHardIma(const UHardIma2D*, double x, double y) const;
  void drawHardPix(const UHardPix*, double x, double y) const;
private:
  Display* sys_disp;
  GC sys_gc;
  //if WITH_2D_GRAPHICS
  //URect clipbegin;
  //endif
};

}
#endif
#endif
