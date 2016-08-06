/************************************************************************
 *
 *  usource.cpp: file and socket data management
 *  Ubit GUI Toolkit - Version 6.0
 *  (C) 1999-2008 / Eric Lecolinet / ENST Paris / www.enst.fr/~elc/ubit
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

#include <ubit/ubit_features.h>
#include <iostream>
#include <unistd.h>       // darwin
#include <sys/stat.h>
#include <ubit/uappli.hpp>
#include <ubit/uappliImpl.hpp>
#include <ubit/uon.hpp>
#include <ubit/usource.hpp>
#include <ubit/ucall.hpp>
using namespace std;
#define NAMESPACE_UBIT namespace ubit {
NAMESPACE_UBIT

USource::USource(int _source) : is_opened(false), source(_source) {
# ifdef UBIT_WITH_GDK
  gid = 0;
# endif 
  if (source > -1) open(_source);
}

USource::~USource() {
# ifdef UBIT_WITH_GDK
  if (is_opened) gdk_input_remove(gid);
  gid = 0;
# endif 
  if (is_opened) fireClose();
  is_opened = false;
}

#if UBIT_WITH_GDK
static void inputCB(gpointer input, gint source, GdkInputCondition) {
  USource* i = (USource*)input;
  i->fireInput();
}
# endif

void USource::open(int _source) {
  source = _source;
  is_opened = true;
  UElem* sources = UAppli::impl.sources;
  UChildIter i = sources->children().find(*this); 
  // ne pas mettre 2 fois dans la liste!
# if UBIT_WITH_GDK
  if (i != sources->cend()) g_source_remove(gid);
  gid = gdk_input_add(source, GDK_INPUT_READ, inputCB, (gpointer)this);
# endif
  if (i == sources->cend()) sources->add(this);
}

void USource::close() {
# if UBIT_WITH_GDK
  gdk_input_remove(gid);
  gid = 0;
# endif  
  UElem* sources = UAppli::impl.sources;
  sources->remove(*this, false);
  if (is_opened) fireClose();
  is_opened = false;
}

void USource::onAction(UCall& c) {
  UChild* l = new UChild(&c);
  l->setCond(UOn::action);
  _addAttr(*l);
}

void USource::onClose(UCall& c) {
  UChild* l = new UChild(&c);
  l->setCond(UOn::close);
  _addAttr(*l);
}

void USource::fireInput() {
  if (isDestructed()) return;  // securite
  UEvent e(UOn::action, this);  //UNodeEvent
  //e.setWhen(UAppli::getTime());
  fire(e);
}

void USource::fireClose() {
  if (isDestructed()) return;  // securite
  UEvent e(UOn::close, this);  //UNodeEvent
  //e.setWhen(UAppli::getTime());
  fire(e);
}

//==============================================================================

void UAppliImpl::resetSources(UElem* sources, fd_set& read_set, int& maxfd) {
  if (!sources) return;
  for (UChildIter c = sources->cbegin(); c != sources->cend(); ++c) {
    USource* i = static_cast<USource*>(*c);
    int fd = 0;
    if (i && ((fd = i->source) >= 0)) {
      FD_SET(fd, &read_set);
      maxfd = std::max(maxfd, fd);
    }
  }
}

void UAppliImpl::cleanSources(UElem* sources) {
  if (!sources) return;
  struct stat statbuf;
  for (UChildIter c = sources->cbegin(); c != sources->cend(); ) {
    int fd = 0;
    USource* i = static_cast<USource*>(*c);
    ++c;  // possible destruction => faire ++ avant
    if (!i) continue;
    if (!i->isOpened() || (fd=i->getSource())<=0 || fstat(fd, &statbuf) < 0) {
      i->is_opened = false;
      i->close();
    }
  }
}

void UAppliImpl::fireSources(UElem* sources, fd_set& read_set) {
  if (!sources) return;
  for (UChildIter c = sources->cbegin(); c != sources->cend(); ) {
    USource* i = static_cast<USource*>(*c);
    ++c;  // stop() peut detruire le USource => faire ++ avant
    
    if (!i || i->isDestructed()) {
      // cas malheureux ou fireInput() a detruit celui d'apres
      // => sortir sinon plantage
      return;
    }
    
    int fd = 0;
    if (i->is_opened && ((fd=i->source) >= 0) && FD_ISSET(fd,&read_set)) {
      i->fireInput();
    }
  }
}


}

