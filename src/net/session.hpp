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
#ifndef SESSION_HPP
#define SESSION_HPP

#include "rtp.hpp"


/**
 * Session class
 * Structure of a RTP session
 */
class Session {

public:
  uint32_t	ssrc;		///< RTP ssrc id
  uint32_t	group;		///< multicast address
  uint16_t	rtp_port;	///< RTP port
  uint16_t	rtcp_port;	///< RTCP port
  uint8_t	ttl;		///< ttl
  uint8_t	mode;		///< WORLD, MANAGER
  rtp_hdr_t	rtp_hdr;	///< session RTP header
  uint16_t	rtp_seq;	///< RTP sequence number
  rtcp_sr_t	sr;		///< RTCP sender report
  class Source	*source;	///< informations on sources
  uint16_t	nbsources;	///< amount of sources
  SdesItem	*mysdes;	///< my own RTCP SDES
  Session	*next;		///< next session

  Session();			///< Constructor

  virtual ~Session();		///< Destructor

  virtual uint32_t create(uint32_t group, uint16_t port, uint8_t ttl, uint32_t ssrc);
  /**< creates a new session, called by channel */

  virtual void buildRtpHeader(rtp_hdr_t *pkt, uint32_t ssrc);
  /**< builds a RTP header */

  virtual void buildRTCPcommon(rtcp_common_t *prtcp_hdr, uint8_t pt);
  /**< Builds RTCP commun header */

  virtual int buildSR(rtcp_common_t *prtcp_hdr, uint8_t *pkt);
  /**< Builds a SR RTCP packet */

  virtual int buildSDES(rtcp_common_t *prtcp_hdr, uint8_t* pkt);
  /**<
   * Builds a SDES packet.
   * Fills rtcp buffer and return length.
   */

  virtual int buildBYE(rtcp_common_t *prtcp_hdr, uint8_t *pkt);
  /**< Builds a BYE packet */

  virtual int sendRTCPPacket(const struct sockaddr_in *to, uint8_t pt);
  /**< Sends a RTCP packet */

  virtual int sendSRSDES(const struct sockaddr_in *to);
  /**<  Sends a RTCP compound packet (SR + SDES) */

  virtual void deleteSourceBySsrc(uint32_t ssrc);

  virtual void incrSources();

  virtual void createMySdes();
  virtual void refreshMySdes();
  virtual void freeMySdes();
  virtual void dump();
  virtual void stat();

  static Session * current();
  /**< Gets the current session */

  static void deleteSessionBySsrc(uint32_t ssrc);
  static void clearList();
  static Session * getList();
  static void dumpAll();
};

#endif
