/************************************************************************
 *
 *  uappli.cpp: Appplication Context (must be created first)
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

#include <ubit/ubit_features.h>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <locale.h>
#include <unistd.h>       // darwin
#include <sys/stat.h>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/udialogs.hpp>
#include <ubit/umenuImpl.hpp>
#include <ubit/uwinImpl.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/ueventflow.hpp>
#include <ubit/uupdate.hpp>
#include <ubit/uselection.hpp>
#include <ubit/uconf.hpp>
#include <ubit/ustr.hpp>
#include <ubit/usource.hpp>
#include <ubit/utimer.hpp>
#include <ubit/ugraph.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

//UConf UAppli::conf is in uconf.cpp to avoid init problems

// the (unique) implementation of the application context
static UAppliImpl appli_impl;
UAppliImpl& UAppli::impl = appli_impl;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UAppliImpl::UAppliImpl() : 
appli(null),
disp(null),
app_name(null),
is_terminated(false),
// should not be set to null because UAppliImpl constr may be called after the
// init. of this variable
//error_handler(null), cf UAppli::getErrorHandler()
main_frame(null),
sources(null),
modalwins(null),
messmap(null),
app_motion_lag(15),
nat_motion_lag(100),
main_status(0), modal_status(0),
mainloop_running(false), subloop_running(false),
request_mask(0),
is_processing_update_requests(false),
is_processing_layout_update_requests(false) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UStr& UAppli::initConf(int& argc, char** argv, UOption* opts, const char* cfile) {
  conf.readAttributes(cfile);
  // parseOptions(opts) first because they superseede Ubit options
  if (opts) conf.parseOptions(argc, argv, opts);
  conf.parseUbitOptions(argc, argv);
  conf.freeze_options = true;

#ifdef UBIT_WITH_GDK
  gdk_init(&argc, &argv);     // une seule fois!
#endif  
  return conf.display;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UAppli::UAppli(int& argc, char** argv, UOption* opts, const char* confile) {
  UStr s = argv[0];
  impl.app_name = new UStr(s.basename());
  impl.disp = UDisp::create(initConf(argc, argv, opts, confile));

  if (!impl.disp->isOpened()) {   // throws an exception
    UAppli::fatalError("UAppli","The UAppli display can't be opened (is the X11 Server running?)");
  }

  if (impl.appli) {
    UAppli::fatalError("UAppli","The UAppli is already created (an application must have only one UAppli)");
    return;
  }
  impl.appli = this;
  impl.sources = new UElem();
  // UWinList elements are not counted has parents when auto deleting children
  impl.modalwins = new UWinList();
  
  // creates the default flow (action required by certain toolkits)
  impl.disp->obtainChannelFlow(0);
  
  impl.mainloop_running = impl.subloop_running = false; 
  impl.disp->startAppli(); 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UAppli::~UAppli() {
  impl.is_terminated = true;
  //pose probleme: des destructeurs peuvent etre appeles juste apres
  //-> ne pas faire d'exception dans un tel cas!  
  // impl.appli = null;
  impl.disp->quitAppli();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*  APPELE OU ?
void UAppli::realize() {
  UDisp::realize();
  //if (conf.locale) setlocale(LC_CTYPE, conf.locale);
  if (!conf.locale.empty()) setlocale(LC_ALL, conf.locale.c_str());
  //impl.natappli->setProperties(conf);
  const UStr& name = getAppName();
  static const UStr res_class = "Ubit";
  natdisp->getMainFrame().setNames(res_class, name, name, name);
} 
*/
  //bool UAppli::isRealized() const {return natdisp->isRealized();}
  //const char* UAppli::getCommandName() const {return conf.app_name;}
  //const char* UAppli::getCommandPath() const {return (conf.app_argc>0 ? conf.app_argv[0] : null);}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
UAppli& UAppli::appli() throw (UError) {
  if (!impl.appli)    // throws an exception
    UAppli::fatalError("UAppli::appli()","no UAppli instance (not created yet?, already destroyed?)");
  return *impl.appli;
}

UAppli* UAppli::getAppli() {return impl.appli;}

bool UAppli::isExiting() {return impl.is_terminated;}

const char* UAppli::getVersion() {return UBIT_VERSION;}

//const char* UAppli::getWindowingToolkit() {return conf.windowing_toolkit;}

