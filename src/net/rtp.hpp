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
// rtp.hpp  --  RTP header file
//
// Real Time Protocol (RFC 1890)
//---------------------------------------------------------------------------
#ifndef RTP_HPP
#define RTP_HPP

#include "config.h"


#if WORDS_BIGENDIAN
#define RTP_BIG_ENDIAN 1
#else
#define RTP_LITTLE_ENDIAN 1
#endif

/*
 * Current protocol version.
 */
#define RTP_VERSION 2

#define RTP_SEQ_MOD (1<<16)
#define RTP_MAX_SDES 255	/* maximum text length for SDES */

#define RTP_TS_MOD  (0xffffffff)

/* Offset from UNIX's epoch to the NTP epoch in seconds (NTP's JAN_1970) */
#define RTP_EPOCH_OFFSET        2208988800UL

#define RTP_M   0x0080  /* Marker: significant event <e.g. frame boundary> */
#define RTP_P   0x2000  /* Padding is present */
#define RTP_X   0x1000  /* Extension Header is present */

/*
 * Parameters controling the RTCP report rate timer.
 */
#define RTCP_SESSION_BW_FRACTION (0.05)
#define RTCP_MIN_RPT_TIME (5.)
#define RTCP_SENDER_BW_FRACTION (0.25)
#define RTCP_RECEIVER_BW_FRACTION (1. - RTCP_SENDER_BW_FRACTION)
#define RTCP_SIZE_GAIN (1./8.)

/*
 * Largest (user-level) packet size generated by our rtp applications.
 * Individual video formats may use smaller mtu's.
 */
#define RTP_MTU 1024

#define MAXHDR 24

typedef enum {
  RTCP_SR   = 200,
  RTCP_RR   = 201,
  RTCP_SDES = 202,
  RTCP_BYE  = 203,
  RTCP_APP  = 204
} rtcp_type_t;

typedef enum {
  RTCP_SDES_END    =  0,
  RTCP_SDES_CNAME  =  1,
  RTCP_SDES_NAME   =  2,
  RTCP_SDES_EMAIL  =  3,
  RTCP_SDES_PHONE  =  4,
  RTCP_SDES_LOC    =  5,
  RTCP_SDES_TOOL   =  6,
  RTCP_SDES_NOTE   =  7,
  RTCP_SDES_PRIV   =  8,
  RTCP_SDES_IMG    =  9,
  RTCP_SDES_DOOR   = 10,
  RTCP_SDES_SOURCE = 11
} rtcp_sdes_type_t;

/**
 * RTP data header
 */
typedef struct {
#if RTP_BIG_ENDIAN
  unsigned int version:2;  // protocol version
  unsigned int p:1;        // padding flag
  unsigned int x:1;        // header extension flag
  unsigned int cc:4;       // CSRC count
  unsigned int m:1;        // marker bit
  unsigned int pt:7;       // payload type
#elif RTP_LITTLE_ENDIAN
  unsigned int cc:4;       // CSRC count
  unsigned int x:1;        // header extension flag
  unsigned int p:1;        // padding flag
  unsigned int version:2;  // protocol version
  unsigned int pt:7;       // payload type
  unsigned int m:1;        // marker bit
#else
#error Define one of RTP_LITTLE_ENDIAN or RTP_BIG_ENDIAN
#endif
  uint16_t seq;             // sequence number
  uint32_t ts;              // timestamp
  uint32_t ssrc;            // synchronization source
  uint32_t csrc[1];         // optional CSRC list
} rtp_hdr_t;

/**
 * RTCP common header word
 */
typedef struct {
#if RTP_BIG_ENDIAN
  unsigned int version:2;  // protocol version
  unsigned int p:1;        // padding flag
  unsigned int count:5;    // varies by payload type
#elif RTP_LITTLE_ENDIAN
  unsigned int count:5;    // varies by packet type
  unsigned int p:1;        // padding flag
  unsigned int version:2;  // protocol version
#else
#error Define one of RTP_LITTLE_ENDIAN or RTP_BIG_ENDIAN
#endif
  unsigned int pt:8;       // RTCP packet type
  uint16_t length;         // packet length in words, without this word
} rtcp_common_t;

/*
 * Big-endian mask for version, padding bit and packet type pair
 */
#define RTCP_VALID_MASK     (0xc000 | 0x2000 | 0xfe)
#define RTCP_VALID_VALUE    ((RTP_VERSION << 14) | RTCP_SR)

