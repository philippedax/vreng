//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// session.cpp
//
// Session handling (RTCP)
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "session.hpp"
#include "netobj.hpp"	// NetObj
#include "vrep.hpp"	// RTP_VREP_TYPE
#include "channel.hpp"	// Channel
#include "source.hpp"	// Source
#include "rtp.hpp"	// Rtp
#include "universe.hpp"	// MANAGER_NAME
#include "world.hpp"	// current
#include "stat.hpp"	// statSendRTCP setLostPackets


// local
static Session *sessionList;		// RTP sessions list
static Session *currentSession = NULL;	// RTP current session

static char rtcp_name[Rtp::RTPNAME_LEN]; // rtcp name
static char rtcp_email[Rtp::EMAIL_LEN];  // rtcp email
static char rtcp_tool[Rtp::TOOL_LEN];    // tool name


// Macro for debug
#define CHECK_SESSION_LIST \
  { Session *sess = sessionList; \
    while (sess) { \
      if (sess->next == sess) { \
        error("RtpSession list invalid at %s:%d", __FILE__, __LINE__); \
        break; \
      } \
      sess = sess->next; \
    } \
  }

/*
 * Handling Sessions
 */
void Session::buildRtpHeader(rtp_hdr_t *rtp_hdr, uint32_t _ssrc)
{
  struct timeval ts;

  rtp_hdr->version = RTP_VERSION;
  rtp_hdr->p = 0;
  rtp_hdr->x = 0;
  rtp_hdr->cc = 0;
  rtp_hdr->m = 0;
  rtp_hdr->pt = RTP_VREP_TYPE;
  rtp_hdr->seq = htons(++(rtp_seq));
  gettimeofday(&ts, NULL);
  rtp_hdr->ts = htonl(ts.tv_sec*1000 + ts.tv_usec/1000);
  rtp_hdr->ssrc = htonl(_ssrc);

#ifdef DEBUG
  echo("RTP: %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x", hdr[0], hdr[1], hdr[2], hdr[3], hdr[4], hdr[5], hdr[6], hdr[7], hdr[8], hdr[9], hdr[10], hdr[11]);
#endif
}

/**
 * creates a new Session
 */
Session::Session()
{
  new_session++;
  group = 0;
  rtp_port = 0;
  rtcp_port = 0;
  ttl = 0;
  mode = 0;
  nbsources = 0;
  source = NULL;
  mysdes  = NULL;
  next  = NULL;
  sr.psent = 0;
  sr.osent = 0;

  if (! sessionList) {
    sessionList = this;
    next = NULL;
  }
  else {
    next = sessionList;
    sessionList = this;
  }
  currentSession = this;
}

Session * Session::current()
{
  return currentSession;
}

void Session::clearList()
{
  sessionList = NULL;
}

Session * Session::getList()
{
  return sessionList;
}

void Session::incrSources()
{
  nbsources = Source::incrSourcesNumber();
}

/**
 * initialize variables for a session already allocated, return local ssrc
 */
uint32_t Session::create(uint32_t _group, uint16_t _rtp_port, uint8_t _ttl, uint32_t _ssrc)
{
  group = htonl(_group);
  rtp_port = htons(_rtp_port);
  rtcp_port = htons(_rtp_port + 1);
  ttl = _ttl;
  //echo("Session: rtp_port=%x rtcp_port=%x", rtp_port, rtcp_port);

  /* seq number initialization */
  rtp_seq = Rtp::createSeq();

  /* SSRC number initialization */
  if (_ssrc)
    ssrc = _ssrc;	// ssrc already used
  else
    ssrc = htonl(Rtp::createSsrc(rtp_seq));	// new ssrc

  //echo("Session: ssrc=%x", ssrc);
  rtp_hdr.ssrc = ssrc;
  sr.ssrc = ssrc;

  /* alloc Source */
  source = new Source(ssrc);

  nbsources = source->incrSourcesNumber();
  //echo("Session: nbsources=%d", nbsources);

  Rtp::initSource(&source->s, rtp_seq);
  createMySdes();

  return ssrc;
}

