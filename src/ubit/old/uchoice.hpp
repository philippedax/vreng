/************************************************************************
 *
 *  uchoice.hpp: item chooser in a list
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

#ifndef _uchoice_hpp_
#define	_uchoice_hpp_ 1
#include <ubit/uattr.hpp>
namespace ubit {
  
  /** Makes the children of a widget (exclusively) selectable.
   *
   * This object makes the CHILDREN of a widget selectable. It must be added to
   * the chilsd or attribute list of this widget. Selection is exclusive. 
   * No child is initially selected. 
   * UListbox, UTreebox, UCombobox use an internal UChoice
   * @see also: URadioSelect.
   */
  class UChoice : public UAttr {
  public:
    UCLASS(UChoice)
    
    UChoice();  
    ///< create a new UChoice object; @see also shortcut uchoice().
    
    virtual ~UChoice();
    
    virtual void clearSelection();
    ///< deselects the selected object.
    
    virtual UBox* getSelectedItem() const;
    ///< returns the current selection; returns null if there is no selection.
    
    virtual UBox* setSelectedItem(UBox&);
    ///< selects and returns this item if it is in the list, returns null otherwise.
    
    virtual UBox* setSelectedItem(UBox*);
    ///< selects this item or clear the selection if this item is null.

    virtual int getSelectedIndex() const;
    /**< returns the index of the selection; returns -1 if there is no selection.
     * NOTE: the returned value is the index among the *selectable* objects of
     * the list. A list can contain non-selectable objects, which are ignored 
     * when counting indexes.
     */
    
    virtual UBox* setSelectedIndex(int N);
    /**< selects the Nth *selectable* object in the list.
     * the last selectable object is selected if N = -1. The selected object is
     * returned (if any, null is returned otherwise).
     * NOTE: N is the index of the Nth *selectable* object. A list can contain 
     * non-selectable objects, which are ignored when counting indexes.
     */
    
    virtual UBox* getSelectableItem(int N) const;
    /**< returns the Nth *selectable* object.
     * the last selectable object is returned if N = -1.
     * NOTE: A list can contain non-selectable objects, which are ignored when
     * counting indexes. Use getChild() to have access to all objects.
     */
    
    virtual UBox* getSelectableItem(const UStr& s, bool ignore_case = true) const;
    /**< returns the 1st *selectable* object which contains a string equal to 's'.
     * this function compares the content of the strings (not their addresses).
     * NOTE: A list can contain non-selectable objects, which are ignored by
     * this function. Use getChild() to have access to all objects.
     */
    
    struct IsSelectable {
      virtual bool operator()(const UBox*) const;
      virtual ~IsSelectable() {}
    };
    
    virtual void setSelectionRule(IsSelectable&);
    /**< define which objects can be selected in the list.
     * by default, ARMable objects that derive from UBox can be selected, such as
     * UItem, UButton ... or any box that has been made armable by adding its 
     * setArmable() method.
     */
    
    virtual void update();
    
    // - - - impl - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void putProp(UUpdateContext*, UElem&);
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    // NB: removingFrom() requires a destructor to be defined
    
    /*
    enum {
      NO_SELECTION, SINGLE_SELECTION, 
      SINGLE_INTERVAL_SELECTION, MULTIPLE_INTERVAL_SELECTION
    };
    
    virtual int getSelectionMode() const {return sel_mode;}
    virtual void setSelectionMode(int m) {sel_mode = v;}
     **< gets/sets the current selection mode.
     * no effect with the current version: always in SINGLE_SELECTION mode.
    
    enum {SELECT_ON_DISARM = 1<<0, SELECT_ON_ARM = 1<<1,  SELECT_ON_DRAG = 1<<2};
    
    virtual int getSelectionStyle() const {return sel_style;}
    
     virtual void setSelectionStyle(int);{sel_style = v;}
     **< gets/sets the current selection style.
     * the selection style is an ORed combination of 
     * SELECT_ON_DISARM, SELECT_ON_ARM, SELECT_ON_DRAG
     */
  protected:
    uptr<UCall>  callbacks;
    uptr<UBox>   container;
    uptr<UElem> last_browsing_group;
    uptr<UElem> sel_items;
    IsSelectable*is_selectable;
    //short sel_mode, sel_style;
    virtual void mouseCB(UInputEvent&);
    virtual void actionCB(UInputEvent*);
    virtual void changeCB(UInputEvent*);
    virtual void changed(bool update = true);
    virtual UBox* setSelectedItemImpl(UBox* item, UInputEvent*);
    virtual UBox* getSelectedItemImpl() const;
#endif
  };
  
  inline UChoice& uchoice() {return *new UChoice;}
  ///< shortcut function that returns *new UChoice().
  
  /* ==================================================== [Elc] ======= */
  /** Makes widgets (exclusively) selectable.
   *
   * This object makes widgets (such as: UCheckbox, URadiobutton, UButton) selectable.
   * The same URadioSelect must be added to the child or attribute list of all widgets 
   * that must become exclusively selectable. No widget is initially selected.
   *  <pre>
   *    URadioSelect& sel = *new URadioSelect();
   *    ubox( ucheckbox(sel+"One") + ucheckbox(sel+"Two") + ubutton(sel+"Three") ) 
   *    sel.setSelectedItem(0);
   *  </pre>
   * @see also: UChoice.
   */
  class URadioSelect : public UAttr {
  public:
    UCLASS(URadioSelect)
    
    URadioSelect();
    ///< create a new URadioSelect object; @see also shortcut uradioselect().
    
    URadioSelect(UInt& index);
    ///< create a new URadioSelect object; @see also shortcut uradioselect().
    
    virtual ~URadioSelect();
    
    virtual void setCanUnselectMode(bool);
    virtual bool isCanUnselectMode() const;
    ///< clicking on a selected item will unselect it if true (default is false).
    
    virtual void clearSelection();
    ///< deselects the selected object (if any).
    
    virtual UElem* getSelectedItem() const;
    ///< returns the selected object; returns null if there is no selected object.
    
    virtual void setSelectedItem(UElem&);
    ///< selects this object.
    
    UInt& index() const {return *pindex;}
    virtual int getSelectedIndex() const;
    ///< returns the index of the selected object; returns -1 if there is no selected object.
    
    virtual void setSelectedIndex(int n);
    virtual void setSelectedIndex(const UInt& n);
    ///< selects the Nth  object; selects the last selectable object if n = -1.
    
    virtual void update();
    
    // - - impl  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
    virtual void changed(bool update = true);
    virtual void changed(UElem* target);
    /**< called when the selection is changed.
     * This function:
     * - updates grahics (if arg is true)
     * - first fires object's UOn::change callbacks
     * - then fires parents' UOn::select callbacks
     */
    
    virtual void putProp(UUpdateContext*, UElem&);
    virtual void addingTo(UChild&, UElem& parent);
    virtual void removingFrom(UChild&, UElem& parent);
    // NB: removingFrom() requires a destructor to be defined
    
  protected:
    bool can_unselect_mode;
    uptr<UInt> pindex;
    uptr<UCall> pselect_callback;
    void itemChanged(UInputEvent&);
    void setIndexImpl();
    void _selectItem(UElem*);
#endif
  };
  
  inline URadioSelect& uradioSelect() {return *new URadioSelect();}
  ///< shortcut function that return *new URadioSelect();
  
}
#endif
