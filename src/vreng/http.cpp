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
#include "http.hpp"
#include "texture.hpp"	// Texture
#include "socket.hpp"	// openStream
#include "nsl.hpp"	// my_gethostbyname
#include "thread.hpp"	// initMutex
#include "url.hpp"	// parser
#include "env.hpp"	// sysname
#include "pref.hpp"	// maxsimcon
#include "timer.hpp"	// timer
#include "cache.hpp"	// inCache
#include "universe.hpp"	// universe_name
#include "stat.hpp"	// new_http new_htppthread


// local
static int32_t nbsimcon;		// current number of simultaneous connections
static Vpthread_mutex_t nbsimcon_mutex;	// lock on the global variable simcon
static tWaitFifo *fifofirst, *fifolast;	// variables protected by nbsimcon_mutex

static const char HTTP_PROXY[] = "http_proxy";
static const char NO_PROXY[] = "no_proxy";


HttpThread::HttpThread()
{
  new_httpthread++;
  httpfifo = NULL;
  begin_thread();
}

HttpThread::~HttpThread()
{
  del_httpthread++;
  end_thread();
}

void Http::init()
{
  initMutex(&nbsimcon_mutex);
  nbsimcon = 0;
  trace(DBG_INIT, "HttpThread initialized");
}

void HttpThread::begin_thread()
{
#if defined(HAVE_LIBPTHREAD) && defined(WITH_PTHREAD)
  if (modethr > 0) {
    //trace(DBG_HTTP, "-> begin_thread %s", url);
    if (httpfifo) {	// Wait authorization to begin_thread
      lockMutex(&nbsimcon_mutex);
      //[[[ lock
        pthread_cond_wait(&httpfifo->cond, &nbsimcon_mutex);
        nbsimcon++;	// increments nbsimcon
        fifofirst = httpfifo->next;	// removes one element from the fifo
        if (httpfifo) delete[] httpfifo;
        httpfifo = NULL;
      //]]] unlock
      unlockMutex(&nbsimcon_mutex); // free fifo handling
    }
  }
#endif
}

void HttpThread::end_thread()
{
#if defined(HAVE_LIBPTHREAD) && defined(WITH_PTHREAD)
  if (modethr > 0) {
    lockMutex(&nbsimcon_mutex);	// lock access to global variable nbsimcon
    //[[[ lock
      nbsimcon--;		// decrements nbsimcon
      if (nbsimcon < 0) nbsimcon = 0;
      if (fifofirst) {		// if something in fifo, awake it
        trace(DBG_HTTP, "thread awake (%d) %s", nbsimcon, url);
        pthread_cond_signal(&fifofirst->cond);
      }
    //]]] unlock
    unlockMutex(&nbsimcon_mutex);
  }
#endif
}

int HttpThread::putfifo()
{
#if defined(HAVE_LIBPTHREAD) && defined(WITH_PTHREAD)
  lockMutex(&nbsimcon_mutex);			// lock access to global variable nbsimcon
  //[[[ lock
  if (nbsimcon >= ::g.pref.maxsimcon) {		// test number of active connections
    trace(DBG_HTTP, "too many threads=%d, waiting for %s", nbsimcon, url);
    tWaitFifo *waitfifo = new tWaitFifo[1];	// new element in the fifo
    pthread_cond_init(&(waitfifo->cond), NULL);	// put thread into fifo
    waitfifo->next = NULL;
    if (! fifofirst) fifofirst = waitfifo;
    if (fifolast) fifolast->next = waitfifo;
    fifolast = waitfifo;
    //]]] unlock
    unlockMutex(&nbsimcon_mutex);	// unlock the global variable
    httpfifo = waitfifo;		// block the thread
  }
  else {
    nbsimcon++;				// add a connection
    trace(DBG_HTTP, "thread going now (%d) %s", nbsimcon, url);
    //]]] unlock
    unlockMutex(&nbsimcon_mutex);
    httpfifo = NULL;			// thread not blocked
  }

  /* start new thread */
  Vpthread_t tid;
  return pthread_create(&tid, NULL, HttpThread::connectionHttpd, (void *) this);

#else
  HttpThread::connectionHttpd((void *) this);
  return 0;
#endif
}

/** Fills buffer rep */
int HttpThread::answerHttpd(int s, char *rep, int max)
{
  return recv(s, rep, max, 0);
}

/** Sends request to the http server */
int HttpThread::sendHttpd(int fd, const char *buf, int size)
{
  int sent, r=0;

  for (sent = 0; sent < size; sent += r) {
    if ((r = write(fd, buf + sent, size - sent)) == -1) {
      perror("httpSend: write");
      return -BADSEND;
    }
  }
  return 0;
}

