/* ==================================================== ======== ======= *
 *
 *  umsproto.hpp: UMS (Ubit Mouse/Message Server) protocol
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

#ifndef _umsproto_hpp_
#define	_umsproto_hpp_
#include <ubit/udefs.hpp>
#include <ubit/usocket.hpp>
namespace ubit {

/** Ubit Protocol for the UMS (Ubit Mouse/Message Server)
 */

#define UMS_ZEROCONF_NAME    "_ums._tcp."
///< ZeroConf/Bonjour name of the UMS server.

#define UMS_PORT_NUMBER   9666
///< the default port used by the UMS server (available in directory ubit/ums).
  
#define UMS_EVENT_MASK    (1UL<<14)
/**< state field of XEvents sent by the UMS sever.
* if this bit is true in XEvent.state, the No of the mouse pointer
* is stored in XEvent.subwindow (see also: UEvent::getFlow())
*/
// NOTE: cette valeur est choisie pour eviter des interferences avec les Button*Mask
// et les Modifier*Mask. elle est < 1<<15 car cette valeur est definie par X
// et car les Event.xxx.state sont des unsigned int.  

#define UMS_WINDOW_PROPERTY   "_UBIT_WINDOW"
///< a X window that has this property will receive UMS events.

#define UMS_MESSAGE_PROPERTY  "_UBIT_MESSAGE"
///< the property for exchanging messages between applications and the UMS server.

/* ==================================================== ===== ======= */
/** UMS Request (from a Client to the UMS server).
 * <pre>
 * ----------------------------------------------------------------------
 * reqtype(uchar) | arguments(c = uchar / c* = char* / l = long)
 * ----------------------------------------------------------------------
 * KEY_MOUSE_CTRL (c)evtype (c)evflow (l)x (l)y (l)btn|key|mvm
 * SEND_EVENT     (c)evtype (c)evflow (l)x (l)y (l)btn|key|mvm (c*)target
 * SEND_MESSAGE   (c*)target (c*)message
 * SERVER_REQUEST (c*)request
 * ...
 * ----------------------------------------------------------------------
 * target identifies a window as follows:
 * - decimal (1234) or hexa (0x1234a) number of the window ID
 * - ascii-name-without-blanks of the window
 * - quoted 'asci name' of the window (if the name contains spaces)
  * </pre>
 */
struct UMSrequest : public UOutbuf {
  enum RequestType { 
    KEY_MOUSE_CTRL    = 1,   
    SEND_EVENT        = 2,   
    SEND_MESSAGE      = 3,
    SET_NEIGHBOR      = 8,
    CALIBRATE         = 9,
    BROWSE_NEIGHBORS  = 10,
    BROWSE_SERVERS    = 11,
    RESOLVE_SERVER    = 12,
    RESOLVE_WINDOW    = 13,
    BROWSE_WINDOWS    = 14,
    GET_SCREEN_SIZE   = 20,
    MOVE_BACK_POINTER = 30,
    FILE_TRANSFERT    = 33,  //aka '!'
    OPEN_DEVICE       = 40,
    CLOSE_DEVICE      = 41,
    TACTOS            = 50,
    REQUEST_COUNT/*NO COMMA*/
  };
  UMSrequest(unsigned char reqtype) {buffer[outpos++] = reqtype;}
};

}
#endif
