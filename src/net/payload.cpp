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
// payload.cpp
//
// Payload handling
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "payload.hpp"
#include "netobj.hpp"	// NetObj
#include "noid.hpp"	// Noid
#include "vrep.hpp"	// VREP_VERSION_V3
#include "nsl.hpp"	// inet4_ntop
#include "channel.hpp"	// Channel
#include "session.hpp"	// buildRtpHeader
#include "source.hpp"	// Source
#include "netprop.hpp"	// NetProperty
#include "socket.hpp"	// NEEDLOOPBACK
#include "stat.hpp"	// statSendRTP getSentPackets


/** Constructor */
Payload::Payload()
{
  new_payload++;
  len = 0;
  idx = 0;
  gzipped = false;
  vrep = VREP_VERSION_V3;
}

/** Destructor */
Payload::~Payload()
{
  del_payload++;
}

Payload * Payload::resetPayload()
{
  len = 0;
  idx = 0;
  return this;
}

/** Checks validity */
bool Payload::isValid()
{
  return (len <= PAYLOAD_LEN && idx <= len);
}

/** Fills the payload */
int Payload::putPayload(const char *format, ...)
{
  va_list ap;

  if (! isValid()) {
    error("putPayload: invalid payload %s", this);
    return -1;
  }
  if (format == NULL) {
    error("putPayload: NULL format");
    return -1;
  }

  va_start(ap, format);
  len = idx;	// "rewrite" mode rather than "append"
  trace(DBG_NET, "putPayload: idx=%d", idx);

  /* Parse format */
  while (*format) {
    switch (*format) {
    case 'c': // char
      {
	uint8_t c = va_arg(ap, int);  	

	data[idx++] = 'c';
	data[idx++] = c;
	break;
      }
    case 'h': // short
      {
	int16_t h = htons(va_arg(ap, int));  	

	data[idx++] = 'h';
	memcpy(data + idx, &h, sizeof(int16_t));
	idx += sizeof(int16_t);
	break;
      }
    case 'd': // int32_t
      {
	int32_t d = htonl(va_arg(ap, int));

	data[idx++] = 'd';
	memcpy(data + idx, &d, sizeof(int32_t));
	idx += sizeof(int32_t);
	break;
      }
    case 'f': // float
      {
	float f = va_arg(ap, double);
	int32_t *p = (int32_t*) &f;
	int32_t j = htonl(*p);

	data[idx++] = 'f';
	memcpy(data + idx, &j, sizeof(int32_t));
	idx += sizeof(int32_t);
	break;
      }
    case 's': // string
      {
	char *s = va_arg(ap, char *);
	uint16_t l;
        l = (*s) ? strlen(s) : 0;

	data[idx++] = 's';
	data[idx++] = l;
        if (l)
	  memcpy(data + idx, s, l);
	idx += l;
	break;
      }
    case 'n': // Noid
      {
	Noid n = va_arg(ap, Noid);

	data[idx++] = 'n';

	/* everything is already in network format */
	memcpy(data + idx, &n.src_id, sizeof(n.src_id));
	idx += sizeof(n.src_id);
	memcpy(data + idx, &n.port_id, sizeof(n.port_id));
	idx += sizeof(n.port_id);
	memcpy(data + idx, &n.obj_id, sizeof(n.obj_id));
	idx += sizeof(n.obj_id);
	break;
      }
    case 't': // timeval
      {
	struct timeval t = va_arg(ap, struct timeval);
	time_t sec = htonl(t.tv_sec);
	time_t usec = htonl(t.tv_usec);

	data[idx++] = 't';
	memcpy(data + idx, &sec, sizeof(int32_t));
	idx += sizeof(int32_t);
	memcpy(data + idx, &usec, sizeof(int32_t));
	idx += sizeof(int32_t);
	break;
      }	

    default: // unknown type in format
      error("putPayload: invalid format '%c' in %s", *format, format);
      va_end(ap);
      return -1;
    }

    format++;
    if (idx > len)
      len = idx;

    /* check the length */
    if (len >= PAYLOAD_LEN) {
      error("putPayload: payload too long (%d > %d)", len, PAYLOAD_LEN);
      dump(stderr);
      len = idx = 0; // just in case
      va_end(ap);
      return -1;
    }
  }
  va_end(ap);
  return 0;
}