/** Connects to server defined by sa */
int HttpThread::connectHttpd(const struct sockaddr_in *sa)
{
  int sdhttp;

  if ((sdhttp = Socket::openStream()) < 0) {
    perror("httpConnectHttpd: socket");
    return -BADSOCKET;
  }
  if (Socket::connection(sdhttp, sa) < 0) {
    perror("httpConnectHttpd: connect");
    return -BADCONNECT;
  }
  return sdhttp;
}

/** Converts host/port into sockaddr */
int HttpThread::resolver(char *hoststr, char *portstr, struct sockaddr_in *sa)
{
  struct hostent *hp = NULL;

  // hostname
  if ((hp = my_gethostbyname_r(hoststr, AF_INET)) == NULL) {
    return -BADNAME;
  }

  uint16_t port;

  if (isdigit((int) *portstr)) {
    port = htons(atoi(portstr));
  }
  else {
    if (! strcmp(portstr, "http")) {
      port = htons(DEF_HTTP_PORT);
    }
    else {
      return -BADSERV;
    }
  }

  sa->sin_family = hp->h_addrtype;
  memcpy(&sa->sin_addr, hp->h_addr_list[0], hp->h_length);
  sa->sin_port = port;
  my_free_hostent(hp);

  return 0;
}

static uint8_t proxy=0, noproxy=0;
static uint16_t portproxy;
static char *domnoproxy, *hostproxy;

/** Checks if http proxy */
void HttpThread::checkHttpProxy()
{
  static bool done = false;
  if (done) return;

  char *p;

  if (::g.pref.httpproxystr) {
    p = ::g.pref.httpproxystr;
  }
  else {
    p = getenv(HTTP_PROXY);  // syntax: http://hostname:port/
  }
  if (p && *p) {
    char envproxy[90];

    hostproxy = new char[strlen(p)];
    if (p[strlen(p) - 1] == '/') {
      p[strlen(p) - 1] = '\0';
    }
    strcpy(envproxy, p);
    p = strrchr(envproxy, ':');
    *p = '\0';
    portproxy = atoi(++p);
    if ((p = strrchr(envproxy, '/')) == NULL) {
      sprintf(hostproxy, "http://%s", envproxy);
    }
    else {
      strcpy(hostproxy, ++p);
    }
    proxy = 1;
    trace(DBG_HTTP, "proxy=%s:%d", hostproxy, portproxy);
  }
  p = (::g.pref.noproxystr) ? ::g.pref.noproxystr : getenv(NO_PROXY);
  if (p && *p) {
    domnoproxy = new char[strlen(p)];
    strcpy(domnoproxy, p);
    noproxy = 1;
  }
  done = true;
}

/** Opens a local file and returns its file descriptor */
int HttpThread::openPath(const char *path)
{
  return open(path, O_RDONLY);
}

