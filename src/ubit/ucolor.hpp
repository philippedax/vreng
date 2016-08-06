/* ***********************************************************************
 *
 *  ucolor.hpp: Color Attribute
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
#ifndef _ucolor_hpp_
#define	_ucolor_hpp_ 1
#include <string.h>
#include <ubit/uattr.hpp>
namespace ubit {
  
  /** Specifies a RGBA value for defining UColor(s).
   */
  class URgba {
  public:
    URgba() {comps[0] = comps[1] = comps[2] = 0; comps[3] = 255u;}
        
    URgba(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255u) 
    {comps[0] = r; comps[1] = g; comps[2] = b; comps[3] = a;}
    
    URgba(float r, float g, float b, float a = 1.0f) {setRgbaF(r,g,b,a);}

    void setRgbaI(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255u) 
    {comps[0] = r; comps[1] = g; comps[2] = b; comps[3] = a;}
    
    void setRgbaF(float r, float g, float b, float a = 1.0f);

    bool operator==(const URgba& c) const {
      return (comps[0]==c.comps[0] && comps[1]==c.comps[1] 
              && comps[2]==c.comps[2] && comps[3]==c.comps[3]);
    }    

    //bool equals(unsigned int r, unsigned int g, unsigned int b, unsigned int a) const {
    //return components[0]==r && components[1]==g && components[2]==b && components[3]==a;}
    
    //bool equals(float r, float g, float b, float a) const;

    unsigned int getRedI()   const {return comps[0];}
    unsigned int getGreenI() const {return comps[1];}
    unsigned int getBlueI()  const {return comps[2];}
    unsigned int getAlphaI() const {return comps[3];}
    
    void setRedI(unsigned int c)   {comps[0] = c;}
    void setGreenI(unsigned int c) {comps[1] = c;}
    void setBlueI(unsigned int c)  {comps[2] = c;}
    void setAlphaI(unsigned int c) {comps[3] = c;}
    
    unsigned char comps[4];  // must be compatible with GLubyte
  };
  
  
  /** Color attribute of an element or a widget.
   * This attributes specifies the foreground color of an object that derives
   * from UElem (i.e. element nodes and widgets). This color will be inherited
   * by the children of this element if they dont't specify a color of their own.
   *
   * UColor attributes can either be added to the element's attribute or child list.
   *
   * The alpha value is ignored if OpenGL is not used: see UAppli::isUsingGL().
   *
   * @see also: UAttr.
   */
  class UColor : public UAttr {
  public:
    UCLASS(UColor)
    
    static UColor none, inherit, white, black, grey, lightgrey, darkgrey, 
    navy, lightblue, blue, red, green, yellow, orange, wheat, teal, disabled;
    /**< predefined colors.
     * the value of these color are predefined and unmutable (attempting to change
     * them would produce an error)
     */
    
    UColor();
    ///< creates a solid black color.
    
    UColor(const UColor& color);
    ///< creates a new color object that is a copy of 'color'.
        
    UColor(const UColor& color, float a);
    ///< creates a new color object that is a copy of 'color' but with a different alpha.

    UColor(const UColor& color, unsigned int a);
    ///< creates a new color object that is a copy of 'color' but with a different alpha.

    UColor(const URgba&);
    ///<creates a new color object from a URgba value.
    
    UColor(float r, float g, float b, float a = 1.0f);
    /**< creates a new color object with a RGBA value; see also shortcut ucolor(r,g,b,a).
     * r,g,b,a are floats in the range [0 - 1]. To avoid ambiguities, they MUST be
     * explicitely specified as floats, for instance: UColor(1f, 0f, 0f, 0.5f).
     */
    
    UColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255u);
    /**< creates a new color object with a RGBA value; see also shortcut ucolor(r,g,b,a).
     * r,g,b,a are unsigned integers in the range [0 - 255]. To avoid ambiguities, they
     * MUST be explicitely specified as unsigned ints, e.g.: UColor(255u, 0u, 0u, 125u).
     */
  
    UColor(const UStr& color_name, float a = 1.);
    ///<creates a new color object from a color name (see setNamedColor()).

    ~UColor();
    
    UColor& operator=(const UColor& c);
    ///< changes color from another color.

    UColor& setRgba(const URgba&);
    ///< changes color, arguments is a URgba struct.
         
    UColor& setRgbaF(float r, float g, float b, float a = 1.0f);
    ///< changes color, arguments are floats in range [0 - 1].
    
    UColor& setRgbaI(unsigned int r, unsigned int g, unsigned int b, unsigned int a=255u);
    ///< changes color, arguments are unsigned ints in range [0 - 255].
    
    UColor& setNamedColor(const UStr& color_name, float a = 1.);
    /**< changes color, arguments are a color name and an alpha value in range [0,1].
     * 'color_name' can be on of the following:
     * - #rgb or #rrggbb or #rrrrggggbbbb with r,g,b being hexadecimal digits
     * - grayXXX with XXX being a number from 0 to 100
     * - a HTML color name
     * the alpha value is ignored if OpenGL is not used for rendition: see UAppli::isUsingGL()
     */
    
    const URgba& getRgba() const {return rgba;}
     
    bool operator==(const UColor& c) const {return equals(c);}
    bool operator!=(const UColor& c) const {return !equals(c);}
    
    virtual bool equals(const UColor&) const;
    virtual bool equals(const URgba&) const;
    virtual bool equals(float r, float g, float b, float a) const;
    virtual bool equals(unsigned int r, unsigned int g, unsigned int b, unsigned int a) const;
    
    virtual void update();  // redefinition

    virtual void putProp(UUpdateContext*, UElem&);   // redefinition
    
    static void addNamedColor(const char* name, const URgba&);
    ///< adds a color to the database of named colors.
    
    static bool parseColor(const char* name, URgba&);
    ///< returns the URgba corresponding to this color name, if found.
    
    // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    friend class UGraph;
    URgba rgba;
#if WITH_2D_GRAPHICS
    mutable std::vector<unsigned long> pixels;
    void unsetPixels();
    unsigned long getPixel(UDisp*) const;
#endif
    UColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a, UConst);
    //[Impl] creates a new CONSTANT color with a RGB value; @see UObject::UCONST.
    
    UColor(unsigned char special, UConst);
    //[Impl] creates a new CONSTANT color with a special value; @see UObject::UCONST.
    
#endif
  };
  
  
  inline UColor& ucolor() {return *new UColor();}
  ///< shortcut function that performs *new UColor();
  
  inline UColor& ucolor(float r, float g, float b, float a = 1.) 
  {return *new UColor(r,g,b,a);}
  ///< shortcut function that performs *new UColor(r, g, b, a);
  
  inline UColor& ucolor(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255) 
  {return *new UColor(r,g,b,a);}
  ///< shortcut function that performs *new UColor(r, g, b, a);
  
  inline UColor& ucolor(const UStr& color_name, float alpha = 1.) 
  {return *new UColor(color_name, alpha);}
  ///< shortcut function that performs *new UColor(color_name, alpha);

}
#endif
