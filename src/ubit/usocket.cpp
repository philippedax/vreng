/*************************************************************************
 *
 *  usocket.cpp: simple sockets
 *  Ubit GUI Toolkit - Version 6
 *  (C) 2009 | Eric Lecolinet | ENST Paris | www.enst.fr/~elc/ubit
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

//#include "config.h"  // HAVE_STDINT_H + socklen_t (needed on Linux but doesn't exist on BSD)
#include <ubit/ubit_features.h>
#ifdef HAVE_STDINT_H
#  include <stdint.h> // uint16_t etc.
#endif
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ubit/ucall.hpp>
#include <ubit/uappli.hpp>
#include <ubit/usource.hpp>
#include <ubit/usocket.hpp>
using namespace std;
namespace ubit {

// NOTE: attention aux possibles abort() sur un SIGPIPE: ce n'est pas gere
// ici, par contre ca l'est dans UMS via un signal(SIGPIPE)

const int DEFAULT_BACKLOG = 20;

/* ==================================================== ====== ======= */

UServerSocket::UServerSocket() :
listen_port(-1),
listen_sock(-1),
sin(new sockaddr_in()),
input(null) {
}

UServerSocket::UServerSocket(int _port) :
  listen_port(_port),
  listen_sock(-1),
  sin(new sockaddr_in()),
  input(null)
{
  bind(listen_port, 0, true);
}

bool UServerSocket::bind(int port, int backlog, bool reuse_address) {
  listen_port = port;
  listen_sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0) {
    //cerr << "UServerSocket: can't create listen socket" << endl;
    return false;
  }

  if (listen_port < 0) listen_port = 0;
  sin->sin_family = AF_INET;          // host byte order
  sin->sin_addr.s_addr = INADDR_ANY;  // short, network byte order
  sin->sin_port = htons(listen_port);
  //memset(&(sockaddr.sin_zero),'\0',8); //plante! zero the rest of the struct

  // pour reutiliser plusieurs fois la meme adresse
  int tmp = int(reuse_address);
  ::setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,(char*)&tmp,sizeof(tmp));
  
  if (::bind(listen_sock, (struct sockaddr*)sin, sizeof(*sin)) < 0) {
    //cerr << "UServerSocket: port " << listen_port << " busy" << endl;
    if (!reuse_address) return false;
  }

  // socklen_t required on Linux but doesn't exist on BSD
#ifdef HAVE_NO_SOCKLEN_T
  int taille = sizeof sin;
#else
  socklen_t taille = sizeof sin;
#endif

  // le serveur se met en attente sur le socket d'ecoute
  if (backlog <= 0) backlog = DEFAULT_BACKLOG;
  ::listen(listen_sock, backlog);

  // verifications sur le serveur
  if (::getsockname(listen_sock, (struct sockaddr*)sin, &taille) < 0) {
    //cerr << "UServerSocket: fatal error on getsocketname" << endl;
    listen_sock = -1;
    return false;
  }

  int nport = ntohs(sin->sin_port);
  /*if (nport != listen_port)*/ listen_port = nport;
  return true;
}

void UServerSocket::onInput(UCall& c) {
  if (!input) input = new USource();
  input->onInput(c);
  if (listen_sock >= 0) input->open(listen_sock);
}

//void UServerSocket::acceptOnInput() {
  //  onInput(ucall(this, &UServerSocket::accept));
// ne sert a rien car de toute facon il faut faire qq chose sur le socket
// (typiquement un  onInput(ucall(socket, qqchose))
//}

UServerSocket::~UServerSocket() {
  close();
  delete sin;
}

void UServerSocket::close() {
  if (listen_sock >= 0) {
    //::shutdown(sock, 2); ??
    ::close(listen_sock);
  }
  listen_sock = -1;  // indiquer com_sock inutilisable par write/read*()

  delete input;
  input = null;
}


USocket* UServerSocket::accept() {
  int sock_com = -1;

  // cf. man -s 3n accept, attention EINTR ou EWOULBLOCK ne sont pas geres!
  if ((sock_com = ::accept(listen_sock, NULL, NULL)) == -1) {
    //cerr << "UServerSocket: error on accept" << endl;
    return null;
  }

  // turn off TCP coalescence for INET sockets
  // NB: necessaire (au moins) pour MacOS, sinon delai de transmission
  int tmp = 1;
  ::setsockopt(sock_com, IPPROTO_TCP, TCP_NODELAY, (char*)&tmp,sizeof(int));

  USocket* s = createSocket();
  s->sock = sock_com;
  return s;
}

// ============================================================== ====== =======

USocket::USocket()
: remport(0), sock(-1), sin(null), input(null) {}

USocket::USocket(const char* _host, int _port) :
remport(0), sock(-1), sin(null), input(null) {
  connect(_host, _port);
}

USocket::USocket(const UStr& _host, int _port) :
remport(0), sock(-1), sin(null), input(null) {
  connect(_host.c_str(), _port);
}

USocket::~USocket() {
  close();  // en fait close fait delete input !
  delete input;
}

