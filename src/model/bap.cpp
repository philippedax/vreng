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

float Bap::getBap(uint8_t param) const
{
  return ba[param];
}

void Bap::setBap(uint8_t param, float val)
{
  ba[param] = val;
}

float Bap::getFap(uint8_t param) const
{
  return fa[param];
}

void Bap::setFap(uint8_t param, float val)
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

  char *l = NULL;
  static uint8_t nbr_frames;
  uint8_t num_frame = 0;

  //error("parse: %s", bapline);
  l = strtok(bapline, " \t\"\n");	// type

  if (! stringcmp(l, HEAD_BAP_V31)) {		// Bap3.1 Header
    num_baps = NUM_BAPS_V31;
    resetMask(num_baps);
    baptype = TYPE_BAP_V31;
    //error("type: %d", baptype);
    l = strtok(NULL, " \t\n");	// file_name
    //error("file: %s", l);
    l = strtok(NULL, " \t\n");	// rate
    //error("rate: %s", l);
    l = strtok(NULL, " \t\n");	// nbr_frame
    nbr_frames = atoi(l);
    //error("nbr: %s", l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_BAP_V32)) {	// Bap3.2 Header
    num_baps = NUM_BAPS_V32;
    resetMask(num_baps);
    baptype = TYPE_BAP_V32;
    l = strtok(NULL, " \t\n");
    //error("file: %s", l);
    l = strtok(NULL, " \t\n");
    //error("rate: %s", l);
    l = strtok(NULL, " \t\n");
    //error("nbr: %s", l);
    nbr_frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_FAP_V20)) {	// Fap2.0 Header
    num_baps = NUM_FAPS;
    resetMask(num_baps);
    baptype = TYPE_FAP_V20;
    l = strtok(NULL, " \t\n");
    l = strtok(NULL, " \t\n");
    l = strtok(NULL, " \t\n");
    nbr_frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_FAP_V21)) {	// Fap2.1 Header
    num_baps = NUM_FAPS;
    resetMask(num_baps);
    baptype = TYPE_FAP_V21;
    l = strtok(NULL, " \t\n");
    l = strtok(NULL, " \t\n");
    l = strtok(NULL, " \t\n");
    nbr_frames = atoi(l);
    return baptype;
  }

  // Masks and Values
  else {
    if (baptype == TYPE_BAP_V31 || baptype == TYPE_BAP_V32) {
      if (baptype == TYPE_BAP_V31) num_baps = NUM_BAPS_V31;
      if (baptype == TYPE_BAP_V32) num_baps = NUM_BAPS_V32;

      // bap masks
      int i;
      for (i=1; i <= num_baps; i++) {
        setMask(i, atoi(l));  // set all the mask
        l = strtok(NULL, " \t\n");
        //printf("%d ", isMask(i));
      } 
      //printf("i=%d num_baps=%i\n", i, num_baps);

      // bap num_frame
      //error("num: %s", l);
      num_frame = atoi(l);
      l = strtok(NULL, " \t\n");

      // bap values
      for (int i=1; i <= num_baps; i++) {
        if (! l) break;		// no values
        if (! isMask(i)) continue;
        //error("val: %s", l);
        if (i >= TR_VERTICAL && i <= TR_FRONTAL) {  // 170..172 translations
          setBap(i, atof(l) / TR_DIV);		// magic formula (300)
        }
        else {	// rotations
          if (num_baps == NUM_BAPS_V31)
            setBap(i, atof(l) / BAPV31_DIV);	// magic formula (1745)
          else if (num_baps == NUM_BAPS_V32)
            setBap(i, atof(l) / BAPV32_DIV);	// magic formula (555)
        }
        trace(DBG_MAN, "parse bap: l=%s ba[%d]=%.2f", l, i, getBap(i));
        if ((l = strtok(NULL, " \t\"\n")) == NULL) break;	// no more values
      }
      if (num_frame + 1 == nbr_frames) {
        //error("end of frames");
        return 0;
      }
    }

    else if (baptype == TYPE_FAP_V20 || baptype == TYPE_FAP_V21) {
      num_baps = NUM_FAPS;

      // fap mask
      for (int i=1; i <= num_baps; i++) {
        setMask(i, atoi(l));	// set all the mask values
        l = strtok(NULL, " \t\n");
      }

      // fap num_frame
      //error("parse: num_frame=%s", l);
      num_frame = atoi(l);
      //error("num: %s", l);
      l = strtok(NULL, " \t\n");

      // fap values
      for (int i=1; i <= num_baps; i++) {
        if (! l) break;		// no values
        if (! isMask(i)) continue;
        if (baptype == TYPE_FAP_V20)
          setFap(i, atof(l) / FAPV20_DIV); //unknown formula
        else
          setFap(i, atof(l) / FAPV21_DIV); //unknown formula
        if ((l = strtok(NULL, " \t\"\n")) == NULL) break;
      }
      if (num_frame + 1 == nbr_frames) {
        //error("end of frames");
        return 0;
      }
    }
    else {
      error("parse: unknown type=%d", baptype);
      return 0;
    }
  }
  //error("end frame");
  return baptype;  // frame ready to play
}
