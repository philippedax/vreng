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

#define VRENG_MPORT_INCR	10
#define WORLD_LEN		32
#define UNIVERSE_LEN		32
#define VACS_FILE		"worlds"
#define DEF_VACS_LOG		"/var/log/vacs.log"

// internal structure
struct _vacs {
  char url[URL_LEN];
  char world[WORLD_LEN];
  char channel[CHAN_LEN];
  char universe[UNIVERSE_LEN];
  uint32_t maddr;
  uint16_t mport;
  uint8_t ttl;
  uint8_t version;
  bool qualified;
  struct _vacs *next;
};


// global
bool debugvacs = 0;
char *log_file = NULL;
FILE *flog = NULL;
uint32_t next_maddr;
uint16_t next_mport;
uint8_t vre_version = VRE_VERSION;
struct _vacs *vacsList = NULL;
char vacs_file[PATH_LEN];
uint32_t cachesize = 0;


static void quitSig(int sig)
{
  fprintf(flog, "Got signal %d, aborting Vacs.\n", sig); fflush(flog);
  exit(sig);
}


/* Init signals */
void initSignals()
{
#ifndef WIN32
  signal(SIGILL, quitSig);
  signal(SIGTRAP, quitSig);
  signal(SIGFPE, quitSig);
  signal(SIGBUS, quitSig);
  signal(SIGSEGV, quitSig);
#endif // !WIN32
}


/* Compute date */
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


/* Get a line from cache */
int getVacLine(char *line, FILE *fpvacs)
{
  do {
    if (fgets(line, BUFSIZ, fpvacs) == NULL)
      return -1;
  } while (line[0] == '#');
  line[strlen(line) -1] = 0;
  return strlen(line);
}


/* Extract universe name */
void getUniverse(const char *url, char *name)
{
  if (! strncmp(url, "http://", 7)) {
    char *u = strdup(url);
    char *p = strchr(u, '/');
    p++;
    p = strchr(p, '/');
    p++;
    char *p2 = strchr(p, '/');
    *p2 = 0;
    strcpy(name, p);      
    free(u);
  }
  else if (! strncmp(url, "file:", 5)) {
    strcpy(name, "local");      
  }
  else {
    strcpy(name, "none");      
  }
}


void getWorld(const char *url, char *name)
{
  char *u = strdup(url);
  char *p = strrchr(u, '.');
  *p = 0;
  p = strrchr(u, '/');
  p++;
  strcpy(name, p);
  free(u);
}


uint32_t getAddr(const char *channel)
{
  char *c = strdup(channel);
  char *p = strtok(c, "/");
#if HAVE_INET_PTON
  uint32_t addr;
  inet_pton(AF_INET, p, &addr);
#else
  uint32_t addr = inet_addr(p);
#endif
  free(c);
  return addr;
}


uint16_t getPort(const char *channel)
{
  char *c = strdup(channel);
  char *p = strtok(c, "/");
  p = strtok(NULL, "/");
  uint16_t port = atoi(p);
  free(c);
  return port;
}


uint8_t getTtl(const char *channel)
{
  uint8_t ttl = DEF_VRE_TTL;

  char *c = strdup(channel);
  char *p = strtok(c, "/");
  p = strtok(NULL, "/");
  p = strtok(NULL, "/");
  if (p) {
    ttl = atoi(p);
    if (ttl == 0)
      ttl = DEF_VRE_TTL;
  }
  free(c);
  return ttl;
}


/** Saves all the cache */
void saveCache()
{
  FILE *fpvacs;

  if ((fpvacs = fopen(vacs_file, "w")) == NULL) {
    perror("fopen write");
    return;
  }
  for (struct _vacs *w = vacsList->next; w ; w = w->next)
    fprintf(fpvacs, "%s\t%s\n", w->url, w->channel);
  fclose(fpvacs);
}


