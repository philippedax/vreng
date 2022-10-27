//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2009 Philippe Dax
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
#ifndef INITOBJ_HPP
#define INITOBJ_HPP

#include "wobject.hpp"	// WObject


/*
 * If you define a new object
 * please, increment OBJECTSNUMBER below
 */
#define OBJECTSNUMBER   87

#define UNKNOWN_TYPE	-1


/**
 * InitFunc struct
 */
struct InitFunc {
  void (*initfunc) (void);	///< init function
};

/**
 * ProperFunc struct
 */
struct ProperFunc {
  void (*method) (WObject *po, class Payload *pp);
};
#define _Payload (void (*)(WObject *o, class Payload *p))

/**
 * ActionFunc struct
 */
struct ActionFunc {
  char name[ACTIONNAME_LEN];
  void (*method) (WObject *po, void *d, time_t s, time_t u);
};
#define _Action (void (*)(WObject *o, void *d, time_t s, time_t u))


/** external functions **/

/* Action */
void setActionFunc(uint8_t type, uint8_t action, void (*method)(WObject *o, void *d, time_t s, time_t u), const char *action_name);
bool isAction(uint8_t type, uint8_t action);
void doAction(uint8_t type, uint8_t action, WObject *o, void *d, time_t s, time_t u);
bool isActionName(uint8_t type, uint8_t action);
int indexAction(uint8_t type, const char *name);
void copyActionName(uint8_t type, uint8_t action, char *dest);
char * getActionName(uint8_t type, uint8_t action);
void * getActionMethod(uint8_t type, uint8_t action);

/* Lasting */
void setMaxLastings(uint8_t type, float maxlast);
float getMaxLastings(uint8_t type);

/* Property */
void setPropertiesnumber(uint8_t type, uint8_t nbprop);
uint8_t getPropertiesnumber(uint8_t type);

void getPropertyFunc(uint8_t type, uint8_t prop, void (*get_method)(WObject *o, Payload *p));
void putPropertyFunc(uint8_t type, uint8_t prop, void (*put_method)(WObject *o, Payload *p));
bool isGetPropertyFunc(uint8_t type, uint8_t prop);
bool isPutPropertyFunc(uint8_t type, uint8_t prop);
void runGetPropertyFunc(uint8_t type, uint8_t prop, WObject *o, Payload *p);
void runPutPropertyFunc(uint8_t type, uint8_t prop, WObject *o, Payload *p);

#endif
