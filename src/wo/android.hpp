//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
#ifndef ANDROID_HPP
#define ANDROID_HPP

#include "wobject.hpp"

#define ANDROID_TYPE 40
#define ANDROID_NAME "Android"


/**
 * Android class
 */
class Android: public WObject {

 private:
  // VAPS values
  static const uint16_t VAPS_BUFSIZ = 8192;	///< buffer line.
  static const uint16_t VAPS_PORT   = DEF_VAPS_PORT;	///< vaps port.

  float cloth[3];		///< cloth color.
  char *face_url;		///< face url.
  int sdtcp;			///< fd tcp.
  int sdudp;			///< fd udp.
  int ipmode;			///< IP mode.
  struct sockaddr_in tcpsa;	///< sa tcp.
  struct sockaddr_in udpsa;	///< sa udp.
  struct ip_mreq mreq;		///< mreq.
  bool usercontrol;		///< control by user.
  uint16_t vaps_port;		///< vaps server port.
  char vaps[64];		///< vaps server name.

 public:
  static const uint8_t VAPS_OFFSET_PORT = 10;	///< offset port.

  class Body * body;		///< body ptr.
  class Bap * bap;		///< bap ptr.

  char bapline[VAPS_BUFSIZ];	///< bap/fap line.

  enum {
    INACTIVE,
    PLAYING,
    LISTENING
  };

  enum {
    UNICAST,
    MULTICAST
  };

  static const OClass oclass;	///< class variable.

  virtual const OClass* getOClass() {return &oclass;} ///< instance method.

  static void funcs();		///< init funclist.

  Android(char *l);
  Android();
  /**< Constructor. */

  static WObject * (creator)(char *l);
  /**< Creates from fileline. */

  virtual void changePermanent(float lasting);
  /**< Changes permanent. */

  virtual void render();
  /**< Renders body and face. */

  virtual void quit();
  /**< Quits. */

  virtual int connectToBapServer(int _ipmode);
  /**< Establishes a TCP connection to the vaps server
   * and send the setup packet.
   * Sets sdtcp; return 1 if OK else return 0 if failed.
   */

  virtual int initReceiver();
  /**< Inits UDP listening. */

  virtual void sendPlayToBapServer(const char *bap_name);
  /**< Sends a play command to the vaps server. */

  virtual void disconnectFromBapServer();
  /**< Closes connection with the vaps server. */

private:
  virtual void parser(char *l);
  /**< Parses. */

  virtual void makeSolid();
  /**< Build geometry. */

  virtual void defaults();
  /**< Sets defaults. */

  virtual void behaviour();
  /**< Sets behaviour. */

  virtual void inits();
  /**< Makes some initializations. */

  virtual int readBapFrame();
  /**< Gets a frame from the vaps server. */

  virtual void reset();

  // GUI callbacks
  static void pause_cb(Android *android, void *d, time_t s, time_t u);
  static void hi_cb(Android *android, void *d, time_t s, time_t u);
  static void bye_cb(Android *android, void *d, time_t s, time_t u);
  static void ask_cb(Android *android, void *d, time_t s, time_t u);
  static void sit_cb(Android *android, void *d, time_t s, time_t u);
  static void show_cb(Android *android, void *d, time_t s, time_t u);
  static void clap_cb(Android *android, void *d, time_t s, time_t u);
  static void nak_cb(Android *android, void *d, time_t s, time_t u);
  static void test_cb(Android *android, void *d, time_t s, time_t u);
  static void eyes_cb(Android *android, void *d, time_t s, time_t u);
  static void joy_cb(Android *android, void *d, time_t s, time_t u);
  static void sad_cb(Android *android, void *d, time_t s, time_t u);
  static void surp_cb(Android *android, void *d, time_t s, time_t u);
  static void jag_cb(Android *android, void *d, time_t s, time_t u);
  static void reset_cb(Android *android, void *d, time_t s, time_t u);
};

#endif