void sizeCache(int sdvre)
{
  char sizevacs[8];
  memset(sizevacs, 0, sizeof(sizevacs));
  sprintf(sizevacs, "%d", cachesize);
  write(sdvre, sizevacs, strlen(sizevacs));
}


void releaseCache(const char *world)
{
}


/** Resolves a world in its universe, returns channel if found */
bool resolveCache(const char *url, char *chanstr)
{
  fprintf(flog, "resolveCache: url=%s\n", url); fflush(flog);
  char world[WORLD_LEN], universe[UNIVERSE_LEN];

  getWorld(url, world);
  getUniverse(url, universe);

  for (struct _vacs *w = vacsList; w ; w = w->next) {
    if ((! strcmp(world, w->world)) && (! strcmp(universe, w->universe))) {
      char maddrstr[CHAN_LEN];
#if HAVE_INET_NTOP
      char dst[46];
      strcpy(maddrstr, inet_ntop(AF_INET, &w->maddr, dst, sizeof(dst)));
#else
      strcpy(maddrstr, inet_ntoa((struct in_addr &)w->maddr));
#endif
      sprintf(chanstr, "%s/%d/%d", maddrstr, w->mport, DEF_VRE_TTL);
      fprintf(flog, "resolveCache: chanstr=%s\n", chanstr); fflush(flog);
      return true;
    }
  }
  return false;
}


/** Adds vacs structure, appends to cache, returns new channel */
void addCache(const char *url, char *channel)
{
  fprintf(flog, "addCache: url=%s\n", url); fflush(flog);
  struct _vacs *vacs = (struct _vacs *) malloc(sizeof(_vacs));

  strcpy(vacs->url, url);			// url
  getWorld(vacs->url, vacs->world);		// extract world
  getUniverse(vacs->url, vacs->universe);	// extract universe

  if (! strncmp(vacs->url, "http://", 7)) {
    vacs->qualified = true;
  }
  else {
    vacs->qualified = false;
  }

  // maddr mport
  vacs->maddr = next_maddr;
  vacs->mport = next_mport;

  // channel
  char maddrstr[CHAN_LEN];
  memset(maddrstr, 0, sizeof(maddrstr));
#if HAVE_INET_NTOP
  char dst[46];
  strcpy(maddrstr, inet_ntop(AF_INET, &vacs->maddr, dst, sizeof(dst)));
#else
  strcpy(maddrstr, inet_ntoa((struct in_addr &)vacs->maddr));
#endif
  memset(vacs->channel, 0, sizeof(vacs->channel));
  sprintf(vacs->channel, "%s/%d/%d", maddrstr, vacs->mport, DEF_VRE_TTL); //vreng
  strcpy(channel, vacs->channel);
  sprintf(vacs->channel, "%s/%d", maddrstr, vacs->mport);	// record

  vacs->next = NULL;

  // add this entry at the end of the vacsList
  struct _vacs *wlast = NULL;
  for (struct _vacs *w = vacsList; w ; w = w->next) {
    wlast = w;
  }
  if (wlast) {
    wlast->next = vacs;
  }
  else
    fprintf(flog, "addCache: wlast NULL\n"); fflush(flog);

  // append this record to the file
  FILE *fpvacs;

  if (vacs->qualified) {
    if ((fpvacs = fopen(vacs_file, "a")) == NULL) {
      perror("fopen append");
      return;
    }
    fprintf(fpvacs, "%s\t%s\n", vacs->url, vacs->channel);
    fclose(fpvacs);
    fprintf(flog, "appended: %s\t%s\n", vacs->url, vacs->channel); fflush(flog);
  }
  else {
    fprintf(flog, "not appended: %s universe=%s\n", vacs->url, vacs->universe); fflush(flog);
  }

  // next values
  next_maddr = vacs->maddr + htonl(1);
  next_mport = vacs->mport + VRENG_MPORT_INCR;
}


