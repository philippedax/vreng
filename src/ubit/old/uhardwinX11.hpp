/*************************************************************************
 *
 *  UHardwinX11.hpp : Native Layer: platform dependent implementation
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#if UBIT_WITH_X11

#ifndef UHardwinX11_hpp
#define	UHardwinX11_hpp 1

#include <X11/X.h>
#include <X11/Xlib.h>
#include <ubit/ugl.hpp>
#include <ubit/udisp.hpp>
#include <ubit/uwinImpl.hpp>
namespace ubit {

class UDispX11;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** X11 window.
*/
class UHardwinX11 : public UHardwinImpl {
public:
  UHardwinX11(UDispX11*, UWin*);
  virtual ~UHardwinX11();  

  UDispX11* getDispX11() const {return (UDispX11*)disp;}
  Window    getSysWin() const {return sys_win;}
  Display*  getSysDisp() const;
  Screen*   getSysScreen() const;

  virtual void realize(WinType, float w, float h);
  virtual bool isRealized() const {return sys_win != None;}

  virtual void show(bool);
  virtual void toBack();
  virtual void toFront();

  virtual UPoint getScreenPos() const;
  ///< returns window position relatively to the screen.

  virtual UPoint getPos() const;  
  ///< returns window position relatively to its parent window.
  
  virtual void setPos(const UPoint&);
  ///< changes window position relatively to its parent window.
      
  virtual UDimension getSize() const;
  ///< returns window size.
  
  virtual void setSize(const UDimension&);
  ///< changes window size.
  
  virtual UStr getTitle() const;
  ///< returns window title.
  
  virtual void setTitle(const UStr&);
  ///< changes window title.
  
  virtual UStr getIconTitle() const;
  ///< returns icon title.
  
  virtual void setIconTitle(const UStr&);  
  ///< changes icon title.
  
  virtual void setCursor(const UCursor*);
  ///< changes window cursor (can be null).
  
  virtual void setClassProperty(const UStr& instance_name, const UStr& class_name);
  ///< changes the WM_CLASS property.
  
protected:
  friend class UDispX11;
  friend class UGLcanvas;
  friend class UGraph;
  friend class UX11context;
  Window sys_win;
};

}
#endif
#endif // UBIT_WITH_X11
