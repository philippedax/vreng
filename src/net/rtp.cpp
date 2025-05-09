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
// rtp.cpp
//
// Realtime Transport Protocol
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "rtp.hpp"
#include "md5.hpp"	// from RFC 1321
#include "channel.hpp"	// Channel
#include "nsl.hpp"	// my_gethostbyname
#include "session.hpp"	// Session
#include "source.hpp"	// Source
#include "stat.hpp"	// statSendPacket
#include "file.hpp"	// open, close


/****************************
 * Handling RTP packet header
 */

uint16_t Rtp::createSeq()
{
  time_t t;

  srand(time(&t));
  return rand();
}

uint32_t Rtp::createSsrc(int value)
{
  time_t t;

  srand(time(&t));
  return random32(value);
}

void Rtp::initSource(sourceInfos *s, uint16_t seq)
{
  s->base_seq = seq - 1;
  s->max_seq = seq;
  s->bad_seq = RTP_SEQ_MOD + 1;
  s->cycles = 0;
  s->received = 0;
}

int Rtp::updateSeq(sourceInfos *s, uint16_t seq)
{
  uint16_t udelta = seq - s->max_seq;
  const int MAX_DROPOUT = 3000;
  const int MAX_MISORDER = 100;
  const int MIN_SEQUENTIAL = 2;

  /*
   * Source is not valid until MIN_SEQUENTIAL packets with
   * sequential sequence numbers have been received.
   */
  if (s->probation) {
    // packet is in sequence
    if (seq == s->max_seq + 1) {
      s->probation--;
      s->max_seq = seq;
      if (s->probation == 0) {
	initSource(s, seq);
	s->received++;
	return 1;
      }
    }
    else {
      s->probation = MIN_SEQUENTIAL - 1;
      s->max_seq = seq;
    }
    return 0;
  }
  else if (udelta < MAX_DROPOUT) {
    // in order, with permissible gap
    if (seq < s->max_seq) {
      // Sequence number wrapped - count another 64K cycle.
      s->cycles += RTP_SEQ_MOD;
    }
    s->max_seq = seq;
  }
  else if (udelta <= RTP_SEQ_MOD - MAX_MISORDER) {
    // the sequence number made a very large jump
    if (seq == s->bad_seq) {
      // Two sequential packets -- assume that the other side
      // restarted without telling us so just re-sync
      // (i.e., pretend this was the first packet).
      initSource(s, seq);
    }
    else {
      s->bad_seq = (seq + 1) & (RTP_SEQ_MOD-1);
      return 0;
    }
  }
  else {
    // duplicate or reordered packet
  }
  s->received++;
  return 1;
}

/*
 * Handling RTCP SDES
 */

/** Returns name */
const char * Rtp::getRtpName(char *name)
{
  FILE *fp;
  char *p, line[128];

  p = getenv("HOME");
  if (! p || *p == '\0') return NULL;
  sprintf(line, "%s/.RTPdefaults", p);
  File *file = new File();
  if ((fp = file->open(line, "r"))) {
    while (fgets(line, sizeof(line), fp)) {
      line[strlen(line) -1] = '\0';
      if (strncmp(line, "*Name:", 9) == 0) {
        p = strchr(line, ':');
        p += 2;
        strcpy(name, p);
        file->close();
        delete file;
        return name;
      }
    }
    file->close();
    delete file;
  }
  return NULL;
}

/** Returns email */
void Rtp::getRtcpEmail(char *email)
{
  char mail[EMAIL_LEN+10], hostfqdn[255], hostname[MAXHOSTNAMELEN];

  gethostname(hostname, sizeof(hostname)-1);

  struct hostent *ph = my_gethostbyname_r(hostname, AF_INET);
  if (ph) {
    strcpy(hostfqdn, ph->h_name);
    my_free_hostent(ph);
  }
  else {
    strcpy(hostfqdn, hostname);
  }

#if HAVE_GETPWUID
  struct passwd *pwd;

  if ((pwd = getpwuid(getuid())))
    sprintf(mail, "%s@%s", pwd->pw_name, hostfqdn);
  else
    sprintf(mail, "unknown@%s", hostfqdn);
#else
    sprintf(mail, "unknown@%s", hostfqdn);
#endif
  strcpy(email, mail);
}

