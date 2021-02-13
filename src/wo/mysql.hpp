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
#if !HAVE_SQLITE
#define HAVE_MYSQL 1
#include <mysql/mysql.h>
#endif
#endif
#include "wobject.hpp"


#if HAVE_MYSQL

#define	ERR_MYSQL	-11111

/**
 * MySql class
 */
class MySql : public VRSql {

 private:
  static const uint16_t CMD_SIZE = 1024;	///< query size max

  MYSQL_RES *ressql;	///< MySqsl result
  MYSQL_ROW row;	///< MySql row
  char sqlcmd[CMD_SIZE];///< MySql command

 public:
  MYSQL *mysqlhdl;	///< MySql handle

  MySql();		///< constructor

  virtual ~MySql() {};	///< destructor

  static MySql* init();
  /**< initialization */

  virtual bool connect();
  /**< connects to the MySql server */

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

  virtual void createTable(const char *table);
  /**< creates a table */

  virtual void insertRow(WObject *o);
  /**< inserts a row into a table */

  virtual void insertCol(const char *table, const char *col, const char *object, const char *world);
  /**< inserts a column into a row */

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

  virtual void deleteRows(const char *table);
  /**< deletes all rows from table */

  virtual void deleteRow(WObject *o, const char *table, const char *object, const char *world);
  /**< deletes a row from table */

  virtual void deleteRow(WObject *o, const char *str);
  /**< deletes a row from table matching string */

};

#endif // HAVE_MYSQL

#endif
