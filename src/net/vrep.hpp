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
#ifndef VREP_HPP
#define VREP_HPP

/* packet format */
#define RTP_HDR_SIZE		12	// RTP header length
#define RTP_VREP_TYPE		65	// vreng RTP payload type

#if 1
#define VREP3			1
#define VREP_VERSION		0x30
#define PKTSIZE			1472	// Max packet size (1500 - (20 + 8))
#else
#define VREP_VERSION		2
#define PKTSIZE			536	// Max packet size
#endif

//
// VREP3
//
//+========+========+========+========+
//|  vers  |     payload              |
//+========+                          +
//|                ...                |
//+--------+--------+--------+--------+
#define VREP_VERSION_V3		0x30
#define VREP_HDR_SIZE_V3	1

#define VREP_VERS_MASK		0x70
#define VREP_FLAGS_MASK		0x8f
#define VREP_GZIP_FLAG		1
#define VREP_GZIP_MASK		1
#define VREP_IPV4_FLAG		4
#define VREP_IPV6_FLAG		6
#define VREP_IP_MASK		6

/* Compatibility with older protocols in reception only */

//
// VREP2
//
//+========+========+========+========+
//|  vers  | family |  flags |  len   |
//+========+========+========+========+
//|              payload              |
//|                ...                |
//+--------+--------+--------+--------+
#define VREP_VERSION_V2		0x02
#define VREP_HDR_SIZE_V2	4

#define VREP_HDR_VERSION	0
#define VREP_HDR_IP		1
#define VREP_HDR_FLAGS		2
#define VREP_HDR_PLLEN		3

//
// VREP1
//
//+========+========+========+========+
//|  len   |        hostid...         |
//+--------+--------+--------+========+
//|.hostid |      portid     |payload |
//+========+========+========+--------+
//|              payload              |
//|                ...                |
//+--------+--------+--------+--------+
#define VREP_VERSION_V1		0x01
#define VREP_HDR_SIZE_V1	7

#define VREP_HDR_PLLEN_V1	0
#define VREP_HDR_SRCID_V1	1
#define VREP_HDR_PORTID_V1	5
#define RTP_VREP_TYPE_V1	105	// vreng RTP payload type

//
// VREP command types
//
#define VREP_CREATE		1
#define VREP_DELTA		2
#define VREP_QUERY		3
#define VREP_DELETE		4
#define VREP_WORLD_SET		10
#define VREP_WORLD_INFO		11
#define VREP_WORLD_RESET	12
#define VREP_ADDRESS_REQ	20
#define VREP_ADDRESS_RET	21
#define VREP_ADDRESS_REL	22
#define VREP_ADDRESS_REN	23

#define VREP_CREATE_V1	'C'
#define VREP_DELTA_V1	'd'
#define VREP_QUERY_V1	'?'
#define VREP_DELETE_V1	'D'

/* socket address indexes */
enum {
  SA_RTP,
  SA_RTCP,
  SA_RTP_R,
  SA_RTCP_R,
  SA_UDP
};

/* socket descriptor indexes */
enum {
  SD_R_RTP,
  SD_W_RTP,
  SD_R_RTCP,
  SD_W_RTCP,
  SD_R_UDP,
  SD_W_UDP = SD_R_UDP
};

#define VREP_VERS_JUMP	10

#define VRUM_PORT_OFFSET 1000
#define MCAST_PROXY	"mcast_proxy"
#define MPROXY_CTRL_PORT 3128
#define MPROXY_DATA_PORT 3129

#endif
