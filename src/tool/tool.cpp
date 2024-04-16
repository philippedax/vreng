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
// tool.cpp
//
// Manages tools
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "tool.hpp"
#include "audio.hpp"
#include "video.hpp"
#include "wb.hpp"
#include "browser.hpp"
#include "ps.hpp"
#include "pdf.hpp"
#include "mp3.hpp"
#include "wav.hpp"
#include "mpeg.hpp"
#include "midi.hpp"
#include "office.hpp"
#include "modeler.hpp"
#include "xterm.hpp"

void Tool::quitTools()
{
  Audio::quit();
  Video::quit();
  Wb::quit();
  Xterm::quit();
}

void Tool::quitAllTools()
{
  quitTools();

  Browser::quit();
  Ps::quit();
  Pdf::quit();
  Office::quit();
  Modeler::quit();
  Mp3::stop();
  Wav::stop();
  Mpeg::stop();
  Midi::stop();
}
