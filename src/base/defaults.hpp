//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
// defaults.hpp
//---------------------------------------------------------------------------
#ifndef DEFAULTS_HPP
#define DEFAULTS_HPP


#include "defines.hpp"


/* default servers */
#if STANDALONE			// by ./configure --enable-standalone
#define IPMC_ENABLED		0	// ipmulticast disabled
#define DEF_HTTP_SERVER		"localhost"
#define DEF_VJS_SERVER		"localhost"
#define DEF_VAPS_SERVER		"localhost"
#define DEF_VACS_SERVER		"localhost"
#define DEF_VRUM_SERVER		"localhost"
#define DEF_MYSQL_SERVER	"localhost"
#define DEF_PGSQL_SERVER	"localhost"
#define DEF_VNC_SERVER		"localhost"

#else	// by default

#define IPMC_ENABLED		1	// ipmulticast enabled
#define DEF_HTTP_SERVER		"localhost"
				// was "www.vreng.enst.fr" but recently unreachable
				// because it is no more maintained and hosted today.
				// The httpd server and its associated data
				// should be well known at a stable location
				// and so be reachable for everybody from everywhere
				// allowing multi-users and decentralized behavior
				// "localhost" is not the ideal solution but
				// "localhost" allows to run simply autonomously.
				// see also DEF_URL_PFX which can be tuned at your convenience.
#define DEF_VJS_SERVER		"localhost"
#define DEF_VAPS_SERVER		"localhost"
#define DEF_VACS_SERVER		"localhost"
#define DEF_VRUM_SERVER		"localhost"
#define DEF_MYSQL_SERVER	"localhost"
#define DEF_PGSQL_SERVER	"localhost"
#define DEF_VNC_SERVER		"localhost"
#endif // def servers

#define VRE_VERSION	8	// current stable version of vre files

#if !defined(DEF_URL_PFX)	// by ./configure --with-httpd-prefix
// Default Url prefix allowing access to the htdocs (data) location
// the default url prefix can be tuned to be adapted as your usage.
#define DEF_URL_PFX "~%s/vreng"	// %s represent the current user's loginname
				// this prefix may be tuned, it should be "" (original prefix).
				// It is equivalent to the HTDOCS location of your
				// local httpd server (http://httpserver/<def_url_prefix>).
				// For example:
				// if url = http://httpserver/~login/vreng
                                //   the location of data is
				//     ~/public_html/vreng/ (for Linux) or 
				//     ~/Sites/vreng/ (for MacOS)
				// where "vreng/" is equivalent to the "htdocs/" directory
				// in this distribution.
#endif //DEF_URL_PFX

// these paths are relative to the htdocs/ (data) directory
#define DEF_URL_WORLD     "/vre/Rendezvous.vre"		// location: /vre/Rendezvous.vre
#define DEF_URL_WORLD_BAK "/vre/v8/Rendezvous.vre"	// alt location: /vre/v8/Rendezvous.vre
#define DEF_URL_WORLDS    "/vre/worlds"			// list of worlds

/* default IPmulticast addresses & ports */
#define DEF_VRENG_MADDR		"224.255.0.0"
#define DEF_VRENG_PORT		52656
#define DEF_VRENG_TTL		127
#define DEF_VRENG_CHANNEL	"224.255.0.1/52666/127"
#define DEF_MANAGER_CHANNEL	"224.255.0.0/52656/127"
#define DEF_VACS_PORT		4150 + VRE_VERSION	// port of vacs server
#define DEF_SAP_ADDR		"224.2.127.254"		// address of Session Address Protocol
#define DEF_SAP_PORT		9875			// port of Session Address Protocol
#define DEF_HTTP_PORT		80			// port of http server
#define DEF_VNC_PORT		5900			// port of Vnc server
#define DEF_VAPS_PORT		4150			// port of Vaps server
#define DEF_VJS_PORT		4151			// port of Vjs server

#define DEF_RATE		50		// 50 frames/sec
#define DEF_RATIO3D		(16./9.)	// screen ratio 16/9
#define DEF_WIDTH3D		960 		// screen width
#define DEF_HEIGHT3D		540 		// screen height
#define DEF_MAXSIMCON		8		// MAX simultaneous threaded connections
#define DEF_MAXFRAMES		255		// MAX frames
#define DEF_REFRESH_TIMEOUT	1.5		//orig: 1.5

#endif
