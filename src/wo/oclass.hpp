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
#ifndef WCLASS_HPP
#define WCLASS_HPP

#include <vector>
using namespace std;

typedef class WObject * (WCreator) (char *);
typedef class WObject * (WReplicator) (uint8_t, class Noid, class Payload *);
typedef void (WBuiltin) ();

/* aliases */
struct sObj {
  const char *objalias;
  const char *objreal;
};


/**
 * OClass Class
 *
 * WObject factory system
 */
class OClass {

private:
  static const struct sObj Objs[];

protected:
  static OClass **otable;		///< objects table
  static uint16_t otable_size;		///< table size

public:
  const uint8_t       type_id;		///< class id
  const char *  const type_name;	///< class name
  WCreator *    const creator;		///< create from file
  WReplicator * const replicator;	///< create from network
  WBuiltin *    const builtin;		///< create from internal

  OClass(uint8_t type_id, const char *type_name, WCreator*, WReplicator* = NULL, WBuiltin* = NULL);
  /**< Constructor */

  static WObject * creatorInstance(uint8_t type_id, char *l);
  /**< Creator from fileline */

  static void builtinInstance(uint8_t type_id);
  /**< Creator from internal */

  static class WObject * replicatorInstance(uint8_t type_id, class Noid noid,
					    class Payload *pp);
  /**< Creator from network */

  static const OClass * getOClass(uint8_t type_id);
  /**< Gets oclass by id */

  static const OClass * getOClass(const char *type_name);
  /**< Gets oclass by name */

  static void dumpTable();
  /**< Dump table */

  static bool isValidType(int type_id);
  /**< Check if a type is valid, exists */
};

#endif
