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
// stat.hpp
//---------------------------------------------------------------------------
#ifndef STAT_HPP
#define STAT_HPP


void statLog();
void statTimings(FILE *fout);
void statNetwork(FILE *fout);
void statRender(FILE *fout);
void statMemory(FILE *fout);
void statIO(FILE *fout);
void printStats(FILE *fout);
bool writeStats(const char * statsfile);
FILE * openStats(const char * statsfile);
bool getStats(FILE* fin, char *line);

void statSendPacket(int len);
uint32_t getSentPackets();
void setLostPackets(int lost);
void statSendRTP(int len);
void statSendRTCP(int len);
void statReceivePacket(int len);
void statSessionRTP(class Session *session);
void statKbps();

/*** memory ***/
extern uint32_t new_object;
extern uint32_t del_object;
extern uint32_t new_world;
extern uint32_t del_world;
extern uint32_t new_solid;
extern uint32_t del_solid;
extern uint32_t new_texture;
extern uint32_t del_texture;
extern uint32_t new_image;
extern uint32_t del_image;
extern uint32_t new_channel;
extern uint32_t del_channel;
extern uint32_t new_session;
extern uint32_t del_session;
extern uint32_t new_source;
extern uint32_t del_source;
extern uint32_t new_payload;
extern uint32_t del_payload;
extern uint32_t new_netobj;
extern uint32_t del_netobj;
extern uint32_t new_olist;
extern uint32_t del_olist;
extern uint32_t new_netprop;
extern uint32_t del_netprop;
extern uint32_t new_http;
extern uint32_t del_http;
extern uint32_t new_cache;
extern uint32_t del_cache;
extern uint32_t new_file;
extern uint32_t del_file;

/*** io ***/
extern uint32_t opn_file;
extern uint32_t cls_file;
extern uint32_t opn_sock;
extern uint32_t cls_sock;

#endif
