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
#include "bap.hpp"
#include "timer.hpp"	// getRate
#include "format.hpp"	// MODEL_*
#include "str.hpp"	// stringcmp

#include "gestures.hpp"	// gestures bap + fap


Bap::Bap()
{
  baptype = 0;
  num_baps = 0;

  //set all values to 0
  for (int i=0; i <= NUM_BAPS_V32; i++) {
    ba[i] = 0;
    balast[i] = 0;
    bit[i] = 0;
  }
  for (int i=0; i <= NUM_FAPS; i++) {
    fa[i] = 0;
  }
}

void Bap::resetMask(uint8_t num)
{
  for (int i=0; i <= num; i++) {
    bit[i] = 0;
  }
}

uint8_t Bap::getType() const
{
  return baptype;
}

bool Bap::isMask(uint8_t param) const
{
  return bit[param];
}

void Bap::setMask(uint8_t param, uint8_t val)
{
  bit[param] = val;
}

GLfloat Bap::getBap(uint8_t param) const
{
  return ba[param];
}

void Bap::setBap(uint8_t param, int val)
{
  ba[param] = val;
}

GLfloat Bap::getFap(uint8_t param) const
{
  return fa[param];
}

void Bap::setFap(uint8_t param, uint8_t val)
{
  fa[param] = val;
}

bool Bap::equalLast(uint8_t param)
{
  return (ba[param] == balast[param]);
}

void Bap::copyLast(uint8_t param)
{
  balast[param] = ba[param];
}

void Bap::jpRX(int param, uint8_t model)
{
  //if (! isMask(param)) return;
  //if (equalLast(param)) return;
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(+sign * ba[abs(param)], 1,0,0); break;
  case MODEL_OBJ: glRotatef(-sign * ba[abs(param)], 1,0,0); break;
  }
  copyLast(param);
}

void Bap::jpRY(int param, uint8_t model)
{
  //if (! isMask(param)) return;
  //if (equalLast(param)) return;
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * ba[abs(param)], 0,1,0); break;
  case MODEL_OBJ: glRotatef(sign * ba[abs(param)], 0,1,0); break;
  }
  copyLast(param);
}

void Bap::jpRZ(int param, uint8_t model)
{
  //if (! isMask(param)) return;
  //if (equalLast(param)) return;
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * ba[abs(param)], 0,0,1); break;
  case MODEL_OBJ: glRotatef(sign * ba[abs(param)], 0,0,1); break;
  }
  copyLast(param);
}

/* parses bapline and returns baptype */
uint8_t Bap::parse(char *bapline)
{
  if (! strcmp(bapline, ""))  return 0;  // discard empty bapline
  else if (bapline[0] == '#') return 0;  // discard comment bapline

  // TODO: mix bap and fap
  // needs header: HBAP, HFAP, B3.1, B3.2, F2.1 F2.0 inserted by vaps server
  // num_faps to be added
  char *l;

  num_baps = NUM_BAPS_V31;
  //error("parse: %s", bapline);
  l = strtok(bapline, " \t\n");	// type

  if (! stringcmp(l, HEAD_BAP_V31)) {		// Bap3.1 Header
    num_baps = NUM_BAPS_V31;
    resetMask(num_baps);
    baptype = TYPE_BAP_V31;
    l = strtok(NULL, " \t\n");	// file_name
    l = strtok(NULL, " \t\n");	// max_frame
    //error("parse baptype: %d", baptype);
  }
  else if (! stringcmp(l, HEAD_BAP_V32)) {	// Bap3.2 Header
    num_baps = NUM_BAPS_V32;
    resetMask(num_baps);
    baptype = TYPE_BAP_V32;
    l = strtok(NULL, " \t\n"); l = strtok(NULL, " \t\n");
  }
  else if (! stringcmp(l, HEAD_FAP_V20)) {	// Fap2.0 Header
    num_baps = NUM_FAPS;
    resetMask(num_baps);
    baptype = TYPE_FAP_V20;
    l = strtok(NULL, " \t\n"); l = strtok(NULL, " \t\n");
  }
  else if (! stringcmp(l, HEAD_FAP_V21)) {	// Fap2.1 Header
    num_baps = NUM_FAPS;
    resetMask(num_baps);
    baptype = TYPE_FAP_V21;
    l = strtok(NULL, " \t\n"); l = strtok(NULL, " \t\n");
  }

  // Masks and Values
  else {
    if (baptype == TYPE_BAP_V31 || baptype == TYPE_BAP_V32) {

      // masks
      for (int i=1; i <= num_baps; i++) {
        if (l) {
          bit[i] = atoi(l);  //extract all the mask
          l = strtok(NULL, " \t\n");
        }
      }
      // num_frame
      //error("parse: num_frame=%s", l);
      //l = strtok(NULL, " \t\n");

      // values
      for (int i=1; i <= num_baps; i++) {
        if (bit[i] == 0) continue;
        if ((l = strtok(NULL, " \t\"\n")) == NULL) break;
        if (i >= TR_VERTICAL && i <= TR_FRONTAL) {  // translations
          ba[i] = atoi(l);	// millimeters ?
        }
        else {	// rotations
          if (num_baps == NUM_BAPS_V32) ba[i] = atoi(l) / BAPV32_DIV; //magic formula (555) //GB
          else                          ba[i] = atoi(l) / BAPV31_DIV; //magic formula (1745)
        }
        trace(DBG_MAN, "parse bap: l=%s ba[%d]=%d", l, i, ba[i]);
      }
    }
    else if (baptype == TYPE_FAP_V20 || baptype == TYPE_FAP_V21) {
      // fap
      for (int i=1; i <= num_baps; i++) {
        if (l) {
          bit[i] = atoi(l);  //extract all the mask values
          l = strtok(NULL, " \t\n");
        }
      }
      trace(DBG_MAN, "parse fap: num_frame=%s", l);
      for (int i=1; i <= num_baps; i++) {
        if (bit[i] == 0) continue;
        if ((l = strtok(NULL, " \t\"\n")) == NULL) break;
        if (baptype == TYPE_FAP_V20) fa[i] = atoi(l) / FAPV20_DIV; //unknown formula
        else                         fa[i] = atoi(l) / FAPV21_DIV; //unknown formula
      }
    }
    else {
      error("parse: unknown type=%d", baptype);
      return 0;
    }
  }
  return baptype;  // frame ready to play
}