/** Returns rtcpname */
void Rtp::getRtcpName(char *rtcpname)
{
  const char *p;
  char name[RTPNAME_LEN];

  memset(name, 0, sizeof(name));
  if (! (p = getRtpName(name)))
    getRtcpEmail(rtcpname);
  else {
    strcpy(rtcpname, name);
  }
}

/** Returns tool */
void Rtp::getRtcpTool(char *tool)
{
  sprintf(tool, "%s-%s", PACKAGE_NAME, PACKAGE_VERSION);
}

bool Rtp::isSdesType(uint8_t sdes_type)
{
  switch (sdes_type) {
  case RTCP_SDES_CNAME:
  case RTCP_SDES_NAME:
  case RTCP_SDES_EMAIL:
  case RTCP_SDES_LOC:
  case RTCP_SDES_TOOL:
    return true;
  }
  return false;
}

SdesItem * Rtp::allocSdesItem()
{
  return new SdesItem[1];
}

/*
 * Handling packet IO
 */

/**
 * Sends a packet
 */
int Rtp::sendPacket(int sd, const uint8_t *pkt, int pkt_len, const struct sockaddr_in *to)
{
  if (sd < 0) {
    error("sendPacket: sd=%d", sd);
    return -1;
  }
  if (sendto(sd, pkt, pkt_len, 0, (struct sockaddr*) to, sizeof(*to)) < 0) {
    if (errno == EHOSTUNREACH) {
      error("sendto: %s (%d) len=%d", strerror(errno), errno, pkt_len);
    }
    return -1;
  }
  statSendPacket(pkt_len);
  return 0;
}

/**
 * Receives a RTCP packet
 */