void USocket::onInput(UCall& c) {
  if (!input) input = new USource();
  input->onInput(c);
  if (sock >= 0) input->open(sock);  
}
  
void USocket::close() {
  if (sock >= 0) {
    ::shutdown(sock, 2);
    ::close(sock);
  }
  sock = -1;  // indiquer com_sock inutilisable par write/read*()
  delete sin; sin = null;
  delete input; input = null;  //9aug05: = null etait oublie !
}
  
int USocket::connect(const char* host, int port) {
  close();
  if (!host) host = "localhost";
  remport = port;
  
  // creer l'adresse du remote host a partir de son nom (getipnodebyname)
  struct hostent* hostinfo = gethostbyname(host);
  if (!hostinfo) return (sock = -2);

  sin = new sockaddr_in();
  sin->sin_family = AF_INET;
  sin->sin_port = htons(remport);
  memcpy(&sin->sin_addr, hostinfo->h_addr_list[0], hostinfo->h_length);
  //memcpy(&rhost->sin_zero, 0, 8);

  // create the socket
  sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return (sock = -1);

  // turn off TCP coalescence for INET sockets
  // NB: necessaire (au moins) pour MacOS, sinon delai de transmission
  int tmp = 1;
  ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,(char*)&tmp, sizeof(int));  // A REVOIR (option)

  // connection with the remote host
  if (::connect(sock, (struct sockaddr*)sin, sizeof(*sin)) < 0){
    //UAppli::error("UMSclient::openComSocket",
    //	   "Can't connect port %d of remote host %s", remote_port, remote_server);
    return (sock = -1);
  }

  if (input) input->open(sock);  
  return sock;
}

bool USocket::sendBytes(const char* data, unsigned int size) {
  if (sock < 0) return false;
  
  // !!! ici il faut une boucle car il se peut qu'il faille plusieurs write !!! A REVOIR
  // de plus si rec.pos est trop grand il faudrait decomposer

  if (::write(sock, data, size) < 0) {
    sock = -1;   // error  dangereux: 0 serait preferable ?
    return false;
  }
  return true;
}

bool USocket::sendBlock(const char* data, unsigned short size) {
  if (sock < 0) return false;

  uint16_t net_size = htons(size); // sends the size of the packet first
  if (::write(sock, &net_size, 2) != 2) {
    sock = -1;  // error
    return false;
  }
  return sendBytes(data, size);
}

bool USocket::sendBlock(UOutbuf& ob) {
  if (sock < 0) return false;

  uint16_t net_size = htons(ob.outpos);
  memcpy(ob.buffer, &net_size, 2);  // stores the size of the packet first
  
  return sendBytes(ob.buffer, ob.outpos+2);  // +2 for the size
}

bool USocket::receiveBytes(char* data, unsigned int size) {
  if (sock < 0) return false;
  unsigned int received = 0;

  // attention, plusieurs read() peuvent etre necessaires
  while (1) {
    int n = ::read(sock, data + received, size-received);
    if (n < 0) {
      close();   //error
      return false;
    }
    else if (n == 0) {  // the remote side has closed the connection
      close();
      return false;
    }
    else {
      received += n;
      if (received >= size) break;
    }
  }
  return true;
}

/*
bool USocket::receiveBlock(char*& data, unsigned short& size) {
  if (sock < 0) return false;

  uint16_t net_size = 0;   // get block size
  if (!receiveBytes((char*)&net_size, 2)) return false;

  size = ntohs(net_size);
  data = (char*) realloc(data, size);
  if (!data) return false;
  
  return receiveBytes(data, size);
}
*/
// NB: UInbuf contient egalement 2 bytes de size au debut, ce qui n'est
// pas indispensable mais permet un traitement symetrique

bool USocket::receiveBlock(UInbuf& buf) {
  if (sock < 0) return false;

  uint16_t net_size = 0;   // get block size
  if (!receiveBytes((char*)&net_size, 2)) return false;

  unsigned short size = ntohs(net_size);
  // !!NB: un peu absurde: ne pas sauver les 2 bytes dans le buffer !
  buf.resize(size);
  buf.outpos = size +2;  //13nov04: +2 rajoute
  memcpy(buf.buffer, &net_size, 2);  // coherence
  return receiveBytes(buf.buffer+2, size);
}

/* ==================================================== ====== ======= */

UIObuf::UIObuf() {
  buffer  = default_buffer;
  bufsize = DEFAULT_BUFSIZE;
  inpos = outpos = 2;  // skip the size
}

UIObuf::~UIObuf() {
  if (buffer != default_buffer && buffer) free(buffer);
  buffer = null;
}

/*
UIObuf::UIObuf(unsigned short _size) {
  if (_size+2 <= DEFAULT_SIZE) {
    buffer  = default_buffer;
    memsize = DEFAULT_SIZE;
  }
  else {
    buffer = (char*)malloc(_size+2);
    memsize = _size+2;
  }
  pos = 2; // skip the size
}
*/
 
