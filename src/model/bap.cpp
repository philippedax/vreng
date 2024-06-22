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


/** Constructor */
Bap::Bap()
{
  type = 0;
  params = 0;

  // set all values to 0
  for (int i=1; i <= NUM_BAPS; i++) {
    bapvalues[i] = 0;
    masks[i] = false;
  }
  for (int i=1; i <= NUM_FAPS; i++) {
    fapvalues[i] = 0;
    masks[i] = false;
  }
}

/** Gets type of bap */
uint8_t Bap::getType() const
{
  return type;
}

/** Is Bap ? */
bool Bap::isBap() const
{
  switch(type) {
  case TYPE_BAP_V32:
  case TYPE_BAP_V31:
    return true;
  default:
    return false;
  }
}

/** Is Fap ? */
bool Bap::isFap() const
{
  switch(type) {
  case TYPE_FAP_V20:
  case TYPE_FAP_V21:
    return true;
  default:
    return false;
  }
}

/** Resets bit masks */
void Bap::resetMask(int params)
{
  for (int i=1; i <= params; i++) {
    masks[i] = false;
  }
}

/** Checks bap mask */
bool Bap::isBapMask(int param) const
{
  if (! isBap()) return false;
  return masks[param];
}

/** Checks fap mask */
bool Bap::isFapMask(int param) const
{
  if (! isFap()) return false;
  return masks[param];
}

/** Sets bit mask */
void Bap::setMask(int param, bool bit)
{
  masks[param] = bit;
}

/** Gets bap value */
float Bap::get(int param) const
{
  switch(type) {
  case TYPE_BAP_V31:
  case TYPE_BAP_V32:
    return bapvalues[param];
  case TYPE_FAP_V20:
  case TYPE_FAP_V21:
    return fapvalues[param];
  default:
    return 0;
  }
}

/** Sets bap value */
void Bap::set(int param, float val)
{
  if (param >= TR_VERTICAL && param <= TR_FRONTAL) {	// body translations
    bapvalues[param] = val/TR_DIV;		// magic formula: 300
  }
  else {
    switch(type) {
    case TYPE_BAP_V31:
      bapvalues[param] = val/BAPV31_DIV;	// magic formula: 1745 (PI * 100000 / 180)
      break;
    case TYPE_BAP_V32:
      bapvalues[param] = val/BAPV32_DIV;	// magic formula: 555 (100000 / 180)
      break;
    case TYPE_FAP_V20:
      fapvalues[param] = val/FAPV20_DIV;	// magic formula: 20
      break;
    case TYPE_FAP_V21:
      fapvalues[param] = val/FAPV21_DIV;	// magic formula: 1
      break;
    }
  }
}

/** Parses a bap line - returns type else if error 0 - bapline will be tokenized */
uint8_t Bap::parse(char *bapline)
{
  if (! strcmp(bapline, ""))  return 0;  // discard empty bapline
  else if (bapline[0] == '#') return 0;  // discard comment bapline

  char *l = NULL;
  uint16_t frame = 0;

  //echo("bapparse: %s", bapline);
  l = strtok(bapline, " ");
  if (! stringcmp(l, HEAD_BAP_V31)) {		// Bap3.1 Header
    params = NUM_BAPS;
    resetMask(params);
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    type = TYPE_BAP_V31;
    return type;
  }
  else if (! stringcmp(l, HEAD_BAP_V32)) {	// Bap3.2 Header
    params = NUM_BAPS;
    resetMask(params);
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    type = TYPE_BAP_V32;
    return type;
  }
  else if (! stringcmp(l, HEAD_FAP_V20)) {	// Fap2.0 Header
    params = NUM_FAPS;
    resetMask(params);
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    type = TYPE_FAP_V20;
    return type;
  }
  else if (! stringcmp(l, HEAD_FAP_V21)) {	// Fap2.1 Header
    params = NUM_FAPS;
    resetMask(params);
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    l = strtok(NULL, " ");
    frames = atoi(l);
    type = TYPE_FAP_V21;
    return type;
  }

  // masks
  else {
    if (type == TYPE_BAP_V31 || type == TYPE_BAP_V32) {
      for (int i=1; i <= params; i++) {
        if (l) {
          masks[i] = atoi(l);  // set all the masks bits
          l = strtok(NULL, " ");
        }
      }
      //echo("bapparse: frame=%s", l);
      frame = atoi(l);

      for (int i=1; i <= params; i++) {
        if (masks[i] == false) continue;
        if ((l = strtok(NULL, " ")) == NULL) break;	// no more values
        set(i, static_cast<float>(atof(l)));
        //echo("bapparse: l=%s values[%d]=%.1f", l, i, values[i]);
      }
      if (frame < frames) {
        //echo("bapparse: end of bap frames");
        return 0;
      }
    }
    else if (type == TYPE_FAP_V20 || type == TYPE_FAP_V21) {
      for (int i=1; i <= params; i++) {
        if (l) {
          masks[i] = atoi(l);  // set all the masks bits
          l = strtok(NULL, " ");
        }
      }
      //echo("bapparse fap: frame=%s", l);
      frame = atoi(l);

      for (int i=1; i <= params; i++) {
        if (masks[i] == false) continue;
        if ((l = strtok(NULL, " ")) == NULL) break;	// no more values
        set(i, static_cast<float>(atof(l)));
      }
      if (frame < frames) {
        //echo("bapparse: end of fap frames");
        return 0;
      }
    }
    else {
      echo("bapparse: unknown type=%d", type);
      return 0;
    }
  }
  return type;  // frame ready to play
}

/** Gets params */
uint16_t Bap::getParams()
{
  switch(type) {
  case TYPE_BAP_V31:
  case TYPE_BAP_V32:
    params = NUM_BAPS;
    break;
  case TYPE_FAP_V20:
  case TYPE_FAP_V21:
    params = NUM_FAPS;
    break;
  default:
    params = 0;
  }
  return params;
}
