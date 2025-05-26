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
// file.cpp
//
// File operations
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "file.hpp"
#include "endian.hpp"
#include "stat.hpp"	// opn_file cls_file


/** Constructor */
File::File()
{
  new_file++;
  f = NULL;
}

/** Deestructor */
File::~File()
{
  del_file++;
}

/** Open a file */
FILE * File::open(const char *filename, const char *param)
{
  f = fopen(filename, param);
  if (f) opn_file++;
  return f; 
}

/** Close a file */
void File::close()
{
  if (f) {
    fclose(f);
    cls_file++;
  }
}

/** Close a file - static */
void File::close(FILE *fp)
{
  if (fp) {
    fclose(fp);
    cls_file++;
  }
}

////////////////////////////////////
//
// methods commun to file and cache
//

/** Reads a char */
int File::read_char(FILE *f)
{
  int c = fgetc(f);

  if (c == EOF)
    return -1;
  return c;
}

/** Reads a short in little-endian */
int File::read_short_le(FILE *f)
{
  return (read_char(f)) | (read_char(f)<<8);
}

/** Reads a short in big-endian */
int File::read_short_be(FILE *f)
{
  return (read_char(f)<<8) | read_char(f);
}

/** Reads a short */
int File::read_short(FILE *f)
{
  if (Endian::littleEndian())
    return (read_short_le(f));
  else
    return (read_short_be(f));
}

/** Reads a long in little-endian */
int File::read_long_le(FILE *f)
{
  return (read_char(f)) | (read_char(f)<<8) | (read_char(f)<<16) | (read_char(f)<<24);
}

/** Reads a long in big-endian */
int File::read_long_be(FILE *f)
{
  return (read_char(f)<<24) | (read_char(f)<<16) | (read_char(f)<<8) | read_char(f);
}

/** Reads a long */
int File::read_long(FILE *f)
{
  if (Endian::littleEndian())
    return (read_long_le(f));
  else
    return (read_long_be(f));
}

/** Reads a float in big-endian */
float File::read_float_be(FILE *f)
{
  int v = read_long_be(f);
  return static_cast<float>(*(float *)&v);
}

/** Reads a float in little-endian */
float File::read_float_le(FILE *f)
{
  int v = read_long_le(f);
  return (float) *(float *)&v;
}

/** Reads a float */
float File::read_float(FILE *f)
{
  if (Endian::littleEndian())
    return (read_float_le(f));
  else
    return (read_float_be(f));
}

/** Reads a string */
int File::read_string(FILE *f, char *str, int maxlen)
{
  int c;
  int cnt = 0;

  do {
    c = read_char(f);
    if (cnt >= maxlen) {
      str[maxlen-1] = '\0';
      break;
    }
    str[cnt] = c;
    cnt++;
  } while (c != 0);
  /* if length of string (including \0) is odded skip another byte */
  if (cnt%2) {
    read_char(f);
    cnt++;
  }
  return cnt;
}

/** Reads a bloc */
int File::read_buf(FILE *f, char *buf, int len)
{
  int c;
  int cnt = 0;

  do {
    c = read_char(f);
    buf[cnt] = c;
    cnt++;
    len--;
  } while (len != 0) ;
  return cnt;
}

/** Skips an offset */
void File::skip(FILE *f, int offset)
{
  fseek(f, offset+(offset%2), SEEK_CUR);
}

/** Returns true and the line else returns false */
bool File::nextLine(FILE *f, char *line)
{
  int i = 0;

  for (i = 0; ; i++) {
    int c = read_char(f);

    if (c == '\n') break;       // eol
    if (c < 0) {      		// file eof
      line[i] = '\0';
      return false;             // end of file
    }
    line[i] = c;
  }
  line[i] = '\0';       // null terminated
  return true;
}
