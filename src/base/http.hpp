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
#define HTTP_BUFSIZ     4096	// size of buffer for io
#define HTTP_200        200     // OK
#define HTTP_202        202     // accepted
#define HTTP_301        301     // move permanently
#define HTTP_302        302     // move temporarily
#define HTTP_307        307     // temporarily redirect
#define HTTP_400        400     // bad request
#define HTTP_403        403     // forbidden
#define HTTP_404        404     // not found
#define HTTP_503        503     // server unavailable

class Http;

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
  tWaitFifo *fifo;	///< wait http fifo
  void *handle;		///< thread handle
  bool mode;		///< thread or not
  Http *http;		///< http handle for I/O
  char url[URL_LEN];	///< url

  void begin_thread();	///< begins a thread
  void end_thread();	///< ends a thread

  int putfifo();
  /**< Puts thread into a wait fifo. */

  void (*httpReader) (void *handle, class Http *http);
  /**< Http reader. */

 public:
  HttpThread();			/**< Constructor. */
  virtual ~HttpThread();	/**< Destructor. */
};


/**
 * Http class
 */
class Http {

 private:
  enum {
    TCPEOF,
    BADNAME,
    BADSERV
  };

  void *handle;		///< thread handle

  static void checkProxy();
  /**< Checks proxy environment variables. */

 public:
  // members
  int sd;		///< http fd
  int off;		///< offset in buf
  int len;		///< length
  char *buf;		///< buffer
  char *url;		///< url

  Http();		///< constructor
  virtual ~Http();	///< destructor

  // methods
  static void init();
  /**< Initializes Http. */

  static int httpOpen(const char *url, void (*httpReader)(void *h, Http *httpio), void *_handle, int _mode);
  /**< Opens an url
   *   and gives which reader to use
   *   and tells whether the connection is threaded or not. */

  static void * connection(void *_httpthread);
  /**< Makes a http connection to the httpd server. */

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
};

#endif
