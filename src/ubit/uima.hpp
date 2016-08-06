/* ***********************************************************************
 *
 *  uima.hpp: images
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

#ifndef _uima_hpp_
#define	_uima_hpp_ 1
#include <ubit/udata.hpp>
namespace ubit {

/** Image.
  *
  * Depending on the constructor, one can create: an empty image, an image that
  * is loaded from a JPEG, GIF or XPM file (see below) or an image that uses
  * XPM data compiled in the program code.
  * 
  * Loading a file image.
  * By default, the constructor does not load the file immediately but first time
  * the UIma is displayed on the screen (but an optional arg changes this behavior).
  * The load() and read() functions can also be used to (re)load a file.
  *
  * Filename and formats:
  * - XPM, GIF and JPEG formats are supported *IF* the corresponding libraries
  *   where linked to the Ubit library at ompiltaion time
  * - the filename must have the appropriate suffix (xpm, gif, jpg, jpeg)
  * - the filename is prefixed by UAppli::getImaPath() if it does not start by / or .
  *   this prefix is empty by default. It can be changed by UAppli::setImaPath()
  *
  * Loading compiled XPM data 
  * XPM data is compiled in the program code. It is not duplicated and must not be
  * freed. As for files, the data is loaded when the image appers for the fisrt time.
  * 
  * Alpha channel and Background Transparency
  * - background transparency (image shaping) is supported by the UPix subclass.
  * - alpha channel is only supported in OpenGL mode (when available, see UAppli).
  *
  * @see: the UPix subclass.
  */
class UIma: public UData {
public:
  UCLASS(UIma)

  UIma(int width, int height);
  /**< creates an empty image.
  */
     
  UIma(const char* filename = null, bool load_now = false);
  /**< creates a new image from an image file.
  * By default, the image file is NOT loaded immediately but the first time
  * the UIma is displayed on the screen. 'load_now' = true, forces the file 
  * to be loaded immediately
  * @see: shortcut uima(const char* filename) and class UIma for more details.
  */
    
  UIma(const UStr& filename, bool load_now = false);
  /**< creates a new image from an image file.
  * By default, the image file is NOT loaded immediately but the first time
  * the UIma is displayed on the screen. 'load_now' = true, forces the file 
  * to be loaded immediately
  * @see: shortcut uima(const UStr& filename) and class UIma for more details.
  */
  
  UIma(const char** xpm_data, bool load_now = false);
  /**< creates a new image from XPM data (beware that XPM data is NOT duplicated!).
  * By default, the data is NOT loaded immediately but the first time
  * the UIma is displayed on the screen. 'load_now' = true, forces the data 
  * to be loaded immediately  
  *
  * !NOTE that the XPM data is not duplicated and should NEVER be freed!
  *
  * @see: shortcut uima(const char** xpm_data) and class UIma for more details.
  */
  
  UIma(const char** xpm_data, UConst);

  virtual ~UIma();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UIma& operator=(const UIma& ima2);
  /**< copies the content of ima2 to this image.
    * bewaee that no action is performed if ima2 has not yet been loaded (@see isLoaded()).
    */
  
  void resize(int width, int height) { resize(width, height, false, false);}
  /**< resizes the image data to the specified size.
    * @see: resize(int width, int height, bool preserve_ratio, bool dont_magnify);
    */

  void resize(int max_width, int max_height, bool preserve_ratio, bool dont_magnify);
  /**< resizes this image data according to the specified constraints.
    * the new size will be inferior or egal to the one specified according to:
    * - preserves the aspect ratio if 'preserve_ratio' is true (either the actual 
    *   width or height may be smaller than the requested one)
    * - dont magnify images that are smaller than the requested size if 
    *   'dont_magnify' is true
    */

  bool rescale(float scale) {return rescale(scale, scale);}
  ///< rescales this image.
    
  bool rescale(float xscale, float yscale);
  ///< rescales this image.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual int read(const UStr& filename, int max_width = 0, int max_height =0);
  /**< loads an image file.
    * calls resize(max_width,max_height,true,true) if max_width and max_height are >0.
    * returns the reading status (@see UFilestat).
    * note: use UIma::set() instead of read() to postone loading.
    */
    