void Session::deleteSession(uint32_t _ssrc)
{
  //dax CHECK_SESSION_LIST
  for (Session *pse = sessionList; pse; pse = pse->next) {
    for (Source *pso = pse->source; pso; pso = pso->next) {
      if (pso->ssrc == _ssrc) {
        pse->deleteSource(_ssrc);
        return;
      }
    }
  }
  //echo("deleteSession: ssrc=%x not found", _ssrc);
}

void Session::deleteSource(uint32_t _ssrc)
{
  Source *pso, *psolast;
  int i = 0;

  for (psolast = pso = source; pso && (i < nbsources); pso = pso->next, i++) {
    if (pso->ssrc == _ssrc) {
      if (psolast == NULL) {	// no source found
        source = NULL;
      }
      else {
        // MS : this is a NOOP if pso == source
        // Bad things happen after that
        // psolast->next = pso->next;
        if (pso == source) {
          source = pso->next;
        }
        else {
          psolast->next = pso->next;
        }
      }
      setLostPackets(pso->lost);
      nbsources = Source::decrSourcesNumber();
      //echo("nbsources--=%d", nbsources);
      delete pso;		// delete Source
      pso = NULL;
      break;
    }
    psolast = pso;
  }
}

/**
 * Session destructor
 */
Session::~Session()
{
  del_session++;
  freeMySdes();
  deleteSource(NetObj::getSsrc());
}

/**
 * creates my SDES item
 */
void Session::createMySdes()
{
  SdesItem *scname, *sname, *semail, *stool, *sloc;

  Rtp::getRtcpName(rtcp_name);		// fill rtcp Name
  Rtp::getRtcpEmail(rtcp_email);	// fill rtcp Email
  Rtp::getRtcpTool(rtcp_tool);		// fill rtcp Tool
  //echo("createMySdes: name=%s, email=%s, pse=%p", rtcp_name, rtcp_email, this);

  if ((scname = Rtp::allocSdesItem()) == NULL) return;
  mysdes = scname;
  scname->si_type = RTCP_SDES_CNAME;
  scname->si_len = strlen(rtcp_email);
  scname->si_str = (uint8_t *) rtcp_email;

  if ((sname = Rtp::allocSdesItem()) == NULL) return;
  scname->si_next = sname;
  sname->si_type = RTCP_SDES_NAME;
  sname->si_len = strlen(rtcp_name);
  sname->si_str = (uint8_t *) rtcp_name;

  if ((semail = Rtp::allocSdesItem()) == NULL) return;
  sname->si_next = semail;
  semail->si_type = RTCP_SDES_EMAIL;
  semail->si_len = strlen(rtcp_email);
  semail->si_str = (uint8_t *) rtcp_email;

  if ((sloc = Rtp::allocSdesItem()) == NULL) return;
  semail->si_next = sloc;
  sloc->si_type = RTCP_SDES_LOC;
  if (World::current() && World::current()->getName()) {
    sloc->si_len = strlen(World::current()->getName());
    sloc->si_str = (uint8_t *) World::current()->getName();
  }
  else {
    sloc->si_len = strlen(MANAGER_NAME);
    sloc->si_str = (uint8_t *) MANAGER_NAME;
  }

  if ((stool = Rtp::allocSdesItem()) == NULL) return;
  sloc->si_next = stool;
  stool->si_type = RTCP_SDES_TOOL;
  stool->si_len = strlen(rtcp_tool);
  stool->si_str = (uint8_t *) rtcp_tool;

  stool->si_next = NULL;
}

/**
 * refreshes my SDES item
 */
