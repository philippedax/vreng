/************************************************************************
 *
 *  ufilebox.hpp: File Box Element
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

#ifndef _ufilebox_hpp_
#define	_ufilebox_hpp_ 1
#include <ubit/ubox.hpp>
#include <ubit/uchoice.hpp>
namespace ubit {

/* File chooser.
* A UFilebox can be included into a UDialog or a UMenu as follows:
* examples:
* <pre>
*   class Demo {
*   public:
*     void openFile(UEvent& e) {...}   // the UEvent& parameter is optional
*     ...
*   };
*
*   Demo* d = new Demo();
*
*   // NB: ufilebox(...) is a shortcut for *new UFilebox(...)
*   UBox& fbox = ufilebox( utitle("Open File")
*                          + UOn::action / ucall(this, &Demo::openFile)
*                         );
*   UDialog& fdialog = udialog(fbox)
*   // and/or:
*   UMenu& fmenu = umenu(fbox)
* </pre>
*
* this->openFile() is called when a file is double clicked or when
* the OK button of the filebox is clicked.
*
* @see: UCall for more info on callback methods/functions and their parameters
*/
class UFilebox : public UBox {
public:
  UCLASS(UFilebox)

  UFilebox(const UArgs& arglist = UArgs::none);
  /**< create a new file box (@see also shortcut ufilebox()).
    * The arglist is typically used to add a callback function.
    * see class UFilebox for an example.
    */
    
  virtual ~UFilebox();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  const UStr& getDir() const {return *fdir2;}
  ///< returns the directory of the file (without the file name).

  const UStr& getName() const {return *fname;}
  ///< returns the name of the file (without the directory).

  const UStr& getPath() const {return *fpath;}
  UStr& path() {return *fpath;}
  ///< returns the full pathname: getDir()/getName().

  const UStr& getFilter() const {return *ffilter;}
  ///< returns the extension filter.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual void setDir(const UStr&);
  ///< note that setDir and setFilter reload the directory.

  virtual void setName(const UStr&);

  virtual void setFilter(const UStr&);
  ///< syntax: "C++ Files (*.cpp; *.hpp)" or "*.cc; *.hh" or "*.*" 

  virtual void showList(bool);
  virtual void showHiddenFiles(bool);

  virtual void rescan();
  ///< rescans the directory and updates the display.

  void setAutoClose(bool stat) {autoclose = stat;}
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
  UStr fspec;
  uptr<UStr> fname, fdir2, ffilter, fpath;
  //UScrollpane* scrollpane;
  UBox *mainbox;
  UBox *show_list, *show_hidden_files;
  URadioSelect new_sel;
  bool autoclose;
  
  virtual void cancelBehavior(UInputEvent&);
  virtual void okBehavior(UInputEvent&);
  virtual void selectBehavior(UEvent&, UStr* pathname);
  virtual void setDirImpl(UStr*);
  virtual void changeDirImpl(UStr*);
  /* sets the directory in a relative way.
    * - goes to the parent directory if arg is ".."
    * - goes to subdirectory "xxx" if arg is "xxx"
    */
};

UFilebox& ufilebox(const UArgs& arglist = UArgs::none);
///< shortcut that returns *new UFilebox().

}
#endif



