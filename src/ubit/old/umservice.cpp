/* ==================================================== ======== ======= *
*
*  umservice.cpp: UMS (Ubit Mouse/Message Server) services 
*  Ubit GUI Toolkit - Version 6.0
*  (C) 2008 EricLecolinet / ENST Paris / www.enst.fr/~elc/ubit
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
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <ubit/ucall.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/usource.hpp>
#include <ubit/usocket.hpp>
#include <ubit/umessage.hpp>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/uevent.hpp>
#if UBIT_WITH_X11
#  include <X11/X.h>
#else
#  define KeyPress                2
#  define KeyRelease              3
#  define ButtonPress             4
#  define ButtonRelease           5
#  define MotionNotify            6
#endif
using namespace std;
namespace ubit {

UMService::UMService(const UStr& _host, int _port, const char* _client_name)
: USocket(_host, (_port != 0 ? _port : UMS_PORT_NUMBER)) {
}

UMService::UMService(const UStr& _host, int _port)
: USocket(_host, (_port != 0 ? _port : UMS_PORT_NUMBER)) {
  onInput(ucall(this, &UMService::inputCallback));
}

UMService::~UMService() {
  UMessagePort& mp1 = UAppli::getMessagePort("_umsBrowse");
  if (browse_call) mp1.remove(*browse_call);

  UMessagePort& mp2 = UAppli::getMessagePort("_umsNhb");
   if (neighbor_call) mp2.remove(*neighbor_call);
}

/* ==================================================== ======== ======= */

void UMService::inputCallback() {
  UInbuf ib;
  if (receiveBlock(ib) && UAppli::impl.messmap) {
    //ib.data()[ib.size()-1] = 0; // faux
    UAppli::impl.messmap->fireMessagePort(ib.data());
  }
  /*
   if (input) {
     UAppli* app = UAppli::getApp();
     if (app) {
       // le champ 'display' doit etre celui de l'appli et non de l'UMS
       xev.xany.display = app->getNatDisp()->getXDisplay();
       app->getNatDisp()->dispatchEvent(&xev);
     }
   }
   */
}

/* ==================================================== ======== ======= */
/* ==================================================== [(c)Elc] ======= */

bool UMService::browseUMServers(UCall& c) {
  UMessagePort& mp = UAppli::getMessagePort("_umsBrowse");
  //mp.removeAll();
  if (browse_call) mp.remove(*browse_call);
  browse_call = c;
  mp.add(c);
  return sendRequest(UMSrequest::BROWSE_SERVERS);
}

bool UMService::browseUMSNeighbors(UCall& c) {
  UMessagePort& mp = UAppli::getMessagePort("_umsNeighbor");
  //mp.removeAll();
  if (neighbor_call) mp.remove(*neighbor_call);
  neighbor_call = c;
  mp.add(c);
  return sendRequest(UMSrequest::BROWSE_NEIGHBORS);
}

/* ==================================================== ======== ======= */