/** Gets the payload */
int Payload::getPayload(const char *format, ...)
{
  va_list ap;

  if (! isValid()) {
    error("getPayload: invalid len=%d idx=%d %02x%02x%02x%02x", len, idx, data[idx], data[idx+1], data[idx+2], data[idx+3]);
    return -1;
  }
  if (! format) {
    error("getPayload: NULL format");
    return -1;
  }

  va_start(ap, format);

  while (*format) {
    /* Format known ? */
    if (strchr("chdfsnt", *format) == NULL) {
      error("getPayload: invalid format [%c] in %s", *format, format);
      format++;
      continue;
    }

#if 0 //debug
    switch (*format) {
    case 'c':
      echo("format=%c idx=%d len=%d data=[%c %02x]", *format, idx, len, data[idx+0], data[idx+1]);
      break;
    case 'h':
      echo("format=%c idx=%d len=%d data=[%c %02x%02x]", *format, idx, len, data[idx+0], data[idx+1], data[idx+2]);
      break;
    case 'd':
    case 'f':
      echo("format=%c idx=%d len=%d data=[%c %02x%02x%02x%02x]", *format, idx, len, data[idx+0], data[idx+1], data[idx+2], data[idx+3], data[idx+4]);
      break;
    case 'n':
      echo("format=%c idx=%d len=%d data=[%c %02x%02x%02x%02x %02x%02x %02x%02x]", *format, idx, len, data[idx+0], data[idx+1], data[idx+2], data[idx+3], data[idx+4], data[idx+5], data[idx+6], data[idx+7], data[idx+8]);
      break;
    case 's':
      echo("format=%c idx=%d len=%d data=[%c %02x %02x%02x%02x%02x%02x%02x%02x]", *format, idx, len, data[idx+0], data[idx+1], data[idx+2], data[idx+3], data[idx+4], data[idx+5], data[idx+6], data[idx+7], data[idx+8]);
      break;
    }
#endif //debug

    /* test matching Payload - format */
    if (data[idx] != *format) {
      error("getPayload: mismatch '%c'[x'%02x'], format='%s', len=%d, idx=%d[x'%02x']",
	    data[idx], data[idx], format, len, idx, idx);
      dump(stderr);
      return -1;
    }
    idx++;	// points on following data

    switch (*format) {
    case 'c': // char
      {
	uint8_t *p = va_arg(ap, uint8_t *);

	memcpy(p, data + idx, sizeof(uint8_t));
	idx++;
	break;
      }
    case 'h': // int16_t
      {
	int16_t h;
	int16_t *p = va_arg(ap, int16_t *);

	memcpy(&h, data + idx, sizeof(int16_t));
	*p = ntohs(h);
	idx += sizeof(int16_t);
	break;
      }
    case 'd': // int32_t
      {
	int32_t d;
	int32_t *p = va_arg(ap, int32_t *);

	memcpy(&d, data + idx, sizeof(int32_t));
	*p = ntohl(d);
	idx += sizeof(int32_t);
	break;
      }
    case 'f': // float
      {
	int32_t f;
	float *p = va_arg(ap, float *);

	memcpy(&f, data + idx, sizeof(int32_t));
	f = ntohl(f);
	memcpy(p, &f, sizeof(int32_t));
	idx += sizeof(int32_t);
	break;
      }
    case 's': // string
      {
	/* Note: no length check */
	uint8_t *s = va_arg(ap, uint8_t *);
	uint16_t l = data[idx++];

        if (l)
	  memcpy(s, data + idx, l);
	s[l] = 0; // NULL terminated
	idx += l;
	break;
      }
    case 'n': // Noid
      {
	Noid *n = va_arg(ap, Noid*);

	memcpy(&n->src_id, data + idx, sizeof(n->src_id));
	idx += sizeof(n->src_id);
	memcpy(&n->port_id, data + idx, sizeof(n->port_id));
	idx += sizeof(n->port_id);
	memcpy(&n->obj_id, data + idx, sizeof(n->obj_id));
	idx += sizeof(n->obj_id);
	break;
      }
    case 't': // timeval
      {
	time_t sec, usec;
	struct timeval *p = va_arg(ap, struct timeval*);

	memcpy(&sec, data + idx, sizeof(int32_t));
	idx += sizeof(int32_t);
	memcpy(&usec, data + idx, sizeof(int32_t));
	idx += sizeof(int32_t);
	p->tv_sec = ntohl(sec);
	p->tv_usec = ntohl(usec);
	break;
      }	
    default:
      error("getPayload: format unimplemented [%c] in %s", *format, format);
      va_end(ap);
      return -1;
    }

    /* verify if not too far */
    if (idx > len) {
      error("getPayload: past end of payload: idx=%d len=%d", idx, len);
      dump(stderr);
      idx = len = 0;
      va_end(ap);
      return -1;
    }
    format++;
  }
  va_end(ap);
  return 0;
}

