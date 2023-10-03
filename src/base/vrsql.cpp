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
static const char * USER = "vreng";	///< username
static const char * PASSWD = NULL;	///< no password
static const char * C_NAME = "name";	///< column name      : varchar(32)
static const char * C_STATE = "state";	///< column state     : int
static const char * C_X = "x";		///< column x         : real
static const char * C_Y = "y";		///< column y         : real
static const char * C_Z = "z";		///< column z         : real
static const char * C_AZ = "az";	///< column az        : real
static const char * C_OWNER = "owner";	///< column owner     : varchar(16)
static const char * C_GEOM = "geom";	///< column geom      : varchar(256)


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

  char pathdb[128];
  char *err_msg = NULL;

  sprintf(pathdb, "%s/.vreng/%s", ::g.env.home(), DB);
  int rc = sqlite3_open(pathdb, &db);
  if (rc != SQLITE_OK) {
    error("Cannot open database: %s", sqlite3_errmsg(db));
    //echo("open db: %x pathdb=%s rc=%d", db, pathdb, rc);
    return false;
  }
  return true;
}
#endif

#if USE_MYSQL
/** Establishes a link with the mysql server */
bool VRSql::connectDB()
{
  if (::g.pref.nopersist == true)
    return false;

#ifdef HAVE_MYSQL_REAL_CONNECT
  if ((db = mysql_init(db)) != NULL) {
    if (! mysql_real_connect(db, DEF_MYSQL_SERVER, USER, PASSWD, DB, 0, NULL, 0)) {
      error("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
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
/** Establishes a link with the pgsql server */
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

/** Allocates and returns VRSql handle */
VRSql * VRSql::init()
{
  vrsql = new VRSql();

  bool r = false;
  if (vrsql) {
#if USE_SQLITE
    r = vrsql->openDB();	// open sqlite database
#elif USE_MYSQL
    r = vrsql->connectDB();     // connect to database MySql server
    vrsql->createDatabase(DB);
#elif USE_PGSQL
    r = vrsql->connectDB();     // connect to database PGSql server
    vrsql->createDatabase(DB);
#endif
    if (! r) {
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

void VRSql::queryTrace(const char *sql)
{
  char strupd[16], strins[32];

  sprintf(strupd, "UPDATE %s", ::g.pref.sqltable);
  sprintf(strins, "INSERT INTO %s", ::g.pref.sqltable);
  if (! stringcmp(sql, strupd) || ! stringcmp(sql, strins)) {
    echo("%s", sql);
  }
}

/** Sends a query SQL command */
bool VRSql::query(const char *sql)
{
  //echo("query: %s", sql);
  if (::g.pref.sql) queryTrace(sql);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
#if 0 //dax
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
    echo("query step: %s", sqlite3_column_text(stmt, 0));
  }
  if (rc != SQLITE_DONE) {
    error("query err: rc=%d", rc);
    sqlite3_finalize(stmt);
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
    //PQfinish(db);
    return false;
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


/** Selects an integer value from a row in the sql table
 *  if value doesn't exist, the value is inserted
 */
int VRSql::selectInt_cb(void *val, int argc, char **argv, char**azColName)
{
  echo("int_cb: argc=%d argv=%s azcolname=%s", argc, argv[0], azColName[0]);
  int *v = (int *)val;
  *v = atoi(argv[0]);
  return 0;
}

int VRSql::selectInt(const char *table, const char *col, const char *name, const char *world, uint16_t irow)
{
  int val = 0;

  if (! name) return ERR_SQL;

  sprintf(sql, "SELECT %s FROM %s WHERE name='%s%s%s'",
          col, table, name, (*world) ? "@" : "", world);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
  //echo("selectint %s", sql);
  createTable(table);
#if 0 //dax
  rc = sqlite3_exec(db, sql, &VRSql::selectInt_cb, &val, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d selectint %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  //echo("selectInt: %s.%s %d", table, col, val);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s.%s %d prepare %s", table,col,irow, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    error("%s.%s %d stepint rc=%d %s", table, col, irow, rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    val = ERR_SQL;
  }
  if (rc != SQLITE_DONE) {
    error("%s.%s %d stepint rc=%d %s", table, col, irow, rc, sqlite3_errmsg(db));
    val = ERR_SQL;
  }
  //else if (sqlite3_column_type(stmt, 0) == SQLITE_NULL) {
  //  echo("%s.%s %d stepint null", table, col, irow);
  //  val = ERR_SQL;
  //}
  else {
    val = sqlite3_column_int(stmt, 0);
    //echo("selectInt: %s.%s val=%d", table, col, val);
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql))
    return ERR_SQL;
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

/** Selects a float value from a row in the sql table
 *  if value doesn't exist, the value is inserted
 */
int VRSql::selectFloat_cb(void *val, int argc, char **argv, char**azColName)
{
  echo("float_cb: argc=%d argv=%s azcolname=%s", argc, argv[0], azColName[0]);
  float *v = (float *)val;
  *v = atof(argv[0]);
  return 0;
}

float VRSql::selectFloat(const char *table, const char *col, const char *name, const char *world, uint16_t irow)
{
  float val = 0;

  if (! name) return ERR_SQL;

  sprintf(sql, "SELECT %s FROM %s WHERE name='%s%s%s'",
          col, table, name, (*world) ? "@" : "", world);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  //echo("selectfloat %s", sql);
  if (! db) {
    openDB();	// we need to reopen database
  }
  createTable(table);
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s prepare %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    error("%s.%s %d stepfloat rc=%d %s", table, col, irow, rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    val = ERR_SQL;
  }
  if (rc != SQLITE_DONE) {
    error("%s.%s %d stepfloat rc=%d %s", table, col, irow, rc, sqlite3_errmsg(db));
    val = ERR_SQL;
  }
  //else if (sqlite3_column_type(stmt, 0) == SQLITE_NULL) {
  //  //echo("%s.%s %d stepfloat null", table, col, irow);
  //  val = ERR_SQL;
  //}
  else {
    val = sqlite3_column_double(stmt, 0);
    //echo("selectFloat: %s.%s val=%.2f", table, col, val);
  }
  sqlite3_finalize(stmt);

#elif USE_MYSQL
  if (! query(sql))
    return ERR_SQL;
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

/** Selects a string (retstring) from a row in the sql table
 *  if string is not found, the string is inserted
 */
int VRSql::selectString_cb(void *val, int argc, char **argv, char**azColName)
{
  char *v = (char *)val;
  strcpy(v, argv[0]);
  return 0;
}

int VRSql::selectString(const char *table, const char *col, const char *name, const char *world, char *retstring, uint16_t irow)
{
  int val = 0;

  if (! name) return ERR_SQL;

  sprintf(sql, "SELECT %s FROM %s WHERE name='%s%s%s'",
          col, table, name, (*world) ? "@" : "", world);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  //echo("selectstring %s %s", table, sql);
  createTable(table);
#if 0 //dax
  rc = sqlite3_exec(db, sql, selectString_cb, retstring, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d selectstring %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  echo("selectString: %s.%s %s", table, col, retstring);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s selectstring %s sql=%s", table, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    error("%s.%s %d stepstring rc=%d %s", table, col, irow, rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    val = ERR_SQL;
  }
  if (rc != SQLITE_DONE) {
    error("%s.%s %d stepstring rc=%d %s", table, col, irow, rc, sqlite3_errmsg(db));
    val = ERR_SQL;
  }
  //else if (sqlite3_column_type(stmt, 0) == SQLITE_NULL) {
  //  //echo("%s.%s %d stepstring null", table, col, irow);
  //  val = ERR_SQL;
  //}
  else {
    if (retstring) {
      strcpy(retstring, (char *) sqlite3_column_text(stmt, 0));
      //echo("selectString: %s.%s %s", table, col, retstring);
    }
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql))
    return ERR_SQL;
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

int VRSql::selectSubstring(const char *table, const char *like, uint16_t irow, char *retstring)
{
  int val = 0;

  sprintf(sql, "SELECT name FROM %s WHERE 'name' LIKE '%s'", table, like);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  //echo("selectsubstring %s %s", table, sql);
  createTable(table);
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s selectsubstring %s sql=%s", table, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    error("%s %d stepsubstring rc=%d %s", table, irow, rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    val = ERR_SQL;
  }
  if (rc != SQLITE_DONE) {
    error("%s %d stepsubstring rc=%d %s", table, irow, rc, sqlite3_errmsg(db));
    val = ERR_SQL;
  }
  //else if (sqlite3_column_type(stmt, 0) == SQLITE_NULL) {
  //  //echo("%s %d stepsubstring null", table, irow);
  //  val = ERR_SQL;
  //}
  else {
    if (retstring) {
      strcpy(retstring, (char *) sqlite3_column_text(stmt, 0));
      //echo("selectSubstring: %s %s", table, retstring);
    }
  }
  sqlite3_finalize(stmt);

#elif USE_MYSQL
  if (! query(sql))
    return ERR_SQL;
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
  if (! strstr(row[0], like)) {
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
int VRSql::countRows_cb(void *val, int argc, char **argv, char**azColName)
{
  echo("row_cb: argc=%d argv=%s azcolname=%s", argc, argv[0], azColName[0]);
  int *v = (int *)val;
  *v = atoi(argv[0]);
  return 0;
}

int VRSql::countRows(const char *table)
{
  int val = 0;

  sprintf(sql, "SELECT COUNT(DISTINCT name) FROM %s", table);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
  createTable(table);
#if 0 //dax
  rc = sqlite3_exec(db, sql, countRows_cb, &val, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d countrows %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  echo("countrows: %s val=%d", table, val);
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s preparerows %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    error("%s steprow rc=%d %s", table, rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    val = ERR_SQL;
  }
  if (rc != SQLITE_DONE) {
    error("%s steprow rc=%d %s", table, rc, sqlite3_errmsg(db));
    val = ERR_SQL;
  }
  //else if (sqlite3_column_type(stmt, 0) == SQLITE_NULL) {
  //  //echo("%s steprow null", table);
  //  val = ERR_SQL;
  //}
  else {
    val = sqlite3_column_int(stmt, 0);
    //echo("countRows: %s val=%d", table, val);
  }
  sqlite3_finalize(stmt);
#endif

#elif USE_MYSQL
  if (! query(sql))
    return ERR_SQL;
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

int VRSql::countRows(const char *table, const char *col, const char *like)
{
  int val = 0;

  sprintf(sql, "SELECT COUNT(DISTINCT name) FROM %s WHERE '%s' LIKE '%s'", table, col, like);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

  if (! db) {
    openDB();	// we need to reopen database
  }
  createTable(table);
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("%s preparerows %s", table, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    error("%s.%s steprows rc=%d %s", table, col, rc, sqlite3_errmsg(db));
    sqlite3_free(err_msg);
    val = ERR_SQL;
  }
  if (rc != SQLITE_DONE) {
    error("%s.%s steprows rc=%d %s", table, col, rc, sqlite3_errmsg(db));
    val = ERR_SQL;
  }
  //else if (sqlite3_column_type(stmt, 0) == SQLITE_NULL) {
  //  //echo("%s.%s steprows null", table, col);
  //  val = ERR_SQL;
  //}
  else {
    val = sqlite3_column_int(stmt, 0);
    //echo("countrowswhere: %s.%s.%s val=%d", table, col, like, val);
  }
  sqlite3_finalize(stmt);

#elif USE_MYSQL
  if (! query(sql))
    return ERR_SQL;
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

int VRSql::checkRow(const char *table, const char *name, const char *world)
{
  int val = 0;

  if (!name) return 0;
  createTable(table);
  sprintf(sql, "SELECT * FROM %s WHERE name='%s%s%s'", table, name, (*world) ? "@" : "", world);

#if USE_SQLITE
  int rc = 0;
  char *err_msg = NULL;

#if 0 //dax
  rc = sqlite3_exec(db, sql, 0, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    error("%s rc=%d checkrow %s sql=%s", table, rc, sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    val = ERR_SQL;
    return ERR_SQL;
  }
#else
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    error("checkrow prepare %s sql=%s", sqlite3_errmsg(db), sql);
    sqlite3_free(err_msg);
    return ERR_SQL;
  }
  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    error("checkrow err: rc=%d", rc);
    sqlite3_finalize(stmt);
    return ERR_SQL;
  }
  val = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
#endif
#endif

  echo("checkrow: %d", val);
  return val;
}

//
//---------------------- methods independants of RDB system ---------------------
//


///////////
// create
///////////

/** Creates database */
void VRSql::createDatabase(const char *database)
{
  //echo("createdatabase %s", database);
  sprintf(sql, "CREATE DATABASE IF NOT EXISTS %s", database);
  query(sql);
}

/** Creates table */
void VRSql::createTable(const char *table)
{
  //echo("createtable %s", table);
  sprintf(sql, "CREATE TABLE IF NOT EXISTS %s (name VARCHAR(32) NOT NULL, state INT NOT NULL, x REAL NOT NULL, y REAL NOT NULL, z REAL NOT NULL, az REAL, owner VARCHAR(16), geom VARCHAR(256))", table);
  query(sql);
}


///////////
// inserts
///////////

/** Insert row into the sql table */
void VRSql::insertRow(WO *o)
{
  if (! o->named()) return;	// no name
  //echo("insertrow: %.1f %.1f %.1f", o->pos.x, o->pos.y, o->pos.z);
  createTable(o->typeName());
  sprintf(sql, "INSERT INTO %s (%s,%s,%s,%s,%s,%s,%s,%s) \
                VALUES ( '%s@%s', '%d', '%f', '%f', '%f', '%f', '%s', '%s' )",
          o->typeName(),
          C_NAME, C_STATE, C_X, C_Y, C_Z, C_AZ, C_OWNER, C_GEOM,
          o->named(), World::current()->getName(),		// name
          o->state,						// state
          o->pos.x, o->pos.y, ABSF(o->pos.z), o->pos.az,	// pos
          o->ownerName(), o->geomsolid);			// owner + geom
  //echo("insertrow %s", sql);
  query(sql);
}

/** Insert col into the sql table */
void VRSql::insertCol(const char *table, const char *col, const char *name, const char *world)
{
  if (! name) return;	// no name
  sprintf(sql, "INSERT INTO %s (name,%s) VALUES ('%s%s%s', 'NULL')",
          table, col, name, (*world) ? "@" : "", world);
  echo("insertcol %s", sql);
  query(sql);
}


///////////
// updates
///////////

/** Updates int into the sql table */
void VRSql::updateInt(WO *o, const char *table, const char *col, const char *name, const char *world, int val)
{
  if (! name) return;	// no name -> no update
  createTable(table);
  char pat[32];
  sprintf(pat, "%s@%s", name, world);
  //if (checkRow(table, name, world) == ERR_SQL) {
  //if (countRows(table, C_NAME, pat) == 0) {
  //  echo("int insertrow");
  //  insertRow(o);
  //}
  sprintf(sql, "UPDATE %s SET %s=%d WHERE name='%s%s%s'",
          table, col, val, name, (*world) ? "@" : "", world);
  echo("updateint %s %s", table, sql);
  query(sql);
}

/** Updates float into the sql table */
void VRSql::updateFloat(WO *o, const char *table, const char *col, const char *name, const char *world, float val)
{
  if (! name) return;	// no name -> no update
  createTable(table);
  char pat[32];
  sprintf(pat, "%s@%s", name, world);
  //if (checkRow(table, name, world) == ERR_SQL) {
  //if (countRows(table, C_NAME, pat) == 0) {
  //  echo("float insertrow");
  //  insertRow(o);
  //}
  sprintf(sql, "UPDATE %s SET %s=%.2f WHERE name='%s%s%s'",
          table, col, val, name, (*world) ? "@" : "", world);
  //echo("updatefloat %s %s", table, sql);
  query(sql);
}

/** Updates string into the sql table */
void VRSql::updateString(WO *o, const char *table, const char *col, const char *name, const char *world, const char *str)
{
  if (! name) return;	// no name -> no update
  createTable(table);
  char pat[32];
  sprintf(pat, "%s@%s", name, world);
  //if (checkRow(table, name, world) == ERR_SQL) {
  //if (countRows(table, C_NAME, pat) == 0) {
  //  echo("string insertrow");
  //  //insertRow(o);
  //}
  sprintf(sql, "UPDATE %s SET %s='%s' WHERE name='%s%s%s'",
          table, col, str, name, (*world) ? "@" : "", world);
  echo("updatestring %s %s %s", table, str, sql);
  query(sql);
}

/** Updates int into the sql table */
void VRSql::updateInt(WO *o, const char *col, int val)
{
  if (! o->named()) return;	// no name
  updateInt(o, o->typeName(), col, o->named(), World::current()->getName(), val);
}

/** Updates float into the sql table */
void VRSql::updateFloat(WO *o, const char *col, float val)
{
  if (! o->named()) return;	// no name
  updateFloat(o, o->typeName(), col, o->named(), World::current()->getName(), val);
}

/** Updates string into the sql table */
void VRSql::updateString(WO *o, const char *col, const char *str)
{
  if (! o->named()) return;	// no name
  updateString(o, o->typeName(), col, o->named(), World::current()->getName(), str);
}

/** Updates string into the sql table */
void VRSql::updateString(WO *o, const char *table, const char *col, const char *str)
{
  if (! o->named()) return;	// no name
  updateString(o, table, col, o->named(), World::current()->getName(), str);
}

void VRSql::updateState(WO *o)
{
  updateInt(o, C_STATE, o->state);
}

void VRSql::updateState(WO *o, int val)
{
  updateInt(o, C_STATE, val);
}

void VRSql::updatePosX(WO *o)
{
  updateFloat(o, C_X, o->pos.x);
}

void VRSql::updatePosY(WO *o)
{
  updateFloat(o, C_Y, o->pos.y);
}

void VRSql::updatePosZ(WO *o)
{
  updateFloat(o, C_Z, o->pos.z);
}

void VRSql::updatePosAZ(WO *o)
{
  updateFloat(o, C_AZ, o->pos.az);
}

void VRSql::updatePos(WO *o)
{
  updatePosX(o);
  updatePosY(o);
  updatePosZ(o);
  updatePosAZ(o);
}

void VRSql::updateGeom(WO *o, char *geom)
{
  if (geom) updateString(o, C_GEOM, geom);
}

void VRSql::updateGeom(WO *o, const char *table, char *geom)
{
  if (geom) updateString(o, table, C_GEOM, geom);
}

void VRSql::updateOwner(WO *o)
{
  updateString(o, C_OWNER, o->ownerName());
}


///////////
// deletes
///////////

/** Deletes all rows from the sql table */
void VRSql::deleteRows(const char *table)
{
  //echo("deleterows %s", table);
  createTable(table);
  sprintf(sql, "DELETE FROM %s", table);
  query(sql);
}

/** Deletes a row from the sql table */
void VRSql::deleteRow(WO *o, const char *table, const char *name, const char *world)
{
  //echo("deleterow %s", table);
  if (!name) return;
  createTable(table);
  sprintf(sql, "DELETE FROM %s WHERE name='%s%s%s'",
          table, name, (*world) ? "@" : "", world);
  query(sql);
}

/** Deletes a row from the sql table matching a string */
void VRSql::deleteRow(WO *o, const char *str)
{
  //echo("deleterowstring %s %s", o->typeName(), str);
  createTable(o->typeName());
  sprintf(sql, "DELETE FROM %s WHERE name='%s@%s'",
          o->typeName(), str, World::current()->getName());
  query(sql);
}

/** Deletes a row of this object */
void VRSql::deleteRow(WO *o)
{
  if (! o->named()) return;	// no name
  deleteRow(o, o->typeName(), o->named(), World::current()->getName());
}

/** Deletes all rows of this object */
void VRSql::deleteRows(WO *o)
{
  deleteRows(o->typeName());
}


///////////
// select
///////////

/** Selects an integer value from a row in the sql table */
int VRSql::getInt(WO *o, const char *col, uint16_t irow)
{
  return selectInt(o->typeName(), col, o->named(), World::current()->getName(), irow);
}

/** Selects a float from a row in the sql table */
float VRSql::getFloat(WO *o, const char *col, uint16_t irow)
{
  return selectFloat(o->typeName(), col, o->named(), World::current()->getName(), irow);
}

/** Selects a string from a row in the sql table */
int VRSql::getString(WO *o, const char *col, char *str, uint16_t irow)
{
  return selectString(o->typeName(), col, o->named(), World::current()->getName(), str, irow);
}


///////////
// gets
///////////

int VRSql::getState(WO *o)
{
  int val = getInt(o, C_STATE, 0);
  //echo("state_old=%d val=%d", o->state, val);
  return (val != ERR_SQL) ? val : o->state;
}

int VRSql::getState(WO *o, uint16_t irow)
{
  int val =  getInt(o, C_STATE, irow);
  //echo("state_old=%d val=%d", o->state, val);
  return (val != ERR_SQL) ? val : o->state;
}

void VRSql::getPos(WO *o)
{
  o->pos.x = getPosX(o, 0);
  o->pos.y = getPosY(o, 0);
  o->pos.z = getPosZ(o, 0);
  o->pos.az = getPosAZ(o, 0);
}

void VRSql::getPos(WO *o, uint16_t irow)
{
  o->pos.x = getPosX(o, irow);
  o->pos.y = getPosY(o, irow);
  o->pos.z = getPosZ(o, irow);
  o->pos.az = getPosAZ(o, irow);
}

float VRSql::getPosX(WO *o, uint16_t irow = 0)
{
  float val = getFloat(o, C_X, irow);
  return (val != ERR_SQL) ? val : o->pos.x;
}

float VRSql::getPosY(WO *o, uint16_t irow)
{
  float val = getFloat(o, C_Y, irow);
  return (val != ERR_SQL) ? val : o->pos.y;
}

float VRSql::getPosZ(WO *o, uint16_t irow)
{
  float val = getFloat(o, C_Z, irow);
  return (val != ERR_SQL) ? val : o->pos.z;
}

float VRSql::getPosAZ(WO *o, uint16_t irow)
{
  float val = getFloat(o, C_AZ, irow);
  return (val != ERR_SQL) ? val : o->pos.az;
}

/* Deletes objects owned by user in Cart */
int VRSql::getCountCart()
{
  char pattern[64];
  sprintf(pattern, "'^%s'", ::g.user);
  int val = countRows("Cart", C_OWNER, pattern);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getCount(const char *table)
{
  int val = countRows(table);
  return (val != ERR_SQL) ? val : 0;
}

/* Gets number of rows where world is - called by checkPersist in world.cpp */
int VRSql::getCount(const char *table, const char *world)
{
  char pattern[64];
  sprintf(pattern, "'@%s'", world);
  int val = countRows(table, C_NAME, pattern);
  return (val != ERR_SQL) ? val : 0;
}

int VRSql::getCount(const char *table, const char *name, const char *world)
{
  if (! name) return 0;
  char pattern[64];
  sprintf(pattern, "'%s@%s'", name, world);
  int val = countRows(table, C_NAME, pattern);
  return (val != ERR_SQL) ? val : 0;
}

/* Gets qualified name in C_NAME - called by checkPersist in world.cpp */
int VRSql::getName(const char *table, const char *pattern, int numrow, char *retname)
{
  int irow = selectSubstring(table, pattern, numrow, retname);
  //echo("num=%d irow=%d str=%s", numrow, irow, retname);
  return (irow >= 0 ) ? irow : -1;
}

void VRSql::getGeom(WO *o)
{
  getGeom(o, (uint16_t)0);
}

void VRSql::getGeom(WO *o, uint16_t irow)
{
  getGeom(o, o->geomsolid, irow);
}

void VRSql::getGeom(WO *o, char *geom)
{
  if (geom) getGeom(o, geom, (uint16_t)0);
}

void VRSql::getGeom(WO *o, char *geom, uint16_t irow)
{
  if (geom) getString(o, C_GEOM, geom, irow);
}

void VRSql::getOwner(WO *o)
{
  getOwner(o, (uint16_t)0);
}

void VRSql::getOwner(WO *o, uint16_t irow)
{
  getString(o, C_OWNER, (char *) o->ownerName(), irow);
}