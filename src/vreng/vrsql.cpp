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

#if VRSQL
static const char * DB = "vreng_db";	///< database name
#else
static const char * DB = NULL;		///< no database
#endif
static const char * USER = "vreng";	///< user name
static const char * PASSWD = NULL;	///< no password
static const char * C_NAME = "name";	///< column name      : text(16)
static const char * C_ST = "state";	///< column state     : tinyint
static const char * C_X = "x";		///< column x         : float
static const char * C_Y = "y";		///< column y         : float
static const char * C_Z = "z";		///< column z         : float
static const char * C_AZ = "az";	///< column az        : float
static const char * C_AX = "ax";	///< column ax        : float
static const char * C_AY = "ay";	///< column ay        : float
static const char * C_R = "r";		///< column r (red)   : float
static const char * C_G = "g";		///< column g (green) : float
static const char * C_B = "b";		///< column b (blue)  : float
static const char * C_A = "a";		///< column a (alpha) : float
static const char * C_OWN = "owner";	///< column owner     : text(16)
static const char * C_GEOM = "geom";	///< column geom      : text(64)
static const char * C_URL = "url";	///< column url       : text(64)
static const char * C_BAP = "bap";	///< column bap       : text(128)


/** Constructor */
VRSql::VRSql()
{
  db = NULL;
#if USE_SQLITE
  stmt = NULL;
#else
  res = NULL;
#endif
}

#if USE_SQLITE
bool VRSql::openDB()
{
  if (::g.pref.nopersist == true) return false;

  char path[128];
  char *err_msg = NULL;

  sprintf(path, "%s/.vreng/%s", ::g.env.home(), DB);
  int rc = sqlite3_open(path, &db);
  if (rc != SQLITE_OK) {
    error("Cannot open database: %s", sqlite3_errmsg(db));
    //echo("open db: %x path=%s rc=%d", db, path, rc);
    return false;
  }
  //echo("open db: %x path=%s rc=%d", db, path, rc);
  return true;
}
#endif

#if USE_MYSQL
/**
 * Establishes a link with the mysql server
 */
