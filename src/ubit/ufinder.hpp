/************************************************************************
 *
 *  ufinder.hpp: UFinder element
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

#ifndef _ufinder_hpp_
#define _ufinder_hpp_
#include <ubit/ucolor.hpp>
#include <ubit/ubackground.hpp>
#include <ubit/umenu.hpp>
#include <ubit/udoc.hpp>
#include <ubit/uicon.hpp>
namespace ubit {

 class UFinderDir;
 class UFinderHost;
 class UFinderFullwin;
 class UFinderControls;
 class UFinderCom;

/* ==================================================== ===== ======= */
/** UFinder Listener.
*/
struct UFinderListener {
  virtual ~UFinderListener() {}
  virtual void fileRequest(const UStr& pathname) {}
  virtual void docLoaded(const UStr& pathname, UDoc*) {}
  virtual void docShown(const UStr& pathname, UDoc*) {}
  virtual void dirLoaded(const UStr& pathname) {}
  virtual void dirShown(const UStr& pathname) {}
};

/* ==================================================== ===== ======= */
/** file finder.
*/
class UFinder : public UBox {
public:
  UCLASS(UFinder)

  struct Options : UElem {
    friend class UFinder;
    Options();
    USize  clone_frame_size;
    UBackground default_background;
    UColor default_color;
    bool show_icon_images;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  UFinder(const UStr& pathname = ".");
  virtual ~UFinder();
    
  virtual void open(const UStr& pathname);
  ///< opens a directory or a file (the second variant normalizes pathname).

  virtual void openParent();
  virtual void openEntry();  
  virtual void nextEntry();
  virtual void previousEntry();
  ///< navigates in the finder.

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  virtual bool browseHosts();
  virtual UFinderHost* addHost(const UStr& hostname);
  virtual UFinderHost* addHostCB(const UStr* hostname);
  virtual void removeHost(UFinderHost*);
  virtual void addHosts(const std::vector<UStr*>& hostnames);
  virtual void addHosts(const char* hostnames[]);

  virtual void createClone(const UStr& hostname);
  ///< creates a synchronized clone on hostname.

  // virtual void compactEvents(bool = true);
  // compact events to avoid delays.

  virtual void setTracking(bool);
  virtual void setTracking(bool doc, bool icons);
  ///< tracking mode updates the value while the scrollbar is being dragged (default is true).

  virtual UFinderListener* getListener();
  virtual void setListener(UFinderListener*);
  /**< gets/set the current listener.
    * the finder creates a default listener that opens documents
    * when hyperlinks are clicked (see UFinderListener)
    * limitation: only one listener in this version!
    */
    
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Options& getOptions() {return opts;}
  ///< returns the options of the UFinder.
  
  UDocbox*  getDocbox() {return pdocbox;}
  UIconbox* getIconbox() {return piconbox;}

  UDocbox* getCurrentBox();
  ///< returns docbox or iconbox depending on which is currently shown.
  
  UIcon* getSelectedIcon();
  ///< return the icon box that is currently shown (if any, null otherwise).
  
  UDoc* getDoc() {return pdocument;}
  ///< return the document box that is currently shown (if any, null otherwise).

  virtual void zoom(float zoom = 1.);
  virtual void zoomIn();
  virtual void zoomOut();
  virtual void setZoomQuantum(float);
    
  UCtlmenu& getContextMenu() {return *ctlmenu;}
  ///< returns the Contextual menu of the Finder.
  
  virtual void openContextMenuIn(UBox&);
  virtual UCtlmenu& createContextMenu();
 
  void showSideBar(bool state);
  ///< shows/hides the side bar (located on the left side).

  void showAlert(const UStr& msg);
  void hideAlert();
  ///< shows/hides an alert message.
  
  //void showDocGlass(bool state);
  // shows/hides the document glass.
  //UBox* getGlass() {return docglass;}
  //returns the glass box that is on top of the document box.
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
protected:
  // documents.
  void openFBox(UFilebox&);
  void openMsg(UMessageEvent&);
  virtual void openImpl(const UStr& path, int path_mode, int path_type);
  virtual int openFile(const UStr& path, int path_type);
  virtual int openDir(const UStr& path, int path_type);
  virtual int openDir(UFinderDir*);
  virtual void showFile(const UStr& path, UDoc*);
  virtual void showDir(const UStr& path);
  virtual void showPreview(UIcon*);
  virtual void showPreviewRequest(UIcon*);
  virtual void showIconPreviews();  
  
  // dirs and icons.
  virtual void addToHistory(UFinderDir*);
  virtual void removeFromHistory(UFinderDir*);
  virtual UFinderDir* findDir(const UStr& path);
  virtual void showDirInfo(UFinderDir*);
  virtual void removeIconbox(UFinderDir*, bool upd);
  virtual void iconSelectCB(UIconbox*);
  virtual void iconActionCB(UIconbox*);
  virtual void linkActionCB(UInputEvent&, UDoc*);

  // hosts and clones.
  virtual void browseCB(UMessageEvent&);
  bool isBrowsing() const;
  virtual void createCloneRequest(UMessageEvent&);
  virtual UFrame* createCloneFrame(const UStr& title);
  
  virtual void initOptbox();
  void showSelectedIcon();
  void showDirBox();
  void showDocBox();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
private:
  friend class UFinderDir;
  friend class UFinderHost;
  friend class UFinderCom;  
  static UPix& doc_pix;
  enum {NoMode, DirMode, DocMode} mode;
  bool is_tracking, open_in_fullwin;
  Options opts;
  UBar toolbar;
  UBox mainbox, optbox, folderlist, hostlist, filelist;
  uptr<UBox> optbox_btn, filelist_btn;
  uptr<UDialog> ask_dialog;
  uptr<UStr> ask_dialog_msg;
  UFinderListener* listener;
  uptr<UAlertbox> alertbox;
  uptr<UCtlmenu> ctlmenu;
  UMService *local_ums, *remote_ums;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // current selection
  uptr<UDoc> pdocument;
  uptr<UDocbox> pdocbox;
  uptr<UIconbox> piconbox;
  //uptr<UBox> docglass;
  uptr<UFinderDir>last_direntry;
  UIcon* last_preview_request;
  UIcon* last_preview;
  UChildIter previews_current, previews_end;
  uptr<UTimer> preview_timer;
 };

}
#endif