UMService::BrowseReply::BrowseReply(UMessageEvent& e) {
  const UStr* msg = e.getMessage();
  flags = 0;
  errorCode = 0;
  interfaceIndex = 0;
  serviceName = "";
  serviceType = "";
  replyDomain = "";
  status = -1;
  if (!msg || !msg->c_str()) return;

  const char* s = msg->c_str();
  const char* p = strchr(s, '\t');
  if (p) serviceName.append(s, p-s);
  else {serviceName = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  if (p) serviceType.append(s, p-s);
  else {serviceType = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  status = atoi(s);

  if (p) {
    s = p+1;
    flags = atoi(s);
  }
}

/* ==================================================== [(c)Elc] ======= */

struct _UMSResolveData : public UCall {
  uptr<UCall>pc;

  _UMSResolveData(UCall& _c) : pc(_c) {}

  void operator()(UEvent& e) {
    (*pc)(e);
    pc = null;      // destruction de pc si pas reference' ailleurs
    delete this;    // destruction de _UMSResolveData
  };
};

// c est auto-detruit si pas reference
bool UMService::resolveUMServer(const UStr& name, UCall& c) {
  _UMSResolveData* rd = new _UMSResolveData(c);
  
  UAppli::onMessage("_umsResolve", *rd);
  return sendRequest(UMSrequest::RESOLVE_SERVER, name);
}

UMService::ResolveReply::ResolveReply(UMessageEvent& e) {
  const UStr* msg = e.getMessage();
  flags = 0;
  errorCode = 0;
  interfaceIndex = 0;
  fullname = "";
  hosttarget = "";
  port = 0;
  if (!msg || !msg->c_str()) return;

  const char* s = msg->c_str();
  const char* p = strchr(s, '\t');
  if (p) fullname.append(s, p-s);
  else {fullname = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  if (p) hosttarget.append(s, p-s);
  else {hosttarget = s; return;}

  s = p+1;
  p = strchr(s, '\t');
  port = atoi(s);

  if (p) {
    s = p+1;
    flags = atoi(s);
  }
}

/* ==================================================== ======== ======= */
/* ==================================================== [(c)Elc] ======= */

bool UMService::moveMouse(int event_flow, int x, int y, bool abs_coords) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(MotionNotify, event_flow, x, y, abs_coords);
  return sendBlock(req);
}

bool UMService::pressMouse(int event_flow, int button_mask) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(ButtonPress, event_flow, 0, 0, button_mask);
  return sendBlock(req);
}

bool UMService::releaseMouse(int event_flow, int button_mask) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(ButtonRelease, event_flow, 0, 0, button_mask);
  return sendBlock(req);
}

bool UMService::pressKey(int event_flow, int keycode) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(KeyPress, event_flow, 0, 0/*ex:mods*/, keycode);
  return sendBlock(req);
}

bool UMService::releaseKey(int event_flow, int keycode) {
  UMSrequest req(UMSrequest::KEY_MOUSE_CTRL);
  req.writeEvent(KeyRelease, event_flow, 0, 0/*ex:mods*/, keycode);
  return sendBlock(req);
}

/*
bool UMService::tactos(const char* data) {
  UMSrequest req(UMSrequest::TACTOS);
  req.writeString(data);
  return sendBlock(req);
}
*/

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

bool UMService::sendMousePress(const char* target,
			       int x, int y, int mouse_btn, int mods) {
  UMSrequest req(UMSrequest::SEND_EVENT);
  req.writeEvent(ButtonPress, /*flow*/0, x, y, mouse_btn | mods);
  req.writeString(target);
  return sendBlock(req);
}

bool UMService::sendMouseRelease(const char* target,  
				 int x, int y, int mouse_btn, int mods) {
  UMSrequest req(UMSrequest::SEND_EVENT);
  req.writeEvent(ButtonRelease, /*flow*/0, x, y, mouse_btn | mods);
  req.writeString(target);
  return sendBlock(req);
}

bool UMService::sendMouseClick(const char* target, 
			       int x, int y, int mouse_btn, int mods) {
  return 
    sendMousePress(target, x, y, mouse_btn, mods)
    && sendMouseRelease(target, x, y, mouse_btn, mods);
}

/* ==================================================== [(c)Elc] ======= */
/* ==================================================== ======== ======= */

bool UMService::sendRequest(int request, const UStr& data) {
  return sendRequest(request, data.c_str());
}

bool UMService::sendRequest(int request, const char* data) {
  if (!data) data = "";    // data can be empty in this case
  UMSrequest req(request);
  req.writeString(data);
  return sendBlock(req);
}

/* ==================================================== ======== ======= */

bool UMService::sendMessage(const char* target, const UStr& message) {
  return sendMessage(target, message.c_str());
}

bool UMService::sendMessage(const char* target, const char* message) {
  // on ne doit retrourner false que si la comm deconne !
  //if (!message || !*message || !target || !*target) {
  //  UError::error("warning@UMSclient::sendMessage","empty target or message (not sent)");
  //  return false;
  //}
  UMSrequest req(UMSrequest::SEND_MESSAGE);
  req.writeString(target);
  req.writeString(message);
  return sendBlock(req);
}

}
/* ==================================================== [TheEnd] ======= */
/* ==================================================== [(c)Elc] ======= */

