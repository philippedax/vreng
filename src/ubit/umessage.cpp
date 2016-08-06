/************************************************************************
 *
 *  umessage.cpp: Ubit Inter-Client Messages: see umservice.hpp 
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 2008 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <ubit/uon.hpp>
#include <ubit/ucall.hpp>
#include <ubit/ubox.hpp>
#include <ubit/uevent.hpp>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/umessage.hpp>
#include <ubit/umservice.hpp>
#include <ubit/umsproto.hpp>
#include <ubit/udispX11.hpp>
using namespace std;
namespace ubit {

/* ==================================================== ====== ======= */

UMessagePort::UMessagePort(const UStr& _name) : name(_name) {}

UMessagePort* UMessagePortMap::findMessagePort(const UStr& name) {
  MessMap::iterator k = mess_map.find(&name);
  if (k != mess_map.end()) return k->second;
  else return null;
}

UMessagePort& UMessagePortMap::getMessagePort(const UStr& name) {
  UMessagePort* msg = findMessagePort(name);
  if (!msg) {
    msg = new UMessagePort(name);
    mess_map[&msg->getName()] = msg;
  }
  return *msg;
}

/* ==================================================== ======== ======= */

void UMessagePortMap::fireMessagePort(const char* buf) {
  if (buf) {
    UStr name;
    const char* p = strchr(buf,' ');
    //if (p) {*p = 0; p++;}
    //name.append(buf);
    if (!p) name = buf;
    else {
      name.append(buf, p-buf);
      p++;
    }

    UMessagePort* port = findMessagePort(name);
    if (port) {
      if (p) port->value = p;  // A REVOIR: marche pas si multistrings ou binaires
      else port->value = "";

      //UMessageEvent e(UOn::message, null, null, null);  // a quoi sert message ?
      //e.setCond(UOn::action);
      //e.setSource(port);
      UMessageEvent e(UOn::action, port);
      port->fire(e);
    }
  }
}
  
/* ==================================================== ====== ======= */
#if !UBIT_WITH_X11

void UMessage::send(UHardwinImpl&, const char* message) {
  cerr << "UMessage::send not implemented " << endl;
}

#else  // - - - UBIT_WITH_X11 - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//ifdef UBIT_WITH_GDK
//typedef GdkEventClient ClientEvent;
//else
typedef XClientMessageEvent ClientEvent;
//endif

static void sendShortMessage(UDispX11* nd, UHardwinX11* nw, const char* message) {
  Atom UMS_MESSAGE = nd->getAtoms().UMS_MESSAGE;
  ClientEvent e;
  e.type = ClientMessage;
  e.serial = 0;
  e.send_event = false;	//true if this came from a SendEvent request

  // ici il y un pbm: ca doit etre le display de l'appli pas celui du mouse
  // driver. c'est OK pour XSendEvent (qui corrige) mais pas pour write
  // direct (il faut alors corriger a la reception)
  e.display = nd->getSysDisp();   // Display the event was read from
  e.window = nw->getSysWin();      //``event'' window it is reported relative to
  e.message_type = UMS_MESSAGE,   // property
  e.format = 8;                   // table of chars
  strcpy(e.data.b, message);

  long ev_mask = 0;
  XSendEvent(nd->getSysDisp(), nw->getSysWin(), False, ev_mask, (XEvent*)&e);
  XFlush(nd->getSysDisp());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 
static void sendLongMessage(UDispX11* nd, UHardwinX11* nw,
                            const char* message, int message_len) {
  Atom UMS_MESSAGE = nd->getAtoms().UMS_MESSAGE;

  // ATT: If there is insufficient space, a BadAlloc error results.
  XChangeProperty(nd->getSysDisp(), nw->getSysWin(),
                  UMS_MESSAGE,     // property
                  XA_STRING,       // type
                  8,               // format
                  // PropModeAppend: ajoute a la fin (NB: PropModeReplace remplacerait
                  // la valeur courante ce qui aurait pour effet de perdre des donnees
                  // si le callback du client est appele apres un 2e appel de XChangeProperty)
                  PropModeAppend,
                  (unsigned char*)message, message_len);
  
  XFlush(nd->getSysDisp()); // necessaire sinon notification differee  
  // a revoir: le cas ou les XChangeProperty sont trop proches et s'accumulent
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UMessage::send(UHardwinImpl& nw, const char* message) {
  if (!message) return;
  unsigned int len = strlen(message);
  UDisp* nd = UAppli::impl.disp;
  
  ClientEvent e;
  if (len < sizeof(e.data.b)) 
    sendShortMessage((UDispX11*)nd, (UHardwinX11*)&nw, message);
  else sendLongMessage((UDispX11*)nd,(UHardwinX11*)&nw, message, len);
}

void UMessage::send(UHardwinImpl& nw, const UStr& message) {
  if (message.empty()) return;
  unsigned int len = message.length();
  UDisp* nd = UAppli::impl.disp;

  ClientEvent e;
  if (len < sizeof(e.data.b)) 
    sendShortMessage((UDispX11*)nd, (UHardwinX11*)&nw, message.c_str());
  else sendLongMessage((UDispX11*)nd,(UHardwinX11*)&nw, message.c_str(), len);
}

#endif  // UBIT_WITH_X11


}
/* ==================================================== [TheEnd] ======= */

