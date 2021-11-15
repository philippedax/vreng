//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2021 Philippe Dax
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
#ifndef VRSQL_HPP
#define VRSQL_HPP

#if HAVE_SQLITE | HAVE_MYSQL
#define VRSQL 1
#else
#define VRSQL 0
#endif
//dax1 #define VRSQL 0	//debug forced to 0
//#define HAVE_SQLITE 0 //debug try without SQLITE !!!
//#if !HAVE_SQLITE
//#define HAVE_MYSQL 1 // mysql forced if sqlite not present
//#endif

// includes system
#if HAVE_SQLITE
#include <sqlite3.h>
#elif HAVE_MYSQL
#include <mysql/mysql.h>
#endif

#include "wobject.hpp"	// typeName (table)


#define	ERR_SQL	-11111	// query error


/**
 * VRSql class
 */
class VRSql {

 private:
  static const uint16_t CMD_MAX = 1024;	///< query size max
  char sqlcmd[CMD_MAX];	///< Sql command

#if HAVE_SQLITE
  sqlite3_stmt *res;	///< Sqlite result
#elif HAVE_MYSQL
  MYSQL_RES *res;	///< MySqsl result
  MYSQL_ROW row;	///< MySql row
#endif

 public:
#if HAVE_SQLITE
  sqlite3 *db;		///< Sqlite handle
#elif HAVE_MYSQL
  MYSQL *db;		///< MySql handle
#endif

  VRSql();		///< constructor

  virtual ~VRSql() {};	///< destructor

  static VRSql* init();
  /**< initialization */

  static VRSql* getVRSql();
  /**< returns the vrsql pointer */

  virtual bool query(const char *sqlcmd);
  /**< sends a Sql query */

  virtual void quit();
  /**< closes the VRSql link */

#if HAVE_SQLITE
  virtual bool openDB();
  /**< opens to the Sqlite database */
#elif HAVE_MYSQL
  virtual bool connectDB();
  /**< connects to the MySql server */
#endif

#if HAVE_SQLITE
  static int callback(void *, int argc, char **argv, char **azColName);
  virtual int prepare(const char *sqlcmd);
#endif

#if HAVE_SQLITE
#elif HAVE_MYSQL
  virtual MYSQL_RES * result();
  /**< gets the Sql result */
#endif

  virtual int getInt(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns an int from a column */

  virtual float getFloat(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns a float from a column */

  virtual int getString(const char *table, const char *col, const char *object, const char *world, char *str, uint16_t irow);
  /**< returns a string from a column */

  virtual int getSubstring(const char *table, const char *pattern, uint16_t irow, char *substring);
  /**< gets a string and returns an index if pattern matches */

  virtual int getCount(const char *table, const char *col, const char *pattern);
  /**< gets a count of rows matching the pattern */

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

  virtual void deleteRow(WObject *o, const char *table, const char *object, const char *world);
  /**< deletes a row from table */

  virtual void deleteRow(WObject *o, const char *str);
  /**< deletes a row from table matching string */

  virtual void deleteRow(WObject *o);
  /**< deletes a row of this object */

  virtual void deleteRows(WObject *o);
  /**< deletes all rows of this object */

  // gets values

  virtual int getCount(const char *table);
  virtual int getCount(const char *table, const char *world);
  /**< gets the count of rows from this table */

  virtual int getCountCart();
  /**< gets the count of rows from Cart table */

  virtual int getName(const char *table, const char *pattern, int num, char *retstr);
  /**< gets the qualified name of an object */

  virtual int getState(WObject *o);
  virtual int getState(WObject *o, uint16_t irow);
  /**< returns the state of object id in the world worldname */

  virtual void getPos(WObject *o);
  virtual void getPos(WObject *o, uint16_t irow);

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

  virtual void updateState(WObject *o);
  virtual void updateState(WObject *o, int val);
  /**< updates the state of object id in the database */

  virtual void updatePos(WObject *o);
  virtual void updateColor(WObject *o);
  virtual void updateGeom(WObject *o, char *geom);
  virtual void updateGeom(WObject *o, const char *table, char *geom);
  virtual void updateUrl(WObject *o);
  virtual void updateOwner(WObject *o);
  virtual void updateBap(class Humanoid *o);

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

#endif // VRSQL
