//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-Paris (Ecole Nationale Superieure des Telecommunications)
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
// channel.hpp
//---------------------------------------------------------------------------
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <list>


/**
 * Channel class
 */
class Channel {
 private:
  static std::list<Channel*> channelList;

  void namingId();
  /**<
   * Channel naming : my_host_id, my_port_id, my_obj_id
   */

  int createMcast(struct sockaddr_in *sa);
  /**<
   * Creates a Multicast listen socket on the channel defined by group/port.
   * Returns sock else -1 if problem.
   */

  static void decodeChan(const char *chan, uint32_t *group, uint16_t *port, uint8_t *ttl);
  /**<
   * Decodes the string format "group[/port[/ttl]]".
   * Returns group, port, ttl.
   * If chan == "" or NULL, we get the default values.
   */

  void sendBYE();
  /**< Sends a BYE RTCP */

  void deleteFromList();
  /**< Deletes a channel from a list */

  static void clearList();
  /**< Clears channelList */

  void closeMcast();
  /**< Closes Multicast sockets */

  int joinGroup(int sock);
  /**<
   * Joins group (IP_ADD_MEMBERSHIP).
   * "group" in network format,
   * returns sock if OK, else -1.
   */

  int leaveGroup(int sock);
  /**<
   * Leaves group (IP_DROP_MEMBERSHIP).
   * Returns sock if OK, else -1.
   */

  static void initReflector();
  /**< Reflector Initialization */

  static void newChanStr(char *chan);

 public:
  uint32_t	ssrc;		///< ssrcid
  uint32_t	group;		///< group address
  uint16_t	port;		///< RTP port
  uint8_t	ttl;		///< ttl scope
  uint8_t	cntfds;		///< fd numbers
  int		sd[5];		///< sockets descriptors
  struct sockaddr_in * sa[5];	///< sockets addresses
  struct sockaddr_in sa_rtp;	///< RTP data
  struct sockaddr_in sa_rtcp;	///< RTCP data
  struct sockaddr_in sa_rtp_r;	///< RTP data reflector
  struct sockaddr_in sa_rtcp_r;	///< RTCP data reflector
  struct sockaddr_in sa_data;	///< UDP datas
  struct ip_mreq mreq;		///< IGMPv2
  class Session *session;	///< RTP session

  enum {
    MANAGER_MODE,
    WORLD_MODE
  };

  Channel();		/**< Constructor */
  virtual ~Channel();	/**< Destructor */

  void quit();
  /**< Quits a channel */

  static void init();
  /**< Network Initialization */

  int create(const char *chan, int **fds);
  /**<
   * Create a Channel
   * channel string is given by decode
   * return number of fd to survey
   *
   * Usage:
   * int *fds;
   * int count = createChannel(chan, &fds);
   * for (i=0; i < count; i++) {
   *   addThisFdToList(fds[i]);
   * }
   * create sockets mcast_recv_rtp and mcast_send_rtp
   *        sockets mcast_recv_rtcp and mcast_send_rtcp
   */

  static bool join(char *chan);
  /**<
   * Joins the channel and return the new channel string.
   * Sets the global variables: currentChannel, cntFd, tabFd
   */

  static bool joinManager(char *manager, const char *chan);
  /**<
   * Only for vreng client.
   * Sets the global variables: managerChannel, cntManagerFd, tabManagerFd.
   */

  static Channel * current();
  /**< Gets current channel instance */

  static Channel * getbysa(const struct sockaddr_in *sa);
  static int getFdSendRTP(const struct sockaddr_in *sa);
  static int getFdSendRTCP(const struct sockaddr_in *sa);
  static struct sockaddr_in * getSaRTCP(const struct sockaddr_in *sa);
  static int getfdbysa(const struct sockaddr_in *sa, int sd);
  static struct sockaddr_in * getsabysa(const struct sockaddr_in *sa, int sd);

//
// ChanStr
//
  static void getGroup(const char *chanstr, char *group);
  static uint16_t getPort(const char *chanstr);
  static uint8_t getTtl(const char *chanstr);
  static uint8_t currentTtl();
};

#endif