/**
 * Reception report block
 */
typedef struct {
  uint32_t ssrc;           // data source being reported
  unsigned int fraction:8; // fraction lost since last SR/RR
  int lost:24;             // cumulative number of packets lost (signed!)
  uint32_t last_seq;       // extended last sequence number received
  uint32_t jitter;         // interarrival jitter
  uint32_t lsr;            // last SR packet from this source
  uint32_t dlsr;           // delay since last SR packet
} rtcp_rr_t;

/**
 * SDES item
 */
typedef struct {
  uint8_t type;		// type of SDES item (rtcp_sdes_type_t)
  uint8_t length;	// length of SDES item (in octets)
  char data[1];		// text, not zero-terminated
} rtcp_sdes_item_t;

struct rtcp_sr {
  uint32_t ssrc;	// source this RTCP packet refers to
  uint32_t ntp_sec;	// NTP timestamp
  uint32_t ntp_frac;
  uint32_t rtp_ts;	// RTP timestamp
  uint32_t psent;	// packets sent
  uint32_t osent;	// octets sent
  rtcp_rr_t rr[1];	// variable-length list
};
typedef struct rtcp_sr rtcp_sr_t;

struct rtcp_rr {
  uint32_t ssrc;	// source this generating this report
  rtcp_rr_t rr[1];	// variable-length list
};

typedef struct rtcp_sdes rtcp_sdes_t;
struct rtcp_sdes {
  uint32_t src;			// first SSRC/CSRC
  rtcp_sdes_item_t item[1];	// list of SDES items
};

struct rtcp_bye {
  uint32_t src[1];	// list of sources
  /* can't express trailing text for reason */
};

/**
 * One RTCP packet
 */
typedef struct {
  rtcp_common_t common;		// common header
  union {
    struct rtcp_sr sr;		// sender report (SR)
    struct rtcp_rr rr;		// reception report (RR)
    struct rtcp_sdes sdes;	// source description (SDES)
    struct rtcp_bye bye;	// BYE
  } r;
} rtcp_t;

/**
 * Per-source state information
 * network infos
 */
typedef struct {
  uint16_t max_seq;		// highest seq. number seen
  uint32_t cycles;		// shifted count of seq. number cycles
  uint32_t base_seq;		// base seq number
  uint32_t bad_seq;		// last 'bad' seq number + 1
  uint32_t probation;		// sequ. packets till source is valid
  uint32_t received;		// packets received
  uint32_t expected_prior;	// packet expected at last interval
  uint32_t received_prior;	// packet received at last interval
  uint32_t transit;		// relative trans time for prev packet
  uint32_t jitter;		// estimated jitter
} sourceInfos;

typedef struct {
  uint32_t upper;	// more significant 32 bits
  uint32_t lower;	// less significant 32 bits
} ntp64;


/**
 * SDES item
 */
typedef struct _sitem {
  uint8_t	si_type;
  uint8_t	si_len;
  uint8_t 	*si_str;
  struct _sitem *si_next;
} SdesItem;


uint32_t md_32(char *string, int length);

/*
 * Return random unsigned 32-bit quantity. Use 'type' argument if you
 * need to generate several different values in close succession.
 */
uint32_t random32(int type);


/**
 * Rtp class
 */
class Rtp {
 private:
  static bool isSdesType(uint8_t sdes_type);

 public:
  static const uint8_t RTPNAME_LEN = 128;
  static const uint8_t EMAIL_LEN = 255;
  static const uint8_t TOOL_LEN = 16;

  static int sendPacket(int fd, const uint8_t *pkt, int pkt_len, const struct sockaddr_in *to);
  /**<
   * Sends a packet
   * Returns result of sendto(): -1 if error, else >=0
   */

  static int recvRTCPPacket(struct sockaddr_in *from, uint8_t *pkt, int pkt_len);
  /**< Receive a RTCP packet */

  static uint16_t createSeq();
  static uint32_t createSsrc(int value);
  static SdesItem * allocSdesItem();
  static void initSource(sourceInfos *s, uint16_t seq);
  static int updateSeq(sourceInfos *s, uint16_t seq);
  static const char * getRtpName(char *name);
  static void getRtcpName(char *name);
  static void getRtcpEmail(char *email);
  static void getRtcpTool(char *tool);
};

#endif
