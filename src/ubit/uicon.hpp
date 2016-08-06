/************************************************************************
 *
 *  uicon.hpp: Icon widget
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

#ifndef _uicon_hpp_
#define _uicon_hpp_
#include <ubit/udoc.hpp>
namespace ubit {
  
  /** UIcon (for UIconbox)
   */
  class UIcon : public UItem {
  public:
    UCLASS(UIcon)
    
    UIcon(const UStr& name = "", UArgs content = UArgs::none);
    ///< creates a new icon.
            
    virtual ~UIcon();
    static UStyle* createStyle();

    virtual int loadImage(const UStr& image_path);
    ///< load and shows the icon image, returns file loading status.
    
    const UStr& getName() const {return *pname;}
    
    bool isDir() const {return is_dir;}
    void setDir(bool state) {is_dir = state;}
    
  protected:
    uptr<UBox> ima_box, text_box;
    uptr<UStr> pname;
    bool is_dir;
  };
  

  /* ==================================================== ======== ======= */
  /** Icon box
   */
  class UIconbox : public UDocbox {
  public:
    UCLASS(UIconbox)
    
    UIconbox(UArgs = UArgs::none);
    virtual ~UIconbox();
    static UStyle* createStyle();

    virtual int readDir(const UStr& pathname, bool remote_dir = false);
    
    // inherited:
    // virtual UBox& titlebar() {return *ptitle_bar;}
    // virtual UBox& content()  {return *pcontent;}
    // virtual UScrollpane& scrollpane()  {return *pspane;}
    // virtual UScale& scale()  {return *pscale;}
    
    virtual UStr& pathname()  {return *ppathname;}
    virtual UStr& title()     {return *ptitle;}
    virtual UListbox& icons() {return *picons;}
    virtual UChoice& choice();
    virtual const UChoice& choice() const;
    
    virtual void addIcon(class UIcon&);
    virtual void removeIcon(class UIcon&, bool auto_delete=true);
    virtual void removeAllIcons(bool auto_delete = true);
    
    virtual class UIcon* getSelectedIcon();
    virtual class UIcon* getPreviousIcon();
    virtual class UIcon* getNextIcon();
    virtual class UIcon* getIcon(int) const;
    
    virtual void selectIcon(UIcon&);
    virtual void selectIcon(const UStr& name);
    virtual void selectPreviousIcon();
    virtual void selectNextIcon();
    
    // - - - impl.  - - - - - - - - - - - - - - - - - - - - - - - - - -
  protected:
    friend class UFinder;
    uptr<UStr> ppathname, ptitle;
    uptr<UListbox> picons;
    uptr<UHspacing> icon_hspacing;
    uptr<UVspacing> icon_vspacing;
    unsigned long filetime;
    bool show_parent_dir;
    virtual void okBehavior(UInputEvent&); 
  };
  
}
#endif


