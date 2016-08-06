/************************************************************************
 *
 *  ulistbox.hpp
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

#ifndef _ulistbox_hpp_
#define	_ulistbox_hpp_ 1
#include <ubit/uinteractors.hpp>
#include <ubit/uchoice.hpp>
namespace ubit {
  
  /** Radiobox container: makes buttons (and items, checkboxes, etc) contained in 
   * this box exclusive.
   * A Radiobox is horizontal by default, add attribute UOrient::vertical to make
   * it vertical. 
   */
  class URadiobox: public UBox {
  public:
    UCLASS(URadiobox)
    
    URadiobox(const UArgs& args = UArgs::none);
    ///< creates a new Radiobox widget (see also shortcut function uradiobox()).
    
    virtual UBox* getSelectedItem() const;
    ///< returns the selected item; null if there is no selected item.
    
    virtual int getSelectedIndex() const;
    ///< returns the index of the selected item; -1 if there is no selected item.
    
    virtual URadiobox& setSelectedItem(UBox& item);
    ///< selects this item.
    
    virtual URadiobox& setSelectedItem(UBox* item);
    ///< selects this item or clear the selection if this item is null.

    virtual URadiobox& setSelectedIndex(int n);
    ///< selects the Nth item; selects the last selectable item if n = -1.
    
    URadiobox& select(UBox& item) {return setSelectedItem(item);}
    ///< selects this item (synonym for setSelectedItem()).

    URadiobox& select(int n) {return setSelectedIndex(n);}
    ///< selects the Nth item (synonym for setSelectedIndex()).    
     
    UChoice& choice() {return *pchoice;}
    ///< returns the internal UChoice object that controls the selection.
    
    const UChoice& choice() const {return *pchoice;}
    ///< returns the internal UChoice object that controls the selection.
    
  protected:
    uptr<UChoice> pchoice;
  };
  
  inline URadiobox& uradiobox(UArgs args = UArgs::none) {return *new URadiobox(args);}
  ///< shortcut function that returns *new URadiobox(args).
  
  
  /* ==================================================== ===== ======= */
  /** List widget: enforces a list layout and makes buttons (and items, checkboxes, etc)
   * contained in this box exclusive
   *
   * A Listbox is vertical by default, add attribute UOrient::horizontal to make 
   * it horizontal. Although, it can contain any widget, UItem widgets should be
   * included in a Listbox to obtain the usual look of list widgets. Note that there
   * is no restriction on what included widgets may contain (ie. text, images, 
   * nested widgets, etc.)
   *
   * Strings and non selectable widgets (such as ULabel) can be added to a Listbox.
   * They will be displayed but won't be selected (and won't be taken into account
   * by getSelectedIndex(), setSelectedIndex()).
   *
   * The background is white by default, the font and the foreground color are 
   * inherited from parents.
   *
   * A Listbox can have UOn::change and UOn::action callbacks. Both are fired when
   * an item is selected, except that UOn::change is not fired if the same item was
   * already selected, while UOn::action is always fired. UOn::action is the default,
   * which means that is can be omitted in ucall() expressions.
   *
   * UElemEvent events are produced in both cases. Their getSource() method returns
   * the list object, while getTarget() returns the selected item.
   *
   * <pre>
   * class Example {
   *    void changeCB(UEvent&);
   * };
   *
   * Example* ex = new Example;
   *
   * UListbox& list = ulistbox
   *    (UOn::change / ucall(ex, &GUI::changeCB)
   *    + ulabel("Modes")     // this object won't be selected as it is a Label
   *    + uitem(select_image + "Select") 
   *    + uitem(draw_image + "Draw") 
   *    + uitem(destroy_image + "Destroy") 
   *   );
   *
   * void GUI::changeCB(UEvent& e) {
   *    // retreives the selected item
   *    UBox* item = e.getTarget() ? e.getTarget()->toBox() : null;
   *    ...etc...
   * }
   * </pre>
   */
  class UListbox: public URadiobox {
  public:
    UCLASS(UListbox)
    
    UListbox(const UArgs& args = UArgs::none);
    ///< creates a new List widget (see also shortcut function ulistbox()).
    
    static UStyle* createStyle();
    
    bool isTransparent() const {return transp_style;}
    UListbox& setTransparent(bool s = true) {transp_style = s; return *this;}
    
    
    UListbox& setSelectedItem(UBox& item);
    ///< selects this item.
    
    UListbox& setSelectedIndex(int n);
    ///< selects the Nth item; selects the last selectable item if n = -1.
         
    UListbox& select(UBox& item) {return setSelectedItem(item);}
    ///< selects this item (synonym for setSelectedItem()).
    
    UListbox& select(int n) {return setSelectedIndex(n);}
    ///< selects the Nth item (synonym for setSelectedIndex()).

        
    virtual UListbox& addItem(const UStr& string);
    virtual UListbox& addItem(UStr& string, bool duplicate);
    /**< adds an item to the listbox.
     * the string argument is duplicated except if 'duplicate' if false
     * (in which case it is shared and directly added to the scene graph)
     *
     * Objects can also be be added/removed to the UListbox by using
     * the add()/remove() methods that are inherited from UElem
     *
     * addItem() is equivalent to: 
     * - add(uitem(ustr(s)))   if duplicate is true (or not provided)
     * - add(uitem(s))         if duplicate is false
     */
    
    virtual UListbox& addItems(const UStr& strings, const UStr& separ = ",");
    virtual UListbox& addItems(const UArgs& prefix, const UStr& strings, const UStr& separ = ",");
    /**< adds a list of items to the listbox.
     * - parses a string containing the item names separated by a character
     *   in 'separ' (the , char by default) and adds each item to the list
     *   by calling addItem().
     * - in addition addItems(prefix...) adds this prefix (typically a small
     *   image or a pixmap, see UIma and UPix) before each item
     * - see addItem() for more details.
     */
    
    virtual UListbox& addItems(const char* items[]);
    virtual UListbox& addItems(const std::vector<UStr*>& items, bool duplicate = true);
    virtual UListbox& addItems(const UArgs& prefix, const char* items[]);
    virtual UListbox& addItems(const UArgs& prefix, const std::vector<UStr*>& items, 
                               bool duplicate = true);
    /**< adds a list of items to the listbox.
     * - performs addItem() for each string in the 'items' vector
     * - in addition addItems(prefix...) adds this prefix (typically a small
     *   image or a pixmap, see UIma and UPix) before each item
     */
    
  protected:
    bool transp_style;
  };
  
  inline UListbox& ulistbox(UArgs args = UArgs::none) {return *new UListbox(args);}
  ///< shortcut function that returns *new UListbox(args).
  
  
  /* ==================================================== ===== ======= */
  /** Combo box gadget.
   *
   *  Geometry:
   *  - Same geometry as UTextfield: keeps its initial size, which is calculated
   *    to make the first item of the list entirely visible in the textfield.
   *
   *  List and Selection:
   * - list() returns the UListbox that was passed as an argument to the constructor
   * - choice() returns the UChoice object that manages the selection in list()
   *
   *  Callbacks
   *  - UOn::action (default callback) is fired when an item is selected or re-selected
   *    and when the RETURN key is pressed in the textfield
   *  - UOn::change is fired when an item is selected (if it was not already selected)
   *  - UOn::strChange is fired when the text is changed in the textfield
   *
   * Exemple
   * <pre>
   *    XXX* obj = new XXX();
   *    UListbox& list = ulistbox( ... );   // see UList
   *    UCombobox& combo = ucombobox(list, 
   *                                 UOn::action / ucall(obj, &XXX::actionCB)
   *                                 + UOn::change / ucall(obj, &XXX::changeCB));
   * </pre>
   */
  class UCombobox : public UTextfield  {
  public:
    UCLASS(UCombobox)
    
    UCombobox();
    
    UCombobox(UListbox&, const UArgs& = UArgs::none);
    /* creates a new Combo Box widget (see also shortcut function ucombobox()).
     * arguments:
     * - 1st arg: the UListbox that is displayed in the menu
     * - 2nd arg: a standard arglist (callback specs should be added here)
     */

    bool isTextMode() const {return text_only;};
    UCombobox& setTextMode(bool = true);
    ///< the textfield only contains the textual part of the selected item in text mode.
    
    UListbox& list() {return *plist;}
    class UChoice& choice();
    UElem& entry() {return *pentry;}
    UMenu&  menu()  {return *pmenu;}
    ///< gives access to combobox subcomponents.
    
    static UStyle* createStyle();
    
  protected:
    virtual void changedCB(UEvent&);  //UElemEvent
    virtual void actionCB(UEvent&);   //UElemEvent
    virtual void openMenuCB(UInputEvent&);
    virtual void syncText();
    
  private:
    uptr<UListbox> plist;
    uptr<UElem> pentry;
    uptr<class UMenu> pmenu;
    bool text_only;
    
    //UCombobox(const UArgs& = UArgs::none);
    void constructs(const UArgs&);
  };
  
  
  inline UCombobox& ucombobox(UListbox& l, UArgs a = UArgs::none) {return *new UCombobox(l,a);}
  ///< shortcut function that returns *new UCombobox(l,a).
  
} 
#endif

