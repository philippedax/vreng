/************************************************************************
 *
 *  ubackground.hpp: Widget background and transparency.
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

#ifndef ubackground_hpp
#define	ubackground_hpp 1
#include <ubit/uattr.hpp>
namespace ubit {
  

  /** Widget background.
   * This attributes controls the background of widgets (objects deriving from UBox).
   * If not added to the widget's attribute list, the default background specified
   * in the widget's UStyle is used. This default background if transparent for most
   * widgets, so that the background of the first non transparent parent widget
   * will appear.
   */
  class UBackground : public UAttr {
  public:
    UCLASS(UBackground)
    
    static UBackground none, inherit,
    metal, velin, white, black, grey, lightgrey, darkgrey, navy, lightblue, blue, 
    red, green, yellow, orange, wheat, teal;
    ///< predefined background constants.
    
    ~UBackground();
    
    UBackground();
    ///< creates an empty background attribute.
    
    UBackground(const UBackground&);
    ///< creates a background from another one.
    
    UBackground(const UColor&);
    ///< creates a background prop with a solid color (the UColor argument is copied).
    
    UBackground(UIma&);  
    ///< creates a background prop with an image (the UIma argument is copied).
    
    UBackground& operator=(const UBackground&);
    ///< copies a background attribute from another one.

    bool operator==(const UBackground&);
    ///< compares two background attributes.

    virtual UBackground& setColor(const UColor& color);
    /**< changes the background color.
     * the UColor argument is copied internally so that this background will remain
     * valid if this argument is destroyed.
     */
    
    virtual UBackground& setRgba(const URgba&);
    ///< changes the background color.
     
    virtual UBackground& setRbgaF(float r, float g, float b, float a = 1.);
    ///< changes the background color, arguments are floats in range [0,1].
    
    virtual UBackground& setRbgaI(unsigned int r, unsigned int g, unsigned int b,
                                      unsigned int a = 255);
    ///< changes the background color, arguments are integers in range [0,255].

    virtual UBackground& setNamedColor(const UStr& color_name, float a = 1.);
    ///< changes the background color, arguments are a color name and an alpha value in range [0,1].
    
    virtual UBackground& setIma(UIma& image, bool tiling = true);
    /**< changes the background image (WARNING: see details).
     * WARNING: the UIma argument is NOT copied internally: this background will
     * NOT be valid if this argument is destroyed.
     */
    
    UBackground& setTiling(bool);
    ///< sets tiling on or off.
    
    const UColor* getColor() const;
    ///< returns the current color if any; null otherwise.
    
    const UIma* getIma() const;
    ///< returns the current image if any; null otherwise.
    
    bool isTiled() const;
    ///< returns true if the image is tiled.
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
        
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    UBackground(UColor&, UConst);
    UBackground(UIma&, UConst);
  protected:
    uptr<UColor> pcolor;
    uptr<UIma> pima;
    float alpha;
    bool tiling;
#endif
  };
  
  inline UBackground& ubackground() {return *new UBackground();}
  ///< shortcut for *new UBackground().
  
  inline UBackground& ubackground(const UColor& col) {return *new UBackground(col);}
  ///< shortcut for *new UBackground(col).

  inline UBackground& ubackground(UIma& ima) {return *new UBackground(ima);}
  ///< shortcut for *new UBackground(ima).

  // ==================================================== Ubit Toolkit =========
  /** Widget transparency.
   * value must be in range [0.0, 1.0]. A value of 1. makes the widget opaque,
   * a value of 0. makes it fully transparent
   */
  class UAlpha : public UAttr {
  public:
    UCLASS(UAlpha)
    
    UAlpha(float = 1.0);
    /**< creates an alpha blending property; see also shortcut function ualpha().
     * specifies if the widget (UBox and subclass) that contains this prop
     * is opaque (value = 1.0), fully stransparent (value = 0.0)
     * or translucent (value in range ]0.0, 1.0[)
     */
    
    operator float() const {return val;}
    ///< type conversion: returns value  which is in range [0., 1.].
    
    UAlpha& operator=(float v)         {return set(v);}
    UAlpha& operator=(const UFloat& v) {return set(v);}
    UAlpha& operator=(const UAlpha& v) {return set(v.val);}
    virtual UAlpha& set(float);  
    ///< changes alpha value. must be in range [0., 1.].
    
    bool operator==(float v)         const {return val == v;}  
    bool operator==(const UFloat& v) const {return val == v;}  
    bool operator==(const UAlpha& v) const {return val == v.val;}
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
    
  private:
    float val;
  };
  
  inline UAlpha& ualpha(float value = 1.0) {return *new UAlpha(value);}
  ///< shortcut function that returns *new UAlpha(value).
  
}
#endif

