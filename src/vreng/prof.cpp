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
#include "vreng.hpp"
#include "prof.hpp"


/*** memory ***/
uint32_t new_wobject;
uint32_t del_wobject;
uint32_t new_world;
uint32_t del_world;
uint32_t new_solid;
uint32_t del_solid;
uint32_t new_texture;
uint32_t del_texture;
uint32_t new_image;
uint32_t del_image;
uint32_t new_channel;
uint32_t del_channel;
uint32_t new_session;
uint32_t del_session;
uint32_t new_source;
uint32_t del_source;
uint32_t new_payload;
uint32_t del_payload;
uint32_t new_netobject;
uint32_t del_netobject;
uint32_t new_objectlist;
uint32_t del_objectlist;
uint32_t new_netproperty;
uint32_t del_netproperty;
uint32_t new_httpthread;
uint32_t del_httpthread;
uint32_t new_http;
uint32_t del_http;
uint32_t new_imgreader;
uint32_t del_imgreader;

/*** io ***/
uint32_t cnt_open;
uint32_t cnt_close;
uint32_t cnt_open_socket;
uint32_t cnt_close_socket;


/** stat memory usage (new, delete) */
void statMemory()
{
  echo("### Memory (new/delete) ###");

  echo("WObject    : n=%8d d=%8d", new_wobject, del_wobject);
  echo("World      : n=%8d d=%8d", new_world, del_world);
  echo("Channel    : n=%8d d=%8d", new_channel, del_channel);
  echo("Session    : n=%8d d=%8d", new_session, del_session);
  echo("Source     : n=%8d d=%8d", new_source, del_source);
  echo("Solid      : n=%8d d=%8d", new_solid, del_solid);
  echo("Texture    : n=%8d d=%8d", new_texture, del_texture);
  echo("Image      : n=%8d d=%8d", new_image, del_image);
  echo("NetObject  : n=%8d d=%8d", new_netobject, del_netobject);
  echo("Payload    : n=%8d d=%8d", new_payload, del_payload);
  echo("NetProperty: n=%8d d=%8d", new_netproperty, del_netproperty);
  echo("ObjectList : n=%8d d=%8d", new_objectlist, del_objectlist);
  echo("HttpThread : n=%8d d=%8d", new_httpthread, del_httpthread);
  echo("Http       : n=%8d d=%8d", new_http, del_http);
  echo("ImgReader  : n=%8d d=%8d", new_imgreader, del_imgreader);
}

/** stat io usage (open, close) */
void statIO()
{
  echo("### IO (open/close) ###");

  echo("File       : o=%8d c=%8d", cnt_open, cnt_close);
  echo("Socket     : o=%8d c=%8d", cnt_open_socket, cnt_close_socket);
}
