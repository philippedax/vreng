//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)
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
#include "sysdep.hpp"
#include "defaults.hpp"
#include "macros.hpp"


#define VAPS_PORT	4150
#define VAPS_BUFSIZ	8192
#define VAPS_HDRSIZ	80
#define DEF_BAP_VERS	"3.1"
#define DEF_BAP_FILE	"welcome.bap"
#define DEF_BAP_RATE	1
#define DEF_VRENG_GROUP	"224.255.0.25"
#define DEF_VRENG_RATE	10.

/* modes */
enum {
  MODE_UNICAST,
  MODE_MULTICAST
};

#if CYGWIN32
#include <_syslist.h>
#define MISSING_SYSCALL_NAMES
extern pid_t		fork(void);
extern unsigned int	sleep(unsigned int);
extern int		close(int);
#endif //CYGWIN32


// global
bool debug = 0;
bool playing = 0;
bool ipmode = MODE_UNICAST;
uint16_t num_frames;
long offset_frames;
char bap_hdr[VAPS_HDRSIZ];
char bap_version[6] = DEF_BAP_VERS;
uint16_t bap_rate = DEF_BAP_RATE;
float vreng_rate = DEF_VRENG_RATE;
char *bap_file = NULL;
char *log_file = NULL;
FILE *flog = NULL;
struct sockaddr_in vrengsa;
struct ip_mreq mreq;


