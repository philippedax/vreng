/* ==================================================== ======== ======= *
 *
 *  ugl.hpp : OpenGL includes
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

#ifndef _ugl_hpp_
#define	_ugl_hpp_ 1
#include <ubit/ubit_features.h>

#if UBIT_WITH_GLUT  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GLUT includes GL headers

#if (defined(__MACH__) && defined(__APPLE__))
#  include <GLUT/glut.h>
#else 
#  include <GL/glut.h>
#endif

#elif UBIT_WITH_X11   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#if UBIT_WITH_GL
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glx.h>
#endif

#endif //endif UBIT_WITH_X11  // - - - - - - - - - - - - - - - - - - - - - - - -


#endif


