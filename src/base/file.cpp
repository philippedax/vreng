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
#include "vreng.hpp"
#include "file.hpp"
#include "stat.hpp"	// opn_file cls_file


/* Constructor */
File::File()
{
  new_file++;
  f = NULL;
}

/* Deestructor */
File::~File()
{
  del_file++;
}


/* Open a file - after a new File() */
FILE * File::open(const char *filename, const char *param)
{
  f = fopen(filename, param);
  if (f) opn_file++;
  return f; 
}

/* Close a file */
void File::close()
{
  if (f) {
    fclose(f);
    cls_file++;
  }
}

/* Open a file - static */
FILE * File::openFile(const char *filename, const char *param)
{
  FILE *fp = fopen(filename, param);
  if (fp) opn_file++;
  //debug fopenlog(fp, filename);
  return fp; 
}

/* Close a file - static */
void File::close(FILE *fp)
{
  if (fp) {
    fclose(fp);
    cls_file++;
  }
}

/* Close a file - static */
void File::closeFile(FILE *fp)
{
  fclose(fp);
  cls_file++;
  //debug fcloselog(fp);
}

/**
 * check whether this machine is big endian
 * return 1 if big endian else 0 if little endian
 * static
 */
bool File::bigEndian()
{
  union {
    int word;
    char byte[4];
  } endian;

  endian.word = 1;
  return (endian.byte[0] == 1) ? 0 : 1;
}

/**
 * check whether this machine is little endian
 * return 1 if little endian else 0 if big endian
 * static
 */
bool File::littleEndian()
{
  return (! bigEndian());
}

void * File::swapEndian(void* data, int n)
{
  static char swapped[16];

  switch (n) {
    case 2: swapped[0] = *((char*)data+1);
            swapped[1] = *((char*)data  );
            break;
    case 4: swapped[0] = *((char*)data+3);
            swapped[1] = *((char*)data+2);
            swapped[2] = *((char*)data+1);
            swapped[3] = *((char*)data  );
            break;
#if 0 //notused
    case 6: swapped[0] = *((char*)data+5);
            swapped[1] = *((char*)data+4);
            swapped[2] = *((char*)data+3);
            swapped[3] = *((char*)data+2);
            swapped[4] = *((char*)data+1);
            swapped[5] = *((char*)data  );
            break;
    case 8: swapped[0] = *((char*)data+7);
            swapped[1] = *((char*)data+6);
            swapped[2] = *((char*)data+5);
            swapped[3] = *((char*)data+4);
            swapped[4] = *((char*)data+3);
            swapped[5] = *((char*)data+2);
            swapped[6] = *((char*)data+1);
            swapped[7] = *((char*)data  );
            break;
     case 16:swapped[0] = *((char*)data+15);
            swapped[1] = *((char*)data+14);
            swapped[2] = *((char*)data+13);
            swapped[3] = *((char*)data+12);
            swapped[4] = *((char*)data+11);
            swapped[5] = *((char*)data+10);
            swapped[6] = *((char*)data+9);
            swapped[7] = *((char*)data+8);
            swapped[8] = *((char*)data+7);
            swapped[9] = *((char*)data+6);
            swapped[10] = *((char*)data+5);
            swapped[11] = *((char*)data+4);
            swapped[12] = *((char*)data+3);
            swapped[13] = *((char*)data+2);
            swapped[14] = *((char*)data+1);
            swapped[15] = *((char*)data  );
            break;
#endif //notused
      default:error("Unsupported length for swapEndian");
  }
  return static_cast<void *>(swapped);
}

void File::localEndian(void *data, int n)
{           
  if (bigEndian()) {
    char *tmp = reinterpret_cast<char *>(swapEndian(data, n));
    memcpy(data, tmp, n);
  }         
}

void File::convertShort(uint16_t *array, int len)
{
  uint32_t b1, b2;
  char *p;

  p = reinterpret_cast<char *>(array);
  while (len--) {
    b1 = *p++;
    b2 = *p++;
    *array++ = (b1 << 8) | (b2);
  }
}

void File::convertLong(uint32_t *array, int len)
{
  uint32_t b1, b2, b3, b4;
  char *p;

  p = reinterpret_cast<char *>(array);
  while (len--) {
    b1 = *p++;
    b2 = *p++;
    b3 = *p++;
    b4 = *p++;
    *array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
  }
}

//
// methods commun to file and cache
//

/* Reads a char */
int File::read_char(FILE *f)
{
  int c = fgetc(f);

  if (c == EOF)
    return -1;
  return c;
}

/* Reads a short in big-endian */
int File::read_short(FILE *f)
{
  return (read_char(f)<<8) | read_char(f);
}

/* Reads a short in little-endian */
int File::read_short_le(FILE *f)
{
  return (read_char(f)) | (read_char(f)<<8);
}

/* Reads a long in big-endian */
int File::read_long(FILE *f)
{
  return (read_char(f)<<24) | (read_char(f)<<16) | (read_char(f)<<8) | read_char(f);
}

/* Reads a long in little-endian */
int File::read_long_le(FILE *f)
{
  return (read_char(f)) | (read_char(f)<<8) | (read_char(f)<<16) | (read_char(f)<<24);
}

/* Reads a float in big-endian */
float File::read_float(FILE *f)
{
  int v = read_long(f);
  return static_cast<float>(*(float *)&v);
}

/* Reads a float in little-endian */
float File::read_float_le(FILE *f)
{
  int v = read_long_le(f);
  return (float) *(float *)&v;
}

/* Reads a string */
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

/* Reads a bloc */
void File::read_buf(FILE *f, char *buf, int len)
{
  int c;
  int cnt = 0;

  do {
    c = read_char(f);
    buf[cnt] = c;
    cnt++;
    len--;
  } while (len != 0) ;
}

/* Skips an offset */
void File::skip(FILE *f, int offset)
{
  fseek(f, offset+(offset%2), SEEK_CUR);
}

/** returns true and the line else returns false */
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
