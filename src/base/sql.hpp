//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2021 Philippe Dax
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
// sql.hpp
//---------------------------------------------------------------------------
#ifndef SQL_HPP
#define SQL_HPP


#if HAVE_SQLITE | HAVE_MYSQL
#define VSQL 1
#else
#define VSQL 0
#endif
#if HAVE_SQLITE
#define USE_SQLITE 1
#endif
#if HAVE_MYSQL
#define USE_MYSQL 1
#endif
#if HAVE_SQLITE & HAVE_MYSQL
#define USE_SQLITE 1
#define USE_MYSQL 0
#endif


// includes system
#if USE_SQLITE
#include <sqlite3.h>
#elif USE_MYSQL
#include <mysql/mysql.h>
#endif

class Object;


#define	ERR_SQL	-11111	// query error


/**
 * VSql class
 */
class VSql {

private:
  static const uint16_t SQLCMD_MAX = 2048;	///< query size max
  char sql[SQLCMD_MAX];	///< Sql command

#if USE_SQLITE
  sqlite3_stmt *stmt;	///< Sqlite statement
#elif USE_MYSQL
  MYSQL_RES *res;	///< MySqsl result
  MYSQL_ROW row;	///< MySql row
#else
  void *res;		///< no dbms
#endif

  void queryTrace(const char *sql);

 public:
#if USE_SQLITE
  sqlite3 *db;		///< Sqlite handle
#elif USE_MYSQL
  MYSQL *db;		///< MySql handle
#else
  void *db;		///< no dbms
#endif

  VSql();		///< constructor

  virtual ~VSql() {};	///< destructor

  static VSql* init();
  /**< initialization */

  static VSql* getVSql();
  /**< returns the vsql pointer */

  bool query(const char *sql);
  /**< sends a Sql query */

  void quit();
  /**< closes the VSql link */

 private:
#if USE_SQLITE
  bool openDB();
  /**< opens to the Sqlite database */
#elif USE_MYSQL
  bool connectDB();
  /**< connects to the MySql server */
#endif

#if USE_MYSQL
  MYSQL_RES * result();
  /**< gets the Sql result */
#endif

  //////////////
  // select

 private:
  static int selectInt_cb(void *val, int argc, char **argv, char **azColName);
  static int selectFloat_cb(void *val, int argc, char **argv, char **azColName);
  static int selectString_cb(void *val, int argc, char **argv, char **azColName);
  static int selectSubstring_cb(void *val, int argc, char **argv, char **azColName);
  static int countRows_cb(void *val, int argc, char **argv, char **azColName);

  int selectInt(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns an int from a column */

  float selectFloat(const char *table, const char *col, const char *object, const char *world, uint16_t irow);
  /**< returns a float from a column */

  int selectString(const char *table, const char *col, const char *object, const char *world, char *str, uint16_t irow);
  /**< returns a string from a column */

  int selectSubstring(const char *table, const char *pat, uint16_t irow, char *substring);
  /**< gets a string and returns an index if pattern matches */

  int getInt(Object *o, const char *col, uint16_t irow);
  /**< returns an int from a column */

  float getFloat(Object *o, const char *col, uint16_t irow);
  /**< returns a float from a column */

  int getString(Object *o, const char *col, char *str, uint16_t irow);
  /**< returns a string from a column */

 public:
  int countRows(const char *table);

  int countRows(const char *table, const char *col, const char *pattern);
  /**< gets a count of rows matching the pattern */

  //////////////
  // create

 private:
  void createDatabase(const char *database);
  /**< creates a database */

  void createTable(const char *table);
  /**< creates a table */

 public:
  int checkRow(const char *table, const char *name, const char *world);
  /**< checks if row exists or not */

  //////////////
  // insert

 public:
  void insertRow(Object *o);
  /**< inserts a row into a table */

  void insertCol(const char *table, const char *col, const char *object, const char *world);
  /**< inserts a column into a row */

  //////////////
  // update

 private:
  void updateInt(Object *o, const char *table, const char *col, const char *object, const char *world, int val);
  /**< updates an integer into a column */

  void updateFloat(Object *o, const char *table, const char *col, const char *object, const char *world, float val);
  /**< updates a float into a column */

  void updateInt(Object *o, const char *col, int val);
  /**< updates an integer into a column */

  void updateFloat(Object *o, const char *col, float val);
  /**< updates a float into a column */

  void updateString(Object *o, const char *col, const char *str);
  /**< updates a string into a column */

  void updateString(Object *o, const char *table, const char *col, const char *str);
  /**< updates a string into a column for a given table */

 public:
  void updateString(Object *o, const char *table, const char *col, const char *object, const char *world, const char *str);
  /**< updates a string into a column */

  //////////////
  // delete

 public:
  void deleteRow(Object *o, const char *table, const char *object, const char *world);
  /**< deletes a row from table */

  void deleteRow(Object *o, const char *str);
  /**< deletes a row from table matching string */

  void deleteRow(Object *o);
  /**< deletes a row of this object */

  void deleteRows(const char *table);
  /**< deletes all rows from table */

  void deleteRows(Object *o);
  /**< deletes all rows of this object */

  //////////////
  // gets values

 public:
  int getCount(const char *table);
  int getCount(const char *table, const char *world);
  int getCount(const char *table, const char *name, const char *world);
  /**< gets the count of rows from this table */

  int getCountCart();
  /**< gets the count of rows from Cart table */

  int getName(const char *table, const char *pattern, int num, char *retstr);
  /**< gets the qualified name of an object */

  int getState(Object *o);
  int getState(Object *o, uint16_t irow);
  /**< returns the state of object id in the world worldname */

  void getPos(Object *o);
  void getPos(Object *o, uint16_t irow);

 private:
  float getPosX(Object *o, uint16_t irow);
  float getPosY(Object *o, uint16_t irow);
  float getPosZ(Object *o, uint16_t irow = 0);
  float getPosAZ(Object *o, uint16_t irow);

 public:
  void getGeom(Object *o);
  void getGeom(Object *o, char *geom);
  void getGeom(Object *o, uint16_t irow);
  void getGeom(Object *o, char *geom, uint16_t irow);
  void getOwner(Object *o);
  void getOwner(Object *o, uint16_t irow);

  void updateState(Object *o);
  void updateState(Object *o, int val);
  void updatePos(Object *o);
  void updateGeom(Object *o, char *geom);
  void updateGeom(Object *o, const char *table, char *geom);
  void updateOwner(Object *o);

  void updatePosX(Object *o);
  void updatePosY(Object *o);
  void updatePosZ(Object *o);
  void updatePosAZ(Object *o);
};

#endif // SQL
