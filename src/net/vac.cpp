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
//
// VAC: Vreng Addresses Cache (client)
//
// should be compiled with ./configure --enable-vacs
//
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "vac.hpp"
#include "socket.hpp"	// openStream
#include "nsl.hpp"	// my_gethostbyname
#include "universe.hpp"	// MANAGER_NAME

// local
static Vac *vacList = NULL;	// vacList head
static Vac *vac = NULL;		// vac singleton


/** Constructor */
Vac::Vac()
{
  memset(url, 0, sizeof(url));
  memset(channel, 0, sizeof(channel));
  next = NULL;
  connected = false;
  sdvac = 0;
}

/** Initialization */
Vac* Vac::init()
{
  vac = new Vac();

  vac->getList();
  return (Vac *)vac;
}

// static
Vac* Vac::current()
{
  return (Vac *)vac;
}

void Vac::setConnected()
{
  connected = true;
}

/** Connect to the VACS server: return false if connect fails */
bool Vac::connectVacs()
{
  pthread_t tid;
  pthread_create(&tid, NULL, Vac::connectThread, (void * ) NULL);
  return connected;
}

/** Thread */
void * Vac::connectThread(void *)
{
  int sdvac;

  if ((sdvac = Socket::openStream()) < 0) {
    error("socket vacs");
    return NULL;
  }
  // resolve vacs address
  struct hostent *hp;
  if ((hp = my_gethostbyname(DEF_VACS_SERVER, AF_INET)) == NULL) {
    error("can't resolve vacs");
  }

  // set a timeout of 10 sec
  struct sockaddr_in savac;
  struct timeval timeout;      
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  if (setsockopt(sdvac, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)) < 0) {
    error("setsockopt failed\n");
  }

  memset(&savac, 0, sizeof(struct sockaddr_in));
  savac.sin_family = AF_INET;
  savac.sin_port = htons(DEF_VACS_PORT);
  memcpy(&savac.sin_addr, hp->h_addr_list[0], hp->h_length);
  my_free_hostent(hp);

  if (connect(sdvac, (const struct sockaddr *) &savac, sizeof(savac)) < 0) {
    perror("can't connect vacs");
    return NULL;
  }
  vac->setConnected();
  return NULL;
}

bool Vac::getList()
{
  uint32_t sizecache = 0;
  char reqvacs[8];

  if (connected == false) return false;

  strcpy(url, MANAGER_NAME);
  strcpy(channel, DEF_MANAGER_CHANNEL);

  // first get size of cache
  Vac::connectVacs();
  if (! connected)  return false;
  memset(reqvacs, 0, sizeof(reqvacs));
  sprintf(reqvacs, "size");
  write(sdvac, reqvacs, strlen(reqvacs));
  memset(reqvacs, 0, sizeof(reqvacs));
  read(sdvac, reqvacs, sizeof(reqvacs));
  sizecache = atoi(reqvacs);
  //printf("sizecache: %d\n", sizecache);

  // and then get the cache
  Vac::connectVacs();
  if (! connected)  return false;
  memset(reqvacs, 0, sizeof(reqvacs));
  sprintf(reqvacs, "list%d", VRE_VERSION);
  write(sdvac, reqvacs, strlen(reqvacs));

  vacList = this;
  next = NULL;
  Vac *prev = vacList;
  int r;

  char *cache = new char[sizecache + 1];
  memset(cache, 0, sizecache + 1);
  while ((r = read(sdvac, cache, sizecache)) > 0) {
    cache[r] = '\0';
    //DEBUG printf("readvac: r=%d,%d l=%s\n", r, strlen(cache), cache);
    char *p = strtok(cache, " ");
    while (p) {
      if (strncmp(p, "http://", 7) != 0) break;  // !!! ELC: pour eviter blocage
      Vac *vac = new Vac();
      strcpy(vac->url, p);
      p = strtok(NULL, " ");
      if (p) strcpy(vac->channel, p);
      vac->next = NULL;
      prev->next = vac;
      prev = vac;
      p = strtok(NULL, " ");
      //DEBUG printf("u=%s c=%s\n", vac->url, vac->channel);
    }
  }
  if (cache) delete[] cache;
  Socket::closeStream(sdvac);
  trace(DBG_INIT, "VacC initialized");
  return true;
}

bool Vac::resolveWorldUrl(const char *url, char *chanstr)
{
  if (connected == false) return false;

  // connect to the vacs server
  Vac::connectVacs();
  if (connected == false) return false;

  // send the request "resolve"
  char reqvacs[URL_LEN + 8];
  memset(reqvacs, 0, sizeof(reqvacs));
  sprintf(reqvacs, "resolve %s", url);
  write(sdvac, reqvacs, strlen(reqvacs));

  char line[CHAN_LEN + 2];
  memset(line, 0, sizeof(line));
  if (read(sdvac, line, sizeof(line)) > 0) {
    strcpy(chanstr, line);
    if (vacList && vacList->next) {
      bool cached = false;
      Vac *w = NULL, *wlast = NULL;
      for (w = vacList->next; w ; w = w->next) {
        if (! strcmp(url, w->url)) {
          cached = true;
          break;
        }
        wlast = w;
      }
      if (! cached) {

        Vac *vac = new Vac();

        strcpy(vac->url, url);
        strcpy(vac->channel, line);
        vac->next = NULL;
        wlast->next = vac;
        //trace(DBG_FORCE, "resolveWorld: channel=%s", vac->channel);
      }
    }
    Socket::closeStream(sdvac);
    return true;
  }
  else {
    error("resolveWorld: channel NULL");
    strcpy(chanstr, DEF_VRENG_CHANNEL);
    Socket::closeStream(sdvac);
    return false;
  }
}

bool Vac::getChannel(const char *url, char *chanstr)
{
  if (vacList && vacList->next) {
    for (Vac *w = vacList->next; w ; w = w->next) {
      if (strstr(w->url, url)) {
        strcpy(chanstr, w->channel);
        return true;
      }
    }
  }
  return false;
}

bool Vac::getUrlAndChannel(const char *name, char *url, char *chanstr)
{
  if (vacList && vacList->next) {
    for (Vac *w = vacList->next; w ; w = w->next) {
      if (strstr(w->url, name)) {
        strcpy(url, w->url);
        strcpy(chanstr, w->channel);
        return true;
      }
    }
  }
  return false;
}
