/************************************************************************
 *
 *  uboxgeom.hpp: attributes for controlling UBox geometry
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

#ifndef uboxgeom_hpp
#define	uboxgeom_hpp 1
#include <ubit/uattr.hpp>
#include <ubit/ulength.hpp>
#include <ubit/ugeom.hpp>
namespace ubit {
  
  /** Widget scale.
   * The UScale attribute specifies the scaling factor of widgets (UBox subclasses)
   * and their direct and indirect children in the instance graph. 
   *
   * If a child widget has also a UScale attribute, this scale spec. is multiplied
   * with the scale spec. of the parent (and so on, recursively)
   */
  class UScale : public UAttr {
  public:
    UCLASS(UScale)
    
    UScale(float value = 1.);
    ///< creates a new scale attribute; see also shortcut uscale().
    
    UScale(const UFloat&);
    ///< creates a new scale attribute; see also shortcut uscale().
    
    operator float() const {return value;}
    ///< type conversion: returns scale value.
    
    UScale& operator=(float v)         {set(v); return *this;}
    UScale& operator=(const UFloat& v) {set(v); return *this;}
    UScale& operator=(const UScale& v) {set(v.value); return *this;}
    
    bool operator==(float v) const         {return v == value;}
    bool operator==(const UFloat& v) const {return v == value;}
    bool operator==(const UScale& v) const {return value == v.value;}
    
    virtual void set(float);
    virtual void mult(float);
    virtual void div(float);
    virtual void update();
    
  private:
    float value;
    virtual void putProp(UUpdateContext*, UElem&);
  };
  
  inline UScale& uscale(float v = 1.)    {return *new UScale(v);}
  ///< shortcut that creates a new UScale.
  
  inline UScale& uscale(const UFloat& v) {return *new UScale(v);}
  ///< shortcut that creates a new UScale.
  
  
  // ==================================================== Ubit Toolkit =========  
  /** Widget position.
   * The UPos Attribute specifies the position of a widget (UBox subclasses) relatively
   * to its parent widget (note that, currently, UPos don't work with UWin widgets)
   *
   * Most widgets do not need to have a UPos attribute: their position is then 
   * calculated automatically by their parent widgets (see UHalign, UValign).
   * 
   * Depending of its x and y ULength parameters, a UPos attribute works as follows:
   * - it is ignored if either 'x' or 'y' is set to UAUTO or UIGNORE
   * - the widget is floating otherwise and 'x' and 'y' are relative to its parent origin
   *
   * 'x' and 'y' units can either be:
   * - UPX (pixel), UIN (inch), UCM (centimeter), UMM (millimeter), UPT (point), UPC (pica)
   * - UEM (current font size), UEX (x-height of a font)
   * - UPERCENT (percentage of the parent size in range [0f,100f])
   * - UPERCENT_CTR (same as UPERCENT except that x,y then correspond to the widget center)
   *
   * Exemple: 
   * - UPos(100|UPX, 25|UPERCENT) specifies a width of 100 pixels and a height equals
   *   to 25% of the parent's height
   */
  class UPos : public UAttr, public UPosSpec {
  public:
    UCLASS(UPos)

    static const ULength::Modes TOP, LEFT, RIGHT, BOTTOM;
      
    UPos() : UPosSpec(0,0) {}
    ///< creates a new position attribute with x=0 and y=0.
      
    UPos(ULength x, ULength y) : UPosSpec(x, y) {}
    ///< creates a new position attribute; see also shortcut upos(ULength x, ULength y).
           
    UPos(const UPoint& p) : UPosSpec(p.x, p.y) {}
    ///< creates a new position attribute; see also shortcut upos().
    
    UPos(const UPos& p) : UPosSpec(p.x, p.y) {}
    ///< creates a new position attribute.
    
    //UPos(const UStr& xSpec, const UStr& ySpec);
    //creates a new position attribute.

    virtual ~UPos() {destructs();}
      
    const ULength& getX() const {return x;}
    const ULength& getY() const {return y;}

    UPos& setX(const ULength& _x) {return set(_x, y);}
    UPos& setY(const ULength& _y) {return set(x, _y);}
    
    virtual UPos& set(const ULength& x, const ULength& y);
    UPos& operator=(const UPos& p)   {return set(p.x, p.y);}
    UPos& operator=(const UPoint& p) {return set(p.x, p.y);}

    virtual bool equals(const ULength& x, const ULength& y) const;
    bool operator==(const UPos& p) const {return equals(p.x, p.y);}
    bool operator==(const UPoint& p) const {return equals(p.x, p.y);}

    bool isFloating() const;
    /**< returns true if this UPos makes the widget to be floating.
     * this UPos is ignored, and thus, the widget nt floating, if either 'x' or 'y'
     * has been set to UAUTO or UIGNORE
     */
    
    virtual bool is3Dpos() const {return false;}
    ///< returns true if this is a U3Dpos.

    virtual void update();

   private:
    friend class UView;
    friend class UBox;
    friend class UWin;
    virtual void putProp(UUpdateContext*, UElem&);
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    //NB: removingFrom() requires UPos to have a destructor.    
  };
    
  inline UPos& upos(const ULength& x, const ULength& y) {return *new UPos(x, y);}
  ///< shortcut that creates a new UPos.
  
  inline UPos& upos(const UPoint& p) {return *new UPos(p);}
  ///< shortcut that creates a new UPos.

  
  // ==================================================== Ubit Toolkit =========
  /** Position Control: interactive control of the position of a widget.
   * when dragged interactivelly, a handle widget (typically a button) having  
   * this attribute changes the UPos of another widget (typically a palette) 
   * that contains this handle.
   */
  class UPosControl : public UAttr {
  public:
    UCLASS(UPosControl)
    
    UPosControl(UPos* = null);
    ~UPosControl();
    
    UPosControl& setModel(UPos*);
    UPosControl& changeCursor(bool state);
    UPosControl& freezeX(bool state);
    UPosControl& freezeY(bool state);
    
    // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  protected:
    static const int MARGIN = 5;
    UPoint box_pos0, pt_in_pane0;
    bool change_cursor, freeze_x, freeze_y;
    UView* moved_view;
    uptr<UCall> callbacks;
    uptr<UPos> posAttr;
    
    virtual void putProp(UUpdateContext*, UElem&) {}
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    // NOTE that removingFrom() requires a destructor to be defined.
    
    virtual void mouseCB(UMouseEvent&);
    virtual void pressCB(UMouseEvent&);
    virtual void releaseCB(UMouseEvent&);
    virtual void dragCB(UMouseEvent&);
#endif
  };
  
  /** Makes it possible to change the UPos of a Magic Lens interactively.
   * PBM: aucun effet si on modifie pos directement par programme !!
   */
  class UMagicLensControl : public UPosControl {
  public:
    UCLASS(UMagicLensControl)
    UMagicLensControl(UPos* = null, UScrollpane* = null);
    UMagicLensControl& setModel(UPos*, UScrollpane*);
  private:
    uptr<UScrollpane> pane;
    virtual void dragCB(UMouseEvent&);
  };
  
  
  // ==================================================== Ubit Toolkit =========
  /** Widget Size.
   * The USize attribute specifies the size of a widget (a node deriving from UBox).
   *
   * USize specifies the preferred size of the widget and its behavior. The actual
   * widget size may depend on other factors such as its scale (see UScale) and 
   * layout constraints (see uhflex(), uvflex())
   *
   * The type of the width and the height of the USize is ULength, which makes it
   * to specify a unit (length are in pixels if no unit is specified)
   *
   * A length of -1 means that the size of the initial content of the widget
   * should be used to compute the widget size (in this direction)
   *
   * UNRESIZABLE prevents uhflex() and uvflex() attributes to resize the widget
   *
   * Note: A USize object can be shared by several widgets. 
   */
  class USize : public UAttr, public USizeSpec {
  public:
    UCLASS(USize)
    
    static const ULength INITIAL;
    static const ULength::Modes UNRESIZABLE;
    
    USize() : USizeSpec(UIGNORE,UIGNORE) {}
    ///< creates a new size attribute with width=UAUTO and height=UAUTO.
    
    USize(ULength width, ULength height = UIGNORE) : USizeSpec(width, height) {}
    ///< creates a new size attribute, @see also the usize() shortcut.
    
    USize(const UDimension& d) : USizeSpec(d.width, d.height) {}
    ///< creates a new size attribute.

    USize(const USize& s) : USizeSpec(s) {}
    ///< creates a new size attribute.
    
    //UPos(const UStr& widthSpec, const UStr& heightSpec);
    //creates a new position attribute.

    const ULength& getWidth()  const {return width;}
    const ULength& getHeight() const {return height;}
    
    USize& setWidth(const ULength& _w)  {return set(_w, height);}
    USize& setHeight(const ULength& _h) {return set(width, _h);}
    
    virtual USize& set(const ULength& width, const ULength& height);
    USize& operator=(const USize& s) {return set(s.width, s.height);}
    USize& operator=(const UDimension& d) {return set(d.width, d.height);}
    
    virtual bool equals(const ULength& w, const ULength& h) const;
    bool operator==(const USize& s) const {return equals(s.width, s.height);}
    bool operator==(const UDimension& d) const {return equals(d.width, d.height);}

    virtual void update();
    
  private:
    virtual void putProp(UUpdateContext*, UElem&);  
    friend class UViewLayoutImpl;
  };
  
  inline USize& usize() {return *new USize();}
  ///< shortcut function that returns *new USize().

  inline USize& usize(const ULength& w, const ULength& h =UIGNORE) {return *new USize(w,h);}
  ///< shortcut function that returns *new USize(w, h).
  
  inline USize& usize(const UDimension& d) {return *new USize(d);}
  ///< shortcut function that returns *new USize(d).
  
  
  // ==================================================== Ubit Toolkit =========
  /** Size Control: interactive control of the size of a widget.
   * when dragged interactivelly, a handle widget (typically a button) having  
   * this attribute changes the USize of another widget (typically a palette) 
   * that contains this handle.
   */
  class USizeControl : public UAttr {
  public:
    UCLASS(USizeControl)
    
    USizeControl(USize* = null);    
    ~USizeControl();
    
    USizeControl& setModel(USize*);
    USizeControl& freezeWidth(bool state);
    USizeControl& freezeHeight(bool state);
    
    // - - - impl - - - - -- - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void putProp(UUpdateContext*, UElem&) {}
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    // NOTE that removingFrom() requires a destructor to be defined.
  protected:
    float w0, h0;
    float xev0, yev0;
    bool freeze_w, freeze_h;
    uptr<UCall> callbacks;
    uptr<USize> psize;
    virtual void mouseCB(UMouseEvent&);
    virtual void pressCB(UMouseEvent&);
    virtual void releaseCB(UMouseEvent&);
    virtual void dragCB(UMouseEvent&);
#endif
  };
  
  
  // ==================================================== Ubit Toolkit =========
  /** Widget padding.
   */
  class UPadding : public UAttr {
  public:
    UCLASS(UPadding)
    
    UPadding() : val(UIGNORE,UIGNORE) {}
    UPadding(ULength width, ULength height) : val(width, height) {}
        
    UPadding& operator=(const UPadding&);
    
    UPadding& setWidth(ULength);
    ///< sets top and bottom padding.
    
    UPadding& setHeight(ULength);
    ///< sets left and right padding.

    UPadding& setTop(ULength);
    UPadding& setRight(ULength);
    UPadding& setBottom(ULength);
    UPadding& setLeft(ULength);
    
    ULength getTop()   const {return val.top;}
    ULength getRight() const {return val.right;}
    ULength getBottom()const {return val.bottom;}
    ULength getLeft()  const {return val.left;}
    
    virtual void update();
  private:
    UPaddingSpec val;
    virtual void putProp(UUpdateContext*, UElem&);
  };
  
  
  inline UPadding& upadding() {return *new UPadding();}
  ///< shortcut function that returns *new UPadding().
  
  inline UPadding& upadding(ULength horiz, ULength vert) {return *new UPadding(horiz,vert);}
  ///< shortcut function that returns *new UPadding(horiz,vert).

  
  // ==================================================== Ubit Toolkit =========
  /** Widget Orientation.
   * specifies the orientation of box(es) that contain this brick.
   * Possible values: UOrient::vertical or UOrient::horizontal
   */
  class UOrient : public UAttr {
  public:
    UCLASS(UOrient)
    
    static UOrient horizontal, vertical;
    
    UOrient();
    ///< creates a new UOrient object; @see also shortcut uorient().
    
    UOrient(const UOrient&);
    ///< creates a new UOrient object; @see also shortcut uorient().
    
    UOrient& operator=(const UOrient& v);
    bool operator==(const UOrient& v) const {return value == v.value;}
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    enum {HORIZONTAL=0, VERTICAL=1, INHERIT=2}; //dont change values
    void addingTo(UChild&, UElem& parent);
    void removingFrom(UChild&, UElem& parent);
  private:
    char value;
    UOrient(char value, UConst);
#endif
  };
  
  inline UOrient& uorient(const UOrient& val) {return *new UOrient(val);}
  ///< shortcut function that creates a new UOrient object.
  
  
  // ==================================================== Ubit Toolkit =========
  /** Box horizontal layout.
   * specify the horizontal layout of the children that *follow* this brick.
   * Possible values: UHalign::left, ::right, ::center, ::flex.
   * Shortcut functions uleft(), uright(), uhcenter(), uhflex() can be used
   * to improve source code readability.
   *
   * Example:
   * <pre>
   *    uhbox( uleft() + ch1 + ch2 + uvflex() + ch3 + ch4 + uright() + ch5 )
   * </pre>
   * uleft() specifies the layout of ch1 and ch2, uhflex() the layout of
   * ch3 and ch4 and uright() the layout of ch5.
   */
  class UHalign : public UAttr {
  public:
    UCLASS(UHalign)

    static UHalign left, right, flex, center;
    
    UHalign();
    ///< creates a new UHalign; @see also shortcuts: uhalign(), uleft(), uright(), uhcenter(), uhflex(). 
    
    UHalign(const UHalign&);
    ///< creates a new UHalign; @see also shortcuts: uhalign(), uleft(), uright(), uhcenter(), uhflex(). 
    
    UHalign& operator=(const UHalign&);
    bool operator==(const UHalign& v) const {return value == v.value;}
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT, LEFT, RIGHT, FLEX, CENTER};
  private:
    char value;
    UHalign(char value, UConst);
  };
  
  
  inline UHalign& uhalign(const UHalign& val) {return *new UHalign(val);}
  ///< shortcut function that creates a new UHalign object.
  
  inline UHalign& uleft() {return UHalign::left;}
  /**< child widgets following uleft() are laid out sequentially from the left side of their parent.
   * uleft have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see UHalign.
   */
  
  inline UHalign& uright() {return UHalign::right;}
  /**< child widgets following uright() are laid out sequentially so that the last child reaches the right side of their parent.
   * uright have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see UHalign.
   */
  
  inline UHalign& uhcenter() {return UHalign::center;}
  /**< child widgets following uhcenter() are horizontally centered in their parent.
   * uhcenter has no effect on children width (in contrast with uhflex()).
   * uhcenter has the same effect as uleft if uhflex() appears in the same child list
   * @see UHalign.
   */
  
  inline UHalign& uhflex() {return UHalign::flex;}
  /**< child widgets following uhflex() have a "flexible" horizontal layout.
   * these children will occupy the remaining space and will be resized
   * (in the horizontal direction) when their parents are resized.
   * These rules dont apply for children that are NOT resizable.
   * @see UHalign.
   */
  
  
  // ==================================================== Ubit Toolkit =========
  /** Box vertical layout.
   * specify the vertical layout of the children that follow this brick.
   * Possible values: UHalign::top, ::bottom, ::center, ::flex.
   * Shortcut functions utop(), ubottom(), uvcenter(), uvflex() can be used
   * to improve readability (they return the corresponding value).
   *
   * Behaviors: same behaviors as UHalign (but in the vertical direction).
   */
  class UValign : public UAttr {
  public:
    UCLASS(UValign)

    static UValign top, bottom, flex, center;
    
    UValign();
    ///< creates a new UValign; @see also shortcuts: uvalign(), utop(), ubottom(), uvcenter(), uvflex(). 
    
    UValign(const UValign&);
    ///< creates a new UValign; @see also shortcuts: uvalign(), utop(), ubottom(), uvcenter(), uvflex(). 
    
    UValign& operator=(const UValign& v);
    bool operator==(const UValign& v) const {return value == v.value;}
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT, TOP, BOTTOM, FLEX, CENTER};
  private:
    char value;
    UValign(char value, UConst);
  };
  
  inline UValign& uvalign(const UValign& val) {return *new UValign(val);}
  ///< shortcut function that creates a new UValign object.
  
  inline UValign& utop() {return UValign::top;}
  /**< child widgets following utop() are laid out sequentially from the top of their parent.
   * uleft have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see UValign.
   */
  
  inline UValign& ubottom() {return UValign::bottom;}
  /**< child widgets following ubottom() are laid out sequentially so that the last child reaches the bottom of their parent.
   * uright have no effect on children width (in contrast with uhflex()).
   * uleft and uright have the same effect if uhflex() appears in the same child list
   * @see UValign.
   */
  
  inline UValign& uvcenter() {return UValign::center;}
  /**< child widgets following uvcenter() are vertically centered in their parent.
   * uhcenter has no effect on children width (in contrast with uhflex()).
   * uhcenter has the same effect as uleft if uhflex() appears in the same child list
   * @see UValign.
   */
  
  inline UValign& uvflex() {return UValign::flex;}
  /**< child widgets following uvflex() have a "flexible" vertical layout.
   * these children will occupy the remaining space and will be resized
   * (in the horizontal direction) when their parents are resized.
   * These rules dont apply for children that are NOT resizable.
   * @see UValign.
   */
  
  
  // ==================================================== Ubit Toolkit =========
  /** Box vertical spacing.
   */
  class UVspacing : public UAttr {
  public:
    UCLASS(UVspacing)

    static UVspacing none;
    
    UVspacing(float = 0);
    
    operator float() const {return value;}
    ///< type conversion: returns value.
    
    UVspacing& operator=(float);
    UVspacing& operator=(const UVspacing&);
    
    bool operator==(float v) const {return value == v;}  
    bool operator==(const UVspacing& v) const {return value == v.value;}
    
    virtual void update();
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT = -1};
    virtual void putProp(UUpdateContext*, UElem&);
  private:
    float value;
  };
  
  inline UVspacing& uvspacing(float val = 0) {return *new UVspacing(val);}
  ///< shortcut function that returns a new UVspacing.
  
  // ==================================================== Ubit Toolkit =========
  /** Box horizontal spacing.
  */
  class UHspacing : public UAttr {
  public:
    UCLASS(UHspacing)
    
    static UHspacing none;
    
    UHspacing(float = 0);
    
    operator float() const {return value;}
    ///< type conversion: returns value.
    
    UHspacing& operator=(float);
    UHspacing& operator=(const UHspacing&);
    
    bool operator==(float v) const {return value == v;}  
    bool operator==(const UHspacing& v) const {return value == v.value;}
    
    virtual void update();
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    enum {INHERIT = -1};
    virtual void putProp(UUpdateContext*, UElem&);
  private:
    float value;
  };
  
  inline UHspacing& uhspacing(float val) {return *new UHspacing(val);}
  ///< shortcut function that returns a new UHspacing.
  
}  
#endif