void Session::refreshMySdes()
{
  SdesItem *scname, *sname, *semail, *sloc;

  //echo("refreshMySdes: pse=%p", this);

  if ((scname = mysdes) == NULL) return;
  if ((sname = scname->si_next) == NULL) return;
  if ((semail = sname->si_next) == NULL) return;
  if ((sloc = semail->si_next) == NULL) return;
  sloc->si_type = RTCP_SDES_LOC;
  if (World::current()->getName()) {
    sloc->si_len = strlen(World::current()->getName());
    sloc->si_str = (uint8_t *) World::current()->getName();
  }
  else {
    sloc->si_len = strlen(MANAGER_NAME);
    sloc->si_str = (uint8_t *) MANAGER_NAME;
  }
  //echo("sloc: %p %s", sloc, sloc->si_str);
}

void Session::freeMySdes()
{
  SdesItem *sitem, *tmp;
  for (sitem = mysdes; sitem; ) {
    tmp = sitem;
    sitem = sitem->si_next;
    delete[] tmp;
  }
  mysdes = NULL;
}

/**
 * Builds common part of rtcp packet
 */
void Session::buildRTCPcommon(rtcp_common_t *prtcp_hdr, uint8_t pt)
{
  memset(prtcp_hdr, 0, sizeof(rtcp_common_t));	// clear P:1, count:5
  prtcp_hdr->version = RTP_VERSION;
  prtcp_hdr->pt = pt;
}

/**
 * Builds a SR packet
 */
int Session::buildSR(rtcp_common_t *prtcp_hdr, uint8_t *pkt)
{
  int len = 0;

  buildRTCPcommon(prtcp_hdr, RTCP_SR);
  prtcp_hdr->count = 0;	// only one SSRC
  sr.ssrc = htonl(NetObj::getSsrc());

  struct timeval ts;
  gettimeofday(&ts, NULL);
  sr.rtp_ts = htonl(ts.tv_sec*1000 + ts.tv_sec/1000);
  prtcp_hdr->length = htons(sizeof(rtcp_sr_t) >>2);
  memcpy(pkt, prtcp_hdr, sizeof(rtcp_common_t));
  len += sizeof(rtcp_common_t);
  memcpy(pkt+len, &sr, sizeof(rtcp_sr_t));
  len += sizeof(rtcp_sr_t);

  //echo("setSR: ssrc=%x len=%d", sr.ssrc, len);
  return len;
}

/**
 * Builds a SDES packet
 */
int Session::buildSDES(rtcp_common_t *prtcp_hdr, uint8_t* pkt)
{
  uint8_t xitem = 0;
  int len = 0;
  uint8_t items[PKTSIZE-sizeof(rtcp_sr_t)-sizeof(rtcp_common_t)-sizeof(ssrc)];
  SdesItem *pchunk;

  buildRTCPcommon(prtcp_hdr, RTCP_SDES);
  memset(items, 0, sizeof(items));
  prtcp_hdr->count = 1; // Only one SDES with multiple chunks

  /* fill chunks */
  for (pchunk = mysdes; pchunk; pchunk = pchunk->si_next) {
    items[xitem++] = pchunk->si_type;
    items[xitem++] = pchunk->si_len;
    memcpy(items+xitem, pchunk->si_str, pchunk->si_len);
    xitem += pchunk->si_len;
  }
  items[xitem++] = RTCP_SDES_END; // end of chunks is indicated by a zero

  while (xitem % 4) {	// Pad to 4 bytes word boundary and store zeros there
    items[xitem++] = 0;
  }

  /* SDES header */
  prtcp_hdr->length = htons(1 + (xitem >> 2));
  memcpy(pkt, prtcp_hdr, sizeof(rtcp_common_t));
  len += sizeof(rtcp_common_t);
  memcpy(pkt+len, &ssrc, sizeof(ssrc));
  len += sizeof(ssrc);
  memcpy(pkt+len, items, xitem);
  len += xitem;

  //echo("setSDES: ssrc=%x len=%d xitem=%d", ssrc, len, xitem);
  return len;
}

/**
 * Builds a BYE packet
 */
