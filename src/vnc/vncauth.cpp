//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2008 Philippe Dax
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
// vncauth.cpp
//
// Functions for VNC password management and authentication.
//
//  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "vncauth.hpp"
#include "file.hpp"	// open, close


/**
 * We use a fixed key to store passwords, since we assume that our local
 * file system is secure but nonetheless don't want to store passwords
 * as plaintext.
 */
uint8_t fixedkey[8] = {23,82,107,6,35,78,88,7};

/**
 * Encrypt a password and store it in a file.  Returns 0 if successful,
 * 1 if the file could not be written.
 */
int vncEncryptAndStorePasswd(char *passwd, char *fname)
{
  FILE *fp;
  uint8_t encryptedPasswd[8];

  File *file = new File();
  if ((fp = file->open(fname, "w")) == NULL) return 1;

  chmod(fname, S_IRUSR|S_IWUSR);

  /* pad password with nulls */
  for (uint32_t i=0; i < 8; i++) {
    encryptedPasswd[i] = (i < strlen(passwd)) ? passwd[i] : '\0';
  }

  // Do encryption in-place - this way we overwrite our copy of the plaintext password
  deskey(fixedkey, EN0);

  des(encryptedPasswd, encryptedPasswd);

  for (int i=0; i < 8; i++) {
    putc(encryptedPasswd[i], fp);
  }
  file->close();
  delete file;
  return 0;
}

/**
 * Decrypt a password from a file.  Returns a pointer to a newly allocated
 * string containing the password or a null pointer if the password could
 * not be retrieved for some reason.
 */
char * vncDecryptPasswdFromFile(char *fname)
{
  uint8_t *passwd = new uint8_t[9];

  File *file = new File();
  FILE *fp;
  if ((fp = file->open(fname, "r")) == NULL) return NULL;

  for (int i=0; i < 8; i++) {
    int ch = getc(fp);
    if (ch == EOF) {
      file->close();
      delete file;
      return NULL;
    }
    passwd[i] = ch;
  }
  file->close();
  delete file;
  deskey(fixedkey, DE1);
  des(passwd, passwd);
  passwd[8] = '\0';

  return reinterpret_cast<char *>(passwd);
}

/**
 * Encrypt CHALLENGESIZE bytes in memory using a password.
 */
void vncEncryptBytes(uint8_t *bytes, char *passwd)
{
  uint8_t key[8];

  // key is simply password padded with nulls
  for (uint32_t i=0; i < 8; i++) {
    key[i] = (i < strlen(passwd)) ? passwd[i] : '\0';
  }
  deskey(key, EN0);

  for (int i=0; i < CHALLENGESIZE; i += 8) {
    des(bytes+i, bytes+i);
  }
}
