/*************************************************************************
 *
 *  UHardwinGLUT.hpp : Native Layer: platform dependent implementation
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
#if UBIT_WITH_GLUT

#ifndef UHardwinGLUT_hpp
#define	UHardwinGLUT_hpp 1
#include <ubit/udisp.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/ugl.hpp>
namespace ubit {

class UDispGLUT;
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GLUT window.
*/
class UHardwinGLUT : public UHardwinImpl {
public:
  UHardwinGLUT(class UDispGLUT*, UWin*);
  virtual ~UHardwinGLUT();
  
  UDispGLUT* getDispGLUT() const {return (UDispGLUT*)disp;}
  int getSysWin() const {return sys_win;}
  
  virtual void realize(WinType, float w, float h);
  virtual bool isRealized() const {return sys_win > 0;}
  
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
  ///< no action with GLUT.
    
protected:
  friend class UDispGLUT;
  friend class UGraph;
  friend class UGlcontext;
  int sys_win;
};

}
#endif
#endif // UBIT_WITH_GLUT