char* UIObuf::data() {
  return ((buffer && outpos > 2) ? buffer+2 : null);
}

const char* UIObuf::data() const {
  return ((buffer && outpos > 2) ? buffer+2 : null);
}

unsigned int UIObuf::size() const {
  return outpos > 2 ? outpos-2 : 0;
}

unsigned int UIObuf::consumed() const {
  return inpos > 2 ? inpos-2 : 0;
}

bool UIObuf::resize(unsigned short _size) {
  bufsize = _size+2;
  
  if (buffer == null) {  // should not happen
    buffer = (char*)malloc(bufsize);
  }
  else if (buffer == default_buffer) {
    if (bufsize > DEFAULT_BUFSIZE) {
      buffer = (char*)malloc(bufsize);
      memcpy(buffer, default_buffer, DEFAULT_BUFSIZE);
    }
  }
  else buffer = (char*)realloc(buffer, bufsize);

  if (!buffer) bufsize = 0;
  return (buffer!=null);
}

bool UIObuf::augment(unsigned short sz) {
  int nblocks = sz / AUGMENT_QUANTUM + 1;
  bufsize += nblocks * AUGMENT_QUANTUM;
  resize(bufsize-2);
  return (buffer && bufsize-2 <= sizeof(short));
}

/* ==================================================== ====== ======= */

void UOutbuf::writeChar(char x) {
  if (outpos >= bufsize) augment(1);
  buffer[outpos++] = x;
}

void UOutbuf::writeChar(unsigned char x) {
  if (outpos >= bufsize) augment(1);
  buffer[outpos++] = char(x);
}

void UOutbuf::writeShort(short x) {
  if (outpos+1 >= bufsize) augment(2);
  uint16_t net_x = htons((uint16_t) x);
  memcpy(&buffer[outpos], &net_x, 2);
  outpos += 2;
}

void UOutbuf::writeLong(long x) {
  if (outpos+3 >= bufsize) augment(4);
  uint32_t net_x = htonl((uint32_t) x);
  memcpy(&buffer[outpos], &net_x, 4);
  outpos += 4;
}

void UOutbuf::writeString(const char* s, unsigned int ll) {
  if (!s || !*s) return; 
  if (outpos+ll >= bufsize) augment(ll+1);
  strncpy(&buffer[outpos], s, ll);
  buffer[outpos+ll] = 0;
  outpos += ll+1;
}

void UOutbuf::writeString(const char* s) {
  if (s) writeString(s, strlen(s));
}

void UOutbuf::writeString(const UStr& s) {
  writeString(s.c_str(), s.length());
}

/* ==================================================== [Elc:] ======= */

void UInbuf::readChar(char& x) {
  x = (char)buffer[inpos++];
}

void UInbuf::readChar(unsigned char& x) {
  x = (unsigned char)buffer[inpos++];
}

void UInbuf::readShort(short& x) {
  uint16_t netl;
  memcpy(&netl, &buffer[inpos], 2);
  inpos += 2;
  x = short(htons(netl));
}

void UInbuf::readLong(long& x) {
  uint32_t netl;
  memcpy(&netl, &buffer[inpos], 4);
  inpos += 4;
  x = long(htonl(netl));
}

void UInbuf::readString(UStr& s) {
  s = "";
  // 12jan05: c'est seulement le 0 qui sert de separateur, plus space!
  int ll = 0;
  // FAUDRAIT TESTER LA TAILLE !!!!!
  while (buffer[inpos+ll] != 0 /* && buffer[inpos+ll] != ' ' */) ll++;
  s.append(buffer+inpos, ll);
  inpos += ll+1;
}

/* ==================================================== ====== ======= */

void UOutbuf::writeEvent(unsigned char event_type, unsigned char event_flow,
                         long x, long y, unsigned long detail) {
  if (outpos+13 >= bufsize) augment(14);

  buffer[outpos] = event_type;
  outpos += 1;

  buffer[outpos] = event_flow;
  outpos += 1;

  uint32_t net_x = htonl((uint32_t) x);
  memcpy(&buffer[outpos], &net_x, 4);
  outpos += 4;

  uint32_t net_y = htonl((uint32_t) y);
  memcpy(&buffer[outpos], &net_y, 4);
  outpos += 4;

  uint32_t net_d = htonl((uint32_t)detail);
  memcpy(&buffer[outpos], &net_d, 4);
  outpos += 4;
};

void UInbuf::readEvent(unsigned char& event_type, unsigned char& event_flow,
                       long& x, long& y, unsigned long& detail) {
  event_type = buffer[inpos];
  inpos += 1;

  event_flow = buffer[inpos];
  inpos += 1;

  uint32_t netl;

  memcpy(&netl, &buffer[inpos], 4);
  x = long(htonl(netl));
  inpos += 4;

  memcpy(&netl, &buffer[inpos], 4);
  y = long(htonl(netl));
  inpos += 4;

  memcpy(&netl, &buffer[inpos], 4);
  detail = (unsigned long) htonl(netl);
  inpos += 4;
};

}

