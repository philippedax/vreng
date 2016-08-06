/************************************************************************
 *
 *  uborder.hpp: widget border
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

#ifndef _uborder_hpp_
#define	_uborder_hpp_ 1
#include <ubit/uattr.hpp>
#include <ubit/ulength.hpp>
namespace ubit {
  
  class UBorderContent;
  
  /** UBox Border.
   */
  class UBorder : public UAttr {
  public:
    UCLASS(UBorder)
    
    enum {NONE, LINE, SHADOW, ETCHED};
    ///< predefined decorations.
    
    static UBorder none, empty, line, shadowIn, shadowOut, etchedIn, etchedOut;
    ///< predefined borders.
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    UBorder(int decoration = NONE);
    
    UBorder(int decoration, const UColor& color, const UColor& bgcolor,
            float width = 1, float height = 1);
    /**< creates a new Border with custom properties.
     * arguments:
     * - 'decoration' is one of NONE, +/-SHADOW, +/-ETCHED, +/-RECT (a negative 
     *    value sets an INwards shadow, a positive value an OUTwards shadow)
     * - 'color' and 'bgcolor' are the colors of the shadows. These values
     *    are shared and should not be deleted (by calling the delete operator)
     */
    
    UBorder(const UBorder&);
    ///< creates a new Border that is a copy of another Border.
     
    virtual ~UBorder();
    
    virtual UBorder& operator=(const UBorder&);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    int getDecoration() const {return decoration;}
     UBorder& setDecoration(int decoration);
    ///< changes the decoration.
    
    const UColor& getColor() const {return *pcolor;}
    UBorder& setColor(const UColor&);
    ///< changes foreground color; beware that the color arg is NOT duplicated and can't be freed.
    
    const UColor& getBgcolor() const {return *pbgcolor;}
    UBorder& setBgcolor(const UColor&);
    ///< changes background color; beware that the color arg is NOT duplicated and can't be freed.
         
    const UPaddingSpec& getPadding() const {return padding;}
    UBorder& setPadding(const UPaddingSpec&);
    
    bool isRounded() const {return is_rounded;}
    ///< returns true if the border is rounded.
    
    bool isOverlaid() const {return is_overlaid;}
    ///< returns true if the border is overlaid.
    
    UBorder& setOverlaid(bool state);
    
    //float getThickness() const {return thickness;}
    //UBorder& setThickness(float);

    virtual void update();

    virtual void getSize(const UUpdateContext&, UPaddingSpec&) const;
    virtual void paint(UGraph&, const UUpdateContext&, const URect& r) const;
    virtual void paintDecoration(UGraph& g, const URect& r, UElem& obj, 
                                 const UColor& fg, const UColor& bg) const;
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    short decoration;
    bool is_overlaid, is_rounded;
    UPaddingSpec padding;
    uptr<const UColor> pcolor, pbgcolor;
    
    UBorder(int decoration, const UColor& color, const UColor& bgcolor,
            float width, float height, UConst);    
    virtual void constructs(int decoration, const UColor& _color, const UColor& _bgcolor);
    virtual void putProp(UUpdateContext*, UElem&);
#endif
  };
  
  inline UBorder& uborder(UBorder& b) {return *new UBorder(b);}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  /** Rounded border.
   */
  class URoundBorder : public UBorder {
  public:
    UCLASS(URoundBorder)

    URoundBorder(int decoration = NONE);
    URoundBorder(int decoration, const UColor& color, const UColor& bgcolor,
                 float width, float height, float arc_w, float arc_h);

    float getArcWidth()  const {return arc_w;}
    float getArcHeight() const {return arc_h;}
    
    virtual void paintDecoration(UGraph& g, const URect& r, UElem& obj, 
                                 const UColor& fg, const UColor& bg) const;
  protected:
    float arc_w, arc_h;
  };

  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  /** Border that can contain other objects, including elements.
   */
class UCompositeBorder : public UBorder {
  public:
    UCLASS(UCompositeBorder)

    UCompositeBorder();

    UCompositeBorder(const UArgs& children);
    ///< creates a border that contains widgets.
    
    virtual UElem* getSubGroup() const {return pchildren;}
    ///< [Impl] returns the children.
    
    virtual void putProp(UUpdateContext*, UElem&);

protected:    
    uptr<UElem> pchildren;
  };
  
}
#endif