/** Makes an http connection */
void * HttpThread::connectionHttpd(void *_httpthread)
{
  HttpThread *httpthread = (HttpThread *) _httpthread;

  Http *httpio = httpthread->httpio;

  HttpThread::checkHttpProxy();

  struct sockaddr_in sa;
  bool httperr = false;
  bool httpeoheader = false;
  bool answerline = true;	// position first line

  char host[MAXHOSTNAMELEN], scheme[8], path[URL_LEN], req[256];

  memset(host, 0, sizeof(host));
  memset(scheme, 0, sizeof(scheme));
  memset(path, 0, sizeof(path));

  int urltype = Url::parser(httpthread->url, host, scheme, path);
  //echo("connectionHttpd: url=%s", httpthread->url);
  trace(DBG_HTTP, "url=%s, universe=%s scheme=%s host=%s path=%s type:%d",
                  ::g.url, ::g.universe, scheme, host, path, urltype);

  /* which kind of URL ? */
  switch (urltype) {

  case Url::URLFILE:	// file://
    trace(DBG_HTTP, "HTTP: %s://%s/%s", scheme, host, path);
    if ((httpio->fd = HttpThread::openPath(path)) < 0) {
      httperr = true;
    }
    else {	// file not found
      httpio->off = -1;
      httpthread->httpReader(httpthread->handle, httpio);
      httperr = false;
    }
    break;

  case Url::URLHTTP:	// http://
    trace(DBG_HTTP, "HTTP: %s://%s/%s", scheme, host, path);

htagain:
    if (proxy && (!noproxy || strstr(host, domnoproxy) == 0)) {  // proxy
      struct hostent *hp;
      if ((hp = my_gethostbyname(hostproxy, AF_INET)) == NULL) {
        trace(DBG_HTTP, "my_gethostbyname hostproxy=%s", hostproxy);
        proxy = 0;
        noproxy = 0;
        goto htagain;
      }
      memset(&sa, 0, sizeof(struct sockaddr_in));
      sa.sin_family = AF_INET;
      sa.sin_port = htons(portproxy);
      memcpy(&sa.sin_addr, hp->h_addr_list[0], hp->h_length);
      my_free_hostent(hp);
    }
    else {
      int r;
      if ((r = HttpThread::resolver(host, scheme, &sa)) != 0) {
        if (! strncmp(host, "localhost", 9)) {
          httperr = false;
        }
        else {
          error("can't resolve %s err=%d", host, r);
          httperr = true;
        }
        break;
      }
    }
    if ((httpio->fd = HttpThread::connectHttpd(&sa)) < 0) {
      error("can't connectHttpd %s", host);
      httperr = true;
      break;
    }

    /*
     * send the GET request to the http server with adding useful infos
     */
    if (::g.pref.loghttpd) {	// more infos
      if (proxy && (!noproxy || strstr(host, domnoproxy) == 0)) {
        sprintf(req,
                "GET %s?version=%s&target=%s-%s%s&user=%s HTTP/1.0\r\nHost: %s\r\n\r\n",
                httpthread->url, PACKAGE_VERSION, ::g.env.machname(), ::g.env.sysname(), 
                ::g.env.relname(), ::g.user, host);
      }
      else {
        sprintf(req,
                "GET %s?version=%s&target=%s-%s%s&user=%s HTTP/1.0\r\nHost: %s\r\n\r\n",
                path, PACKAGE_VERSION, ::g.env.machname(), ::g.env.sysname(), 
                ::g.env.relname(), ::g.user, host);
      } 
    } 
    else {	// classic
      sprintf(req, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
    } 
    //echo("req: %s", req);
    if (HttpThread::sendHttpd(httpio->fd, req, strlen(req)) < 0) {
      error("can't sendHttpd req=%s", req);
      httperr = true;
      break;
    }

    /*
     * parses HTTP/1.1 headers received from the server
     */

    httpio->reset();

    do {
      if ((httpio->len = HttpThread::answerHttpd(httpio->fd, httpio->buf, HTTP_BUFSIZ)) <= 0) {
        httpeoheader = true;
        httperr = true;
        break;
      }
      //echo("http answerHttpd: (%d) %p", httpio->len, httpio->buf);

      for (httpio->off = 0; 1; ) {
        int i = 0;
        char httpheader[HTTP_BUFSIZ];

        while ( (httpio->off < httpio->len) && (httpio->buf[httpio->off] != '\n') ) {
          httpheader[i++] = httpio->buf[httpio->off++];
        }

        if (httpio->off < httpio->len) {
          if (httpheader[0] == '\r') { // test end of header
            httpio->off++;
            httpeoheader = true;
            httperr = true;
            break;
          }
          httpheader[i-1] = '\0';	// replace '\r' by '\0'
          httpio->off++;		// skip '\0'
          trace(DBG_HTTP, "->%s", httpheader);

          if (answerline) {
            /* first line => get error code */
            int httperr, major, minor;

            sscanf(httpheader, "HTTP/%d.%d %d", &major, &minor, &httperr);
            trace(DBG_HTTP, "HTTP-Code_err %d (%d.%d) - %s %s",
                  httperr, major, minor, httpheader+12, httpthread->url);

            switch (httperr) {

            case HTTP_200:	// good
            case HTTP_202:
              answerline = false; // answerline done
              break;

            case HTTP_301:	// transcient
            case HTTP_302:
            case HTTP_307:
              {
                char *p, *q;
                if ( (p = strstr(httpheader, "Location:")) != 0 ) {
                  if ( (q = strchr(p+17, '/')) != 0 ) {
                    *q = '\0';
                    strcpy(host, p+17);	// redirect host
                    warning("redirect host = %s", host);
                    goto htagain;
                  }
                }
              }
              break;

            case 400:		// bad request
              warning("HTTP-err: %d - %s %s on %s", httperr, httpheader, httpthread->url, host);
              httperr = true;
              break;
            case 404:		// not found
              warning("HTTP-err: %d - %s %s on %s", httperr, httpheader, httpthread->url, host);
              httperr = true;
              break;
            case HTTP_503:	// server unavailable
              warning("HTTP-err: %d - server %s unavailable", httperr, host);
              httperr = true;
              break;

            default:
              error("HTTP-err: %d - %s %s", httperr, httpheader+12, httpthread->url);
              httperr = true;
              break;
            }
          }
          if (httperr) {
            break;
          }

          // mime type
          if (! strncmp(httpheader, "Content-Type: ", 14)) {
            char *p, *q;
            if ((p = strchr(httpheader, '/')) != NULL) {
              p++;
              if ((q = strchr(p, ';')) != NULL) {
                *q = '\0';
              }
              else {
                p[MIME_LEN] = 0;
              }
              trace(DBG_HTTP, "mime=%s %s", p, httpthread->url);
              // only for textures
              if (httpthread->handle && strcmp(p, "plain")) {
                Texture *tex = (Texture *) httpthread->handle;
		tex->setMime(p);
              }
            }
          }
        }
        else {
          break;
        }
      } // end for
    } while (! httpeoheader);

    /*
     * Call here the appropriated httpReader
     */
    httpthread->httpReader(httpthread->handle, httpio);
    httperr = false;
    break;

  default:	// scheme:// unknown
    if (urltype) {
      error("unknown scheme urltype=%d", urltype);
    }
    httperr = true;
    break;
  }

  if (httperr && httpthread) {
    httpthread->httpReader(httpthread->handle, httpio);
  }

  // free memory
  if (httpthread) delete httpthread;
  httpthread = NULL;
  if (httpio) delete httpio;
  httpio = NULL;

  return NULL;
}

/////////////
//
// Http class
//

Http::Http()
{
  new_http++;
  fd = -1;		// socket descriptor
  len = off = 0;
  url = new char[URL_LEN];
  buf = new char[HTTP_BUFSIZ];
  reset();
}

Http::~Http()
{
  del_http++;
  if (url) delete[] url;
  if (buf) delete[] buf;
  buf = NULL;
  if (fd > 0) {
    Socket::closeStream(fd);
  }
}

/** Opens a HTTP transaction, returns -1 if error */
int Http::httpOpen(const char *url, void (*httpReader)(void *h, Http *http), void *handle, int threaded)
{
  if (! isprint(*url)) {
    error("httpOpen: url not printable");
    return -1;
  }

  HttpThread *httpthread = new HttpThread();

  trace(DBG_HTTP, "httpOpen: %s", url);
  ::g.timer.image.start();

  httpthread->httpio = new Http();	// create a http IO instance

  // Fills the httpthread structure
  httpthread->handle = handle;
  httpthread->httpReader = httpReader;
  httpthread->modethr = threaded;
  strcpy(httpthread->url, url);
  strcpy(httpthread->httpio->url, url);

  // Checks if url is in the cache (threaded < 0 : don't use the cache)
  if (threaded >= 0 && Cache::inCache(url)) { // in cache
    httpthread->httpReader(httpthread->handle, httpthread->httpio);  // call the appropiated httpReader
    if (httpthread->httpio) delete httpthread->httpio;	// segfault
    httpthread->httpio = NULL;
    progression('c');	// 'c' as cache
    delete httpthread;
    httpthread = NULL;
    return 0;
  }
  else {		// not in cache
    progression('i');	// 'i' as image
    if (threaded > 0) {	// is it a thread ?
      return httpthread->putfifo();	// yes, put it into fifo
    }
    else {
      HttpThread::connectionHttpd((void *) httpthread);	// it's not a thread
      ::g.timer.image.stop();
      return 0;
    }
  }
}

int Http::httpRead(char *pbuf, int maxl)
{
  int lread = 0;

  /* Modifie par Fabrice, lread= longueur lue, maxl= longueur restante a recevoir */
  int length = (off < 0) ? 0 : len - off;

  if (length > 0) {
    if (length > maxl) length = maxl;
    memcpy(pbuf, buf + off, length);
    lread += length;
    maxl -= length;
    off += length;
  }

  while (maxl > 0) {
    int r = read(fd, pbuf+lread, maxl);
    if (r < 0) {
      error("%s (%d) maxl=%d off=%d len=%d", strerror(errno), errno, maxl, off, len);
      return r;
    }
    if (r == 0)	{
      return lread;	// http eof
    }
    lread += r;
    maxl -= r;
  }
  return lread;
}

static uint8_t http_buf[BUFSIZ];
static int32_t http_pos = 0;
static int32_t http_len = 0;
static bool    http_eof = false;

void Http::reset()
{
  http_pos = http_len = 0;
  http_eof = false;
}

/** returns if http eof */
bool Http::heof()
{
  return http_eof;
}

/** returns current position */
int Http::htell()
{
  return http_pos;
}

/** returns a byte or an error */
int Http::getChar()
{
  if (http_pos >= http_len) {	// eob
    http_pos = 0;
    if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return -1;	// http eof
    }
    if (http_len < 0) {
      error("getChar: len=%d", http_len);
      http_eof = true;
      return -2;	// err
    }
  }
  return http_buf[http_pos++];
}