int Rtp::recvRTCPPacket(struct sockaddr_in *from, uint8_t *pkt, int pkt_len)
{
  rtcp_common_t rtcp_hdr;
  rtcp_t *end = NULL;
#ifdef CHECK_RTCP_VALIDITY
  rtcp_t *r;
#endif
  uint16_t length = 0;
  uint32_t ssrc = 0;
  Channel *pchan = Channel::current(); // maybe indeterminated

  // we are supposed to receive compounds RTCP packets
  for (int len = 0; len < pkt_len ; ) {
    memcpy(&rtcp_hdr, pkt+len, sizeof(rtcp_common_t));
    length = ntohs(rtcp_hdr.length);
    end = (rtcp_t *) ((uint32_t *) &rtcp_hdr + (length<<2) + sizeof(rtcp_common_t));
    len += sizeof(rtcp_common_t);

#ifdef CHECK_RTCP_VALIDITY
    // Check RTCP validity
    r = (rtcp_t *) &rtcp_hdr;
    do {
      r = (rtcp_t *) ((uint32_t *) r + ntohs(r->common.length + sizeof(rtcp_common_t)));
    }
    while (r < end && r->common.version == RTP_VERSION);
    if (r != end) {
      error("RTCP wrong size: r=%x end=%x length=%d pkt_len=%d sizeof(rtcp_common_t)=%d", r, end, length<<2, pkt_len, sizeof(rtcp_common_t));
      len += (length << 2);
      continue;
    }
#endif

    switch (rtcp_hdr.pt) {

     // receives a SR
      case RTCP_SR: {
        rtcp_sr_t sr;
        Source *pso;

        trace1(DBG_RTP, "Got SR: length=%d len=%d", length<<2, len);
        memcpy(&sr, pkt+len, sizeof(rtcp_sr_t));
        if ((pso = Source::getSource(ntohl(sr.ssrc)))) {
          pso->sr = sr;
        }
        len += (length << 2);
      }
      break;

      // receives a SDES
      case RTCP_SDES: {
        SdesItem *sitem = NULL;
        Source *pso;
        uint8_t *p = pkt + len;

        end = (rtcp_t *) ((uint32_t *) p + (length<<2) + sizeof(rtcp_common_t));
        memcpy(&ssrc, p, sizeof(ssrc));
        p += sizeof(ssrc);
        trace1(DBG_RTP, "Got SDES: ssrc=%x pkt_len=%d length=%d len=%d p=%x",
              ntohl(ssrc), pkt_len, length<<2, len, p);

        if ((pso = Source::getSource(ntohl(ssrc)))) { // source found

          sitem = &pso->sdes;	// first sitem which ever exists

          // parses SDES chunks
          while (p < (uint8_t *) end) {
            trace1(DBG_RTP, "SDES: sitem=%p p=%p end=%p", sitem, p, end);
            sitem->si_type = *p++;
            sitem->si_len = *p++;
            if (! sitem->si_str) {	// asumption
              /* alloc string space, *FIX: memory leak no delete[] ! */
              sitem->si_str = new uint8_t[sitem->si_len + 1];
            }
            memcpy(sitem->si_str, p, sitem->si_len);
            sitem->si_str[sitem->si_len] = 0; // now string null terminated
            trace1(DBG_RTP, "SDES: %s [%d]", sitem->si_str, sitem->si_len);
            p += sitem->si_len;	// next item
            if (*p == RTCP_SDES_END) {
              trace1(DBG_RTP, "end SDES: p=%p end=%p", p, end);
              break;	// end of SDES packet
            }
            if (! sitem->si_next) {
              /* alloc next item */
              trace1(DBG_RTP, "alloc new item");
              sitem->si_next = new SdesItem[1];
              sitem->si_next->si_type = 0;
              sitem->si_next->si_len = 0;
              sitem->si_next->si_str = NULL;
              sitem->si_next->si_next = NULL;
            }
            sitem = sitem->si_next;
            trace1(DBG_RTP, "SDES: sitem=%p", sitem);
          }
        }
        len += (length << 2);
      }
      break;

     // receives a BYE
      case RTCP_BYE: {
        memcpy(&ssrc, pkt+len, sizeof(ssrc));
        trace1(DBG_RTP, "Got BYE: ssrc=%x", ntohl(ssrc));
        if (pchan)
          pchan->session->deleteSource(ntohl(ssrc));
        len += (length << 2);
      }
      break;

     // receives a APP
      case RTCP_APP:
        trace1(DBG_RTP, "Got APP");

     // receives something unknown
      default:
        len += (length << 2);
        error("Got RTCP unknown type: pt=%u, pkt_len=%d len=%d",
                         rtcp_hdr.pt, pkt_len, len);
        trace1(DBG_FORCE, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
                         pkt[0], pkt[1], pkt[2], pkt[3],
                         pkt[4], pkt[5], pkt[6], pkt[7],
                         pkt[8], pkt[9], pkt[10], pkt[11]);
        return -3;
    }
  }
  return 0;
}

/**
 * Generate a random 32-bit value
 *
 * RFC 1889
 */
uint32_t md_32(char *str, int len)
{
  MD5_CTX context;
  union {
    char  c[16];
    uint32_t  x[4];
  } digest;
  uint32_t r = 0;

  MD5Init(&context);
  MD5Update(&context, (uint8_t *) str, len);
  MD5Final((unsigned char *)&digest, &context);
  for (int i = 0; i < 3; i++) {
    r ^= digest.x[i];
  }
  return r;
}

/**
 * Returns random unsigned 32-bit value. Use 'type' argument if you
 * need to generate several different values in close succession.
 */
uint32_t random32(int type)
{
  struct {
    int			type;
    struct timeval	tv;
    clock_t		cpu;
    pid_t		pid;
    uint32_t		hid;
#if HAVE_GETUID
    uid_t		uid;
    gid_t		gid;
#endif
#if HAVE_UNAME
    struct utsname	name;
#endif
  } s;

  gettimeofday(&s.tv, 0);
#if HAVE_UNAME
  uname(&s.name);
#endif
  s.type = type;
  s.cpu  = clock();
  s.pid  = getpid();
#if HAVE_GETHOSTID
  s.hid  = gethostid();
#else
  s.hid  = 1;
#endif
#if HAVE_GETUID
  s.uid  = getuid();
  s.gid  = getgid();
#endif

  return md_32(reinterpret_cast<char *>(&s), sizeof(s));
}
