//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#include "vreng.hpp"
#include "stat.hpp"
#include "netobj.hpp"
#include "session.hpp"	// Session
#include "gui.hpp"	// getCycles
#include "timer.hpp"	// rate diffDates


/*** network counters ***/
uint32_t pkts_sent = 0;
uint32_t sum_pkts_lost = 0;

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
static float kbps_sent = 0.0;
static float kbps_recvd = 0.0;


// local
static FILE *flog;
static float d;


void statSendPacket(int pkt_len)
{
  bytes_sent += pkt_len;
  sum_bytes_sent += pkt_len;
  pkts_sent++;
  sum_pkts_sent++;
  pkt_max = MAX((int) pkt_max, pkt_len);
  pkt_min = MIN((int) pkt_min, pkt_len);
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
  pkt_max = MAX((int) pkt_max, pkt_len);
  pkt_min = MIN((int) pkt_min, pkt_len);
}

/* compute kbps_sent & co */
void statAdjust()
{
  static struct timeval then;
  struct timeval now;

  gettimeofday(&now, NULL);
  float d = Timer::diffDates(then, now);
  if (fabs(d) > 0.1) {
    kbps_sent = bytes_sent/d/1000*8;
    kbps_recvd = bytes_recvd/d/1000*8;
    trace(DBG_NET, "kbps sent: %5.2f recvd: %5.2f",
	   bytes_sent/d/1000*8, bytes_recvd/d/1000*8);
    then = now;
    bytes_sent = bytes_recvd = 0;
  }
}

void statSessionRTP(Session *session)
{
  if ((flog = writelog("--- RTP stat ---")) == (FILE *) NULL) return;
  session->stat();
}

void statTimings()
{
  fprintf(stderr, "\n### Timings ###\n");

  Timer& timer = ::g.timer;
  d = timer.net.stop();

  fprintf(stderr, "elapsed time    : %5.2fs\n", d);
  fprintf(stderr, "init time       : %5.2fs, %4.2f%%\n", 
          (float) timer.init.cumul_time, (float) 100*timer.init.cumul_time/d);
  fprintf(stderr, "simul time      : %5.2fs, %4.2f%%\n", 
          (float) timer.simul.cumul_time, (float) 100*timer.simul.cumul_time/d);
  fprintf(stderr, "render time     : %5.2fs, %4.2f%%\n", 
          (float) timer.render.cumul_time, (float) 100*timer.render.cumul_time/d);
  fprintf(stderr, "object time     : %5.2fs, %4.2f%%\n", 
          (float) timer.object.cumul_time, (float) 100*timer.object.cumul_time/d);
  fprintf(stderr, "image time      : %5.2fs, %4.2f%%\n", 
          (float) timer.image.cumul_time, (float) 100*timer.image.cumul_time/d);
  fprintf(stderr, "mysql time      : %5.2fs, %4.2f%%\n", 
          (float) timer.mysql.cumul_time, (float) 100*timer.mysql.cumul_time/d);
  fprintf(stderr, "idle time       : %5.2fs, %4.2f%%\n", 
          (float) timer.idle.cumul_time, (float) 100*timer.idle.cumul_time/d);
  fprintf(stderr, "cycles          : %d\n", ::g.gui.getCycles());
  fprintf(stderr, "cycles/s        : %5.2f/s\n", timer.rate());
}

void statNetwork()
{
  fprintf(stderr, "### Network ###\n");

  if (!d) d = ::g.timer.net.stop();
  float bw = (float)((sum_bytes_sent + sum_bytes_recvd + 2 * (8 + 20)) * 8) / d;

  if (sum_pkts_sent) {
    fprintf(stderr, "pkts sent       : %d\n", sum_pkts_sent);
    fprintf(stderr, "pkts_rtp sent   : %d\n", sum_pkts_rtp_sent);
    fprintf(stderr, "pkts_rtcp sent  : %d\n", sum_pkts_rtcp_sent);
    fprintf(stderr, "pkts sent/s     : %.0f/s\n", sum_pkts_sent/d);
  }
  if (sum_pkts_recvd) {
    fprintf(stderr, "pkts received   : %d\n", sum_pkts_recvd);
    fprintf(stderr, "pkts received/s : %.0f/s\n", sum_pkts_recvd/d);
    fprintf(stderr, "pkts lost       : %d\n", sum_pkts_lost);
    fprintf(stderr, "pkts lost %%     : %2.2f%%\n", ((double) 100*sum_pkts_lost)/(double) sum_pkts_recvd);
  }
  fprintf(stderr, "bytes sent      : %d\n", sum_bytes_sent);
  fprintf(stderr, "bytes_rtp sent  : %d\n", sum_bytes_rtp_sent);
  fprintf(stderr, "bytes_rtcp sent : %d\n", sum_bytes_rtcp_sent);
  fprintf(stderr, "bytes sent/s    : %.0f/s\n", sum_bytes_sent/d);
  if (sum_bytes_recvd) {
    fprintf(stderr, "bytes received  : %d\n", sum_bytes_recvd);
    fprintf(stderr, "bytes received/s: %.0f/s\n", sum_bytes_recvd/d);
  }
  fprintf(stderr, "pkt max         : %d\n", pkt_max);
  fprintf(stderr, "pkt min         : %d\n", pkt_min);
  if (sum_pkts_sent)
    fprintf(stderr, "bytes/pkt sent  : %d\n", sum_bytes_sent / sum_pkts_sent);
  if (sum_pkts_recvd)
    fprintf(stderr, "bytes/pkt recvd : %d\n", sum_bytes_recvd / sum_pkts_recvd);
  fprintf(stderr, "bw IP+UDP+RTP+PL: %.0f bps\n", bw);
#if 0
  fprintf(stderr, "### Sources ###\n");
  Source::dumpAll();
#endif
}

void statLog()
{
  if (flog != (FILE *) NULL) {
    closelog(flog);
    printlog();
  }
}
