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
#include "vreng.hpp"
#include "download.hpp"
#include "cache.hpp"	// download


const OClass Download::oclass(DOWNLOAD_TYPE, "Download", Download::creator);


WObject * Download::creator(char *l)
{
  return new Download(l);
}

void Download::parser(char *l)
{
  l = tokenize(l);
  begin_while_parse(l) {
    l = parseAttributes(l);
    if (!l) break;
    if (!stringcmp(l, "url")) l = parse()->parseUrl(l, names.url);
  }
  end_while_parse(l);
}

Download::Download(char *l)
{
  parser(l);
  enableBehavior(COLLIDE_NEVER);
  initStillObject();
}

void Download::get(Download *wo, void *d, time_t s, time_t u)
{
  char *pfile = strrchr(wo->names.url, '/');
  if (pfile) {
    Cache::download(wo->names.url, ++pfile, "anon");
  }
}

void Download::quit()
{
  savePersistency();
}

void Download::funcs()
{
  setActionFunc(DOWNLOAD_TYPE, 0, _Action get, "Download");
}
