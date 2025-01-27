//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2009 Philippe Dax
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
// dart.hpp
//---------------------------------------------------------------------------
#ifndef DART_HPP
#define DART_HPP

#include "object.hpp"
#include "projectile.hpp"

class User;

#define DART_TYPE	16
#define DART_NAME	"Dart"


/**
 * Dart class
 */
class Dart: public Projectile {
 private:
  static const float LSPEED;
  static const float TTL;

 public:
  static const OClass oclass;	///< class variable

  const OClass* getOClass() {return &oclass;}

  static void funcs();	///< init funclist

  Dart(Object *po, void *d, time_t s, time_t u);
  /**< Constructor */

  virtual ~Dart() {};

  Dart(uint8_t type_id, Noid noid, Payload *pp);
  /**< Constructor replicator */

  static Object * replicator(uint8_t type_id, Noid noid, Payload *pp);

 private:
  void defaults();
  /**< Sets default values */

  void geometry();
  /**< Builds solid geometry */

  void behaviors();
  /**< Sets behaviors. */

  void inits();
  /**< Sets initializations. */

  // callbacks
  static void get_hit(Dart *pcur, Payload *pp);
  static void create(User *pu, void *d, time_t s, time_t u);
};

#endif
