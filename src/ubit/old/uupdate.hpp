/* ==================================================== ======== ======= *
 *
 *  update.hpp : graphics update
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

#ifndef _update_hpp_
#define	_update_hpp_ 1
#include <ubit/udefs.hpp>
namespace ubit {

/** specifies how to update UElem, UBox, UWin objects and subclasses.
 * See also: UElem::update(), UBox::update(), UWin::update().
 */
class UUpdate {
public:
  enum {
    PAINT = 1<<0,
    ///< updates the paint (not the layout).
    
    LAYOUT = 1<<1,
    ///< updates the layout (not the paint).
    
    LAYOUT_PAINT = (LAYOUT | PAINT),
    ///< updates the layout and the paint.
    
    HIDDEN_OBJECTS = 1<<3,
    /**< updates objects even if they are not shown.
     * by default, hidden objects (when isShown() is false) are not updated.
     */

    SHOW = 1<<6,
    HIDE = 1<<7,
    ADJUST_WIN_SIZE = 1<<8
  };
  
  static const UUpdate paint;
  ///< paint only: prefined constant for UUpdate(PAINT);
  
  static const UUpdate layoutAndPaint;
  ///< layout then paint: prefined constant for UUpdate(LAYOUT|PAINT);
  
  ~UUpdate();

  UUpdate(unsigned int mode_mask = LAYOUT|PAINT);  
  /**< creates an object that specifies how to update UElem(s) and subclasses.
    * This object is given as an argument to UElem::update(), UBox::update(),
    * UWin::update()
    * 'mode_mask' is a OREd combination of the constants defined in the UUpdate enum
    * example:
    * <pre>
    *     ;
    *     UBox* box = ...;
    *     box->update(UUpdate upd(UUpdate::LAYOUT | UUpdate::PAINT));
    * </pre>
    */
    
  UUpdate(const UUpdate&);
  UUpdate& operator=(const UUpdate&);
  
  bool operator==(const UUpdate&);
  
  void addModes(unsigned int m) {modes |= m;}

  long getModes() const {return modes;}
  ///< returns the current mode mask.

  bool isHiddenObjectsMode() const {return (modes & HIDDEN_OBJECTS) != 0;}
  ///< if true, hidden objects (which are normally ignored) will be updated.

  void setPaintData(const UData*);
  ///< [impl] set the data to be painted.
  
  void setPaintStr(const UStr*, int strpos1, int strpos2);
  ///< [impl] set the string to be painted.
  
  void setMove(float delta_x, float delta_y, bool x_percent, bool y_percent);
  ///< [impl] used to move objects.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  enum {
    ADD_REMOVE = 1<<11,
    STR_DATA = 1<<12,  // doit impliquer PAINT
    //SCROLL = 1<<13,    // appel direct de updateView() par Scrollpane sans update()
    MOVE   = 1<<14     // appel en NO_DELAY par UPos
  };
  
private:
  friend class UElem;
  friend class UBox;
  friend class UWin;
  friend class UView;
  friend class UAppliImpl;
  friend class UpdateRequest;
  long modes;

public:  // a cause d'un bug de CC
  struct ScrollMove {
    float delta_x, delta_y;
    bool xpercent, ypercent;
  };
  
  struct StrData {
    const UData* data;
    mutable URect* region;  // mutable impose par UBox::doUpdate()
    int pos1, pos2;
  };
  
private:
  // IMPL NOTE: il ne DOIT pas y avoir de subclasses car UUpdate est copie
  // directement dans UpdateRequest (voir uappliImpl.hpp)  
  union /*UpdateImpl*/ {
    StrData sd;
    ScrollMove sm;
  } props;
};


}
#endif
