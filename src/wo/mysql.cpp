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
#include "vreng.hpp"
#include "mysql.hpp"
#include "vrsql.hpp"
#include "world.hpp"	// current
#include "pref.hpp"	// g.pref.user

#if HAVE_MYSQL

static const char * DB = "vreng";		///< database name
static const char * USER = "vreng";		///< user name
static const char * PASSWD = NULL;		///< no password
static const char * COL_NAME = "name";		///< column name
static const char * COL_STATE = "state";	///< column state
static const char * COL_X = "x";		///< column x
static const char * COL_Y = "y";		///< column y
static const char * COL_Z = "z";		///< column z
static const char * COL_AZ = "az";		///< column az
static const char * COL_AX = "ax";		///< column ax
static const char * COL_AY = "ay";		///< column ay
static const char * COL_R = "r";		///< column r (red)
static const char * COL_G = "g";		///< column g (green)
static const char * COL_B = "b";		///< column b (blue)
static const char * COL_A = "a";		///< column a (alpha)
static const char * COL_OWNER = "owner";	///< column owner
static const char * COL_GEOM = "geom";		///< column geom
static const char * COL_URL = "url";		///< column url
static const char * COL_BAP = "bap";		///< column bap


/** Constructor */
MySql::MySql()
{
  mysqlhdl = NULL;
  ressql = NULL;
}

/**
 * Establishes a link with the mysql server
 */
bool VRSql::connect()
{
  if (::g.pref.fast == true)
    return false;

#ifdef HAVE_MYSQL_REAL_CONNECT
  if ((mysqlhdl = mysql_init(mysqlhdl)) != NULL) {
    if (! mysql_real_connect(mysqlhdl, DEF_MYSQL_SERVER, USER, PASSWD, DB, 0, NULL, 0)) {
      warning("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
#if HAVE_MYSQL_ERROR
      if (mysql_errno(mysqlhdl))
        error("mysql_error: %s", mysql_error(mysqlhdl));
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
  if ((mysqlhdl = mysql_connect(NULL, DEF_MYSQL_SERVER, USER, PASSWD)) == NULL) {
    warning("VRSql: %s can't connect %s", USER, DEF_MYSQL_SERVER);
    return false;
  }
  if (mysqlhdl && mysql_select_db(mysqlhdl, DB) != 0) {
    error("VRSql: can't find database %s", DB);
    return false;
  }
  return true;
#endif
}

/** Allocates VRSql */
VRSql * VRSql::init()
{
  if (World::current())
    return NULL;

  vrsql = new VRSql();

  if (vrsql) {
    if (! vrsql->connect()) {
      delete vrsql;
      vrsql = NULL;
    }
  }
  trace(DBG_INIT, "init: vrsql = %p", vrsql);
  return vrsql;
}

/** Closes the sql link */
void MySql::quit()
{
  if (vrsql) {
    if (mysqlhdl) mysql_close(mysqlhdl);
    mysqlhdl = NULL;
  }
}

/** Sends a query SQL command */
bool VRSql::query(const char *cmd)
{
  if (! mysqlhdl) connect();	// we need to reconnect to the MySql server

  trace(DBG_SQL, "query: %s", cmd);
  if (mysql_query(mysqlhdl, cmd) != 0) {
#if HAVE_MYSQL_ERROR
    if (mysql_errno(mysqlhdl)) error("mysql_error: %s", mysql_error(mysqlhdl));
    error("query: %s", cmd);
#endif
    return false;
  }
  return true;
}

/** Gets a result, fetching the row */
MYSQL_RES * VRSql::result()
{
  MYSQL_RES *res = mysql_store_result(mysqlhdl);

  if (res == NULL)
#if HAVE_MYSQL_ERROR
    error("mysql_error: %s", mysql_error(mysqlhdl));
#endif
  return res;
}


/** Gets an integer value from a row in the sql table
 *  if value doesn'nt exist, the value is inserted
 */
int VRSql::getInt(const char *table, const char *col, const char *object, const char *world, uint16_t irow)
{
  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s='%s%s%s'",
      col, table, COL_NAME, object, (*world) ? "@" : "", world);
  if (! query(sqlcmd)) return ERR_MYSQL;
  if ((ressql = result()) == NULL) return ERR_MYSQL;
  mysql_data_seek(ressql, irow);
  if ((row = mysql_fetch_row(ressql)) == NULL) {
    // then insert col into the table
    insertCol(table, col, object, world);
    mysql_free_result(ressql);
    return ERR_MYSQL;	// no row
  }
  int val = (row[0] == NULL ? ERR_MYSQL : atoi(row[0]));
  mysql_free_result(ressql);
  return val;
}

/** Gets a float value from a row in the sql table
 *  if value doesn'nt exist, the value is inserted
 */
float VRSql::getFloat(const char *table, const char *col, const char *object, const char *world, uint16_t irow)
{
  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s='%s%s%s'",
      col, table, COL_NAME, object, (*world) ? "@" : "", world);
  if (! query(sqlcmd)) return ERR_MYSQL;
  if ((ressql = result()) == NULL) return ERR_MYSQL;
  mysql_data_seek(ressql, irow);
  if ((row = mysql_fetch_row(ressql)) == NULL) {
    // then insert col into the table
    insertCol(table, col, object, world);
    mysql_free_result(ressql);
    return ERR_MYSQL;	// no row
  }
  float val = (row[0] == NULL ? ERR_MYSQL : atof(row[0]));
  mysql_free_result(ressql);
  return val;
}

/** Gets a string from a row in the sql table
 *  if string is not found, the string is inserted
 */
int VRSql::getString(const char *table, const char *col, const char *object, const char *world, char *str, uint16_t irow)
{
  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s='%s%s%s'",
      col, table, COL_NAME, object, (*world) ? "@" : "", world);
  if (! query(sqlcmd)) return ERR_MYSQL;
  if ((ressql = result()) == NULL) return ERR_MYSQL;
  mysql_data_seek(ressql, irow);
  if ((row = mysql_fetch_row(ressql)) == NULL) {
    // then insert col into the table
    insertCol(table, col, object, world);
    mysql_free_result(ressql);
    return ERR_MYSQL;	// no row
  }
  if (row[0] == NULL) return ERR_MYSQL;
  if (str) strcpy(str, row[0]);
  mysql_free_result(ressql);
  return irow;
}

