//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2011 Philippe Dax
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
#ifndef CLOTH_HPP
#define CLOTH_HPP

#include "wobject.hpp"
#include <list>

#define CLOTH_TYPE	70
#define CLOTH_NAME	"Cloth"

/**
 * Cloth class
 */
class Cloth: public WObject {

protected:
  static list<WObject*> wearList;

  bool taken;				///< bool taken by avatar
  float ttl;				///< time to live with avatar
  float dx, dy, dz, dax, day, daz;	///< difference with avatar positions
  float ox, oy, oz, oax, oay, oaz;	///< original positions
  bool hat, halo, skirt, wings;		///< flags of article
  uint8_t article;			///< types of article
  char color[16];			///< given color

  /* cloth models */
  enum cloth_model {
    HALO,
    HAT,
    DRESS,
    WINGS,
    NONE
  };

public:
  /* net properties */
  enum {
    PROPHNAME,
    PROPXY,
    PROPZ,
    PROPAZ,
    PROPAX,
    PROPAY,
    PROPS       ///< last item = properties number
  };

  /* actions */
  enum cloth_action {
    WEAR,
    TAKEOFF,
    RECREATE,
    BASKET,
    MOVE,
    DESTROY,
    BUY
  };

  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();		///< init funclist

  Cloth(char *l);		///< Constructor
  Cloth() {}			///< Constructor for sub-classes

  static WObject * (creator)(char *l);
  /**< Creation from a file */

  virtual void changePermanent(float lasting);
  /**< movement */

  virtual void quit();
  /**< Quits */

protected:
  void behavior();
  /**< Sets behavior */

  void setName(const char *modelname);
  /**< Sets qualified name */

  void setName();
  /**< Sets qualified name */

  void setPersist();
  /**< inserts object */

  void getPersist();
  /**< Selects object */

  void delPersist();
  /**< Deletes object */

  void inits();
  /**< Do specific inits */

  void restorePosition();
  /**< Restores original position */

  void addToWearList();
  void delFromWearList();
  /**< List handling */

private:
  void defaults();
  /**< Sets default values */

  void parser(char *l);
  /**< Parses xml */

  void makeSolid();
  /**< Builds solid geometry */

};

#endif
