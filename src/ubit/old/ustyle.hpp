/************************************************************************
 *
 *  ustyle.hpp: Element and Box styles
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

#ifndef _ustyle_hpp_
#define	_ustyle_hpp_ 1
#include <ubit/udefs.hpp>
#include <ubit/uborder.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ugeom.hpp>
#include <ubit/uon.hpp>
namespace ubit {
  
  struct ULocalProps {
    ULocalProps();
    
    USizeSpec size;                // object size
    UPaddingSpec padding;          // space beween content and border (if any)
    const UBorder* border;         // object border
    const UBackground* background; // image or color background of the component
    float alpha;                   // for alpha blending
    // const UShape* shape;        // shape of the component
    // if this group is not null the objects it contains are added to children for 
    // display (e.g. for adding list-item or checkboxes markers)
    UElem* content;    // NB: = null in most cases    
  };
  
  
  /** Compiled Object Style.
   * specify the appearance of UElem/UBox/UWin objects and subclasses.
   */
  class UStyle {
  public:
    UStyle();
    virtual ~UStyle();
    virtual const UStyle& getStyle(UUpdateContext*) const {return *this;}
    
    void setAlpha(float a) {local.alpha = a;}
    //void setBackground(const UBackground*);
    void setBorder(const UBorder* b) {local.border = b;}
    void setCursor(const UCursor* c) {cursor = c;}
    void setFont(const UFont* f) {font = f;}
    void setSize(ULength width, ULength height);
    void setPadding(ULength horiz, ULength vert);
    void setHorizPadding(ULength left, ULength right);
    void setVertPadding(ULength top, ULength bottom);

    const UColor* getColor(const UElem&) const;
    const UColor* getBgcolor(const UElem&) const;
    void setColors(UColor& all_colors);
    void setColors(UColor& unselected_colors, UColor& selected_colors);
    void setBgcolors(UColor& all_colors);
    void setBgcolors(UColor& unselected_colors, UColor& selected_colors);
    void setColor(int action, UColor&);
    void setBgcolor(int action, UColor&);
    
    // champs locaux a l'object, pas heritables
    ULocalProps local;
    
    // champs heritables
    char orient;
    char  valign, halign;      
    short vspacing, hspacing;
    const UViewStyle* viewStyle;
    //UEdit* edit;
    const UFont* font;
    const UCursor* cursor;
    const UStr* textSeparator;  // text separator between children when printing
    UColor* colors[2 * UOn::ACTION_COUNT];
    UColor* bgcolors[2 * UOn::ACTION_COUNT];
    //const UColor *color, *bgcolor;
    //class UAttrList* attrs;
  };
  
}
#endif
