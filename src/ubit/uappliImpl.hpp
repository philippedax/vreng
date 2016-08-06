/* ==================================================== ======== ======= *
 *
 *  uappliImpl.hpp
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

#ifndef _uappliImpl_hpp_
#define	_uappliImpl_hpp_ 1
#include <vector>
#include <sys/time.h>  // fd_set
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/uclassImpl.hpp>
#include <ubit/uwin.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/ustyle.hpp>
#include <ubit/umessage.hpp>
#include <ubit/utimer.hpp>
namespace ubit {
  
  class UpdateRequest {
  public:
    UBox* obj;
    UUpdate upd;
    UpdateRequest(UBox* _obj, const UUpdate& _upd, bool remove_paint) 
    : obj(_obj), upd(_upd) {if (remove_paint) upd.modes &= ~UUpdate::PAINT;}
  };
  
  
  class UAppliImpl {
  public:
    UAppliImpl();
    
    bool isTerminated() const {return is_terminated;}
    
    bool hasPendingRequest() {return request_mask != 0;}  
    ///< true if there is a delete, update or paint request 
    
    void processPendingRequests();
    ///< process all requests (process update then delete then paint requests).
    
    void addDeleteRequest(UObject*);
    void addDeleteRequest(UView*);
    void processDeleteRequests();
    
    void addUpdateRequest(UBox*, const UUpdate&);
    void removeUpdateRequests(UBox*);
    void processUpdateRequests();
    bool isProcessingUpdateRequests() const {return is_processing_update_requests;}
    bool isProcessingLayoutUpdateRequests() const {return is_processing_layout_update_requests;}
    
    int  startModalWinLoop(UWin&);
    void addModalWin(UWin&);
    void removeModalWin(UWin&);
    void setModalStatus(int);
    
    void resetSources(UElem* sources, fd_set& read_set, int& maxfd);
    void cleanSources(UElem* sources);
    void fireSources(UElem* sources, fd_set& read_set);
    
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //private:
    friend class UAppli;
    friend class UDisp;
    friend class UView;
    friend class UEventFlow;
    friend class UTimer;
    friend class USource;
    friend class UMService;
    
    typedef std::vector<UpdateRequest> UpdateRequests;
    typedef std::vector<UObject*> DeletedObjects;
    typedef std::vector<UView*> DeletedViews;
    
    UAppli* appli;        // only ONE UAppli object should be created
    UDisp* disp;
    UStr *app_name;
    bool is_terminated;   // true if the UAppli has been terminated
    uptr<UErrorHandler> error_handler;
    UFrame* main_frame;   // the main frame of the UAppli
    // Note: the display list must be static to avoid seg faults if the UAppli
    // is distroyed before the widgets by the client program 
    UDispList displist;
    UFlowList flowlist;    // list of event flows
    UStyleSheet stylesheet;
    UStr imapath;
    UElem* sources;
    UTimerImpl timer_impl;
    class UWinList *modalwins;         // modal windows
    UMessagePortMap* messmap;    // the message port of the UAppli
    unsigned long app_motion_lag, nat_motion_lag;
    
    int main_status;       // status of the event loop of the UAppli
    int modal_status;      // status of the inner loop the current modal dialog
    bool mainloop_running, subloop_running;
    
    enum {DELETE_REQUEST = 1<<0, UPDATE_REQUEST = 1<<1, PAINT_REQUEST = 1<<2};
    int request_mask;
    bool is_processing_update_requests, is_processing_layout_update_requests;  
    UpdateRequests update_list;    // boxes and wins that will be updated
    DeletedObjects del_obj_list;   // objects that will be deleted
    DeletedViews   del_view_list;  // views that will be deleted
  };
  
}
#endif


