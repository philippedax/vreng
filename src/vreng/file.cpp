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
#include "prof.hpp"	// cnt_open cnt_close


/* Open a file */
FILE * File::openFile(const char *filename, const char *param)
{
  FILE *f = NULL;
  f = fopen(filename, param);
  if (f) cnt_open++;
  //if (f) error("%d %s %p", cnt_open, filename, f);
  return f; 
}

/* Close a file */
void File::closeFile(FILE *f)
{
  fclose(f);
  cnt_close++;
  //error("%d %p", cnt_close, f);
}

/**
 * check that this machine is big endian
 * return 1 if big endian else 0 if little endian
 */
bool File::bigEndian()
{
  union {
    int testword;
    char testbyte[4];
  } endiantest;

  endiantest.testword = 1;
  return (endiantest.testbyte[0] == 1) ? 0 : 1;
}

/**
 * check that this machine is little endian
 * return 1 if little endian else 0 if big endian
 */
bool File::littleEndian()
{
  return (! bigEndian());
}

void * File::swapEndian(void* addr, int nb)
{
  static char swapped[16];
  switch (nb) {
    case 2: swapped[0]=*((char*)addr+1);
            swapped[1]=*((char*)addr  );
            break;
    case 4: swapped[0]=*((char*)addr+3);
            swapped[1]=*((char*)addr+2);
            swapped[2]=*((char*)addr+1);
            swapped[3]=*((char*)addr  );
            break;
    case 6: swapped[0]=*((char*)addr+5);
            swapped[1]=*((char*)addr+4);
            swapped[2]=*((char*)addr+3);
            swapped[3]=*((char*)addr+2);
            swapped[4]=*((char*)addr+1);
            swapped[5]=*((char*)addr  );
            break;
    case 8: swapped[0]=*((char*)addr+7);
            swapped[1]=*((char*)addr+6);
            swapped[2]=*((char*)addr+5);
            swapped[3]=*((char*)addr+4);
            swapped[4]=*((char*)addr+3);
            swapped[5]=*((char*)addr+2);
            swapped[6]=*((char*)addr+1);
            swapped[7]=*((char*)addr  );
            break;
     case 16:swapped[0]=*((char*)addr+15);
            swapped[1]=*((char*)addr+14);
            swapped[2]=*((char*)addr+13);
            swapped[3]=*((char*)addr+12);
            swapped[4]=*((char*)addr+11);
            swapped[5]=*((char*)addr+10);
            swapped[6]=*((char*)addr+9);
            swapped[7]=*((char*)addr+8);
            swapped[8]=*((char*)addr+7);
            swapped[9]=*((char*)addr+6);
            swapped[10]=*((char*)addr+5);
            swapped[11]=*((char*)addr+4);
            swapped[12]=*((char*)addr+3);
            swapped[13]=*((char*)addr+2);
            swapped[14]=*((char*)addr+1);
            swapped[15]=*((char*)addr  );
            break;
      default:error("Unsupported length for swapEndian");
  }
  return (void*)swapped;
}

void File::localEndian(void * addr, int nb)
{           
  if (bigEndian()) {
    char *tmp = (char *) swapEndian(addr, nb);
    memcpy(addr, tmp, nb);
  }         
}

void File::convertShort(uint16_t *array, long length)
{
  uint32_t b1, b2;
  uint8_t *ptr;

  ptr = (uint8_t *) array;
  while (length--) {
    b1 = *ptr++;
    b2 = *ptr++;
    *array++ = (b1 << 8) | (b2);
  }
}

void File::convertLong(uint32_t *array, long length)
{
  uint32_t b1, b2, b3, b4;
  uint8_t *ptr;

  ptr = (uint8_t *) array;
  while (length--) {
    b1 = *ptr++;
    b2 = *ptr++;
    b3 = *ptr++;
    b4 = *ptr++;
    *array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
  }
}

int File::read_char(FILE *f)
{
  int c = fgetc(f);

  if (c != EOF)
    return c;
  else
    return -1;
}

int File::read_short(FILE *f)
{
  return (read_char(f)<<8) | read_char(f);
}

int File::read_short_le(FILE *f)
{
  return (read_char(f)) | (read_char(f)<<8);
}

int File::read_long(FILE *f)
{
  return (read_char(f)<<24) | (read_char(f)<<16) | (read_char(f)<<8) | read_char(f);
}

int File::read_long_le(FILE *f)
{
  return (read_char(f)) | (read_char(f)<<8) | (read_char(f)<<16) | (read_char(f)<<24);
}

float File::read_float(FILE *f)
{
  int v = read_long(f);
  return (float) *(float *)&v;
}

float File::read_float_le(FILE *f)
{
  int v = read_long_le(f);
  return (float) *(float *)&v;
}

int File::read_string(FILE *f, char *s, int maxlen)
{
  int c;
  int cnt = 0;
  do {
    c = read_char(f);
    if (cnt < maxlen)
      s[cnt] = c;
    else
      s[maxlen-1] = '\0';
    s[cnt] = c;
    cnt++;
  } while (c != 0);
  /* if length of string (including \0) is odd skip another byte */
  if (cnt%2) {
    read_char(f);
    cnt++;
  }
  return cnt;
}

void File::read_buf(FILE *f, char *s, int len)
{
  int c;
  int cnt = 0;
  do {
    c = read_char(f);
    s[cnt] = c;
    cnt++;
    len--;
  } while (len != 0);
}

void File::skip_byte(FILE *f, int skiplen)
{
  fseek(f, skiplen+(skiplen%2), SEEK_CUR);
}
