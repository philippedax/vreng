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

#if HAVE_SQLITE | HAVE_MYSQL | HAVE_PGSQL
#define VRSQL 1
#else
#define VRSQL 0
#endif
#define USE_SQLITE 0
#define USE_MYSQL 0
#define USE_PGSQL 0
#if HAVE_SQLITE
#define USE_SQLITE 1
#endif
#if HAVE_MYSQL
#define USE_MYSQL 1
#endif
#if HAVE_PGSQL
#define USE_PGSQL 1
#endif
#if HAVE_SQLITE & HAVE_MYSQL
#define USE_SQLITE 1
#define USE_MYSQL 0
#define USE_PGSQL 0
#endif
#if HAVE_SQLITE & HAVE_PGSQL
#define USE_SQLITE 1
#define USE_MYSQL 0
#define USE_PGSQL 0
#endif
#if HAVE_MYSQL & HAVE_PGSQL
#define USE_SQLITE 0
#define USE_MYSQL 1
#define USE_PGSQL 0
#endif


// includes system
#if USE_SQLITE
#include <sqlite3.h>
#elif USE_MYSQL
#include <mysql/mysql.h>
#elif USE_PGSQL
#include <libpq-fe.h>
#endif

class WObject;


#define	ERR_SQL	-11111	// query error


/**
 * VRSql class
 */
class VRSql {

 private:
  static const uint16_t SQLCMD_MAX = 1024;	///< query size max
  char sql[SQLCMD_MAX];	///< Sql command

#if USE_SQLITE
  sqlite3_stmt *stmt;	///< Sqlite statement
#elif USE_MYSQL
  MYSQL_RES *res;	///< MySqsl result
  MYSQL_ROW row;	///< MySql row
#elif USE_PGSQL
  PGresult *res;	///< PGSqsl result
#else
  void *res;		///< no dbms
#endif

 public:
#if USE_SQLITE
  sqlite3 *db;		///< Sqlite handle
#elif USE_MYSQL
  MYSQL *db;		///< MySql handle
#elif USE_PGSQL
  PGconn *db;		///< PgSql handle
#else
  void *db;		///< no dbms
#endif

  VRSql();		///< constructor

  virtual ~VRSql() {};	///< destructor

  static VRSql* init();
  /**< initialization */

  static VRSql* getVRSql();
  /**< returns the vrsql pointer */

  bool query(const char *sql);
  /**< sends a Sql query */

  void quit();
  /**< closes the VRSql link */

#if USE_SQLITE
  bool openDB();
  /**< opens to the Sqlite database */
#elif USE_MYSQL
  bool connectDB();
  /**< connects to the MySql server */
#elif USE_PGSQL
  bool connectDB();
  /**< connects to the MySql server */
#endif

#if USE_MYSQL
  MYSQL_RES * result();
  /**< gets the Sql result */
#endif


  // select

  static int getInt_cb(void *val, int argc, char **argv, char **azColName);

  int getInt(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns an int from a column */

  static int getFloat_cb(void *val, int argc, char **argv, char **azColName);

  float getFloat(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns a float from a column */

  static int getString_cb(void *val, int argc, char **argv, char **azColName);

  int getString(const char *table, const char *col, const char *object, const char *world, char *str, uint16_t irow);
  /**< returns a string from a column */

  static int getSubstring_cb(void *val, int argc, char **argv, char **azColName);

  int getSubstring(const char *table, const char *pattern, uint16_t irow, char *substring);
  /**< gets a string and returns an index if pattern matches */

  static int getRows_cb(void *val, int argc, char **argv, char **azColName);

  int getRows(const char *table);

  int getRows(const char *table, const char *col, const char *pattern);
  /**< gets a count of rows matching the pattern */

  int getInt(WObject *o, const char *col, uint16_t irow);
  /**< returns an int from a column */

  float getFloat(WObject *o, const char *col, uint16_t irow);
  /**< returns a float from a column */

  int getString(WObject *o, const char *col, char *str, uint16_t irow);
  /**< returns a string from a column */

  // create

  void createDatabase(const char *database);
  /**< creates a database */

  void createTable(const char *table);
  /**< creates a table */

  // insert

  void insertRow(WObject *o);
  /**< inserts a row into a table */

  void insertCol(const char *table, const char *col, const char *object, const char *world);
  /**< inserts a column into a row */

  // update

  void updateInt(WObject *o, const char *table, const char *col, const char *object, const char *world, int val);
  /**< updates an integer into a column */

  void updateFloat(WObject *o, const char *table, const char *col, const char *object, const char *world, float val);
  /**< updates a float into a column */

  void updateString(WObject *o, const char *table, const char *col, const char *object, const char *world, const char *str);
  /**< updates a string into a column */

  void updateInt(WObject *o, const char *col, int val);
  /**< updates an integer into a column */

  void updateFloat(WObject *o, const char *col, float val);
  /**< updates a float into a column */

  void updateString(WObject *o, const char *col, const char *str);
  /**< updates a string into a column */

  void updateString(WObject *o, const char *table, const char *col, const char *str);
  /**< updates a string into a column for a given table */

  // delete

  void deleteRows(const char *table);
  /**< deletes all rows from table */

  void deleteRow(WObject *o, const char *table, const char *object, const char *world);
  /**< deletes a row from table */

  void deleteRow(WObject *o, const char *str);
  /**< deletes a row from table matching string */

  void deleteRow(WObject *o);
  /**< deletes a row of this object */

  void deleteRows(WObject *o);
  /**< deletes all rows of this object */

  // gets values

  int getCount(const char *table);
  int getCount(const char *table, const char *world);
  /**< gets the count of rows from this table */

  int getCountCart();
  /**< gets the count of rows from Cart table */

  int getName(const char *table, const char *pattern, int num, char *retstr);
  /**< gets the qualified name of an object */

  int getState(WObject *o);
  int getState(WObject *o, uint16_t irow);
  /**< returns the state of object id in the world worldname */

  void getPos(WObject *o);
  void getPos(WObject *o, uint16_t irow);

  float getPosX(WObject *o, uint16_t irow);
  float getPosY(WObject *o, uint16_t irow);
  float getPosZ(WObject *o, uint16_t irow = 0);
  float getPosAZ(WObject *o, uint16_t irow);
  float getPosAX(WObject *o, uint16_t irow);
  float getPosAY(WObject *o, uint16_t irow);

  void getGeom(WObject *o);
  void getGeom(WObject *o, char *geom);
  void getGeom(WObject *o, uint16_t irow);
  void getGeom(WObject *o, char *geom, uint16_t irow);
  void getOwner(WObject *o);
  void getOwner(WObject *o, uint16_t irow);

  void updateState(WObject *o);
  void updateState(WObject *o, int val);
  void updatePos(WObject *o);
  void updateGeom(WObject *o, char *geom);
  void updateGeom(WObject *o, const char *table, char *geom);
  void updateOwner(WObject *o);

  void updatePosX(WObject *o);
  void updatePosY(WObject *o);
  void updatePosZ(WObject *o);
  void updatePosAZ(WObject *o);
  void updatePosAX(WObject *o);
  void updatePosAY(WObject *o);
};

#endif // VRSQL