/** Loads the cache file */
int loadCache()
{
  FILE *fpvacs;
  char line[BUFSIZ];
  char vacs_path[PATH_LEN];

  sprintf(vacs_path, "v%d/%s", VRE_VERSION, VACS_FILE);
  if ((fpvacs = fopen(vacs_path, "r")) == NULL) {
    perror("fopen read");
    exit(1);
  }

  // first record
  struct _vacs *vacs = (struct _vacs *) malloc(sizeof(_vacs));
  vacsList = vacs;
  strcpy(vacs->world, "Manager");
  *vacs->url = 0;
  sprintf(vacs->channel, "%s/%d/%d", VRENG_MADDR_BASE, VRENG_MPORT_BASE, DEF_VRE_TTL);
#if HAVE_INET_PTON
  inet_pton(AF_INET, VRENG_MADDR_BASE, &vacs->maddr);
#else
  vacs->maddr = inet_addr(VRENG_MADDR_BASE);
#endif
  vacs->mport = VRENG_MPORT_BASE;
  vacs->ttl = DEF_VRE_TTL;
  strcpy(vacs->universe, DEF_HTTP_SERVER);
  vacs->next = NULL;
  struct _vacs *prev = vacs;

  next_maddr = vacs->maddr + htonl(1);
  next_mport = vacs->mport + VRENG_MPORT_INCR;

  // other records
  int r = 0;
  while ((r = getVacLine(line, fpvacs) != -1)) {
    struct _vacs *vacs = (struct _vacs *) malloc(sizeof(_vacs));
    cachesize += strlen(line) + 1;

    // url
    char *p = strtok(line , " \t");
    if (!p)
      break;	// end of data

    strcpy(vacs->url, p);
    // world
    getWorld(vacs->url, vacs->world);
    // universe
    getUniverse(vacs->url, vacs->universe);

    // channel
    p = strtok(NULL, " \t");
    if (!p)
      break;	// end of data

    strcpy(vacs->channel, p);
    // maddr
    vacs->maddr = getAddr(p);
    // mport
    vacs->mport = getPort(p);
    vacs->ttl = getTtl(p);
    if (vacs->ttl == 0)
      vacs->ttl = DEF_VRE_TTL;

    // next values
    next_maddr = vacs->maddr + htonl(1);
    next_mport = vacs->mport + VRENG_MPORT_INCR;

    prev->next = vacs;
    vacs->next = NULL;
    prev = vacs;
    //fprintf(flog, "w=%s u=%s c=%s a=%x p=%d t=%d n=%x/%d\n", vacs->world, vacs->universe, vacs->channel, vacs->maddr, vacs->mport, vacs->ttl, next_maddr, next_mport); fflush(flog);
  }
  fclose(fpvacs);
  return cachesize;
}


void sendCache(int sdvre)
{
  char linevacs[URL_LEN + CHAN_LEN + 1 + 64];
  fprintf(flog, "sendCache:\n"); fflush(flog);

  for (struct _vacs *w = vacsList->next; w ; w = w->next) {
    memset(linevacs, 0, sizeof(linevacs));
    sprintf(linevacs, "%s %s/%d ", w->url, w->channel, DEF_VRE_TTL);
    write(sdvre, linevacs, strlen(linevacs));
#if HAVE_USLEEP
    //usleep(20000);	// tempo 20 ms
#endif
    //fprintf(flog, "%s %s/%d [%x]\n", w->url, w->channel, DEF_VRE_TTL, w->maddr); fflush(flog);
  }
  close(sdvre);
}


