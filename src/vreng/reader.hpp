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

#include "prof.hpp"	// new_


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

  virtual FILE * getFileCache(class Texture *tex);
  virtual FILE * getFileCache(void *tex, bool flagclo);
  /**< Gets a file into the cache. */
  virtual FILE * getFileCache(const char *url, char *filename);
  /**< Downloads a file into the cache. */

  virtual char * getFilename(void *tex);
  /**< Gets filename. */

  virtual uint32_t getUInt(FILE *fp);
  /**< Gets unsigned integer. */

  virtual int16_t getShort(FILE *fp);
  /**< Gets short integer. */

private:

#if 0 //notused
  virtual uint8_t getChar(FILE *fp);
  virtual uint8_t getChar();
  /**< Gets next char. */

  virtual uint8_t getChar1(FILE *fp);
  virtual uint8_t getChar1();
  /**< Gets next usable char. */

  //virtual uint8_t getByte(void *texhdl, ImageReader read_func);
  /**< Reads and returns a byte. */

  //virtual int32_t getInt(FILE *fp);
  //virtual int32_t getInt();
  /**< Gets next integer. */

  //virtual void skipSpaces(FILE *fp);
  //virtual void skipSpaces();
  /**< Skips spaces. */

  //virtual void reset();
  /**< Resets buffer. */
#endif
};


#endif
