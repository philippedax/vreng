/* ***********************************************************************
 *
 *  uhardfont.hpp
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 1999-2008 Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

#ifndef _uhardfont_hpp_
#define _uhardfont_hpp_ 1
#include <ubit/ubit_features.h>

#if WITH_2D_GRAPHICS
#if UBIT_WITH_X11
#  include <X11/Xlib.h> 
#elif UBIT_WITH_GDK
#  include <gdk/gdk.h> 
#endif
#endif

class FTFont;

namespace ubit {
  
/** [Impl] Native Font.
*/
class UHardFont {
public:
  enum {NO_FONT=0, SYS_FONT, GLX_FONT, FTGL_FONT};

  UHardFont(UDisp*, const UFontDesc&);
  ~UHardFont();

  void  drawString(const char* str, int str_len, float x, float y) const;
  
  bool  isReady() {return status > NO_FONT;}
  int   getStatus() {return status;}
  float getAscent() const;
  float getDescent() const;
  float getHeight() const;
  float getWidth(char) const;  
  float getWidth(const char* str, int len = -1) const;

private:
  friend class UDisp;
  friend class UGraph;
  short status, count;

#if UBIT_WITH_GL
  union {
    unsigned int glf;  //GLuint  glf;
    FTFont* ftf;
  };
  FTFont* loadFTGLFont(UDisp*, const UFontDesc&);
  ///< loads a FTGL font; requires OpenGL and TrueType.
#endif
  
#if WITH_2D_GRAPHICS
# if UBIT_WITH_X11
  friend class UX11context;
  XFontStruct* sysf;
  XFontStruct* loadSysFont(UDisp*, const UFontDesc&);
# elif UBIT_WITH_GDK
  friend class UGDKContext;
  GdkFont* sysf;
  GdkFont* loadSysFont(UDisp*, const UFontDesc&);
# endif
#endif
};

}
#endif

