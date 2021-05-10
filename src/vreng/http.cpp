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
#include "prof.hpp"	// new_http new_htppthread
#include "timer.hpp"	// timer
#include "cache.hpp"	// inCache
#include "universe.hpp"	// universe_name


// local
static int32_t nbsimcon;		// Nb actual of simultaneous connections
static Vpthread_mutex_t nbsimcon_mutex;	// lock on the global variable simcon
static tWaitFifo *fifofirst, *fifolast;	// variables protected by nbsimcon_mutex

static const char HTTP_PROXY[]="http_proxy";
static const char NO_PROXY[]="no_proxy";


HttpThread::HttpThread()
{
  new_httpthread++;
  waitfifo = NULL;
  begin();
}

HttpThread::~HttpThread()
{
  del_httpthread++;
  end();
}

void HttpThread::init()
{
  initMutex(&nbsimcon_mutex);
  nbsimcon = 0;
  trace(DBG_INIT, "HttpThread initialized");
}

void HttpThread::begin()
{
#if defined(HAVE_LIBPTHREAD) && defined(WITH_PTHREAD)
  if (modethr > 0) {
    //trace(DBG_HTTP, "-> begin %s", url);
    if (waitfifo) {	// Wait authorization to begin
      lockMutex(&nbsimcon_mutex);
      //------------ lock
        pthread_cond_wait(&waitfifo->cond, &nbsimcon_mutex);
        nbsimcon++; // increments nbsimcon

        /* remove element from fifo */
        fifofirst = waitfifo->next;
        if (waitfifo) delete[] waitfifo;
        waitfifo = NULL;
      //------------ unlock
      unlockMutex(&nbsimcon_mutex); // free fifo handling
    }
  }
#endif
}

void HttpThread::end()
{
#if defined(HAVE_LIBPTHREAD) && defined(WITH_PTHREAD)
  if (modethr > 0) {
    lockMutex(&nbsimcon_mutex); // lock access to global variable nbsimcon
    //------------ lock
      nbsimcon--;	// decrements nbsimcon
      if (nbsimcon < 0) nbsimcon = 0;
      if (fifofirst) {	// if something in fifo, awake it
        trace(DBG_HTTP, "thread awake (%d) %s", nbsimcon, url);
        pthread_cond_signal(&fifofirst->cond);
      }
    //------------ unlock
    unlockMutex(&nbsimcon_mutex);
  }
#endif
}

int HttpThread::putfifo()
{
#if defined(HAVE_LIBPTHREAD) && defined(WITH_PTHREAD)
  lockMutex(&nbsimcon_mutex);	// lock access to global variable nbsimcon
  //------------ lock
  if (nbsimcon >= ::g.pref.maxsimcon) {	// test number of active connections
    trace(DBG_HTTP, "too many threads=%d, waiting for %s", nbsimcon, url);

    tWaitFifo *wf = new tWaitFifo[1];		// new fifo

    pthread_cond_init(&(wf->cond), NULL);	// put thread into fifo
    wf->next = NULL;
    if (!fifofirst) fifofirst = wf;
    if (fifolast)   fifolast->next = wf;
    fifolast = wf;

    //------------ unlock
    unlockMutex(&nbsimcon_mutex);	// unlock the global variable
    waitfifo = wf;			// block the thread
  }
  else {
    /* add a connection */
    nbsimcon++;
    trace(DBG_HTTP, "thread going now (%d) %s", nbsimcon, url);
    //------------ unlock
    unlockMutex(&nbsimcon_mutex);
    waitfifo = NULL;			// thread not blocked
  }

  /* start new thread */
  Vpthread_t tid;
  return pthread_create(&tid, NULL, HttpThread::connection, (void *) this);

#else
  HttpThread::connection((void *) this);
  return 0;
#endif
}

/** Fill buffer rep */
int HttpThread::answer(int s, char *rep, int max)
{
  return recv(s, rep, max, 0);
}

