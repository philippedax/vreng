/* ==================================================== ======== ======= *
 *
 *  upix.hpp: X11 pixmaps
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

#ifndef _upix_hpp_
#define	_upix_hpp_ 1
#include <ubit/uima.hpp>
namespace ubit {

/** Pixmap Image.
  * Pixmaps are a special kind of images that allows for faster rendition when
  * X11 is used as a graphics engine (see UAppli). 
  * 
  * Pixmaps also allows for background transparency (image shaping) for XPM
  * files and data.
  *
  * @see class UIma for details.
 */
class UPix: public UIma {
public:
  UCLASS(UPix)
  
  /** Predefined Pixmap Images (small icons).
   * some of these pixmaps were originally designed by Ludvig A. Norin
   * for the FVWM window manager. Please refer to file upixmaps.hpp
   * for notice (more pixmaps are also available in this file).
   * NB: most of these pixmaps are shaped (= have a "transparent" background)
   */
  static UPix
  metal, velin, folder, ofolder, doc, diskette, plus, minus, 
  zoom, undo, bigFolder, bigDoc, bigText, bigIma, bigLeft, bigRight, bigUp, bigDown, 
  hslider, vslider, ray, question, link, stop;
    
  UPix(const char* filename = null, bool load_now = false);
  ///< creates a new UPix image from an image file; @see UIma and the upix() shortcut.
  
  UPix(const UStr& filename, bool load_now = false);
  ///< creates a new UPix image from an image file; @see UIma and the upix() shortcut.

  UPix(const char** xpm_data, bool load_now = false);
  ///< creates a new UPix image from XPM data; @see UIma and the upix() shortcut.

  UPix(const char** xpm_data, UConst);

  virtual ~UPix();

  virtual bool isPix() const {return true;}
  ///< returns true as this UIma is a UPix.
  
  // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef NO_DOC
  virtual void cleanCache();
  virtual void getSize(UUpdateContext&, UDimension&) const;
  virtual void paint(UGraph&, UUpdateContext&, const URect&) const;
  UHardPix* getOrCreatePix(UDisp*, float xyscale) const;  // not virtual!
private:
#if WITH_2D_GRAPHICS
  std::vector<UHardPix*>& getNatPixs() const {return natpixs;}
  mutable std::vector<UHardPix*> natpixs;
#endif
#endif
};


inline UPix& upix(const UStr& filename)  {return *new UPix(filename);}
///< shortcut function that creates a new UPix image.

inline UPix& upix(const char* filename)  {return *new UPix(filename);}
///< shortcut function that creates a new UPix image.

inline UPix& upix(const char** xpm_data) {return *new UPix(xpm_data);}
///< shortcut function that creates a new UPix image.

}
#endif
