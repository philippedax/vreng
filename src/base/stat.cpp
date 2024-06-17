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
// stat.cpp
//
// Statistics operations
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "stat.hpp"
#include "session.hpp"	// Session
#include "gui.hpp"	// getCycles
#include "timer.hpp"	// rate diffDates

// global

/*** memory ***/
uint32_t new_wobject;
uint32_t del_wobject;
uint32_t new_world;
uint32_t del_world;
uint32_t new_solid;
uint32_t del_solid;
uint32_t new_texture;
uint32_t del_texture;
uint32_t new_image;
uint32_t del_image;
uint32_t new_channel;
uint32_t del_channel;
uint32_t new_session;
uint32_t del_session;
uint32_t new_source;
uint32_t del_source;
uint32_t new_payload;
uint32_t del_payload;
uint32_t new_netobj;
uint32_t del_netobj;
uint32_t new_olist;
uint32_t del_olist;
uint32_t new_netprop;
uint32_t del_netprop;
uint32_t new_http;
uint32_t del_http;
uint32_t new_cache;
uint32_t del_cache;
uint32_t new_file;
uint32_t del_file;

/*** io ***/
uint32_t opn_file;
uint32_t cls_file;
uint32_t opn_sock;
uint32_t cls_sock;

// local

/*** network counters ***/
static uint32_t pkts_sent = 0;
static uint32_t pkts_recvd = 0;
static uint32_t bytes_sent = 0;
static uint32_t bytes_recvd = 0;
static uint32_t pkts_rtp_sent = 0;
static uint32_t bytes_rtp_sent = 0;
static uint32_t pkts_rtcp_sent = 0;
static uint32_t bytes_rtcp_sent = 0;
static uint32_t pkt_max = 0;
static uint32_t pkt_min = 512;
static uint32_t sum_pkts_sent = 0;
static uint32_t sum_pkts_recvd = 0;
static uint32_t sum_pkts_rtp_sent = 0;
static uint32_t sum_pkts_rtcp_sent = 0;
static uint32_t sum_bytes_sent = 0;
static uint32_t sum_bytes_rtp_sent = 0;
static uint32_t sum_bytes_rtcp_sent = 0;
static uint32_t sum_bytes_recvd = 0;
static uint32_t sum_pkts_lost = 0;
static float kbps_sent = 0.0;
static float kbps_recvd = 0.0;

static FILE *flog;
static float d;


void statSendPacket(int pkt_len)
{
  bytes_sent += pkt_len;
  sum_bytes_sent += pkt_len;
  pkts_sent++;
  sum_pkts_sent++;
  pkt_max = MAX(pkt_max, pkt_len);
  pkt_min = MIN(pkt_min, pkt_len);
}

uint32_t getSentPackets()
{
  return pkts_sent;
}

void setLostPackets(int lost)
{
  sum_pkts_lost += lost;
}

void statSendRTP(int pkt_len)
{
  bytes_rtp_sent += pkt_len;
  sum_bytes_rtp_sent += pkt_len;
  pkts_rtp_sent++;
  sum_pkts_rtp_sent++;
}

void statSendRTCP(int pkt_len)
{
  bytes_rtcp_sent += pkt_len;
  sum_bytes_rtcp_sent += pkt_len;
  pkts_rtcp_sent++;
  sum_pkts_rtcp_sent++;
}

void statReceivePacket(int pkt_len)
{
  bytes_recvd += pkt_len;
  sum_bytes_recvd += pkt_len;
  pkts_recvd++;
  sum_pkts_recvd++;
  pkt_max = MAX(pkt_max, pkt_len);
  pkt_min = MIN(pkt_min, pkt_len);
}

/* compute kbps_sent & co */
void statKbps()
{
  static struct timeval top;
  struct timeval now;

  gettimeofday(&now, NULL);
  float d = Timer::diffDates(top, now);
  if (fabs(d) > 0.1) {
    kbps_sent = bytes_sent/d/1000*8;
    kbps_recvd = bytes_recvd/d/1000*8;
    //echo("kbps sent: %5.2f recvd: %5.2f", bytes_sent/d/1000*8, bytes_recvd/d/1000*8);
    top = now;
    bytes_sent = bytes_recvd = 0;
  }
}

void statSessionRTP(Session *session)
{
  if ((flog = writelog("--- RTP stat ---")) == (FILE *) NULL) return;
  session->stat();
}

