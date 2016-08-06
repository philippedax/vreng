/*************************************************************************
 *
 *  UDispGLUT.hpp : Native Layer: platform dependent implementation
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

#ifndef UDispGLUT_hpp
#define	UDispGLUT_hpp 1
#include <ubit/udisp.hpp>
#include <ubit/ugl.hpp>
#include <ubit/uhardwinGLUT.hpp>
namespace ubit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GLUT cursor.
  */
struct UCursorImpl {
  int cursor;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** GLUT display.
*/
class UDispGLUT : public UDisp {
public:
  UDispGLUT(const UStr&);
  virtual ~UDispGLUT();
  
  virtual void   setPointerPos(const UPoint& screen_pos);
  virtual UPoint getPointerPos() const;
  virtual int    getPointerState() const;
  virtual bool   grabPointer(const UCursor*);
  virtual void   ungrabPointer();
  virtual bool   pickWindow(int& x_in_win, int& y_in_win, UHardwinImpl* window,
                            UCursor* cursor = null, UCall* callback = null);

  virtual unsigned long createColorPixel(const URgba&);  // not used with GLUT
  virtual UHardwinGLUT* createWinImpl(UWin*);
  virtual UCursorImpl*  createCursorImpl(int curtype);
  virtual void          deleteCursorImpl(UCursorImpl*);
   
  int getSysFrame() const;
  UWin* retrieveWin(int sys_win);
  void initWinCallbacks(int sys_win);

protected:
  virtual void startAppli();
  virtual void quitAppli();
  virtual void startLoop(bool main_loop);   
  virtual void quitLoop(bool main_loop); 
};

}
#endif
#endif // UBIT_WITH_GLUT