void Payload::seekPayload(uint16_t _idx)
{
  idx = _idx;
}

uint16_t Payload::tellPayload()
{
  return idx;
}

int Payload::tellPayload(const char *str)
{
  uint16_t save_idx = idx;

  for (idx = 0; idx < len; ) {
    uint8_t format = data[idx];

    /* checks format */
    if (strchr("chdfsnt", format) == NULL) {
      error("tellPayload: invalid format [%c]", format);
      idx = save_idx;
      return -1;
    }
    idx++;	// skip format

    switch (format) {
    case 'c': // char
	idx++;
	break;
    case 'h': // int16_t
	idx += sizeof(int16_t);
	break;
    case 'd': // int32_t
    case 'f': // float
	idx += sizeof(int32_t);
	break;
    case 's': // string
	if (! strncmp(reinterpret_cast<const char *>(data + idx + 1), str, strlen(str))) {
          idx--;	// string matches
	  return idx;
        }
	idx += data[idx];
        idx++;
	break;
    case 'n': // Noid
	idx += sizeof(uint32_t);
	idx += sizeof(uint16_t);
	idx += sizeof(uint16_t);
	break;
    case 't': // timeval
	idx += sizeof(int32_t);
	idx += sizeof(int32_t);
	break;
    default:
        if (format != 0) {
	  error("tellPayload: format unimplemented [%c 0x%02x]", format, format);
          dump(stderr);
        }
	idx = save_idx;
	return -1;
    }
  }
  error("tellPayload: past end of payload: idx=%d len=%d", idx, len);
  dump(stderr);
  idx = save_idx;
  return -1;
}

void Payload::dump(FILE *f)
{
  fprintf(f, "dump: len=%d 0x%03x, idx=%d 0x%03x\n", len, len, idx, idx);

  char adr[4], hex[49], asc[17];

  for (int i=0; i<len; ) {
    memset(adr, 0, sizeof(adr));
    memset(hex, 0, sizeof(hex));
    memset(asc, 0, sizeof(asc));
    sprintf(adr, "%03x", i);
    for (int j=0; j<16; j++, i++) {
      int c = (uint8_t) data[i];
      sprintf(&hex[j*3], "%02x ", c);
      if (isprint(c))
        sprintf(&asc[j], "%c", c);
      else
        sprintf(&asc[j], ".");
      if (i == len-1) {
        sprintf(&hex[(j+1)*3], ">  ");
        sprintf(&asc[j+1], ">");
        i = len;
        break;
      }
    }
    fprintf(f, "%s:  %s  | %s |\n", adr, hex, asc);
  }
  fflush(f);
}

/**
 * Sends a payload
 *
 * TODO: add vrep_version argument
 */