/** Send request to the http server */
int HttpThread::send(int fd, const char *buf, int size)
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

/** Connect to server defined by sa */
int HttpThread::connect(const struct sockaddr_in *sa)
{
  int sdhttp;

  if ((sdhttp = Socket::openStream()) < 0) {
    perror("httpConnect: socket");
    return -BADSOCKET;
  }
  if (Socket::connection(sdhttp, sa) < 0) {
    perror("httpConnect: connect");
    return -BADCONNECT;
  }
  return sdhttp;
}

/** Converts host/port into sockaddr */
int HttpThread::resolver(char *hoststr, char *portstr, struct sockaddr_in *sa)
{
  struct hostent *hp = NULL;
  bool hack = false;

  // hostname
  if ((hp = my_gethostbyname_r(hoststr, AF_INET)) == NULL) return -BADNAME;

  uint16_t port;

  if (isdigit((int) *portstr))
    port = htons(atoi(portstr));
  else {
    if (! strcmp(portstr, "http"))
      port = htons(HTTP_PORT);
    else
      return -BADSERV;
  }

  sa->sin_family = hp->h_addrtype;
  memcpy(&sa->sin_addr, hp->h_addr_list[0], hp->h_length);
  sa->sin_port = port;
  if (! hack) my_free_hostent(hp);
  return 0;
}

static uint8_t proxy=0, noproxy=0;
static uint16_t portproxy;
static char *domnoproxy, *hostproxy;

