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
// bap.cpp
//
// Body Animation Parameter
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "bap.hpp"
#include "timer.hpp"	// getRate
#include "format.hpp"	// MODEL_*
#include "str.hpp"	// stringcmp

#include "gestures.hpp"	// gestures


/** Constructor */
Bap::Bap()
{
  baptype = 0;
  params = 0;

  // set all values to 0
  for (int i=0; i <= NUM_BAPS_V32; i++) {
    baps[i] = 0;
    bits[i] = 0;
  }
  for (int i=0; i <= NUM_FAPS; i++) {
    faps[i] = 0;
  }
}

/** Gets type of bap */
uint8_t Bap::getType() const
{
  return baptype;
}

/** Resets bit masks */
void Bap::resetBit(int params)
{
  for (int i=0; i <= params; i++) {
    bits[i] = 0;
  }
}

/** Checks bit mask */
bool Bap::isBit(int param) const
{
  return bits[param];
}

/** Sets bit mask */
void Bap::setBit(int param, uint8_t bit)
{
  bits[param] = bit;
}

/** Gets bap value */
float Bap::getBap(int param) const
{
  return baps[param];
}

/** Sets bap value */
void Bap::setBap(int param, float val)
{
  if (param >= TR_VERTICAL && param <= TR_FRONTAL) {	// 170..172 translations
    baps[param] = val;
    //baps[param] = val/TR_DIV;		// magic formula: 300
  }
  else {
    switch(baptype) {
    case TYPE_BAP_V31:
      baps[param] = val/BAPV31_DIV;	// magic formula: 1745 (PI * 100000 / 180)
      break;
    case TYPE_BAP_V32:
      baps[param] = val/BAPV32_DIV;	// magic formula: 555 (100000 / 180)
      break;
    default:
      baps[param] = val;
    }
  }
}

/** Gets fap value */
float Bap::getFap(int param) const
{
  return faps[param];
}

/** Sets fap value */
void Bap::setFap(int param, float val)
{
  switch(baptype) {
  case TYPE_FAP_V20:
    faps[param] = val/FAPV20_DIV;	// magic formula: 20
    break;
  case TYPE_FAP_V21:
    faps[param] = val/FAPV21_DIV;	// magic formula: 1
    break;
  default:
    faps[param] = val;
  }
}

/** Parses a bap line - returns baptype else if error 0 - bapline will be tokenized */
uint8_t Bap::parse(char *bapline)
{
  if (! strcmp(bapline, ""))  return 0;  // discard empty bapline
  else if (bapline[0] == '#') return 0;  // discard comment bapline

  char *l = NULL;
  uint16_t frame = 0;

  //echo("parse: %s", bapline);
  l = strtok(bapline, " ");
  if (! stringcmp(l, HEAD_BAP_V31)) {		// Bap3.1 Header
    params = NUM_BAPS_V31;
    resetBit(params);
    baptype = TYPE_BAP_V31;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_BAP_V32)) {	// Bap3.2 Header
    params = NUM_BAPS_V32;
    resetBit(params);
    baptype = TYPE_BAP_V32;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_FAP_V20)) {	// Fap2.0 Header
    params = NUM_FAPS;
    resetBit(params);
    baptype = TYPE_FAP_V20;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    return baptype;
  }
  else if (! stringcmp(l, HEAD_FAP_V21)) {	// Fap2.1 Header
    params = NUM_FAPS;
    resetBit(params);
    baptype = TYPE_FAP_V21;
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    return baptype;
  }

  // masks
  else {
    if (baptype == TYPE_BAP_V31 || baptype == TYPE_BAP_V32) {
      for (int i=1; i <= params; i++) {
        if (l) {
          bits[i] = atoi(l);  // set all the mask values
          l = strtok(NULL, " ");
        }
      }

      //echo("parse: frame=%s", l);
      frame = atoi(l);

      for (int i=1; i <= params; i++) {
        if (bits[i] == 0) continue;
        if ((l = strtok(NULL, " ")) == NULL) break;	// no more values

        if (i >= TR_VERTICAL && i <= TR_FRONTAL)	// translations
          baps[i] = static_cast<float>(atof(l));	// magic formula (300)
        else {  	// angles
          if (params == NUM_BAPS_V32)
            baps[i] = static_cast<float>(atof(l) / BAPV32_DIV);	// magic formula (555) //GB
          else
            baps[i] = static_cast<float>(atof(l) / BAPV31_DIV);	// magic formula (1745)
        }
        //echo("bap: l=%s baps[%d]=%.1f", l, i, baps[i]);
      }
      if (frame < frames) {
        //echo("end of bap frames");
        return 0;
      }
    }
    else if (baptype == TYPE_FAP_V20 || baptype == TYPE_FAP_V21) {
      for (int i=1; i <= params; i++) {
        if (l) {
          bits[i] = atoi(l);  // set all the mask values
          l = strtok(NULL, " ");
        }
      }

      //echo("parse fap: frame=%s", l);
      frame = atoi(l);

      for (int i=1; i <= params; i++) {
        if (bits[i] == 0) continue;
        if ((l = strtok(NULL, " ")) == NULL) break;	// no more values
        if (baptype == TYPE_FAP_V20)
          faps[i] = static_cast<float>(atof(l) / FAPV20_DIV);	// fap formula
        else
          faps[i] = static_cast<float>(atof(l) / FAPV21_DIV);	// fap formula
      }
      if (frame < frames) {
        //echo("end of fap frames");
        return 0;
      }
    }
    else {
      echo("parse: unknown type=%d", baptype);
      return 0;
    }
  }
  return baptype;  // frame ready to play
}

/** Gets params */
uint16_t Bap::getParams()
{
  switch(baptype) {
  case TYPE_BAP_V31:
    params = NUM_BAPS_V31;
    break;
  case TYPE_BAP_V32:
    params = NUM_BAPS_V32;
    break;
  case TYPE_FAP_V20:
  case TYPE_FAP_V21:
    params = NUM_FAPS;
    break;
  }
  return params;
}
