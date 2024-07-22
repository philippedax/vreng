//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// http.hpp
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
 * Http class
 */
class Http {
 private:
  enum {
    TCPEOF,
    BADNAME,
    BADSERV
  };

  // members
  tWaitFifo *fifo;	///< wait http fifo
  void *handle;		///< thread handle
  bool thread;		///< thread (>0) or not (<=0)

  // methods
  void begin_thread();	///< begins a thread
  void end_thread();	///< ends a thread

  int putfifo();
  /**< Puts thread into a wait fifo. */

  void (*httpReader) (void *handle, class Http *http);
  /**< Http reader. */

  static void checkProxy();
  /**< Checks proxy environment variables. */

 public:
  //
  // members
  //
  int sd;		///< http fd.
  int off;		///< offset in buf.
  int len;		///< length.
  char *buf;		///< buffer.
  char *url;		///< url.

  //
  // methods
  //
  Http();		///< constructor.
  virtual ~Http();	///< destructor.

  static void init();
  /**< Initializes Http. */

  static int httpOpen(const char *url, void (*httpReader)(void *handle, Http *http), void *_handle, int _thread);
  /**< Opens an url
   *   and gives which reader to use
   *   and tells whether the connection is threaded or not. */

  static void * connection(void *_httpthread);
  /**< Makes a http connection to the httpd server. */

  bool heof();
  /**< Returns if eof. */

  void reset();
  /**< Reset read buffer. */

  int read_char();
  /**< Gets a byte or eof. */

  int httpRead(char *buf, int size);
  /**< Reads a block. */

  bool nextLine(char *line);
  /**< Gets nextline. */

  int readHttp(char *buf, int maxlen);
  /**< Gets a block. */

  //notused int32_t read_int();
  //notused /**< Gets an integer. */
  //notused int32_t read_long();
  //notused /**< Gets a long. */
  //notused float read_float();
  //notused /**< Gets a float. */
  //notused int fread(char *ptr, int size, int nitems);
  //notused /**< Emulates fread. */
  //notused uint32_t skip(int32_t skiplen);
  //notused /**< Skips a length. */
  //notused int32_t read_short();
  //notused /**< Gets a short. */
  //notused int getChar();
  //notused /**< Gets a char. */
  //notused int read_string(char *s, int maxlen);
  //notused /**< Gets a string. */
  //notused int htell();
  //notused /**< Returns current position. */
  //notused bool getLine(char *line);
  //notused /**< Gets a line. */
};

#endif