/** returns a byte */
uint8_t Http::read_char()
{
  if (http_pos >= http_len) {
    http_pos = 0;
    if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return -1;	// http eof
    }
  }
  return http_buf[http_pos++];
}

/** returns an int */
int32_t Http::read_int()
{
  int32_t val;

  if (http_pos >= http_len) {
    if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return -1;	// http eof
    }
    http_pos = 0;
  }
  val = http_buf[http_pos++];
  if (http_pos >= http_len) {
    if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return -1;	// http eof
    }
    http_pos = 0;
  }
  val |= (http_buf[http_pos++] << 8);
  if (http_pos >= http_len) {
    if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return -1;	// http eof
    }
    http_pos = 0;
  }
  val |= (http_buf[http_pos++] << 16);
  if (http_pos >= http_len) {
    if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return -1;	// http eof
    }
    http_pos = 0;
  }
  val |= (http_buf[http_pos++] << 24);
  return val;
}

/** returns a short */
int32_t Http::read_short()
{
  return (read_char()<<8) | read_char();
}

/** returns a long */
int32_t Http::read_long()
{
  return (read_char()<<24) | (read_char()<<16) | (read_char()<<8) | read_char();
}

/** returns a float */
float Http::read_float()
{
  int32_t n = read_long();
  return (float) *((float *) &n);
}