void statTimings(FILE *fout)
{
  fprintf(fout,"### Timings ###\n");

  Timer& timer = ::g.timer;
  d = timer.net.stop();

  fprintf(fout,"elapsed time    : %5.2fs\n", d);
  fprintf(fout,"init time       : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.init.cumul_time), static_cast<float>(100*timer.init.cumul_time/d));
  fprintf(fout,"simul time      : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.simul.cumul_time), static_cast<float>(100*timer.simul.cumul_time/d));
  fprintf(fout,"render time     : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.render.cumul_time), static_cast<float>(100*timer.render.cumul_time/d));
  fprintf(fout,"object time     : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.object.cumul_time), static_cast<float>(100*timer.object.cumul_time/d));
  fprintf(fout,"image time      : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.image.cumul_time), static_cast<float>(100*timer.image.cumul_time/d));
  fprintf(fout,"sql time        : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.sql.cumul_time), static_cast<float>(100*timer.sql.cumul_time/d));
  fprintf(fout,"idle time       : %5.2fs, %4.2f%%\n", 
       static_cast<float>(timer.idle.cumul_time), static_cast<float>(100*timer.idle.cumul_time/d));
  fprintf(fout,"cycles          : %d\n", ::g.gui.getCycles());
  fprintf(fout,"cycles/s        : %5.2f/s\n", timer.rate());
}

void statNetwork(FILE *fout)
{
  fprintf(fout, "### Network ###\n");

  if (!d) d = ::g.timer.net.stop();
  float bw = static_cast<float>(((sum_bytes_sent + sum_bytes_recvd + 2 * (8 + 20))) * 8) / d;

  if (sum_pkts_sent) {
    fprintf(fout, "pkts sent       : %d\n", sum_pkts_sent);
    fprintf(fout, "pkts_rtp sent   : %d\n", sum_pkts_rtp_sent);
    fprintf(fout, "pkts_rtcp sent  : %d\n", sum_pkts_rtcp_sent);
    fprintf(fout, "pkts sent/s     : %.0f/s\n", sum_pkts_sent/d);
  }
  if (sum_pkts_recvd) {
    fprintf(fout, "pkts received   : %d\n", sum_pkts_recvd);
    fprintf(fout, "pkts received/s : %.0f/s\n", sum_pkts_recvd/d);
    fprintf(fout, "pkts lost       : %d\n", sum_pkts_lost);
    fprintf(fout, "pkts lost %%     : %2.2f%%\n", ((double) 100*sum_pkts_lost)/(double) sum_pkts_recvd);
  }
  fprintf(fout, "bytes sent      : %d\n", sum_bytes_sent);
  fprintf(fout, "bytes_rtp sent  : %d\n", sum_bytes_rtp_sent);
  fprintf(fout, "bytes_rtcp sent : %d\n", sum_bytes_rtcp_sent);
  fprintf(fout, "bytes sent/s    : %.0f/s\n", sum_bytes_sent/d);
  if (sum_bytes_recvd) {
    fprintf(fout, "bytes received  : %d\n", sum_bytes_recvd);
    fprintf(fout, "bytes received/s: %.0f/s\n", sum_bytes_recvd/d);
  }
  fprintf(fout, "pkt max         : %d\n", pkt_max);
  fprintf(fout, "pkt min         : %d\n", pkt_min);
  if (sum_pkts_sent)
    fprintf(fout, "bytes/pkt sent  : %d\n", sum_bytes_sent / sum_pkts_sent);
  if (sum_pkts_recvd)
    fprintf(fout, "bytes/pkt recvd : %d\n", sum_bytes_recvd / sum_pkts_recvd);
  fprintf(fout, "bw IP+UDP+RTP+PL: %.0f bps\n", bw);
#if 0
  fprintf(fout, "### Sources ###\n");
  Source::dumpAll();
#endif
}

/** stat memory usage (new, delete) */
void statMemory(FILE *fout)
{
  fprintf(fout, "### Memory (new/delete) ###\n");

  fprintf(fout, "WO      : n=%8d d=%8d\n", new_wobject, del_wobject);
  fprintf(fout, "World   : n=%8d d=%8d\n", new_world, del_world);
  fprintf(fout, "Channel : n=%8d d=%8d\n", new_channel, del_channel);
  fprintf(fout, "Session : n=%8d d=%8d\n", new_session, del_session);
  fprintf(fout, "Source  : n=%8d d=%8d\n", new_source, del_source);
  fprintf(fout, "Solid   : n=%8d d=%8d\n", new_solid, del_solid);
  fprintf(fout, "Texture : n=%8d d=%8d\n", new_texture, del_texture);
  fprintf(fout, "Image   : n=%8d d=%8d\n", new_image, del_image);
  fprintf(fout, "NetObj  : n=%8d d=%8d\n", new_netobj, del_netobj);
  fprintf(fout, "Payload : n=%8d d=%8d\n", new_payload, del_payload);
  fprintf(fout, "OList   : n=%8d d=%8d\n", new_olist, del_olist);
  fprintf(fout, "Http    : n=%8d d=%8d\n", new_http, del_http);
  fprintf(fout, "Cache   : n=%8d d=%8d\n", new_cache, del_cache);
  fprintf(fout, "File    : n=%8d d=%8d\n", new_file, del_file);
}

/** stat io usage (open, close) */
void statIO(FILE *fout)
{
  fprintf(fout, "### IO (open/close) ###\n");

  fprintf(fout, "File            : o=%8d c=%8d\n", opn_file, cls_file);
  fprintf(fout, "Socket          : o=%8d c=%8d\n", opn_sock, cls_sock);
}

/** stat rendering */
void statRender(FILE *fout)
{
  int dlist = glGenLists(1);
  fprintf(fout, "### Graphic ###\n");
  fprintf(fout, "display-lists   : %d\n", --dlist);
}

void printStats(FILE *fout)
{
  statLog();
  statTimings(fout);
  statNetwork(fout);
  statRender(fout);
  statMemory(fout);
  statIO(fout);
}

bool writeStats(const char * statsfile)
{
  FILE *f;
  f = fopen(statsfile, "w");
  if (f) {
    printStats(f);
    fclose(f);
  }
  else {
    perror("create stats");
    return false;
  }
  return true;
}

FILE * openStats(const char * statsfile)
{
  FILE *f;
  if ((f = fopen(statsfile, "r")) != NULL) {
    return f;
  }
  return NULL;
}

/** returns current line of stats */
bool getStats(FILE* f, char *line)
{
  if (fgets(line, 128, f)) {
    return 1;
  }
  fclose(f);
  return 0;
}

void statLog()
{
  if (flog) {
    fcloselog(flog);
    printlog();
  }
}

