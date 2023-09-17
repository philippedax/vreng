//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
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
#ifndef HTTP_HPP
#define HTTP_HPP

#include "thread.hpp"	// Vpthread_cond_t

#define MIME_LEN        32
#define HTTP_BUFSIZ     4096
#define HTTP_200        200     // OK
#define HTTP_202        202     // accepted
#define HTTP_301        301     // move permanently
#define HTTP_302        302     // move temporarily
#define HTTP_307        307     // temporarily redirect
#define HTTP_404        404     // not found
#define HTTP_503        503     // server unavailable

/**
 * FIFO
 * if max connections is reached
 */
typedef struct sWaitfifo {
  Vpthread_cond_t cond;
  struct sWaitfifo *next;
} tWaitFifo;


/**
 * HttpThread class
 */
class HttpThread {
 friend class Http;

 private:

  /* errors */
  enum {
    TCPEOF,
    BADSEND,
    BADSOCKET,
    BADCONNECT,
    BADNAME,
    BADSERV
  };

  tWaitFifo *httpfifo;	///< wait http fifo

  void *handle;		///< thread handle
  bool modethr;		///< thread or not
  class Http *httpio;	///< http handle for I/O
  char url[URL_LEN];	///< url

  void begin_thread();	///< begins a thread
  void end_thread();	///< ends a thread

  int putfifo();
  /**< Puts thread into a wait fifo. */

  static int sendHttpd(int fd, const char *buf, int size);
  /**< Sends a http request. */

  static int answerHttpd(int s, char *rep, int max);
  /**< Receives a response from the http server. */

  static int connectHttpd(const struct sockaddr_in *sa);
  /**< Establishes a connection to the http server. */

  static void checkHttpProxy();
  /**< Checks proxy environment variables. */

 public:

  HttpThread();
  /**< Constructor. */

  virtual ~HttpThread();
  /**< Destructor. */

  void (*httpReader) (void *handle, class Http *http);
  /**< Http reader. */

  static void * connectionHttpd(void *_hthr);
  /**< Makes an http connection to the httpd server. */

  static int resolver(char *hoststr, char *portstr, struct sockaddr_in *sa);
  /**< Resolves a hostname. */

  static int openPath(const char *path);
};


/**
 * Http class
 * I/O methods
 */
class Http {
 public:
  int fd;		///< http fd
  int off;		///< offset in buf
  int len;		///< length
  char *url;		///< url
  char *buf;		///< buffer

  Http();		///< constructor

  virtual ~Http();	///< destructor

  static void init();
  /**< Initializes Http. */

  void reset();
  /**< Reset read buffer. */

  int httpRead(char *buf, int size);
  /**< Reads a block. */

  bool heof();
  /**< Returns if eof. */

  int htell();
  /**< Returns current position. */

  int getChar();
  /**< Gets a char. */

  uint8_t read_char();
  /**< Gets a byte. */

  int32_t read_short();
  /**< Gets a short. */

  int32_t read_int();
  /**< Gets an integer. */

  int32_t read_long();
  /**< Gets a short. */

  float read_float();
  /**< Gets a float. */

  int read_string(char *s, int maxlen);
  /**< Gets a string. */

  bool nextLine(char *line);
  /**< Gets nextline. */

  bool getLine(char *line);
  /**< Gets a line. */

  int fread(char *ptr, int size, int nitems);
  /**< Emulates fread. */

  uint32_t read_buf(char *buf, int maxlen);
  /**< Gets a block. */

  uint32_t skip(int32_t skiplen);
  /**< Skips a length. */

  static int httpOpen(const char *url, void (*httpReader)(void *h, Http *httpio), void *hdl, int _thread);
  /**< Opens an url
   *   and gives which reader to use
   *   and tells whether the connection is threaded or not. */
};


#endif
