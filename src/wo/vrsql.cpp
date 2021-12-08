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
#include "vreng.hpp"
#include "vrsql.hpp"
#include "world.hpp"	// current
#include "humanoid.hpp"	// Humanoid bap
#include "pref.hpp"	// g.user
#include "env.hpp"	// home


// local
static VRSql *vrsql = NULL;		// vrsql handle, only one by universe

#if HAVE_SQLITE | HAVE_MYSQL | HAVE_PGSQL
static const char * DB = "vreng_db";	///< database name
#else
static const char * DB = NULL;		///< no database
#endif
static const char * USER = "vreng";	///< user name
static const char * PASSWD = NULL;	///< no password
static const char * COL_NAME = "name";	///< column name
static const char * COL_ST = "state";	///< column state
static const char * COL_X = "x";	///< column x
static const char * COL_Y = "y";	///< column y
static const char * COL_Z = "z";	///< column z
static const char * COL_AZ = "az";	///< column az
static const char * COL_AX = "ax";	///< column ax
static const char * COL_AY = "ay";	///< column ay
static const char * COL_R = "r";	///< column r (red)
static const char * COL_G = "g";	///< column g (green)
static const char * COL_B = "b";	///< column b (blue)
static const char * COL_A = "a";	///< column a (alpha)
static const char * COL_OWN = "owner";	///< column owner
static const char * COL_GEOM = "geom";	///< column geom
static const char * COL_URL = "url";	///< column url
static const char * COL_BAP = "bap";	///< column bap


/** Constructor */
VRSql::VRSql()
{
#if HAVE_SQLITE | HAVE_MYSQL | HAVE_PGSQL
  db = NULL;
  res = NULL;
#endif
}

#if HAVE_SQLITE
bool VRSql::openDB()
{
  if (::g.pref.fast == true)
    return false;

  char pathDB[128];
  char *err_msg = 0;

  sprintf(pathDB, "%s/.vreng/%s", Env::home(), DB);
  int rc = sqlite3_open(pathDB, &db);
  if (rc != SQLITE_OK) {
    error("Cannot open database: %s", sqlite3_errmsg(db));
    sqlite3_close(db);
    return false;
  }
  return true;
}

int VRSql::callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  error("callback: argc=%d", argc);
  for (int i = 0; i < argc; i++) {
    printf("%d: %s = %s\n", i, azColName[i], argv[i] ? argv[i] : "NULL");
  }
  return 0;
}
#endif

#if HAVE_MYSQL
/**
 * Establishes a link with the mysql server
 */