/** Check if http proxy */
void HttpThread::checkProxy()
{
  static bool done = false;
  if (done) return;

  char *p;

  if (::g.pref.httpproxystr)
    p = ::g.pref.httpproxystr;
  else   
    p = getenv(HTTP_PROXY);  // syntax: http://hostname:port/
  if (p && *p) {
    char envproxy[90];

    hostproxy = new char[strlen(p)];
    if (p[strlen(p) - 1] == '/') p[strlen(p) - 1] = '\0';
    strcpy(envproxy, p);
    p = strrchr(envproxy, ':');
    *p = '\0';
    portproxy = atoi(++p);
    if ((p = strrchr(envproxy, '/')) == NULL)
      sprintf(hostproxy, "http://%s", envproxy);
    else
      strcpy(hostproxy, ++p);
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

int HttpThread::openFile(const char *path)
{
  return open(path, O_RDONLY);
}

/** make an http connection */
void * HttpThread::connection(void *_ht)
{
  HttpThread *ht = (HttpThread *) _ht;

  Http *http = ht->http;

  HttpThread::checkProxy();

  struct sockaddr_in sa;
  bool hterr = false;
  bool eoheader = false;
  bool answerline = true;	// position first line

  char host[MAXHOSTNAMELEN], scheme[8], path[URL_LEN], req[BUFSIZ];
  memset(host, 0, sizeof(host));
  memset(scheme, 0, sizeof(scheme));
  memset(path, 0, sizeof(path));

  int urltype = Url::parser(ht->url, host, scheme, path);
  trace(DBG_HTTP, "connection: url=%s", ht->url);
  trace(DBG_HTTP, "url=%s, universe=%s scheme=%s host=%s path=%s type:%d",
                  ::g.url, ::g.universe, scheme, host, path, urltype);

  /* which kind of URL ? */
  switch (urltype) {

  case Url::URLFILE:	// file://
    if ((http->fd = HttpThread::openFile(path)) < 0)
      hterr = true;
    else {
      http->off = -1;
      ht->httpReader(ht->thrhdl, http);
      hterr = false;
    }
    break;

  case Url::URLHTTP:	// http://
    trace(DBG_HTTP, "HTTP: %s://%s%s", scheme, host, path);
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
          hterr = false;
        }
        else {
          error("can't resolve %s err=%d", host, r);
          hterr = true;
        }
        break;
      }
    }
    if ((http->fd = HttpThread::connect(&sa)) < 0) {
      error("can't connect %s", host);
      hterr = true;
      break;
    }

    /*
     * send the GET request to the http server with adding useful infos
     */
    if (proxy && (!noproxy || strstr(host, domnoproxy) == 0)) {
      sprintf(req,
              "GET %s?version=%s&target=%s-%s%s&user=%s HTTP/1.0\r\nHost: %s\r\n\r\n",
              ht->url, PACKAGE_VERSION, ::g.env.machname(), ::g.env.sysname(), 
              ::g.env.relname(), ::g.user, host);
    }
    else {
      sprintf(req,
              "GET %s?version=%s&target=%s-%s%s&user=%s HTTP/1.0\r\nHost: %s\r\n\r\n",
              path, PACKAGE_VERSION, ::g.env.machname(), ::g.env.sysname(), 
              ::g.env.relname(), ::g.user, host);

    } 
    if (HttpThread::send(http->fd, req, strlen(req)) < 0) {
      error("can't send req=%s", req);
      hterr = true;
      break;
    }

    /*
     * parse HTTP/1.1 headers received from the server
     */

    http->reset();

    do {
      if ((http->len = HttpThread::answer(http->fd, http->buf, HTTP_BUFSIZ)) <= 0) {
        eoheader = true;
        hterr = true;
        break;
      }
      //trace(DBG_FORCE, "http answer = %d %p", http->len, http->buf);

      for (http->off = 0; 1; ) {
        int i = 0;
        char httpheader[HTTP_BUFSIZ];

        while ((http->off < http->len) && (http->buf[http->off] != '\n')) {
          httpheader[i++] = http->buf[http->off++];
        }

        if (http->off < http->len) {
          if (httpheader[0] == '\r') { // test end of header
            http->off++;
            eoheader = true;
            hterr = true;
            break;
          }
          httpheader[i-1] = '\0';	// replace '\r' by '\0'
          http->off++;			// skip '\0'
          trace(DBG_HTTP, "->%s", httpheader);

          if (answerline) {
            /* first line => get error code */
            int httperr, major, minor;

            sscanf(httpheader, "HTTP/%d.%d %d", &major, &minor, &httperr);
            trace(DBG_HTTP, "HTTP-Code_err %d (%d.%d) - %s %s",
                  httperr, major, minor, httpheader+12, ht->url);

            switch (httperr) {
            case HTTP_200:
            case HTTP_202:
              answerline = false; // answerline done
              break;
            case HTTP_301:
            case HTTP_302:
            case HTTP_307:
              {
                char *p, *q;
                if ((p = strstr(httpheader, "Location:")) != 0) {
                  if ((q = strchr(p+17, '/')) !=0) {
                    *q = '\0';
                    strcpy(host, p+17);	// redirect host
                    trace(DBG_HTTP, "redirect host = %s", host);
                    goto htagain;
                  }
                }
              }
              break;
            case 404:	// not found
              warning("HTTP-err: %d - %s %s on %s", httperr, httpheader, ht->url, host);
              hterr = true;
              break;
            case HTTP_503:
              warning("HTTP-err: %d - server %s unavailable", httperr, host);
              hterr = true;
              break;
            default:
              error("HTTP-err: %d - %s %s", httperr, httpheader+12, ht->url);
              hterr = true;
              break;
            }
          }
          if (hterr)
            break;

          if (! strncmp(httpheader, "Content-Type: ", 14)) {
            char *p, *q;

            if ((p = strchr(httpheader, '/')) != NULL) {
              p++;
              if ((q = strchr(p, ';')) != NULL) *q = '\0';
              else p[MIME_LEN] = 0;
              trace(DBG_HTTP, "mime=%s %s", p, ht->url);
              /* only for textures */
              if (ht->thrhdl && strcmp(p, "plain")) {
                Texture *tex = (Texture *) ht->thrhdl;
		tex->setMime(p);
              }
            }
          }
        }
        else {
          break;
        }
      } // end for
    } while (!eoheader);

    /*
     * Call here the appropriated httpReader
     */
    ht->httpReader(ht->thrhdl, http);
    hterr = false;
    break;

  default:	// scheme:// unknown
    if (urltype)
      error("unknown scheme urltype=%d", urltype);
    hterr = true;
    break;
  }

  if (hterr && ht) {
    ht->httpReader(ht->thrhdl, http);
  }

  // free memory
  if (ht) delete ht;
  ht = NULL;
  if (http) delete http;
  http = NULL;

  return NULL;
}

