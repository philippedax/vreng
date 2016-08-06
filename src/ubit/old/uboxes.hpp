/************************************************************************
 *
 *  uboxes.hpp : misc containers.
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

#ifndef _uboxes_hpp_
#define	_uboxes_hpp_ 1
#include <ubit/ubox.hpp>
namespace ubit {
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** UFlowBox = Box with a Flow Layout (similar to an HTML page).
   *  Base class for creating text areas or hypermedia gadgets.
   *
   *  Geometry: the width of a UFlowbox object does do not change when its content
   *  is modified. This behavior differs from those of most other boxes.
   *  It is generally well suited for displaying/editing text (UTexfield and 
   *  UTextarea enforce this behavior)
   *
   *  Implementation note: a UFlowbox is an UBox with UFlowview renderer.
   */
  class UFlowbox: public UBox {
  public:
    UCLASS(UFlowbox)

    UFlowbox(UArgs args = UArgs::none);
    /**< create a new flowbox; @see also shortcut: uflowbox().
     * a flowbox displays strings and other objects as a continous 2D flow
     */
    
    static UStyle* createStyle();
  };
  
  inline UFlowbox& uflowbox(UArgs args = UArgs::none) {return *new UFlowbox(args);}
  ///< shortcut that creates a new UFlowBox.
  
   
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** UBar = horizontal toolbar or status bar.
   *  This class is similar to UHbox but with a specific ("toolbar like") decoration.
   *  Buttons included in this object are displayed in a specific
   *  way (their border does not appear, etc.)
   *
   *  See class UBox for details.
   *  See also: class UMenubar for creating menu bars
   */
  class UBar: public UBox {
  public:
    UCLASS(UBar)

    UBar(UArgs args = UArgs::none): UBox(args) {}
    ///< creates a new horizontal bar; @see also shortcut: ubar().
    
    static UStyle* createStyle();
  };
  
  inline UBar& ubar(UArgs args = UArgs::none) {return *new UBar(args);}
  ///< shortcut function that creates a new UBar.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Status bar.
   *  This class is similar to UHbox but with a specific ("statusbar like") 
   *  decoration.
   *  Buttons included in this object are also displayed in a specific
   *  way (their border won't appear, etc.)
   *
   *  See class UBox for details.
   */
  class UStatusbar: public UBox {
  public:
    UCLASS(UStatusbar)

    UStatusbar(UArgs args = UArgs::none): UBox(args) {}
    ///< create a new status bar; @see also shortcut: ustatusbar().
    
    static UStyle* createStyle();
  };
  
  inline UStatusbar& ustatusbar(UArgs args = UArgs::none) {return *new UStatusbar(args);}
  ///< shortcut that creates a new UStatusbar.
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Card box (or Tabbed Pane).
   */
  class UCardbox : public UBox {
  public:
    UCLASS(UCardbox)

    UCardbox(UArgs args = UArgs::none);
    ///< creates a new card box; @see also shortcut: ucardbox().
    
    virtual ~UCardbox();
    
    static  UStyle* createStyle();
    
    virtual UCardbox& addCard(UBox& card);
    /**< adds a superimposed card (without a tab).
     * @see also: addTab()
     */
    
    virtual UCardbox& addTab(UArgs tab_content, UBox& card);
    /**< adds a superimposed card with an associated tab.
     * tab_content can be a string, an icon and a string, or any other valid +arglist.
     * @see also: addCard()
     */
    
    virtual UBox* getCard(int index) const;
    ///< returns the card at this index.
    
    virtual int getCardIndex(UBox& card) const;
    ///< returns the index of this card (-1 if not found).
    
    virtual UBox* getSelectedCard() const;
    ///< returns the card that is currently selected (null if none).
    
    virtual int getSelectedIndex() const;
    ///< returns the index of the card that is currently selected (-1 if none).
    
    virtual void setSelectedCard(UBox& card);
    ///< selects this card.
    
    virtual void setSelectedIndex(int index);
    ///< selects the card at this index (the last card if n = -1).
    
    UChoice& choice();
    ///< returns the UChoice object that controls the selection of cards and tabs.
    
    UListbox& tablist() {return *ptabs;}
    ///< returns the tab list.
    
#ifndef NO_DOC
  protected:
    uptr<UListbox> ptabs;
    virtual void setSelectedImpl();
#endif
  };
  
  inline UCardbox& ucardbox(UArgs args = UArgs::none) {return *new UCardbox(args);}
  ///< shortcut that creates a new UCardbox.
  
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** document box gadget.
   *¨ a box that has a titlebar and a zoomable+scrollable working area.
   *  when a docbox is iconfied the working area is hidden but the titlebar
   * remains visible.
   */
  class UDocbox : public UBox {
  public:
    UCLASS(UDocbox)

    UDocbox(UArgs args = UArgs::none);
    virtual ~UDocbox();
    
    virtual UBox& titlebar()           {return *ptitlebar;}
    virtual UScrollpane& scrollpane()  {return *pspane;}
    virtual UBox& content()            {return *pcontent;}
    virtual UScale& scale()            {return *pscale;}
    virtual void iconify(bool);
    virtual bool isIconified() const;
    virtual void zoom(float value = 1.);
    virtual void zoomIn();
    virtual void zoomOut();
    virtual void setZoomQuantum(float);
    
#ifndef NO_DOC
  protected:
    uptr<UBox> ptitlebar, pcontent;
    uptr<UScrollpane> pspane;
    uptr<UScale> pscale;
    float zoom_quantum;
#endif
  };
  
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  /** Alertbox gadget
   */
  class UAlertbox : public UBox {
  public:
    UCLASS(UAlertbox)

    UAlertbox(UArgs args = UArgs::none);
    
    static UStyle* createStyle();
    
    // NB: show doit etre redefini, sinon show("abcd") est pris comme show(bool)
    // a cause des conversions implicites (stupides) du C++
    virtual void show(bool b) {UBox::show(b);}
    virtual void show(const char* msg) {showMsg(msg);}
    virtual void show(const UStr& msg) {showMsg(msg);}
    
    virtual void showMsg(const char* msg);
    virtual void showMsg(const UStr& msg);
    ///< shows the alert box with this message.
    
  private:
    UElem message;
  };
  
}
#endif
