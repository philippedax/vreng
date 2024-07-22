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
//
// http.cpp
//
// Http I/Os operations
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
#include "stat.hpp"	// new_http, del_http


// local

static int32_t nbsimcon;		// current number of simultaneous connections
static Vpthread_mutex_t nbsimcon_mutex;	// lock on the global variable simcon
static tWaitFifo *fifofirst = NULL, *fifolast = NULL;	// variables protected by nbsimcon_mutex

static uint8_t proxy=0, noproxy=0;
static uint16_t portproxy;
static char *domnoproxy, *hostproxy;
static const char HTTP_PROXY[] = "http_proxy";
static const char NO_PROXY[] = "no_proxy";


/** Begins a thread */
void Http::begin_thread()
{
  fifo = NULL;
#if defined(HAVE_LIBPTHREAD)
  if (mode > 0) {
    //echo("-> begin_thread: %s", url);
    if (fifo) {				// Wait authorization to begin_thread
      // [[ lock
      lockMutex(&nbsimcon_mutex);
      pthread_cond_wait(&fifo->cond, &nbsimcon_mutex);
      nbsimcon++;			// increments nbsimcon
      fifofirst = fifo->next;		// removes one element from the fifo
      if (fifo) delete[] fifo;
      fifo = NULL;
      unlockMutex(&nbsimcon_mutex);	// free fifo handling
      // unlock ]]
    }
  }
#endif
}

/** Ends a thread */
void Http::end_thread()
{
#if defined(HAVE_LIBPTHREAD)
  if (mode > 0) {
    // [[ lock
    lockMutex(&nbsimcon_mutex);		// lock access to global variable nbsimcon
    nbsimcon--;				// decrements nbsimcon
    nbsimcon = MAX(nbsimcon, 0);
    //echo("-> end_thread: %d %s", nbsimcon, url);
    if (fifofirst) {			// if something in fifo, awake it
      //echo("-> end_thread: thread awake (%d) %s", nbsimcon, url);
      pthread_cond_signal(&fifofirst->cond);
    }
    unlockMutex(&nbsimcon_mutex);
    // unlock ]]
  }
#endif
}

/** Puts into Fifo */
int Http::putfifo()
{
#if defined(HAVE_LIBPTHREAD)
  // [[ lock
  lockMutex(&nbsimcon_mutex);			// lock access to global variable nbsimcon
  if (nbsimcon >= ::g.pref.maxsimcon) {		// test number of active connections
    //echo("-> putfifo: too many threads=%d, waiting for %s", nbsimcon, url);
    tWaitFifo *newfifo = new tWaitFifo[1];	// new element in the fifo
    pthread_cond_init(&(newfifo->cond), NULL);	// put thread into fifo

    newfifo->next = NULL;
    if (! fifofirst)
      fifofirst = newfifo;
    if (fifolast)
      fifolast->next = newfifo;
    fifolast = newfifo;
    fifo = newfifo;				// block the thread
  }
  else {
    nbsimcon++;					// add a connection
    //echo("-> putfifo: thread going now (%d) %s", nbsimcon, url);
    fifo = NULL;				// thread not blocked
  }
  unlockMutex(&nbsimcon_mutex);			// unlock nbsimcon
  // unlock ]]

  /* Starts a new thread */
  Vpthread_t tid;
  return pthread_create(&tid, NULL, Http::connection, static_cast<void *> (this));
#else
  Http::connection(static_cast<void *> (this));
  return 0;
#endif
}

/** Constructor */
Http::Http()
{
  new_http++;
  sd = -1;		// socket descriptor
  len = off = 0;
  url = new char[URL_LEN];
  buf = new char[HTTP_BUFSIZ];
  reset();

  initMutex(&nbsimcon_mutex);
  nbsimcon = 0;
  begin_thread();
}

/** Destructor */
Http::~Http()
{
  del_http++;
  if (url) delete[] url;
  if (buf) delete[] buf;
  buf = NULL;
  if (sd > 0) {
    Socket::closeStream(sd);
  }

  end_thread();
}

/** Http initialization - static */
void Http::init()
{
  trace1(DBG_INIT, "Http initialized");
}

