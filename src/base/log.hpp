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
//  log.hpp
//---------------------------------------------------------------------------
#ifndef LOG_HPP
#define LOG_HPP

#undef NDEBUG		// assert()
#include <assert.h>

//	symbol		bit	   value	cumul
#define DBG_INIT	0	// 1		1
#define DBG_GUI		1	// 2		3
#define DBG_WO		2	// 4		7
#define DBG_HTTP	3	// 8		15
#define DBG_IPMC	4	// 16		31
#define DBG_3D		5	// 32		63
#define DBG_2D	 	6	// 64		127
#define DBG_NET		7	// 128		255
#define DBG_RTP		8	// 256		511
#define DBG_SQL		9	// 512		1023
#define DBG_COL		10	// 1024		2047
#define DBG_IFC		11	// 2048		4095
#define DBG_MOD		12	// 4096		8191
#define DBG_MAN 	13	// 8192		16383
#define DBG_VNC		14	// 16384	32767
#define DBG_TOOL	15	// 32768	65535

#define DBG_FORCE	-1	// always true

extern const char *e_fork;
extern const char *e_exec;
extern const char *e_tool;
extern const char *e_not_implemented;
extern const char *e_bad_type;


void fatal(const char *s, ...);
void warning(const char *s, ...);
void notice(const char *s, ...);
void trace(int mask, const char *s, ...);
void trace2(int mask, const char *s, ...);
void error(const char *s, ...);
void echo(const char *s, ...);
void progression(char c);
void endprogression();

FILE * writelog(const char *s, ...);

void printlog();

void fopenlog(FILE *f, const char *name);
void fcloselog(FILE *f);

//notused void closelog(FILE *fl);

#endif
