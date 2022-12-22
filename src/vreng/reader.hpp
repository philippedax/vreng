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
#ifndef READER_HPP
#define READER_HPP

#include "stat.hpp"	// new_


typedef int (*ImageReader) (void *tex, char *buf, int len);

/**
 * Reader class
 */
class Reader {

 public:

  enum {
    KEEP_OPEN,
    KEEP_CLOSE
  };

  void *img_handle;		///< handle (tex).
  ImageReader read_func;	///< http read function.
  int ch;			///< current char.

  Reader(void *tex, ImageReader read_func);
  /**< Constructor. */

  virtual ~Reader() {};
  /**< Destructor */

  uint32_t getUInt(FILE *fp);
  /**< Gets unsigned integer. */

  int16_t getShort(FILE *fp);
  /**< Gets short integer. */

  char * getFilename(void *tex);
  /**< Gets filename. */

#if 0 //notused

  FILE * getFileCache(class Texture *tex);
  FILE * getFileCache(void *tex, bool flagclo);
  /**< Gets a file from the cache. */
  FILE * getFileCache(const char *url, char *filename);
  /**< Downloads a file into the cache. */

private:

  uint8_t getChar(FILE *fp);
  uint8_t getChar();
  /**< Gets next char. */

  uint8_t getChar1(FILE *fp);
  uint8_t getChar1();
  /**< Gets next usable char. */

  //uint8_t getByte(void *texhdl, ImageReader read_func);
  /**< Reads and returns a byte. */

  //int32_t getInt(FILE *fp);
  //int32_t getInt();
  /**< Gets next integer. */

  //void skipSpaces(FILE *fp);
  //void skipSpaces();
  /**< Skips spaces. */

  //void reset();
  /**< Resets buffer. */
#endif
};


#endif