/** Opens a HTTP transaction, returns -1 if error - static */
int Http::httpOpen(const char *url,
                   void (*httpReader)(void *handle, Http *http),
                   void *_handle,
                   int _mode)
{
  if (! url) return -1;

  //echo("httpOpen: %s", url);
  ::g.timer.image.start();

  Http *http = new Http();		// create a http instance

  // Fills the http structure
  http->handle = _handle;
  http->httpReader = httpReader;
  http->mode = _mode;
  strcpy(http->url, url);

  // Checks if url is in the cache (_mode < 0 : don't use the cache)
  if (_mode >= 0 && Cache::inCache(url)) {	// in cache
    http->httpReader(http->handle, http);	// call the appropiated httpReader
    if (http) {
      delete http;			// segfault
    }
    progression('c');			// 'c' as cache
    http = NULL;
  }
  else {				// not in cache
    progression('i');			// 'i' as image
    if (_mode > 0) {			// is it a thread ?
      return http->putfifo();		// yes, put it into fifo
    }
    else {
      connection(static_cast<void *> (http));	// it's not a thread
    }
  }
  ::g.timer.image.stop();
  return 0;
}

/** Makes a http connection */
void * Http::connection(void *_http)
{
  Http *http = static_cast<Http *> (_http);

  checkProxy();

  struct sockaddr_in httpsa;
  struct hostent *hp = NULL;
  uint16_t port = 0;
  int err = 0;
  bool httperr = false;
  bool httpeoh = false;
  bool hanswer = true;	// position at first line

  char *host = new char[MAXHOSTNAMELEN];
  char *scheme = new char[8];
  char *path = new char[PATH_LEN];
  char *req = new char[512];

  int urltype = Url::parser(http->url, host, scheme, path);
  //echo("HTTP: %s://%s/%s", scheme, host, path);

  switch (urltype) {	// which kind of URL ?

  case Url::URLFILE:	// file://
    if ((http->sd = ::open(path, O_RDONLY)) < 0) {
      httperr = true;
    }
    else {		// file not found
      http->off = -1;
      http->httpReader(http->handle, http);
      httperr = false;
    }
    break;

  case Url::URLHTTP:	// http://
httpretry:
    if (proxy && (!noproxy || strstr(host, domnoproxy) == 0)) {  // proxy
      if ((hp = my_gethostbyname(hostproxy, AF_INET)) == NULL) {
        echo("my_gethostbyname hostproxy=%s", hostproxy);
        proxy = 0;
        noproxy = 0;
        goto httpretry;
      }
      memset(&httpsa, 0, sizeof(struct sockaddr_in));
      httpsa.sin_family = AF_INET;
      httpsa.sin_port = htons(portproxy);
      memcpy(&httpsa.sin_addr, hp->h_addr_list[0], hp->h_length);
    }
    else {		// normal
      // Resolves hostname
      //dax if ((hp = my_gethostbyname_r(host, AF_INET)) == NULL) {
      if ((hp = my_gethostbyname(host, AF_INET)) == NULL) {
        err = -BADNAME;	// not resolved
      }
      if (! strcmp(scheme, "http")) {
        port = htons(DEF_HTTP_PORT);
      }
      else {
        err = -BADSERV;
      }
      if (err < 0 && ! strcmp(host, "localhost")) {	// force localhost (not resolved)
        httpsa.sin_family = AF_INET;
        struct in_addr myip;
        inet_aton("127.0.0.1", &myip);
        httpsa.sin_addr = myip;
        err = 0;
      }
      else {
        httpsa.sin_family = hp->h_addrtype;
        memcpy(&httpsa.sin_addr, hp->h_addr_list[0], hp->h_length);
      }
      httpsa.sin_port = port;

      if (err < 0) {
        error("can't resolve %s", host);
        httperr = true;
      }

      // Opens a socket to connect
      if ((http->sd = Socket::openStream()) < 0) {
        perror("connect: socket");
        break;
      }
      if (Socket::connection(http->sd, &httpsa) < 0) {
        perror("connect: connect");
        break;
      }

      /*
       * Sends the GET request to the http server with useful infos
       */
      if (::g.pref.loghttpd) {	// more infos
        if (proxy && (!noproxy || strstr(host, domnoproxy) == 0)) {
          sprintf(req,
                  "GET %s?version=%s&target=%s-%s%s&user=%s HTTP/1.1\r\nHost: %s\r\n\r\n",
                  http->url, PACKAGE_VERSION, ::g.env.machname(), ::g.env.sysname(), ::g.env.relname(), ::g.user, host);
        }
        else {
          sprintf(req,
                  "GET %s?version=%s&target=%s-%s%s&user=%s HTTP/1.1\r\nHost: %s\r\n\r\n",
                  path, PACKAGE_VERSION, ::g.env.machname(), ::g.env.sysname(), 
                  ::g.env.relname(), ::g.user, host);
        } 
      } 
      else {		// GET classic
        sprintf(req, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
      } 
      //echo("reqGet: %s", req);

      // Sends the request
      if (::write(http->sd, req, strlen(req)) < 0) {
        error("can't send req=%s", req);
        httperr = true;
        break;
      }

      /*
       * Parses HTTP/1.1 header received from the httpd server
       */
      http->reset();

      do {
        //
        // Reads a http block
        //
        http->len = ::recv(http->sd, http->buf, HTTP_BUFSIZ, 0);
        if (http->len <= 0) {
          httpeoh = true;
          httperr = true;
          break;
        }
        //echo("recv: (%d) %p", http->len, http->buf);

        for (http->off = 0; 1; ) {
          int i = 0;
          char httpheader[HTTP_BUFSIZ];

          while ( (http->off < http->len) && (http->buf[http->off] != '\n') ) {
            httpheader[i++] = http->buf[http->off++];
          }

          if (http->off < http->len) {
            if (httpheader[0] == '\r') { // test end of header
              http->off++;
              httpeoh = true;
              httperr = true;
              break;
            }
            httpheader[i-1] = '\0';	// replace '\r' by '\0'
            http->off++;		// skip '\0'
            //echo("->%s", httpheader);

            if (hanswer) {
              int herr, hmajor, hminor;

              sscanf(httpheader, "HTTP/%d.%d %d", &hmajor, &hminor, &herr);
              trace1(DBG_HTTP, "HTTP-Code_err %d (%d.%d) - %s %s",
                    herr, hmajor, hminor, httpheader+12, http->url);

              switch (herr) {
              case HTTP_200:		// good
              case HTTP_202:
                hanswer = false;	// answer done
                break;
              case HTTP_301:		// transcient
              case HTTP_302:
              case HTTP_307: {
                  char *p, *q;
                  if ( (p = strstr(httpheader, "Location:")) != 0 ) {
                    if ( (q = strchr(p+17, '/')) != 0 ) {
                      *q = '\0';
                      strcpy(host, p+17);	// redirect host
                      goto httpretry;
                    }
                  }
                }
                break;
              case HTTP_400:		// bad request
              case HTTP_403:		// forbidden
              case HTTP_404:		// not found
                error("HTTP-err: %d - %s %s on %s", herr, httpheader, http->url, host);
                httperr = true;
                break;
              case HTTP_503:		// server unavailable
                error("HTTP-err: %d - server %s unavailable", herr, host);
                httperr = true;
                break;
              default:
                error("HTTP-err: %d - %s %s", herr, httpheader+12, http->url);
                httperr = true;
                break;
              }
            }
            if (httperr) {
              break;
            }
#if 0 //notused mime
            // Extracts mime type
            if (! strncmp(httpheader, "Content-Type: ", 14)) {
              char *p, *q;
              if ((p = strchr(httpheader, '/')) != NULL) {
                if ((q = strchr(++p, ';')) != NULL) {
                  *q = '\0';
                }
                else {
                  p[MIME_LEN] = '\0';
                }
                //echo("mime=%s %s", p, http->url);
                // only for textures
                if (http->handle && strcmp(p, "plain")) {
                  Texture *tex = static_cast<Texture *> (http->handle);
                  tex->setMime(p);
                }
              }
            }
#endif //notused
          }
          else {		// end of block
            break;
          }
        } // end for
      } while (! httpeoh);	// end do

      /*
       * Calls the appropriated httpReader
       */
      http->httpReader(http->handle, http);
      httperr = false;
      break;

    default:	// scheme:// unknown
      if (urltype) {
        error("unknown scheme urltype=%d", urltype);
      }
      httperr = true;
      break;
    } //end else normal
  } //end switch(urltype)

  if (httperr && http) {
    http->httpReader(http->handle, http);
  }

  // Frees memory
  if (host) delete[] host;
  if (scheme) delete[] scheme;
  if (path) delete[] path;
  if (req) delete[] req;
  if (hp) my_free_hostent(hp);
  if (http) delete http;
  http = NULL;
  return NULL;
}

/** Returns number of bytes read */
int Http::httpRead(char *pbuf, int maxl)
{
  int lread = 0;

  // modified by Fabrice, lread= read length, maxl= remain length to receive
  int length = (off < 0) ? 0 : len - off;

  if (length > 0) {
    if (length > maxl) length = maxl;
    memcpy(pbuf, buf + off, length);
    lread += length;
    maxl -= length;
    off += length;
  }

  while (maxl > 0) {
    int r = ::read(sd, pbuf+lread, maxl);
    if (r < 0) {
      error("%s (%d) maxl=%d off=%d", strerror(errno), errno, maxl, off);
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

//
// Http IOs methods
//

static char    http_buf[BUFSIZ];
static int32_t http_pos = 0;
static int32_t http_len = 0;
static bool    http_eof = false;

/** Resets http */
void Http::reset()
{
  http_pos = http_len = 0;
  http_eof = false;
}

/** Returns if http eof */
bool Http::heof()
{
  return http_eof;
}

/** Returns a http byte */
int Http::read_char()
{
  if (http_pos >= http_len) {	// eob
    http_pos = 0;
    if ((http_len = httpRead((http_buf), sizeof(http_buf))) == 0) {
      http_eof = true;
      return 0;		// http eof
    }
  }
  return http_buf[http_pos++];
}

/** Returns a block and its size else 0 if eof */
int Http::readHttp(char *buf, int maxlen)
{
  int32_t siz = http_len - http_pos;

  if (siz >= maxlen) {
    memcpy(buf, http_buf, maxlen);
    http_pos += maxlen;
    return maxlen;
  }
  else {
    memcpy(buf, http_buf, siz);
    http_pos = http_len;
    int r = httpRead(buf+siz, maxlen-siz);
    if (r == 0) {
      http_eof = true;
      return 0;	// http eof
    }
    uint32_t size = siz + r;
    return size;
  }
}

/** Returns true and the line else returns false */
bool Http::nextLine(char *line)
{
  int i = 0;

  for (i = 0; ; i++) {
    int c = read_char();

    if (c == '\n') break;	// eol
    if (c < 0 || heof()) {	// http eof
      line[i] = '\0';
      return false;		// end of file
    }
    line[i] = c;
  }
  line[i] = '\0';	// null terminated
  return true;
}

/** Checks if http proxy - static */
void Http::checkProxy()
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
    trace1(DBG_HTTP, "proxy=%s:%d", hostproxy, portproxy);
  }
  p = (::g.pref.noproxystr) ? ::g.pref.noproxystr : getenv(NO_PROXY);
  if (p && *p) {
    domnoproxy = new char[strlen(p)];
    strcpy(domnoproxy, p);
    noproxy = 1;
  }
  done = true;
}

#if 0 //notused --------------------------------------------------------------

/** Returns current position */
int Http::htell()
{
  return http_pos;
}

/** returns a byte or an error */
int Http::getChar()
{
  if (http_pos >= http_len) {	// eob
    http_pos = 0;
    if ((http_len = httpRead(http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return 0;		// http eof
    }
    if (http_len < 0) {
      error("getChar: len=%d", http_len);
      http_eof = true;
      return -1;	// err
    }
  }
  return http_buf[http_pos++];
}

/** Returns an int */
int32_t Http::read_int()
{
  int32_t val;

  if (http_pos >= http_len) {
    if ((http_len = httpRead(http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return 0;		// http eof
    }
    http_pos = 0;
  }
  val = http_buf[http_pos++];
  if (http_pos >= http_len) {
    if ((http_len = httpRead(http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return 0;		// http eof
    }
    http_pos = 0;
  }
  val |= (http_buf[http_pos++] << 8);
  if (http_pos >= http_len) {
    if ((http_len = httpRead(http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return 0;		// http eof
    }
    http_pos = 0;
  }
  val |= (http_buf[http_pos++] << 16);
  if (http_pos >= http_len) {
    if ((http_len = httpRead(http_buf, sizeof(http_buf))) == 0) {
      http_eof = true;
      return 0;		// http eof
    }
    http_pos = 0;
  }
  val |= (http_buf[http_pos++] << 24);
  return val;
}

/** Returns a long */
int32_t Http::read_long()
{
  return (read_char()<<24) | (read_char()<<16) | (read_char()<<8) | read_char();
}

/** Returns a float */
float Http::read_float()
{
  int32_t n = read_long();
  return static_cast<float> (*((float *) &n));
}

/** Returns a short */
int32_t Http::read_short()
{
  return (read_char()<<8) | read_char();
}

/** Returns a string */
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
      break;
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

/** Skips an offset */
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
      if ((http_len = httpRead(http_buf, sizeof(http_buf))) == 0) {
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

/** Returns an item */
int Http::fread(char *pbuf, int size, int nitems)
{
  int toread, len = nitems * size;

  while (len > 0) {
    if (http_pos >= http_len) {
      if ((http_len = httpRead(http_buf, sizeof(http_buf))) < 0) {
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

/** Returns a line non empty without comments */
bool Http::getLine(char *line)
{
  do {
    if (! nextLine(line)) {
      return false;
    }
  } while (isEmptyLine(line)) ;
  return true;
}

#endif //notused --------------------------------------------------------------