/** Opens a HTTP transaction, returns -1 if error */
int Http::httpOpen(const char *_url, void (*_httpReader)(void *h, Http *http), void *_thrhdl, int _threaded)
{
  if (! isprint(*_url)) {
    error("httpOpen: url not printable");
    return -1;
  }

  HttpThread *ht = new HttpThread();

  trace(DBG_HTTP, "httpOpen: %s", _url);
  ::g.timer.image.start();

  ht->http = new Http();	// create a http IO

  // fill the ht structure
  ht->thrhdl = _thrhdl;
  ht->httpReader = _httpReader;
  ht->modethr = _threaded;
  strcpy(ht->url, _url);
  strcpy(ht->http->url, _url);

  // Checks if url is in the cache (_threaded < 0 : don't use the cache)
  if (_threaded >= 0 && Cache::inCache(_url)) { // in cache
    ht->httpReader(ht->thrhdl, ht->http);  // call the appropiated httpReader
    if (ht->http) delete ht->http;	// segfault
    ht->http = NULL;
    progression('c');	// c as cache
    delete ht;
    ht = NULL;
    return 0;
  }
  else {		// it's a thread
    progression('i');	// i as image
    if (_threaded > 0) {
      return ht->putfifo();
    }
    else {
      HttpThread::connection((void *) ht);	// it's not a thread
      ::g.timer.image.stop();
      return 0;
    }
  }
}

//
// Http class
//
Http::Http()
{
  new_http++;
  fd = -1;
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
  if (fd > 0)
    Socket::closeStream(fd);
}

int Http::httpRead(char *abuf, int maxl)
{
  int l = 0, length;

  /* Modifie par Fabrice, l= longueur lue, maxl= longueur restante a recevoir */
  length = (off < 0) ? 0 : len - off;

  if (length > 0) {
    if (length > maxl) length = maxl;
    memcpy(abuf, buf + off, length);
    l += length;
    maxl -= length;
    off += length;
  }

  while (maxl > 0) {
    int r;
    r = read(fd, abuf+l, maxl);
    if (r < 0) {
      error("%s (%d) l=%d maxl=%d off=%d len=%d", strerror(errno), errno, l, maxl, off, len);
      return r;
    }
    if (r == 0)	break;  // eof
    l += r;
    maxl -= r;
  }
  return l;
}

static uint8_t htbuf[BUFSIZ];
static int32_t htbuf_pos = 0;
static int32_t htbuf_len = 0;

void Http::reset()
{
  htbuf_pos = htbuf_len = 0;
}

/** returns current position */
int Http::tell()
{
  return htbuf_pos;
}

/** returns a byte or an error */
int Http::getChar()
{
  if (htbuf_pos >= htbuf_len) {	// eob
    htbuf_pos = 0;
    if ((htbuf_len = httpRead((char *) htbuf, sizeof(htbuf))) == 0)
      return -1;	// eof
    if (htbuf_len < 0)
      return -2;	// err
  }
  return htbuf[htbuf_pos++];
}

/** returns a byte */
uint8_t Http::read_char()
{
  if (htbuf_pos >= htbuf_len) {
    htbuf_pos = 0;
    htbuf_len = httpRead((char *) htbuf, sizeof(htbuf));
  }
  return htbuf[htbuf_pos++];
}