/** returns a string */
int Http::read_string(char *str, int maxlen)
{
  int c;
  int cnt = 0;

  do {
    c = read_char();
    if (cnt < maxlen) {
      str[cnt] = c;
    }
    else {
      str[maxlen-1] = '\0';
    }
    cnt++;
  } while (c != 0) ;
  /* if length of string (including \0) is odded skip another byte */
  if (cnt % 2) {
    read_char();
    cnt++;
  }
  return cnt;
}

/** returns true and the line else returns false */
bool Http::nextLine(char *line)
{
  int i = 0;

  while (1) {
    int c = getChar();

    if (c == '\n') break;  // eol
    if (c < 0) { // http eof
      line[i] = '\0';
      return false;
    }
    line[i++] = c;
  }
  line[i++] = '\0';	// null terminated
  return true;
}

static
bool isEmptyLine(char *line)
{
  if (*line == '#')
    return true;	// commented line
  if (strlen(line) == 0)
    return true;	// empty line
  if (isprint(*line))
    return false;	// non empty
  return true;
}

/** returns a line non empty without comments */
bool Http::getLine(char *line)
{
  do {
    if (! nextLine(line)) {
      return false;
    }
  } while (isEmptyLine(line)) ;
  return true;
}

/** returns an item */
int Http::fread(char *pbuf, int size, int nitems)
{
  int toread, len = nitems * size;

  while (len > 0) {
    if (http_pos >= http_len) {
      if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) < 0) {
        http_eof = true;
        return (nitems - (len / size));
      }
      http_pos = 0;
    }
    toread = (len < (http_len-http_pos)) ? len : (http_len-http_pos);
    memcpy(pbuf, http_buf + http_pos, toread);
    http_pos += toread;
    pbuf += toread;
    len -= toread;
  }
  return nitems;
}

/** returns a block and its size */
uint32_t Http::read_buf(char *buf, int maxlen)
{
  int32_t siz = http_len - http_pos;

  if (siz >= maxlen) {
    memcpy(buf, http_buf, maxlen);
    http_pos += maxlen;
    return (uint32_t) maxlen;
  }
  else {
    memcpy(buf, http_buf, siz);
    http_pos = http_len;
    int r = httpRead((char *) buf+siz, maxlen-siz);
    if (r == 0) {
      http_eof = true;
      return -1;	// http eof
    }
    int size = siz + r;
    return (uint32_t) size;
  }
}

/** skips an offset */
uint32_t Http::skip(int32_t skiplen)
{
  int32_t ptr = http_len - http_pos;

  if (ptr >= skiplen) {
    http_pos += skiplen;
    return 0;
  }
  else {
    skiplen -= ptr;
    while (skiplen > 0) {
      if ((http_len = httpRead((char *) http_buf, sizeof(http_buf))) == 0) {
        break;
      }
      if (skiplen >= http_len) {
        skiplen -= http_len;
        http_pos = http_len;
      }
      else {
        http_pos = skiplen;
        skiplen = 0;
      }
    }
    return (uint32_t) skiplen;
  }
}
