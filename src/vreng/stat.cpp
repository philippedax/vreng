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
#include "session.hpp"	// Session
#include "gui.hpp"	// getCycles
#include "timer.hpp"	// rate diffDates
#include "render.hpp"	// stats
#include "prof.hpp"	// statMemory, statIO


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
  static struct timeval top;
  struct timeval now;

  gettimeofday(&now, NULL);
  float d = Timer::diffDates(top, now);
  if (fabs(d) > 0.1) {
    kbps_sent = bytes_sent/d/1000*8;
    kbps_recvd = bytes_recvd/d/1000*8;
    trace(DBG_NET, "kbps sent: %5.2f recvd: %5.2f", bytes_sent/d/1000*8, bytes_recvd/d/1000*8);
    top = now;
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
  echo("\n### Timings ###");

  Timer& timer = ::g.timer;
  d = timer.net.stop();

  echo("elapsed time    : %5.2fs", d);
  echo("init time       : %5.2fs, %4.2f%%", 
       (float) timer.init.cumul_time, (float) 100*timer.init.cumul_time/d);
  echo("simul time      : %5.2fs, %4.2f%%", 
       (float) timer.simul.cumul_time, (float) 100*timer.simul.cumul_time/d);
  echo("render time     : %5.2fs, %4.2f%%", 
       (float) timer.render.cumul_time, (float) 100*timer.render.cumul_time/d);
  echo("object time     : %5.2fs, %4.2f%%", 
       (float) timer.object.cumul_time, (float) 100*timer.object.cumul_time/d);
  echo("image time      : %5.2fs, %4.2f%%", 
       (float) timer.image.cumul_time, (float) 100*timer.image.cumul_time/d);
  echo("mysql time      : %5.2fs, %4.2f%%", 
       (float) timer.mysql.cumul_time, (float) 100*timer.mysql.cumul_time/d);
  echo("idle time       : %5.2fs, %4.2f%%", 
       (float) timer.idle.cumul_time, (float) 100*timer.idle.cumul_time/d);
  echo("cycles          : %d", ::g.gui.getCycles());
  echo("cycles/s        : %5.2f/s", timer.rate());
}

void statNetwork()
{
  echo("### Network ###");

  if (!d) d = ::g.timer.net.stop();
  float bw = (float)((sum_bytes_sent + sum_bytes_recvd + 2 * (8 + 20)) * 8) / d;

  if (sum_pkts_sent) {
    echo("pkts sent       : %d", sum_pkts_sent);
    echo("pkts_rtp sent   : %d", sum_pkts_rtp_sent);
    echo("pkts_rtcp sent  : %d", sum_pkts_rtcp_sent);
    echo("pkts sent/s     : %.0f/s", sum_pkts_sent/d);
  }
  if (sum_pkts_recvd) {
    echo("pkts received   : %d", sum_pkts_recvd);
    echo("pkts received/s : %.0f/s", sum_pkts_recvd/d);
    echo("pkts lost       : %d", sum_pkts_lost);
    echo("pkts lost %%     : %2.2f%%", ((double) 100*sum_pkts_lost)/(double) sum_pkts_recvd);
  }
  echo("bytes sent      : %d", sum_bytes_sent);
  echo("bytes_rtp sent  : %d", sum_bytes_rtp_sent);
  echo("bytes_rtcp sent : %d", sum_bytes_rtcp_sent);
  echo("bytes sent/s    : %.0f/s", sum_bytes_sent/d);
  if (sum_bytes_recvd) {
    echo("bytes received  : %d", sum_bytes_recvd);
    echo("bytes received/s: %.0f/s", sum_bytes_recvd/d);
  }
  echo("pkt max         : %d", pkt_max);
  echo("pkt min         : %d", pkt_min);
  if (sum_pkts_sent)
    echo("bytes/pkt sent  : %d", sum_bytes_sent / sum_pkts_sent);
  if (sum_pkts_recvd)
    echo("bytes/pkt recvd : %d", sum_bytes_recvd / sum_pkts_recvd);
  echo("bw IP+UDP+RTP+PL: %.0f bps", bw);
#if 0
  echo("### Sources ###");
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

void printStats()
{
  statLog();
  statTimings();
  statNetwork();
  Render::stat();
  statMemory();
  statIO();
}

