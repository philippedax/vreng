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
#ifndef HUMANOID_HPP
#define HUMANOID_HPP

#include "wobject.hpp"

#define HUMANOID_TYPE 40
#define HUMANOID_NAME "Humanoid"


/**
 * Humanoid class
 */
class Humanoid: public WO {

 private:
  // VAPS values
  static const uint16_t VAPS_BUFSIZ = 8192;	///< buffer line.
  static const uint16_t VAPS_PORT   = DEF_VAPS_PORT;	///< vaps port.

  float cloth[3];		///< cloth color.
  char *head_url;		///< head url.
  int sdtcp;			///< fd tcp.
  int sdudp;			///< fd udp.
  int ipmode;			///< IP mode.
  struct sockaddr_in tcpsa;	///< sa tcp.
  struct sockaddr_in udpsa;	///< sa udp.
  struct ip_mreq mreq;		///< mreq.
  bool usercontrol;		///< control by user.
  uint16_t vaps_port;		///< vaps server port.
  char vaps[64];		///< vaps server name.
  char *bapfile;		///< content of bap file

 public:
  static const uint8_t VAPS_OFFSET_PORT = 10;	///< offset port.

  class Body *body;		///< body ptr.
  class Bap *bap;		///< bap ptr.
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

  const OClass* getOClass() {return &oclass;} ///< instance method.

  static void funcs();		///< init funclist.

  Humanoid(char *l);
  Humanoid();
  /**< Constructor. */

  static WO * (creator)(char *l);
  /**< Creates from fileline. */

  void changePermanent(float lasting);
  /**< Changes permanent. */

  void render();
  /**< Renders body and face. */

  void quit();
  /**< Quits. */

  void pause();

 private:
  int connectToBapServer(int _ipmode);
  /**< Establishes a TCP connection to the vaps server
   * and send the setup packet.
   * Sets sdtcp; return 1 if OK else return 0 if failed.
   */

  int initReceiver();
  /**< Inits UDP listening. */

  bool sendPlayToBapServer(const char *bap_name);
  /**< Sends a play command to the vaps server. */

  void disconnectFromBapServer();
  /**< Closes connection with the vaps server. */

  void parser(char *l);
  /**< Parses. */

  void geometry();
  /**< Build geometry. */

  void defaults();
  /**< Sets defaults. */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Makes some initializations. */

  int readBapFrame();
  /**< Gets a frame from the vaps server. */

  //char * toPlay(const char *str);

  void hi();
  void bye();
  void ask();
  void sit();
  void show();
  void clap();
  void nak();
  void test();
  void eyes();
  void joy();
  void sad();
  void surp();
  void jag();
  void reset();

  // GUI callbacks
  static void pause_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void hi_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void bye_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void ask_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void sit_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void show_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void clap_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void nak_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void test_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void eyes_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void joy_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void sad_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void surp_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void jag_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
  static void reset_cb(Humanoid *humanoid, void *d, time_t s, time_t u);
};

#endif
