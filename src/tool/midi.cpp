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
// midi.cpp
//
// Plays MIDI files
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "midi.hpp"
#include "tool.hpp"
#include "url.hpp"	// abs
#include "cache.hpp"	// download


#if MACOSX
int Midi::toolid = QUICKTIME_TOOL;
#else
int Midi::toolid = TIMIDITY_TOOL;
#endif

// local
static pid_t pid = -1;


void Midi::init(int _toolid)
{
  toolid = _toolid;
}

void Midi::launch(const char *tool, const char *url)
{
  switch (pid = fork()) {
  case -1:
    error("%s %s", e_fork, tool);
    break;
  case 0:
    switch (toolid) {
    case TIMIDITY_TOOL:  execlp(tool, tool, "-idqq", "-j", url, NULL); break;
#if MACOSX
    case QUICKTIME_TOOL: execlp(tool, tool, "-a", QUICKTIME_PATH, url, NULL); break;
#endif
    }
    error("%s %s", e_exec, tool);
    signal(SIGCHLD, SIG_IGN);
    exit(1);
  default:
    break;
  }
}

void Midi::start(const char *_url)
{
  char url[URL_LEN];
  Url::abs(_url, url);

  switch (toolid) {
  case TIMIDITY_TOOL:  launch("timidity", url); break;
  case QUICKTIME_TOOL: launch("open", url); break;
  default:             error("%s midi", e_tool);
  }
}

void Midi::stop()
{
  if (pid > 0) kill(pid, SIGKILL);
}

void Midi::pause()
{
  if (pid > 0) kill(pid, SIGSTOP);
}

void Midi::cont()
{
  if (pid > 0) kill(pid, SIGCONT);
}