int Session::buildBYE(rtcp_common_t *prtcp_hdr, uint8_t *pkt)
{

  buildRTCPcommon(prtcp_hdr, RTCP_BYE);
  prtcp_hdr->count = 1;	// me only says bye
  prtcp_hdr->length = htons(1);
  memcpy(pkt, prtcp_hdr, sizeof(rtcp_common_t));
  int len = sizeof(rtcp_common_t);
  memcpy(pkt+len, &ssrc, sizeof(ssrc));
  len += sizeof(ssrc);

  //echo("setBYE: ssrc=%x len=%d", ssrc, len);
  return len;
}

/**
 * Sends a RTCP packet
 */
int Session::sendRTCPPacket(const struct sockaddr_in *to, uint8_t pt)
{
  int pkt_len = 0, r, sd;
  rtcp_common_t rtcp_hdr;
  struct sockaddr_in *sin_rtcp;
  uint8_t pkt[PKTSIZE];

  switch (pt) {
  case RTCP_SR: // send a SR
    pkt_len = buildSR(&rtcp_hdr, pkt);
    break;
  case RTCP_SDES: // send a SDES
    pkt_len = buildSDES(&rtcp_hdr, pkt);
    break;
  case RTCP_BYE: // send a BYE
    pkt_len = buildBYE(&rtcp_hdr, pkt);
    break;
  }
  if ((sin_rtcp = Channel::getSaRTCP(to)) == NULL) {
    error("sendRTCPPacket: sin_rtcp NULL"); return -1;
  }
  if ((sd = Channel::getFdSendRTCP(sin_rtcp)) < 0) {
    error("sendRTCPPacket: sd <0"); return -1;
  }

  r = Rtp::sendPacket(sd, pkt, pkt_len, sin_rtcp);
  statSendRTCP(pkt_len);
  return r;
}

/**
 * Sends a RTCP compound packet (SR + SDES)
 */
int Session::sendSRSDES(const struct sockaddr_in *to)
{
  int pkt_len = 0, sd;
  rtcp_common_t rtcp_hdr;
  struct sockaddr_in *sin_rtcp;
  uint8_t pkt[PKTSIZE];

  int len = buildSR(&rtcp_hdr, pkt);
  pkt_len += len;
  len = buildSDES(&rtcp_hdr, pkt+len);
  pkt_len += len;

  if ((sin_rtcp = Channel::getSaRTCP(to)) == NULL) {
    error("sendSRSDES: sin_rtcp NULL");
    return -1;
  }
  if ((sd = Channel::getFdSendRTCP(sin_rtcp)) < 0) {
    error("sendSRSDES: sd <0");
    return -1;
  }

  //echo("sendSRSDES: pkt_len=%d", pkt_len);
  int r = Rtp::sendPacket(sd, pkt, pkt_len, sin_rtcp);
  statSendRTCP(pkt_len);
  return r;
}

void Session::dump()
{
  echo("group/port/ttl=%x/%x/%x", group, rtp_port, ttl);

  SdesItem *sitem;
  int i;
  for (i=0, sitem = mysdes; sitem ; sitem = sitem->si_next, i++) {
    if (sitem->si_type > RTCP_SDES_END && sitem->si_type <= RTCP_SDES_SOURCE && sitem->si_len > 0 && sitem->si_len < 128 && sitem->si_str)
      echo("  sdes[%d]=%s", i, sitem->si_str);
  }
  fflush(stderr);
}

void Session::dumpAll()
{
  for (Session *pse = sessionList; pse ; pse = pse->next) {
    pse->dump();
  }
}

void Session::stat()
{
  writelog("worldname       : %s", World::current()->getName());
  writelog("channel         : %s", World::current()->getChan());
  writelog("source ssrc     : %x", source->ssrc);
  writelog("sources number  : %d", nbsources);
  writelog("source received : %d", source->s.received);
  writelog("source sr psent : %d", sr.psent);
  writelog("source sr osent : %d", sr.osent);
  writelog("source lost     : %d", source->lost);
  writelog("source next     : %x", source->next);
}
