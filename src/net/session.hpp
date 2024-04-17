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
// session.hpp
//---------------------------------------------------------------------------
#ifndef SESSION_HPP
#define SESSION_HPP

#include "rtp.hpp"


/**
 * Session class
 * Structure of a RTP session
 */
class Session {
 private:
  uint32_t	ssrc;		///< RTP ssrc id
  uint32_t	group;		///< multicast address
  uint16_t	rtp_port;	///< RTP port
  uint16_t	rtcp_port;	///< RTCP port
  uint8_t	ttl;		///< ttl
  rtp_hdr_t	rtp_hdr;	///< session RTP header
  uint16_t	rtp_seq;	///< RTP sequence number
  SdesItem	*mysdes;	///< my own RTCP SDES

 public:
  uint8_t	mode;		///< WORLD, MANAGER
  rtcp_sr_t	sr;		///< RTCP sender report
  uint16_t	nbsources;	///< amount of sources
  class Session	*next;		///< next session
  class Source	*source;	///< informations on sources

  Session();			///< Constructor
  virtual ~Session();		///< Destructor

  uint32_t create(uint32_t group, uint16_t port, uint8_t ttl, uint32_t ssrc);
  /**< creates a new session, called by channel */

  int buildBYE(rtcp_common_t *prtcp_hdr, uint8_t *pkt);
  /**< Builds a BYE packet */

  int sendRTCPPacket(const struct sockaddr_in *to, uint8_t pt);
  /**< Sends a RTCP packet */

  int sendSRSDES(const struct sockaddr_in *to);
  /**<  Sends a RTCP compound packet (SR + SDES) */

  void buildRtpHeader(rtp_hdr_t *pkt, uint32_t ssrc);
  /**< builds a RTP header */

  void deleteSource(uint32_t ssrc);

  void incrSources();

  void refreshMySdes();

  void stat();

  static Session * current();
  /**< Gets the current session */

  static Session * getList();
  static void clearList();

 private:
  void buildRTCPcommon(rtcp_common_t *prtcp_hdr, uint8_t pt);
  /**< Builds RTCP commun header */

  int buildSR(rtcp_common_t *prtcp_hdr, uint8_t *pkt);
  /**< Builds a SR RTCP packet */

  int buildSDES(rtcp_common_t *prtcp_hdr, uint8_t* pkt);
  /**<
   * Builds a SDES packet.
   * Fills rtcp buffer and return length.
   */

  void createMySdes();
  void freeMySdes();
  void dump();
  static void deleteSession(uint32_t ssrc);
  static void dumpAll();
};

#endif
