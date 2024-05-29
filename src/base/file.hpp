//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2024 Philippe Dax
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
// file.hpp
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

  int read_char(FILE *f);
  int read_short_le(FILE *f);
  int read_short_be(FILE *f);
  int read_short(FILE *f);
  int read_long_le(FILE *f);
  int read_long_be(FILE *f);
  int read_long(FILE *f);
  float read_float_le(FILE *f);
  float read_float_be(FILE *f);
  float read_float(FILE *f);
  int read_string(FILE *f, char *s, int maxlen);
  int read_buf(FILE *f, char *s, int len);
  void skip(FILE *f, int offset);
  bool nextLine(FILE *f, char *line);
};

#endif