/** Handles requests from vreng client */
void parseRequest(int sdvre, const struct sockaddr_in *svre)
{
  char request[128], channel[CHAN_LEN];
  int len;

  memset(request, 0, sizeof(request));
  if ((len = read(sdvre, request, sizeof(request))) < 0) {
    perror("vacs: recv");
    return;
  }
  request[len] = '\0';
  fprintf(flog, "request: %s\n", request); fflush(flog);

  if (! strncmp(request, "list", 4)) {		// list
    char *p = request + 4;	// get the data version number
    uint8_t vers = atoi(p);
    if (vers)
      vre_version = vers;
    sprintf(vacs_file, "v%d/%s", vre_version, VACS_FILE);
    sendCache(sdvre);
  }
  else if (! strncmp(request, "resolve", 7)) {	// resolve
    char *url = strtok(request, " ");
    url = strtok(NULL, " ");
    if (resolveCache(url, channel) == 0) {
      addCache(url, channel);
    }
    channel[strlen(channel)] = 0;
    write(sdvre, channel, strlen(channel) + 1);
  }
  else if (! strncmp(request, "release", 7)) {	// release
    char *world = strtok(request, " ");
    world = strtok(NULL, " ");
    releaseCache(world);
  }
  else if (! strncmp(request, "size", 4)) {	// size
    char *world = strtok(request, " ");
    world = strtok(NULL, " ");
    sizeCache(sdvre);
  }
  else {					// unknown
    if (len != 0) {
      fprintf(flog, "unknown request: %s len=%d\n", request, len); fflush(flog);
    }
  }
  return;
}


/** Runs in the background */
void background()
{
  if (fork())
    exit(0);
}


/** Inits the server */
void initVacs()
{
  if (! debugvacs)
    background();

  initSignals();

  fprintf(flog, "%s started\n", date());
  fflush(flog);

  loadCache();

  int sdvacs, sdvre;
  struct sockaddr_in sa, svre;
  socklen_t slen = sizeof(struct sockaddr_in);

  if ((sdvacs = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(VACS_PORT);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  int one = 1;
  setsockopt(sdvacs, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one));
  if (bind(sdvacs, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror("binding");
    exit(1);
  }

  listen(sdvacs, 5);

  /* Server Main Loop */
  while (1) {
    memset(&svre, 0, sizeof(svre));
    if ((sdvre = accept(sdvacs, (struct sockaddr *) &svre, &slen)) < 0) {
      sleep(1);	// wait hoping that will be better next time...
      continue;
    } 
#if HAVE_INET_NTOP
    char dst[46];
    const char *pdst = inet_ntop(AF_INET, &svre.sin_addr, dst, sizeof(dst));
#else
    const char *pdst = inet_ntoa(svre.sin_addr);
#endif
    struct hostent *hp;
    if ((hp = gethostbyaddr((const char *) &svre.sin_addr,
                            sizeof(struct in_addr), AF_INET)) == NULL)
      perror("gethostbyaddr");
    fprintf(flog, "%s %s (%s) connected\n",
                  date(), (hp) ? hp->h_name : "<unresolved>", pdst);
    fflush(flog);

    parseRequest(sdvre, &svre);
    close(sdvre);
  } 
}


void usage(char *argv[])
{
  printf("usage: %s [-?] [-h] [-D] [-v] [-d vacsdir] [-l logfile]\n", argv[0]);
  exit(0);
} 

static char *vacs_dir = NULL;


/* Parse command line args */
void parseArgs(int argc, char *argv[])
{
  int c;

  while ((c = getopt(argc, argv, "?Dhvd:l:")) != -1) {
    switch (c) {
      case '?':
      case 'h': usage(argv); break;
      case 'D': debugvacs = true; break;
      case 'd': vacs_dir = optarg; break;
      case 'l': log_file = optarg; break;
      case 'v': printf("%s\n", PACKAGE_VERSION); exit(0);
    }
  }
  if (optind < argc) {
    usage(argv);
  }
}


int main(int argc, char *argv[])
{
  parseArgs(argc, argv);

  if (log_file) {
    if ((flog = fopen(log_file, "a")) == NULL) {
      fprintf(stderr, "can't open log_file %s\n", log_file);
    }
  }
  else
    flog = stderr;

  if (vacs_dir) {
    if (chdir(vacs_dir) < 0) {
      fprintf(stderr, "vacs: %s is not a directory\n", vacs_dir);
      exit(1);
    }
  }

  initVacs();
  return 0;
} 