bool VRSql::connectDB()
{
  if (::g.pref.fast == true)
    return false;

#ifdef HAVE_MYSQL_REAL_CONNECT
  if ((db = mysql_init(db)) != NULL) {
    if (! mysql_real_connect(db, DEF_MYSQL_SERVER, USER, PASSWD, DB, 0, NULL, 0)) {
      warning("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
#if HAVE_MYSQL_ERROR
      if (mysql_errno(db))
        error("mysql_error: %s", mysql_error(db));
#endif
      return false;
    }
  }
  else {
    warning("VRSql: %s can't init %s", USER, DEF_MYSQL_SERVER);
    return false;
  }
  return true;
#else
  if ((db = mysql_connect(NULL, DEF_MYSQL_SERVER, USER, PASSWD)) == NULL) {
    warning("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
    return false;
  }
  if (db && mysql_select_db(db, DB) != 0) {
    error("VRSql: can't find database %s", DB);
    return false;
  }
  return true;
#endif
}
#endif

#if HAVE_PGSQL
/**
 * Establishes a link with the pgsql server
 */
bool VRSql::connectDB()
{
  if (::g.pref.fast == true)
    return false;

  char args[32];
  sprintf(args, "user=%s dbname=%s", ::g.user, DB);
  PGconn *db = PQconnectdb(args);
  if (PQstatus(db) == CONNECTION_BAD) {
    warning("VRSql: %s can't connect %s", ::g.user, DEF_PGSQL_SERVER);
    PQerrorMessage(db);
    PQfinish(db);
    return false;
  }
  return true;
}
#endif

/** Allocates VRSql */
VRSql * VRSql::init()
{
#if HAVE_SQLITE | HAVE_MYSQL | HAVE_PGSQL
  if (World::current())
    return NULL;

  vrsql = new VRSql();

  int r = 0;
  if (vrsql) {
#if HAVE_SQLITE
    r = vrsql->openDB();	// open database
#elif HAVE_MYSQL
    r = vrsql->connectDB();	// connect to database server
    vrsql->createDatabase(DB);
#elif HAVE_PGSQL
    r = vrsql->connectDB();	// connect to database server
    vrsql->createDatabase(DB);
#endif
    if (! r) {
      trace(DBG_INIT, "init: can't reach database");
      delete vrsql;
      vrsql = NULL;
    }
  }
  trace(DBG_INIT, "init: vrsql = %p", vrsql);
  return vrsql;
#else
  return NULL;
#endif
}

/** Returns VRSql ptr */
VRSql * VRSql::getVRSql()
{
  return vrsql;
}

/** Closes the sql link */
void VRSql::quit()
{
  if (vrsql) {
#if HAVE_SQLITE
    if (db) sqlite3_close(db);
    db = NULL;
#elif HAVE_MYSQL
    if (db) mysql_close(db);
    db = NULL;
#elif HAVE_PGSQL
    if (db) PQfinish(db);
#endif
  }
}

#if HAVE_SQLITE
int VRSql::prepare(const char *sqlcmd)
{
  int rc = sqlite3_prepare_v2(db, sqlcmd, strlen(sqlcmd) + 1, &res, NULL);
/** bad code !!! FIXME 
  //rc = sqlite3_step(res);
  //sqlite3_finalize(res);
  //sqlite3_close(db);
**/
  return rc;
}
#endif

/** Sends a query SQL command */
bool VRSql::query(const char *sqlcmd)
{
  trace(DBG_SQL, "query: %s", sqlcmd);

#if HAVE_SQLITE
  char *err_msg = 0;

  //trace(DBG_FORCE, "query: %s", sqlcmd);
  if (! db) {
    openDB();	// we need to reopen database
  }

/** bad code !!! FIXME 
  int rc = sqlite3_exec(db, sqlcmd, 0, 0, &err_msg);	// without callback
  //rc = sqlite3_prepare_v2(db, sqlcmd, -1, &res, 0);
  if (rc != SQLITE_OK) {
    //error("query: err %s", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return false;
  }
  rc = sqlite3_step(res);
  //if (rc == SQLITE_ROW) {
  //  error("step: %s", sqlite3_column_text(res, 0));
  //}
  sqlite3_finalize(res);
**/
  //sqlite3_close(db);
  return true;

#elif HAVE_MYSQL
  if (! db) {
    connectDB();	// we need to reconnect to the MySql server
  }

  if (mysql_query(db, sqlcmd) != 0) {
#if HAVE_MYSQL_ERROR
    if (mysql_errno(db)) error("mysql_error: %s", mysql_error(db));
    error("query: %s", sqlcmd);
#endif
    return false;
  }
  return true;

#elif HAVE_PGSQL
  if (! db) {
    connectDB();	// we need to reconnect to the MySql server
  }

  PGresult *res = PQexec(db, sqlcmd);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    PQclear(res);
    //PQfinish(db);
  }
  return true;

#else // NO DBMS
  return false;		// SQLite and MySql not presents
#endif
}

#if HAVE_MYSQL
/** Gets a result, fetching the row */
MYSQL_RES * VRSql::result()
{
  MYSQL_RES *res = mysql_store_result(db);

#if HAVE_MYSQL_ERROR
  if (! res) error("mysql_error: %s", mysql_error(db));
#endif
  return res;
}
#endif


/** Gets an integer value from a row in the sql table
 *  if value doesn't exist, the value is inserted
 */
int VRSql::getInt(const char *table, const char *col, const char *object, const char *world, uint16_t irow)
{
  int val = 0;

  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s='%s%s%s'",
          col, table, COL_NAME, object, (*world) ? "@" : "", world);

#if HAVE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  createTable(table);
  rc = prepare(sqlcmd);
  if (rc != SQLITE_OK) {
    error("%s %s %d err prepare %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  rc = sqlite3_bind_int(res, 1, 1);
  if (rc != SQLITE_OK) {
    error("%s %s %d err bind %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  rc = sqlite3_step(res);
  if (rc == SQLITE_ROW) {
    val = sqlite3_column_int(res, 0);
    //error("rc=%d val=%d", rc,val);
  }
  else {
    error("%s %s %d err step %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  sqlite3_finalize(res);
#elif HAVE_MYSQL
  query(sqlcmd);
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    insertCol(table, col, object, world); // then insert col into the table
    mysql_free_result(res);
    return ERR_SQL;	// no row
  }
  val = atoi(row[0]));
  mysql_free_result(res);
#endif

  return val;
}

/** Gets a float value from a row in the sql table
 *  if value doesn't exist, the value is inserted
 */
float VRSql::getFloat(const char *table, const char *col, const char *object, const char *world, uint16_t irow)
{
  float val = 0;

  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s='%s%s%s'",
          col, table, COL_NAME, object, (*world) ? "@" : "", world);

#if HAVE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  createTable(table);
  rc = sqlite3_exec(db, sqlcmd, callback, this, &err_msg);	// with callback
  error("exec rc=%d", rc);
  //if (rc != SQLITE_OK) {
  //  error("%s err exec %s", table, sqlite3_errmsg(db));
  //  sqlite3_free(err_msg);
  //}
  rc = prepare(sqlcmd);
  if (rc != SQLITE_OK) {
    error("%s err prepare %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  rc = sqlite3_bind_double(res, 1, 0);
  if (rc != SQLITE_OK) {
    error("%s %s %d err bind %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  rc = sqlite3_step(res);
  if (rc == SQLITE_ROW) {
    val = sqlite3_column_double(res, 0);
    error("rc=%d val=%.1f", rc,val);
  }
  else if (rc == SQLITE_DONE) {
    val = sqlite3_column_double(res, 0);
    error("rc=%d val=%.1f", rc,val);
  }
  else {
    error("rc=%d err step %s", rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  sqlite3_finalize(res);

#elif HAVE_MYSQL
  query(sqlcmd);
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    insertCol(table, col, object, world); // then insert col into the table
    mysql_free_result(res);
    return ERR_SQL;	// no row
  }
  val = atof(row[0]));
  mysql_free_result(res);
#endif

  return val;
}

/** Gets a string (retstring) from a row in the sql table
 *  if string is not found, the string is inserted
 */
int VRSql::getString(const char *table, const char *col, const char *object, const char *world, char *retstring, uint16_t irow)
{
  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s='%s%s%s'",
          col, table, COL_NAME, object, (*world) ? "@" : "", world);

#if HAVE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  createTable(table);
  rc = prepare(sqlcmd);
  rc = sqlite3_bind_text(res, 1, NULL, -1, NULL);
  rc = sqlite3_step(res);
  if (rc == SQLITE_ROW) {
    if (retstring) {
      strcpy(retstring, (char *) sqlite3_column_text(res, 0));
      //error("rc=%d str=%s", rc,retstring);
    }
  }
  else {
    error("rc=%d err step %s", rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
  }
  sqlite3_reset(res);
  sqlite3_finalize(res);

#elif HAVE_MYSQL
  if (! query(sqlcmd)) return ERR_SQL;
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    insertCol(table, col, object, world); // then insert col into the table
    mysql_free_result(res);
    return ERR_SQL;	// no row
  }
  if (row[0] == NULL) return ERR_SQL;
  mysql_free_result(res);
  if (retstring)
    strcpy(retstring, row[0]);
#endif

  return irow;
}

/** Gets a substring (retstring) if the pattern matches
 *  and returns the index of the row
 */
int VRSql::getSubstring(const char *table, const char *pattern, uint16_t irow, char *retstring)
{
  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s regexp '%s'",
          COL_NAME, table, COL_NAME, pattern);

#if HAVE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  createTable(table);
  rc = prepare(sqlcmd);
  rc = sqlite3_bind_text(res, 1, NULL, -1, NULL);
  rc = sqlite3_step(res);
  if (rc == SQLITE_ROW) {
    if (retstring) {
      strcpy(retstring, (char *) sqlite3_column_text(res, 0));
      //error("rc=%d str=%s", rc,retstring);
    }
  }
  sqlite3_reset(res);
  sqlite3_finalize(res);

#elif HAVE_MYSQL
  if (! query(sqlcmd)) return ERR_SQL;
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    mysql_free_result(res);
    return ERR_SQL;	// no row
  }
  if (row[0] == NULL) {
    mysql_free_result(res);
    return ERR_SQL;
  }
  if (! strstr(row[0], pattern)) {
    mysql_free_result(res);
    return ERR_SQL;	// no match
  }
  mysql_free_result(res);
  if (retstring)
    strcpy(retstring, row[0]);
#endif

  return irow;
}

/** Gets a count of rows from a sql table */
int VRSql::getCount(const char *table, const char *col, const char *pattern)
{
  int val = 0;

  sprintf(sqlcmd, "select SQL_CACHE count(*) from %s where %s regexp %s",
          table, col, pattern);

#if HAVE_SQLITE
/** bad code !!! FIXME 
  prepare(sqlcmd);
  sqlite3_bind_int(res, 1, 1);
  sqlite3_step(res);
  val = sqlite3_column_int(res, 0);
  sqlite3_finalize(res);
**/

#elif HAVE_MYSQL
  if (! query(sqlcmd)) return ERR_SQL;
  res = mysql_store_result(db);
  if ((row = mysql_fetch_row(res)) == NULL) {
    mysql_free_result(res);
    return 0;	// no row
  }
  val = atoi(row[0]);
  mysql_free_result(res);
#endif

  return val;
}


// create

/** Creates database */
void VRSql::createDatabase(const char *database)
{
  sprintf(sqlcmd, "create database if not exists %s", database);
  query(sqlcmd);
}

/** Creates table */
void VRSql::createTable(const char *table)
{
  sprintf(sqlcmd, "create table if not exists %s ('NULL')", table);
  query(sqlcmd);
}


// inserts

/** Insert row into the sql table */
void VRSql::insertRow(WObject *o)
{
  createTable(o->typeName());
  sprintf(sqlcmd, "insert into %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) values ('%s@%s', '0', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', '%s', 'NULL', 'NULL')",
          o->typeName(),
          COL_NAME, COL_ST, COL_X, COL_Y, COL_Z, COL_AZ, COL_AX, COL_AY, COL_OWN, COL_GEOM, COL_URL,
          o->named(), World::current()->getName(), o->ownerName());
  query(sqlcmd);
}

/** Insert col into the sql table */
void VRSql::insertCol(const char *table, const char *col, const char *object, const char *world)
{
  sprintf(sqlcmd, "insert into %s (%s,%s) values ('%s%s%s', 'NULL')",
          table, COL_NAME, col, object, (*world) ? "@" : "", world);
  query(sqlcmd);
}


// updates

/** Updates row int into the sql table */
void VRSql::updateInt(WObject *o, const char *table, const char *col, const char *object, const char *world, int val)
{
  sprintf(sqlcmd, "update %s set %s=%d where %s='%s%s%s'",
          table, col, val, COL_NAME, object, (*world) ? "@" : "", world);
  query(sqlcmd);
}

/** Updates row float into the sql table */
void VRSql::updateFloat(WObject *o, const char *table, const char *col, const char *object, const char *world, float val)
{
  sprintf(sqlcmd, "update %s set %s=%.3f where %s='%s%s%s'",
          table, col, val, COL_NAME, object, (*world) ? "@" : "", world);
  query(sqlcmd);
}

/** Updates row string into the sql table */
void VRSql::updateString(WObject *o, const char *table, const char *col, const char *object, const char *world, const char *str)
{
  sprintf(sqlcmd, "update %s set %s='%s' where %s='%s%s%s'",
          table, col, str, COL_NAME, object, (*world) ? "@" : "", world);
  query(sqlcmd);
}


// deletes

/** Deletes all rows from the sql table */
void VRSql::deleteRows(const char *table)
{
  sprintf(sqlcmd, "delete from %s", table);
  query(sqlcmd);
}

/** Deletes a row from the sql table */
void VRSql::deleteRow(WObject *o, const char *table, const char *object, const char *world)
{
  sprintf(sqlcmd, "delete from %s where %s='%s%s%s'",
          table, COL_NAME, object, (*world) ? "@" : "", world);
  query(sqlcmd);
}

/** Deletes a row from the sql table matching a string */
void VRSql::deleteRow(WObject *o, const char *str)
{
  sprintf(sqlcmd, "delete from %s where %s='%s@%s'",
          o->typeName(), COL_NAME, str, World::current()->getName());
  query(sqlcmd);
}

//
//---------------------- methods independants of RDB system ---------------------
//

// updates

/** Updates row int into the sql table */
void VRSql::updateInt(WObject *o, const char *col, int val)
{
  updateInt(o, o->typeName(), col, o->named(), World::current()->getName(), val);
}

/** Updates row float into the sql table */
void VRSql::updateFloat(WObject *o, const char *col, float val)
{
  updateFloat(o, o->typeName(), col, o->named(), World::current()->getName(), val);
}

/** Updates row string into the sql table */
void VRSql::updateString(WObject *o, const char *col, const char *str)
{
  updateString(o, o->typeName(), col, o->named(), World::current()->getName(), str);
}

/** Updates row string into the sql table */
void VRSql::updateString(WObject *o, const char *table, const char *col, const char *str)
{
  updateString(o, table, col, o->named(), World::current()->getName(), str);
}

// delete

/** Deletes a row of this object */
void VRSql::deleteRow(WObject *o)
{
  deleteRow(o, o->typeName(), o->named(), World::current()->getName());
}

/** Deletes all rows of this object */
void VRSql::deleteRows(WObject *o)
{
  deleteRows(o->typeName());
}

// gets

/** Gets an integer value from a row in the sql table */
int VRSql::getInt(WObject *o, const char *col, uint16_t irow)
{
  return getInt(o->typeName(), col, o->named(), World::current()->getName(), irow);
}

/** Gets a float from a row in the sql table */
float VRSql::getFloat(WObject *o, const char *col, uint16_t irow)
{
  return getFloat(o->typeName(), col, o->named(), World::current()->getName(), irow);
}

/** Gets a string from a row in the sql table */
int VRSql::getString(WObject *o, const char *col, char *str, uint16_t irow)
{
  return getString(o->typeName(), col, o->named(), World::current()->getName(), str, irow);
}

int VRSql::getState(WObject *o)
{
  return getInt(o, COL_ST, 0);
}

int VRSql::getState(WObject *o, uint16_t irow)
{
  return getInt(o, COL_ST, irow);
}

void VRSql::getPos(WObject *o)
{
  o->pos.x = getPosX(o, 0);
  o->pos.y = getPosY(o, 0);
  o->pos.z = getPosZ(o, 0);
  o->pos.az = getPosAZ(o, 0);
  o->pos.ax = getPosAX(o, 0);
  o->pos.ay = getPosAY(o, 0);
}

void VRSql::getPos(WObject *o, uint16_t irow)
{
  o->pos.x = getPosX(o, irow);
  o->pos.y = getPosY(o, irow);
  o->pos.z = getPosZ(o, irow);
  o->pos.az = getPosAZ(o, irow);
  o->pos.ax = getPosAX(o, irow);
  o->pos.ay = getPosAY(o, irow);
}

float VRSql::getPosX(WObject *o, uint16_t irow = 0)
{
  float val = getFloat(o, COL_X, irow);
  return (val != ERR_SQL) ? val : o->pos.x;
}

float VRSql::getPosY(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_Y, irow);
  return (val != ERR_SQL) ? val : o->pos.y;
}

float VRSql::getPosZ(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_Z, irow);
  return (val != ERR_SQL) ? val : o->pos.z;
}

float VRSql::getPosAZ(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_AZ, irow);
  return (val != ERR_SQL) ? val : o->pos.az;
}

float VRSql::getPosAX(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_AX, irow);
  return (val != ERR_SQL) ? val : o->pos.ax;
}