const char *date()
{
  time_t clock;
  struct tm *tm;
  static char datestr[20];

  clock = time(0);
  tm = localtime(&clock);
  sprintf(datestr, "%02d/%02d/%02d-%02d:%02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year % 100, tm->tm_hour, tm->tm_min, tm->tm_sec);
  return datestr;
}

/* UDP server initialization */
int initUdpSender(bool ipmode, const struct sockaddr_in *sa, uint32_t addr, uint16_t port, uint8_t ttl)
{
  int sdudp = -1;

  if ((sdudp = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("child UDP socket");
    return -1;
  }

  memset(&vrengsa, 0, sizeof(vrengsa));
  vrengsa.sin_family = AF_INET;
  vrengsa.sin_port = htons(port);
  if (ipmode == MODE_MULTICAST) {	//Multicast
    mreq.imr_multiaddr.s_addr = addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
#if !defined(CYGWIN32)
    if (setsockopt(sdudp, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0)
      perror("setsockopt");
#endif
    setsockopt(sdudp, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    vrengsa.sin_addr.s_addr = addr;
    fprintf(flog, "initUdpSender: vreng_addr/port/ttl = %x/%d/%d\n",
                   ntohl(vrengsa.sin_addr.s_addr),
                   ntohs(vrengsa.sin_port), ttl);
    fflush(flog);
  }
  else {	// Unicast
    vrengsa.sin_addr.s_addr = sa->sin_addr.s_addr;
    fprintf(flog, "initUdpSender: vreng_addr/port=%x/%d\n",
                  ntohl(vrengsa.sin_addr.s_addr), ntohs(vrengsa.sin_port));
    fflush(flog);
  }
  return sdudp;
}

void leaveMulticast(int sdudp)
{
  setsockopt(sdudp, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
}

void closeUdpSender(int sdudp)
{
  if (sdudp > 0) {
    if (ipmode == MODE_MULTICAST && sdudp > 0) leaveMulticast(sdudp);
    close(sdudp);
  }
}

void doStop(int sdudp)
{
  if (playing) {
    closeUdpSender(sdudp);
    playing = 0;
  }
}

int getBapLine(char *bapline, FILE *fpbap)
{
  do {
    if (fgets(bapline, VAPS_BUFSIZ, fpbap) == NULL) return -1;
    bapline[strlen(bapline) -1] = ' ';
  } while (bapline[0] == '#');
  return strlen(bapline);
}

FILE * getBapHeader(const char *fname)
{
  FILE *fpbap = NULL;
  char *l;
  char bapline[VAPS_BUFSIZ];

  /* open the local bap file */
  if ((fpbap = fopen(fname, "r")) == NULL) {
    fprintf(flog, "can't open the bap file: %s\n", fname); fflush(flog);
    perror("fopen");
    return NULL;
  }

  /* parsing the Bap header */
  getBapLine(bapline, fpbap);
  strcpy(bap_hdr, bapline);
  l = strtok(bapline, " ");
  strcpy(bap_version, l);
  l = strtok(NULL, " ");	// bapsource
  l = strtok(NULL, " ");
  bap_rate = atoi(l);
  l = strtok(NULL, " ");
  num_frames = atoi(l);
  offset_frames = ftell(fpbap);	// note the begining of frames
  return fpbap;
}

int sendBapHeader(FILE *fpbap, int sdudp, const struct sockaddr_in *vresa)
{
  if (sendto(sdudp, bap_hdr, strlen(bap_hdr), 0, (struct sockaddr *) vresa, sizeof(*vresa)) <0) {
    fprintf(flog,"sendto: sdudp=%d len=%d addr=%x port=%d family=%d\n", sdudp, (int) strlen(bap_hdr), vresa->sin_addr.s_addr, vresa->sin_port, vresa->sin_family); fflush(flog);
    return 0;
  }
  fprintf(flog, "sendBapHeader: %s\n", bap_hdr); fflush(flog);
  return 1;
}

int sendBapFrame(FILE *fpbap, int sdudp, const struct sockaddr_in *vresa)
{
  int index;
  char bapline[VAPS_BUFSIZ];
  char *l; 
  struct timeval delay;
  fd_set set;

  if ((index = getBapLine(bapline, fpbap)) >= 0) {	// Bap mask
    l = &(bapline[index]);
    if ((index = getBapLine(l, fpbap)) < 0) {	// values
      fprintf(flog, "bapline values not there\n"); fflush(flog);
      return -1;
    }
    l[index] = '\0';
    if (debug) { fprintf(flog, ">> %s\n", bapline); fflush(flog); }

    FD_ZERO(&set);
    FD_SET(sdudp, &set);
    delay.tv_sec = 0;
    delay.tv_usec = MAX((time_t)(1e6 / bap_rate), (time_t)(1e6 / vreng_rate));
    if (delay.tv_usec >= 1e6) {
      delay.tv_sec += (delay.tv_usec / 1000000);
      delay.tv_usec %= 1000000;
    }
    if (debug) {
      fprintf(flog, "delay=%ld.%06ld bap_rate=%d vreng_rate=%.2f\n", (long) delay.tv_sec, (long) delay.tv_usec, bap_rate, vreng_rate); fflush(flog);
    }

    select(FD_SETSIZE, NULL, NULL, NULL, &delay); // wait delay before send
    if (sendto(sdudp, bapline, strlen(bapline), 0, (struct sockaddr *) vresa, 
               sizeof(*vresa)) < 0) {
      switch (errno) {
      case EBADF: fprintf(flog,"err EBADF\n"); fflush(flog); break;
      case ENOTSOCK: fprintf(flog,"err ENOTSOCK\n"); fflush(flog); break;
      case EFAULT: fprintf(flog,"err EFAULT\n"); fflush(flog); break;
      case EMSGSIZE: fprintf(flog,"err EMSGSIZE\n"); fflush(flog); break;
      case EAGAIN: fprintf(flog,"err EAGAIN\n"); fflush(flog); break;
      case ENOBUFS: fprintf(flog,"err ENOBUFS\n"); fflush(flog); break;
      case EINTR: fprintf(flog,"err EINTR\n"); fflush(flog); break;
      case ENOMEM: fprintf(flog,"err ENOMEM\n"); fflush(flog); break;
      case EINVAL: fprintf(flog,"err EINVAL\n"); fflush(flog); break;
      case EPIPE: fprintf(flog,"err EPIPE\n"); fflush(flog); break;
      default: fprintf(flog,"err unknown\n"); fflush(flog);
      }
      fprintf(flog,"sendto: sdudp=%d len=%d addr=%x port=%d family=%d\n", sdudp, (int) strlen(bapline), vresa->sin_addr.s_addr, vresa->sin_port, vresa->sin_family); fflush(flog);
      return -1;
    }

    fprintf(flog, "sendto: bap: %s", bapline); fflush(flog);
    return 0;
  }

  else {	// end of file
#ifdef LOOPING
    fseek(fpbap, offset_frames, 0);  // rewind
#else
    fclose(fpbap);
    // send empty packet
    if (sendto(sdudp, bapline, 0, 0, (struct sockaddr *) vresa, sizeof(*vresa)) <0)
      perror("sendto eof");
#endif
    playing = 0;
    return 1;
  }
}

void waitingCommands(int sdtcp, const struct sockaddr_in *sa)
{
  int sdudp = -1;
  int len, r;
  uint32_t vreng_addr;
  uint16_t vreng_port = VAPS_PORT;  // vreng side
  uint8_t vreng_ttl = DEF_VRE_TTL;
  fd_set set;
  struct timeval tv, *unblocking;
  FILE *fpbap = NULL;
  char *l = NULL, command[BUFSIZ];
  char *bap2play = NULL;

#if HAVE_INET_PTON
  inet_pton(AF_INET, DEF_VRENG_GROUP, &vreng_addr);
#else
  vreng_addr = inet_addr(DEF_VRENG_GROUP);
#endif


  // Child commands loop
  while (1) {
    FD_ZERO(&set);
    FD_SET(sdtcp, &set);
    if (playing) {
      tv.tv_sec = tv.tv_usec = 0;
      unblocking = &tv;		// passing when playing
    }
    else
      unblocking = NULL;	// blocking when no playing

    if (select(FD_SETSIZE, &set, NULL, NULL, unblocking) != 0) {
      if ((len = recv(sdtcp, command, sizeof(command), 0)) < 0) {
        perror("vaps: recv TCP");
        sleep(1);
        continue;
      }

      command[len] = '\0';
      fprintf(flog, "command: %s\n", command); fflush(flog);

      /* setup */
      if (! strncmp(command, "setup", 5)) {
        l = strtok(command, " \t");
        while (l && (l = strtok(NULL, " \t")) != NULL) {

          switch (*l) {
          case 'a':	// addr
            if (*(++l) == '=') {
#if HAVE_INET_PTON
              inet_pton(AF_INET, ++l, &vreng_addr);
#else
              vreng_addr = inet_addr(++l);
#endif
            }
            break;

          case 'p':	// port
            if (*(++l) == '=') vreng_port = atoi(++l);
            break;

          case 't':	// ttl
            if (*(++l) == '=') {
              //vreng_ttl = atoi(++l);
              // if ttl is sent, then we assume we are in IP Multicast
              ipmode = MODE_MULTICAST;
            }
            break;

          case 'r':	// rate
            if (*(++l) == '=') {
              vreng_rate = (float) atoi(++l);

              sdudp = initUdpSender(ipmode, sa, vreng_addr, vreng_port, vreng_ttl);
              if (sdudp < 0) {
                fprintf(flog, "abort child: setup sdudp=%d\n", sdudp); fflush(flog);
                return;	// abort child
              }
              fprintf(flog, "setup port=%d sdudp=%d\n", vreng_port, sdudp); fflush(flog);
            }
            break;

          case 0:
            continue;

          default:
            if (! strncmp(l, "stop", 4)) {
              fprintf(flog, "setup default stop: %s\n", l); fflush(flog);
              doStop(sdudp);
              return;
            }
            if (! strncmp(l, "f=", 2)) {
              fprintf(flog, "setup default play: %s\n", l); fflush(flog);
              bap2play = l+2;
              continue;
            }
            fprintf(flog, "setup default: %s\n", l); fflush(flog);
          } // switch setup
        } // while setup end

        // test if play follows
        if (bap2play) {
          fprintf(flog, "bap2play: %s\n", bap2play); fflush(flog);
          bap_file = bap2play;
          bap2play = NULL;	// avoid fopen loop
          l = NULL;
          goto play2;
        }
      } // end parse setup

      /* play */
      else if (! strncmp(command, "play", 4)) {
        l = strtok(command, " \t");
        while (l && (l = strtok(NULL, " \t")) != NULL) {
          switch (*l) {
          case 'f':	// file
            if (*(++l) == '=') bap_file = ++l;
            break;
          default:
            continue;
          }
        }
play2:
        if (bap_file == NULL) {	// no bap_file given
          fprintf(flog, "play: bapfile null\n"); fflush(flog);
          if (bap2play) {
            bap_file = bap2play;
            bap2play = NULL;	// avoid fopen loop
          }
          else
            bap_file = (char*) DEF_BAP_FILE;
        }
        if (*bap_file == '\0') { // bap_file null
          closeUdpSender(sdudp);
          return;	// abort child: no bapfile
        }
        fprintf(flog, "playing %s\n", bap_file); fflush(flog);
        if ((fpbap = getBapHeader(bap_file)) == NULL) {
          closeUdpSender(sdudp);
          return;	// abort child: no bapfile
        }
        if (! sendBapHeader(fpbap, sdudp, &vrengsa)) return;
        playing = 1;
      } // end parse play

      /* stop */
      else if (! strncmp(command, "stop", 4) ||
               ! strncmp(command, "teardown", 8)) {
        doStop(sdudp);
        return;
      }

      else {
        if (len) {
	  fprintf(flog, "unknown command: %s len=%d l=%s\n", command, len, l);
          fflush(flog);
        }
        return;		// abort child: unknown command
      }
    } // end select

    // streaming bap
    if (playing) {
      r = sendBapFrame(fpbap, sdudp, &vrengsa);
      if (r < 0) {
        fprintf(flog, "bap err=%d\n", r); fflush(flog);
        fclose(fpbap);
        doStop(sdudp);	// abnormal terminaison
        return;
      }
    }
  } // end commands loop
}

void reapchild(int sig)
{
  signal(SIGCHLD, reapchild);
  while (waitpid(-1, 0, WNOHANG) > 0) ;
}

/* run in the background */
void background()
{
  if (fork()) exit(0);
}

void initVapsServer()
{
  int sock, sdtcp;
  int one = 1;
  struct sockaddr_in sa, svre;
  socklen_t slen = sizeof(struct sockaddr_in);
  struct hostent *hp;
  const char *pdst;
#if HAVE_INET_NTOP
  char dst[64];
#endif

#if HAVE_SETRLIMIT
  struct rlimit rlp;
  getrlimit(RLIMIT_NOFILE, &rlp);
  rlp.rlim_cur = rlp.rlim_max;
  setrlimit(RLIMIT_NOFILE, &rlp);
#endif
  
  /* TCP server initialisation */
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("TCP socket");
    exit(1);
  }

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(VAPS_PORT);
  sa.sin_addr.s_addr = INADDR_ANY;

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one))<0)
    perror("reuseaddr");
  if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror("binding TCP");
    exit(1);
  }

  if (!debug) background();

  signal(SIGCHLD, reapchild);
  listen(sock, 5);

  /* Server Main Loop */
  while (1) {
    if ((sdtcp = accept(sock, (struct sockaddr *) &svre, &slen)) < 0) {
      perror("vaps: accept");
      sleep(1);	// wait hoping that will be better next time...
      continue;
    } 
    switch (fork()) {
      case -1:	// failed
        close(sdtcp);
        sleep(3);
        continue;

      case 0:	// child
#if HAVE_INET_NTOP
        pdst = inet_ntop(AF_INET, &svre.sin_addr, dst, sizeof(dst));
#else
        pdst = inet_ntoa(svre.sin_addr);
#endif
        if ((hp = gethostbyaddr((const char *) &svre.sin_addr,
                                sizeof(struct in_addr), AF_INET)) == NULL)
          perror("gethostbyaddr");
        fprintf(flog, "%s %s (%s) connected\n",
                      date(), (hp) ? hp->h_name : "<unresolved>", pdst);
        fflush(flog);

	// Command Main loop for this child
        waitingCommands(sdtcp, &svre);

        // Exiting this child
        fprintf(flog, "%s %s (%s) disconnected\n",
                      date(), (hp) ? hp->h_name : "<unresolved>", pdst);
        fflush(flog);
        close(sdtcp);
        exit(0);

      default:	// parent server, always listening for accept
        close(sdtcp);
        continue;
    }
  } 
} 

void usage(char *argv[])
{
  printf("usage: %s [-?] [-h] [-D] [-v] [-d vapsdir] [-l logfile]\n", argv[0]);
  exit(0);
} 

static char *vaps_dir = NULL;

/* Parse command line args */
void parseArgs(int argc, char *argv[])
{
  int c;

  while ((c = getopt(argc, argv, "?Dhvd:l:")) != -1) {
    switch (c) {
      case '?':
      case 'h': usage(argv); break;
      case 'D': debug = true; break;
      case 'd': vaps_dir = optarg; break;
      case 'l': log_file = optarg; break;
      case 'v': printf("%s\n", PACKAGE_VERSION); exit(0);
    }
  }
  if (optind < argc) usage(argv);
}

int main(int argc, char *argv[])
{
  parseArgs(argc, argv);

  if (log_file) {
    if ((flog = fopen(log_file, "a")) == NULL)
      fprintf(stderr, "can't open log_file %s\n", log_file);
  }
  else flog = stderr;

  if (vaps_dir) {
    if (chdir(vaps_dir) < 0) {
      fprintf(stderr, "vaps: %s is not a directory\n", vaps_dir);
      exit(1);
    }
  }

  initVapsServer();
  return 0;
} 
