/************************************************************************
 *
 *  ufinderImpl.hpp: ufinder implementation (see ufinder.hpp)
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

#ifndef _ufinderImpl_hpp_
#define _ufinderImpl_hpp_
#include <pthread.h>
#include <ubit/uwin.hpp>
#include <ubit/uicon.hpp>
#include <ubit/ufont.hpp>
#include <ubit/ulistbox.hpp>
namespace ubit {
  
  /* [Impl] A directory in the UFinder.
   */ 
  class UFinderDir : public UItem {
    friend class UFinder;
    UStr fpath, fname;
    uptr<UIconbox> iconbox;
    UPopmenu popmenu;
    bool keep_open;
    int path_type;
    
  public:
    UFinderDir(class UFinder*, const UStr& path, int path_type);
    ~UFinderDir();
    
    const UStr& getDir() const;
    class UIconbox* getIconbox() const;
    
    void setDir(const UStr& dirname);
    void setIconbox(class UIconbox*);
    
    void emph(bool state);
    void setKeepOpen(bool st) {keep_open = st;}
    bool isKeepOpen() {return keep_open;}
  };
  
  /* ==================================================== ===== ======= */
  /* [Impl] A host in the UFinder.
   */ 
  class UFinderHost : public UBox {   // UVbox
  public:
    typedef void (UFinderHost::*HostMethod)();
    
    UFinderHost(class UFinder*, const UStr& hostname);
    void resolve(HostMethod);
    void createClone();
    void deleteClone();
    void addXhost();
    void removeXhost();
    void calibrate();
    void putFile();
    
  protected:
    friend class UFinder;
    class UFinder& fd;
    UStr hostname, address;
    UBox *clone_btn, *xhost_btn, *put_btn, *get_btn;
    UWin *clone_win;
    
    void resolveCB(UMessageEvent&, HostMethod);
    void createCloneImpl();
    void addXhostImpl();
    void calibrateImpl();
    void putFileImpl();
  };
  
  /* ==================================================== ===== ======= */
  /* [Impl] UFinder communication (remote file access by using ssh, etc)
   */ 
  class UFinderCom {
  public:
    UFinderCom(UFinder*, const UStr& path, int path_type);
    
  protected:
    enum {INVALID_URL=-2};
    static void loadDoc(UFinderCom*);  // pthread_create() callback
    void showDoc();
    
    pthread_t thread_id;
    UFinder* fd;
    UStr path, cachepath;
    int type, mode;
    int stat;
  };
  
}
#endif
