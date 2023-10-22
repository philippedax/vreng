//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
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
#ifndef FILE_HPP
#define FILE_HPP

/**
 * File class
 */

class File {

public:

  FILE *f;	///< file handle

  File();
  virtual ~File();

  FILE * open(const char *filename, const char *param);
  void close();
  void close(FILE *fp);

  static FILE * openFile(const char *filename, const char *param);
  static void closeFile(FILE *f);

  // Endians
  static bool bigEndian();
  static bool littleEndian();
  static void * swapEndian(void *addr, int nb);
  static void localEndian(void *addr, int nb);
  static void convertShort(uint16_t *array, long length);
  static void convertLong(uint32_t *array, long length);

  // IO
  // see also http.hpp
  int read_char(FILE *f);
  int read_short(FILE *f);
  int read_long(FILE *f);
  float read_float(FILE *f);
  int read_short_le(FILE *f);
  int read_long_le(FILE *f);
  float read_float_le(FILE *f);
  int read_string(FILE *f, char *s, int maxlen);
  void read_buf(FILE *f, char *s, int len);
  void skip(FILE *f, int skiplen);
};


#endif
