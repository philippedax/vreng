/************************************************************************
 *
 *  ufont.hpp: Font Attribute
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

#ifndef _ufont_hpp_
#define	_ufont_hpp_ 1
#include <ubit/uattr.hpp>
#include <ubit/ustr.hpp>
namespace ubit {
  
  /** Font of a UElem or UBox container.
   * this property specifies the font in a UElem or a UBox. It is inherited
   * by children, except if they have their own UFont, or if their UStyle
   * specifies another font.
   * As other UAttr(s), a UScale instance must be added to the attribute list
   * or child list of the container: @see: UAttr.
   */
  class UFont : public UAttr {
  public:
    UCLASS(UFont)
    
    enum Style {
      PLAIN         = 0,
      BOLD          = 1<<0,	// DON'T CHANGE VALUE!
      ITALIC        = 1<<1,	// DON'T CHANGE VALUE!
      FILL          = 1<<2,
      UNDERLINE     = 1<<3,
      OVERLINE      = 1<<4,
      STRIKETHROUGH = 1<<5
    };
    ///< bitmask values for setStyle().
    
    static UFont sans_serif, serif, monospace;
    ///< default font family modifiers.
    
    static UFont xx_small, x_small, small, medium, large, x_large, xx_large;
    ///< default font size modifiers.
    
    static UFont plain, bold, italic, fill, underline, overline, strikethrough;
    /**< font style modifiers.
     * - 'plain' means not bold, not italic, not underlined
     * -  mofifiers can be combined: example:
     *    <pre>   ubox( UFont::bold + UFont::italic + "abcd" )  </pre>
     */
    
    UFont();
    ///< create a new UFont; @see also shortcut ufont().
    
    UFont(const UFont&);
    ///< create a new UFont; @see also shortcut ufont().
    
    UFont(const UFont& font, int style, float pixel_size);
    /**< create a new UFont.
     * keeps the same characteristics as 'font' with a specific 'style' and 'point_size'
     * style' is an ORed combination of UFont::FontStyle constants such as BOLD|ITALIC
     * or -BOLD|-ITALIC (negative values deactivate these styles)
     * @see also shortcut ufont().
     */
    
    UFont(const UFontFamily&, int style, float point_size);
    /**< create a new UFont.
     * style' is an ORed combination of UFont::FontStyle constants such as BOLD|ITALIC
     * or -BOLD|-ITALIC (negative values deactivate these styles)
     * @see also shortcut ufont().
     */
    
    virtual UFont& operator=(const UFont& f) {set(f); return *this;}
    ///< copies font.
    
    virtual void set(const UFont&);
    ///< copies font.
    
    virtual bool operator==(const UFont& f) const {return equals(f);}
    virtual bool operator!=(const UFont& f) const {return !equals(f);}
    virtual bool equals(const UFont&) const;
    ///< compares fonts.
    
    const UFontFamily& getFamily() const {return *family;}
    ///< returns the font family.
    
    float getPixelSize() const {return fsize;}
    ///< returns the font size (in pixels).
    
    float getPointSize() const;
    ///< returns the font size (in points).
    
    int getStyles() const {return on_styles;}
    ///< returns the font styles.
    
    UFont& setFamily(const UFontFamily&);
    ///< changes the font family.
    
    UFont& setFamily(const UStr& font_families);
    ///< changes the font family (takes the closest one in a comma separated list).
    
    UFont& setSize(const UStr& size);
    ///< changes the font size.
    
    UFont& setPixelSize(float pixel_size);
    ///< changes the font size (in pixels).
    
    UFont& setPointSize(float point_size);
    ///< changes the font size (in points).
    
    UFont& setStyles(int styles);
    ///< changes the font styles.
    
    UFont& changeStyles(int styles, bool add);
    /**< changes the font style.
     * 'styles' is an ORed bitmask of UFont::Style values
     * 'styles' can be negative, which means 'disable these styles'
     */
    
    bool isPlain() const;
    bool isBold() const;
    bool isItalic() const;
    bool isFilled() const;
    bool isUnderlined() const;
    bool isOverlined() const;
    bool isStrikethrough() const;
    
    UFont& setBold(bool = true);
    UFont& setItalic(bool = true);
    UFont& setFilled(bool = true);
    UFont& setUnderlined(bool = true);
    UFont& setOverlined(bool = true);
    UFont& setStrikethrough(bool = true);
    
    void merge(const UFont&);
    /**< merge font characteristics.
     * sets NON default values and combines styles
     */
    
    virtual void update();
    ///< update parents' graphics.
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void realize(UDisp*);
    /**< [impl] allocates physical resources.
     * this function is automatically called when this font is used
     * for the first time. There no need to call it in client code except
     * for loading fonts in advance.
     * - note: the argument can be the UAppli (the Application Context)
     */
    
    virtual void putProp(UUpdateContext*, UElem&);
    
  private:
    friend class UDisp;
    friend class UFontDesc;
    const UFontFamily* family;
    //short size;
    float fsize;
    short on_styles, off_styles;  // activated and disactivated styles
    UFont(const UFontFamily*, int styles, float pixel_size, UConst);
  };
  
  inline UFont& ufont(const UFont& f) {return *new UFont(f);}
  ///< shortcut for *new UFont(font).
  
  
  /* ==================================================== ===== ======= */
  /** A Font Family represents a collection of fonts for all possible sizes and styles.
   * Note that actual fonts are only loaded when necessary
   */
  class UFontFamily : public UObject {
  public:
    static UFontFamily sans_serif, serif, monospace;
    ///< predefined font families that can be used in UFont(s).
    
    static UFontFamily defaults;
    /**< specifies default characteristics used when FontFamilies can't be found
     * should be changed before creating the UAppli
     */
    
    UFontFamily(const UStr& logical_name);
    ///< create a new font family.
    
    UFontFamily& operator=(const UFontFamily&);  
    
    const UStr& getName() const {return name;}
    
    void setFamily(const char* native_families);
    ///< changes font characteristics for this family (args are coma separated lists).
    
    void setStyles(const char* normal_styles, const char* italic_styles);
    ///< changes font characteristics for this family.
    
    void setWeights(const char* normal_weights, const char* bold_weights);
    ///< changes font characteristics for this family.
    
    void setCompression(const char* compressions);
    
    void setEncoding(const char* encodings);
    
    void setPreferredSizes(const char* sizes);
    
    void setFreeType(const char* regular_files, 
                     const char* bold_files,
                     const char* italic_files, 
                     const char* bold_italic_files);
    /**< specifies FreeType font files for this family.
     * each argument is a comma separated list of of font pathnames (the first 
     * available one is used).
     */
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    friend class UFontDesc;  
    friend class UHardFont;  
    friend class UDisp;  
    
    UFontFamily(const char* logical_name, UConst);  // private constr
    UFontFamily(const UFontFamily&); // forbidden
    
    void initSizes() const;
    int sizeToIndex(float fsize) const;
    
    static int family_count;
    mutable int ffindex;
    mutable bool ready;
    UStr name;
    mutable std::vector<int> fsizes;
    struct Defs {
      UStr
      families, 
      normal_style,
      italic_style,
      normal_weight,
      bold_weight,
      compression,
      encoding,  
      preferred_sizes,
      fty_regular, fty_bold, fty_italic, fty_bold_italic;  // freetype
    } defs;
  };
  
}
#endif
