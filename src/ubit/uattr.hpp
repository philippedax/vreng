/************************************************************************
 *
 *  uattr.hpp: Base class for the attributes of the Scene Graph
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

#ifndef _uattr_hpp_
#define	_uattr_hpp_ 1
#include <ubit/unumber.hpp>
namespace ubit {
  
  /** Base class for attributes.
   *
   * attributes specify the layout, the geometry, the color, the font...
   * of elements that derive from UBox (some attributes also apply
   * on UElem elements, as explained in the doc of these properties).                                              
   *
   * they can be added to the attribute list of one or several containers by using
   * UElem::addAttr(). They can also be inserted in the *beginning* of their child
   * list, before any viewable child (that is to say a child that derive from 
   * UElem or UData). Certains properties (mainly UFont and UColor) can be inserted
   * anywhere in the child list and take effect of the following children.
   *
   * attributes update their parents automatically when they are modified. All
   * parents that share the same property are thus automatically synchronized.
   *
   * attributes can have callback functions that are fired when their value is
   * changed (see an example below). Property parents can also be notified when
   * attribute values are modified by using the UOn::propChange condition (see below).
   *
   * Example:
   * <pre>
   *    class Demo {
   *    public:
   *       void colorCB(UColor* c);
   *       void boxCB(UEvent& e);
   *       ....
   *    };
   *
   *    Demo* d = new Demo();
   *
   *    UColor& color = ucolor(255, 0, 0);  // shorcut for: *new UColor(...)
   *
   *    // d->colorCB(&color) will be called when the color is changed
   *    color.onChange(ucall(d, &color, &Demo::colorCB));
   * 
   *    UBox& btn = ubutton("Click Me");    // shorcut for: *new UButton(...)
   *
   *    btn.addAttr(color);                 // adds color to the attribute list
   *
   *    // the content of UColor::blue (which is a predefined value) is copied
   *    // into the content of the color object (note that color is not a pointer
   *    // but a reference)
   *
   *    color = UColor::blue;               // fires colorCB
   * </pre>
   *
   * Alternatively, we could insert color at the beginning of the child list:
   * <pre>
   *    UBox& btn = ubutton(color + "Click Me");
   *    // btn.addAttr(color); useless: color is in the child list!
   * </pre>
   *
   * Finally, 
   * <pre>
   *    UBox& btn = ubutton(color + "Click Me" 
   *                        + UOn::propChange / ucall(d, &Demo::boxCB));
   * </pre>
   *
   * will call d->boxCB(event) when the value of any property of btn is changed.
   * In this case, color is the only property, and boxCB is fired when color
   * is modified. The UEvent& argument of boxCB() makes it possible to retrieve
   * the color property as follows:
   * 
   * <pre>
   *    void Demo::boxCB(UEvent& e) {
   *      UColor* c = null;
   *      if (e.getTarget()) c = dynamic_cast<UColor*>(e.getTarget());
   *      ....
   *   }
   * </pre>
   * Note that we could have use the same technique to retrieve the color property
   * in colorCB(), instead of giving color as an argument to this function
   *
   * @see also: the UCall doc, that explains how to use ucall() expressions
   */
  class UAttr: public UNode {
  public:
    //UCLASS("#attr", UAttr, null)  
    UCLASS(UAttr)  
    
    UAttr();
    UAttr(UConst);
    virtual ~UAttr();
    
    virtual UAttr* toAttr() {return this;}
    virtual const UAttr* toAttr() const {return this;}
     
    virtual int getDisplayType() const {return ATTRIBUTE;}    
    virtual int getNodeType() const {return ATTRIBUTE_NODE;}
    
    virtual const UStr& getNodeName() const {return getClassName();}
    ///< returns the name of this attribute (calls getName()).

    virtual UStr getNodeValue() const;
    ///< returns the value of this attribute (calls getValue()).
        
    virtual const UStr& getName() const {return getClassName();}
    ///< returns the name of this attribute.
    
    virtual bool getValue(UStr& value) const;
    /**< gets the value of this attribute.
     * return false if this attribute has no value. 
     * Stores it in argument 'value' otherwise, 
     */
    
    virtual void setValue(const UStr& value) {}
    /**< changes the value of this attribute
     * does nothing if not applicable for a given subclass.
     */
    
    virtual void initNode(UDoc*, UElem* parent) {}
    ///< called when the document is created.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual UAttr& onChange(UCall&);
    /**< adds a callback that is fired when the value of the property is modified.
     * @see an example in the UAttr doc.
     */
    
    UAttr& setConst() {UObject::setConst(); return *this;}
    
    virtual void update() {}
    ///< updates parents graphics.
    
    // - - - Impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    virtual void changed(bool update = true);
    /**< [impl] called when object's content is changed.
     * This function updates graphics (if 'update' is true) then fires 
     * parents' UOn::propChange callbacks
     */
    
    virtual void putProp(UUpdateContext*, UElem&) {}
    ///< [impl] changes corresponding value in the UUpdateContext
    
  protected:
    friend class UBox;
  };
  
  
  // ==================================================== Ubit Toolkit =========
  /** Attribute list.
   * used by UConf and by the style management system (among other things).
   */
  class UAttrList : public UAttr {
  public:
    //UCLASS("#attrlist", UAttrList, new UAttrList)
    UCLASS(UAttrList)
    
    void addAttr(UAttr&);
    void addAttr(const UStr& name, const UStr& value);
    
    UStr* getAttr(const UStr&, bool ignore_case = true);
    
    template <class AttrClass> 
    AttrClass* getAttr() const {AttrClass* a; _attributes.findClass(a); return a;}
    /**< returns the attribute that derives from AttrClass.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     */
    
    template <class AttrClass> 
    AttrClass& obtainAttr() const {AttrClass* a; _attributes.findClass(a); return a ? *a : *new AttrClass;}
    /**< returns the attribute that derives from AttrClass.
     * null is returned if there is no such attribute in the element's ATTRIBUTE list.
     */
    
    virtual void update();
    virtual void putProp(UUpdateContext*, UElem&);  
  };

  
  // ==================================================== Ubit Toolkit =========
  /** Box tool tip.
   */
  class UTip: public UAttr {
  public:
    UCLASS(UTip)
    
    UTip(const char* label = null);
    /**< create a new Tool Tip with a string label.
     * 'label' is duplicated internally and can be freed.
     * Example:
     *    ubutton( "Build" + utip("Build active target") )
     */
    
    UTip(const UArgs& arglist);
    /**< create a new Tool Tip with an arglist.
     * 'arglist' is NOT duplicated internally. The tooltip will show nothing if
     * it is destroyed.
     * Example:
     *    ubutton( "Build"
     *            + utip(UFont::bold + "Build" 
     *                   + UElem(UColor::red + "active")
     *                   + "target") )
     */
    
    virtual ~UTip();
    
    //UStr& value() {return *pvalue;}
    // returns the string value.
    
    virtual UTip& set(const char* label);
    virtual UTip& set(const UArgs& arglist);
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    //NB: removingFrom() impose un destructeur
    uptr<UNode> content;
#endif
  };
  
  inline UTip& utip(const char* label = null) {return *new UTip(label);}
  ///< shortcut function that returns *new UTip(label).
  
  inline UTip& utip(const UArgs& args) {return *new UTip(args);}
  ///< shortcut function that returns *new UTip(args).
   
  
  
  /** Window Title.
   */
  class UTitle: public UAttr {
  public:
    UCLASS(UTitle)
    
    UTitle(const char* = "");
    /**< create a new Window Title (the argument is duplicated internally).
     * example:  udialog( utitle("Debug Window") + ... )
     */
    
    UTitle(const UStr&);
    /**< create a new Window Title (the argument is duplicated internally).
    * example:  udialog( utitle("Debug Window") + ... )
    */
    
    UStr& value() {return *pstring;}
    /** returns the value.
    * - example: UTitle t; t.value() = "Main Win"; UStr s = t.value();
    * - value() can be shared (but can't be deleted!)
    */
    
    virtual void update();
    
    // - impl. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  private:
    uptr<UStr> pstring;
    //bool changed;
#endif
  };
  
  
  inline UTitle& utitle(const char*_s) {return *new UTitle(_s);}
  inline UTitle& utitle(const UStr&_s) {return *new UTitle(_s);}

}
#endif