/** returns an int */
int32_t Http::read_int()
{
  int32_t val;

  if (htbuf_pos >= htbuf_len) {
    htbuf_len = httpRead((char *) htbuf, sizeof(htbuf));
    htbuf_pos = 0;
  }
  val = htbuf[htbuf_pos++];
  if (htbuf_pos >= htbuf_len) {
    htbuf_len = httpRead((char *) htbuf, sizeof(htbuf));
    htbuf_pos = 0;
  }
  val |= (htbuf[htbuf_pos++] << 8);
  if (htbuf_pos >= htbuf_len) {
    htbuf_len = httpRead((char *) htbuf, sizeof(htbuf));
    htbuf_pos = 0;
  }
  val |= (htbuf[htbuf_pos++] << 16);
  if (htbuf_pos >= htbuf_len) {
    htbuf_len = httpRead((char *) htbuf, sizeof(htbuf));
    htbuf_pos = 0;
  }
  val |= (htbuf[htbuf_pos++] << 24);
  return val;
}

int32_t Http::read_short()
{
  return (read_char()<<8) | read_char();
}

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

GLfloat Http::read_GLfloat()
{
  int32_t n = read_long();
  return (GLfloat) *(GLfloat *) &n;
}

int Http::read_string(char *str, int maxlen)
{
  int c;
  int cnt = 0;

  do {
    c = read_char();
    if (cnt < maxlen)
      str[cnt] = c;
    else
      str[maxlen-1] = '\0';
    cnt++;
  } while (c != 0) ;
  /* if length of string (including \0) is odded skip another byte */
  if (cnt%2) {
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
    if (c < 0) { // eof
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
  if (*line == '#') return true;  // commented line
  if (strlen(line) == 0) return true;  // empty line
  if (isprint(*line)) return false;
  return true;
}

/** return a line non empty without comments */
bool Http::getLine(char *line)
{
  do {
    if (! nextLine(line))
      return false;
  } while (isEmptyLine(line)) ;
  return true;
}

/** returns an item */
int Http::fread(char *pbuf, int size, int nitems)
{
  int toread, length = nitems * size;

  while (length > 0) {
    if (htbuf_pos >= htbuf_len) {
      if ((htbuf_len = httpRead((char *) htbuf, sizeof(htbuf))) < 0)
        return (nitems - (length / size));
      htbuf_pos = 0;
    }
    toread = (length < (htbuf_len-htbuf_pos)) ? length : (htbuf_len-htbuf_pos);
    memcpy(pbuf, htbuf + htbuf_pos, toread);
    htbuf_pos += toread;
    pbuf += toread;
    length -= toread;
  }
  return nitems;
}

/** returns a block and its size */
uint32_t Http::read_buf(char *buffer, int maxlen)
{
  int32_t siz = htbuf_len - htbuf_pos;

  if (siz >= maxlen) {
    memcpy(buffer, htbuf, maxlen);
    htbuf_pos += maxlen;
    return maxlen;
  }
  else {
    memcpy(buffer, htbuf, siz);
    htbuf_pos = htbuf_len;
    int size = siz + httpRead((char *) buffer+siz, maxlen-siz);
    return size;
  }
}

/** skips an offset */
uint32_t Http::skip(int32_t skiplen)
{
  int32_t ptr = htbuf_len - htbuf_pos;

  if (ptr >= skiplen) {
    htbuf_pos += skiplen;
    return 0;
  }
  else {
    skiplen -= ptr;
    while (skiplen > 0) {
      if ((htbuf_len = httpRead((char *) htbuf, sizeof(htbuf))) == 0) break;
      if (skiplen >= htbuf_len) {
        skiplen -= htbuf_len;
        htbuf_pos = htbuf_len;
      }
      else {
        htbuf_pos = skiplen;
        skiplen = 0;
      }
    }
    return skiplen;
  }
}