int Payload::sendPayload(const struct sockaddr_in *to)
{
  uint8_t pkt[PKTSIZE];
  uint8_t *hdrpl;
  int pkt_len, sd;
  rtp_hdr_t *rtp_hdr = (rtp_hdr_t *) pkt;

  /*
   * initial checks
   */
  if (! isValid()) {
    error("sendPayload: invalid");
    return -1;
  }
  if (! to) {
    error("sendPayload: to NULL");
    return -1;
  }
  //dax if (! len) { return -1; }

  /*
   * builds the RTP Header
   */
  Channel *pchan;
  if ((pchan = Channel::getbysa(to)) == NULL) {
    pchan = Channel::current();	// hack !!!
    error("sendPayload: pchan NULL, to=%p", to);
    return -1;			//FIXME: channel NULL
  }
  if (pchan->session) {
    pchan->session->buildRtpHeader(rtp_hdr, pchan->ssrc);
  }
  else return -1;		//FIXME: session NULL

  /*
   * builds the VREP Header
   */
  hdrpl = pkt + RTP_HDR_SIZE;

  hdrpl[VREP_HDR_VERSION] = vrep;
  switch (vrep) {
  case VREP_VERSION_V3:
    pkt_len = RTP_HDR_SIZE + VREP_HDR_SIZE_V3 + len;	// bytes
    memcpy(hdrpl+VREP_HDR_SIZE_V3, data, len);
    //TODO: if (gzipped) do compression
    break;
  case VREP_VERSION_V2:
    hdrpl[VREP_HDR_FLAGS] = 1;	// payload header length in words
    hdrpl[VREP_HDR_IP] = AF_INET;
    hdrpl[VREP_HDR_PLLEN] = (len + 3 + VREP_HDR_SIZE_V2) >> 2;	// words
    pkt_len = RTP_HDR_SIZE + (hdrpl[VREP_HDR_PLLEN] <<2);	// bytes
    memcpy(hdrpl+VREP_HDR_SIZE_V2, data, len);
    hdrpl[VREP_HDR_SIZE_V2 + len] = 0;	// end of payload chunk
    break;
  default:
    error("sendpayload: bad vrep_version=%d", vrep);
    return -1;
  }
  trace(DBG_NET, "S: %02x%02x%02x%02x/%02x", hdrpl[0], hdrpl[1], hdrpl[2], hdrpl[3], hdrpl[4]);

  /*
   * finds the file descriptor
   */
  if ((sd = Channel::getFdSendRTP(to)) <= 0) {
    sd = pchan->sd[SD_W_RTP];	// hack !!!
  }

  /*
   * sends the packet
   */
  Rtp::sendPacket(sd, pkt, pkt_len, to);
  statSendRTP(pkt_len);

  /* Updates SR, TODO: computes RCTP interval */
  pchan->session->sr.psent++;
  pchan->session->sr.osent += pkt_len;

  uint32_t sent = getSentPackets();
  if (sent && ((sent % 100) == 0)) {
    /*
     * sends RTCP compound (SR + SDES)
     */
    pchan->session->refreshMySdes();
    pchan->session->sendSRSDES(to);
  }
  return pkt_len;
}

/**
 * Receives a Payload packet
 */
