/************************************************************************
*
*  usocket.hpp: simple sockets
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | TELECOM ParisTech | http://www.enst.fr/~elc/ubit
*
* ***********************************************************************
* COPYRIGHT NOTICE :
* THIS PROGRAM IS DISTRIBUTED WITHOUT ANY WARRANTY AND WITHOUT EVEN THE
* IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
* YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE TERMS OF THE GNU
* GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION;
* EITHER VERSION 2 OF THE LICENSE, OR (AT YOUR OPTION) ANY LATER VERSION.
* SEE FILES 'COPYRIGHT' AND 'COPYING' FOR MORE DETAILS.
* ***********************************************************************/

#ifndef _usocket_hpp_
#define	_usocket_hpp_ 1
#include <ubit/udefs.hpp>
#include <ubit/ustr.hpp>

extern "C" {
  struct sockaddr_in;
}

namespace ubit {
  class UOutbuf;
  class UInbuf;

/* ==================================================== ===== ======= */
/** Ubit Simple Sockets.
 * Example:
 <pre><tt>
  // creates a new socket connected to host "myhost" on port 666
  USocket* s = new USocket("myhost", 666);

  // adds a callback function so that socketInputCB(s) will be called each time
  // s receives data.
  s->onInput(ucall(s, socketInputCB));

  void socketInputCB(USocket* s) {
    UInbuf ibuf;
  
    // receiveBlock() retreives the data sent by sendBlock()
    if (s->receiveBlock(ibuf)) {
    
      // ibuf.data() contains the data that has been received. It is
      // allocated by receiveBlock() and freed when ibuf is destroyed.
      char* received_data = ibuf.data();
      
      // ibuf.size() is the size of ibuf.data() (in bytes)
      unssigned int received_count = ibuf.size();

      // scans the received data according to a predefined format
      // (a short int followed by a long int in this case)
      short i; long l;
      ibuf.readShort(i); ibuf.readLong(l); ...;
   }
 }
</tt></pre>
 *
 * See also: UServerSocket.
 */
class USocket {
public:
  USocket();
  USocket(const char* remote_host, int remote_port);
  USocket(const UStr& remote_host, int remote_port);
  virtual ~USocket();

  virtual int connect(const char* remote_host, int remote_port);
  virtual void close();

  virtual void onInput(UCall&);
  ///< adds a callback that is fired when data is received on the socket.
  
  bool isConnected() const {return sock >= 0;}
  // returns the connection state of the Socket

  int getRemotePort() const {return remport;}
  int getDescriptor() const {return sock;}

  bool sendBlock(const char* buffer, unsigned short size);
  ///< see receiveBlock().

  bool sendBlock(UOutbuf&);
  ///< see receiveBlock().

  bool receiveBlock(UInbuf&);
  /**< simplified block oriented I/O.
   * this function makes it possible to send/receive blocks of data of
   * an arbitrary size. ONE call of receiveBlock() always gets ALL 
   * the data sent by ONE call of sendBlock().
   *
   * Memory is managed automatically by classes UOutbuf and UInbuf.
   * UInbuf.data() returns the data received by receiveBlock() and
   * UInbuf.size() the number of bytes. UInbuf.data() is allocated
   * and deallocated automatically in memory and should NOT be freed.
   *
   * NOTES: 
   * - the same UInbuf can be used in multiple calls of receiveBlock()
   *   (this may minimize the number of reallocations)
   *
   * - sendBlock() adds a two byte integer before the actual data
   *   to indicate its length. receiveBlock() removes this integer from
   *   the received data and uses this value to determine when the 
   *   transaction is completed.
   */

  bool sendBytes(const char* buffer, unsigned int size);
  ///< see receiveBytes().
  
