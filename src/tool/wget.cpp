//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	https://github.com/philippedax/vreng
//
// Copyright (C) 1997-2012 Philippe Dax
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
// wget.cpp
//
// launches wget
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "wget.hpp"
#include "url.hpp"	// abs
#include "cache.hpp"	// setCachePath
#include "str.hpp"	// stringcmp

extern int my_wait(pid_t);


int Wget::start(const char *_url, char *file, const char opts[])
{
#if HAVE_WGET
  bool cached = false;
  char url[URL_LEN];
  Url::abs(_url, url);

  if (file && (! strcmp(opts, "-qO"))) {	// use the cache
    cached = true;
    if (Cache::setCachePath(_url, file) == 0) return 0;

    struct stat bufstat;
    if (stat(file, &bufstat) == 0) {
      if (bufstat.st_size != 0) return 1;  // file yet in the cache
      unlink(file);	// remove empty file
    }
  }

  pid_t pid;
  int status;
  bool check = false;
  if (!stringcmp(opts, "check")) check = true;

  switch (pid = fork()) {
  case -1:
    error("%s wget", e_fork);
    return 0;
  case 0:		// child starts transfert
    if (! file) {
      if (check) execlp("wget", "wget", "-q", "--spider", url, NULL);
      else       execlp("wget", "wget", "-q", url, NULL);
    }
    if (*file == '-') {
      execlp("wget", "wget", "-nv", "-O", "-", url, NULL);
    }
    else if (! stringcmp(opts, "inout"))		// local copy
      execlp("wget", "wget", "-i", url, "-O", file, NULL);
    else
      execlp("wget", "wget", "-O", file, url, NULL);
    error("%s wget", e_exec);
    exit(1);
  default:
    if (! cached && ! check)	// not in cache
      return 1;
    status = my_wait(pid);	// parent waits end of transfert
    if (status) {
      if (! check) {
        error("wget status=%d file=%s", status, file);
        unlink(file);	// because wget leaves an empty file
      }
      return 0;
    }
#if HAVE_UTIME
    utime(file, NULL);	// set current time
#endif
    return 1;
  }
#else
  return 0;
#endif
}

#if 0 //notused
void Wget::unlinktmp(const char *file)
{
  if (access(file, W_OK) == 0) unlink(file);
}
#endif //notused