bool VRSql::connectDB()
{
  if (::g.pref.nopersist == true)
    return false;

#ifdef HAVE_MYSQL_REAL_CONNECT
  if ((db = mysql_init(db)) != NULL) {
    if (! mysql_real_connect(db, DEF_MYSQL_SERVER, USER, PASSWD, DB, 0, NULL, 0)) {
      error("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
      if (mysql_errno(db))
        error("mysql_error: %s", mysql_error(db));
      return false;
    }
  }
  else {
    error("VRSql: %s can't init %s", USER, DEF_MYSQL_SERVER);
    return false;
  }
  return true;
#else
  if ((db = mysql_connect(NULL, DEF_MYSQL_SERVER, USER, PASSWD)) == NULL) {
    error("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
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

#if USE_PGSQL
/**
 * Establishes a link with the pgsql server
 */
bool VRSql::connectDB()
{
  if (::g.pref.nopersist == true)
    return false;

  char args[32];
  sprintf(args, "user=%s dbname=%s", ::g.user, DB);
  PGconn *db = PQconnectdb(args);
  if (PQstatus(db) == CONNECTION_BAD) {
    error("VRSql: %s can't connect %s", ::g.user, DEF_PGSQL_SERVER);
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
  vrsql = new VRSql();

  int r = 0;
  if (vrsql) {
#if USE_SQLITE
    r = vrsql->openDB();	// open sqlite database
#elif USE_MYSQL
    r = vrsql->connectDB();	// connect to database mysql server
#elif USE_PGSQL
    r = vrsql->connectDB();	// connect to database postgres server
#endif
    //dax vrsql->createDatabase(DB);
    if (! r) {
      error("init: can't reach database");
      delete vrsql;
      vrsql = NULL;
    }
  }
  trace(DBG_INIT, "vrsql init: %d", r);
  return vrsql;
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
#if USE_SQLITE
    sqlite3_close(db);
#elif USE_MYSQL
    mysql_close(db);
#elif USE_PGSQL
    PQfinish(db);
#endif
    db = NULL;
  }
}

/** Sends a query SQL command */
bool VRSql::query(const char *sql)
{
  trace(DBG_SQL, "query: %s", sql);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
/** bad code !!! FIXME 
**/
#if 1 //dax
  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);	// without callback
  if (rc != SQLITE_OK) {
    error("query exec %s sql=%s", sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return false;
  }
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("query prepare %s sql=%s", sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return false;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    error("query step: %s", sqlite3_column_text(stmt, 0));
  }
  if (rc != SQLITE_DONE) {
    error("query err: rc=%d", rc);
    return false;
  }
  sqlite3_finalize(stmt);
#endif

  return true;

#elif USE_MYSQL
  if (! db) {
    connectDB();	// we need to reconnect to the MySql server
  }
  if (mysql_query(db, sql) != 0) {
    if (mysql_errno(db)) error("mysql_error: %s", mysql_error(db));
    error("query: err %s", sql);
    return false;
  }
  return true;

#elif USE_PGSQL
  if (! db) {
    connectDB();	// we need to reconnect to the Postgres server
  }
  PGresult *rc = PQexec(db, sql);
  if (PQresultStatus(rc) != PGRES_TUPLES_OK) {
    PQclear(rc);
    error("query: err %s", sql);
    return false;
    //PQfinish(db);
  }
  return true;

#else // NO DBMS
  return false;		// neither SQLite MySql PgSql are presents
#endif
}

#if USE_MYSQL
/** Gets a result, fetching the row */
MYSQL_RES * VRSql::result()
{
  MYSQL_RES *res = mysql_store_result(db);

  if (! res)
    error("mysql_error: %s", mysql_error(db));
  return res;
}
#endif


/** Gets an integer value from a row in the sql table
 *  if value doesn't exist, the value is inserted
 */
int VRSql::getInt_cb(void *val, int argc, char **argv, char**azColName)
{
  echo("int_cb: argc=%d argv=%s azcolname=%s", argc, argv[0], azColName[0]);
  int *v = (int *)val;
  *v = atoi(argv[0]);
  return 0;
}

int VRSql::getInt(const char *table, const char *col, const char *name, const char *world, uint16_t irow)
{
  int val = 0;

  sprintf(sql, "SELECT %s FROM %s WHERE %s='%s%s%s'",
          col, table, C_NAME, name, (*world) ? "@" : "", world);

#if USE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
  //echo("sql getint %s", sql);
  createTable(table);
#if 0 //dax
  rc = sqlite3_exec(db, sql, &VRSql::getInt_cb, &val, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d err getint %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  echo("getInt: %s.%s %d", table, col, val);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s %s %d err prepare %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  //rc = sqlite3_bind_int(stmt, 1, 1);
  //if (rc != SQLITE_OK) {
  //  error("%s %s %d err bindint %s", table,col,irow, sqlite3_errmsg(db));
  //  sqlite3_free(err_msg);
  //  return ERR_SQL;
  //}
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    val = sqlite3_column_int(stmt, 0);
    echo("getInt: %s.%s %d", table, col, val);
  }
  else if (rc != SQLITE_DONE) {
    error("%s %s %d err stepint %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  query(sql);
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    insertCol(table, col, name, world); // then insert col into the table
    mysql_free_result(res);
    return ERR_SQL;	// no row
  }
  val = atoi(row[0]);
  mysql_free_result(res);
#endif

  return val;	// return an int
}

/** Gets a float value from a row in the sql table
 *  if value doesn't exist, the value is inserted
 */
int VRSql::getFloat_cb(void *val, int argc, char **argv, char**azColName)
{
  echo("float_cb: argc=%d argv=%s azcolname=%s", argc, argv[0], azColName[0]);
  float *v = (float *)val;
  *v = atof(argv[0]);
  return 0;
}

float VRSql::getFloat(const char *table, const char *col, const char *name, const char *world, uint16_t irow)
{
  float val = 0;

  sprintf(sql, "SELECT %s FROM %s WHERE %s='%s%s%s'",
          col, table, C_NAME, name, (*world) ? "@" : "", world);

#if USE_SQLITE
/** bad code !!! FIXME 
**/
  int rc = 0;
  char *err_msg = NULL;

  //echo("sql getfloat %s", sql);
  createTable(table);
#if 0 //dax
  if (! db) {
    openDB();	// we need to reopen database
  }
  createTable(table);
  rc = sqlite3_exec(db, sql, getFloat_cb, &val, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d err getfloat %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  //echo("getFloat: %s.%s %.2f", table, col, val);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s err prepare %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    val = sqlite3_column_double(stmt, 0);
    echo("getFloat: %s.%s %.2f", table, col, val);
  }
  else if (rc != SQLITE_DONE) {
    error("rc=%d err stepdouble %s", rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  query(sql);
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    insertCol(table, col, name, world); // then insert col into the table
    mysql_free_result(res);
    return ERR_SQL;	// no row
  }
  val = atof(row[0]);
  mysql_free_result(res);
#endif

  return val;	// return a float
}

/** Gets a string (retstring) from a row in the sql table
 *  if string is not found, the string is inserted
 */
int VRSql::getString_cb(void *val, int argc, char **argv, char**azColName)
{
  char *v = (char *)val;
  strcpy(v, argv[0]);
  return 0;
}

int VRSql::getString(const char *table, const char *col, const char *name, const char *world, char *retstring, uint16_t irow)
{
  sprintf(sql, "SELECT %s FROM %s WHERE %s='%s%s%s'",
          col, table, C_NAME, name, (*world) ? "@" : "", world);

#if USE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  createTable(table);
  //echo("sql getstring %s %s", table, sql);
#if 0 //dax
  rc = sqlite3_exec(db, sql, getString_cb, retstring, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d err getstring %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  echo("getString: %s.%s %s", table, col, retstring);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s err getstring %s sql=%s", table, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    if (retstring) {
      strcpy(retstring, (char *) sqlite3_column_text(stmt, 0));
      echo("getString: %s.%s %s", table, col, retstring);
    }
  }
  else {
    error("rc=%d err stepstring %s", rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql)) return ERR_SQL;
  res = mysql_store_result(db);
  mysql_data_seek(res, irow);
  if ((row = mysql_fetch_row(res)) == NULL) {
    insertCol(table, col, name, world); // then insert col into the table
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
int VRSql::getSubstring_cb(void *val, int argc, char **argv, char**azColName)
{
  char *v = (char *)val;
  strcpy(v, argv[0]);
  return 0;
}

int VRSql::getSubstring(const char *table, const char *pattern, uint16_t irow, char *retstring)
{
  sprintf(sql, "SELECT %s FROM %s WHERE %s regexp '%s'",
          C_NAME, table, C_NAME, pattern);

#if USE_SQLITE
/** bad code !!! FIXME 
**/
  int rc;
  char *err_msg = NULL;

  createTable(table);
  //echo("sql getsubstring %s %s", table, sql);
#if 0 //dax
  rc = sqlite3_exec(db, sql, getSubstring_cb, retstring, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d err getsubstring %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  echo("getSubstring: %s.%s %s", table, pattern, retstring);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s err getsubstring %s sql=%s", table, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    if (retstring) {
      strcpy(retstring, (char *) sqlite3_column_text(stmt, 0));
      echo("getSubstring: %s.%s %s", table, pattern, retstring);
    }
  }
  else {
    error("rc=%d err stepsubstring %s", rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql)) return ERR_SQL;
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
int VRSql::getRows_cb(void *val, int argc, char **argv, char**azColName)
{
  echo("row_cb: argc=%d argv=%s azcolname=%s", argc, argv[0], azColName[0]);
  int *v = (int *)val;
  *v = atoi(argv[0]);
  return 0;
}

int VRSql::getRows(const char *table)
{
  int val = 0;

  sprintf(sql, "SELECT count(*) FROM %s", table);

#if USE_SQLITE
  int rc;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
  createTable(table);
#if 0 //dax
  rc = sqlite3_exec(db, sql, getRows_cb, &val, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d err getcount %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  //echo("getRows: %s %d", table, val);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s err preparerows %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    val = sqlite3_column_count(stmt);
    //echo("getRosw: %s %d", table, val);
  }
  else if (rc != SQLITE_DONE) {
    error("%s err steprows %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql)) return ERR_SQL;
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

int VRSql::getRows(const char *table, const char *col, const char *pattern)
{
  int val = 0;

  sprintf(sql, "SELECT count(*) FROM %s WHERE %s REGEXP %s", table, col, pattern);

#if USE_SQLITE
  int rc;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
  createTable(table);
#if 0 //dax
  rc = sqlite3_exec(db, sql, getRows_cb, &val, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d err getcountwhere %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return 0;
  }
  //echo("getRowswhere: %s.%s.%s %d", table, col, pattern, val);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s err preparerows %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    val = sqlite3_column_count(stmt);
    //echo("getRoswwhere: %s.%s.%s %d", table, col, pattern, val);
  }
  else if (rc != SQLITE_DONE) {
    error("%s err steprows %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql)) return ERR_SQL;
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
  //echo("sql createdatabase %s", database);
  sprintf(sql, "CREATE DATABASE IF NOT EXISTS %s", database);
  query(sql);
}

/** Creates table */
void VRSql::createTable(const char *table)
{
  //echo("sql createtable %s %x", table, db);
  sprintf(sql, "CREATE TABLE IF NOT EXISTS %s (name text(16), state tinyint(255), x float(24), y float(24), z float(24), az float(24), ax float(24), ay float(24), r float(24), g float(24), b float(24), a float(24), owner text(16), url text(64), geom text(64), bap text(128))", table);
  query(sql);
}


// inserts

/** Insert row into the sql table */
void VRSql::insertRow(WObject *o)
{
  createTable(o->typeName());
  sprintf(sql, "INSERT INTO %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) VALUES ('%s@%s', '0', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', '%s', 'NULL', 'NULL')",
          o->typeName(),
          C_NAME, C_ST, C_X, C_Y, C_Z, C_AZ, C_AX, C_AY, C_OWN, C_GEOM, C_URL,
          o->named(), World::current()->getName(), o->ownerName());
  //echo("sql insertrow %s %s", o->typeName(), sql);
  query(sql);
}

/** Insert col into the sql table */
void VRSql::insertCol(const char *table, const char *col, const char *name, const char *world)
{
  sprintf(sql, "INSERT INTO %s (%s,%s) VALUES ('%s%s%s', 'NULL')",
          table, C_NAME, col, name, (*world) ? "@" : "", world);
  //echo("sql insertcol %s %s", table, sql);
  query(sql);
}


// updates

/** Updates row int into the sql table */
void VRSql::updateInt(WObject *o, const char *table, const char *col, const char *name, const char *world, int val)
{
  sprintf(sql, "UPDATE %s SET %s=%d WHERE %s='%s%s%s'",
          table, col, val, C_NAME, name, (*world) ? "@" : "", world);
  //echo("sql updateint %s %s", table, sql);
  query(sql);
}

/** Updates row float into the sql table */
void VRSql::updateFloat(WObject *o, const char *table, const char *col, const char *name, const char *world, float val)
{
  sprintf(sql, "UPDATE %s SET %s=%.3f WHERE %s='%s%s%s'",
          table, col, val, C_NAME, name, (*world) ? "@" : "", world);
  //echo("sql updatefloat %s %s", table, sql);
  query(sql);
}

/** Updates row string into the sql table */
void VRSql::updateString(WObject *o, const char *table, const char *col, const char *name, const char *world, const char *str)
{
  sprintf(sql, "UPDATE %s SET %s='%s' WHERE %s='%s%s%s'",
          table, col, str, C_NAME, name, (*world) ? "@" : "", world);
  //echo("sql updatestring %s %s %s", table, str, sql);
  query(sql);
}


// deletes

/** Deletes all rows from the sql table */
void VRSql::deleteRows(const char *table)
{
  createTable(table);
  //echo("sql deleterows %s %x", table, db);
  sprintf(sql, "DELETE FROM %s", table);
  query(sql);
}

/** Deletes a row from the sql table */
void VRSql::deleteRow(WObject *o, const char *table, const char *name, const char *world)
{
  //echo("sql deleterow %s", table);
  sprintf(sql, "DELETE FROM %s WHERE %s='%s%s%s'",
          table, C_NAME, name, (*world) ? "@" : "", world);
  query(sql);
}

/** Deletes a row from the sql table matching a string */
void VRSql::deleteRow(WObject *o, const char *str)
{
  //echo("sql deleterowstring %s %s", o->typeName(), str);
  sprintf(sql, "DELETE FROM %s WHERE %s='%s@%s'",
          o->typeName(), C_NAME, str, World::current()->getName());
  query(sql);
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

// gets (select)

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
  int val = getInt(o, C_ST, 0);
  echo("state=%d val=%d", o->state, val);
  return (val != ERR_SQL) ? val : o->state;
}

int VRSql::getState(WObject *o, uint16_t irow)
{
  int val =  getInt(o, C_ST, irow);
  echo("state=%d val=%d", o->state, val);
  return (val != ERR_SQL) ? val : o->state;
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
  float val = getFloat(o, C_X, irow);
  return (val != ERR_SQL) ? val : o->pos.x;
}

float VRSql::getPosY(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_Y, irow);
  return (val != ERR_SQL) ? val : o->pos.y;
}

float VRSql::getPosZ(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_Z, irow);
  return (val != ERR_SQL) ? val : o->pos.z;
}

float VRSql::getPosAZ(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_AZ, irow);
  return (val != ERR_SQL) ? val : o->pos.az;
}

float VRSql::getPosAX(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_AX, irow);
  return (val != ERR_SQL) ? val : o->pos.ax;
}

float VRSql::getPosAY(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_AY, irow);
  return (val != ERR_SQL) ? val : o->pos.ay;
}

float VRSql::getColorR(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_R, irow);
  return (val != ERR_SQL) ? val : o->pos.x;
}

float VRSql::getColorG(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_G, irow);
  return (val != ERR_SQL) ? val : o->pos.y;
}

float VRSql::getColorB(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_B, irow);
  return (val != ERR_SQL) ? val : o->pos.z;
}

float VRSql::getColorA(WObject *o, uint16_t irow)
{
  float val = getFloat(o, C_A, irow);
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
  char pattern[64];
  sprintf(pattern, "'^%s$'", ::g.user);
  int val = getRows("Cart", C_OWN, pattern);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getCount(const char *table)
{
  int val = getRows(table);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getCount(const char *table, const char *world)
{
  char pattern[64];
  sprintf(pattern, "'@%s$'", world);
  int val = getRows(table, C_NAME, pattern);
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
  getGeom(o, o->geomsolid, irow);
}

void VRSql::getGeom(WObject *o, char *geom)
{
  if (geom) getGeom(o, geom, (uint16_t)0);
}

void VRSql::getGeom(WObject *o, char *geom, uint16_t irow)
{
  if (geom) getString(o, C_GEOM, geom, irow);
}

void VRSql::getUrl(WObject *o)
{
  getUrl(o, (uint16_t)0);
}

void VRSql::getUrl(WObject *o, uint16_t irow)
{
  getString(o, C_URL, (char *) o->urlName(), irow);
}

void VRSql::getOwner(WObject *o)
{
  getOwner(o, (uint16_t)0);
}

void VRSql::getOwner(WObject *o, uint16_t irow)
{
  getString(o, C_OWN, (char *) o->ownerName(), irow);
}

void VRSql::getBap(WObject *o, char *bap, uint16_t irow)
{
  getString(o, C_BAP, bap, irow);
}

// updates

void VRSql::updateState(WObject *o)
{
  updateInt(o, C_ST, o->state);
}

void VRSql::updateState(WObject *o, int val)
{
  updateInt(o, C_ST, val);
}

void VRSql::updatePosX(WObject *o)
{
  updateFloat(o, C_X, o->pos.x);
}

void VRSql::updatePosY(WObject *o)
{
  updateFloat(o, C_Y, o->pos.y);
}

void VRSql::updatePosZ(WObject *o)
{
  updateFloat(o, C_Z, o->pos.z);
}

void VRSql::updatePosAZ(WObject *o)
{
  updateFloat(o, C_AZ, o->pos.az);
}

void VRSql::updatePosAX(WObject *o)
{
  updateFloat(o, C_AX, o->pos.ax);
}

void VRSql::updatePosAY(WObject *o)
{
  updateFloat(o, C_AY, o->pos.ay);
}

void VRSql::updateColorR(WObject *o)
{
  updateFloat(o, C_R, o->pos.x);
}

void VRSql::updateColorG(WObject *o)
{
  updateFloat(o, C_G, o->pos.y);
}

void VRSql::updateColorB(WObject *o)
{
  updateFloat(o, C_B, o->pos.z);
}

void VRSql::updateColorA(WObject *o)
{
  updateFloat(o, C_A, o->pos.az);
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
  if (geom) updateString(o, C_GEOM, geom);
}

void VRSql::updateGeom(WObject *o, const char *table, char *geom)
{
  if (geom) updateString(o, table, C_GEOM, geom);
}

void VRSql::updateUrl(WObject *o)
{
  updateString(o, C_URL, o->urlName());
}

void VRSql::updateOwner(WObject *o)
{
  updateString(o, C_OWN, o->ownerName());
}

void VRSql::updateBap(Humanoid *o)
{
  updateString(o, C_BAP, o->bapline);
}