int Payload::recvPayload(int sd, struct sockaddr_in *from)
{
  if (! from) {
    error("recvPayload: from NULL");
    return -1;
  }

  uint8_t pkt[PKTSIZE];
  rtp_hdr_t *rtp_hdr = (rtp_hdr_t *) pkt;
  rtcp_common_t *rtcp_hdr = (rtcp_common_t *) pkt;
  int pkt_len;
  socklen_t l = sizeof(struct sockaddr_in);

  memset(from, 0, sizeof(struct sockaddr_in));
  if ((pkt_len = recvfrom(sd, pkt, sizeof(pkt), 0, (struct sockaddr *)from, &l)) <0) {
    error("recvPayload: %s on %d", strerror(errno), sd);
    return pkt_len;			// here pkt_len < 0 -> error
  }
  //echo("recvPayload: %lx (%x)", ntohl(from->sin_addr.s_addr), pkt_len);

  /*
   * test if the packet was sent by myself
   * this is probably broken anyway, loopback or not
   */
  if (NetObj::getHost() == ntohl(from->sin_addr.s_addr)) {
#if NEEDLOOPBACK
    // If two apps are running on the same machine,
    // you need to sort out the packets on something else than just the host ID
    if (ntohl(rtp_hdr->ssrc) == NetObj::getSsrc())
#endif
      return 0; // Loopback from same app : ignore it
  }

  statReceivePacket(pkt_len);		//FIXME! is not at the good place

  /*
   * test if it is a valid RTP header
   */
  uint8_t rtp_hdr_size;
  if (rtp_hdr->version == RTP_VERSION &&
      (rtp_hdr->pt == RTP_VREP_TYPE || rtp_hdr->pt == RTP_VREP_TYPE_V1)) {
    //
    // it's a RTP packet
    //
    rtp_hdr_size = RTP_HDR_SIZE;
    uint16_t seq = ntohs(rtp_hdr->seq);
    Source *pso;

    if ((pso = Source::getSource(ntohl(rtp_hdr->ssrc))) != NULL) {
      sourceInfos *s;

      pso->rr.lost += seq - pso->rr.last_seq - 1;
      pso->rr.last_seq = seq;
      s = &(pso->s);
      Rtp::updateSeq(s, seq);
      pso->extended_max = s->cycles + s->max_seq;
      pso->expected = pso->extended_max - s->base_seq + 1;
      pso->lost = pso->expected - s->received;	// number of packets lost
    }
  }
  else if (rtcp_hdr->version == RTP_VERSION) {
    //
    // it's a RTCP packet
    //
    switch (rtcp_hdr->pt) {
      case RTCP_SR:
      case RTCP_SDES:
      case RTCP_BYE:
      case RTCP_APP:
        return Rtp::recvRTCPPacket(from, pkt, pkt_len);
      case RTCP_RR:	// why send a RR ? we only send SR
        return 0;	// like action done
      default:
        return -3;	// unknown RTCP packet type
    }
  }
  else {
    // it's an UDP packet, old version of VREng <= 1.5.8
    rtp_hdr_size = 0;
  }

  uint8_t *hdrpl = pkt + RTP_HDR_SIZE;
  //echo("R: %02x%02x%02x%02x/%02x",hdrpl[0],hdrpl[1],hdrpl[2],hdrpl[3],hdrpl[4]);

  /*
   * compatibility with older VREP Protocol
   */
  uint8_t vrep_version = hdrpl[VREP_HDR_VERSION]; // vrep version received
  int32_t vrep_len;		// vrep header size + payload size

  uint8_t vrep_hdr_size;	// vrep header size

  switch (vrep_version) {
  case VREP_VERSION_V3:
    vrep_hdr_size = VREP_HDR_SIZE_V3;
    vrep_len = pkt_len - rtp_hdr_size - vrep_hdr_size;
    gzipped = hdrpl[VREP_HDR_VERSION] & VREP_GZIP_MASK;
    //TODO: ungzip data and update len
    break;
  case VREP_VERSION_V2:
    /* old version <= 5.1.5 VREP2 */
    vrep_hdr_size = VREP_HDR_SIZE_V2;
    if (hdrpl[VREP_HDR_FLAGS] == 1)
      vrep_len = hdrpl[VREP_HDR_PLLEN] << 2;	// length in words
    else
      vrep_len = hdrpl[VREP_HDR_PLLEN];		// length in bytes
    break;
  default:
    // old version <= 1.5.8 VREP1
    vrep_hdr_size = VREP_HDR_SIZE_V1;
    vrep_len = hdrpl[VREP_HDR_PLLEN_V1]; // length in bytes <= 1.5.8 VREP1
    memcpy(&from->sin_addr.s_addr, hdrpl+VREP_HDR_SRCID_V1, 4);
    memcpy(&from->sin_port, hdrpl+VREP_HDR_PORTID_V1, 2);
  }

  /*
   * check if valid payload header
   */
  if (pkt_len != rtp_hdr_size + vrep_hdr_size + vrep_len || vrep_len < vrep_hdr_size) {
    trace(DBG_NET, "R: %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
                    pkt[0], pkt[1], pkt[2], pkt[3],
                    pkt[4], pkt[5], pkt[6], pkt[7],
                    pkt[8], pkt[9], pkt[10], pkt[11]);
    trace(DBG_NET, "R: %02x%02x%02x%02x/%02x", hdrpl[0], hdrpl[1], hdrpl[2], hdrpl[3], hdrpl[4]);
    trace(DBG_NET, "wrong packet size, pkt_len=%d vrep_len=%d", pkt_len, vrep_len);
    return -2;	// unknown packet type
  }

  /*
   * fill the Payload fields
   */
  resetPayload();
  vrep = vrep_version;
  len = vrep_len;
  memcpy(data, hdrpl+vrep_hdr_size, len);

  return pkt_len;
}