float VRSql::getPosAY(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_AY, irow);
  return (val != ERR_SQL) ? val : o->pos.ay;
}

float VRSql::getColorR(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_R, irow);
  return (val != ERR_SQL) ? val : o->pos.x;
}

float VRSql::getColorG(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_G, irow);
  return (val != ERR_SQL) ? val : o->pos.y;
}

float VRSql::getColorB(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_B, irow);
  return (val != ERR_SQL) ? val : o->pos.z;
}

float VRSql::getColorA(WObject *o, uint16_t irow)
{
  float val = getFloat(o, COL_A, irow);
  return (val != ERR_SQL) ? val : o->pos.az;
}

void VRSql::getColor(WObject *o)
{
  o->pos.x = getColorR(o, 0);
  o->pos.y = getColorG(o, 0);
  o->pos.z = getColorB(o, 0);
  o->pos.az = getColorA(o, 0);
}

void VRSql::getColor(WObject *o, uint16_t irow)
{
  o->pos.x = getColorR(o, irow);
  o->pos.y = getColorG(o, irow);
  o->pos.z = getColorB(o, irow);
  o->pos.az = getColorA(o, irow);
}

int VRSql::getCountCart()
{
  char pattern[256];
  //sprintf(pattern, "'^%s$'", ::g.user);
  int val = getCount("Cart", COL_OWN, pattern);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getCount(const char *table)
{
  char pattern[256];
  sprintf(pattern, "'$'");
  int val = getCount(table, COL_NAME, pattern);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getCount(const char *table, const char *world)
{
  char pattern[256];
  sprintf(pattern, "'@%s$'", world);
  int val = getCount(table, COL_NAME, pattern);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getName(const char *table, const char *pattern, int num, char *retstr)
{
  int irow = getSubstring(table, pattern, num, retstr);
  trace(DBG_SQL, "num=%d irow=%d str=%s", num, irow, retstr);
  return (irow >= 0 ) ? irow : -1;
}

void VRSql::getGeom(WObject *o)
{
  getGeom(o, (uint16_t)0);
}

void VRSql::getGeom(WObject *o, uint16_t irow)
{
  getGeom(o, o->geometry, irow);
}

void VRSql::getGeom(WObject *o, char *geom)
{
  if (geom) getGeom(o, geom, (uint16_t)0);
}

void VRSql::getGeom(WObject *o, char *geom, uint16_t irow)
{
  if (geom) getString(o, COL_GEOM, geom, irow);
}

void VRSql::getUrl(WObject *o)
{
  getUrl(o, (uint16_t)0);
}

void VRSql::getUrl(WObject *o, uint16_t irow)
{
  getString(o, COL_URL, (char *) o->urlName(), irow);
}

void VRSql::getOwner(WObject *o)
{
  getOwner(o, (uint16_t)0);
}

void VRSql::getOwner(WObject *o, uint16_t irow)
{
  getString(o, COL_OWN, (char *) o->ownerName(), irow);
}

void VRSql::getBap(WObject *o, char *bap, uint16_t irow)
{
  getString(o, COL_BAP, bap, irow);
}

// updates

void VRSql::updateState(WObject *o)
{
  updateInt(o, COL_ST, o->state);
}

void VRSql::updateState(WObject *o, int val)
{
  updateInt(o, COL_ST, val);
}

void VRSql::updatePosX(WObject *o)
{
  updateFloat(o, COL_X, o->pos.x);
}

void VRSql::updatePosY(WObject *o)
{
  updateFloat(o, COL_Y, o->pos.y);
}

void VRSql::updatePosZ(WObject *o)
{
  updateFloat(o, COL_Z, o->pos.z);
}

void VRSql::updatePosAZ(WObject *o)
{
  updateFloat(o, COL_AZ, o->pos.az);
}

void VRSql::updatePosAX(WObject *o)
{
  updateFloat(o, COL_AX, o->pos.ax);
}

void VRSql::updatePosAY(WObject *o)
{
  updateFloat(o, COL_AY, o->pos.ay);
}

void VRSql::updateColorR(WObject *o)
{
  updateFloat(o, COL_R, o->pos.x);
}

void VRSql::updateColorG(WObject *o)
{
  updateFloat(o, COL_G, o->pos.y);
}

void VRSql::updateColorB(WObject *o)
{
  updateFloat(o, COL_B, o->pos.z);
}

void VRSql::updateColorA(WObject *o)
{
  updateFloat(o, COL_A, o->pos.az);
}

void VRSql::updatePos(WObject *o)
{
  updatePosX(o);
  updatePosY(o);
  updatePosZ(o);
  updatePosAZ(o);
  updatePosAX(o);
  updatePosAY(o);
}

void VRSql::updateColor(WObject *o)
{
  updateColorR(o);
  updateColorG(o);
  updateColorB(o);
  updateColorA(o);
}

void VRSql::updateGeom(WObject *o, char *geom)
{
  if (geom) updateString(o, COL_GEOM, geom);
}

void VRSql::updateGeom(WObject *o, const char *table, char *geom)
{
  if (geom) updateString(o, table, COL_GEOM, geom);
}

void VRSql::updateUrl(WObject *o)
{
  updateString(o, COL_URL, o->urlName());
}

void VRSql::updateOwner(WObject *o)
{
  updateString(o, COL_OWN, o->ownerName());
}

void VRSql::updateBap(Humanoid *o)
{
  updateString(o, COL_BAP, o->bapline);
}