  bool receiveBytes(char* buffer, unsigned int size);
  /**< byte oriented I/O.
    * - sendBytes() sends 'size' bytes from 'buffer' to the socket.
    * - receiveBytes() receives 'size' bytes from the socket and puts
    *   them into 'buffer'. 'buffer' MUST have been previoulsy allocated 
    *   and be large enough. receiveBytes() will block until 'size' bytes 
    *   have been received.
    * - both functions write/read 'size' bytes in a single call 
    *   (low-level functions send() and recv() are called until all data
    *    has been sent/received)
    */
  
protected:
  friend class UServerSocket;
  int remport, sock;
  struct sockaddr_in* sin;
  USource* input;
};

/* ==================================================== [Elc] ======= */
/** UServerSocket.
 * example:
 <pre><tt>
 // creates a ServerSocket on this port
 UServerSocket* servs = new UServerSocket(666);

 // newConnection() will be called each time 'servs' receives a connection request
 // NB: in this example, 'servs' is given as an argument to the non-member fonction
 // newConnection() via ucall(). Aternatively, newConnection() could be an instance 
 // method of an object having a UServerSocket instance, see UCall for more info.
 
 servs->onInput(ucall(servs, newConnection));

 void newConnection(UServerSocket* servs) {
     // accepts the connection and returns the corresponding socket
     USocket* s = servs->accept();
     s->onInput(ucall(s, socketInputCB));  // see USocket
     ....
 }  
 </tt></pre>
 *
 * See also: USocket.
 */
class UServerSocket {
public:
  UServerSocket();
  ///< creates an unbound server socket (bind(port) MUST then be called).
  
  UServerSocket(int port);
  ///< creates and binds a server socket (by calling bind(port, 0, true)).

  virtual ~UServerSocket();
  ///< destructor; closes this socket.

  virtual void onInput(UCall&);
  /**< adds a callback that is fired when data is received on the server socket.
   * accept() is typically called in such a callback
   */

  virtual USocket* accept();
  /**< listens for a connection to be made and accepts it.
    * This method blocks until a connection is made. It is typically called
    * in a callback function: see onInput() and class UServerSocket
    *
    * Note: accept() calls createSocket() to create the new socket.
    * By default this functions returns {new USocket}. It can be redefined
    * by UServerSocket subclasses to create appropriate socket objets.
    */

  bool bind(int port, int backlog = 0, bool reuse_address = true);
  /* binds this socket.
    * there is no need to call this function if the constructor
    * UServerSocket(int port) was used. bind() must only be called to bind
    * an unbound socket created by UServerSocket() [without an arg].
    *
    * Args:
    * - 'backlog' specifies how many pending connections the queue will hold
    *   (a default value will be used if this arg is <= 0)
    * - 'reuse_address' specifies if the same address can be reused
    */  

  virtual void close();
  ///< closes this socket.

  virtual USocket* createSocket() const {return new USocket();}
  ///< called by accept() to create the new socket (see accept() for details).

  bool isClosed() const {return listen_sock < 0;}
  ///< returns the closed state of the ServerSocket
    
  int getLocalPort()  const {return listen_port;}
  //< returns the port on which the server is listening.
  
  int getDescriptor() const {return listen_sock;}
  //< returns the socket descriptor on which the server is listening.

protected:
  int listen_port, listen_sock;
  struct sockaddr_in* sin;
  USource* input;
};

/* ==================================================== [Elc] ======= */
/** UIObuf (@see USocket).
*/
class UIObuf {
public:
  UIObuf();
  virtual ~UIObuf();

  const char* data() const;
  char* data();
  /**< returns the data in this IObuf (may be NOT null-terminated).
   * data() should NOT be freed (this is done automatically when
   * the UIObuf is destroyed).
   */

  unsigned int size() const;
  unsigned int consumed() const;

  bool resize(unsigned short);
  bool augment(unsigned short);

protected:
  friend class USocket;
  enum {DEFAULT_BUFSIZE = 512, AUGMENT_QUANTUM = 2048};
  char* buffer;
  char  default_buffer[DEFAULT_BUFSIZE];
  unsigned int inpos, outpos;  // current pos of the input and output indexes
  unsigned int bufsize;        // total memory size (int not short!)
};

/* ==================================================== [Elc] ======= */
/** UOutbuf (@see USocket).
*/
class UOutbuf : public UIObuf {
public:
  void writeChar(char);
  void writeChar(unsigned char);
  void writeShort(short);
  void writeLong(long);
  void writeString(const UStr&);
  void writeString(const char*);
  void writeString(const char* s, unsigned int len);
  void writeEvent(unsigned char event_type, unsigned char event_flow,
                  long x, long y, unsigned long detail);
};

  /* ==================================================== [Elc] ======= */
/** UInbuf (@see USocket).
*/
class UInbuf : public UIObuf {
public:
  void readChar(char&);
  void readChar(unsigned char&);
  void readShort(short&);
  void readLong(long&);
  void readString(UStr&);
  void readEvent(unsigned char& event_type, unsigned char& event_flow,
                 long& x, long& y, unsigned long& detail);
};

}
#endif



