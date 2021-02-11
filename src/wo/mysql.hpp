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
#ifndef MYSQL_HPP
#define MYSQL_HPP

#if 0 //debug mysql forced
#if !HAVE_SQLLITE
#define HAVE_MYSQL 1
#include <mysql/mysql.h>
#endif
#endif
#include "wobject.hpp"


#if HAVE_MYSQL

#define	ERR_MYSQL	-11111

/**
 * VRSql class
 *
 * MySql encapsulation of calls to the vreng database
 */
class VRSql {

 private:
  static const uint16_t CMD_SIZE = 1024;	///< query size max

  MYSQL_RES *ressql;	///< MySqsl result
  MYSQL_ROW row;	///< MySql row
  char sqlcmd[CMD_SIZE];///< MySql command

 public:
  MYSQL *mysql;		///< MySql handle

  VRSql();		///< constructor

  virtual ~VRSql() {};	///< destructor

  static VRSql* init();
  /**< initialization */

  static VRSql* getVRSql();
  /**< returns the vrsql pointer */

  virtual bool connect();
  /**< connects to the MySql server */

  virtual void quit();
  /**< closes the MySql link */

  virtual bool query(const char *sqlcmd);
  /**< sends a MySql query */

  virtual MYSQL_RES * result();
  /**< gets the Sql result */

  virtual int getCount(const char *table, const char *col, const char *pattern);
  /**< gets a count of rows matching the pattern */

  virtual int getInt(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns an int from a column */

  virtual float getFloat(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns a float from a column */

  virtual int getString(const char *table, const char *col, const char *object, const char *world, char *str, uint16_t irow);
  /**< returns a string from a column */

  virtual int getSubstring(const char *table, const char *pattern, uint16_t irow, char *substring);
  /**< gets a string and returns an index if pattern matches */

  virtual int getInt(WObject *o, const char *col, uint16_t irow);
  /**< returns an int from a column */

  virtual float getFloat(WObject *o, const char *col, uint16_t irow);
  /**< returns a float from a column */

  virtual int getString(WObject *o, const char *col, char *str, uint16_t irow);
  /**< returns a string from a column */

  // create

  virtual void createTable(const char *table);
  /**< creates a table */

  // insert

  virtual void insertRow(WObject *o);
  /**< inserts a row into a table */

  virtual void insertCol(const char *table, const char *col, const char *object, const char *world);
  /**< inserts a column into a row */

  // update

  virtual void updateInt(WObject *o, const char *table, const char *col, const char *object, const char *world, int val);
  /**< updates an integer into a column */

  virtual void updateFloat(WObject *o, const char *table, const char *col, const char *object, const char *world, float val);
  /**< updates a float into a column */

  virtual void updateString(WObject *o, const char *table, const char *col, const char *object, const char *world, const char *str);
  /**< updates a string into a column */

  virtual void updateInt(WObject *o, const char *col, int val);
  /**< updates an integer into a column */

  virtual void updateFloat(WObject *o, const char *col, float val);
  /**< updates a float into a column */

  virtual void updateString(WObject *o, const char *col, const char *str);
  /**< updates a string into a column */

  virtual void updateString(WObject *o, const char *table, const char *col, const char *str);
  /**< updates a string into a column for a given table */

  // delete

  virtual void deleteRows(const char *table);
  /**< deletes all rows from table */

  virtual void deleteRows(WObject *o);
  /**< deletes all rows of this object */

  virtual void deleteRow(WObject *o, const char *table, const char *object, const char *world);
  /**< deletes a row from table */

  virtual void deleteRow(WObject *o, const char *str);
  /**< deletes a row from table matching string */

  virtual void deleteRow(WObject *o);
  /**< deletes a row of this object */

  // get

  virtual int getCountCart();
  /**< gets the count of rows from Cart table */

  virtual int getCount(const char *table);
  /**< gets the count of rows from this table */

  virtual int getCount(const char *table, const char *world);
  /**< gets the count of rows from Ball table */

  virtual int getName(const char *table, const char *pattern, int num, char *retstr);
  /**< gets the qualified name of an object */

  virtual int getState(WObject *o);
  virtual int getState(WObject *o, uint16_t irow);
  /**< returns the state of object id in the world worldname */

  virtual void getPos(WObject *o);
  virtual void getPos(WObject *o, uint16_t irow);
  virtual void getColor(WObject *o);
  virtual void getColor(WObject *o, uint16_t irow);
  virtual void getGeom(WObject *o);
  virtual void getGeom(WObject *o, char *geom);
  virtual void getGeom(WObject *o, uint16_t irow);
  virtual void getGeom(WObject *o, char *geom, uint16_t irow);
  virtual void getUrl(WObject *o);
  virtual void getUrl(WObject *o, uint16_t irow);
  virtual void getOwner(WObject *o);
  virtual void getOwner(WObject *o, uint16_t irow);
  virtual void getBap(WObject *o, char *bap, uint16_t irow);

  virtual float getPosX(WObject *o, uint16_t irow);
  virtual float getPosY(WObject *o, uint16_t irow);
  virtual float getPosZ(WObject *o, uint16_t irow = 0);
  virtual float getPosAZ(WObject *o, uint16_t irow);
  virtual float getPosAX(WObject *o, uint16_t irow);
  virtual float getPosAY(WObject *o, uint16_t irow);

  virtual float getColorR(WObject *o, uint16_t irow);
  virtual float getColorG(WObject *o, uint16_t irow);
  virtual float getColorB(WObject *o, uint16_t irow);
  virtual float getColorA(WObject *o, uint16_t irow);

  virtual void updateState(WObject *o);
  virtual void updateState(WObject *o, int val);
  /**< updates the state of object id in the database */

  virtual void updatePos(WObject *o);
  virtual void updateColor(WObject *o);
  virtual void updateGeom(WObject *o, char *geom);
  virtual void updateGeom(WObject *o, const char *table, char *geom);
  virtual void updateUrl(WObject *o);
  virtual void updateOwner(WObject *o);
  virtual void updateBap(class Android *o);

  virtual void updatePosX(WObject *o);
  virtual void updatePosY(WObject *o);
  virtual void updatePosZ(WObject *o);
  virtual void updatePosAZ(WObject *o);
  virtual void updatePosAX(WObject *o);
  virtual void updatePosAY(WObject *o);

  virtual void updateColorR(WObject *o);
  virtual void updateColorG(WObject *o);
  virtual void updateColorB(WObject *o);
  virtual void updateColorA(WObject *o);
};

#endif // HAVE_MYSQL

#endif
