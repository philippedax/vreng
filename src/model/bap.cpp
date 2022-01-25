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

#include "gestures.hpp"	// gestures


Bap::Bap()
{
  baptype = 0;
  num_params = 0;

  //set all values to 0
  for (int i=0; i <= NUM_BAPS_V32; i++) {
    ba[i] = 0;
    bit[i] = 0;
  }
  for (int i=0; i <= NUM_FAPS; i++) {
    fa[i] = 0;
  }
}

uint8_t Bap::getType() const
{
  return baptype;
}

void Bap::resetMask(int num)
{
  for (int i=0; i <= num; i++) {
    bit[i] = 0;
  }
}

bool Bap::isMask(int param) const
{
  return bit[param];
}

void Bap::setMask(int param, uint8_t val)
{
  bit[param] = val;
}

float Bap::getBap(int param) const
{
  return ba[param];
}

void Bap::setBap(int param, float val)
{
  ba[param] = val;
}

float Bap::getFap(int param) const
{
  return fa[param];
}

void Bap::setFap(int param, float val)
{
  fa[param] = val;
}

#if 0 //dax - notused
bool Bap::equalLast(int param)
{
  return (ba[param] == balast[param]);
}

void Bap::copyLast(int param)
{
  balast[param] = ba[param];
}
#endif

void Bap::jpRX(int param, uint8_t model)
{
  //if (param == 0) return;
  //if (! isMask(param)) return;
  //if (equalLast(param)) return;
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef( sign * ba[abs(param)], 1,0,0); break;
  case MODEL_OBJ: glRotatef(-sign * ba[abs(param)], 0,1,0); break;
  }
  //copyLast(param);
}

void Bap::jpRY(int param, uint8_t model)
{
  //if (param == 0) return;
  //if (! isMask(abs(param))) return;
  //if (equalLast(param)) return;
  //error("ry: %d (%1.f)", param, ba[abs(param)]);
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * ba[abs(param)], 0,1,0); break;
  case MODEL_OBJ: glRotatef(sign * ba[abs(param)], 1,0,0); break;
  }
  //copyLast(param);
}

void Bap::jpRZ(int param, uint8_t model)
{
  //if (param == 0) return;
  //if (! isMask(param)) return;
  //if (equalLast(param)) return;
  int sign = (param >= 0) ?1:-1;
  switch (model) {
  case MODEL_OFF: glRotatef(sign * ba[abs(param)], 0,0,1); break;
  case MODEL_OBJ: glRotatef(sign * ba[abs(param)], 0,0,1); break;
  }
  //copyLast(param);
}

uint8_t Bap::parse(char *bapline)
{
  if (! strcmp(bapline, ""))  return 0;  // discard empty bapline
  else if (bapline[0] == '#') return 0;  // discard comment bapline

  char *l = NULL;
  static uint16_t nbr_frames;
  uint16_t num_frame = 0;
  //float vreng_rate;
  //float bap_rate;
  //float ratio_rate;

  //error("parse: %s", bapline);
  l = strtok(bapline, " ");
  if (! stringcmp(l, HEAD_BAP_V31)) {		// Bap3.1 Header
    num_params = NUM_BAPS_V31;
    resetMask(num_params);
    baptype = TYPE_BAP_V31;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    nbr_frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_BAP_V32)) {	// Bap3.2 Header
    num_params = NUM_BAPS_V32;
    resetMask(num_params);
    baptype = TYPE_BAP_V32;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    nbr_frames = atoi(l);
    //bap_rate = (float) atof(l);
    //vreng_rate = ::g.times.getRate();
    //ratio_rate = vreng_rate / bap_rate;
    //error("parse bap3.2: num_params=%d vreng_rate=%.2f bap_rate=%.2f ratio_rate=%.2f", num_params, vreng_rate, bap_rate, ratio_rate);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_FAP_V20)) {	// Fap2.0 Header
    num_params = NUM_FAPS;
    resetMask(num_params);
    baptype = TYPE_FAP_V20;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    nbr_frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_FAP_V21)) {	// Fap2.1 Header
    num_params = NUM_FAPS;
    resetMask(num_params);
    baptype = TYPE_FAP_V21;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    nbr_frames = atoi(l);
    return baptype;
  }

  // masks and values
  else {
    if (baptype == TYPE_BAP_V31 || baptype == TYPE_BAP_V32) {
      for (int i=1; i <= num_params; i++) {
        if (l) {
          bit[i] = atoi(l);  // set all the mask values
          l = strtok(NULL, " ");
        }
      }

      //error("parse: num_frame=%s", l);
      num_frame = atoi(l);

      for (int i=1; i <= num_params; i++) {
        if (bit[i] == 0) continue;
        if ((l = strtok(NULL, " ")) == NULL) break;	// no more values

        if (i >= TR_VERTICAL && i <= TR_FRONTAL)	// translations
          ba[i] = (float) atof(l);			// magic formula (300)
        else {  	// angles
          if (num_params == NUM_BAPS_V32)
            ba[i] = (float) atof(l) / BAPV32_DIV;	// magic formula (555) //GB
          else
            ba[i] = (float) atof(l) / BAPV31_DIV;	// magic formula (1745)
        }
        trace(DBG_MAN, "bap: l=%s ba[%d]=%.1f", l, i, ba[i]);
      }
      if (num_frame + 1 == nbr_frames) {
        //error("end of bap frames");
        return 0;
      }
    }

    else if (baptype == TYPE_FAP_V20 || baptype == TYPE_FAP_V21) {
      for (int i=1; i <= num_params; i++) {
        if (l) {
          bit[i] = atoi(l);  // set all the mask values
          l = strtok(NULL, " ");
        }
      }

      error("parse fap: num_frame=%s", l);
      num_frame = atoi(l);

      for (int i=1; i <= num_params; i++) {
        if (bit[i] == 0) continue;
        if ((l = strtok(NULL, " ")) == NULL) break;	// no more values
        if (baptype == TYPE_FAP_V20)
          fa[i] = (float) atof(l) / FAPV20_DIV;		// fap formula
        else
          fa[i] = (float) atof(l) / FAPV21_DIV;		// fap formula
      }
      if (num_frame + 1 == nbr_frames) {
        //error("end of fap frames");
        return 0;
      }
    }
    else {
      error("parse: unknown type=%d", baptype);
      return 0;
    }
  }
  return baptype;  // frame ready to play
}