  virtual int read(const char* filename, int max_width = 0, int max_height =0);
  /**< loads an image file.
    * calls resize(max_width,max_height,true,true) if max_width and max_height are >0.
    * returns the reading status (@see UFilestat).
    * note: use UIma::set() instead of read() to postone loading.
    */
  
  virtual int loadFromData(const char** xpm_data);
  /**< loads XPM data.
    * the XPM data is not duplicated and should not be destroyed.
    * note: use UIma::set() instead of read() to postone loading.
    */
    
  virtual int loadNow();
  /**< loads the image file that was specified by set() or by the constructor.
    * returns the reading status (@see UFilestat).
    */
    
  bool isLoaded() const {return stat > 0;}
  /**< returns true if the image has been loaded.
    * Note that images that have not yet been displayed on the screen are not
    * loaded except if stated explicitely (see the UIma::Uima, load(), read())
    */
  
  virtual void set(const UStr& filename);
  virtual void set(const char* filename);
  virtual void set(const char** xpm_data);
  /**< specifies the file name.
    * the file is not loaded immediately but when the image appears for
    * the first time. Call load() to load the file immediately.
    */

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  int getWidth()  const;
  ///< returns the image width (0 if the image is not loaded).
  
  int getHeight() const;
  ///< returns the image height (0 if the image is not loaded).

  int getBpp() const;
  ///< returns the number of bits per pixel (0 if the image is not loaded).
  
  int getTransparency() const;
  ///< 0 (opaque), 1(bitmask) or >1(alpha channel).
  
  int getStatus() const {return stat;}
  ///< returns the loading status of this image (see UFilestat)

  virtual bool isPix() const {return false;}
  ///< returns true if this UIma is in fact a UPix (that derives from UIma).
  
  virtual void update(); 
  ///< update parents' views.

  // - - - impl - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  int realize(int max_w = 0, int max_h = 0,
              UDisp* disp = null, bool force_reload = true) const;
  /**< 
   * [impl] allocates physical resources and loads the image in memory.
   * this function is automatically called when this image is displayed
   * for the first time: there no need to call it in client code.
   * details:
   * - returns the loading status  (see UFilestat)
   * - the 'disp' arg. can be the UAppli
   * - the 'force_reload' arg. specifies if the file must be reloaded:
   *   #- if false, previously loaded files are not reloaded
   *   #- if true files are always reloaded
   */
   
  bool isRealized() const;
  ///< [impl] returns true if the image has been sucesfully loaded and initialized.

  std::list<UHardIma*>& getNatImas() const {return natimas;}
  ///< [impl] returns internal implementation.

  static void getFullPath(UStr& fullpath, const char* filename);
  /**< gets the full image pathname.
    * UAppli::getImaPath() (the default image pathname) is prefixed
    * to image filenames that don't start with / or .
    */

#ifndef NO_DOC
  friend class UBox;
  friend class UHardIma;
  friend class UPix;

  virtual void setImpl(const char* fname);
  virtual void setImpl(const char** xpm_data);
  virtual void setImpl(int width, int height);
  virtual void cleanCache();
  virtual void getSize(UUpdateContext&, UDimension&) const;
  virtual void paint(UGraph&, UUpdateContext&, const URect&) const;

  UHardIma* getOrCreateIma(UDisp*, float xyscale) const;  // not virtual!
  UHardIma* findImaInCache(UDisp*, float xyscale) const;  // not virtual!
  UHardIma* addImaInCache(UDisp*, float xyscale) const;   // not virtual!
  
protected:
  mutable std::list<UHardIma*> natimas;
  char *name;
  const char **data;
  mutable signed char stat;  // can have negative values
  enum Mode {EMPTY, CREATE, READ_FROM_FILE, READ_FROM_DATA};
  mutable char mode;
  bool show_unknown_ima;
#endif
};

inline UIma& uima(const UStr& filename)  {return *new UIma(filename);}
///< shortcut function that creates a new UIma image.

inline UIma& uima(const char* filename)  {return *new UIma(filename);}
///< shortcut function that creates a new UIma image.

inline UIma& uima(const char** xpm_data) {return *new UIma(xpm_data);}
///< shortcut function that creates a new UIma image.

}
#endif