bool UAppli::isRunning() {return impl.mainloop_running;}

UFrame* UAppli::getMainFrame() {return impl.main_frame;}

const UStr& UAppli::getName() {
  if (!appli_impl.app_name) appli_impl.app_name = new UStr();
  return *appli_impl.app_name;
}

UStyleSheet& UAppli::getStyleSheet() {
  return impl.stylesheet;
}

const UStyle& UAppli::getDefaultStyle() {
  static UStyle* default_style = new UStyle();
  return *default_style;
}


UStr UAppli::getTitle() {
  if (impl.main_frame) return impl.main_frame->getTitle();
  else return "";
}

void UAppli::setTitle(const UStr& title) {
  if (impl.main_frame) impl.main_frame->setTitle(title);
}

void UAppli::setFocus(UView* v) {
  getFlow(0)->setFocus(v);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// En arg: n'importe quelle Window; mais seul le MainFrame (= le 1er Frame 
// ajoute a UAppli) est automatiquement affichee par defaut
void UAppli::add(UWin& win) {
  impl.disp->add(win);
  UFrame* frame = dynamic_cast<UFrame*>(&win);
  if (frame && !impl.main_frame) {    // makes the first UFrame be the 'Main Frame'
    impl.main_frame = frame; 
    impl.main_frame->wmodes.IS_MAINFRAME = true;
  }
}

void UAppli::add(UWin* win) {
  if (!win) error("UAppli::add","null argument");
  else add(*win);
}

void UAppli::remove(UWin& win, bool remove_mode) {
  if (&win == impl.main_frame) impl.main_frame = null;
  impl.disp->remove(win, remove_mode);
}

void UAppli::remove(UWin* win, bool remove_mode) {
  if (!win) error("UAppli::remove","null argument");
  else remove(*win, remove_mode);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void updateAll2(UElem* grp, const UUpdate& mode) {
  // NB: du fait des parents multiples, il est possible de remettre
  // a jour plusieurs fois la meme window
  // ==> il faudrait tenir compte du flag must-update
  if (grp->toWin()) grp->toWin()->update(mode);
  
  for (UChildIter c = grp->cbegin(); c != grp->cend(); ++c) {
    UElem* g = (*c)->toElem();
    if (g) updateAll2(g, mode);    // in any case
  }
}

//  updates all visible windows recursively
void UAppli::updateAll(const UUpdate& mode) {
  for (UChildIter c = impl.disp->winlist.cbegin(); c != impl.disp->winlist.cend(); ++c) {
    UElem* g = (*c)->toElem();
    if (g) updateAll2(g, mode);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UAppli::start() {
  // updateAll() remet l'affichage a jour avant d'entrer dans la mainloop
  // pour eviter des pbms d'initialisation du layout, en part. avec le texte
  impl.appli->updateAll();

  //if (impl.app_title.empty()) setTitle(*impl.app_name);
  //else setTitle(impl.app_title);
  UStr t = getTitle();
  if (t.empty()) setTitle(*impl.app_name);

  impl.main_status = 0;
  impl.disp->startLoop(true);  // main loop

  // detruire ce qui doit l'etre mais ne pas appeler les requetes d'affichage
  impl.processDeleteRequests();
  return impl.main_status;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UAppli::quit(int status) {
  impl.main_status = status;
  impl.is_terminated = true;
  impl.disp->quitLoop(true);  // main loop
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int UAppliImpl::startModalWinLoop(UWin& win) {
  modal_status = 0;
  disp->startLoop(false);  // sub loop
  processPendingRequests();
  return modal_status;
}

void UAppliImpl::setModalStatus(int status) {
  modal_status = status;
}

void UAppliImpl::addModalWin(UWin& win) {
  modalwins->remove(win, false);   // remove if already in the list
  modalwins->add(win, 0);            // add at the beginning of the list
}

void UAppliImpl::removeModalWin(UWin& win) {
  disp->quitLoop(false);     // subloop
  modalwins->remove(win, false);
}

//==============================================================================

void UAppliImpl::processPendingRequests() {
  is_processing_update_requests = false;
  processUpdateRequests();
  processDeleteRequests();
  request_mask = 0;
}

void UAppliImpl::processDeleteRequests() {
  // views
  for (unsigned int k = 0; k < del_view_list.size(); ++k) {
    ::operator delete(del_view_list[k]);    // enforces deletion
  }
  del_view_list.clear();
  
  // bricks
  for (unsigned int k = 0; k < del_obj_list.size(); ++k) {
    ::operator delete(del_obj_list[k]);    // enforces deletion
  }
  del_obj_list.clear();
  
  request_mask &= ~DELETE_REQUEST;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UAppliImpl::addDeleteRequest(UView* v) {
  del_view_list.push_back(v);
  request_mask |= DELETE_REQUEST;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UAppliImpl::addDeleteRequest(UObject* b) {
  b->omodes.IS_DESTRUCTED = true;  // securite: normalement c'est deja le cas
  
  // si b est dans updatelist il faut l'enlever
  if (b->omodes.IS_UPDATING) removeUpdateRequests(b->toBox());
  /*
   if (b->omodes.IS_UPDATING) {  // si b est dans updatelist il faut l'enlever
     UpdateRequest* req = null;
     unsigned int k = 0;
     while ((req = findUpdateRequest(b, k))) {
       req->obj = null;         // remove from the updatelist
       k++;
     }
   }
   */  
  if (UAppli::conf.postpone_delete) {
    del_obj_list.push_back(b);    
    request_mask |= DELETE_REQUEST;    
  }
  else ::operator delete(b);    // enforces deletion
}

//==============================================================================

void UAppliImpl::processUpdateRequests() 
{  
  // NB: is_processing_update_requests: avoid infinite loops: this fct cant call itself
  if (is_processing_update_requests || (request_mask & UPDATE_REQUEST)==0 || is_terminated)
    return;
  
  is_processing_update_requests = true;
    
  // this will prevent UView::updateWinPaint() to draw anything as the final
  // refresh is performed for the entire windows a few lines below
  if (UAppli::conf.is_using_gl) is_processing_layout_update_requests = true;
  
  // incorrect: voir plus bas
  //UpdateRequests::iterator p = update_list.begin();
  //UpdateRequests::iterator p_end = update_list.end();
  //for ( ; p != p_end; ++p) {

  // !!CAUTION: addUpdateRequest() may be called WHILE processUpdateRequests()
  // is being executed => the update_list vector may be reallocated at any time
  // (hence, is size() and the address of its elements may change)
  
  for (unsigned int k = 0; k < update_list.size(); ++k) {
    UBox* obj = update_list[k].obj;
    if (obj) {     // obj == null if the obj was deleted in the meanwhile
      obj->doUpdate(update_list[k].upd, null);     // !!!&&& second arg should be disp !!!
      obj->omodes.IS_UPDATING = false;
      if (is_terminated) return;
    }
  }
  
  is_processing_layout_update_requests = false;

  update_list.clear();

  if (UAppli::conf.is_using_gl) {  // refresh des windows modifiees !!!A METTRE DANS UDisp
    // !!!! IL FAUDRAIT considere TOUS les UDisp  !!!@@@
    UDisp::HardwinList::iterator c = disp->hardwin_list.begin();
    UDisp::HardwinList::iterator c_end = disp->hardwin_list.end();
    for ( ; c != c_end; ++c) {
      UHardwinImpl* hw = *c;
      if (hw && hw->must_update && hw->win) {
        //cerr << "> processUpdateRequests: HW: " << hw <<endl;
        hw->win->doUpdate(UUpdate::paint, disp);
        hw->must_update = false;
        //cerr << "< processUpdateRequests: HW: " << hw <<endl <<endl;
      }
    }
  }
  
  is_processing_update_requests = false;
  request_mask &= ~UPDATE_REQUEST;  // remove UPDATE_REQUEST
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UAppliImpl::addUpdateRequest(UBox* obj, const UUpdate& upd) {
  // don't update an object that has been destructed
  if (obj->omodes.IS_DESTRUCTED || obj->omodes.IS_DESTRUCTING || is_terminated) return;
  
  if (update_list.size() > 0) {   // same obj, same upd => nothing to update
    UpdateRequest& req = update_list[update_list.size()-1];
    if (req.obj == obj && req.upd == upd) return;
  }
  
  // ! addUpdateRequest() may be called WHILE processUpdateRequests() is being executed
  //if (is_processing_update_requests)
  //  cerr << "!!! is_processing_update_requests !!!"<<endl;
  
  bool remove_paint = false;
  
  if (UAppli::conf.is_using_gl) { 
    if (upd.modes == UUpdate::PAINT) {
      for (UView* v = obj->views; v != null; v = v->next) {
        if (v->hardwin) v->hardwin->must_update = true;
      }
      goto END;
    }
    else if (upd.modes & UUpdate::PAINT) {
      for (UView* v = obj->views; v != null; v = v->next) {
        if (v->hardwin) v->hardwin->must_update = true;
      }
      remove_paint = true;   // remove UUpdate::PAINT
    }
  }
  
  update_list.push_back(UpdateRequest(obj, upd, remove_paint)); 
  
END:
  obj->omodes.IS_UPDATING = true;  // objects removed then from the upd list if deleted 
  request_mask |= UPDATE_REQUEST;
}

void UAppliImpl::removeUpdateRequests(UBox* box) {
  if (is_terminated || box == null) return;
  for (unsigned int k = 0; k < update_list.size(); ++k) {
    if (box == update_list[k].obj) update_list[k].obj = null;
  }  
}

/*
UpdateRequest* UAppliImpl::findUpdateRequest(UBox* obj, unsigned int& k) {
  if (is_terminated) return null;
  for (; k < update_list.size(); k++) {
    if (obj == update_list[k].obj) return &update_list[k];
  }
  return null;
}
*/
//==============================================================================

void UAppli::deleteNotify(UDisp* d) {
  for (unsigned int k = 0; k < impl.displist.size(); ++k) {
    if (impl.displist[k] == d) {
      // va tout decaler si on enleve de la liste !
      impl.displist[k] = null;
      break;
    }
  }
}

void UAppli::deleteNotify(UView* deleted_view) {
  UFlowList& flist = impl.flowlist; 
  for (unsigned int k = 0; k < flist.size(); k++) {
    if (flist[k]) flist[k]->deleteNotify(deleted_view);
  }
}

void UAppli::deleteNotify(UElem* deleted_group) {
  UFlowList& flist = impl.flowlist; 
  for (unsigned int k = 0; k < flist.size(); k++) {
    if (flist[k]) flist[k]->deleteNotify(deleted_group);
  }
}

//==============================================================================

const UDispList& UAppli::getDispList() {
  return impl.displist;
}

UDisp* UAppli::getDisp() {return impl.disp;}

// NB: un acces direct par [] serait possible
UDisp* UAppli::getDisp(int _id)  {
  for (unsigned int k = 0; k < impl.displist.size(); ++k) {
    if (impl.displist[k]->getID() == _id) return impl.displist[k];
  }
  return null;
}
 
UDisp* UAppli::openDisp(const UStr& _display_name) {
  //UDisp* d = new UDisp(_display_name);
  UDisp* d = UDisp::create(_display_name);
  if (d && d->isOpened()) return d;
  else {
    delete d;
    return null;
  }
}

void UAppli::closeDisp(UDisp* d) {
  delete d;
}

bool UAppli::hasTelePointers() {
  return (conf.tele_pointers && impl.displist.size() > 1);
}

//==============================================================================

const UFlowList& UAppli::getFlowList() {
  return impl.flowlist;
}

// NB: un acces direct par [] serait possible
UEventFlow* UAppli::getFlow(int _id)  {
  for (unsigned int k = 0; k < impl.flowlist.size(); ++k) {
    if (impl.flowlist[k]->getID() == _id) return impl.flowlist[k];
  }
  return null;
}

USelection* UAppli::getSelection(int _id) {
  UEventFlow* fl = getFlow(_id);
  if (!fl) return null;
  else return fl->getSelection();
}

//==============================================================================

UMenu* UAppli::getOpenedMenu() {
  UEventFlow* fl = impl.disp->obtainChannelFlow(0);  // DEFAULT IFLOW : A REVOIR
  return fl ? fl->menu_man.getDeepestMenu() : null; 
}

const UStr& UAppli::getImaPath() {
  return impl.imapath;
}

void UAppli::setImaPath(const UStr& value) {
  impl.imapath = value;
  UStr& path = impl.imapath;
  
  if (path[0] == '~' && (path[1] == '/' || path[1] == 0)) {
    //NB: ne PAS faire de free() sur un getenv!
    static char* home = ::getenv("HOME");
    if (home) path.replace(0, 1, home);   // virer ~ mais pas /
  }
  
  // ajouter / final
  if (path[-1] != '/') path.append('/'); 
}

//==============================================================================

void UAppli::setMotionLag(unsigned long app_lag, unsigned long nat_lag) {
  impl.app_motion_lag = app_lag;
  impl.nat_motion_lag = nat_lag;
}

unsigned long UAppli::getTime() {return UTimerImpl::getTime();}

void UAppli::postpone(UCall& c) {  // pas tout a fait correct si mthreads!
  addTimeout(0, 1, c);
  //if (pthread_self() != main_thread) {
  // UMessage::send(*impl.disp->getMainFrame(), "wakeup");    !!!@@@@!!!!
  // }
}

void UAppli::addTimeout(unsigned long _delay, int _ntimes, UCall& c) {
  UTimer* t = new UTimer(_delay, _ntimes, true); // true => auto_delete
  t->onTimeout(c);
  t->start();
}

//==============================================================================

UMessagePortMap* UAppli::getMessagePortMap() {return impl.messmap;}

UMessagePort* UAppli::findMessagePort(const UStr& name) {
  if (impl.messmap) return impl.messmap->findMessagePort(name);
  else return null;
}

UMessagePort& UAppli::getMessagePort(const UStr& name) {
  if (!impl.messmap) impl.messmap = new UMessagePortMap();
  return impl.messmap->getMessagePort(name);
}

void UAppli::onMessage(const UStr& name, UCall& c) {
  UMessagePort& mp = getMessagePort(name);
  //mp.onChange(c); devrait etre onChange si c'etait une UNode
  mp.add(c); 
}

//==============================================================================

void UAppli::error(const char* fun, const char* format, ...) throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(UError::ERROR, null/*object*/, fun, format, ap);
  va_end(ap);
}

void UAppli::warning(const char* fun, const char* format, ...) throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(UError::WARNING, null/*object*/, fun, format, ap);
  va_end(ap);
}

void UAppli::fatalError(const char* fun, const char* format, ...) throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(UError::FATAL_ERROR, null/*object*/, fun, format, ap);
  va_end(ap);
}

void UAppli::internalError(const char* fun, const char* format, ...) throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(UError::INTERNAL_ERROR, null/*object*/, fun, format, ap);
  va_end(ap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UAppli::raiseError(int errnum, const UObject* obj, const char* funcname, 
                        const char* format, ...) throw (UError) {
  va_list ap;
  va_start(ap, format);
  getErrorHandler().raiseError(errnum, obj, funcname, format, ap);
  va_end(ap);
}

void UAppli::raiseError(int errnum, const UObject* obj, const char* funcname, 
                        const char* format, va_list ap) throw (UError) {
  getErrorHandler().raiseError(errnum, obj, funcname, format, ap);
}

UErrorHandler& UAppli::getErrorHandler() {
  // this variable should be null even if this UAppliImpl constr was not called
  if (!appli_impl.error_handler) {
    appli_impl.error_handler = 
    new UErrorHandler((appli_impl.app_name ? *appli_impl.app_name : ""), &std::cerr);
  }
  
  if (appli_impl.error_handler->label().empty() && appli_impl.app_name)
    appli_impl.error_handler->label() = *appli_impl.app_name;
  
  return *appli_impl.error_handler;
}

void UAppli::setErrorHandler(UErrorHandler& eh) {
  appli_impl.error_handler = eh;
}

//==============================================================================

UErrorHandler::UErrorHandler(const UStr& _label, std::ostream* _fout) : 
plabel(ustr(_label)),
pbuffer(null),
fout(_fout) {
}

UErrorHandler::~UErrorHandler() {}

void UErrorHandler::setOutputStream(std::ostream* f) {
  fout = f;
}

void UErrorHandler::setOutputBuffer(UStr* s) {
  pbuffer = s;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UErrorHandler::warning(const char* fun, const char* format, ...) const throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(UError::WARNING, null, fun, format, ap);
  va_end(ap);
}

void UErrorHandler::error(const char* fun, const char* format, ...) const throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(UError::ERROR, null, fun, format, ap);
  va_end(ap);
}

void UErrorHandler::error(int errnum, const UObject *obj, const char* fun, 
                          const char* format, ...) const throw (UError) {
  va_list ap;
  va_start(ap, format);
  raiseError(errnum, obj, fun, format, ap);
  va_end(ap);
}

void UErrorHandler::parserError(int errnum, const UChar* tbuffer,
                                const char* msg1, const UStr& name,
                                const char* msg2, const UChar* line) const throw (UError) {
  // eviter erreurs de positionnement en debut de buffer
  if (line < tbuffer) line = tbuffer;
  
  // calculer la position de line dans text
  int l = 1;
  for (const UChar* t = tbuffer; t <= line; t++ ) {
    if (*t == '\n') l++;
  }
  
  // n'afficher que le debut de line et remplacer les \n par des \ suivis de n
  UStr line_beg;
  if (line) {
    line_beg.append(line, 60);
    for ( int k = 0; k < line_beg.length(); k++) {
      if (line_beg.at(k) == '\n') line_beg.replace(k, 1, "\\n");
    }
  }
  
  UStr& err = ustr("At line ");
  err &= l;
  err &= ":\n";
  err &= line_beg;
  err &= "\n";
  if (msg1) err.append(msg1);
  if (!name.empty()) err.append(name);
  if (msg2) err.append(msg2);
  
  raiseError(errnum, &err);
}

void UErrorHandler::raiseError(int errnum, const UObject* obj, const char* funcname, 
                               const char* format, va_list ap) const throw (UError) {
  UError e(errnum, obj, funcname);
  formatMessage(e, format, ap);
  if (fout && *fout) printOnStream(e);
  if (pbuffer) printOnBuffer(e);
  if (e.errnum < 0) throw e;
}

void UErrorHandler::raiseError(int errnum, UStr* msg) const throw (UError) {
  UError e(errnum, null, null);
    if (msg) {
      strncpy(e.message, msg->c_str(), sizeof(e.message));
      e.message[sizeof(e.message)-1] = 0;
  }
  if (fout && *fout) printOnStream(e);
  if (pbuffer) printOnBuffer(e);
  if (e.errnum < 0) throw e;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UErrorHandler::formatMessage(UError& e, const char* format, va_list ap) const {
  // ICI traiter les translations !
  //char buf[2000] = "";
  char* p = e.message;
  int n = 0, len = 0;
  
  n = 0;
  if (plabel->empty()) sprintf(p, "(uninitialized) %n", &n);
  else sprintf(p, "(%s) %n", plabel->c_str(), &n);  
  len += n;
  p += n;
  
  n = 0;
  const char* errname = getErrorName(e.errnum);
  if (errname) sprintf(p, "%s %n", errname, &n); 
  else sprintf(p, "Custom Error #%d %n", e.errnum, &n); 
  len += n;
  p += n;
  
  if (e.function_name) {
    n = 0;
    if (e.object) sprintf(p, "in %s::%s()%n", 
                          e.object->getClassName().c_str(), e.function_name, &n);
    else sprintf(p, "in %s()%n", e.function_name, &n);
    len += n;
    p += n;
  }
  
  if (e.object) {
    n = 0;
    sprintf(p, " on object %p%n", e.object, &n);
    len += n;
    p += n;
  }
  
  n = 0;
  sprintf(p, "\n%n", &n);
  len += n;
  p += n;
  
  if (format) {
    vsnprintf(p, sizeof(e.message) - len, format, ap);
    //vsprintf(p, format, ap);
  }
  e.message[sizeof(e.message)-1] = 0;
}

const char* UErrorHandler::getErrorName(int errnum) const {
  switch (errnum) {
    case UError::WARNING: 
      return "Warning";
    case UError::ERROR: 
      return "Error"; 
    case UError::FATAL_ERROR: 
      return "Fatal Error";
    case UError::INTERNAL_ERROR: 
      return "Internal Error"; 
    case UError::STYLE_ERROR: 
      return "Style Error"; 
    case UError::CSS_ERROR: 
      return "CSS Error"; 
    case UError::XML_ERROR: 
      return "XML Error"; 
    default:
      return null;
  }
}

void UErrorHandler::printOnStream(const UError& e) const {
  if (!fout) {
    cerr <<  "UErrorHandler::printOnStream: can't print error because output stream is null! " << endl;
  }
  else {
    *fout << e.what() << endl << endl;
  } 
}

void UErrorHandler::printOnBuffer(const UError& e) const {
  if (!pbuffer) {
    cerr <<  "UErrorHandler::printOnBuffer: can't print error because output buffer is null! " << endl;
  }
  else {
    pbuffer->append(e.what());
  }
}

}