/** Gets a count of rows from a sql table */
int VRSql::getCount(const char *table, const char *col, const char *pattern)
{
  sprintf(sqlcmd, "select SQL_CACHE count(*) from %s where %s regexp %s",
      table, col, pattern);
  if (! query(sqlcmd)) return ERR_MYSQL;
  if ((ressql = result()) == NULL) return ERR_MYSQL;
  if ((row = mysql_fetch_row(ressql)) == NULL) {
    mysql_free_result(ressql);
    return 0;	// no row
  }
  int val = (row[0] == NULL ? ERR_MYSQL : atoi(row[0]));
  mysql_free_result(ressql);
  return val;
}

/** Gets a string if the pattern matches
 *  and returns the index of the row
 */
int VRSql::getSubstring(const char *table, const char *pattern, uint16_t irow, char *retstring)
{
  sprintf(sqlcmd, "select SQL_CACHE %s from %s where %s regexp '%s'",
      COL_NAME, table, COL_NAME, pattern);
  if (! query(sqlcmd)) return ERR_MYSQL;
  if ((ressql = result()) == NULL) return ERR_MYSQL;
  mysql_data_seek(ressql, irow);
  if ((row = mysql_fetch_row(ressql)) == NULL) {
    mysql_free_result(ressql);
    return ERR_MYSQL;	// no row
  }
  if (row[0] == NULL) {
    mysql_free_result(ressql);
    return ERR_MYSQL;
  }
  if (! strstr(row[0], pattern)) {
    mysql_free_result(ressql);
    return ERR_MYSQL;	// no match
  }
  if (retstring) strcpy(retstring, row[0]);
  mysql_free_result(ressql);
  return irow;
}

// create

/** Creates table */
void VRSql::createTable(const char *table)
{
#if 1
  sprintf(sqlcmd, "create table if not exists %s ('NULL')", table);
#else
  sprintf(sqlcmd, "create table if not exists %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) values ('NULL', '0', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL')",
          table,
          COL_NAME, COL_STATE, COL_X, COL_Y, COL_Z, COL_AZ, COL_AX, COL_AY, COL_OWNER, COL_GEOM, COL_URL);
#endif
  query(sqlcmd);
}

// inserts

/** Insert row into the sql table */
void VRSql::insertRow(WObject *o)
{
  createTable(o->typeName());
  sprintf(sqlcmd, "insert into %s (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s) values ('%s@%s', '0', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', 'NULL', '%s', 'NULL', 'NULL')",
          o->typeName(),
          COL_NAME, COL_STATE, COL_X, COL_Y, COL_Z, COL_AZ, COL_AX, COL_AY, COL_OWNER, COL_GEOM, COL_URL,
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

#endif // HAVE_MYSQL