/** Incoming Delta */
void Payload::incomingDelta(const struct sockaddr_in *from)
{
  Noid noid;		// netobj name received
  uint8_t prop_id;	// property number received
  int16_t vers_id;	// version received

  if (getPayload("nch", &noid, &prop_id, &vers_id) < 0) {
    return;
  }

  NetObj *pn;
  if ((pn = noid.getNetObj()) == NULL) {
    // delta on an unknown object
    trace(DBG_NET, "inDelta sendQuery on: %s, from=%s, p=%d, v=%d", noid.getNoid(), inet4_ntop(&from->sin_addr), prop_id, vers_id);
    // send a Query to the sender in unicast
    noid.sendQuery(from);
    return;
  }

  /* verify prop_id */
  uint8_t nprop = pn->getProperties();
  if (prop_id >= nprop) {
    error("inDelta: invalid property prop_id=%d" "(type=%d, nprop=%d)", prop_id, pn->type, nprop);
    return;
  }
  NetProperty *pprop = pn->netprop + prop_id;

  /*
   * depends on prop version
   */
  // in complement to 2: d gives the distance, same throught the boundary
  int16_t d = pprop->version - vers_id;	// versions difference
  if (abs(d) > 5000) {	// very far
    echo("inDelta: very different versions: mine is %d, received %d", pprop->version, vers_id);
  }
  if (d > 0) return;	// mine is more recent

  pprop->resetDates();
  if (d < 0) {  	// mine is less recent, its own is more recent
    pn->getProperty(prop_id, this);
    pprop->setResponsible(false); // leave responsibility to the other one
    pprop->version = vers_id;
    return;
  }
  /* same versions, 2 responsibles: conflict */
  else if (pprop->responsible) {
    // resolved by getting a new random version
    // publishes new version to sender in unicast
    pn->declareDelta(prop_id);
    // error("conflict resol: obj=%s, prop=%d, changing vers_num %d->%d",
    // pn->getNoid(), prop_id, vers_id, pn->prop[prop_id].version);
  }
  // else, it's just a "recall" (then nothing to do)
}

/** Incoming Create */
void Payload::incomingCreate(const struct sockaddr_in *from)
{
  Noid noid;
  uint8_t type_id, perm;

  if (getPayload("cnc", &type_id, &noid, &perm) < 0) {
    return;
  }
  if (noid.getNetObj()) return;  // local copy already exists -> ignore this request

  trace(DBG_NET, "inCreate: nobj=%s (type=%d), perm=%d", noid.getNoid(), type_id, perm);
  //dump(stderr);

  //
  // creates the replicated object
  // glue with WO (wobject)
  // very important !!!
  //
  NetObj *pn = NetObj::replicateObject(type_id, noid, this);
  if (!pn) {
    error("inCreate: can't replicate object, type=%d", type_id);
    return;
  }

#if 0 //debug
  if (pn->type != type_id) {
    error("inCreate: bad type=%d", type_id);
    return;
  }
#endif
  //dax if (! pn->equalNoid(pn->noid)) {
  if (! pn->noid.equal(pn->noid)) {
    error("inCreate: bad noid=%s", pn->noid.getNoid());
    return;
  }
  pn->state = perm;
  pn->initProperties(false); // we are not responsible
  pn->addToList();

  // gets properties
  uint8_t nprop = pn->getProperties();
  for (int i=0; i < nprop; i++) {
    if (getPayload("h", &(pn->netprop[i].version)) < 0) {
      return;
    }
  }
}

/** Incoming Query */
void Payload::incomingQuery(const struct sockaddr_in *from)
{
  Noid noid;

  if (getPayload("n", &noid) < 0) return;
  if (noid.port_id == 0) {	//HACK!
    error("inQuery: port_id null");
    return;
  }
  trace(DBG_NET, "inQuery: nobj=%s from=%s", noid.getNoid(), inet4_ntop(&from->sin_addr));

  NetObj *pn;
  if ((pn = noid.getNetObj()) == NULL) {
    // unknown object: may be we have deleted it, we advertize the requester
    error("inQuery: sendDelete nobj=%s from=%s", noid.getNoid(), inet4_ntop(&from->sin_addr));
    noid.sendDelete(from);
  }
  else {
    // object known, but not properties, ask them to sender
    //echo("inQuery: sendCreate nobj=%s from=%s", pn->noid.getNoid(), inet4_ntop(&from->sin_addr));
    pn->sendCreate(from);
  }
}

/** Incoming Delete */
void Payload::incomingDelete(const struct sockaddr_in *from)
{
  Noid noid;

  if (getPayload("n", &noid) < 0) {
    return;
  }

  trace(DBG_NET, "inDelete: nobj=%s from=%s", noid.getNoid(), inet4_ntop(&from->sin_addr));

  NetObj *pn;
  if ((pn = noid.getNetObj())) {
    pn->requestDeletion();
  }
}

/** Incoming Unknown */
void Payload::incomingUnknown(const struct sockaddr_in *from, int size)
{
  error("InUnknown: size=%d from %lx/%x", size, ntohl(from->sin_addr.s_addr), ntohs(from->sin_port));
  if (size) {
    trace(DBG_NET,
          "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
          data[0], data[1], data[2], data[3],
          data[4], data[5], data[6], data[7],
          data[8], data[9], data[10], data[11],
          data[12], data[13], data[14], data[15]);
  }
}
